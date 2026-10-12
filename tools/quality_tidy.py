"""Prepare real compiler inputs for read-only, scoped clang-tidy checks."""

import gzip
import hashlib
import json
from pathlib import Path
import re
import runpy
import shlex
import subprocess
import sys


class TidySetupError(Exception):
    pass


def command(arguments, directory):
    try:
        result = subprocess.run(arguments, cwd=directory, capture_output=True, text=True)
    except OSError as error:
        raise TidySetupError(f"cannot run {arguments[0]}: {error}") from error
    if result.returncode:
        raise TidySetupError(result.stderr.strip() or result.stdout.strip()
                             or f"{arguments[0]} failed")
    return result.stdout


def make_settings(snapshot):
    output = command(["make", "-s", "COMPILER=clang", "CROSS=mips-linux-gnu-",
                      "quality-flags"], snapshot)
    values = {}
    for line in output.splitlines():
        if line.startswith("QUALITY_"):
            key, value = line.split("=", 1)
            values[key] = value
    try:
        flags = shlex.split(values["QUALITY_CFLAGS"])
        sources = values["QUALITY_C_FILES"].split()
        build = values["QUALITY_BUILD_DIR"]
    except (KeyError, ValueError) as error:
        raise TidySetupError("cannot read compiler settings from make quality-flags") from error
    if not flags or Path(build).is_absolute() or ".." in Path(build).parts:
        raise TidySetupError("invalid compiler settings from make quality-flags")
    return flags, sources, build


def generated_headers(snapshot, temporary, flags, compiler):
    """Encode headers from the selected source; never reuse working-tree build output."""
    include = temporary / "include"
    include.mkdir(exist_ok=True)
    templates = ["text_strings", "text_menu_strings"]
    if (snapshot / "include/text_strings.h.in").is_file():
        converter = temporary / "textconv"
        command(["cc", "-I", str(snapshot / "tools"),
                 *[str(snapshot / "tools" / name)
                   for name in ("textconv.c", "utf8.c", "hashtable.c")],
                 "-o", str(converter)], temporary)
        for name in templates:
            charmap = "charmap_menu.txt" if name == "text_menu_strings" else "charmap.txt"
            command([str(converter), str(snapshot / charmap),
                     str(snapshot / "include" / (name + ".h.in")),
                     str(include / (name + ".h"))], snapshot)
    for region in ("us", "jp", "eu", "sh", "de", "fr"):
        template = snapshot / "text/define_text.inc.c"
        directory = snapshot / "text" / region
        if not template.is_file() or not (directory / "dialogs.h").is_file():
            continue
        output = command([compiler, *flags, "-I", str(directory),
                          "-E", "-P", str(template)], snapshot)
        preprocessed = temporary / ("dialogs_" + region + ".c")
        preprocessed.write_text(output)
        destination = include / "text" / region / "define_text.inc.c"
        destination.parent.mkdir(parents=True, exist_ok=True)
        command([str(converter), str(snapshot / "charmap.txt"),
                 str(preprocessed), str(destination)], snapshot)
    template = snapshot / "levels/level_headers.h.in"
    if template.is_file():
        output = command([compiler, *flags, "-E", "-P", "-x", "c", str(template)], snapshot)
        lines = [f'#include "{line.strip()}"' for line in output.splitlines() if line.strip()]
        (include / "level_headers.h").write_text("\n".join(lines) + "\n")
    # Support both "text_strings.h" and the production "include/text_strings.h" spelling.
    return ["-I", str(include), "-I", str(temporary)]


def generated_textures(repository, snapshot, temporary, flags, compiler, changes, sources=(), frontend=False):
    """Encode selected PNGs; recover untracked vanilla data from a verified baserom."""
    include = temporary / "include"
    converter = temporary / "n64graphics"
    decoder = temporary / "mio0"
    assets = None
    images = {}
    roms = {}
    version = None
    pattern = r'^\s*#\s*include\s*"((?:textures|actors|levels)/[^"\n]+\.(?:rgba16|rgba32|ia1|ia4|ia8|ia16|i4|i8|ci4|ci8)(?:\.preswap)?\.(?:inc\.c|pal))"'

    def graphics_converter():
        if not converter.exists():
            command(["cc", "-I", str(snapshot / "tools"), "-DN64GRAPHICS_STANDALONE",
                     str(snapshot / "tools/n64graphics.c"), str(snapshot / "tools/utils.c"),
                     "-lm", "-o", str(converter)], temporary)

    def encode_splash(png, stem):
        # Match the paired-output Makefile rule, including its original RGBA16 source.
        graphics_converter()
        command([str(converter), "-s", "u8", "-i", str(include / (stem + ".ci8.inc.c")),
                 "-g", str(png), "-p", str(include / (stem + ".ci8.pal")),
                 "-f", "ci8", "-c", "rgba16", "-w", "32", "-h", "64"], snapshot)
    directories = include_dirs({"directory": str(snapshot), "arguments": [compiler, *flags]})
    cache = {}
    candidates = set()
    roots = set()
    for change in changes:
        source = snapshot / change.path
        if source.suffix != ".c" or change.path.startswith("tests/"):
            continue
        roots.add(source)
        candidates.update(includes(source, directories, cache))
        if change.path.endswith(".inc.c"):
            # Included data needs the textures of its containing translation unit.
            for name in sources:
                dependencies = includes(snapshot / name, directories, cache)
                if source in dependencies:
                    roots.add(snapshot / name)
                    candidates.update(dependencies)
    # Instrument nested includes too, so caller-defined palette/region macros
    # select textures in their actual inclusion context rather than in isolation.
    probes = temporary / "texture_probes"
    textured = set()
    for source in candidates:
        if not source.is_file() or not source.is_relative_to(snapshot):
            continue
        text = source.read_text()
        if re.search(pattern, text, re.M):
            textured.add(source)
        destination = probes / source.relative_to(snapshot)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(re.sub(pattern, lambda match: 'QUALITY_TEXTURE("' + match[1] + '")',
                                      text, flags=re.M))
    # Probe owning sources rather than their includes in isolation: a palette
    # header's undefined default must not activate an unused texture variant.
    nested = set()
    for source in roots:
        nested.update(includes(source, directories, cache) - {source})
    ordered = sorted(roots - nested)
    for source in ordered:
        if not source.is_file() or not source.is_relative_to(snapshot):
            continue
        if not includes(source, directories, cache).intersection(textured):
            continue
        # Retain conditional preprocessing so inactive region/assets need no inputs.
        probe = probes / source.relative_to(snapshot)
        active = command([compiler, "-I", str(probes), *flags, "-iquote", str(source.parent),
                          "-E", "-P", str(probe)], snapshot)
        names = sorted(set(re.findall(r'QUALITY_TEXTURE\("([^"\n]+)"\)', active)))
        for name in names:
            destination = include / name
            if destination.exists():
                continue
            destination.parent.mkdir(parents=True, exist_ok=True)
            splash = re.fullmatch(r"(actors/water_splash/water_splash_[0-7])\.ci8\.(?:inc\.c|pal)", name)
            png_name = splash[1] + ".rgba16.png" if splash else name[:-6] + ".png"
            png = snapshot / png_name
            if png.is_file():
                if splash:
                    encode_splash(png, splash[1])
                    continue
                graphics_converter()
                parts = png_name.split(".")
                preswap = parts[-2] == "preswap"
                fmt = parts[-3] if preswap else parts[-2]
                command([str(converter), "-s", "u8", "-i", str(destination),
                         "-g", str(png), "-f", fmt, *(["-S"] if preswap else [])], snapshot)
                continue
            if assets is None:
                assets = json.loads((snapshot / "assets.json").read_text())
                versions = [item.removeprefix("-DVERSION_").split("=", 1)[0].lower()
                            for item in flags if item.startswith("-DVERSION_")]
                if len(versions) != 1:
                    raise TidySetupError("cannot determine the baserom region for texture inputs")
                version = versions[0]
            metadata = assets.get(png_name)
            if not metadata:
                raise TidySetupError(f"missing selected texture source {png_name!r}")
            if frontend:
                # Correct storage shape only. Never encode or validate these as game assets.
                size = metadata[-2]
                if splash:
                    size = 512 if name.endswith(".pal") else metadata[0] * metadata[1]
                destination.write_text(", ".join("0" for _ in range(size)) + ",\n")
                continue
            # COMPLETE_EN_US_SEGMENT2 also uses glyphs supplied by JP/SH ROMs.
            regions = sorted(metadata[-1], key=lambda region: region != version)
            for region in regions:
                baserom = repository / ("baserom." + region + ".z64")
                if baserom.is_file():
                    break
            else:
                raise TidySetupError(f"missing vanilla texture {png_name!r}; a baserom for {regions} is required")
            if region not in roms:
                roms[region] = baserom.read_bytes()
                hashes = runpy.run_path(str(snapshot / "tools/detect_baseroms.py"))["sha1_LUT"]
                if hashlib.sha1(roms[region]).hexdigest() != hashes.get(region):
                    raise TidySetupError("texture inputs require an unmodified, matching-region baserom")
            rom = roms[region]
            size, positions = metadata[-2:]
            position = positions[region]
            if len(position) == 1:
                image = rom
            else:
                offset = position[0]
                key = (region, offset)
                if key not in images:
                    if not decoder.exists():
                        command(["cc", "-I", str(snapshot / "tools"), "-DMIO0_STANDALONE",
                                 str(snapshot / "tools/libmio0.c"), "-o", str(decoder)], temporary)
                    result = subprocess.run([str(decoder), "-d", "-o", str(offset),
                                             str(baserom), "-"], capture_output=True)
                    if result.returncode:
                        raise TidySetupError(result.stderr.decode(errors="replace"))
                    images[key] = result.stdout
                image = images[key]
            data = image[position[-1]:position[-1] + size]
            if len(data) != size:
                raise TidySetupError(f"truncated vanilla texture {png_name!r}")
            if splash:
                graphics_converter()
                raw = temporary / "splash.rgba16"
                raw.write_bytes(data)
                png = temporary / "splash.rgba16.png"
                command([str(converter), "-e", str(raw), "-g", str(png), "-f", "rgba16",
                         "-w", str(metadata[0]), "-h", str(metadata[1])], snapshot)
                encode_splash(png, splash[1])
                continue
            destination.write_text(", ".join(f"0x{byte:02X}" for byte in data) + ",\n")


def generated_frontend_arrays(snapshot, temporary):
    """Provide initializer grammar without ROM values for frontend analysis only."""
    manifest = snapshot / "assets/rom/arrays.json"
    compressed = manifest.with_suffix(".json.gz")
    if compressed.is_file():
        recipes = json.loads(gzip.decompress(compressed.read_bytes()))
    elif manifest.is_file():
        recipes = json.loads(manifest.read_text())
    else:
        return
    output = temporary / "include/assets/rom/generated"
    output.mkdir(parents=True, exist_ok=True)
    for identity, recipe in recipes.items():
        if not re.fullmatch(r"[0-9a-f]{20}", identity):
            raise TidySetupError("invalid generated initializer identity")
        text = re.sub(r"@[-+]?(?:0[xX][0-9]+[xX]|d)@", "0", recipe["template"])
        if "@" in text:
            raise TidySetupError("unsupported initializer template placeholder")
        (output / (identity + ".inc.c")).write_text(text)


def generated_verified_arrays(repository, snapshot, temporary):
    """Reconstruct pinned data privately; never reuse working-tree generated values."""
    manifest = snapshot / "assets/rom/arrays.json.gz"
    if not manifest.is_file():
        return
    root = temporary / "include"
    metadata = root / "assets/rom/arrays.json.gz"
    metadata.parent.mkdir(parents=True, exist_ok=True)
    metadata.write_bytes(manifest.read_bytes())
    tools = root / "tools"
    tools.mkdir()
    command(["cc", "-I", str(snapshot / "tools"), "-DMIO0_STANDALONE",
             str(snapshot / "tools/libmio0.c"), "-o", str(tools / "mio0")], temporary)
    program = """
import sys
from pathlib import Path
sys.path.insert(0, sys.argv[1])
from reconstruct_rom_arrays import reconstruct
reconstruct(Path(sys.argv[2]))
"""
    command([sys.executable, "-c", program, str(snapshot / "tools"), str(root)], repository)


def fixture_commands(snapshot, temporary, changes, frontend=False):
    """Use the same extracted inputs and compiler options as the existing host runner."""
    paths = [change.path for change in changes if change.path.startswith("tests/")]
    if not paths:
        return []
    program = """
import json, os, sys
from pathlib import Path
root, temporary = map(Path, sys.argv[1:3])
paths = json.loads(sys.argv[3])
os.environ.pop('SM64_FRONTEND_ASSET_ROOT', None)
os.environ.pop('SM64_VERIFIED_ASSET_ROOT', None)
if sys.argv[4]:
    variable = 'SM64_FRONTEND_ASSET_ROOT' if sys.argv[5] == '1' else 'SM64_VERIFIED_ASSET_ROOT'
    os.environ[variable] = sys.argv[4]
sys.path.insert(0, str(root / 'tests'))
import run_regressions as runner
entries = []
for suite, filename in runner.SUITES.items():
    if 'tests/' + filename not in paths and 'tests/test_support.h' not in paths:
        continue
    directory = temporary / suite
    directory.mkdir()
    runner.prepare(root, suite, directory, {})
    source = root / 'tests' / filename
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                                  '-Wno-unused-parameter', '-I', str(directory),
                                  '-I', str(root / 'tests'), '-c', str(source)]})
if 'tests/particle_budget_test.c' in paths:
    import run_particle_budget_tests as particles
    directory = temporary / 'particles'
    directory.mkdir()
    particles.prepare(directory, {})
    source = root / 'tests/particle_budget_test.c'
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=c99', '-I', str(directory),
                                  '-I', str(root / 'tests'), '-I', str(root / 'include'),
                                  '-c', str(source)]})
if 'tests/physics_lab_render_test.c' in paths:
    from test_physics_lab_rendering import prepare_render_fixture
    directory = temporary / 'lab-render'
    directory.mkdir()
    prepare_render_fixture(directory)
    source = root / 'tests/physics_lab_render_test.c'
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                                  '-I', str(directory), '-c', str(source)]})
if 'tests/physics_lab_behaviors_test.c' in paths:
    from test_physics_lab_behaviors import prepare_behavior_fixture
    directory = temporary / 'lab-behaviors'
    directory.mkdir()
    prepare_behavior_fixture(directory)
    source = root / 'tests/physics_lab_behaviors_test.c'
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                                  '-Wno-unused-parameter', '-I', str(directory),
                                  '-c', str(source)]})
if 'tests/pause_screen_blend_test.c' in paths:
    from test_pause_screen_blend import prepare_pause_blend_fixture
    directory = temporary / 'pause-blend'
    directory.mkdir()
    prepare_pause_blend_fixture(directory)
    source = root / 'tests/pause_screen_blend_test.c'
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=gnu99', '-D_LANGUAGE_C', '-Wall', '-Wextra',
                                  '-Werror', '-Wno-pointer-to-int-cast',
                                  '-I', str(root / 'include'), '-I', str(root / 'include/n64'),
                                  '-I', str(directory), '-c', str(source)]})
if 'tests/pause_easter_egg_test.c' in paths:
    from test_pause_easter_egg import prepare_pause_easter_egg_fixture
    directory = temporary / 'pause-easter-egg'
    directory.mkdir()
    prepare_pause_easter_egg_fixture(directory)
    source = root / 'tests/pause_easter_egg_test.c'
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=gnu99', '-D_LANGUAGE_C', '-DVERSION_US',
                                  '-Wall', '-Wextra', '-Werror', '-Wno-pointer-to-int-cast',
                                  '-I', str(root / 'include'), '-I', str(root / 'include/n64'),
                                  '-I', str(directory), '-c', str(source)]})
for filename, module, helper in (
        ('crash_screen_test.c', 'test_crash_screen', 'prepare_crash_fixture'),
        ('crash_stack_test.c', 'test_crash_screen', 'prepare_stack_fixture'),
        ('simpleflips_confinement_test.c', 'test_simpleflips_confinement', 'prepare_confinement_fixture'),
        ('roof_poison_test.c', 'test_roof_poison', 'prepare_roof_poison_fixture'),
        ('physics_lab_boulder_test.c', 'test_physics_lab_behaviors', 'prepare_boulder_fixture'),
        ('physics_lab_native_paths_test.c', 'test_physics_lab_behaviors', 'prepare_native_paths_fixture'),
        ('compact_explosion_bubble_test.c', 'run_compact_particle_tests', 'prepare_fixture'),
        ('simpleflips_head_test.c', 'test_jrb_simpleflips', 'prepare_head_fixture'),
        ('simpleflips_mips_dialog_test.c', 'test_jrb_simpleflips', 'prepare_npc_dialog_fixture'),
        ('simpleflips_one_up_test.c', 'test_jrb_simpleflips', 'prepare_one_up_fixture')):
    if 'tests/' + filename not in paths:
        continue
    directory = temporary / filename.removesuffix('.c')
    directory.mkdir()
    getattr(__import__(module), helper)(directory)
    source = root / 'tests' / filename
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc',
                                  '-std=c11' if filename == 'compact_explosion_bubble_test.c' else '-std=c99',
                                  '-I', str(directory), '-I', str(root), '-I', str(root / 'include'),
                                  '-c', str(source)]})
for filename, module in (
        ('player_stats_test.c', 'test_player_stats'),
        ('player_stats_menu_test.c', 'test_player_stats_menu')):
    if 'tests/' + filename not in paths:
        continue
    directory = temporary / filename.removesuffix('.c')
    directory.mkdir()
    __import__(module).prepare_fixture(directory)
    source = root / 'tests' / filename
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                                  '-I', str(directory), '-I', str(root), '-c', str(source)]})
for filename, module, helper in (
        ('collision_mesh_test.c', 'test_collision_mesh', 'prepare_fixture'),
        ('persistent_surfaces_test.c', 'test_persistent_surfaces', 'prepare_fixture'),
        ('physics_lab_activity_test.c', 'test_physics_lab_activity', 'prepare_activity_fixture')):
    if 'tests/' + filename not in paths:
        continue
    directory = temporary / filename.removesuffix('.c')
    directory.mkdir()
    prepare = getattr(__import__(module), helper)
    prepare(directory)
    source = root / 'tests' / filename
    entries.append({'directory': str(root), 'file': str(source),
                    'arguments': ['cc', '-std=gnu17', '-fwrapv', '-I', str(directory),
                                  '-I', str(root / 'src/engine'), '-I', str(root / 'include'),
                                  '-c', str(source)]})
print(json.dumps(entries))
"""
    output = command([sys.executable, "-c", program, str(snapshot), str(temporary),
                      json.dumps(paths), str((temporary / "include").resolve()), "1" if frontend else "0"], snapshot)
    return json.loads(output)


def include_dirs(entry):
    arguments = entry["arguments"]
    result = []
    index = 1
    while index < len(arguments):
        value = arguments[index]
        if value in ("-I", "-isystem", "-iquote"):
            index += 1
            if index == len(arguments):
                raise TidySetupError("an include option is missing its directory")
            value = arguments[index]
        elif value.startswith("-I"):
            value = value[2:]
        else:
            index += 1
            continue
        result.append((Path(entry["directory"]) / value).resolve())
        index += 1
    return result


def includes(source, directories, cache):
    """Find candidate owners; the compiler later verifies active header inclusion."""
    visited = set()
    pending = [source]
    while pending:
        path = pending.pop()
        if path in visited:
            continue
        visited.add(path)
        key = (path, tuple(directories))
        if key not in cache:
            children = []
            if path.is_file():
                text = path.read_text(errors="replace")
                for match in re.finditer(r'^\s*#\s*include\s*["<]([^">]+)[">]', text, re.M):
                    for directory in [path.parent, *directories]:
                        child = (directory / match[1]).resolve()
                        if child.is_file():
                            children.append(child)
                            break
            cache[key] = children
        pending.extend(cache[key])
    return visited


def active_header(entry, header, compiler, temporary):
    dependencies = temporary / "dependencies.d"
    arguments = entry["arguments"][1:]
    command([compiler, *arguments, "-M", "-MF", str(dependencies)], entry["directory"])
    text = dependencies.read_text().replace("\\\n", " ")
    paths = shlex.split(text.split(":", 1)[1])
    return header in {(Path(entry["directory"]) / path).resolve() for path in paths}


def remap_database(repository, snapshot, filename):
    try:
        entries = json.loads(Path(filename).read_text())
        if not isinstance(entries, list):
            raise ValueError("expected a list")
        remapped = []
        for entry in entries:
            directory = Path(entry["directory"]).resolve()
            source = (directory / entry["file"]).resolve()
            relative = source.relative_to(repository)
            destination = snapshot / relative
            arguments = entry.get("arguments") or shlex.split(entry["command"])
            if not isinstance(arguments, list) or not arguments or not all(isinstance(arg, str)
                                                                          for arg in arguments):
                raise ValueError("expected compiler arguments")
            result = []
            for argument in arguments:
                joined = re.match(r"^(-isystem|-iquote|-include|-isysroot|--sysroot=|-I)(.+)$", argument)
                prefix = joined[1] if joined else ""
                path = argument[len(prefix):]
                if path == str(repository) or path.startswith(str(repository) + "/"):
                    argument = prefix + str(snapshot / Path(path).relative_to(repository))
                result.append(argument)
            remapped.append({"directory": str(snapshot / directory.relative_to(repository)),
                             "file": str(destination), "arguments": result})
        return remapped
    except (OSError, KeyError, TypeError, ValueError) as error:
        raise TidySetupError("invalid compilation database or paths outside the project") from error


def prepare_database(repository, snapshot, changes, temporary, compiler, database, frontend=False):
    if database != "auto":
        entries = remap_database(repository, snapshot, Path(database) / "compile_commands.json")
    else:
        if frontend:
            generated_frontend_arrays(snapshot, temporary)
        elif any(not change.path.startswith("tools/") for change in changes):
            generated_verified_arrays(repository, snapshot, temporary)
        fixtures = fixture_commands(snapshot, temporary, changes, frontend=frontend)
        fixture_paths = {Path(entry["file"]) for entry in fixtures}
        native_changes = [change for change in changes if snapshot / change.path not in fixture_paths
                          and not change.path.startswith(("tests/", "tools/"))]
        entries = list(fixtures)
        if native_changes:
            flags, sources, _ = make_settings(snapshot)
            flags = generated_headers(snapshot, temporary, flags, compiler) + flags
            generated_textures(repository, snapshot, temporary, flags, compiler, native_changes, sources, frontend=frontend)
            sources += [change.path for change in native_changes
                        if Path(change.path).suffix in (".c", ".cpp")
                        and not change.path.endswith(".inc.c")]
            for name in sorted(set(sources)):
                source = snapshot / name
                if source.is_file():
                    options = flags + (["-std=c++17"] if source.suffix == ".cpp" else [])
                    entries.append({"directory": str(snapshot), "file": str(source),
                                    "arguments": [compiler, *options, "-c", str(source)]})
        for change in changes:
            source = snapshot / change.path
            if (change.path.startswith(("tests/", "tools/")) and source.suffix == ".c"
                    and source not in fixture_paths):
                standard = "-std=gnu17" if change.path.startswith("tools/") else "-std=c99"
                entries.append({"directory": str(snapshot), "file": str(source),
                                "arguments": [compiler, standard, "-I", str(source.parent),
                                              "-c", str(source)]})
    by_source = {Path(entry["file"]): entry for entry in entries}
    owners = {}
    cache = {}
    for change in changes:
        source = snapshot / change.path
        if source in by_source:
            owners[source] = by_source[source]
            continue
        candidates = sorted(entries, key=lambda entry: (not str(Path(entry["file"]).relative_to(snapshot))
                                                       .startswith("src/"), entry["file"]))
        failure = None
        for entry in candidates:
            if source in includes(Path(entry["file"]), include_dirs(entry), cache):
                try:
                    active = active_header(entry, source, compiler, temporary)
                except TidySetupError as error:
                    failure = error
                    continue
                if active:
                    owners[Path(entry["file"])] = entry
                    break
        else:
            detail = f": {failure}" if failure else ""
            raise TidySetupError(f"no active, parseable translation unit includes {change.path!r}{detail}")
    output = temporary / "compile_commands.json"
    output.write_text(json.dumps(list(owners.values())))
    return list(owners)
