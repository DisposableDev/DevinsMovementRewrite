# Testing

The tests run selected production code on the host with controlled engine
services. They check gameplay logic, assets, storage and tools without building
a complete ROM. Graphics, sound, controller feel and console performance require
separate runtime checks.

## Find and run gameplay cases

From the repository root, list cases for a relevant suite:

```sh
python3 tests/run_regressions.py --list --suite geometry --match twirl
```

Listing with a selected suite does not compile. `--match` filters case names;
use `--help` to see the available suites. Run exact cases from that list:

```sh
python3 tests/run_regressions.py --case geometry.air_twirl_cycle \
  --case geometry.air_twirl_gravity
```

Repeat `--case` to batch checks and share fixture compilation. `--suite NAME`
runs the whole selected suite. `--source-root /path/to/checkout` checks another
source tree with the current harness. The case catalog provides navigation;
`--check-case-catalog` checks freshness, and `--refresh-case-catalog` regenerates it.

## Other focused checks

| Change | Starting check |
| --- | --- |
| Statistics/tracking/menu | `python3 tests/test_player_stats.py` |
| Source checker or hook | `python3 tests/test_quality.py` |
| Source reconstruction/export | `python3 tests/test_public_source.py` |
| Capture protocol/reporting | `python3 tests/test_performance_capture.py` |
| Build dependencies/setup failures | `python3 tests/test_build_rules.py` |
| Particle allocation/lifecycle | `python3 tests/run_particle_budget_tests.py` |
| Generated assets | The owning generator's `--check` mode |

Use `PYTHONDONTWRITEBYTECODE=1` to avoid Python bytecode files. Individual test
modules under `tests/` describe their controlled services and dependencies.
Some asset checks require matching baseroms. `tests/source_asset.py` expands
ROM-derived includes privately from verified data; frontend placeholders are
for syntax checking and are not asset-verification evidence.

## Meaning of a result

A behavior failure indicates a tested expectation was not met. An extraction,
compiler, missing-input or matcher failure means the harness did not complete;
it is neither a gameplay failure nor a pass. Check the selected case exercises
the changed behavior and preserves its relevant boundaries.

Optional mutation checks verify that known defects are detected:

```sh
python3 tests/run_regressions.py --list-mutations --suite punch
python3 tests/run_regressions.py --self-check --mutation punch_explosion_missing
```

They modify temporary source copies, not the checkout. A compilation failure or
unmatched mutation is a harness error. Ordinary test runs do not require mutation
checking.

## Add a regression

Use the closest fixture in `tests/regression_suites/`. Exercise an observable
outcome and relevant preservation case, and identify which services are supplied
by the fixture. For a defect, demonstrate failure on the old implementation or
a focused mutation. Add new suite/case metadata to the catalog through its
refresh command when required.

See [contributing](../docs/contributing.md) and [quality checks](../docs/source-quality-standards.md)
for the source workflow. Use [performance captures](../docs/performance-capture.md)
for emulator measurement; passing host tests does not prove an N64 speedup.
