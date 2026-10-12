# Contributing

Build the game with the [build guide](building.md), then use the source map below
to find the code you want to change. The [gameplay guide](gameplay.md) explains
the player-visible behavior; [detailed gameplay rules](../gameplay_rules.md)
record exact timing and edge cases.

## Source map

| Area | Starting point |
| --- | --- |
| Mario actions and movement | `src/game/mario.c`, `mario_actions_*.c`, `mario_step.c` |
| Combat and object interaction | `src/game/interaction.c`, `mario_punch.c`, `behaviors/` |
| Collision queries and terrain loading | `src/engine/surface_collision.c`, `surface_load.c` |
| Camera and scene rendering | `src/game/camera.c`, `src/engine/rendering_graph_node.c` |
| Pause menus and options | `src/game/ingame_menu.c`, `game_options*.c` |
| Statistics | `src/game/player_stats*.c` and [storage reference](reference/player-stats.md) |
| Models and levels | `actors/`, `levels/`; [Physics Lab fixtures](../levels/physics_lab/README.md) |
| Audio | `src/audio/`, `sound/` |
| Build and validation tools | `tools/`, `tests/` |

## Make a change

Describe the behavior you want to change and an example that demonstrates it.
Trace the shared action or object path so equivalent cases get the same fix;
trees and poles, for example, share climbing actions. Keep unrelated changes out
of the patch and preserve existing work in the checkout.

Follow the local naming and layout conventions. [Coding standards](source-quality-standards.md)
explain the formatter, check commands and important state/update-order contracts.
When changing generated assets, edit their generator or source input and use its
check mode rather than hand-editing generated output.

ROM-derived numeric assets are reconstructed during extraction. Asset-reading
tools use `tests/source_asset.py` to expand those includes from the selected
source and ROM data. See [source releases](source-release.md) for the extraction
boundary, and [asset sources](asset-sources.md) when adding or adapting artwork.

## Verify and describe the result

Use the [test guide](../tests/README.md) to find the cases covering your change.
Run those checks and the quality checker for the affected files. Changes to
graphics, sound or controller feel also need an appropriate in-game check:
host tests cannot establish their appearance or feel. Performance changes need
matched measurements; see [N64 performance](n64-performance.md).

Update player or developer documentation when the behavior or interface changes.
In a contribution, describe the problem, resulting behavior, checks performed
and any remaining limitation. Keep build results, host checks, emulator playtests
and console measurements distinct.

## Tools that are optional

Fast64 and editor configuration help with authoring and navigation. Capture and
profiling tools help investigate performance. None is required merely to build
the ROM. See [optional components](optional-components.md).

The `llm/` directory contains agent procedures and development history. Human
contributors can use the public guides without following that workflow.
