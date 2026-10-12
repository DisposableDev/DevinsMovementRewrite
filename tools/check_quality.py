#!/usr/bin/env python3
"""Check the selected change without rewriting source, staging files, or building a ROM."""

import argparse
from contextlib import nullcontext
from dataclasses import dataclass
import difflib
import fnmatch
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import tokenize
import xml.etree.ElementTree as ET

from quality_tidy import TidySetupError, prepare_database
from quality_snapshot import SnapshotError, blobs, index_entries, link_destination, materialize


C_SUFFIXES = {".c", ".h", ".cpp", ".hpp"}
C_QUOTES = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)


class SetupError(Exception):
    """A required check could not run; this is not a quality pass or gameplay failure."""


@dataclass
class Change:
    path: str
    old_path: str
    status: str
    content: bytes = b""
    lines: list = None
    symlink: bool = False


def run(command, cwd, data=None):
    try:
        return subprocess.run(command, cwd=cwd, input=data, capture_output=True, check=False)
    except OSError as error:
        raise SetupError(f"cannot run {command[0]}: {error}") from error


def git(root, *arguments):
    result = run(["git", *arguments], root)
    if result.returncode:
        raise SetupError(result.stderr.decode(errors="replace").strip() or "Git command failed")
    return result.stdout


def revision(root, name):
    # An explicit empty tree checks every source file on a first/root push.
    kind = "tree" if name == "EMPTY_TREE" else "commit"
    if name == "EMPTY_TREE":
        name = git(root, "hash-object", "-t", "tree", "-w", "--stdin").decode().strip()
    return git(root, "rev-parse", "--verify", "--end-of-options", name + "^{" + kind + "}").decode().strip()


def changed_files(root, base, staged, paths=None):
    arguments = ["diff", "--name-status", "-z", "--find-renames", "--no-ext-diff"]
    if staged:
        arguments.append("--cached")
    pathspecs = [":(literal)" + path for path in paths or []]
    fields = git(root, *arguments, base, "--", *pathspecs).split(b"\0")
    changes = {}
    index = 0
    while index < len(fields) - 1:
        status = fields[index].decode()
        old_path = os.fsdecode(fields[index + 1])
        index += 2
        path = old_path
        if status.startswith(("R", "C")):
            path = os.fsdecode(fields[index])
            index += 1
        if status.startswith("U"):
            raise SetupError(f"resolve the conflict in {path!r} first")
        changes[path] = Change(path, old_path, status)
    if not staged:
        for path in git(root, "ls-files", "--others", "--exclude-standard", "-z",
                        "--", *pathspecs).split(b"\0"):
            if path:
                name = os.fsdecode(path)
                changes[name] = Change(name, name, "A")
    return changes


def changed_lines(before, after):
    result = []
    matcher = difflib.SequenceMatcher(None, before.splitlines(keepends=True),
                                     after.splitlines(keepends=True), autojunk=False)
    for operation, _, _, start, end in matcher.get_opcodes():
        if operation in ("insert", "replace"):
            result.append((start + 1, end))
    return result


def formatted_lines(before, after, ranges):
    """Keep the selected code covered when formatting inserts or removes lines."""
    result = []
    matcher = difflib.SequenceMatcher(None, before.splitlines(keepends=True),
                                     after.splitlines(keepends=True), autojunk=False)
    for operation, begin, end, new_begin, new_end in matcher.get_opcodes():
        if operation == "equal":
            for start, finish in ranges:
                first, last = max(begin, start - 1), min(end, finish)
                if first < last:
                    result.append((new_begin + first - begin + 1, new_begin + last - begin))
        elif new_begin < new_end:
            overlaps_selected = any(max(begin, start - 1) < min(end, finish)
                                    for start, finish in ranges)
            inserts_selected = (operation == "insert"
                                and (selected(begin, ranges) or selected(begin + 1, ranges)))
            if overlaps_selected or inserts_selected:
                result.append((new_begin + 1, new_end))
    return sorted(set(result))


def safe_file(root, name):
    path = root / name
    if Path(name).is_absolute() or ".." in Path(name).parts or path.is_symlink():
        raise SetupError(f"expected a regular repository file: {name!r}")
    if not path.resolve().is_relative_to(root.resolve()):
        raise SetupError(f"file leaves the repository: {name!r}")
    return path


def load_json(path):
    try:
        return json.loads(path.read_text())
    except (OSError, ValueError) as error:
        raise SetupError(f"cannot read {path}: {error}") from error


def load_config(root, content=None):
    config = (load_json(safe_file(root, ".quality.json")) if content is None
              else json.loads(content))
    try:
        if not isinstance(config, dict) or not re.fullmatch(r"\d+\.\d+\.\d+", config["clang_version"]):
            raise ValueError("expected a pinned clang version")
        if not isinstance(config["exclusions"], list) or not isinstance(config["validation"], list):
            raise ValueError("expected exclusion and validation lists")
        for item in config["exclusions"]:
            if (not isinstance(item["pattern"], str) or not isinstance(item["reason"], str)
                    or not item["reason"]):
                raise ValueError("expected an exclusion pattern and reason")
        for item in config["validation"]:
            if not isinstance(item["name"], str) or not isinstance(item["paths"], list):
                raise ValueError("expected a validation name and path list")
            if (not all(isinstance(p, str) for p in item["paths"])
                    or not isinstance(item["commands"], list)):
                raise ValueError("expected validation patterns and commands")
            for command in item["commands"]:
                if (not isinstance(command, list) or not command
                        or not all(isinstance(p, str) for p in command)):
                    raise ValueError("expected a nonempty argument list")
                if Path(command[0]).is_absolute() or ".." in Path(command[0]).parts:
                    raise ValueError("validation script must be inside the snapshot")
    except (KeyError, TypeError, ValueError) as error:
        raise SetupError("invalid .quality.json") from error
    return config


def prepare_changes(repository, snapshot, changes, base):
    for change in changes.values():
        if change.status.startswith("D"):
            continue
        path = snapshot / change.path
        if path.is_symlink():
            change.content = os.fsencode(os.readlink(path))
            target = safe_file(snapshot, link_destination(change.path, change.content))
            if not target.is_file():
                raise SetupError(f"expected a regular repository link target: {change.path!r}")
            change.symlink = True
            change.lines = []
            continue
        path = safe_file(snapshot, change.path)
        try:
            change.content = path.read_bytes()
        except OSError as error:
            raise SetupError(f"cannot read {change.path!r}: {error}") from error
        before = b""
        if not change.status.startswith("A"):
            before = git(repository, "show", base + ":" + change.old_path)
        change.lines = changed_lines(before, change.content)


def read_scope(filename, base):
    scope = load_json(Path(filename))
    if not isinstance(scope, dict) or scope.get("base") != base:
        raise SetupError("scope baseline is stale; refresh it against the selected revision")
    files = scope.get("files")
    if not isinstance(files, dict) or not files:
        raise SetupError("scope must contain a nonempty files object")
    return files


def apply_scope(snapshot, changes, base, filename, task_only=False):
    files = read_scope(filename, base)
    for name, item in files.items():
        path = safe_file(snapshot, name)
        try:
            content = path.read_bytes()
            ranges = item["lines"]
            count = len(content.splitlines())
            valid = ranges and all(len(pair) == 2 and all(type(n) is int for n in pair)
                                   and 1 <= pair[0] <= pair[1] <= count for pair in ranges)
            if not valid or not isinstance(item["reason"], str) or not item["reason"].strip():
                raise ValueError("expected reviewed line ranges and a reason")
            if hashlib.sha256(content).hexdigest() != item["sha256"]:
                raise ValueError("content hash differs")
        except (OSError, KeyError, TypeError, ValueError) as error:
            raise SetupError(f"stale or invalid scope for {name!r}") from error
        if name not in changes:
            changes[name] = Change(name, name, "M", content, [])
        if changes[name].status.startswith("D"):
            raise SetupError(f"scope selects a deleted file: {name!r}")
        if changes[name].content != content:
            raise SetupError(f"content changed while selecting scope for {name!r}")
        selected_ranges = [tuple(pair) for pair in ranges]
        if not task_only:
            selected_ranges += changes[name].lines
        changes[name].lines = sorted(set(selected_ranges))


def exclusion(config, path):
    for item in config["exclusions"]:
        if fnmatch.fnmatchcase(path, item["pattern"]):
            return item["reason"]
    return None


def selected(number, ranges):
    return any(start <= number <= end for start, end in ranges)


def whitespace(change):
    failures = []
    for number, line in enumerate(change.content.splitlines(keepends=True), 1):
        if not selected(number, change.lines):
            continue
        body = line.rstrip(b"\r\n")
        if body.rstrip(b" \t") != body:
            failures.append(f"{change.path!r}:{number}: trailing whitespace")
        if line.endswith(b"\r\n"):
            failures.append(f"{change.path!r}:{number}: use LF line endings")
    if change.lines and change.content and not change.content.endswith(b"\n"):
        if selected(len(change.content.splitlines()), change.lines):
            failures.append(f"{change.path!r}: missing final newline")
    return failures


def readability(change):
    text = change.content.decode("utf-8")
    notes = []
    if Path(change.path).suffix == ".py":
        try:
            compile(text, change.path, "exec", dont_inherit=True)
            tokens = tokenize.generate_tokens(io.StringIO(text).readline)
            semicolons = [token.start[0] for token in tokens
                          if token.type == tokenize.OP and token.string == ";"]
        except (SyntaxError, tokenize.TokenError, IndentationError) as error:
            raise SetupError(f"cannot parse {change.path!r}: {error}") from error
        for line in sorted(set(semicolons)):
            if selected(line, change.lines):
                notes.append(f"{change.path!r}:{line}: review packed Python statements")
    elif Path(change.path).suffix in C_SUFFIXES:
        # Technical review notes are shown only in verbose mode.
        masked = C_QUOTES.sub(lambda match: re.sub(r"[^\n]", " ", match.group()), text)
        for number, line in enumerate(masked.splitlines(), 1):
            if not selected(number, change.lines) or line.lstrip().startswith("#"):
                continue
            if line.count("?") > 1:
                notes.append(f"{change.path!r}:{number}: review nested choices")
            if line.count(";") > 1 and not re.search(r"\bfor\s*\(", line):
                notes.append(f"{change.path!r}:{number}: review packed C statements")
    return notes


def clang_tool(repository, name, version):
    major = version.split(".")[0]
    configured = os.environ.get(name.upper().replace("-", "_"))
    local = repository / ".quality-tools" / ("llvm-" + major) / "usr/bin" / (name + "-" + major)
    executable = configured or shutil.which(name + "-" + major) or shutil.which(name)
    if not executable and local.is_file():
        executable = str(local)
    if not executable:
        raise SetupError(f"{name} {version} unavailable; install it or set "
                         + name.upper().replace("-", "_"))
    result = run([executable, "--version"], repository)
    output = (result.stdout + result.stderr).decode(errors="replace")
    match = re.search(r"(?:version\s+)(\d+\.\d+\.\d+)", output)
    if result.returncode or not match or match[1] != version:
        raise SetupError(f"expected {name} {version}; received {output.strip()!r}")
    return executable


def format_changes(snapshot, change, executable):
    style = safe_file(snapshot, ".clang-format")
    if not style.is_file():
        raise SetupError(".clang-format is missing from the selected snapshot")
    command = [executable, "--style=file:" + str(style), "--output-replacements-xml",
               "--assume-filename=" + str(snapshot / change.path)]
    command += [f"--lines={start}:{end}" for start, end in change.lines]
    result = run(command, snapshot, change.content)
    if result.returncode:
        raise SetupError(f"formatter failed for {change.path!r}: "
                         + result.stderr.decode(errors="replace").strip())
    try:
        document = ET.fromstring(result.stdout)
        if document.tag != "replacements" or document.get("incomplete_format") != "false":
            raise ValueError("incomplete format")
        replacements = [(int(item.attrib["offset"]), int(item.attrib["length"]),
                         (item.text or "").encode()) for item in document]
    except (ET.ParseError, KeyError, ValueError) as error:
        raise SetupError(f"invalid or incomplete formatting of {change.path!r}") from error
    offsets = [0]
    for line in change.content.splitlines(keepends=True):
        offsets.append(offsets[-1] + len(line))
    allowed = []
    for start, end in sorted(change.lines):
        begin, finish = offsets[start - 1], offsets[end]
        if allowed and begin <= allowed[-1][1]:
            allowed[-1] = (allowed[-1][0], max(finish, allowed[-1][1]))
        else:
            allowed.append((begin, finish))
    previous_end = 0
    for offset, length, replacement in sorted(replacements):
        if offset < previous_end or offset + length > len(change.content) or offset < 0 or length < 0:
            raise SetupError(f"invalid formatter offsets for {change.path!r}")
        previous_end = offset + length
        # Indentation may include the preceding LF only if it remains unchanged.
        if not any((begin <= offset and offset + length <= end)
                   or (offset == begin - 1 and length > 0 and offset + length <= end
                       and change.content[offset:offset + 1] == b"\n"
                       and replacement.startswith(b"\n")) for begin, end in allowed):
            raise SetupError(f"formatter would change surrounding lines in {change.path!r}; "
                             "review them and supply a hashed --scope instead")
    formatted = change.content
    for offset, length, replacement in sorted(replacements, reverse=True):
        formatted = formatted[:offset] + replacement + formatted[offset + length:]
    return formatted


def tidy_changes(repository, snapshot, changes, config, database, verbose, frontend=False):
    compiler = clang_tool(repository, "clang", config["clang_version"])
    executable = clang_tool(repository, "clang-tidy", config["clang_version"])
    configuration = safe_file(snapshot, ".clang-tidy")
    if not configuration.is_file():
        raise SetupError(".clang-tidy is missing from the selected snapshot")
    filters = [{"name": str(snapshot / change.path), "lines": change.lines} for change in changes]
    headers = "^(" + "|".join(re.escape(item["name"]) for item in filters) + ")$"
    try:
        with tempfile.TemporaryDirectory(prefix="sm64-tidy-") as temporary:
            directory = Path(temporary)
            files = prepare_database(repository, snapshot, changes, directory, compiler, database, frontend=frontend)
            result = run([executable, "-p", str(directory), "--quiet",
                          "--config-file=" + str(configuration),
                          "--checks=-*,readability-braces-around-statements",
                          "--warnings-as-errors=readability-braces-around-statements",
                          "--header-filter=" + headers,
                          "--line-filter=" + json.dumps(filters), *map(str, files)], snapshot)
    except (TidySetupError, ValueError) as error:
        raise SetupError(f"parsed code checking could not run: {error}") from error
    output = (result.stdout + result.stderr).decode(errors="replace")
    if verbose and output.strip():
        print(output.strip())
    setup_output = "\n".join(line for line in output.splitlines()
                             if "[readability-braces-around-statements" not in line)
    if ("[clang-diagnostic-error" in setup_output or "Error while processing" in setup_output
            or re.search(r"\berror:|Error parsing", setup_output)):
        errors = "\n".join(line for line in output.splitlines()
                           if ("error:" in line or "Error parsing" in line)
                           and "[readability-braces-around-statements" not in line)
        raise SetupError("parsed code checking could not run:\n" + errors)
    for line in output.splitlines():
        if "[readability-braces-around-statements" in line:
            location = line.split(": warning:", 1)[0].split(": error:", 1)[0]
            location = location.replace(str(snapshot) + os.sep, "")
            print(f"FAIL: {location}: add braces around this statement.")
    if result.returncode and "[readability-braces-around-statements" not in output:
        raise SetupError("parsed code checking failed before completing the braces check:\n" + output.strip())
    if verbose:
        print(f"DETAIL: parsed {len(files)} translation units for {len(changes)} changed files.")
    return int(bool(result.returncode))


def validate(repository, snapshot, config, paths):
    status = 0
    for group in config["validation"]:
        if not any(fnmatch.fnmatchcase(path, pattern) for path in paths for pattern in group["paths"]):
            continue
        print("VALIDATE:", group["name"], flush=True)
        for arguments in group["commands"]:
            script = safe_file(snapshot, arguments[0])
            environment = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
            # Validators create isolated repositories; a commit hook's index is private to Git.
            for variable in ("GIT_INDEX_FILE", "GIT_DIR", "GIT_WORK_TREE", "GIT_COMMON_DIR",
                             "GIT_OBJECT_DIRECTORY", "GIT_ALTERNATE_OBJECT_DIRECTORIES", "GIT_PREFIX"):
                environment.pop(variable, None)
            for tool in ("clang", "clang-format", "clang-tidy"):
                variable = tool.upper().replace("-", "_")
                if variable not in environment:
                    try:
                        environment[variable] = clang_tool(repository, tool, config["clang_version"])
                    except SetupError:
                        pass
            try:
                result = subprocess.run([sys.executable, str(script), *arguments[1:]],
                                        cwd=snapshot, env=environment, check=False)
            except OSError as error:
                raise SetupError(f"cannot run validation: {error}") from error
            if result.returncode not in (0, 1):
                raise SetupError(f"{group['name']} did not complete (exit {result.returncode})")
            status = max(status, result.returncode)
    return status


def write_fixes(repository, changes, fixes, base, index_before):
    if (revision(repository, "HEAD") != base
            or git(repository, "ls-files", "--stage", "-z") != index_before):
        raise SetupError("HEAD or index changed while formatting; rerun the check")
    for change in changes:
        if git(repository, "diff", "--cached", "--name-only", "HEAD", "--", change.path):
            raise SetupError(f"refusing to rewrite staged content in {change.path!r}")
        if safe_file(repository, change.path).read_bytes() != change.content:
            raise SetupError(f"content changed while formatting {change.path!r}")
    for change in changes:
        formatted = fixes.get(change.path)
        if formatted is not None and formatted != change.content:
            path = safe_file(repository, change.path)
            path.write_bytes(formatted)
            print("".join(difflib.unified_diff(change.content.decode().splitlines(keepends=True),
                                             formatted.decode().splitlines(keepends=True),
                                             "a/" + change.path, "b/" + change.path)), end="")
            change.lines = formatted_lines(change.content, formatted, change.lines)
            change.content = formatted


def check(repository, snapshot, changes, config, arguments, base, index_before):
    included = []
    skipped = 0
    notes = []
    fixes = {}
    formatting = []
    for change in changes.values():
        if change.status.startswith("D"):
            continue
        if change.symlink:
            skipped += 1
            continue
        if exclusion(config, change.path):
            skipped += 1
            continue
        if b"\0" in change.content:
            if Path(change.path).suffix in C_SUFFIXES | {".py"}:
                raise SetupError(f"source contains a NUL byte: {change.path!r}")
            skipped += 1
            continue
        try:
            change.content.decode("utf-8")
        except UnicodeDecodeError as error:
            if Path(change.path).suffix in C_SUFFIXES | {".py"}:
                raise SetupError(f"source is not UTF-8: {change.path!r}") from error
            skipped += 1
            continue
        included.append(change)
        notes.extend(readability(change))
        if change.lines and Path(change.path).suffix in C_SUFFIXES:
            formatting.append(change)
    if formatting:
        executable = clang_tool(repository, "clang-format", config["clang_version"])
        for change in formatting:
            fixes[change.path] = format_changes(snapshot, change, executable)
    if arguments.fix:
        write_fixes(repository, formatting, fixes, base, index_before)
    failures = [message for change in included for message in whitespace(change)]
    for change in formatting:
        if fixes[change.path] != change.content:
            failures.append(f"{change.path!r}: formatting differs in selected lines")
    for message in failures:
        print("FAIL:", message)
    if arguments.verbose:
        for message in notes:
            print("REVIEW NOTE:", message)
    status = int(bool(failures))
    if arguments.tidy and formatting and not status:
        status = max(status, tidy_changes(repository, snapshot, formatting, config,
                                          arguments.tidy, arguments.verbose, arguments.frontend_only))
    if arguments.validate:
        status = max(status, validate(repository, snapshot, config, changes))
    if arguments.verbose:
        print(f"DETAIL: {len(included)} text files checked; {len(formatting)} C/C++ files; "
              f"{skipped} excluded/binary files.")
    checked = "formatting and parsed braces checks" if formatting and arguments.tidy else "code checks"
    print(f"{'FAIL' if status else 'PASS'}: {checked} {'failed' if status else 'passed'}.")
    if formatting and not arguments.tidy:
        print("NOTE: automatic braces checking was explicitly disabled.")
    return status


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--staged", action="store_true", help="check an isolated index snapshot")
    selection.add_argument("--base", help="compare the working tree with this commit")
    parser.add_argument("--fix", action="store_true", help="format only approved working-tree ranges")
    scope_selection = parser.add_mutually_exclusive_group()
    scope_selection.add_argument("--scope", help="add reviewed JSON ranges to changed lines")
    scope_selection.add_argument("--task-scope", help="check only the files and exact ranges in this JSON")
    scope_selection.add_argument("--path", action="append", dest="paths", metavar="FILE",
                                 help="check changed lines only in this repository file; repeatable")
    parser.add_argument("--frontend-only", action="store_true",
                        help="use temporary zero asset placeholders for syntax/braces checks; no asset validation")
    parser.add_argument("--validate", action="store_true",
                        help="run applicable existing host/output checks")
    tidy = parser.add_mutually_exclusive_group()
    tidy.add_argument("--tidy", nargs="?", const="auto", default="auto", metavar="BUILD_DIR",
                      help="check braces by default; optionally use an existing compilation database")
    tidy.add_argument("--no-tidy", action="store_const", dest="tidy", const=None,
                      help="explicitly skip parsed braces checking")
    parser.add_argument("--verbose", action="store_true", help="show technical details and review notes")
    arguments = parser.parse_args(argv)
    try:
        repository = Path(git(Path.cwd(), "rev-parse", "--show-toplevel").decode().strip())
        base = revision(repository, arguments.base or "HEAD")
        if arguments.frontend_only and (arguments.validate or arguments.fix):
            raise SetupError("--frontend-only cannot validate assets or rewrite source")
        if arguments.frontend_only:
            print("FRONTEND ONLY: temporary zero assets; this checks syntax/braces, not reconstructed data or gameplay.")
        if arguments.validate and (arguments.task_scope or arguments.paths):
            raise SetupError("task selection cannot use broad --validate groups; "
                             "run the relevant named tests separately")
        if arguments.fix and (arguments.staged or arguments.base):
            raise SetupError("--fix accepts only working changes; "
                             "check staged/base modes separately")
        index_before = git(repository, "ls-files", "--stage", "-z")
        paths = arguments.paths
        if arguments.task_scope:
            paths = list(read_scope(arguments.task_scope, base))
        if paths:
            for name in paths:
                safe_file(repository, name)
                if not name or name == "." or (repository / name).is_dir():
                    raise SetupError(f"select a repository file, not a directory: {name!r}")
        changes = changed_files(repository, base, arguments.staged, paths)
        if not changes and not (arguments.scope or arguments.task_scope):
            print("NOT APPLICABLE: no selected changes; no source checks required.")
            return 0
        context = nullcontext(None)
        if arguments.staged:
            context = tempfile.TemporaryDirectory(prefix="sm64-quality-")
        with context as temporary:
            snapshot = Path(temporary) if temporary else repository
            if temporary:
                if git(repository, "ls-files", "--unmerged", "-z"):
                    raise SetupError("resolve index conflicts before checking a staged snapshot")
                selected_paths = {name for name, change in changes.items()
                                  if not change.status.startswith("D")}
                for scope in (arguments.scope, arguments.task_scope):
                    if scope:
                        selected_paths.update(read_scope(scope, base))
                entries = index_entries(index_before)
                if ".quality.json" not in entries:
                    raise SetupError(".quality.json is missing from the selected snapshot")
                staged_config = load_config(snapshot, blobs(repository, entries, [".quality.json"])[".quality.json"])
                parsed = bool(arguments.tidy) and any(
                    Path(name).suffix in C_SUFFIXES and not exclusion(staged_config, name)
                    for name in selected_paths)
                copied, total = materialize(repository, snapshot, index_before, selected_paths, parsed,
                                            complete=arguments.validate or arguments.tidy not in (None, "auto"))
                if arguments.verbose:
                    print(f"DETAIL: staged snapshot copied {copied} of {total} index files.")
                if ("tools/check_quality.py" in changes
                        and "tools/check_quality.py" in entries
                        and "tools/quality_tidy.py" not in entries):
                    raise SetupError("include tools/quality_tidy.py in this commit with the updated checker")
                if ("tools/check_quality.py" in changes
                        and (snapshot / "tools/check_quality.py").is_file()
                        and b"from quality_snapshot import" in (snapshot / "tools/check_quality.py").read_bytes()
                        and "tools/quality_snapshot.py" not in entries):
                    raise SetupError("include tools/quality_snapshot.py in this commit with the updated checker")
            config = load_config(snapshot)
            prepare_changes(repository, snapshot, changes, base)
            if arguments.scope:
                apply_scope(snapshot, changes, base, arguments.scope)
            if arguments.task_scope:
                apply_scope(snapshot, changes, base, arguments.task_scope, task_only=True)
            return check(repository, snapshot, changes, config, arguments, base, index_before)
    except (SetupError, SnapshotError, OSError, ValueError) as error:
        print("UNAVAILABLE:", error, file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
