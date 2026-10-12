# Statistics implementation

The [player guide](../player_stats.md) describes the menu and visible behavior.
This reference covers the storage and event contracts needed to modify it.

## Source ownership

| File | Responsibility |
| --- | --- |
| `src/game/player_stats.h` | Stable IDs, units, storage constants and public API |
| `src/game/player_stats.c` | Labels/categories, lifetime values, records and SRAM serialization |
| `src/game/player_stats_tracking.c` | Course visits, movement sequences and attack/defeat credit |
| `src/game/player_stats_menu.c` | Read-only category and row navigation/rendering |

Use the source catalog for current labels and IDs rather than maintaining a second
full table in documentation. Statistics use saturating unsigned 64-bit values;
the menu displays `Lots!` at the maximum. Equal records keep their earlier location.

Time is in game updates: 30 per second in US and 25 in EU. Distance, speed and
health use a fixed-point scale of 256. Record locations store level and area.
Warps and interruptions reset transient movement baselines; only completed
sequences become movement records.

## SRAM layout

Only `SAVETYPE=sram` is supported. The cartridge reserves 32 KB, with the ordinary
save buffer at the beginning and statistics in the final 16 KB.

| File | First record | Second record |
| --- | --- | --- |
| A | `0x4000` | `0x4800` |
| B | `0x5000` | `0x5800` |
| C | `0x6000` | `0x6800` |
| D | `0x7000` | `0x7800` |

Each copy reserves `0x800` bytes. Version 7's record is 1808 bytes and 16-byte
aligned, with `PST1` magic, version, generation, payload size, file identity,
counter values, record locations and CRC-32. Compile-time assertions protect
record size and region boundaries.

Writes use the alternate copy and verify it by reading it back. Loading chooses
the newest valid current generation. Supported older versions migrate values and
locations while leaving new IDs at zero. Legacy versions 1/2 remain at `0x1000`
with a `0x600` stride until both migrated backups are safe; old headers are then
invalidated with verified writes. A valid current record takes priority.

An initial transport-read failure protects the affected file from implicit
writes, including when another backup is readable. Successful reinitialization
or explicit destination copy/erase releases that protection. Failed writes stay
dirty for retry. Copy rejects an unreadable source before replacing its destination.
Statistics and base-save writes are separate transactions, so their individual
journals do not make a combined copy/erase atomic.

## Event tracking

Credit attacks when the player hit is accepted, and count defeats only on
confirmed terminal death. Stuns, repeated death callbacks and uncredited
environmental deaths are not kills. Segmented enemies propagate credit to the
root; credited thrown objects can inherit the original attack. Normal allocation
and deallocation clear per-object metadata; in-place respawns must clear it too.

The shared cheating state suspends tracking. Eligible play starts fresh transient
sequences after suspension, excluding the earlier ineligible interval. Course
visits, damage-free eligibility and enemy streaks must retain their distinct
reset rules. Area changes remain inside a visit; a different-level warp, death
or explicit course exit ends it.

## Extending the catalog

1. Append IDs in `player_stats.h`; never renumber shipped IDs.
2. Add the label, category and units in `player_stats.c`.
3. Add tracking at the accepted event, with cleanup for interruption/respawn.
4. Extend the relevant tests and check saturation, file isolation and eligibility.
5. If serialization changes, bump the version and implement explicit migration.

For enemy coverage, add the supported enemy predicate and use
`player_stats_attack`, `player_stats_inherit_attack` and
`player_stats_enemy_defeated` at their corresponding events. The statistics tests
run production code with controlled engine services:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/test_player_stats.py
```

These host tests cover logic and storage failure handling. Menu appearance,
controller feel and actual cartridge durability require separate runtime checks.
