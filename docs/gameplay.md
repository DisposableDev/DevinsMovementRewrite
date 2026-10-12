# Gameplay guide

Devin's Movement Rewrite keeps Super Mario 64's courses and basic controls while
expanding movement, cap abilities and combat. The defaults use the revised
mechanics. Options can disable individual features or select original cap and
Bowser behavior.

## Controls to try

| Action | Input |
| --- | --- |
| Jump or wall kick | A |
| Attack or dive | B, depending on movement and whether Mario is airborne |
| Crouch or ground pound | Z on the ground or a fresh Z press in the air |
| Long jump or backflip | Z + A; movement and stick direction choose the action |
| Slide kick or breakdance | Z + B; movement chooses the action |
| Ground twirl | L during eligible ground movement |
| Air twirl | Press L during an eligible jump or fall |
| Jump directly into an air twirl | A + L together on the ground |
| Dab | Press and hold L, then press Z on the next game frame; hold both for analogue pose control |
| Leave Wing flight | Press Z; press Z again to ground pound |
| Drop from a pole or tree handstand | Press Z |

Carrying, damage reactions and scripted actions can restrict moves. These are the
main shortcuts; [detailed gameplay rules](../gameplay_rules.md) specify their
precise conditions and timing.

## Movement and combat

Movement preserves momentum through more landings, slides and water exits.
Many actions accept an input shortly before recovery ends, so you can link moves
without waiting for an exact animation frame. Wall kicks have a more forgiving
input window; backward long and triple jumps are also available.

Ground twirls rebound from walls and stun eligible enemies. Air twirls slow the
first descent of an airborne sequence, rebound from walls and can link into a
ground twirl when L stays held at landing. Holding L does not repeatedly start
new air twirls. Collecting a coin refreshes the air-twirl cooldown and its next
fall-slowing benefit. Twirling without Mario's regular hat can cost health on
an enemy recoil; it does not protect against every hazard.

Breakdances attack around Mario rather than only ahead of him. Ground twirls can
parry eligible projectiles. Swimming has dolphin-style upward launches, and
successful water exits carry horizontal momentum onto land. Pole and tree
grabs, climbing and sliding are faster; the castle flags are climbable too.

## Cap abilities

| Cap | Revised behavior |
| --- | --- |
| Wing | Faster ground running, water running at sufficient speed, enhanced air twirls and flight rolls |
| Metal | Greater resistance and traction, enemy-contact effects and ordinary land moves underwater, with water resistance |
| Vanish | Ordinary land moves underwater, protection from drowning and cold-water drain, and relief from Metal's water resistance |

Cap powers combine: Wing flight remains available with Metal or Vanish, and
Vanish can remove the resistance of underwater Metal movement. Water running
requires sufficient speed and cannot immediately catch Mario again after he sinks
below the surface. Dives, slides and ground pounds can deliberately enter water.

The Gameplay category offers Current and Original presets for each cap. A preset
change takes effect on a full level restart; the menu shows when one is pending.

## Health and hazards

Entering a playable course from the castle refills health. Passive healing begins
after 45 seconds without health loss and restores another wedge every three
seconds. Damaging exposure and prolonged inactivity can delay or reset it.

Water leaves Mario Soaked for up to five seconds on dry land. This can protect
against a fire or lava contact, but increases electrical damage; a consuming hit
or wind can remove it sooner. Vanish ordinarily prevents Soaked status. Losing
Mario's regular hat increases damage from many hazards.

Current Bowser fights add attacks and enemy waves. Bowser's attacks can strip
power-up caps, including combined caps. Optional throw assistance helps near-miss
throws; optional Hold spin assistance reduces the need to rotate the stick. The
Original Bowser preset restores the original encounter on the next level restart.

## Options, statistics and timers

Pause, choose Options and use L/R to switch between Controls, Presentation,
Accessibility, Gameplay and Cheats. A selects a row, B goes back, and Start saves
and resumes. Settings are stored separately for each save file.

Controls include camera speed, inversion and input filters. Presentation and
Accessibility include audio, HUD and display preferences. Gameplay controls
twirls, held poses, dab, fall damage, cap loss, passive healing, Soaked status and
the cap/Bowser presets. Options describe changes that need a course restart.

Statistics show each file's activity and records; see the [statistics guide](player_stats.md).
Course and Session timers are optional displays. Cheats and Debug Mode suspend
statistics and prevent eligible course records and new secret awards. Turning
them off resumes counting, but an affected course-time attempt or secret visit
needs a fresh full level reload to become eligible again.

## Custom content

The Physics Lab provides motion, moving-platform, collision and water areas for
experimenting with the controls. The game also includes an unlockable HE THICC
appearance and hidden encounters. The appearance keeps Mario's collision and
basic action set while applying its own accepted movement tuning.
