"""Source-only case discovery for the host runner's supported registration forms."""

import hashlib
import json
from pathlib import Path
import re

from regression_support import HarnessError, Source


CATALOG = "case-catalog.json"


def active_registrations(main, path, suite):
    defaults = {}
    # Fixture adapters override collision defaults; read declarations, not extracted gameplay.
    for dependency in (path.parent.parent / "include/config/config_collision.h",
                       path.parent / "regression_suites" / (suite + ".py"), path):
        if dependency.is_file():
            defaults.update({name: int(value) for name, value in re.findall(
                r"#define\s+(\w+)\s+([01])\b", dependency.read_text())})
    active, stack, result = True, [], []
    for line in main.splitlines(keepends=True):
        directive = re.match(r"\s*#(if|ifdef|ifndef|elif|else|endif)\b(.*)", line)
        if directive:
            kind, expression = directive.groups()
            if kind in {"ifdef", "ifndef", "elif"}:
                raise HarnessError(f"{suite}: unsupported registration guard {line.strip()}")
            if kind == "if":
                condition = re.fullmatch(r"\s*(!?)(\w+)\s*", expression)
                if not condition or condition[2] not in defaults:
                    raise HarnessError(f"{suite}: unsupported registration guard {expression.strip()}")
                enabled = bool(defaults[condition[2]]) != bool(condition[1])
                stack.append((active, enabled))
                active = active and enabled
            elif kind == "else":
                if not stack:
                    raise HarnessError(f"{suite}: unbalanced registration guards")
                parent, enabled = stack[-1]
                active = parent and not enabled
            else:
                if not stack:
                    raise HarnessError(f"{suite}: unbalanced registration guards")
                active, _ = stack.pop()
        elif active:
            result.append(line)
    if stack:
        raise HarnessError(f"{suite}: unbalanced registration guards")
    return "".join(result)


def discover(path, suite):
    source = Source(path)
    main = source.function("main")
    # Remove comments without removing string literals used by registrations.
    tokens = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/', re.S)
    main = tokens.sub(lambda m: " " if m[0].startswith(("//", "/*")) else m[0], main)
    main = active_registrations(main, path, suite)
    if suite == "punch":
        if (not re.search(r'for\s*\(int moving = 0; moving <= 1; moving\+\+\)', main)
                or '"punch.%s.%s"' not in main):
            raise HarnessError("unsupported punch registration; update case_catalog.py")
        names = re.findall(r'\{\s*"([a-z0-9_]+)"\s*,\s*\w+\s*\}', main)
        cases = [f"punch.{mode}.{name}" for mode in ("standing", "moving") for name in names]
    else:
        cases = []
        prefix = re.escape(suite) + r"\."
        # Literal registrations, tables, and RUN("short_name", fn) prefix macros.
        short = bool(re.search(r'"' + prefix + r'"\s+name', main))
        table = re.findall(r'\{\s*"(' + prefix + r'[a-zA-Z0-9_.]+)"\s*,', main)
        pattern = (r'\b(?:RUN|RUN_CASE|test_selected)\s*\(\s*"('
                   + prefix + r'[a-zA-Z0-9_.]+)"'
                   + r'|\btest_selected\s*\(\s*\w+\[\w+\]\.name\s*\)')
        if short:
            pattern += r'|\bRUN\s*\(\s*"([a-zA-Z0-9_.]+)"'
        for match in re.finditer(pattern, main):
            names = ([match[1]] if match[1] else
                     [suite + "." + match[2]] if short and match[2] else table)
            cases.extend(names)
        if not cases:
            raise HarnessError(f"{suite}: unsupported case registrations; update case_catalog.py")
    if not cases or len(cases) != len(set(cases)):
        raise HarnessError(f"{suite}: missing or duplicate case registrations")
    return cases


def build(directory, suites):
    entries = {}
    for suite, name in suites.items():
        path = directory / name
        entries[suite] = {"file": name, "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
                          "cases": discover(path, suite)}
    return {"version": 1, "suites": entries}


def refresh(directory, suites):
    catalog = build(directory, suites)
    (directory / CATALOG).write_text(json.dumps(catalog, indent=2) + "\n")
    return catalog


def read(directory, suites, selected):
    try:
        catalog = json.loads((directory / CATALOG).read_text())
        if catalog["version"] != 1 or set(catalog["suites"]) != set(suites):
            raise ValueError("suite inventory changed")
        result = {}
        for suite in selected:
            entry = catalog["suites"][suite]
            path = directory / suites[suite]
            digest = hashlib.sha256(path.read_bytes()).hexdigest()
            if entry["file"] != suites[suite] or entry["sha256"] != digest:
                raise ValueError(f"{suite} fixture changed")
            cases = entry["cases"]
            if cases != discover(path, suite):
                raise ValueError(f"{suite} registrations changed")
            result[suite] = cases
        return result
    except (OSError, ValueError, KeyError, TypeError) as error:
        raise HarnessError(f"case catalog missing or stale ({error}); "
                           "run tests/run_regressions.py --refresh-case-catalog") from error
