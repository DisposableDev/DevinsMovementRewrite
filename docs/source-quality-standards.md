# Coding standards and quality checks

Use the surrounding HackerSM64 conventions so changes remain easy to read and
review. The shared `.clang-format` uses four spaces, attached braces and a
104-column limit.

## Coding conventions

- Match nearby types, `s`/`g` ownership names and `TRUE`/`FALSE` usage.
- Use braces and one statement per line. Keep ordinary loop headers intact.
- Prefer direct conditionals; use a ternary for a simple two-way expression.
- Name meaningful tuning values, state distinctions and operations. Avoid
  wrappers or generic frameworks that obscure a small piece of game logic.
- Make initialization, ownership, reset and cleanup clear. Preserve update
  order, once-per-frame timers, temporary flags and numerical evaluation.
- Use query names such as `can_*` for queries, and action names for mutations.
  Document units, failure results and output validity at public interfaces.
- Explain non-obvious reasons in comments. Keep historical discussion in docs.
- Generate assets from their source inputs. Retain imported notices and credits.

Function length and helper counts are not quality gates. A readable change should
make its behavior and state transitions apparent. Checks do not substitute for
reviewing gameplay, storage ownership or cleanup.

## Run checks

From the repository root, select the affected files:

```sh
python3 tools/check_quality.py --path src/game/mario.c
python3 tools/check_quality.py --staged --path src/game/mario.c
```

`--path` is repeatable and selects changed lines in the named files. Without a
selector, the checker examines all working changes against `HEAD`. `--staged`
uses an isolated snapshot of staged source and dependencies; it does not stage
anything or mix in unstaged versions. `--base COMMIT` selects another comparison.

Check mode does not edit files or build a ROM. It reports whitespace, formatting,
Python syntax and parsed C/C++ brace issues. Imported/generated exclusions are
recorded in `.quality.json`. Parsed checks require a valid owning translation
unit and its dependencies, even when diagnostics select only a few changed lines.

| Exit code | Meaning |
| --- | --- |
| 0 | Checks completed successfully |
| 1 | A quality or behavior check failed |
| 2 | Tools, input preparation or parsing prevented completion |

No selected changes means not applicable; it is not a gameplay-test result.
Use `--verbose` for setup details and `--help` for all options.

## Check dependencies

Clang, clang-format and clang-tidy are pinned to **18.1.3**. Install matching
tools and Clang's standard headers, or set `CLANG`, `CLANG_FORMAT` and
`CLANG_TIDY` to their executable paths. An ignored `.quality-tools/llvm-18/usr/bin/`
installation is also recognized. These tools do not replace the N64 GCC compiler.

The checker obtains compiler settings from the read-only `make quality-flags`
target and prepares required headers/assets in a temporary directory. Missing
inputs or compilation failures are setup errors. An existing compilation
database can be selected with `--tidy PATH_TO_BUILD_DIRECTORY`.

Public CI uses temporary placeholders for ROM-dependent syntax inputs. Passing
that frontend check does not validate asset bytes or gameplay. Actual-data and
generator checks use their required ROM inputs separately.

## Formatting and tests

`--fix` explicitly formats working-tree C/C++ ranges. It does not format staged
content. If a replacement needs lines outside the selected scope, the checker
stops before writing; inspect the context before expanding the selection.

Quality checking and behavior testing are separate. Use the [test guide](../tests/README.md)
for gameplay cases and generator checks. When modifying the checker itself, run:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/test_quality.py
```

The Git hook and CI use the same checker. `.quality.json`, `.clang-format`,
`.editorconfig` and `.github/workflows/source-quality.yml` define its configuration.
