# Player statistics

Open Statistics from the pause menu to browse the active save file's lifetime
counters and personal records. Each of the four files has its own statistics.

## Navigation

| Input | Action |
| --- | --- |
| L / R | Previous / next category |
| Stick or D-pad Up / Down | Browse rows and pages |
| B | Return to the pause menu |
| Start | Resume playing |

Each page shows five rows. The seven standard categories are General,
Collectibles, Moves, Distance and Records, Combat, Damage and Recovery, and
World Interactions. A HE THICC file also has a character-specific category.
Values are read-only; browsing does not advance gameplay or write the save.

## What is recorded

- Play time, course visits, collected stars, coins, lives and cap pickups.
- Jumps, wall kicks, dives, slides, twirls, breakdances, swimming and other moves.
- Distance, speed and completed movement records, with their level/area.
- Confirmed enemy defeats, attack methods, bounce streaks and Bowser outcomes.
- Damage, recovery, cap usage, Soaked time and damage-free achievements.
- Object handling, switches, dialogue and other supported world interactions.

Time uses active game updates rather than real elapsed time. Pauses, demos and
credits do not contribute. Distances and speeds use game units, rather than
metres. Record locations show where the record was set; equal results retain
the earlier location.

## Cheats and eligibility

Cheats, Debug Mode and debug movement suspend counters and records while active.
Saved values remain readable. Counting resumes when play becomes eligible again;
activity during the suspended interval is not added later. An affected course-time
attempt remains disqualified until a full course reload.

Damage-free stars require a visit without health loss or pending damage. Healing
does not undo that loss. Defeat counters count confirmed kills, not stuns or
repeated hits on the same dying object.

## Saving

Statistics checkpoint during ordinary saves, deaths, warps and file changes,
and every 60 active gameplay seconds. Resetting can lose activity since the last
successful checkpoint, including unfinished movement records. A storage failure
can extend that interval.

The game uses 32 KB SRAM and validates backup records. Older supported statistics
formats migrate automatically; newly added counters start at zero. EEPROM saves
are not automatically imported. Copy and erase operate on the selected file's
statistics along with its normal save management.

See the [developer reference](reference/player-stats.md) for storage layout,
tracking hooks and how to add a counter.
