# Accepted gameplay rules

## Options persistence foundation

Options use per-file SRAM records outside the base save and both statistics
layouts, with generation/checksum validation and readback-verified backup writes.
If the base save cannot be read, automatic writes and corruption recovery must
preserve the unread files, menu data, statistics, times and settings. Partial
read contents are discarded. Ordinary saving resumes after a successful load;
explicit erase replaces only its selected file, and copying requires a readable
source file. A successful read of invalid data still permits ordinary recovery.
Failed base writes reject file copies before auxiliary changes. The destination
retains its previous state and remains available for retry; the menu reports
completion only after the copied base progress is written.
File-menu erasure also requires a successful base write before its completion
animation and auxiliary resets. A rejected write keeps the file, prompt and
button models available for retry. Automatic invalid-save recovery still clears
untrusted progress and retains its pending write.
Unread settings are not overwritten after a transport failure. Cheats enable consent is session-only. Debug Mode and Debug free movement
are saved per file and survive restart; enabling Debug Mode still requires confirmation. The
catalog/storage, paused options shell, Settings/save deletion, Controls, and the
debounce/minimum-hold input filters are implemented. Implementation checkpoints are retained in the optional
`llm/docs/options-menu-implementation.md`; player guidance is in `docs/gameplay.md`.
The eight audio and six display controls and Course/Session timers are integrated.

Options appears below Statistics in the pause menu. A opens the selected Options
row, and B also opens options. L/R change categories in the approved
order; B returns without resuming, while Start saves and resumes. Save deletion
requires a fresh two-second continuous A hold over YES, bound to its selected
file; release, cancellation, cursor/file changes or skipped updates reset it.
Cheat/debug warning pages require release followed by five elapsed seconds of A.
Both warning pages display hold progress in seconds to one decimal place.
Every file uses the same concise warnings: "Enable debug mode?" followed by
"While enabled, press D-pad Up + L." Debug and cheat warnings both state that
records and statistics are unavailable and secrets stay blocked until restart.

HE THICC files have an Accessibility option "Disable THICC text", off by default
and saved per file. On disables all HE THICC name and spelling replacements;
Z restores the default. Other files do not show this option. Character appearance,
movement and save-file names are unchanged.

The empty Settings page is removed. Options initially open on Controls and cycle through
Presentation, Accessibility, Gameplay and Cheats; the version remains on every
page. Per-file storage and held save-deletion confirmation are fixed behavior.
Reopening Options after B or unpausing restores the last category and selected
option for the current file. This navigation position is session-only; switching
files or rebooting starts on the first Controls row. Held inputs and unfinished
enable warnings are cleared on reopening.

Failed explicit progress saves retain a retry through Game Over. Reloading restores
the last attempted save and discards later unsaved changes; a successful retry persists
its keys and stars. Other files retain their own pending saves.

Debug Mode controls debug shortcuts and tools independently of Cheats enabled.
For record and Easter egg eligibility, any enabled Cheats-page preference counts
as cheating, including Debug Mode and Debug free movement even before use or
without their master permission. Actual debug flight, legacy debug level selection,
UNLOCK_ALL and COMPLETE_SAVE_FILE also count. Ordinary presentation, accessibility
and gameplay preferences do not. Cheat controls take effect immediately. Enabling
any cheat taints Easter egg eligibility for the current visit, even if disabled
before the next world update. Legacy debug selection and actual debug flight also
taint it. Turning cheating off does not restore secrets or a disqualified
course-time attempt; a fresh full level reload starts a new eligibility check.
The reload ends outgoing debug flight before checking eligibility; enabled cheat
preferences and developer build flags still disqualify the new visit.
Ordinary within-level warps and area transitions do not clear the restriction.
Statistics resume immediately once the current cheating state is clear.
All Easter egg activation and new secret awards are blocked for a tainted visit:
the pause quote, rare scream, JRB encounter and optional NPC messages, castle
flag/window/portrait secrets, hidden ending, rooftop Yoshi and completion 1-Ups,
fountain and TAS HE THICC unlocks, circle-running cap loss, shallow-water sink
death, rare shallow-water splash fish and title face-tile Easter egg. Existing saves
and unlocks are preserved;
ongoing dialogue, room confinement and exit/death cleanup remain functional.
Both master permissions require the active playing file and are ineffective in
demos/credits. The legacy title selector cannot bypass consent. Disabling Debug
Mode clears overlay/page/navigation state, queued debug warps and trial fast walk
before the next world update. Private credits profiling keeps only its read-only
General overlay without granting debug controls.

Ordinary builds select the `DISABLE_ALL` release preset. It disables test-level
startup, boot-time level selection, automatic unlocks/fake completed saves,
legacy debug shortcuts, uncapped FPS, assertion-triggered crashes and detailed
capture/collision instrumentation. Player-controlled Debug Mode, free movement,
debug warps/pages, input/controller displays and crash diagnostics remain.
Explicit debug, benchmark and capture builds retain their developer configuration;
an explicitly selected release preset overrides those switches and rejects GCC
instrumentation. Release builds ignore local configuration overrides.

Save signatures use `UNIQUE_SAVE_DATA` with the stable `DevinMovementRewrite`
internal ROM name. Incompatible base saves reset progression, options, statistics
and records at boot; matching saves load normally.

Debug free movement additionally requires its own On setting. D-pad Up enters;
stick/Up/Down move, B accelerates, Z slows, L restores health and A exits. Revoking
either setting exits on Mario's next update, restoring the ordinary camera and
water/floor/air action without healing, movement or a new upward boost. Collision,
cap-in-hand and render/squash cleanup remain active. Ordinary first-person controls
retain their existing behavior. The production camera is ReonuCam; the unused
Puppycam implementation has been retired.

Environmental cheats default Off and require Cheats enabled for the active playing
file. Ignore slippery surfaces uses ordinary ground traction for slippery/ice,
slide-course and tilted Bowser platform surfaces; grippy floors and ordinary steep
slopes retain their rules. Ignore sand removes normal sand/slow-floor slowdown,
burial, sinking and moving quicksand. It clears existing sand constraints before
input processing, preserving snow burial and unrelated shared death actions.
Ignore wind removes horizontal forces both on the ground and in the air, vertical
forces, gust/tornado capture, wind cap
loss and wind drying; enabling exits existing wind captures. Wind death planes
remain fatal without wind-entry grace. Ignore water currents removes floor flow,
whirlpool attraction/repulsion and capture; enabling releases an existing capture
into the appropriate ordinary water/underwater action. Collision, drowning and
other death planes remain active. Disabling restores the native environmental
rules on subsequent updates without storing altered surfaces or objects.

Option values wrap at both ends with Left/Right or confirmation, retaining each
option's increments and warning/arena restrictions. Unavailable choices are
skipped. Controller Display Off is available while Input Display is On.
A fresh Z press resets the highlighted setting to its default, retaining arena
restrictions. Resetting Controller Display leaves Input Display unchanged.
Action rows do not reset.

Infinite health restores full health before damage/death input processing and
clears pending hurt/heal bookkeeping. Suppressed contact or queued damage retains
Mario's damage blink and grants at least 30 updates of invincibility; longer
existing invincibility is preserved, and idle updates do not renew it.
It prevents health costs from enemies,
burning, poison, drowning and falls, while preserving death planes, scripted
deaths, collision and ordinary reactions when Disable knockback is Off.
Disable knockback prevents harmful enemy, Bowser, shock, fire/lava, wind, thrown
and heavy-fall launches, without removing damage when Infinite health is Off.
Enabling it clears an existing harmful capture/reaction, releasing enemy ownership
and extinguishing fire before returning to ordinary movement. Deliberate Metal
ground-pound lava recoil and forced death/warp actions remain active.
When both cheats are effective, new Mario damage voices are suppressed, including
attack, hurt grunt, burn and painful landing voices. Other voices and sound
effects remain audible; disabling either cheat or the master restores damage voices.

Ordinary release and diagnostic builds retain earned progression: no automatic
moat drainage, cap-switch unlocks, cannon unlocks or door bypasses. Vanilla
level-specific progression checks are enabled. Explicit full-unlock and benchmark
presets retain their opt-in overrides. Breaking the castle secret window keeps
its collision until its replacement room has published collision; allocation
failure retries with the glass intact.

Nonstop stars are disabled. Ordinary stars use the normal course-exit flow and
course-specific star-dance camera. Explicit no-exit stars, including 100-coin
stars, retain their existing behavior; Bowser keys and the Grand Star retain
their normal progression.

The castle pause choices Continue, Statistics and Options form one vertical list,
selected with Up/Down and wrapping at either end. The course viewer uses
Left/Right, preserving empty-course skipping and secret-star wrapping. Horizontal
input does not select pause choices; vertical input does not change the course.
Z plus horizontal D-pad input retains the camera-speed shortcut. Debug Mode's
warning identifies its activation chord as D-pad Up + L.
Pause titles and choices share a horizontal centre, including with the controller
overlay visible. Each choice uses its actual text width, and the cursor follows
its left edge. The PAUSE title leaves at least ten pixels above the first choice.

Active controller offers Automatic and only ports present in the latest completed
physical poll. The menu skips unplugged ports in both directions, and direct
setting changes reject them. Automatic uses the first connected port. A saved or
previously selected explicit port may later disconnect; it retains its preference
for reconnect and provides no gameplay actions while missing. A connected recovery controller can use Start
to reach pause and then operate menus. Switching ports, reconnecting or changing
file/menu/filter state quarantines held buttons until release. Disconnect pause
uses the ordinary pause gate once per loss, retaining special-room/dialogue/warp
restrictions. Physical samples remain unchanged by GameCube button remapping.

Controller deadzone is 0–63 raw units, default 6, with the first active unit mapping
to 1 and the existing circular magnitude limit retained. Camera presets retain
2/3/4/6/8 degrees per update; Custom uses 0.5–10 in half-degree steps. First-person
look scales with that speed. Inversion changes camera controls only. Rumble Off
stops motor output; Low halves requested pulses without changing effect timers;
changing ports stops the old motor before using the new port.

Music and effects volume compose with native fades; Mario voice additionally uses
the effects slider, including all three streamed clips. Muting a stream leaves
playback advancing. Pause music defaults Quiet (20%); Normal is 100% and Silent is
0%. Unpause retains the ordinary restoration fade. Stereo/Mono belongs to the
active save file during gameplay; file select retains its global sound choice.
Master and balance apply to completed mixed PCM, with balance
trimming the opposite channel. Reduced audio range uses linked stereo peaks,
quiet makeup and bounded compression before user gain, preserving exact mute.
Default master/balance/range output is bit-for-bit unchanged.

The ordinary HUD is suppressed while the options page is open so that labels and
the version remain readable. This does not alter HUD flags, health or meter state.
Course and Session timer text has a one-pixel down-right black shadow, matching
the cap timer's shadow opacity and fading with the foreground text.
When both timers are shown, their two rows are centered vertically on the cap
timer's text baseline, retaining 14 pixels between rows. A single timer retains
that same baseline.
Automatic retains the ordinary reveal policy. Always shows enabled counters and
meters; Hidden changes drawing only, retaining meter updates. Always uses a
temporary meter draw position. In widescreen, Near centre insets edge groups by
40 logical pixels and keeps meters below the counter row; Screen edges is the default and retains
the original anchors. The existing 16:9 projection follows the active file, and
4:3 remains unchanged. Menu control hints always remain visible; Button prompts is no longer an option.

Debounce accepts the first press immediately, then requires its chosen consecutive
released-frame count to rearm. Minimum hold accepts only after its chosen
consecutive held-frame count, once per hold. Each is a single frame-count setting
from 0–30, defaulting to 0 frames (disabled). Start and menu inputs bypass these filters. Releases remain
immediate, and pending holds cannot survive pause/file/port changes.

Menu navigation supports Stick, D-pad or Both across options, statistics, pause,
dialogue choices and star selection. Require release between menu selections
allows one discrete choice per hold from either the stick or D-pad, then requires
all enabled directions to be released or neutral before another choice. Options
warning pages keep tracking navigation holds, so leaving a warning does not
count as a release or immediately repeat a held choice when release is required. A stick
held when pause, Options or Statistics opens must return to neutral before it
can move a selection or change a value. Discrete menu navigation accepts only
one direction from the enabled sources: diagonal stick positions, multiple
D-pad directions and conflicting D-pad/stick directions produce no movement.
Adding another direction after a clean press stops navigation without adding a
choice; release all directions before resuming. Starting a new dialogue or menu
clears navigation state from the previous context. Returning from Options or
Statistics also clears old pause navigation and quarantines any held stick.
With the release requirement Off, clean single-direction repeat
and confirmation buttons retain their existing behavior. File
selection retains continuous pointer movement, with the chosen sources. Dialogue
buttons and playerless menus bypass gameplay input filters. A held before a
dialogue or pop-up opens carries into it without a new press edge, including a
pending minimum hold. Other held buttons, and simultaneous file/controller/filter
changes or reconnects, retain release quarantine. Ordinary response choices still
require a fresh confirmation; the existing cap-switch hold-to-save exception remains.
After collecting a star, held A automatically accepts Save and Continue once the
save prompt is ready, even when held before the course transition. The star-exit
sequence carries A as held input without generating a fresh press; deliberate
selection of another save-menu row still requires a fresh confirmation.

Bowser spin assistance is a Cheats option, defaults Off and requires Cheats
enabled. It follows the same record restrictions as other cheats.
Hold accelerates from a held stick direction
by 0x80 spin units per update to the normal 0x1000 maximum, keeping spin direction
until neutral. Neutral uses the ordinary 0x40 decay. Throw prediction follows the
same function on a private copy; throwing still needs a separate press.

Bowser's pickup, spinning, inhaling, roar, defeat, laughter, teleport, fire and mine
explosion cues take priority over competing wave enemies and boss footsteps.
Their samples, timing and distance-based volume remain unchanged.

Water-entry splash uses the impacts bank so it can play alongside breaststroke,
fast breaststroke and flutter-kick sounds. Swim cues share priority and can replace
one another. Twirl and plunge-overlay sounds retain their separate banks.

Grounded stunned Swoopers use a level rendered pose with 10 scaled units of floor
clearance. Their collision position, native flight angles and stun timing are unchanged;
airborne, underwater and hazard poses retain their existing rendering.

Debug fly allows pausing while preserving the usual dialogue, warp and special-room
pause restrictions. Other intangible actions retain their existing pause rules.

Allow adjusting camera when paused defaults Off. On the main pause page it uses
the ordinary Mario camera modes, C-button/R controls, speed, inversions, zoom,
smoothing and geometry collision checks. The adjusted view persists on resume.
Mario, objects, gameplay timers, shake RNG and pending cutscenes remain frozen.
Field-of-view curves and shake phase/decay also freeze at the last rendered view
and resume on unpause, with either paused-camera setting.
Entry holds, duplicate updates and Z shortcuts cannot accidentally move it. Camera shake On/Off
uses 100/0 percent output while retaining effect timing and random updates.
Reduced flashing makes cap-expiry warnings steady at half opacity and damage
invincibility steady at quarter opacity, preserving darker warp fades, protection,
expiry and hitboxes. Mario and mirror silhouettes follow the same steady damage
opacity. The star-counter warning stays steadily at half opacity. Low-health
warning sound Off mutes its dedicated bank without changing pulse timing or rumble.

Switch activation is hidden from Options and fixed to Current, including for
previously saved Original selections. Current accepts the existing Metal and
underwater pound impacts in addition to ordinary contact. It retains current
press latches, refresh/expiry, structure/box lifetimes and collision fixes;
unrelated poundable objects keep their own impact rules.

Wing Cap Current retains the hack's flight, water-walking, faster ground walking,
swimming launch and held-A impact protection. Original restores flight's native
pitch/turning speed loss and stalls, flight Z ground pound and takeoff B/Z exits,
and disables those Wing enhancements. Original takeoff B keeps priority over Z,
including the shared input stage's retained simultaneous chord. Slow falling,
original Wing fall thresholds, cap timers, combined caps and warning rendering
remain. Twirls and their Wing rolls/benefits are independently controlled, and
other global movement and collision fixes remain active.

Metal Cap Current keeps the hack's resistance, traction, wind immunity, heavy
impact/squash, passive stun and underwater land actions. Original restores native
Metal underwater walking/jumping/sinking, water-damage immunity and ordinary
fall/knockback/lava reactions. Electric contact still reacts but causes no damage
and retains its powers. Vanish Cap Original uses ordinary swimming, breath/cold
water damage, drowning, Soaked status, burial and water effects, while keeping
native phasing and fire/electric immunity. Current Vanish keeps the enhanced
water and environmental behavior.

Mixed presets evaluate each cap independently: either Current Metal or Current
Vanish permits land actions underwater; Current Vanish or Original Metal prevents
water damage. Original Vanish does not cancel Current Metal's resistance. Preset
changes preserve cap timers, held-shell ownership, fall height and shared safety
fixes; Original Metal clears only queued Metal impact/squash. Twirls and their cap
benefits remain independently controlled. Current Bowser scripted flame removal
keeps priority over Original Vanish immunity in Current fights. Original Bowser
flames use native cap interactions.

Bowser fights defaults to Current. Bowser fights and the
Current/Original Wing, Metal and Vanish Cap presets are selected immediately in
Options but applied only on a full level reload, before object initialization.
They can be queued inside a Bowser arena without altering the ongoing fight.
Pending selected rows show "Change applies on course restart." Saved selections
retain their normal per-file storage. All other preferences remain immediate.
Original restores native health 1/1/3, AI/fire/teleport/jumps, body and landing
reactions, tail-grab/hold rules, mine range/consumption and final-arena platform
progression. Fight 3 Original big jumps emit one native two-ring wave; quick jumps
emit none. Current enemy waves, scripted cap stripping and extra progression are
disabled in Original. Shared collision, ownership, recovery, reward and cleanup
fixes remain; cap presets, Twirls and both throw/spin assists stay independent.

Gameplay's nine On/Off rows default On and are integrated. Bowser throw
assistance Off skips prediction and automatic adjustment, independently of Hold
spin assistance. Twirls Off blocks ground, air, swimming and Wing flight rolls,
including chained/buffered entries. Enemy/tornado release twirls use ordinary
freefall with their incoming bounce velocity; tornado capture remains a hazard.
Active twirls retire before interactions, restore their displayed orientation and
clear pending input/control boosts. Airborne cancellation retains fall height.
Held twirl pose Off leaves the spin/recovery available and ends the held pose.
Dab and Circle dizziness Off retire active poses and discard shortcut/rotation
progress; already dropped hats are not restored by these preferences.

Fall damage Off blocks future landing damage without removing Metal landing
feedback, wet splash effects, burial or hazard damage. Already queued damage still
runs. Cap loss Off blocks new theft, hat knock-off, wind/death cap removal and
queued lethal cap loss, without changing existing dropped/stolen cap ownership.
Scripted Bowser/shock power stripping remains part of those encounters. Passive
healing Off clears automatic regeneration progress; coins, hearts, respawn healing
and hazard drain retain their ordinary order. Re-enabling starts a fresh delay.
Soaked status Off clears its timer, drips and consumed flame-protection latch,
and removes wet-status fire protection and electrical damage multipliers,
including the multiplier from ongoing water contact. Ordinary swimming, water
movement and fire extinguishing by submersion remain available.

This records settled custom behavior. Check it before fixing or reorganizing the
relevant feature. Update it when the user changes that behavior; do not infer a
new design from an assistant's previous claim that something was fixed.

## Animation cancel tuning

Player-facing animation frames start at 1; code animation frames start at 0.
Gameplay timers are counted separately and pause with gameplay.

| Animation or action | Accepted timing |
| --- | --- |
| Jump-kick recovery | Nine gameplay updates; a B follow-up for kick or dive expires after six updates, including the press update. B must remain held; early presses do not wait indefinitely. |
| Air-throw landing | Ordinary landing controls open on animation frame 7, after object release; the fourth-update object release remains unchanged. |
| Uncrouching | Standing controls open on animation frame 4; earlier backflip, punch and direct crouch reversal remain. |
| Crawl exit | Legal crouching or standing controls open on animation frame 3. Held Z keeps crouch controls; released Z or C-Up uses standing controls. |
| Turnaround finishing animation | 16 samples, preserving its first and last poses. |
| Single/double-jump landing animation | 16 samples, preserving its first and last poses. |
| Triple-jump/backflip landing animation | Shared 28-sample animation, preserving its first and last poses. |
| Light-object jump landing animation | 16 samples, preserving its first and last poses. |
| Twirl landing | Ordinary landing controls are available immediately; A/B/Z presses carry through airborne twirling into landing for six gameplay updates, including the press update. Released buffered buttons remain valid; other exits discard them. |
| Slow pole/tree grab | Input cancels open on animation frame 3; no Mario “Whoa” voice on slow grab. |
| Fast pole/tree grab finishing phase | Input cancels open on animation frame 11; its voice and initial spin remain. |
| Slow ledge climb | Movement/jump cancellation opens at action timer 24. |
| Fast ledge climb | Returns ordinary control and plays its landing sound on animation frame 6; six-update A/B requests replay at control return, and strong forward stick input adds 20 forward speed once. The 16-sample asset remains unchanged. |
| Ledge hanging | Z release and directional stick controls require timer 8; A climb remains immediate; forced release remains immediate. |
| Wall-bounce ground twirl | Walking opens after seven gameplay updates; attack/jump/crouch windows remain unchanged. |
| Ordinary flat forward butt slide | Jump delay is six gameplay updates; slope and backward-slide delays retain their separate rules. |

Crouch entry has a one-update Z entry delay in idle/walking. Its immediate A/B,
release-Z and C-Up exits have no additional input buffer. Crawl-start long jump
is a narrow strong-stick fresh-A window on animation frames 1–4; ordinary crawling
has no equivalent long-jump exit.

## Input buffering

- Buffer lengths count gameplay updates, including the press update. A six-update
  buffer accepts the press update and five later updates; a three-update buffer
  accepts the press update and two later updates. Requests age once per gameplay
  update, including during cooldowns, and are consumed once. Pause does not age them.
- The six-update landing-jump buffer also covers ordinary freefall, held-object
  jumps/falls, steep jumps, pole-top jumps, water-exit jumps and other ordinary
  airborne landing paths. The initiating takeoff press does not count. Release
  is allowed; damage, interrupted airtime and unsupported landings discard it.
  Backflip landings also accept released A presses: holding Z at execution
  selects another backflip; releasing Z selects an ordinary jump. Both use
  the six-update fresh-A window; holding A alone does not refresh it.
  A valid request at contact survives into the following landing update,
  including when walking off a ledge with empty hands or a held object.
- Fresh airborne A can wall kick on contact or up to two updates before it.
  The post-contact wall-kick window is six updates. Existing contact, speed,
  approach-angle, ledge priority and backward/water-jump locks still apply.
- Ordinary airborne ground-pound entry buffers Z for three updates while its
  current entry restriction clears. Release is allowed. Entering ground pound,
  landing, damage or leaving ordinary airborne control consumes/discards it.
- Dive-pickup stand-up and light/heavy object-throw recovery buffer A/B for six
  updates until their existing cancel window opens. Released presses remain
  valid; hazards, action exit and consumption discard them. Bowser release permits
  all ordinary idle cancels after animation frame 25 (starting at frame 26), with
  six-update A/B/Z buffers including the press update. Released Z can crouch;
  buffered A+Z can backflip. The initial throw/drop remains on the first update.
  Cannon firing remains unbuffered.
- Pole/tree jump-off and Z release use three-update buffers during their shared
  attachment/grab/handstand actions. Ceiling-hang A/B/Z release and ledge Z
  release use three-update buffers, including attachment/climb animations.
  Leaving the attachment family clears them. Ledge A climb and the existing
  ledge-climb A jump remain available and retain their original input timing.
- Ground twirl remembers an L press for six updates through cooldown and
  temporarily ineligible grounded actions. L may be released; a new press
  refreshes the one pending request. Entry consumes it once, and leaving ground,
  death or shortcut cancellation discards it. Swimming, pole/ceiling/ledge
  attachments and cannon control cannot retain or queue a ground-twirl request.
  Ordinary airborne L uses its own
  six-update buffer; the grounded A+L chord retains its one-update handoff.
- Jump kicks allow airborne cancels from gameplay update 16, counting entry as
  update 1: B starts a forward dive (selected by the current stick),
  Z starts a ground pound, and L starts an eligible air twirl. Each press buffers
  for three updates and survives release; neither A nor B needs to stay held.
  The initiating B press is excluded. Consumption clears the request, so one
  press cannot repeat forever. Without a cancel, the animation finishes normally;
  the cancel window stays open during recovery. A second jump kick cannot cancel
  the first: it requires the completed animation plus ten recovery updates.
  Its B request retains the three-update expiry.
- Ground-twirl recovery remembers the latest A/B/Z press for six updates.
  Release is allowed; an expired press cannot cancel recovery. The existing
  cancel window and A-before-B-before-Z priority remain.
- Slide jumps remember a fresh A press for six updates, even after release.
  The existing jump delay still applies; expiry cancels the request.
  This includes held-object butt slides.
- A forward dive requested during the first update of a butt-slide jump retains
  its existing one-update buffer: it executes on the next update even if B or
  the stick is released. The grounded A+L air-twirl chord likewise retains its
  existing one-update buffer; these windows are not widened.
- An underwater B tap released within six updates can queue a punch through
  cooldown. Holding A/B again cancels a completed tap. Ordinary initial A swim
  taps and airborne water-entry taps both use a six-update window.
- Strong/air-entry downward water bursts remember a fresh Z repeat for six
  updates, including the press update. Expired requests cannot restart the burst;
  floor contact, interruption and repeat consumption discard them. Holding Z
  alone never queues a repeat. Weak bursts retain their existing restrictions.
- All other moves retain their existing unbuffered input handling. No universal
  button replay, stick buffer, Bowser-throw buffer or cannon-fire buffer is added.

## Ground punches

These controls apply to both standing and moving punches.

| Input | Expected result |
| --- | --- |
| Hold B continuously from the start | Repeat first punch, second punch, ground kick until B is released. |
| Release B, then press and keep holding it during punch one | Repeat the first punch until B is released. |
| Release B, then press and keep holding it during punch two | Repeat first–second pairs until B is released. |
| Release B before the repeat recovery finishes | Cancel the pending repeat; the current punch finishes normally or uses an existing recovery cancel. |

- The release/re-press gesture is accepted during the punch animation as well as
  its recovery. The initiating B press alone does not select a repeat mode.
- A held full combo restarts after the ground kick finishes its recovery.
- Each kick in a held-B combo plays its kick sound once, including repeated kicks.
- Each repeated single waits for the entire first-punch recovery. Each repeated
  pair waits for the entire second-punch recovery before restarting. Within a
  pair, first-to-second timing remains the ordinary held-B timing.
- The selected mode survives animation changes and repeated cycles. Leaving or
  restarting the punch action resets the mode and its input history, including
  exits caused by grabs, damage, jumping, or falling.
- A fresh press after another release selects the mode for the punch currently
  being performed. A new sequence starts in ordinary combo mode.
- Stick input alone does not cancel the extra recovery wait of a held repeat.
  Existing jump, crouch, first-person, and environmental cancellation rules still
  apply. Releasing B also permits ordinary recovery movement cancels.
- Each repeated attack gets a fresh object/wall hit record. A repeated first
  punch has the same grab eligibility and lunge behavior as the initial one.
- If an object disappears during pickup, Mario cancels the pickup; a newly spawned
  object cannot inherit that grab. Ordinary pickup timing and fall/stomp interruptions remain.
- Breakdance knocks Bullies radially away from Mario, regardless of his facing;
  ordinary attacks retain their facing-based Bully knockback.
- A breakdance damages each eligible object once per spin. A new spin resets
  that record; ordinary body contact and the existing attack arc remain unchanged.
- Each attack phase can launch a given Bully only once, including air kicks,
  slide kicks, ground pounds, enemy-bounce twirls and underwater ground pounds.
  A dive or slide kick keeps its hit record through its landing slide; pounds
  and twirls keep theirs through landing. New strikes and distinct targets remain
  eligible, including with Metal Cap. Shell, flying and other sustained attacks,
  plus Metal body contact, re-arm each target after physical separation.
  Ground twirl keeps its existing per-spin contact rules; body collision and
  Bully recovery immunity remain unchanged.
- Breakdance uses a 24-frame animation. In player-facing frame numbers, startup
  is frames 1–2, the attack is active on frames 3–12, recovery begins on frame 13,
  and recovery cancels open on frame 14. Each target is hit once per spin.
- Breakdance blocks knockback throughout the move, including its final frames
  and special flame, shock, stomp, lava and wind launch reactions. Damage can
  begin only on frames 1 and 13; the other frames also block environmental drain.
  Existing cap protection and earned invulnerability still apply. Previously
  queued damage drains normally. On a vulnerable fire contact, the normal total
  three-wedge burn loss is queued without entering the burning launch action.
  Fresh spins begin with the frame-1 damage window even with a stale animation.

### Current tuning

Punch timing and movement values:

- Punch recovery cancels open 5 frames before the final animation frame;
  held-B follow-ups open 6 frames before it. Both require recovery frame 2 or later.
- Second-punch recovery lasts 11 animation frames (two fewer than its original
  13 frames); first-punch recovery timing is unchanged.
- Ground-kick recovery starts at frame 8; the cancel constant remains 10 frames
  before the final frame (the existing automatic kick recovery check uses 11).
- First-punch thrust scale: `1.5f` (weak); second-punch scale: `2.25f`
  (medium); ground-kick scale: `3.0f` (strong). The second-punch thrust begins
  when its hit becomes active, and the kick thrust begins on its first frame.
- Lunge eligibility starts from speed magnitude below `10.0f`. A repeated
  first punch can gain a lunge after Mario has slowed below that threshold;
  an already active lunge remains active through repeats.
- Standing and moving punches use a `0.7f` ground deceleration coefficient
  (1.4 speed per frame on ordinary flat ground); the ground kick uses `0.6f`.
  Standing and sliding breakdance both use `0.10f` (0.2 speed per frame on
  ordinary flat ground). The same relative drag applies when slowing backward
  travel. A sliding breakdance with at least 1 speed turns its travel toward the
  stick by at most `0x100` yaw units per update, scaled by stick strength; neutral
  stick and stationary breakdances do not steer. An early recovery cancel from a
  sliding breakdance into running keeps Mario's current facing so walking turns
  him toward the stick; other recovery choices keep their normal input priority.
- Standing and moving first punches, second punches and ground kicks can aim
  toward the stick by at most `0x80` yaw units per update (about 0.7 degrees),
  scaled by stick strength. Neutral input does not turn them. Signed speed,
  lunge strength, attack windows and repeat/cancel timing remain unchanged.
  Returning to walking preserves the attack's facing and turns gradually even
  below 16 speed, until Mario reaches the held direction or leaves walking.

### Code ownership

- `src/game/mario_punch.c`: input gesture, sequence mode, recovery,
  follow-up transitions, lunge, and shared first-punch classification.
- `src/game/mario_actions_object.c` and `mario_actions_moving.c`: standing and
  moving punch action steps.
- `src/game/mario.c`: reset the sequence when entering, leaving, or restarting a
  punch action through `set_mario_action`. Animation follow-ups change
  `actionArg` within that action and preserve the mode.
- `src/game/interaction.c`: use `mario_is_first_punch` for grab eligibility;
  retain the existing wall, room, and object restrictions.

## Movement and jump controls

- Controller axes keep raw -6 through +6 neutral and map raw +/-7 to +/-1.
  Inputs of magnitude 8 or greater retain their established response, including
  the 64-unit circular limit.
- Normal locked-framerate builds use `VIDEO_LOW_LATENCY=1`. Each graphics task
  records its framebuffer; after RDP FullSync completion, the scheduler requests
  that completed frame for the next VI retrace. This removes the deliberate
  older-frame presentation queue without adding a graphics wait to the game
  loop. Triple-buffer rotation, two-retrace pacing, input sampling and audio
  scheduling remain unchanged. Reset suppresses new frame publication and draws
  reset bars on the latest completed buffer. The existing instant-input emulator
  whitelist and unlocked-framerate builds keep their legacy presentation path;
  `VIDEO_LOW_LATENCY=0` also restores that path.


- Shared binary-angle sine/cosine use Bhaskara's single-precision rational
  cosine approximation and derive sine with `sqrtf(1 - cosine * cosine)`.
  Quadrant folding preserves periodicity and cardinal values; the full 16-bit
  angle is used. Paired matrix callers share one cosine calculation. The vanilla
  sine table remains available to reference fixtures but is excluded from runtime.
  This accepted approximation has host accuracy coverage; its performance cost
  and gameplay effects have not been measured in an emulator.
- Mario's camera-relative stick direction uses a table-free, single-precision
  arctangent polynomial, rounded to the nearest 16-bit angle unit before quadrant
  reflection. Cardinal/diagonal directions and neutral-stick facing are retained;
  stick deadzone, magnitude, response curve and sampling cadence are unchanged.
  Other engine angle calculations retain their existing lookup table.
- Side flips are available immediately after entering turnaround, including
  from reverse-stick punch recovery. The existing late-animation cutoff remains.
- The PuppyPrint Walking page toggles full-stick walking between the normal 40
  target and the 127-speed trial with D-pad left or right. Debug builds start
  at 40; builds without PuppyPrint debug also use 40. Partial stick
  scales the selected target. Speed carried into walking still slows gradually
  above the target. Slow surfaces, held-object walking, and ground twirl's
  post-bounce speed limit keep their previous behavior.
  Full-stick walking settles exactly at 40 on ordinary flat or grippy sloped
  ground instead of alternating around or retaining speed above that target.
- Ordinary running targets 32 instead of 40 in shallow water more than ten units
  above Mario's feet and on sand-classified floors. Partial stick scales this
  target; carried speed slows gradually and dry stone/grass running returns to 40.
  Metal also receives this shallow-water running slowdown. Vanish and Wing Cap
  bypass it, including combined caps. Wing Cap targets 48 for ordinary ground
  walking, scaled by partial stick, and returns gradually to the applicable
  target when it expires. Sand still targets 32 with any cap.
  Existing slow-floor/quicksand rules, held-object walking, crawling, slides and
  ground twirl retain their own speeds. The fast-walk debug trial also respects
  the sand/wading target and resumes its trial target on ordinary dry ground.
- An active Wing Cap supports eligible land movement on the water, including
  when combined with Metal or Vanish. Metal and Vanish alone do not grant support.
  Water-walking requires forward speed 30 or higher, including shallow pools approached
  from above or from dry land.
  Water acts as a flat floor while sufficient standing headroom remains; shore
  transitions and cap expiry restore the real floor. Flying and Wing flight
  takeoff, forward dives,
  butt slides (including held-object slides), slide kicks and their active
  belly/slide recoveries bypass water support.
  Both rollouts bypass water support only for their first eight gameplay updates;
  their ninth update can water-walk when the other requirements are met. This
  rollout delay does not refresh the separate five-update cooldown.
  Entering a ground twirl from an already supported water floor lets Mario keep
  that support through the held T-pose at any speed while Wing Cap is active,
  L stays held and the stick stays neutral. Dry-land T-poses do not gain this
  support. The existing pose timeout remains; leaving the pose or losing Wing
  Cap restores the ordinary water-walking requirements.
  Braking, deceleration and slide-stop actions may water-walk while they meet the
  speed and re-entry requirements. Falling below 30 forward speed also removes
  support. Vertical velocity -60 or lower bypasses support too, including exactly
  -60. Wing Cap also prohibits support when a landing at the water height would
  cause fall damage under the usual Wing thresholds. Held-A Wing protection,
  Metal immunity, spawn grace, twirling and handstand grace remain effective;
  the restriction shares the five-update timeout. The hypothetical water floor
  uses ordinary ground friction, regardless of the submerged terrain.
  Once Mario's feet go below the surface, water walking remains disabled
  until he touches dry land or stands on shallow-water ground within 100 units
  of the surface. Shallow footing rearms support and allows stepping onto water
  at sufficient speed; surfacing over deep water and changing caps do not rearm
  it. A restriction blocks re-catching water for five gameplay updates after it
  clears; collision substeps do not consume this timeout. All support, cutoff and re-entry rules also apply to cold water.
  Ground pound passes through, allowing ordinary Wing Mario to enter deep-water
  swimming through the existing underwater pound entry. Swimming remains active until Mario leaves
  it; a swimming jump must clear the surface before landing on water again.
  Surface contact still applies Soaked, with its existing consumption/re-entry
  rules and Vanish exemption. Water footsteps and landing splash particles/sounds
  remain active even with Vanish, retaining existing Metal and Vanish sounds. Water
  walking uses ordinary ground friction, so stopping
  slides match flat land while supported. Shell-riding water friction remains native.
  Metal/Vanish underwater action permissions remain.
- With the stick held during ordinary ground walking or landing recovery,
  negative forward speed approaches zero by 6 per update, plus ordinary
  landing friction during recovery, before forward acceleration resumes.
  If Mario turns more than 90 degrees during that walking slowdown, remaining
  negative speed clears immediately before ground movement follows his new facing.
  This also applies while carrying an object or walking underwater with Metal;
  neutral input, slow surfaces, quicksand and ground twirl keep their own drag.
- Ground and air movement use four collision substeps per movement pass.
  Faster movement uses larger increments rather than more checks. Ground checks follow actual
  travel along the current slope and recalculate when the floor changes.
  Movement that a wall fully blocks stops repeating identical substeps.
  Mario first checks whether the frame's movement corridor contains wall
  geometry. When it contains no reachable walls, ground and air substeps skip
  wall searches while still consuming the normal one-shot collision flags.
  The corridor is checked even without movement to preserve wall separation.
  The check also excludes walls outside Mario's vertical movement range and
  refreshes after a ground floor change, so stepping onto a new floor can
  still find walls at its height.
  Only near wall geometry, when a step crosses a nearby wall's front plane,
  he raycasts for a crossed wall, including pushes across a neighboring wall
  at a seam. An existing corner push is not repeated after this repair. If the
  correction still crosses another wall, the unsafe horizontal push is rejected;
  vertical travel continues unless a tilted wall also blocks it.
  A hit removes inward movement while keeping movement along the wall.
  A lower-probe-only wall contact can be crossed when its matching walkable
  top is a low step with standing room. Taller walls, unsupported sides and
  low ceilings still block ground movement.
  Camera-only and vanish-cap wall rules follow Mario's collision behavior.
  Ordinary wall queries use conservative height groups on static lists with
  useful vertical separation. Terrain loading prepares ordered groups of eight;
  short or single-height lists keep the direct scan. A query uses groups only
  when they reject at least eight references and half the maximum list length.
  Moving walls and copied candidate lists keep their existing live/filtered scans.
  Pool shortage, the 16 KiB metadata cap, area changes and late static additions
  restore the ordinary path. Areas without a valid height index use the original
  cell scan, keeping grouped bookkeeping outside that path. Wall order, pushes
  and camera/Vanish rules remain;
  floor, ceiling and swept-body/ray searches retain their existing coverage.
  Horizontal Mario movement rays also skip static wall height groups outside
  their probe height, using the same ordered metadata and rejection threshold.
  Pitched rays, camera rays, moving walls and unavailable/stale metadata keep
  the ordinary scan. Per-face bounds, surface filters and equal-distance hit
  ownership remain unchanged; this allocates no additional terrain metadata.
  With opt-in MARIO_WALL_CANDIDATE_CACHE=1, movement reuses wall lists for the
  current cells when excluding
  walls outside Mario's height and movement path makes those lists shorter.
  It rebuilds after leaving the cached cells, height range, or path region.
  A wall push outside that region reruns the full query in the original wall
  order; an oversized cache also uses the ordinary lookup.
  Candidate counting and copying share one scan. Dense lists retain the native
  query; cache bounds and wall order stay unchanged.
  While truly idle on unchanged static geometry, Mario reuses his settled
  wall, floor, and ceiling result. Movement, area changes, nearby dynamic
  collision, and special collision flags restore the normal searches.
  Water, gas, input, and interaction checks continue every frame.
  The mesh/BVH query experiment is diagnostic only (Collision page, Z).
  It does not select contacts or move Mario; ordinary collision remains active.
  Memory-tradeoff optimizations are disabled by default: STATIC_SURFACE_INDEX,
  DYNAMIC_SURFACE_INDEX, PERSISTENT_DYNAMIC_SURFACES, COLLISION_RESULT_CACHES,
  MARIO_WALL_CANDIDATE_CACHE and PAINTING_CACHES. Normal builds use linked
  collision scans, ordinary moving-surface loading, uncached floor/ceiling queries,
  and the original painting ripple calculation and unbatched vertex renderer.
  Their retained opt-in implementations are described below. Enemy sleeping and
  accepted audio microcode optimizations remain enabled.
  Tick Tock Clock spinners retain the original 34-triangle collision mesh,
  including the raised center ridge and vanilla surface properties. Their loading
  range below Mario uses the ordinary collision radius plus 200 units and his
  vertical speed, instead of the extra 2,000-unit allowance. Distant spinners stop
  supplying floors/shadows until he approaches. Rotation, rendering and existing
  rider requirements continue.
  Persistent moving-surface storage retains up to two large meshes
  and publishes their ordered faces into the existing dynamic grid. It preserves
  native vertex rounding, planes, metadata, room eligibility, publication order,
  time stop and object lifetime; unsupported or oversized publications fall back
  to the ordinary loader without partial grid updates. It does not change queries
  or movement. General persistence remains opt-in with PERSISTENT_DYNAMIC_SURFACES=1.
  Normal builds independently enable DORRIE_STATIONARY_SURFACES: Dorrie attempts
  reuse only when his mesh, owner, full transform and scale match the previous
  eligible publication. Moving, bobbing, scaling and head-mesh changes use ordinary
  loading. Object and area teardown retire eligibility; room/range/time-stop checks,
  scripted head lift, platform bookkeeping and drawing updates retain their order.
  Unsupported, full-cache and duplicate publications use the ordinary fallback.
  This reserves about 33 KiB and targets longer stationary waits; isolated native
  loader savings do not establish whole-frame or console performance.
  With opt-in COLLISION_RESULT_CACHES=1, ordinary repeated floor queries share
  a small cache of identical static-list
  searches within the same frame. Position, dynamic floor height, grate eligibility,
  list identity and static terrain generation must match. Moving floors still
  update on every query; special collision flags use the ordinary search.
  The eight-entry cache mixes higher coordinate bits to avoid repeated eviction
  at aligned nearby positions, without increasing its storage.
  Profiler floor counts include enemy support queries and cached requests.
  With opt-in STATIC_SURFACE_INDEX=1, static terrain references retain their
  original order in a packed per-cell index. Normal builds use linked scans.
  The indexed precise floor/ceiling searches, Mario movement rays and swimming
  body-edge searches can skip groups outside conservative bounds. Exact contacts,
  geometry and movement budgets retain their existing rules. Dynamic surfaces
  use linked traversal in normal builds. The experimental dynamic index is
  disabled after a fresh-boot performance regression; it remains opt-in for
  investigation. Presence checks bypass construction. Wall-push searches retain
  their exact scans. Low memory or late static additions restore linked static
  traversal.
  Water height is queried per substep only while riding a shell. Existing early
  stops still apply; swimming keeps its own step.
  Water lookups skip cells with no possible water. Shallow-water splash bursts
  share their initial surface lookup; moving droplets still check their own positions.
- Falling below an edge cannot snap Mario upward onto it through the floor-query
  height buffer and repeat a fall/landing cycle (edge grinding). Airborne movement
  keeps real downward plane crossings, including slopes and narrow fence tops,
  while ignoring floors approached from below. Missing-floor and ceiling fallbacks
  cannot use a remembered floor above Mario's starting feet as a landing.
  Airborne movement catches floors crossed between substep endpoints, including
  high-speed dives over the Physics Lab's uneven strip. An earlier ridge contact
  takes priority over a lower endpoint floor. Ordinary landings and coplanar seams
  retain their existing horizontal travel; floors approached from below stay ignored.
  A later platform-side push cannot erase an earlier valid top landing. Walls
  are checked again at the top contact, retaining intervening side obstruction.
  Nearby wall-radius pushes cannot throw that contact underneath a slope;
  a clear approach retains the first top contact. Flat-floor wall separation
  and real intervening wall crossings keep their ordinary response.
  A grounded takeoff uses its accepted support plane as the air-query origin,
  accounting for integer floor-query rounding on slopes. This does not change
  Mario's position or launch velocity. A stale floor outside its footprint or a
  remembered floor above Mario's feet cannot grant that correction.
  Diving off a slope crest retains a floor already below the destination feet,
  even when its extended plane is above the takeoff point. This avoids a false
  missing-floor bonk while retaining below-plane rejection for actual landings.
  A feet-level crossing of a real low riser can land on its walkable top within
  the existing 55-unit step allowance, including stair jumps and short gaps
  between platforms. The top must match the crossed side, extend at least
  50 units beyond it and provide standing clearance. Narrow fence tops,
  unsupported sides and floors reached from underneath without that crossing
  remain excluded. Published moving floors can catch downward movement when
  their plane rises at Mario's footprint, including gradual platform tilt;
  Collision history survives the temporary rendering-matrix pointer being cleared,
  so ascending TTC spinners can catch descending Mario after ordinary rendering.
  Stale poses, scale changes and discontinuous motion do not grant support.
  Gradual tilt allows one unit for integer collision-vertex rounding when
  reconstructing the previous floor plane. Pure translation keeps its exact rise.
- Mario and moving solids use the previous native collision path. The general
  moving-solid sweep, transform history, shared displacement guards and explicit
  crush solver were reverted after severe performance regressions. The Tox Box
  / shared moving-solid repair is postponed. Retain the independent stale-squish
  input and actual-floor-support fixes, including Metal damage immunity.
- Low poses cannot snap upward onto a floor steeper than 73 degrees. With
  valid floor contact, they can move level or downhill along those faces,
  including belly slides on JRB rocks and transitions between rock triangles.
  Real drops still use the normal fall-off behavior; wall and ceiling checks
  continue to constrain movement.
- With the stick neutral, ordinary airborne landings keep their remaining
  horizontal speed and decelerate after the landing window instead of snapping
  to a stationary landing. This also applies while holding a light object.
- Ordinary jumps and freefalls spend the unused movement substeps of a landing
  frame on the ground, including while holding a light object. This preserves
  horizontal travel through touchdown without advancing landing recovery or
  processing input twice. Walls and floor changes still constrain that travel;
  fall damage and burial are checked at first contact. Special air actions keep
  their existing collision/bounce handling.
- Ordinary moving and stopped landings accept A, B and Z during recovery,
  including long/triple jumps and light-object landings. A preserves jump chains;
  A+Z chooses a strong-stick long jump or a backflip. A buffered landing jump
  uses those same choices while Z is held, even if A was released before touchdown.
  B attacks or throws a held
  object; Z crouches/slides and drops a held object. Environmental exits take
  priority. Damage and scripted recoveries retain their separate rules.
- Braking stop, butt-slide stop and slide-kick stop accept ordinary landing
  controls immediately: jump/chords, attack, crouch, movement and C-Up. Cancels
  preserve incoming momentum; Z+B follows the usual slide-kick/breakdance route.
  Stomp and floor-loss exits retain priority; neutral recovery animations keep
  their existing completion behavior.
- Belly/dive slides stand up automatically at their existing low-speed thresholds
  only when the stick is not held forward relative to Mario. Forward pressure
  keeps ordinary slide friction without adding speed; releasing it allows the
  usual recovery. Held-object belly slides follow the same rule.
- With the stick neutral, held Z during butt-slide, slide-kick or belly/dive
  stop enters crouching even with remaining slide speed. Crouching retains its
  ordinary coasting and allows an immediate A backflip during entry. Held-stick
  crouch-slide behavior and ordinary stop-state jump choices remain unchanged.
- A ground jump consumes the Z press that initiated its chord, so that press
  cannot immediately ground pound. Held Z and a new airborne Z press still work.
- Allowed cannon angles defaults to Current, allowing full 360-degree yaw.
  Original clamps yaw to 90 degrees either side of the cannon's base direction.
  Both preserve aiming speed, the 0–80 degree pitch range, and A-to-fire control.
- Simultaneous fresh ground A+B reduces an immediate belly dive's unobstructed
  apex by 10%, retaining 90% of its rise and accounting for discrete gravity.
  This gentler setting follows the user's height correction. Its horizontal boost is
  unchanged; later airborne dives and water-jump dives retain their height.
- Forward and backward rollouts leave the roll immediately on ground contact.
  There is no timed landing recovery or queued-dive wait. Current A/B/Z inputs
  choose the next ground action on that contact update; stick alone enters
  walking, neutral stick with remaining speed enters deceleration,
  and a stopped neutral landing enters idle. The contact update does not run a
  second movement step. A/B that initiated the rollout cannot trigger a second
  action if that same update also lands.
- Rollout landings preserve their signed horizontal momentum. Fresh B uses
  ordinary moving attack controls: strong forward/sideways stick dives; backward
  or weaker stick punches,
  and Z+B selects the established slide kick or breakdance. A backward rollout's
  immediate dive reverses its remaining carry before the normal dive boost.
  A and A+Z retain ordinary landing jump choices and priority. L twirl, L+Z dab,
  and spin gestures use the normal ground states after touchdown.
  Backward deceleration animates at the magnitude of its ground speed.
  A neutral stick after either rollout loses 1.5 speed units per coast update,
  or a 2.5 braking coefficient at higher forward speed, even if the stick was
  initially held on touchdown and released later.
- Both turnaround phases accept B attacks, Z crouch slides, and Z+B crouch
  attacks immediately. Environmental exits and the existing A side-flip/jump
  priority remain first. While the side-flip window is open, held Z+A instead
  backflips when Mario holds no object and stick deflection is below 85%;
  stronger stick retains the ordinary side-flip choice. Successful cancels preserve facing and momentum without
  applying the turn animation's later visual correction.
- A dive chosen at rollout touchdown accepts fresh A/B on its first airborne
  update as a rollout request. The normal six-update queue still expires, and
  an ordinary dive's initiating B does not count as a follow-up. Grounded belly
  slides accept eligible A/B rollouts from their first update; the stand-up
  recovery accepts ordinary landing jump, attack, crouch, and movement cancels.
  On slide slopes, belly-slide rollouts require actual forward, downhill travel
  and downhill-facing launch direction. Backward, uphill, cross-slope and stopped
  slides cannot roll out. The same rule covers dive-slide queues and held-object
  belly slides; an eligible held rollout drops the object normally. Flat-ground
  rollout direction and held-object pickup handling retain their existing rules.
- The first three airborne dive updates ignore ordinary wall-bonk reactions;
  the fourth restores them. Wall separation and tangential travel remain active,
  and inward horizontal travel is removed on protected contact. Breakable walls,
  lava walls, grabs, enemy damage and the six-update rollout queue keep their rules.
- Forward rollouts from dives and slide kicks cannot open a wall-kick window
  during their first three updates. A qualifying wall hit in those updates
  causes a heavy air bonk. Valid wall contact can open the
  wall-kick window from the fourth update onward; the usual speed and wall
  requirements still apply.
- A forward dive can weakly hit each Huge Goomba once, including Huge Floombas.
  It can launch each Bully variant once, trigger Wiggler's hit feedback once,
  and remove at most one Pokey part. Wiggler's head/body and Pokey's parts share
  their respective enemy's hit record. The landing belly slide shares the dive's
  record; a fresh dive can hit again. Suppressed repeat hits still allow ordinary
  body damage and separation, and a real descending stomp retains its usual result.
- A fresh A or B press after slide-kick startup queues a rollout for six gameplay
  updates, including the press update. It is consumed when Mario settles into
  slide-kick sliding; the startup press cannot queue it.
- A slide-kick forward rollout remembers a fresh strong-stick Z+B chord for
  six rollout updates. If it lands in that window, the next slide kick starts
  at touchdown. The press that started the rollout is excluded;
  dive rollouts and expired chords do not chain a slide kick.
- A kick on takeoff or within the existing three-update A-input grace window
  retains 75% of the single/held/double/triple or steep jump's upward speed,
  with the existing 20-unit minimum. Later airborne kicks and repeated kicks retain their lift.
  Steep takeoff keeps its existing horizontal adjustment and facing.
- Airborne movement cannot enter a ramp through its triangular side below the
  normal wall probes. Rejected top support checks the feet crossing against the
  real side, preserving sideways travel, valid low steps and Vanish-cap grates.
- A failed airborne floor lookup blocks horizontal travel without a phantom
  wall bonk. Ordinary jumps retain ascent, held-object ownership and A-release
  height control, matching jump kicks. Actual wall/ceiling contacts and the
  shared floor-step allowance retain their existing collision behavior.
- Side flips block jump kicks for their first airborne update. A kick pressed
  during that update is buffered and starts on the second update, even if B is
  released or the stick changes; fresh B presses can also kick from that update.
  Dives and ground pounds remain available immediately and can cancel a buffered
  kick. Landing or any other action exit clears it, and each side flip starts a
  fresh one-update restriction.
- A jump kick that touches supported ground shows the ordinary freefall
  landing pose on that contact update. A following A press may still start
  the normal double jump with its existing timing and launch.
- Fresh A during a single, double, triple, side-flip, long jump (including backwards),
  or wall kick queues the next grounded jump for six updates: contact or up to
  five updates before contact, even if A is released. The takeoff press does not
  count; slope, fall, damage, and other action exits cancel the request.
- A successful wall kick from a double jump preserves triple-jump eligibility
  through repeated wall kicks and the normal landing follow-up window. The next
  ordinary jump uses the existing triple-jump speed and Wing-cap rules. Missed
  wall-kick windows, interrupted actions and expired landing windows end the chain;
  Z+A retains its ordinary backflip/long-jump priority.
- A forward rollout from a dive or slide kick opens the ordinary wall-kick
  window on a valid wall hit. Low-speed contacts or missing wall references
  retain their existing stop response.
- Releasing the stick at ground speed uses a `2.0f` braking coefficient,
  including during neutral-stick landing recovery. Floor slipperiness still
  scales the resulting slowdown. Low-speed coasting slows by `1.0f` speed per
  frame, including ordinary landing recovery and while holding a light object.
- During the skid after releasing the stick, Z enters a crouch slide and Z+B
  starts the usual crouch attack, preserving the remaining ground speed.
- Ground slides cap their horizontal slide speed at 100, including butt slides
  on steep courses such as Peach's Secret Slide.
- After its initial bounce, a slide kick flows into a butt slide on a qualifying
  slide slope. Fresh A/B presses and queued rollout inputs take priority.
  The transition keeps slide velocity and facing, subject to the existing
  fast-backward butt-slide entry rule; flat ground retains slide-kick recovery.
- Starting a long jump from walking or a crouch slide requires at least 85%
  stick deflection, using the forward-dive threshold. There is no minimum
  forward-speed requirement. The same-frame A+Z shortcut still works.
- Standing crouch entry skips the first two lowering frames, then plays one
  pose per update. Crouch slides keep the complete eleven-pose lowering animation,
  before held stick starts the crouch-to-crawl animation. A crouch slide that
  stops early, including a guarded
  ledge stop, finishes that same lowering animation without restarting it;
  slides whose entry has completed settle directly into crouching. Crouch entry
  can immediately reverse when Z is released or take its usual jump/attack exits.
  Uncrouching accepts a new
  crouch or punch early; every normal standing input is available from animation
  frame 3 (player-facing frame 4), redispatching through
  idle or deceleration in the same update. With no input, its usual completion
  timing is unchanged. Re-pressing Z during uncrouching returns directly to the
  lowering animation, bypassing idle's delayed Z input. Releasing Z cancels an
  unconsumed delayed crouch request, so short repeated taps cannot start a
  stand-up without first lowering.
- Releasing Z during a crouch slide starts the uncrouch animation immediately,
  preserving remaining momentum whether the stick is neutral, forward or backward.
  Fresh jump/attack inputs, first-person exits and automatic slope slides retain
  their priority. Releasing Z also cancels a pending startup backflip queue.
- A with backward stick input during a crouch slide backflips after one completed
  slide frame, on the second update or later, with no 85% deflection requirement.
  Backward+A on the first update buffers the backflip while A remains held,
  even if the stick changes afterward. Releasing A cancels the buffer; a fresh
  A request for another move replaces it. Releasing Z during startup retains
  the immediate uncrouch exit; Z may be held or released on the backflip launch
  update. Action changes clear the buffer. Standing crouch entry and neutral
  crouch slides with Z held retain their immediate A backflip, including while
  residual slide momentum remains. Other A inputs
  start an ordinary jump. The first 30 updates retain the long jump for strong
  forward input; afterward it starts the ordinary jump. Automatic slope slides
  keep their normal entry; crouch sliding has no manual butt-slide input.
- A crouch slide with neutral stick stops at the last supported part of a ledge
  instead of falling. Holding the stick back while still sliding toward a valid
  deep ledge grabs it through the normal climb-down action before the slide can
  settle into crouching or crawling. The grab follows slide travel even if Mario
  faces backward; if the edge has no usable wall or enough drop, Mario stops
  there. Forward input keeps the normal slide-off fall.
- Backwards long jumps are enabled, bounded by the existing global speed cap.
  An established backwards long-jump chain continues with A+Z while any stick
  deflection is held; releasing the stick restores the ordinary backflip choice.
  After that chain enters crouching, the first five crouching updates retain the
  A+Z backwards long jump with nonzero stick, delaying crawl entry for that window.
  Neutral stick cancels this eligibility permanently for that crouch; interrupted
  chains and later crouching updates retain ordinary controls.
- Shared airborne control applies 1.5 units of extra forward drag per update above
  speed 64, in addition to each action's base drag. At or below 64, existing
  action thresholds and momentum scaling apply. Wing twirl control boosts
  continue to preserve momentum without passive drag.
- Ordinary airborne movement uses vanilla backward drag through 20 backward
  speed, including its extra 2 units below -16. The extra drag rises linearly
  from 2 to 6 between backward speeds 20 and 32, then remains 6 beyond 32.
  Backward stick input eases that extra drag, allowing speed past -16. From
  backward speed 20, held-stick gain starts at 0.5 per frame and falls toward
  zero at 32; held input cannot accelerate Mario past -32. Releasing the stick
  restores the stated deceleration curve. Faster carried backward momentum
  still slows gradually instead of being cut directly to -32.
  Slide kicks retain their separate drag behavior, and jump kicks retain their
  momentum scaling.
- Airborne backward speed clears to zero when Mario moves into a wall, even
  when that contact does not trigger a bonk and the stick is held backward.
  A valid delayed backward-jump wall kick retains its incoming speed until the
  kick response handles it; free-air and sideways wall travel retain theirs.
- A jump launched while a butt slide is moving backward gets two extra units
  of horizontal air drag per update while its forward velocity stays negative.
  The penalty applies after air steering, so holding backward cannot restore
  backward speed after it reaches zero. It is not weakened by jump-kick momentum
  retention. Its jump kicks retain that penalty, including repeated kicks; a dive does not.
  When launched from a slope, a backward butt-slide jump (including held and
  steep-jump variants) cannot gain uphill horizontal distance over that slope.
  Backwards jump kicks instead lose up to 20 units of uphill horizontal speed
  on actual slope landing contact, regardless of launch action. This preserves
  residual travel and never reverses the uphill component through zero.
  Flying above a slope does not trigger this braking, even with a carried
  restriction marker. Sideways/downhill momentum and forward uphill kicks
  remain available. Follow-up kicks retain eligibility for contact braking;
  kicks from other launch actions do not gain the butt-slide air-drag penalty.
  The first butt-slide entry on a slope with backward speed faster than 16
  sets speed to zero. The second qualifying entry in the same slide/jump chain
  sets forward speed to 10. The third and later qualifying entries flip the
  incoming negative speed positive, with a minimum of 20 (-16.01 becomes +20;
  -24 becomes +24).
  This includes held-object slides. Flat-ground contact or an unrelated action
  resets the chain; ordinary slide motion does not retrigger the response.
  Jump kicks use the speed at impact, before the 20-unit contact brake,
  carried through freefall
  landing only. Entering another action discards that saved impact speed.
  Exactly -16, slower backward speed, forward speed, and flat-ground entries
  keep their carry. An ongoing butt slide can still gain speed normally.
  Backward butt-slide jumps require three completed slide updates on the first
  jump in a chain, six on the second, and nine on the third and later jumps,
  including held-object slides on flat or sloped ground. Forward butt-slide
  jumps retain the six-update flat and nine-update slope delays. Buffered A
  input lasts six updates, even after release; expiry cancels it.
  Forward butt-slide jumps and other jumps keep their ordinary air drag.
  A forward dive out of an unheld butt-slide jump waits for one completed
  airborne update. A dive pressed during that update is buffered until the next
  update, even if B or the stick is then released. Ordinary jump kicks keep
  their timing; landing or another action discards the dive buffer.
- Releasing A during a single, double, or triple jump shortens the early ascent
  by reducing upward speed 15 units per frame instead of abruptly quartering it.
  The same release response applies to held-object jumps and immediate jump
  kicks from those jumps.
- After a double-jump landing, a normal triple jump accepts forward speed above
  10 or backward speed below -10. Backward triple jumps keep their backward
  travel and use the backflip animation; the special spin uses its backward
  animation. Wing-cap flying triple jumps keep their separate behavior.
- Backward triple jumps, including the special spin, can open the normal
  wall-kick window from their fourteenth airborne update onward while still moving
  backward. The 60-degree approach cone follows backward travel, and the kick
  faces away from the contacted wall.
- Crouch backflips use the same thirteen-update delay and wall-kick window.
  They qualify while still traveling backward, including their normal
  16-speed launch after air drag; backward double jumps keep their existing behavior.
- Falling death-return warps use the native 75-unit/frame downward limit, so
  returning outside a Bowser arena does not inherit the faster gameplay fall.
- Standard Mario's normal maximum downward speed is 100 units per frame;
  HE THICC's is 127, with either model appearance. Actions with
  their own slower falling limits retain those limits. Below -70 downward
  velocity, air drag reduces further gravitational acceleration by 3.5% for
  each unit beyond 70, capped at the existing 22-unit reduction range so both
  characters can reach their falling-speed limits.
- Belly-slide and dive-slide automatic stand-up retain their existing speed
  thresholds and animation timing, but carry remaining motion into recovery.
  Neutral stand-up coasts with ordinary ground deceleration; if the animation
  ends while Mario is moving, ordinary deceleration finishes the stop. Walls
  still stop him and walking off support still enters freefall.
- Backward slide kicks use speed magnitude to decide when they have stopped.
  Dive-grab recovery enters held deceleration for remaining speed in either
  direction, including fractional speed.
- Crazy Box rides use upward launch speeds of 54, 72 and 120 units per frame
  across their three jumps, 20% above the original speeds. Forward launch
  speeds and the three-jump release sequence retain their existing behavior.
- A dive grab keeps its slide momentum during the light-object stand-up animation.
  Stick input and the slide's automatic facing correction cannot turn Mario
  during that action or its same-frame exit, including with a crazy box.
  Once the object is held, fresh A or B, held Z, or stick deflection can cancel
  the stand-up starting ten animation frames before its normal end. Earlier
  A/B presses are buffered for six updates, including the press update, and may
  be released before that window opens. The usual fall,
  slope, and damage exits keep priority.
- Pressing Z from idle or walking begins crouching after one held frame. A fresh
  A+Z long-jump input while walking and Z+A backflip input still take priority.
- A held Z can buffer a backflip on either backflip landing only if A was
  pressed within the last six frames. Releasing A is allowed; that press is consumed;
  holding A from an earlier backflip does not repeat it indefinitely.
- Pressing B while crouching or crouch sliding slide kicks at 85% or greater
  stick deflection. During crouch slides, holding the stick back prevents a
  slide kick; B breakdances whenever the slide kick is ineligible. The Z+B chord
  while moving follows the same rule, regardless of current speed. Stationary
  crouch attacks retain their stick-strength rule.
- The slide-kick exit animation plays at twice normal speed. Entry into freefall
  retains the original 40-update timeout and greater-than-500-unit floor clearance.
- A low-speed slide kick eases its visual pitch toward flat as forward speed
  approaches zero. At 40 speed or above it keeps the usual tilt; launch,
  movement, and stick-based activation are unchanged.
- Wall contact blends from the current pose over three gameplay frames. Leaving
  contact blends into the next animation and model facing over three frames,
  including kicks, bonks and interrupted transitions. Pausing freezes the blend.
  A kick after the wall pose has fully settled completes its visual yaw turn
  after one gameplay update, keeping the three-frame limb/pitch/roll blend.
  Early kicks keep their usual turn; HE THICC's seated kick uses the same rule.
  The wall-slide pose takes priority on the contact update over backflip and
  backward-triple-jump poses; a same-update kick selects the slide-jump pose.
  A side flip, backflip, ordinary triple jump, or backward special triple jump
  reaching the wall straightens Mario's body immediately while the limbs and
  facing still use the three-frame visual blend.
  The six-update wall-kick window, launch, collision, animation clocks and
  gameplay facing remain unchanged; respawning clears the visual transition.
- Running on slopes limits the model's lean to 60 degrees in either direction,
  including at carried or debug-trial speeds. Normal lean below that limit,
  acceleration, momentum and slope traction retain their existing behavior.
- Riding a bouncing box keeps its dive pose upright when Mario steers backward.
  Its visual pitch uses horizontal speed magnitude; backward travel, the three
  bounce heights, landing progression and wall/lava reactions remain unchanged.
- Normal forward airborne wall-kick entry requires speed strictly greater than
  10, including buffered wall-over-ledge priority and unlocked water jumps.
  Exactly 10, zero and unmarked negative speed cannot open that window. Backward
  jump eligibility and the forward-rollout delay/speed gate keep their own rules.
- The wall-kick slide window requires a stick projection of at least 85% of
  full strength toward Mario's facing on contact or in any of the preceding
  three gameplay updates. Backflips and backward triple jumps use the same
  three-update history opposite his facing. Input pressed only after contact
  cannot create that missed window.
- The wall-kick preparation pose appears only during a live kick window with
  wall contact. Expiry, edge release, landing, ledge/ceiling grabs and lava
  reactions clear it in the same update.
- Moving beyond the side of a wall during a wall-kick slide ends in ordinary
  freefall, without the missed-kick bonk. The expired window still bonks if
  Mario remains in contact with the wall. Recovery after that update's air step
  starts its movement on the next update, including edge release and landing.
- Cannon wall or out-of-bounds bonks retain their recovery action and recoil;
  Wing Cap flight and cannon speed maintenance cannot overwrite that response.
- Non-damaging wall bonks retain reflected impact momentum through air and ground
  recovery. Soft air bonks keep an eight-unit recoil minimum; heavy air bonks keep
  sixteen. Ground slide bonks retain their existing stop threshold. Faster impacts
  rebound at half their incoming speed, subject to those minimums, with normal
  ground friction through recovery.
  A missed wall-kick window uses the speed at contact; kick timing and launch stay
  unchanged. Recovery landing and floor loss carry this momentum, while damage knockback or
  another action ends it. Ordinary damage, Bully and twirl rebounds keep their rules.
- Ground bonks and momentum-preserving wall-bonk recovery return control one
  animation frame earlier while Mario is alive. Recoil/friction, floor-loss handling,
  ordinary damage/death recovery and the existing quick twirl rebound remain unchanged.
- A fresh airborne A press can start a wall kick on valid wall contact in the
  next update. An intervening action change clears it; the ordinary wall-kick
  window still accepts a fresh A press after contact.
- Wall kicks use the vanilla bonk sound once at wall contact, with the metallic
  variant under the Metal Cap. Accepting a kick or missing its window adds no
  second impact sound. Further wall-kick contacts in the same airborne attempt
  stay silent until an actual new kick; landing rearms the first contact.
  Mario's ordinary jump voice remains unchanged.
- Wall-kick contact, air-twirl entry and air-twirl wall impacts share a six-update
  sound gap. Impacts and boosts still happen; sounds inside the gap are skipped,
  never delayed. Ordinary damage and other bonks keep their sound behavior.
- A new Mario bonk stops any earlier normal or Metal bonk from his sound source
  before its own impact plays. Separate shell and water-exit bonks remain
  audible once per contact without stacking their sound effects.
- Holding A at a slide-kick bounce on a water surface, including shallow water,
  gives 60% of the incoming downward speed upward instead of 50%. The same
  bonus applies to a lava-floor bounce; dry-floor bounces keep their usual height.
- An uphill slide-kick contact uses only incoming downward speed for its
  rebound; horizontal approach and slope steepness cannot add upward launch
  speed. Contact during ascent or with zero incoming vertical speed settles
  into slide-kick sliding, even if post-contact gravity makes that speed negative.
  Horizontal speed, the global speed limit, and flat/downhill bounce strengths
  stay unchanged. A rising floor may consume the rebound immediately. Each
  airborne kick still bounces only once.
- Ground, lava and shallow-water slide-kick bounces use the unused movement time
  in the contact frame for the rebound, preserving horizontal travel and starting
  upward travel immediately. This continuation checks collisions without repeating
  gravity, wind, action/input timers or bounce effects.
- Slide kicks ignore ordinary wall-bonk reactions during their first three
  gameplay updates; the fourth update restores the usual recoil. Walls still
  block movement, box-breaking contacts remain active, and lava walls retain
  their reactions. Bounce continuations share the startup window and do not
  reset it when their action timer restarts.
- Holding A while landing on an enemy adds 12 upward-speed units to the normal
  bounce. Releasing A leaves the ordinary bounce height unchanged.
- A held-A ground-pound rebound from a twirl-bounce enemy enters the enemy's
  airborne twirl instead of a normal jump. It keeps the rebound's incoming-speed
  height, Z-input cleanup, and ground-pound cooldown. Holding or releasing A
  afterward keeps the twirl's usual spin controls.
- Air-twirl landings from enemies or tornadoes accept ordinary jump, attack,
  crouch, and movement cancels from animation frame 3. Cancelling commits the
  visible spin facing and clears the remaining spin; falls and stomps retain
  priority at every frame. With no cancel input, the landing finishes normally.
- During frames 0 through 3 of the crawl-start animation, A with at least 85%
  stick deflection cancels into a long jump. Slide kick is unavailable while
  crawling or entering or leaving a crawl; B breakdances instead.
- Crawling begins at 1.5 speed on level ground with a full stick and approaches
  a target of 9 without alternating around it. Its animation follows actual
  crawl speed, with a minimum of 30% of its full-speed rate, and takes about
  17 frames per cycle at full speed.
- Walking with a light object targets speed 20 at full stick; walking with a
  heavy object targets speed 8. Partial stick scales each target normally.
- A moving breakdance flows into a crouch slide that retains its remaining
  momentum until it has nearly stopped, then enters the stationary crouch.
- Early punch/breakdance recovery uses the remaining speed when choosing its
  B/Z exit, even with neutral stick: moving punch or crouch slide at speed 1 or
  greater in either direction; ordinary stationary actions after stopping.
- Crawling and related low poses cannot acquire nearly vertical floor faces
  (normal Y below cos 73 degrees). A crawl already on such a tilted support
  releases into freefall instead of climbing its side.
- Releasing the stick while carrying a heavy object or crawling coasts to rest
  with ordinary ground deceleration. The heavy object stays held until a normal
  throw/drop exit. Crawl recovery keeps its animation timing and carries any
  remaining speed through crouch entry, crouching, crawl restart and standing
  recovery; standing completion enters deceleration if Mario is still moving.
- A breakdance started while crawling or during crawl entry or exit returns
  directly to crawling if Z is still held when the spin ends, at any nonzero
  stick deflection. It skips the crouch action. A breakdance started elsewhere
  resumes walking rather than crouching with Z and at least 85% stick deflection.
- Re-pressing B during a crawl-origin breakdance's recovery starts a fresh
  breakdance once, with its animation and hit records reset; repeated B presses
  can cycle through crawl and breakdance without an action loop.
- Breakdancing adds no punch lunge. At idle it stays in place; a moving
  breakdance can retain momentum already present when it begins.
- Starting a breakdance with a neutral stick and ground speed magnitude below 20
  removes all active power-up caps only while L+Z are held, ending their timer
  and music. This applies to standing
  and crouched starts; becoming slow or neutral later in the spin does nothing.
  A cosmetic cap pops straight upward and fades away after 30 frames, with no
  pickup interaction. The regular cap and its ownership are never removed.
  Vanish Mario can also shed his power-up underwater; losing Vanish immediately
  returns him to swimming, without repeating land/water transitions or ground movement.
- Active standing and moving breakdance attacks hit in a full circle around Mario,
  preserving their existing contact reach and once-per-target rule.
- A breakdance triggers the wall hit effect once per wall during each spin.
- Ground twirl halves Bully knockback; small Bully twirl contacts are further
  capped at 16 speed so the regular knockback action cannot erase that reduction.
  The twirl rebound uses its faster soft-knockback animation and, after it ends,
  restores eight hidden recovery frames. Contact with another eligible enemy
  during the rebound also grants the usual 30 flashing frames.
- Soft, normal and hard front/back knockback, airborne/grounded/underwater
  knockback, bonks and thrown recoveries buffer A/B for six updates.
  Released A/B presses remain valid until expiry. Buffered A has priority over B. Death and unrelated action exits discard
  requests; underwater recovery uses breaststroke or underwater punch.
- Mario has two frames after Bully knockback recovery in which another Bully
  cannot knock him back. Bully body separation and Mario's attacks still work.
- Ground twirl and displaced-enemy defensive contacts respect the usual Vanish
  damage immunity. Collectibles, solids, grabbing and eating keep their own rules.
- Capless twirl contact damage applies only when Mario takes knockback, once
  per twirl. Stunning an enemy without a cap gives Mario a light 16-speed soft
  recoil and one wedge of damage; all enemies reached in that update still get
  their stun. Already displaced enemies and contacts without knockback do not
  charge damage. Capped and Vanish Mario retain their usual protection, and
  ordinary invincibility and rebound grace prevent repeated damage/recoil.
  Hazard rebounds follow the same one-wedge capless rule, including
  all rolling-ball variants and JRB sliding boxes, even with zero base damage.
  Bowser attacks retain their native damage and cap removal instead of using
  this defensive rebound.
- Active ground twirls contact huge Goombas (including huge Floombas) at their
  smaller body/hurtbox radius, plus Mario's radius, instead of their outer stomp
  hitbox. Piranha Plants use a 100-unit body radius rather than their wider
  sleeping or biting hitbox. Other enemies with broader interaction hitboxes
  use their smaller hurtbox radius plus Mario's radius for twirl contact. All
  three Big Boo boss variants and Wiggler's head use this same body limit for
  both ground and air twirls, retaining their native reactions within it.
  Ordinary non-twirl collisions and boss hitboxes are unchanged. Their
  push reach is capped at the original hitbox radius.
- Ground twirl reaches mobile enemies vertically through Mario's full 160-unit
  collision height. Mario can launch and stun a Snufit he touches from below
  within that height; targets entirely above or below the body remain out of
  reach. Existing horizontal body reach, wall checks and Vanish protection apply.
- Debug free movement restores Mario's render-active state and clears explicit
  invisibility every update, including entry from a cannon; ordinary cannon
  aiming retains its hidden Mario model.
  Leaving a cannon through debug flight releases its ownership and camera event
  and lets the cannon reset for reuse; ordinary aiming and firing stay unchanged.
- Debug free movement's L heal restores full health and clears queued damage.
- Debug free movement clears scripted camera effects every camera update,
  including death cutscenes, forced field of view, roll and shake, without
  restarting area entrance cutscenes.
- Ground twirl lasts 12 updates, with an immediate 5-update hit window.
  Ground/air twirl and Wing roll wall impacts spawn three stars instead of seven.
  The final 5 updates (from frame 8) allow jump, attack, crouch or walking cancels. Its visible
  spin lasts 8 updates with a 2-update acceleration ramp. Ground cooldown is
  30 updates after exit, independent of the air twirl's cooldown.
  Wall bounces refresh the 5-update hit window and 12-update steering recovery;
  the wall spin lasts 5 updates and attack cancels use the final 7 recovery
  updates. The early-bounce bonus window lasts 3 updates; its speed bonus ramps
  over 2 updates and fades over 6. Enemy stun and held-pose timers retain their
  separate durations.
- The early first wall bounce of a ground twirl has a 10.0 base speed bonus,
  scaled by 1.15; the regular wall-bounce boost also uses a 1.15 multiplier.
  Full-stick ground twirls accelerate toward 40 speed, including after wall
  bounces. On ordinary flat ground, a fading bounce bonus or repeat-bounce
  drag cannot cancel that acceleration below 40. An early bounce cannot cap
  subsequent acceleration below 40. Ground rebounds and their fading bonuses
  can reach 64 but never exceed it; later bounces may raise an early rebound
  ceiling up to 64 after the repeat-bounce penalty.
- Ground twirls, air twirls and Wing flight rolls accept wall contacts through
  the full 360 degrees. Contacts that previously qualified keep their reflection;
  newly eligible side/rear contacts boost forward along Mario's current facing.
  Contact must still come from collision detection; lava walls retain their hazard
  handling. A lingering newly eligible ground contact and every lingering air
  contact award only one boost until separation or a different wall normal.
  Existing boost strengths, speed ceilings and repeat-bounce penalties remain.
- Ground-twirl and air-twirl wall rebounds, including Wing flight rolls, emit the
  normal bonk stars and a short 5/40 rumble pulse on each accepted bounce.
  Lingering contacts suppressed by the bounce rules do not repeat the feedback.
  Successful twirl enemy hits give a 5/80 rumble pulse, including harmless Boo
  reactions and Wing twirl launches or follow-up defeats. Ignored contacts and
  overlaps during the enemy contact grace do not rumble; passive Metal contact
  keeps its existing feedback.
- Ground-twirl wall rebounds lose 6 percentage points of speed retention per
  repeat bounce in the sequence (100%, 94%, 88%, and so on), reaching zero on bounce 18.
  The penalty applies to the whole rebound, including its speed bonus, and
  resets when Mario leaves the ground twirl. Existing repeat-bounce drag remains.
- An eligible L press starts a ground twirl before a running wall hit or an
  automatic run-to-slide transition. The twirl keeps its normal wall bounce;
  its off-floor, damage, held-object, and other startup restrictions remain.
- Holding L with neutral stick keeps the twirl pose through recovery into the
  held T-pose without briefly playing the landing animation. Releasing L restores
  the ordinary recovery animation; held-pose timers begin only after recovery.
- The held final-frame T-pose makes Mario's ordinary cap vulnerable to damage
  after five completed pose updates while L remains held and the stick is neutral.
- Passive Metal contact first launches/stuns an eligible enemy. Another running
  approach can defeat it after 15 enemy updates, once it is grounded (including
  underwater floors) or touching a wall. Landing/wall impact alone does not kill.
  Stationary or retreating overlap and free flight stay harmless; a contact
  rebound retains its recovery. The countdown pauses during enemy time stop,
  and landing stun lasts through the remaining grace period. Explicit attacks
  keep their native timing. Defeats preserve each family's coins, shell/item
  release, child cleanup and respawn rules, including Monty Mole's hiding and
  kill streak. Koopa the Quick, bosses and rooted/twirl-immune targets retain
  their existing behavior; Boos keep their native bounced-on reaction.
- Pressing A+L together on supported ground, or pressing L (held or tapped for one
  frame) then A exactly one gameplay update later, queues an air twirl for the next
  gameplay update, after the jump starts. This chord can cancel a ground twirl
  started by that L press while moving. The one-frame buffer survives releasing
  L, is consumed once, and expires if the jump is rejected or interrupted; ordinary
  cooldown, carrying, Z and damage restrictions still apply.
- A fresh L press during an ordinary unladen jump or freefall starts one 8-update
  air twirl. It can also interrupt an enemy-bounce twirl, retaining upward bounce
  velocity and the normal air-twirl cooldown and input restrictions. Holding Z
  does not block eligible airborne, wall-window or Wing twirls. A fresh Z press
  keeps its usual action priority; grounded A+L still retains its Z restriction.
  Releasing L
  finishes that cycle; holding L does not repeat it. The
  first air twirl before landing reduces current downward velocity by 70% on entry
  and falling gravity by 98% during the cycle, preserving ascent, short-hop rules,
  horizontal momentum and terminal-speed policy. When entry reduces downward
  velocity, the recorded fall height restarts at that position, including the
  Wing Cap's downward-velocity reset. Rising entries, flight rolls and entries
  that do not reduce velocity preserve fall height; a long fall afterward can
  still cause normal landing damage.
  Each genuine wall bounce reflects horizontal travel, adds 8 to speed magnitude
  (9 for the first bounce within the first 3 updates of an air twirl or Wing
  flight roll), and then multiplies it by 1.3 before the consecutive-bounce penalty
  at any point in the cycle (including Wing flight rolls), and refreshes the full
  cycle/gravity window. Persistent wall overlap cannot refresh it, and A cannot
  wall kick during the twirl. Held L takes priority over A within a valid wall-kick
  window, starting an available air twirl without a wall-kick launch. With Wing
  Cap, a fresh L press also starts a twirl from the wall-contact state or its
  immediate bonk recovery after the kick window expires, taking priority over
  fresh or buffered A. Ordinary damage knockback retains its restrictions.
  Air twirls have a separate 30-update cooldown;
  fresh air presses during cooldown or temporarily ineligible airborne actions are
  buffered for six updates, including the press update. Release does not cancel
  the request; a new press refreshes it. Entry consumes it once; landing, water
  entry, death and scripted cutscenes discard it. Collecting any coin resets
  the air cooldown, including the cooldown due when an active twirl or Wing
  flight roll finishes. Coins also replenish gravity reduction, downward-speed
  braking and the Wing upward boost/control allowances for the next ordinary
  air twirl. A pickup does not restart the current spin; the next twirl resumes
  normal cooldown rules. Lava and normal landing/fall damage take priority.
- Air-twirl rebounds lose 6 percentage points of speed retention per consecutive
  repeat bounce, applied to the whole boosted rebound: 100%, 94%, 88%, and so on,
  reaching zero on bonk 18.
  The final rebound speed magnitude is capped at 64 after the boost and penalty,
  including backward travel, Wing entry wall impacts and active flight rolls.
  Horizontal velocity follows the reduced speed; Wing flight rolls recompute
  their flight velocity from it. Ordinary air-twirl vertical velocity is unchanged.
  Persistent overlap does not add a bonk, and a fresh twirl resets the count.
- A/B/Z action presses during an L-button air twirl remain buffered for six
  gameplay updates, including the press update, through its ordinary freefall
  landing. Released buttons remain valid; landing keeps its normal action
  priorities, and unrelated exits discard requests. Holding L retains ground
  twirl continuation instead of forcing a buffered landing action.
- Holding L links an air twirl into a ground twirl on landing, and a ground twirl
  into an air twirl on ledge departure. The landing continuation survives cycle
  expiry while L stays held, but release or unrelated action interruption clears
  it. Actual landing, water entry and Mario initialization restore air allowances.
  Actual ground landing preserves the remaining air-twirl cooldown, which
  continues counting down normally. Landing action transitions do not restart
  the full cooldown. Ground twirl retains its separate cooldown.
  Held/ridden objects, cannon/shell/tornado actions, damage and cutscenes
  retain their controls; L pressed there cannot queue a twirl. Water-exit jumps
  must finish their existing 10-update lock first.
- Air twirls use ground twirl's eligible enemy stun, body reach, room and wall
  checks. Capped Mario stays airborne when stunning a target. Ineligible enemy
  body contact gives damage-free airborne recoil; capless Mario retains the
  one-wedge recoil penalty once per twirl. Wall refreshes and linked ground/air
  handoffs share that contact record. Hazards and explicit boss attacks retain
  native damage, and Vanish retains its ignored-contact precedence.
- Wing flight's automatic speed-dependent pitch change eases smoothly from
  -1024 angle units per update at speed 4 to zero at speed 16. It stays zero
  through level cruising speed 64, then smoothly joins the `(speed - 64) * 6`
  response at speed 80. Flight speed targets 64 when level, easing toward 32
  at the maximum climb pitch and 100 at the maximum dive pitch. Speed approaches
  its target by 1 per update, with an absolute maximum of 100. Slower launches
  accelerate toward their cruising target. Stick steering and pitch limits retain
  their existing rules.
- A fresh L press during active Wing flight starts a 16-update aileron roll,
  retaining normal flight movement, steering and camera. Expiry resumes the
  ordinary flight pose; walls refresh the roll, while Z, cap expiry and hazards
  retain their native exits. Only active-flight entry uses this roll pose. Held L
  at touchdown links to ground twirl; released L keeps flight's ordinary landing.
- A fresh L press during ordinary unladen swimming starts a 16-update aileron
  roll, without requiring Wing Cap. The visual roll pivots around the animated
  hips so Mario spins in place rather than orbiting his object origin.
  Twirl sound effects use a separate playback group from swimming strokes and
  flutter kicks, allowing both to play together without changing the spin sample
  or shared sound cooldown.
  Wall, floor and slope contacts during the roll use reflection and boosts: +9
  for a first hit in the first three updates, otherwise +8, then 1.3 times
  speed, capped at 64. Successive bounces retain 6% less speed per contact,
  reaching zero on contact 18. Persistent overlap does not repeat the boost.
  Impacts refresh the roll smoothly. Wall impacts preserve swimming pitch;
  floor and slope impacts reflect the incoming 3D direction away from the
  contacted plane and update swimming pitch/yaw to follow the rebound.
  Automatic floor-pitch assistance cannot override a roll rebound.
  Adjacent triangles on the same plane count as one persistent floor contact.
  Each rebound emits three impact stars. Inward contacts reflect away from the
  wall, including head-on contacts and a stopped swimmer facing into the wall.
  Swimming movement, stroke timing and steering continue normally. Holding L does not repeat it; completion or interruption
  starts a 30-update cooldown. Leaving swimming, damage, held-object actions and
  Metal land controls end the roll. Z retains its downward-burst priority.
  Early transitions into underwater land controls or shell riding clear the roll
  and buffered swimming inputs before the next action runs in that update.
- With Wing Cap, ordinary air-twirl activation immediately redirects horizontal
  momentum toward held analog input, preserving speed. Neutral-stick entry keeps
  direction; this instant turn happens only at activation, including later twirls.
  If the incoming update's path reaches an eligible wall, its boost is awarded
  before the stick redirects momentum. The boosted ceiling is retained; the same
  contact cannot boost again merely because Mario turned toward it.
- The first ordinary air twirl before landing with Wing Cap grants 4x analog
  steering for 30 updates, continuing into freefall. Stick input redirects
  existing horizontal momentum without forward acceleration or lateral speed
  gain; the horizontal speed ceiling starts at its value at activation. Genuine
  wall boosts may raise the ceiling to their post-penalty rebound speed;
  persistent contact cannot raise it again. Passive air drag is suspended during
  the control boost, so steering preserves speed; normal drag resumes afterward. Its first
  cycle reduces falling gravity by 99%; ascent is unchanged. Later twirls and
  wall refreshes do not refill the control timer. Landing, water entry and cap
  loss clear it. Active Wing flight keeps its native steering and gravity.
- The first Wing Cap ordinary air twirl before landing clears downward velocity,
  then adds 20 units of upward velocity. Rising entry keeps its upward momentum
  and adds the same boost, consuming the allowance. Flight rolls preserve momentum;
  later twirls and wall bounces cannot refill the boost. Every active Wing air twirl also grants
  Metal-style enemy protection and first-contact stun/later-contact defeat. After
  the same 15 enemy updates, an approaching follow-up can defeat a freely airborne
  stunned target. Native defeat rewards/cleanup remain; ordinary Metal keeps its
  grounded-or-wall restriction. Wing expiry immediately ends this override.
- A committed Metal follow-up knocks a Bob-omb away at 25 horizontal speed and
  30 upward speed, then uses its native landing explosion, coin and respawner.
  It restarts the fuse so an almost-expired fuse cannot explode in place.
- Ordinary surviving Metal enemy contacts use the light knockback reaction,
  including the Mad Piano, and ignore the large-knockback 40-speed override.
  Front/back and ground/air/water direction rules remain native; already lethal
  queued damage and separate scripted, capture, lava and electrical reactions
  retain their existing handling.
- Bowser body contacts, every red/blue flame variant, landing stomps, arena
  mines and expanding shockwaves bypass Wing, Metal and Vanish protection,
  including combinations, and end all active powers and their timer/music when the attack
  connects. Ground and air twirling and standing/moving breakdance cannot avoid
  these attacks, including shockwaves and ground impacts; ordinary flashing and
  rebound grace still protect Mario. Landing stomps in all three fights deal two
  wedges and heavy knockback within 1250 units, or one wedge and light knockback
  from 1250 through 9999 units. Beyond 9999 units the landing does not hit. Each expanding shockwave has
  one visible annulus from 84% to 100% of its outward radius. The damaging band
  is 5% narrower, centered inside the graphic, from 84.4% to 99.6% of its radius.
  Its last damaging update reaches 3486 units (the graphic reaches 3500). Light variants
  deal one wedge; heavy variants deal two. Damage lasts 70 updates.
  Capless damage doubles. Ledge grabs use the same damage with an airborne
  reaction. Body contact during any jump or within 200 units deals four wedges,
  heavy knockback and an upward launch. Charging contact also deals four wedges.
  Capless four-wedge hits retain the existing seven-wedge per-hit limit.
  Every heavy Bowser knockback knocks off Mario's equipped cap, including
  surviving body/jump/charge hits, heavy landing stomps and the second shockwave
  ring. All Bowser knockback cap launches, including mines and lethal light hits,
  use twice the normal horizontal cap speed (minimum 30 units/update). Damage is calculated before that cap loss; it does not double the same
  hit. Light hits retain the normal cap unless existing lethal-damage rules remove it.
  Fire/water/Soaked handling and existing invincibility handling remain in effect.
- All nine outside pillars in Fight 2 and all five bomb-support pillars in Fight 3
  have collision matching their visible faces
  for Mario only, with full grippy traction and no steep-slope restrictions, including
  landing, idle, held idle, crawling and upward low-pose movement. Bowser,
  other objects and camera queries pass through them. Fight 1 has no such pillars.
- Running into a Bowser mine detonates it and deals exactly seven wedges when
  Mario is vulnerable, including while capless. The mine bypasses cap powers
  and removes them, and knocks off the regular hat even on a surviving hit;
  ordinary invincibility still blocks damage and hat loss. Its hitbox has a
  120-unit radius and 100-unit height. The blast launches Mario upward at 50
  units/frame and toward the arena center at 140 units/frame. The explosion
  child only supplies visual and sound effects. A Mario-triggered mine hides and becomes intangible for 60
  gameplay updates, then rearms at the same position. It can explode again after
  respawning. Dormant mines cannot damage Bowser; Bowser-triggered detonations
  still consume the mine and retain their cinematic arena behavior.
- In Fight 2, Bowser's first landing after a bomb hit emits one light shockwave,
  including the final defeat landing. Its single outward ring uses one-wedge light knockback
  and retain the normal shockwave range and lifetime. Later bounces and ordinary
  throws do not emit this wave. Fight 3 instead emits two shockwaves on
  the first bomb-hit landing, including defeat: the first begins immediately with
  one-wedge light knockback; the second begins twenty updates later
  with two-wedge heavy knockback. Each variant has one outward ring, moving at
  the same speed, so the light wave stays ahead of the heavy wave. Each retains its full 70-update damaging
  lifetime; later bounces do not repeat them. Missed throws use the separate
  Fight 3 belly-landing rule below.
- Quick-jump landings emit one light shockwave in Fights 1 and 2. Fight 3
  adds a heavy shockwave twenty updates later. Recovery does not repeat them.
- Fight 3 bomb destruction collapses the matching outside section and adds a
  black spherical fire spitter at its star point. Every active encounter fire spitter
  emits one light shockwave when a bomb is destroyed, including the new spitter;
  the new point's collapse wave counts as its single wave. First landings after hits 1–4 add five Goombas,
  five Bob-ombs, five Mr. I, then five Bullies respectively. Goombas and Bob-ombs
  spawn at the star points; Mr. I spawn near the five inner corners, inset to
  80% of their distance from center. Wave-4 Bullies use 75% of the outer points'
  distance from center. All encounter wave enemies, recurring center Boos and
  fire spitters fade in at their native size over one gameplay second (30 updates
  in US/JP, 25 in EU). They remain intangible and their behavior pauses after
  initialization until the fade completes; attached model parts share the fade.
  Their encounter owner does not act as a native triplet spawner, so Bowser
  returning to idle cannot unload them. Hit 3 collapses the remaining outside
  sections and immediately spawns a Boo 200 units above the center floor.
  Each falling section emits four dust particles, alternating its two edges.
  Fight 3 hit-mine bounce dust uses one particle per burst.
  The center Boo spawns upright with one yellow coin inside. While it remains
  active, its respawn timer stays at the full wave-specific delay. After removal, another
  center Boo spawns after 1800 active updates in Wave 3, or 900 in Wave 4.
  Elapsed countdown time carries into Wave 4; a living Boo still holds the countdown. Hit 4 adds five
  Snufits halfway between adjacent Mr. I positions, 450 units above the floor,
  one Amp circling the center at radius 400 and 150 units above the floor
  (with its native vertical oscillation), and black spherical fire spitters at
  all five points. Wave 4 has no Lakitus or homing Amps. Hit 5's
  first landing removes the encounter enemies, their offspring and fire spawners.
  If Mario is holding an encounter enemy, this cleanup releases it into idle or
  freefall before deletion. Intangible dialogue and ending actions retain their
  script while releasing the held enemy. Unrelated held objects and earlier waves
  are unchanged.
  Dropped Boo reward coins survive this cleanup; coins still carried by Boos do not.
  Encounter Mr. I each drop exactly one yellow coin; center Boos release their
  one carried yellow coin. Other encounter enemies roll once for a 20% chance
  of one yellow coin; Spinies never drop coins. Bob-ombs from these waves do not respawn.
  Ground encounter enemies, Amps and Lakitu's Spinies recover to their home support only after falling
  to Y=107, 200 units below the arena floor; destroyed home support falls back to an interior point
  600 units from center. Recovered enemies face and move toward the arena center.
  Held Bob-ombs and combat defeats retain normal handling.
  Attack-hit, knocked-back, stunned or rebounding encounter enemies are deleted
  at Y=107 instead of teleporting. This protection from teleport recovery persists until they
  safely land again; enemies falling without an attack retain bottom recovery.
  Flying Lakitus retain native movement over arena edges and never use fall recovery.
  Encounter-owned Bob-ombs keep updating beyond their native 4000-unit distance
  limit, and encounter Spinies skip native 2500-unit distance despawning, so
  distance limits cannot interrupt recovery. Ordinary enemies retain those limits.
  Encounter Spinies expire after 900 active updates (30 seconds), across egg and
  walking states, without dropping coins. Other Spinies retain their native lifetime.
- Mr. I acquire nearby Mario within 700 units regardless of either facing direction.
  Active yaw tracking turns up to 22.5 degrees per update; the existing 800-unit
  disengagement range and full-turn spin-defeat rules remain unchanged.
- Walking turn speed starts at 5.625 degrees/update in Fight 1, 6 in Fight 2,
  and 6.4 in Fight 3, increasing by 0.1 degree per bomb hit in every fight.
  Straight walking uses speed 3 in Fight 1, 6 in Fight 2 and 9 in Fight 3;
  any update that turns toward Mario retains speed 3. Startup and stopping
  animation frames still control when walking begins and ends.
  Fight 3 edge recovery uses 10 updates of edge movement and 10 of stopping,
  then turns toward Mario at this normal walking speed. Earlier fights keep
  their native edge recovery. Fight 2 walking can interrupt into teleport with
  a 1-in-300 chance per update. Fight 3 rolls only while not facing Mario,
  with a 1-in-600 chance per update, selecting 90% teleport / 10% floor spit;
  Fight 1 has no random walking interrupt.
- Bowser takes three bomb hits to defeat in Bowser Fight 1, four in Fight 2,
  and five in Fight 3. Each successful Bowser-triggered mine hit removes one health.
- Fight 1 alternates attack decisions with a 5% dance, 55% walk-toward-Mario,
  40% teleport decision. When facing Mario within 1500 units, attacks are 70%
  fire breath and 30% forward quick jump; at 1500 units or farther, 70% forward
  quick jump and 30% teleport. When not facing Mario, he walks toward him.
  The inter-attack decision is the alternate visit to the default AI, selected
  by the reaction flag, including the initial decision after the introduction.
  It is not a timed interval. Attract-demo inter-attack decisions retain their
  walking-only behavior.
- Fight 2's facing-Mario decisions below 1,300 units are 40% teleport, 20% fire
  breath, 30% forward quick jump and 10% blue homing fire-spit burst (no big jump). At 1,300 units or farther,
  while strictly between 500 and 1,500 units from centre, they are 40% big jump,
  20% charge, 10% teleport, 10% quick jump and 20% blue homing fire-spit burst.
  Outside that centre band, far decisions are 60% charge, 15% big jump and 25%
  blue homing fire-spit burst (no quick jump). Not facing Mario remains walking. Alternate decisions
  between attacks are 55% teleport, 40% walk and 5% dance.
  Each Fight 2 walking-action update has a 1/300 chance to interrupt the walk
  with a fire-spit burst, averaging 300 walking updates between triggers.
- Fight 3's facing-Mario decisions below 1,000 units are 60% teleport, 5% fire
  breath, 20% quick jump, 5% blue fire spit and 10% sky fire. At 1,000 units or
  farther, they are 35% teleport, 5% quick jump, no blue fire spit, 40% charge
  and 20% big jump. Not facing Mario selects walking; alternate decisions
  between attacks also select walking.
- Bowser's sky-fire attack in Fights 1 and 2 retains 26 startup updates, with its first emission on
  update 27. Eleven projectiles emit every eight updates through update 107,
  followed by 30 recovery updates (137 total). The final projectile uses the
  native blue-flame mode; earlier projectiles retain the native splitting mode.
  Fight 3 instead has 14 startup updates, ten shots on updates 15, 21, ... 69,
  and ten recovery updates (79 total). Its nine red shots each split into six
  falling red flames, producing up to 54 red ground flames. The final blue shot
  splits into one falling blue flame, whose landing group emits 12 bouncing blue
  flames. Fights 1 and 2 retain three red children and two blue children per shot.
- Fight 3 accidental falls restart the outside-arena return jump when Bowser
  descends more than 300 units below arena height, or rests on lava/death ground.
  Failed return flights reset their windup and timer too; the intentional below-arena
  fade/windup and upward launch are preserved. Defeated Bowser does not return.
- Fight 3 timings override shared defaults without changing Fight 2: floor spit
  emits on update two after one startup update, followed by eight recovery updates
  (ten updates per shot, 30–90 per 3–9-shot burst). Breath has 15 startup updates,
  30 consecutive emission updates and three recovery updates (48 total); every
  fourth emission is a single homing blue flame, for seven blue and 23 red flames.
  Quick jump has six startup updates, 38 upward launch speed, 19 airborne movement
  updates on level ground and 15 recovery updates. Its landing emits a light
  shockwave immediately and a heavy wave twenty updates later. Charge has nine startup
  updates, begins running on update ten at 65 speed, gains 0.4 per running update,
  steers at `DEGREES(3.2f)` (approximately 3.2 degrees/update), brakes at 3.5 per update and
  recovers for 15 updates. Big jumps launch at animation frame eight with 60 upward
  speed, returning to level ground on movement update 30, and emit light, light, heavy
  shockwaves spaced twenty updates apart. The follow-up hop after returning from
  outside the arena instead emits light, heavy, heavy at the same spacing.
  Centre-return arcs retain their existing timing.
  Bomb-hit first landings retain one light shockwave then one heavy wave twenty updates
  later. Teleport has six-update fades and thirteen hidden updates. Below absolute Mario
  speed 30, destinations are 60% beneath, 30% ahead and 10% behind; at 30 or above,
  20% beneath, 50% ahead and 30% behind. Ahead/behind offsets scale with Mario's
  absolute forward speed: `max(200, speed * 16)` units, predicting his travel over
  the thirteen hidden updates and three fade updates until Bowser's body becomes damaging.
  Both sides use the same distance; Mario's sampled facing selects their direction.
- Standard breath flames transition directly into ground fire on floor contact,
  retaining their incoming heading instead of launching upward in a random direction.
  They home toward Mario at `0x20` per update (0.17578125 degrees), moving at
  5 units/update in Fight 1 and 10 in Fight 2. Fight 3 ground fire instead
  turns at 1 degree/update and moves at 15 units/update. Other red-flame launches
  retain their existing airborne behavior.
- Bowser's floor-spit burst chooses 3–9 spits once on entry, independently of
  Mario's health. In Fights 1 and 2, each cycle has three startup updates, emits a native blue
  homing flame on update four, then recovers for eleven updates. Each spit takes
  15 updates; a 3–9-shot burst lasts 45–135 updates. Bowser spins
  toward Mario after each shot at up to 22.5 degrees/update in Fights 1 and 2,
  or 10 degrees/update in Fight 3. The final recovery
  immediately selects another level-appropriate attack using Mario's current
  direction, bypassing the alternate walking/dancing decision. Each spit creates
  only one blue projectile, which uses the existing blue-flame homing rate of
  0xC0 (1.0546875 degrees/update) in the air and travels at 30 units/update airborne
  and 15 units/update grounded. Touchdown starts ground speed at 20, then it
  loses one unit per update until it reaches 15 after five updates.
  After landing, steering changes gradually: a full 180-degree reversal takes
  24 updates (0.8 seconds), at approximately 7.5 degrees/update in Fights 1 and 2.
  Fight 3 overrides these shared speeds and steering: all blue flame variants
  home at 3 degrees/update, including grounded spit. Spit retains air speed 30,
  lands at speed 30 and loses 0.5/update to steady speed 25 after ten updates.
  Fight 3 bouncing blue flames move at speed 20; Fights 1 and 2 retain speed 15.
  Blue homing flames have a 120-update timeout (four seconds) in Fight 1, 180 updates (six seconds)
  in Fight 2 and 240 updates (eight seconds) in Fight 3. Spit flames do not split;
  their timer restarts on touchdown, granting the same duration on the ground.
  Bouncing and floating blue flames use the same fight-specific timeouts. Existing fire coin-drop paths have a 10% chance in Fight 3 and 5% elsewhere to
  spawn one temporary yellow coin; fire types without coin drops remain at zero.
  Sky fire retains its native splitting behavior.
- Walking turn speed is independent of health: Fight 1 turns at `0x400`
  (5.625 degrees/update), Fight 2 at `DEGREES(6)` (approximately 6 degrees/update),
  and Fight 3 at `DEGREES(6.4f)` (approximately 6.4 degrees/update).
  Charging keeps its existing `0x200` turn rate in Fights 1 and 2.
- Bowser's charge has 14 stationary startup updates; running starts on update
  15 at 60 forward speed, gaining 0.2 speed per running update. Each new charge
  starts at 60; startup and braking do not accumulate this acceleration. Speed
  is calculated from elapsed running time so drag does not erase the increase.
  Braking reduces forward speed by 2.5 units per update, stopping from 60 in
  24 updates (0.8 seconds), then recovery lasts exactly
  35 updates including the stop animation. Fight 3 uses its separate faster timing;
  run cycles, steering, early braking and arena-edge interruption retain their rules.
- Fight 1's forward quick jump has 10 gameplay updates of startup, launching
  at 50 upward and 50 forward speed with gravity -4. It reaches a level floor
  on its 25th movement update; terrain can change that duration. Its landing
  hit still occurs on the next behavior update, followed by 15 updates of
  recovery. Start/stop animations accelerate to match the shorter timing;
  Fight 2 instead has eight startup updates and launches at 44 upward speed,
  contacting a level floor on movement update 22; forward speed and the 15-update
  recovery remain unchanged. Ordinary big jumps in Fights 1 and 2 launch at jump-animation frame
  ten; centre-return jumps still launch directly.
- Bowser teleports only while grounded and always materializes on a valid floor.
  Ground contact tolerates the native small bounce velocity (up to 5 upward);
  actual airborne movement or a jump launch cancels teleport.
  At 20 or more absolute horizontal speed, Mario has a 50% chance of Bowser
  choosing 300 units in front of his facing direction and 50% behind. Below 20,
  chances are 50% on Mario, 25% in front and 25% behind. Mario's position and
  facing are sampled when Bowser vanishes; airborne Mario still produces a
  grounded destination. Unsafe offset floors fall back to the floor beneath
  Mario; if neither is safe, the teleport is cancelled. The hidden phase lasts
  15 updates, with ten-update fade-out and fade-in. Fight 2 instead uses eight
  fade-out updates, 14 hidden updates and eight fade-in updates, with 400-unit
  offsets. Below 25 absolute Mario speed it chooses 60% beneath Mario, 30% in
  front and 10% behind; at 25 or above it chooses 60% in front and 40% behind.
  Tail grabs are blocked for the entire teleport, including both fades.
  Bowser's shadow fades from black to white as he disappears, remains white
  while hidden, and fades back to black as he reappears. The colour follows the
  teleport's eight-update fades in Fight 2, six-update fades in Fight 3 and ten-update fades in Fight 1, retaining the shadow's usual opacity and footprint.
- In Fight 3, the first belly landing after a successful bomb hit also triggers
  the ground-impact attack, alongside its light-then-heavy shockwaves.
  Returning from below onto static arena ground emits one light shockwave
  immediately and one heavy shockwave twenty updates later.
  Returning from outside the arena retains the native breaking of the supporting
  outside platform on the following hop; this also triggers the section-break
  ground impact and light shockwave.
- In Fight 3, destroying an arena section produces a ground-impact attack and
  one light shockwave, with no airborne-time requirement.
- In Fight 3, a missed bomb throw produces the normal ground-impact attack and one light
  shockwave on the first belly landing, only after more than 30 airborne updates.
  Short throws, later bounces and successful mine contacts do not trigger this effect.
- Bowser must be released at at least 25 horizontal units/frame to trigger a mine.
  Eligibility is latched before release movement and survives airborne slowing,
  through the first ground-contact check. A new grab/release resets eligibility;
  breaking free does not qualify. The nearest
  armed, tangible mine triggers at strictly less than 600 units in 3D distance. Strong
  throws still count on the update where ground contact stops their movement.
  Button throws begin flight on the release update using the normal collision
  movement step; automatic drops keep their existing timing.
- Pressing the Bowser throw input checks the normal release trajectory first.
  If it misses but the release one gameplay update earlier or later predicts an
  armed, tangible bomb hit, Mario snaps to that release angle and throws immediately.
  One-update-late inputs can snap backward; two-update misses get no assistance.
  Prediction preserves the 25-unit launch minimum, strict 600-unit 3D bomb range,
  normal spin acceleration, throw physics and bomb-hit handling. It runs only on
  throw input and does not consume bombs or record statistics. A new grab clears
  the previous release history.
- Fight 2 centre-return and grab-escape jumps land at x=-800, z=0 instead of
  the exact centre, avoiding the tilt pivot and the hat respawn at x=800.
  Their tilt and flame effects remain unchanged; Fights 1 and 3 still target centre.
- Bowser breaks free after more than 180 held updates (six seconds) in Fights 1
  and 2, or more than 150 (five seconds) in Fight 3, counting
  pickup and spinning together. Mario releases him, and Bowser jumps from that
  release position to the arena center, starting at 80 upward speed before his
  normal landing impact and recovery. Every center-return jump plays Bowser’s
  native roar once at takeoff, including post-attack returns. In Fight 2, a
  centre-return landing tilts the arena and emits eight evenly spaced red flames
  in a 300-unit ring, 200 units above the landing. These flames retain their
  native falling movement but have a 600-update airborne timeout and twice the
  ground-burning lifetime. Other red flames retain their existing lifetime.
  Fight 2's arena uses very-slippery traction during the tilt sequence and
  returns to its usual floor class when tilting finishes. The escape knocks Mario’s normal hat
  backward at 25 units/update. Each new
  grab starts a fresh timer; throws before timeout retain normal swing momentum.
  Idle holding uses the same fight-specific limit instead of the earlier auto-drop.
- Bowser's tail accepts a first punch or the first three gameplay updates of a
  forward dive, within 60 degrees of directly behind him. Mario must face within
  140 degrees of Bowser's direction; later dive updates, dive slides and walking
  contact cannot grab it. Leaving the tail
  closes the grab region. A direct overlap check during Mario's interaction pass
  accepts a ready first punch or early dive without waiting for the tail's next
  behavior update
  or a free slot in the four-object collision list. It preserves body grab reach,
  tail overlap, and the existing earlier priority of flame/damage contacts.
  The tail-overlap cylinder has a 150-unit radius,
  50-unit height and its existing vertical offset. Each release locks grabs throughout flight and standing
  up, then for 300 eligible grounded recovery updates (ten seconds) and until
  Bowser has faced Mario. Finishing an attack and its recovery clears this lock
  immediately, even if it misses. A cancelled teleport does not clear it. Successful grabs immediately restore
  Bowser's
  opacity and fade target to 255; every held update keeps him fully opaque.
  His normal held-object rendering remains responsible for drawing him while carried.
  A cap-in-hand pose cannot replace the carrying hand and hide the held object.
  Debug flight returns a hand-held cap to Mario's head on entry and clears any
  leftover hand-held pose while active, even without debug level select. It does
  not create a missing cap. Other debug play clears leftover cap-in-hand flags
  outside cutscenes and the airborne cap-putting action; those real animations
  retain their cap poses.
- After falling offstage, Bowser's return windup launches after twelve gameplay
  updates. It cannot wait indefinitely for an exact animation frame left behind
  by a faster quick jump. The existing return arc, fade-in, landing effects and
  fallback to the arena center remain in use.
  If a mine hit was interrupted by offstage recovery, the return landing advances
  its pending enemy wave exactly once. Grabbing or releasing Bowser restores his
  normal drag, including when a grab interrupts a center-return jump.
  Defeated Bowser also recovers from below the arena or a lethal landing, staying
  at zero health in his death sequence. He lands above safe center support
  (x=-800 in Fight 2) and resumes the defeat dialog and reward sequence;
  completed defeat impacts and reward/fade phases do not restart.
  Bowser's footsteps take priority over ordinary wave-enemy footsteps regardless
  of camera distance, retaining normal volume falloff, samples, timing and rumble.
  The Amp's buzz continues on its separate playback channel.
- After an attack and its recovery, continuous time without facing Mario triggers
  a grounded jump toward the arena center: 150 gameplay updates (five seconds)
  in Fights 1 and 2, or 120 updates (four seconds) in Fight 3.
  Facing Mario within the existing 45-degree cone resets this timer. A new attack,
  being grabbed or damaged, death, or introduction resets the interval. The jump
  waits for valid grounded support and uses the existing center-return arc.
- Ground and airborne cap-putting animations are vulnerable to Bowser's body,
  flames and mines; grounded cap-putting also takes queued Bowser landing hits
  and shockwaves. Both cap-putting actions use capped damage amounts from the
  start of the animation, including environmental drain, while the cap is still
  visually in hand. Interrupted cap-putting completes the normal cap-in-hand
  cleanup. Other contacts retain the animations' protection; ordinary
  damage invincibility and rebound grace still apply. Airborne cap-putting still
  avoids ground-only landing stomps and shockwaves through normal airborne rules.
- In all three Bowser fights, Mario's dropped normal cap returns to x=800, z=0
  when it falls below the arena or lands on hazardous or unsafe support.
  Recovery requires valid arena support, stops the cap's motion and updates its
  saved position. Fight 1's static main arena and Fight 2's tilting arena remain
  safe support. Intact Fight 3 outer platforms are safe cap support even beyond
  the central bounds; sections marked for collapse or already falling are unsafe.
  Caps resting on the main arena and caps in other levels retain
  their existing movement and ownership rules.
- Mario cannot pick up any cap during ground, air or water knockback, even
  after the normal cap-loss pickup delay expires.
- Blue Bowser fire homes horizontally at up to 0xC0 angle units/update in
  Fights 1 and 2, or 3 degrees/update in Fight 3.
  Moving red breath, sky and falling flames slowly home at 0x20 angle units/update;
  settled red ground-burning flames also home at 0x20 and move at 5 units/update
  in Fight 1, or 10 units/update in Fight 2. Fight 3 ground-burning red flames
  turn at 1 degree/update and move at 15 units/update; airborne red flames keep 0x20 steering.
  Bowser flame hitboxes have base radius 12 and height 40, multiplied by sprite scale.
  Falling red and blue flames have damaging airborne hitboxes. Red flames use
  randomized gravity between -0.8 and -1.2, with falling-speed limits scaled by
  gravity so they land at different times. Blue flames use fixed -1.0 gravity. Falling flames cannot expire just because the
  floor below is lava or a death plane; they persist until landing or a 300-update
  airborne timeout. Ground burning and the sky projectile's split into falling
  flames retain their native stages. Blue bouncing fire also persists near Bowser.
  Ordinary breath retains vanilla animation timing and emits 20 projectiles,
  two updates apart. Fight 2 fire breath has 28 startup updates, emits on update
  29 and every two updates through 67, then recovers for five updates, ending on
  update 72. Other fights retain native timing. Sky fire is one continuous attack with its firing pose slowed
  to one-eighth speed: ten red projectiles and one final blue projectile emerge
  eight updates apart. Its windup and recovery retain their native speed.
  Each blue landing group in Fights 1 and 2 emits 24 bouncing flames in eight
  three-flame rings, two updates apart. Fight 3 emits 12 in four three-flame rings
  on emitter updates 0, 2, 4 and 6. All preserve the three radial directions.
  Repeated animation frames cannot emit duplicate projectiles; interrupted attacks
  stop emitting, and a fresh attack starts a fresh schedule.
  Stationary ground-burning flames keep a fresh floor check and native vertical
  collision handling each update, skipping the redundant horizontal movement and
  floor search. Airborne, moving, water, room-constrained and special-query states
  retain the full movement path.

- Metal Mario ignores explosion contact: no damage, knockback, dropped held
  object, or change to his action, momentum or invincibility countdown. This
  applies to the shared explosion used by Bob-ombs and water bombs; Bowser
  attacks bypass this protection. Bowser mines deal their hit separately and
  their explosion child supplies effects only. Other object interactions stay native.
- Knocked-around stunned enemies sweep their actual travel against finite walls,
  floors and ceilings before endpoint correction. Gravity, drag and water state
  advance once per update; only their own grate flag bypasses Vanish surfaces.
  Resting stunned enemies also resume native water-death handling when submerged,
  including when water rises; enemies that normally survive water retain their stun.
  Existing contact permits travel along or away from the surface, so wall
  contact cannot suspend falling and keep the enemy in its launch state forever.
  Finite vertical-edge distance uses an exact radial witness only when a
  largest-radius body disk reaches that edge within its height bounds. Cones,
  tips, slanted edges and existing overlap retain the general distance solver.
- Vanish Cap surfaces let Vanish Mario pass through their floors and ceilings
  as well as their walls, including the slanted faces of the test sculptures.
  Swimming crossing checks use the same rule. Camera collision is unchanged;
  other objects require their own move-through-grate flag.
- Dire Dire Docks' underwater star cage allows Mario to swim out through its
  side walls without the Vanish Cap. Entering through those walls still requires
  the Vanish Cap; the roof and visible cage remain unchanged.
- The Physics Lab cold pool uses the existing snow-course water drain only
  inside that pool. Other lab water stays ordinary; Metal is vulnerable to
  this drain. Vanish protection, surface breathing rules, drain timing and
  debug-selector protection are preserved.
- Clams remain damaging until opening animation frame 8, then become safe.
  Closing becomes damaging at animation frame 25, matching the snap cue; a fully closed
  shell stays damaging. Pose gating also applies on initialization and offscreen,
  and skipped frames cannot miss the change. The existing two-wedge base damage,
  hitbox shape, sounds, bubbles, shake and wait timing are preserved.
- Stunned Monty Moles rest at their normal exposed height: their origin remains
  50 units above the real floor, with the existing animation and render offset.
  Launches and contact rebounds retain their motion. If support disappears or
  moves downward, they fall and retain the remaining grounded stun time without
  refreshing it on landing. Native burial resumes after stun; attacks and
  water/lava/death handling retain priority, along with hole and held-rock cleanup.
- Stunned Snufits rest with their origin 50 units above the floor so their centered
  body clears the ground. Losing support preserves the remaining stun time.
- Snufit resumes its orbit at its displaced position after stun, with its orbit
  phase and shooting state retained. Native attack/hazard handling stays separate.
- Snufit's hovering orbit respects walls using its existing wall radius. Wall
  corrections shift the orbit's center so later updates cannot continue through
  the wall, including edge pushes without an interactive wall reference; bobbing,
  aim, shooting and stun recovery retain their timing.
- While Mario is immune to projectile damage, supported projectiles touching him
  are destroyed through their normal impact behavior. This covers ordinary damage
  invincibility, protected actions, twirl rebound and its eight hidden recovery
  frames, Metal and Vanish. Supported projectiles are Snufit bullets, Mr. I shots,
  snowballs, thrown Monty Mole rocks, flying books, water bombs and Bullet Bills.
  Further projectiles do not extend immunity. Clearing ends when protection expires;
  inactive, intangible, room-separated and unlaunched projectiles stay unaffected.
- Destroyed projectiles use their normal impact particles,
  including Snufit mist, Mr. I fragments, snowball flakes, rock chips, and
  flying-book death mist. They also play their normal impact sounds where
  present; water bombs keep their usual splash sound and explosion.
- Punches and kicks rebound from walls with the forward speed they had at
  contact, with at least 16 backward speed. Breakdance keeps its speed instead.
- Each airborne jump kick rebounds, plays its wall hit sound, and emits its hit
  particles at most once per wall. A follow-up jump kick starts a fresh hit record.
  Rebuilding dynamic collision preserves the provider's hit record; unloading it
  clears the record before its object slot can be reused.
- Airborne jump kicks have an active hitbox on animation frames 0–6, one fewer
  frame at the end than before. The full animation, movement and recovery/follow-up
  timing retain their separate rules below. Ground kicks keep their existing active window.
- Punches and kicks still hit enemies, but those contacts can also damage and
  knock Mario back. Standing and moving breakdances keep their action and speed
  when hit; damaging contacts can still cost health and start the ordinary
  post-hit invincibility period. Breakdance hits do not recoil from walls or enemies.
- Throws carry up to 12 units of incoming ground speed and slow by 1.5 per
  frame. Light throwable objects add 50% of Mario's forward speed to their normal
  launch speed, sampled when a ground throw starts or when another throw
  releases the object. That combined forward launch speed is increased by 10.25%
  in total, combining the two requested 5% increases;
  upward launch speed stays unchanged. Heavy objects keep their normal release behavior.
  Once an object has left Mario's hand, light and heavy throws accept the
  usual idle action cancels during their final seven animation frames.
  Cancels include jump, punch, crouch, movement, slide, and first-person input.
- A thrown object emits one native large splash with droplets on its first above-water to
  submerged transition, placed at the water surface. The shared behavior update
  covers actor-specific and native movement paths. Regrabbing, dropping or a new
  throw resets its entry tracking, and dry landing ends it. Throws begun underwater do not splash until
  the object emerges and re-enters. Particle-budget failures do not queue a
  delayed splash, and objects ignoring water boxes retain that behavior.
- Air throws keep their signed momentum and remaining movement when landing.
  Mario coasts through the remaining throw animation and accepts normal landing
  controls from animation frame 6 (player-facing frame 7), after object release.
  Early touchdown preserves the fourth-update release clock; stepping off again
  continues that clock. Animation completion carries remaining speed into ordinary
  deceleration instead of stopping Mario. Fall-damage and burial reactions retain priority.
- Collecting any cap while truly capless clears downward speed and backward
  speed relative to Mario's facing. Upward and forward momentum remain, as does
  momentum when he already has a cap. If his feet are at most 16 units above a
  valid floor, the capless pickup places him on that floor and ends vertical
  motion; eligible airborne pickups continue in the grounded cap animation.
  A capless airborne pickup uses 1.0 horizontal air drag and ignores steering
  and action buttons until its put-on animation ends.
- A saved regular cap already present in the current area stays in the world
  through same-area warps without spawning a second copy. Fresh level loads
  restore one cap, and collecting it clears its saved ownership.
- Caps resting at the water surface do not spawn repeated surface waves.
  Their water-entry sound and underwater bubbles still work.
- Ukiki and Klepto can steal Mario's regular hat while Wing, Metal, Vanish or
  combined powers are active. Theft ends all active powers and their shared
  timer/music, while retaining the usual saved cap ownership and recovery rules.
- Wind can blow off Vanish Mario's regular hat and end his powers. Wing and
  Metal still protect against wind cap loss when combined with Vanish.
- After Mario's regular cap is knocked off, stolen, or lost to shallow-water
  death, he cannot recover it for 30 gameplay updates. This also blocks
  power-up cap pickups that would restore the lost cap; Ukiki waits to hand it
  back if its dialog finishes early.
  Ordinary cap pickups when Mario has not just lost his cap are unchanged.
- The three-circle running easter egg blows off Mario's ordinary cap when he
  actually falls backward after releasing the stick. An aborted spin or a fall
  blocked by uneven ground leaves the cap on. The gesture is disabled in all
  three Bowser fights; running circles cannot topple Mario or remove his cap there.
- Landing with Mario's feet more than 10 units below the water surface causes
  no fall damage, including in water too shallow to trigger swimming.
  A landing that would otherwise cause light fall damage requests a small
  droplet splash and light rumble (3/40). A landing that would cause heavy damage
  requests the large water splash and strong rumble (5/80), stops the falling
  ordinary Mario scream and clears its warning flag. The rare SimpleFlips
  replacement keeps playing to completion. These impacts retain the safe landing
  action, health, cap and held object. Ordinary small jumps and falls already
  protected by Metal, spawn protection, twirling or handstand grace do not gain
  this feedback. Wing thresholds and slippery-floor light-fall protection apply.
- Each eligible heavy-fall in-air warning has a 1/8192 chance to replace Mario's
  scream with the supplied SimpleFlips scream. It plays once to completion,
  independently of normal SFX/voice stops, landing, pause exits, warps and other
  streamed quotes. After a successful start, it cannot be selected again until
  game reset or a different save file is loaded. Same-file reloads, deaths,
  level changes and character changes retain the lockout; demos never roll.
- Holding A with active Wing power prevents all fall damage, including both
  forward and backward landing reactions and ground pounds. Physical held A
  controls protection even when the action masks A input. Releasing A or losing
  Wing power before landing restores the ordinary fall-damage rules.
- Metal Cap prevents fall damage while active. A dry landing that would cause
  light or heavy fall damage emits ground-pound particles,
  Metal impact sound, rumble, screen shake and ground-pound interactions (including
  HE THICC's shockwave), while retaining ordinary landing movement and animation
  without ground-pound stalling. Wet and otherwise protected falls do not trigger it.
  Heavy falls also retain their damage-free light-fall squash, recovering in
  half the usual time.
- Active power-up caps blink during the final five seconds (150 gameplay updates),
  with the existing warning pattern stretched from slow to fast. Blinking changes
  appearance only; powers last until expiry, and cap music still fades at 60 updates.
  Wing Cap warning dim frames also render Mario's whole body at 25% opacity
  (64/255), instead of only hiding his wings; darker fades stay darker.
  Both opaque and translucent wing silhouettes reject transparent texture pixels
  after alpha dithering during warning transitions, so feather masks never become
  rectangular backgrounds.
  Each warning blink change spends one gameplay update halfway between the ordinary
  and powered appearances. Wing/cap/Metal geometry blends together, while Vanish
  opacity interpolates. Warp/camera fades and damage-blink darkness still apply;
  animation timing, held-object drawing and mirror views retain their usual behavior.
- Damage invincibility repeats three normal frames followed by two dim frames. The dim phase renders Mario at approximately 25% opacity (64/255). Normal blink phases retain their
  usual appearance; darker teleport fades stay darker, and Vanish/Metal materials,
  mirror rendering, deliberately disappeared states and protection timing remain
  intact. This applies to both palettes and both Mario appearances.
  Mario and mirror Mario suppress their through-wall silhouettes on these dim
  damage-blink frames, including extra copies from translucent Vanish/warp parts.
  Bright frames restore their usual silhouettes immediately. Reduced flashing
  keeps the body steadily dim and suppresses these copies during the same
  invincibility interval. Held objects and cap-warning blinks retain their existing silhouettes.
- Timed object visibility blinks use 20% opacity (51/255) during their formerly
  hidden phase. This includes expiring coins, dropped caps and shells, disappearing
  1-ups, temporary boxes/platforms, and released cork boxes. Blink cadence,
  lifetimes, interaction/collision flags and actual expiry stay unchanged. Hidden
  spawn states and distance hiding still hide completely. Dim phases retain
  texture cutouts and also dim their shadows and collectible silhouettes.
  Consecutive dim draws restore the layer state at the end of the run; every
  model still receives its own blend setup, so material changes cannot leak.
  The HUD star counter's celebration flash also dims to 20%; separate HUD fades
  retain their own opacity, and later HUD text keeps its normal appearance.
- An uninterrupted running-fire episode deals exactly three health wedges,
  whether Mario runs, jumps repeatedly or falls. Damage follows the existing
  burn timer, removing whole wedges at timer values 54, 107 and 160, and stops
  after its three-wedge budget is spent, even while airborne.
  Coin, water and Soaked extinguishing stop the unpaid remainder. Jumping and
  landing preserve the episode's progress; later eligible ignition starts anew.
  Automatic health regeneration stays reset throughout an active burn, including
  between damage intervals and after its damage budget is exhausted.
  Fire motion, animation and duration rules stay unchanged. Lava contacts keep
  their separate damage rules.
- Mario's attached burning flame emits one smoke particle every five updates,
  subject to the existing ambient particle budget. Smoke lifetime is unchanged.
- A coin that extinguishes Mario's grounded burning lets an A press from
  the last six updates start one ordinary jump from grounded recovery, even
  after release. Only the remaining part of that six-update window is retained.
  Expiry or leaving recovery cancels it; airborne extinguishing does not create a jump.
- Metal Mario can stand, walk, slide and land on lava floors as ordinary solid
  ground, without damage or a lava boost. Ground-pounding onto lava deliberately
  starts the normal 84-speed lava bounce without damage. This bounce stays active
  under Metal. Holding A on a return to lava repeats the 84-speed bounce, burn
  animation, voice and rumble without health damage. Releasing A lets the next
  lava contact settle like solid ground; dry ground still settles even with A
  held. Holding A while airborne or simply standing on lava does not start a
  new bounce. Cap expiry restores the floor hazard.
- Metal Mario ignores tornado capture, strong wind gusts, horizontal wind pushes
  and vertical updrafts, including the wind catch above death barriers. Metal
  pickup ends an existing tornado/wind capture or lava-floor boost. Burning-wall
  bounces retain their existing handling. Wind over a death barrier remains a
  real drop after the separate entry grace expires.
- Entering a vertical wind column grants 30 gameplay updates (one second) of
  death-plane protection, provided Mario remains inside the column. The timer
  starts on entry, does not refresh during continuous membership or action/cap
  changes, and ends immediately on leaving. Leaving and reentering earns a new
  second. Entry below the death boundary also earns this grace, down to the
  actual wind floor. Protection ends at the column's upper boundary (Y = 500),
  outside its footprint, or below its actual floor. Respawn and warp initialization
  clear the episode. All caps and ground pounds receive this death-only grace;
  Metal's wind-force immunity and deliberate ground-pound wind bypass remain.
- Lava never grants Soaked. The volcano's animated lava region is visual-only,
  excluded from water and poison-gas contact.
- Water contact makes Mario Soaked. The status lasts through 150 dry gameplay
  updates after he leaves water. Vanish Mario cannot be Soaked. While Soaked
  and out of water, regular droplets spawn every 10 gameplay updates, with
  half-size droplets halfway between them. They appear about 80 units above
  his feet and roughly 60 units to either side of his camera-facing
  silhouette. Regular droplets use 80% of the previous droplet size. Each
  droplet inherits 80% of Mario's horizontal movement after spawning and falls
  under its own gravity when he jumps. Their modest ground splashes start
  visibly translucent and fade 20% more slowly over eight animation updates;
  ordinary water splashes keep their six-update lifetime.
- A direct hit from a falling Bob-omb Battlefield water bomb also makes Mario
  Soaked for 150 dry gameplay updates. A twirl parry or blocked hit does not
  apply it, and Vanish Mario cannot gain it.
- Wind exposure clears Soaked: gusts, tornadoes, grounded horizontal wind and
  vertical updrafts within their active height range. Metal's force immunity
  does not prevent this drying. Explosive blast contacts and Bowser mine
  detonations also clear it, even when a cap or invulnerability blocks damage.
  Water-bomb splashes retain their wetting behavior; visual-only explosions
  do not dry Mario. Drying stops new drips and preserves the water-contact latch,
  so continuous water does not immediately restore Soaked. Leaving and touching
  water again can restore it. Movement, damage and prior flame-contact protection
  keep their existing rules.
- Metal and Vanish caps do not protect against electric shocks. Every damaging
  shock while Mario touches water deals double damage, including repeated
  shocks in the same water and shocks with Vanish active. Away from water,
  Soaked doubles the next damaging shock and is consumed. A damaging shock
  immediately removes every active Wing, Metal and Vanish power, ending the
  shared cap timer and music while leaving Mario's regular cap on. Blocked or
  shocks that apply no damage neither consume Soaked nor remove a power.
  Positive-base shocks deal Metal Mario three wedges dry and six while Soaked
  or touching water; zero-base shocks use the ordinary capless minimum.
  Shock uses Mario's ordinary appearance after power removal. If Mario is
  above water when shocked, he stays airborne instead of snapping to the
  submerged floor; a shock that carries him into water uses water recovery.
- Soaked does not prevent freezing-water bounce damage on snow-course burning
  floors, including the pool beneath Chilly. These contacts retain Soaked;
  initial hits and repeated landings keep their normal capped/capless damage
  and Metal protection. Swimming cold-water drain is unchanged.
- Soaked prevents damage from the fire or lava contact that consumes it,
  including the continuing burn from that contact. Continuous flame overlap
  stays protected after Soaked is consumed, including successive flamethrower
  particles. Leaving all flames ends that protection; a later dry encounter
  burns normally. Each later lava landing or
  burning-wall contact is a new hit and damages Mario normally, even within the
  same bounce sequence. Becoming Soaked during an active fire burn extinguishes
  it. Lava transport and bounce behavior still apply. Vanish Mario cannot use
  this protection because he cannot be Soaked.
- Vanish Mario takes the ordinary fire reaction on dry ground. Metal still
  prevents ignition when both powers are active; submerged flames still go out,
  and ordinary invulnerability still blocks the contact.
- While Wing power is active, holding A prevents feet-, head- and butt-first
  burial in snow, sand and cloud terrain. This uses the physical held button at
  landing, including actions that mask A input. Releasing A or losing Wing power
  restores ordinary burial and fall-damage eligibility; quicksand still applies.
- Vanish Mario cannot be buried by landing in ordinary sand. Quicksand still
  sinks, slows, carries and kills him normally; snow retains its burial rules.
- Metal and Vanish Mario cannot skip off water during a slide kick, including
  shallow water and held A. Ordinary Mario retains the normal/held-A water skip;
  Metal's slide kick uses the ordinary solid-floor bounce on lava.
- Slide-kick bounces use bright yellow-orange droplets and small splashes on lava,
  and icy-blue droplets and splashes on cold water. Cold water includes snow
  terrain and the Physics Lab's cold pool. Ordinary water keeps its existing white splash.
  The coloured burst occurs only on the actual bounce, including a water skip
  that coincides with a shallow-water floor landing; rising travel and final
  grounded recovery do not replay it. Bounce heights, input timing, cap rules,
  landing sounds and HE THICC feedback are unchanged.
- These liquid droplets travel in world space with ordinary droplet launch
  speeds and gravity. Lava droplets are twice the ordinary size, with 50% larger
  landing splashes so the burst stands out against lava. They land on the real
  floor without requiring water; cold-water droplets return to the water surface.
  Their small landing splashes retain the liquid colour and ordinary splash lifetime. Particle
  pressure reduces or suppresses the burst without affecting Mario's movement.
  Ordinary water particles and Soaked drips retain their appearance and fade.
- Ground pounds retain 80% of forward horizontal speed or 92% of backward
  speed per airborne update, with the existing sideways stick drift.
  Vertical descent and landing behavior are unchanged.
- Ground-pound startup checks airborne collision before lifting Mario for the
  spin, so a falling Whomp can contact him instead of being skipped.
- Heave-Ho throws Mario at 54 backward speed and 114 upward speed, 20% above
  its previous launch speeds.
- After landing from a real Heave-Ho throw, Mario gets five interaction updates
  of Heave-Ho grab immunity after his knockback and other invulnerability end.
  A dropped Mario gains no launch immunity. A fatal fall from that throw knocks
  off Mario's regular cap.
- Unsupported squish recovery enters freefall. Quicksand accumulation and its
  visual depth require floor contact; airborne Mario does not sink into a floor
  below him. The actual quicksand-death animation retains its sinking behavior.
- Respawning restores Mario's render activity, including a same-area return
  after crush death. Spawn actions still control their own temporary hiding.
- The playground's Chill Bully uses ordinary floor hazards rather than the
  fixed Snowman's Land death height; other levels keep their existing behavior.

## Enemy sleeping in ordinary levels

- Audited enemies may pause scripts, action timers and animation beyond 2,500
  horizontal units or 1,000 units above/below Mario. Both current and home
  positions must be outside the awake volume. Enemies with larger audited safety
  bounds keep those horizontal bounds: Chain Chomp, Heave-Ho, small Whomps,
  Thwomps, Grindels, Spindels, Tox Boxes, Sushi and large boulders use 3,500 units.
  Exact boundary values stay awake.
- Only audited native idle/resting states qualify. Initialization, transitions,
  combat, lit fuses, held/thrown objects, races, reward/death handling, moving
  supports and time stop retain their updates. Unknown behaviors, ordinary
  platforms, scenery and loose collectibles do not enroll. Triplet Goombas wake
  for their spawner's unload/deletion transition; attachments follow their owner.
- Distant patrol pausing also covers Chuckya walking home, Heave-Ho's resting and
  walking cycle, non-cap Ukiki running/turning or idling away from home, and Chain
  Chomp's tethered turning hops. Their local patrol phase and position freeze until
  they wake. Chomp lunges, chain unloading/releases, grabbing/throwing, cap-monkey
  resets, cage rewards, Swoop attack flights and homing Amp attacks/resets remain live.
  These broader states are host-checked; their runtime savings remain unmeasured.
- Sleeping enemies disappear, wake immediately on entering the protected volume,
  and regain native visibility on their first behavior update. Camera direction
  and walls do not decide sleep. Collision-bearing enemies still publish collision
  before their sleeping models are hidden. Physics Lab keeps its separate policy.
- Tethered Chain Chomp appears within 3,500 horizontal units and stays visible
  and awake through 4,700 units once revealed, so his own tethered hops cannot
  immediately hide him again. His chain shares that visibility history. Vertical
  entry/exit limits remain 1,000/1,150 units; native unloading and released-Chomp
  behavior remain live.
- Awake/Sleep counts include all updated objects, not just enemies. Reduced calls
  are workload evidence; target performance requires matched runtime measurements.

- Small Whomp walking/turning and Chuckya turning/braking sleep in normal builds,
  retaining the existing distance, contact, support and lifecycle guards. Grounded
  Skeeter walking remains an opt-in experiment (`ENEMY_PATROL_SLEEP_TRIAL=2`),
  disabled by default. Boss Whomps,
  Whomp attacks/recovery/death and Chuckya pursuit/grabbing stay awake.
  Native grounded patrols may retain floor-response velocity up to 5 units while
  still on the floor; airborne, bouncing, water and support transitions remain
  excluded. Three matched ares v148 pairs measured about 0.35 ms less main CPU
  time for two active WF Whomps, 0.058 ms for one additional WDW Skeeter walker,
  and 0.55 ms for WDW Chuckya/anchor below-platform sleeping. Nearby Skeeter
  warm-up cost about 0.03 ms more. FPS stayed near 30. Scripted wake/animation
  checks passed; these are emulator results, not console or controller-feel proof.

- Skeeter water-skating sleeping is an opt-in experiment
  (`SKEETER_WATER_SLEEP_TRIAL=1`), disabled by default. Distant steady surface
  skating and water idle may pause; changed water height, vertical movement,
  water entry/exit, wall contact and ordinary interaction guards keep native updates
  live. Ground walking stays unchanged. Existing waves finish their own lifetime.
  Three matched ares v148 pairs found no useful saving in native WDW water:
  ambient height changes prevent additional skating sleep. Native skating,
  wake-up and changed-water traces matched the disabled control. This trial
  remains disabled and parked.

- Fire Spitter physics sleeping is an opt-in experiment (`FIRE_SPITTER_SLEEP_TRIAL=1`),
  disabled by default. Mature, stationary idle actors above static ground may skip floor,
  wall and movement work beyond 2,500 horizontal or 1,000 vertical units. Native
  action logic, attack cooldowns, scale, presentation and object clocks continue.
  Attacks, transitions, contact, movement, dynamic support and rising water restore
  normal physics. Matched ares v148 runs show a WDW saving but HMC regressions;
  this trial remains disabled and parked.

## Idle supplies and ambient actors

- Settled unheld small/metal boxes, unused permanent ground shells, idle Bob-omb
  Buddies and cosmetic fish can sleep beyond 2,500 horizontal or 1,000 vertical
  units in every level, including the Lab. Exact boundaries stay awake;
  collision-bearing supplies retain a larger bound when their collision distance
  plus 600 requires it. Supplies retain native distance/room visibility while
  their scripts and animation clocks pause.
- Initialization, interactions, carrying, movement and moving support keep the
  relevant actors live. Released-small-box timers, box-supplied shell expiry and
  NPC dialogue/rewards keep updating. Sleeping solids still publish collision
  and wake for support. Cannons/barrels, ordinary and exclamation boxes, underwater
  shells, Toads, Hoot and butterflies retain native updates in every level;
  their added sleep checks were removed because their savings are uncertain.
- Fish spawners keep updating. Their fish may sleep only after initialization with
  a live, stable idle spawner; spawner respawn/deletion and missing water restore
  native updates. Both blue-school spawner aliases use the same rule. A nearby
  actor resumes on that update; no saved timer catch-up is applied.
- This extends the existing sleep policy. Host lifecycle checks cover eligibility,
  wake cycles, timer exclusions and collision publication; emulator behavior and
  performance for these additions remain unverified.

## Enemy drawing distance

- Distance-deactivated enemies keep their native drawing/activation boundary,
  with no extra drawing margin that could show suspended movement. Enemies that
  explicitly update from afar and collision-bearing actors retain their 25%
  drawing margin. The behavior-distance flag keeps its original boundary.
  Room hiding, held objects, collision loading and always-active policies retain
  their existing controls; ordinary non-enemy objects keep their old draw range.
  Clams remain awake throughout their drawing range in both opening and closed
  states, so closed-pose sleeping cannot hide a clam that should still be drawn.
- Physics Lab's extra camera-distance cutoff follows the same rule per enemy:
  reveal at 3,500 units, hide beyond 3,850, with both distances extended by zoom.
  Fresh or reused object slots start without visible history. Lab mover limits,
  model-policy fallbacks, shadows, AI and collision ranges keep their existing rules.

## Mario on rising platforms

- A rising platform can catch airborne Mario when its top moves through his feet
  during that update. Use verified continuous vertical movement, including the
  Fire Sea elevator; Mario already below the previous top stays underneath.
  Bowser Fight 2 tilts also catch rollouts and ground pounds when the rising
  top passes their feet, even if the raised edge exceeds the ordinary floor
  lookup buffer. A verified previous plane and current triangle footprint
  are required; missing history and a stationary or falling top grant no recovery.
  Static ledges and ordinary landings keep their existing collision rules.

- Bowser Fight 3 retains up to two waiting arena-piece collision meshes in the
  existing collision-cache slots. Other pieces and falling pieces use native
  loading; a falling piece releases its cached slot. Waiting pieces load their two
  original top triangles while grounded Mario and the camera are above the arena.
  Airborne or descending Mario, below-arena cameras, Bowser throws or below-arena
  Bowser, and moved, rotated, scaled or falling pieces retain full collision.
  Floor order and ownership, platform carry, fall timing and visual effects remain
  unchanged.
  Pieces retain collision within 4,000 units laterally and 6,000 units below
  Mario, with native support/carry overrides; distant fallen pieces keep drawing
  and updating until their ordinary deletion. Settled flames within the static
  center retain their verified flat floor; moving flames, falling-piece support,
  special collision queries and water updates keep native handling.
  Fight 3 flames skip unused steep-floor wall probes while retaining native
  floor, edge, gravity, landing and water handling; special queries or nonzero
  wall radii retain the complete update. Unshadowed red and blue flames draw with
  their original animated billboard matrices and display lists, with native
  rendering for blinking, attached, held, animated or debug objects.
  Fight 3 emits paired skid-smoke puffs every other update; their size, lifetime
  and placement stay unchanged. Bowser movement, attacks and damaging flames
  retain their timing, counts and interactions. The other fights keep native smoke.

## Enemies on moving platforms

- Custom universal enemy platform carry and departure momentum are disabled by
  default (`ENABLE_ENEMY_PLATFORM_MOTION=0`). Enemies retain native movement and
  support handling, without inherited sideways drift or upward launch boosts.
- Mario's platform carry and inertia remain enabled. Bowser retains his native
  platform displacement; other native enemy and attachment behavior remains intact.
- Moving-floor pose history and once-per-frame object visitation remain active.
  Stunned enemies still fall when support is lost and resume native behavior when
  their stun expires. Disabled custom momentum does not prevent enemy sleeping.
- The shared enemy carry implementation remains available behind the opt-in flag.

## Health recovery

- Recovery hearts spin at no more than 11,000 angle units per update; ground
  twirl reaches that cap. Contact uses the magnitude of Mario's forward speed,
  so backward travel spins the heart like equally fast forward travel. Slower
  contact retains the faster existing spin. Each revolution still grants one
  health wedge; leaving the heart retains its 50-unit slowdown and 400-unit idle speed.
- Queued hit damage removes one whole health wedge every four gameplay updates.
  Partial intervals retain their unpaid wedge for lethal-damage predictions;
  additional hits preserve the active interval and add their full damage.
  Healing keeps its quarter-wedge schedule, but overflowing ticks stay queued
  while hit damage is pending, until a whole-wedge debit makes room.
- At zero health (below `0x100`, including the engine's `0xFF` death sentinel),
  coins, recovery hearts, 1-ups, stars, keys, air bubbles, treasure chests,
  water rings and automatic healing cannot restore health.
  Pending healing is discarded when health reaches zero. Coins and extra-life
  rewards still collect normally; ordinary respawn and course-entry refills remain.
  Mario also cannot start new player-controlled actions. Buttons, stick input and
  buffered recovery requests are suppressed before action handling, including
  after same-frame fatal damage. Current motion, forced reactions and death
  handling continue. Start can still pause during the death animation and death
  countdown; active dialogs and level transitions retain their pause restrictions.
- Entering a playable course from the castle interior, courtyard, or grounds
  starts Mario at eight wedges and clears pending hurt or healing. Debug warps
  trigger the same refill wherever Mario spawns, including castle and non-course
  areas and reloads of the current level. Ordinary castle doors, course transitions,
  exits, and demo or credits entry do not trigger this refill.
- After 1,350 gameplay updates (45 seconds) without health loss or sleep,
  Mario recovers one wedge. Each further wedge takes 90 updates (3 seconds)
  without another loss. The falling-asleep ("I'm tired") animation and actual
  sleep keep the automatic healing timer at zero. Other stationary resting
  states also reset it when equivalent inactivity reaches the configured sleep
  delay: 90 seconds with a cap, 30 without. This includes holding, crouching,
  water idle and attached holds. Fresh button presses, stick input, movement,
  attacks and forced/intangible actions restart that separate inactivity clock;
  held Z can maintain an inactive crouch. No sleep animation is forced.
  Ordinary idle before that delay and the camera's separate two-second idle
  declaration do not reset auto-heal. Ordinary coin and heart healing still work
  while falling asleep or sleeping. A hit, drowning,
  poison, or other health loss restarts the wait even if healing also occurs in
  that update. Deep-water or poison exposure restarts the 45-second wait
  immediately. A continuous shallow-water drowning pose restarts it after 90
  updates; leaving that pose sooner preserves the paused progress. Recovery
  remains paused during damaging exposure and cannot revive Mario after death.
  Secret eligibility does not change these exposure delays.

- Poison gas drains one whole wedge every 64 eligible gameplay updates with the
  regular hat, or every 32 updates without it. Changing hat status preserves
  accumulated exposure. Any body contact with poison gas advances its timer,
  including shallow contact near the surface. Leaving gas for fewer than 300
  updates preserves the partially filled interval; 300 consecutive updates
  (10 seconds) outside poison restart it without restoring health. Healing
  and damage from other sources restart the interval, as they do for drowning.
  Gaining Metal, intangibility or debug protection resets poison progress;
  it stays paused while protected, and protection in poison is not clean air.
  Vanish alone is vulnerable.
  Water and poison keep separate intervals; gas retains priority where both overlap.

## Damage values

- Generic object hits deal their base damage with the regular hat and double
  that damage without it, with a one-wedge minimum for capless hazard hits even
  when their base damage is zero. Existing immunities still apply. Each hit
  is limited to seven wedges,
  including after water/Soaked shock multipliers; separate hits can still add
  damage. Bowser mines retain their fixed seven-wedge hit.
- Small falls deal two wedges with the regular hat or four without it; large
  falls deal four with the hat or seven without it. Existing fall protection
  and Metal immunity remain in effect.
- Lava contacts and crushing deal three wedges with the regular hat or six
  without it, retaining their existing immunity and contact rules.
- Metal electrical shocks retain their fixed three-wedge dry / six-wedge wet
  or Soaked damage. Burning and water-drain timing retain their existing rules.

- Tornadoes remain intangible while waiting at zero size and after shrinking
  completely. Capture becomes available only once visible growth begins; this
  applies to the Physics Lab tornado and the shared course behavior.

## Combined caps

- Wing flight and slower falling remain available with Metal or Vanish active.
  This combined-cap behavior is intentional.
- A fresh Z press during Wing Cap flight or its triple-jump takeoff immediately
  exits to freefall, retaining actual horizontal and vertical momentum and
  restoring the normal camera. That press is consumed; another fresh Z press
  can initiate the usual airborne ground pound.

## Swimming

- Vanish Cap prevents drowning and cold-water damage while active, including
  submerged low land poses. Metal alone is vulnerable to both, while Metal
  combined with Vanish remains protected. Vanish alone is vulnerable to poison;
  Metal, including combined Metal/Vanish, retains poison protection. Damage and
  healing from other sources still apply; cap expiry restores the ordinary environmental drain timing.
  Vanish Mario still emits bubbles whenever his breathing point is underwater,
  including his underwater land actions. Bubble visuals do not consume health
  or air, and dry/intangible states do not emit them.
- Metal and Vanish each allow the full ordinary land action set underwater:
  walking, crouching, crawling, dives, slides, rollouts, wall kicks, jump chains,
  attacks, ground pounds, and held-object actions use their usual land controls,
  collisions and recovery. Metal sinks with reduced underwater gravity.
  Picking up either cap during swimming transfers actual
  horizontal momentum and held objects into land idle/coasting/walking or fall
  states. Shock, death, and scripted water actions finish their own handling.
  Losing the last applicable cap restores ordinary swimming on the next action
  update while submerged; underwater shells keep their existing handoff.
- Metal's ordinary moves receive shared water resistance across ground and air
  states, including held objects, slides, attacks, rollouts, jump chains, wall
  kicks and ground pounds. At full body immersion, travel is 65% of its dry
  rate; horizontal acceleration/braking in world space receives the same scale,
  and each action's gravity change is integrated at 65% before scaled travel.
  Mario still sinks, with a fully submerged downward limit of 75 world units
  per update. Direct ground-pound descent is limited before collision movement,
  and its startup lift, pole travel and ceiling-hanging movement are slowed too.
  Resistance blends in from 50 to 140 units of water above his feet, independent
  of pose; foot-only contact adds no resistance beyond the separate shallow-water
  walking target. Vanish overrides Metal's water resistance, scaled gravity and
  descent limit immediately while active; losing Vanish restores them if Metal
  remains. Metal plus Wing still receives deep-water resistance. Action inputs,
  timers, stored horizontal speed and
  collision stops retain their usual rules; there is no repeated speed drain.
  Dry Metal, Vanish alone and ordinary swimming keep their existing physics.
- An underwater shell touched while Vanish or Metal Cap is active becomes a
  rideable shell. A shell already held for swimming becomes rideable when either
  cap is collected. Vanish takes priority over Metal for underwater shells,
  riding on underwater floors even when both caps are active. Metal without
  Vanish rises toward the water surface by up to 60 units per update,
  subject to ceiling clearance. An upward shell jump keeps its normal arc.
  Once the last active cap expires at or below the surface in swimmable water,
  the same shell becomes a held swimming shell without stopping its music.
  A jump above the surface finishes before this handoff. Ordinary uncapped
  underwater shell pickup still uses the water grab.
- Underwater shell swimming allows the normal water exits: held-A upward
  dolphin launches, A-and-back surface jumps, shorelines and falling-water
  exits. A successful exit stops the shell and its music, then uses the ordinary
  exit action, including water-jump ledge grabs and Wing Cap flight. Failed
  launch attempts keep the shell; throwing and forced drops retain priority.

- Leaving water for land transfers actual horizontal velocity into ground
  speed and slide velocity, resets swimming tilt, and coasts with a neutral stick.
  Water-jump attacks, ground pounds, held-object exits and other shoreline
  transitions clear the inherited swimming model tilt as well as the logical tilt.
  A rising forward dive begun from a slow water jump keeps its vertical speed
  but gains enough horizontal speed to keep the launch from becoming nearly
  vertical; faster dives retain their current momentum.
- A momentum-based swimming ascent that clears the surface while holding A
  enters flight directly with Wing Cap and no held object. Flight keeps the
  established water-launch velocity, starts in the flying pose with pitch from
  that velocity, and does not repeat the surface splash or move twice on entry.
  Held-object and non-Wing exits remain water jumps; blocked, slow, damaged and
  uncontrolled ascents do not start flight.
- Ordinary and held-object water jumps allow stick-controlled forward acceleration
  throughout the flight to help clear pool rims. Only the forward component of
  input relative to Mario's facing adds speed, up to 1.5 per update toward 32.
  Backward and sideways input add no braking, reverse acceleration or sideways
  speed. Faster launches and neutral/backward/sideways input retain existing
  momentum. Steering leaves launch height and vertical motion unchanged.
- During the first ten rising updates of a water jump, holding a strong upward
  swim input at 45 degrees or steeper adds up to 3 forward speed per update
  until horizontal speed reaches 20. This helps slow, steep jumps clear a pool
  rim without stacking the ordinary forward acceleration; faster water jumps
  keep their existing launch speed.
- Vanish Mario makes no splash bursts or step droplets in shallow water, while
  preserving the moving wave trail. When Wing supports him on the water surface,
  his footsteps create surface waves and landings create splash particles,
  including with Metal equipped. Steps, jumps, landings and sliding use the
  underlying floor's sounds instead of water sounds; combined Metal/Vanish keeps
  Metal sounds. Losing Vanish restores normal water effects. Low-pose takeoffs
  in shallow water also skip the underwater ring's splash-fleck burst.
- Submerged distance-spaced wave trails survive either frame parity; ordinary
  surface swimming waves spawn once every three updates. Other-object waves
  retain their every-other-update spacing. Underwater trails
  spawn every 100 horizontal units of travel (previously 85), but stop at depths
  of 500 units or more below the water surface. The stationary surface ripple
  remains visible at those depths. Returning above the cutoff starts fresh
  trail spacing; size and fading stay unchanged.
  Moving trails and the stationary surface ripple remain visible above the water,
  including while Vanish Mario moves underwater. While his head is submerged,
  Vanish Mario keeps that trail but does not create step droplets or splash
  bursts on the surface. His bubbles and their sound start from his current
  position, without a bubble-pop splash at the surface. Grounded deep-water
  jumps and ground pounds emit the existing water ground-pound ring plus three
  short-lived splash flecks near Mario. Ground pounds that enter water from
  above also emit the ring. The surface trail persists during Vanish jumps.
- The underwater ground-pound splash sound plays once when the windup finishes,
  before the four-update delay to the downward impulse. An airborne ground pound
  entering water keeps its immediate splash sound.
- The underwater ground-pound impulse emits six evenly spaced mist puffs
  after two updates of descent, only if Mario has not landed and is at least
  150 units above a known floor when the ring is emitted,
  retaining the ring's radius, motion and scale. Puffs disappear on floor contact
  and fade over 18 updates (about 70% of the former 26-update lifetime). Vanish's separate
  takeoff/water-entry ring retains eight puffs and three splash flecks.
- Breathing bubbles emit once every 6–15 updates while continuously requested
  (a 5–14-update spawner delay plus one update to replace the spawner).
- Water-entry bubble batches emit two small particles per update. Ground-pound
  descent retains three-particle batches, emitted initially and then every
  260 units of downward travel (about four updates at full burst speed).
  Entry/surfacing splashes emit nine droplets across five updates in batches
  of two, two, one, two and two, retaining the splash animation and lifetime.
- Successful underwater ground pounds wait through the normal six-frame ground-pound
  landing animation before accepting swim inputs. Floor loss and normal submerged
  damage, cap and surface transitions can interrupt this recovery.
- An underwater ground pound that reaches the floor plays the land pound's
  heavy impact sound, mist and stars once, without a shallow-water splash at the
  water surface. Its impact sound has higher priority than water splash sounds.
  The impact breaks boxes and activates
  poundable switches and platforms like a land pound, then returns to swimming
  without extra fall damage. A burst ending away from the floor has no impact.
  Landing effects and pound interactions have a 16-update cooldown after an
  impact, matching the underwater windup. Earlier slope contacts still end the
  move and return to swimming, without repeating the impact or restarting its
  cooldown. Action changes cannot shorten it; pause and time stop do not age it.
  The hit object is retained through the next object update, including collision
  rebuilds. Purple switches accept this impact during swimming recovery; ordinary
  swimming contact does not press them.
- Underwater cap boxes break on upward Metal jump contact, a land or swimming
  ground pound, and a forward punch. Solid ceiling, floor and punch-wall contacts
  can trigger their normal break sequence even before an object hitbox overlaps.
  Swimming punches hit during their initial reach, not pickup or recovery.
  Contents, locked outlines and the normal cap-box respawn timing stay unchanged.
- Surfacing after a long dive plays no breath, cough, or choking voice while
  Vanish Cap is active. Picking up Vanish stops an ongoing surfacing breath
  sequence; Metal and ordinary Mario retain the normal sequence.
- Metal Cap lets Mario walk uphill on ordinary slopes up to about five degrees
  steeper before steep-jump or sliding rules take over. Uphill walking loses
  half the usual slope speed. Slide terrain, super-slippery floors, downhill
  acceleration, and manual slides keep their existing behavior.
- Crouching, crawling and grounded belly/low slides use a breathing point 50
  units above Mario's feet. When that point is underwater, they use the existing
  water health drain and reentry-reset timing and can drown without entering
  swimming. Vanish prevents that drain; Metal alone does not. Standing and swimming retain their
  existing breathing thresholds; low poses add no wading slowdown.
- Shallow-water low-pose drowning is an easter egg. Only a fatal shallow-water
  low-pose drain uses the quicksand-style sink instead of the drowning animation.
  Mario's cap disappears partway through the sink; he respawns capless, and his
  regular cap is recoverable at the death location when that level is reloaded.
  Ordinary underwater drowning and actual quicksand deaths keep their own behavior.
  Poison and queued damage in the same low pose do not trigger the sink. A blocked
  secret visit uses ordinary drowning after fatal water drain.
- Dabbing counts as a shallow-water low pose for breathing and drowning. A
  nonlethal wedge of shallow-water drowning damage starts one cough that cannot
  be canceled by movement or action buttons until its animation finishes.
  Lethal damage starts the existing sink immediately; forced floor and damage
  exits still work during the cough.
- Swimming, held-object/shell swimming, plunges, and downward bursts share
  adaptive collision steps: up to four checks, targeting 16 units of
  three-dimensional travel per check after currents. Travel beyond the four-check
  budget uses larger increments. Path checks also catch
  crossed walls, floors/slopes, and ceilings before endpoint correction.
  Movement advances to contact and projects only the remaining travel along
  the solid. Floor and ceiling selection, triangle bounds, and heights use the
  fractional position, so neighboring triangles cannot create false barriers
  at seams. Each substep permits four contact/slide attempts, retaining the last
  reached contact and stopping actual velocity when that budget is exhausted.
  Full-body triangle-edge sweeps also catch finite floor/ceiling faces between
  the foot/head probes, including crossings whose endpoints are both clear.
  Temporary edge contacts expire within the sweep and are never retained. Travel
  beyond 64 units uses larger steps, weakening thin-wall detection and overlap
  escape.
- Swimming wall separation sweeps each wall push at the height of its probe
  and stops at intervening solids detected by the retained rays. This includes
  edge pushes even when the wall solver records no wall hit. Per-push clearance
  and overlap checks reject corrections that embed Mario or fail to reduce an
  existing floor/ceiling overlap. These guards apply during geometry-input
  preparation and movement; final movement clearance checks remain active.
  Within one movement update, a single static vertical wall contact can project
  later increments along its plane while the wall probe stays inside the finite
  face. Edges, corners, moving/slanted walls and outward movement keep ordinary
  separation. An unchanged wall correction skips auxiliary before/after clearance
  queries; final substep clearance and obstruction sweeps remain active.
  Big Boo's Haunt swims requiring at least three substeps filter wall candidates
  by height beginning with the second wall query. The copied lists preserve face
  order and expire at the end of the movement call. Nearby moving walls remain
  checked; leaving the covered height/cells, a push beyond the region or storage
  overflow uses the full resolver. Geometry, body sweeps and movement budgets
  retain their existing rules. In every level, swimming movement over 32 units
  can reuse repeated floor/ceiling queries after an actual wall correction.
  Query kind, fractional coordinates and collision flags must match exactly;
  height, surface identity and outgoing flags are restored. The eight-entry scratch
  cache expires with the movement call; open-water movement leaves it inactive.
  Logical query counters and profiling include every request, including hits.
  Starting floor or ceiling overlap can recover by at most 32 vertical units
  when the full body fits at the destination and the correction crosses no
  solid detected by the retained rays. This recovery does not consume the first
  ordinary query's collision flags. Geometry-input preparation publishes the
  recovery floor/ceiling pair directly when the position stays unchanged and
  collision flags are ordinary. Changed positions, special flags and missing
  floors retain the final searches and graphical-position fallback. Results
  expire when that preparation call returns.
- When a swimming slide meets multiple solids, remaining travel and actual
  velocity respect the earlier contacts together. Hull/seabed contact slides
  along their intersection instead of repeatedly projecting into the other
  face; closed corners stop travel. Touching floor/ceiling faces can be reused
  between substeps within the same movement update when their finite triangles
  cover the projected segment. Moving away or crossing an edge uses ordinary
  sweeps again, and no contact state survives the update. New obstacles remain
  checked along redirected movement. Level geometry and collision eligibility
  are unchanged.
  Exact repeated floor/ceiling queries can reuse height and surface identity
  within a preparation or movement call, including flag cleanup and diagnostic
  counts. Results expire at the end of the call so later geometry changes are seen.
  Preparation shares its exact floor/ceiling results with the immediate geometry
  input update, including the graphical-position fallback and query-flag cleanup.
  No query result survives into later interactions or the next update.
- If swimming starts deeper inside a floor or ceiling, requested movement can
  reduce that existing overlap over several collision steps. It must move
  outward from every intersected face, reduce the overlap, and avoid increasing
  another overlap or crossing a new solid. Faces beyond the ordinary vertical
  query buffer are checked against the starting body when first encountered.
  Idle input does not teleport Mario out; the automatic 32-unit correction,
  swim speed, steering and collision sweeps retain their existing rules.
- Controlled swimming builds movement from Mario's current facing each update.
  Wall contact clips that movement without carrying its sideways direction into
  later strokes. Ordinary, held and shell swimming share this direct steering.
  Stored swim speed remains intact, but a small surviving sideways component
  cannot become a full-speed slide on the next update. Wall contact adds no
  automatic pitch; floor assistance is described below.
- Floor contact automatically raises swim pitch to follow the contacted plane
  gradually, at most 0x200 (2.8125 degrees) per update from the previous pitch,
  when the vertical stick is neutral or upward. Deliberately holding down bypasses
  this assistance; releasing down restores it. Upward pitch is never lowered by
  the floor. The plane normal supplies the slope without rounded nearby queries.
  Ordinary, held-object and shell swimming share the assistance. Swept collision,
  wall-speed retention and direct steering remain active; walls do not add pitch.
- Floor contacts survive subsequent substeps so plunges and downward bursts
  still finish on contact. Controls, stroke acceleration, currents, and action
  timers update once per frame. Solid contact always clips actual travel and
  velocity into the surface, but does not reduce stored forward swim speed.
  Walls, floors, ceilings and cancelled steps can stop actual travel without
  draining that stored speed. Steering clear resumes movement immediately.
  Normal action drag, held-button targets and velocity limits still apply.
  Currents are separate from stored swim speed; a ceiling blocks upward launch
  velocity even though stored forward speed is preserved. Existing
  camera-only, intangible-floor, and Vanish Cap wall rules are unchanged.
- Empty-handed held B targets 12 forward speed; held A targets 24. Their
  acceleration and B braking are 0.6 per frame, preserving roughly the previous
  time to reach either target or switch from A to B.
- The base speed threshold for drag and starting breaststroke strength is 20.
  B's faster turning applies while held below the A swimming target. Breaststroke gains,
  the 48 soft limit, and the 64 hard limit keep their existing tuning.
- From water idle, releasing A within six updates of its initial press starts
  a breaststroke; holding it longer keeps the flutter kick.
  During this initial tap window, the current animation continues playing;
  flutter animation begins only after A becomes a hold. This prevents a brief
  flutter pose before breaststroke, including while holding an object. Movement
  still responds to the initial press, and stroke/recovery timing is unchanged.
- During an empty-handed water plunge, a fresh A press starts breaststroke on
  that update without waiting for the plunge to finish. B retains priority
  when it is the active swim input; Metal and held-object plunges keep their
  existing transitions. A breaststroke begun during the plunge plays its stroke
  sound alongside the dive-entry splash; ordinary strokes keep their sound.
- An empty-handed A tap begun in the air uses a separate six-update breaststroke
  window when Mario enters water from above. Releasing A within that window
  starts one stroke on entry or during the plunge; if the plunge ends first,
  the queued stroke starts in ordinary swimming. B priority or an interrupted
  entry cancels it, and holding A past the window does not queue a stroke.
- Empty-handed and held-object breaststrokes cannot bank their new speed by
  pushing into a wall, slope, or ceiling. Glancing contact keeps the part of
  the stroke along the surface; existing momentum and water-current movement
  retain their ordinary handling.

## Pole movement

- All four castle roof flags are climbable poles, retaining their waving
  animation and the shared grab, climb, handstand and release controls. Their
  turret roofs have collision matching the visible roof faces. The highest flag
  hangs from the top of its 512-unit mast, which remains climbable to the tip.
- The two white central castle tower tiers use standard vertical wall collision
  for wall kicks; they do not act as steep floors. All orange castle roof floors
  have full traction and bypass steep-slope movement restrictions, including
  debug-fly configurations. The highest roof collision matches its visible faces.
  The central lower roof and front gable collision also match the visible tile
  planes, keeping Mario and his floor shadow on the roof rather than above it.
  Tower wall support, full traction, and the rendered roof geometry are preserved.
- Jump kicks can grab poles and climbable trees during both the kick and its
  recovery. A successful grab ends the kick and any queued follow-up; the
  usual same-pole regrab prevention still applies.
- Forward-moving dives always use the Shindou fast pole/tree grab, regardless
  of speed or current stick position. Other eligible airborne approaches require
  forward speed above 32 and the strong forward-stick dive input; otherwise
  they use the short grab animation.
- Pole grab animations, upward climbing movement and animation, and downward
  sliding acceleration and maximum movement are 1.3 times the vanilla values.
  These shared pole actions also apply to climbable trees.
- A can jump off during the final startup pose one update before ordinary handstand
  completion. Neutral startup and return-to-grip timing remain unchanged.
- Returning from a pole/tree handstand restores ordinary grip controls at reverse
  animation frame 1 instead of 0, one gameplay update earlier.
- A fresh Z press drops Mario from the top-of-pole handstand, including its
  startup and return animations. Z takes priority over A. The drop keeps his
  displayed facing and animation height on the current pole position, clears
  pitch/roll and uses the ordinary -2-speed pole release. This applies to trees
  and all shared pole objects; jump and stick-down controls remain unchanged.
- A jump off a pole or climbable tree keeps fall-damage protection for eight
  frames after its handstand animation ends.
- Handstand jumps use compatible animation heights at takeoff and ordinary
  touchdown, without frame blending to hide vertical snaps. Takeoff refreshes
  the current tree/pole attachment and transfers its hip position to the
  airborne origin once; the jump animation no longer adds the pole's height
  again. This also covers jumping immediately as handstand startup completes.
  Jump strength, controls, animation clocks and fall-damage grace stay unchanged.
- Handstand jumps start the landing animation on the touchdown frame without
  advancing landing recovery or consuming input again. Damage reactions and
  leaving the floor during the remaining touchdown travel retain priority.
- Diving from a handstand jump eases the displayed pose over three gameplay
  updates. The dive's input, launch, movement and animation clock stay immediate;
  ordinary dives keep their usual animation transition.
- From player-facing frame 3 of a slow grab or frame 11 of a fast grab, held A or Z on a
  fast spin, pressed A or Z on a short grab, or a strong up or down stick input
  cancels into the normal pole hold. Held buttons can cancel a fast spin when
  that window opens.

## Castle flag secret and cold lobby

- Handstanding on each of the four castle roof flags counts one distinct secret,
  with the native orange count and secret sound. Repeating a flag does not count
  again. A fifth flag sits on the highest castle spire; it appears and becomes
  grabbable only at the current maximum of 120 stars without cheating. Debug
  and developer unlock flags do not satisfy secret eligibility. Counts reset on
  each castle-grounds visit and are not saved. Discovering the fifth distinct
  secret also plays Toad's Message once, whichever flag is visited last.
- At five secrets, contacting the exterior Peach stained glass while shot from
  the cannon or actively flying uses the native painting-warp transition to
  castle area 4. Walking, ordinary jumps and other airborne actions cannot
  enter. Eligibility is sampled before wall contact can change the action.
- The destination retains the native lobby's indented central sun platform and
  replaces the surrounding lobby with an enclosed octagonal pool
  and Wet-Dry World's skybox. The platform and sun decal are four times their original size. Its lower
  checkerboard corners taper inward beneath the octagonal rim without protruding.
  The pool and invisible bounds are four times their original size. The bottom uses
  JRB's sandy ground texture. Shared water UVs remove sector seams; JRB mist
  covers the water up to the raised rim's height but leaves the entire platform
  inside an octagonal opening, free of mist. The water is ordinary
  water, lowered below the platform's upward C-Up camera position. Invisible
  walls surround the pool, with over 1,600 units of thickness and an upper rim at Y=19,900, below the collision query ceiling limit.
  An invisible ceiling at the rim prevents debug flight over the boundary.
  They block movement without bonks, wall kicks, wall slides, pushing/sidling or camera collision.
  The dry octagonal platform has its own matching invisible boundary, using the
  same non-interactable wall behavior, with thick walls outside its rim.
  The underside remains upward-facing, one-way collision so escaped swimmers can rise back inside.
  Mario cannot take health, drowning, interaction or fall damage in castle area 4,
  and pausing is disabled there. These protections do not apply elsewhere.
  Arrival remains on the dry sun platform. Death returns to the castle grounds.
  The sun carpet shares the platform's four-times scale and floor height.
  On earned, non-debug visits, Peach appears at the sun carpet's forward point
  after 15 seconds. Completing each conversation brings in the next guest
  clockwise: red Bob-omb, Koopa the Quick, a sign, Bowser, Piranha Plant,
  Motos, then Toad. Ordinary arrivals fade in over 32 frames with the native
  secret-teleport sound. Bowser accelerates slowly down from above, lands with
  his jump-stop animation, dust, native jump screen shake, impact sound and roar,
  without damage or a shockwave. He laughs after his conversation; Piranha Plant
  uses its native growth state. NPC conversations use the standard shoulder camera,
  model-specific face heights and gaze, an above-model B prompt, and the reading
  sound once on initiation. Finale Toad compensates for the native waving pose
  turning his face 30 degrees right, including during dialogue and restored visits.
  All guests visibly follow their dialogue facing direction. Finale Toad adds
  one Toad Conversation statistic on completion, with no recount on restoration.
  The finale cake screen remains indefinitely without
  pause; any departure is routed to the castle grounds.
  The sunbeam fades from transparent to its original opacity over 30 gameplay
  updates (one second), including on a completed return visit. Its texture and
  vertical alpha gradient are preserved. Guest collision bounds match each
  model's size and scale rather than sharing a 150-by-500 hitbox. The finale
  sign retains its custom reading-volume top as its native post sinks.
  Guests remain present and solid but cannot be spoken to again. Completed
  conversations save immediately; re-entering restores those guests silently
  and resumes the next unread conversation. Fully arrived guests use their
  native depth-writing render layers. After Toad, Peach has one final
  conversation; MIPS then appears at the center, facing her.
  Motos remembers whether his SimpleFlips conversation was completed on this
  save file, using an immediately saved, non-star bit. His two finale scripts
  differ accordingly. Every requested sentence is one dialogue page.
  Only completing MIPS's conversation reveals the native-textured sunbeam
  toward the Wet-Dry World skybox sun and enables the sun-ending trigger. Its
  translucent sheets draw before Mario without writing depth, preventing bright
  bands across his model. Looking
  all the way up in C-Up while facing that sun (within about 11 degrees) fades
  to the ordinary end cake screen over 90 frames, locking Mario and the camera
  during the fade and retaining the normal return to the Physics Lab. Mario
  remains visible initially and fades from full opacity to invisible over the
  same 90 frames without updating movement or camera input.
  The Evangelion ending requires all 120 saved stars and a non-debug visit.
  Fewer stars (including debug visits) instead place a sign at the sun carpet's
  forward tip: "Go back. We know you didn't earn this ending." Full-star debug
  visits read "Go back. We know you didn't earn those stars." UNLOCK_ALL and
  COMPLETE_SAVE_FILE builds also count as debug visits, without bypassing the
  saved-star check: only actual star bits count, excluding cheat-generated
  totals and ending-course metadata. Rejected visits have a black sky and pool floor, no beam,
  water or mist rendering, and no music; voices and sound effects remain audible.
  The platform and sun carpet retain their materials.
  After reading the sign, Mario fades out over 32 frozen frames with the native
  secret teleport sound, reaches zero health and goes directly to Game Over,
  bypassing debug level select. The ordinary sun ending cannot trigger there.
  Fade completion counts the health loss, lost life and Game Over once, using
  the ordinary statistics and checkpointing them before the save reload.
  A wooden sign sits on the pool floor directly beneath the platform. It is
  hidden and non-interactable beyond 300 units of Mario, and reads "How did we
  get here?". Completing its dialogue removes it permanently for that save file.
  Ordinary sign-reading eligibility remains unchanged: it cannot be read underwater.
- This area plays the supplied fuller/punchier Good, or Don't Be arrangement:
  its approximately 77.29-second opening and outro play once, then Section 1
  repeats every approximately 32.54 seconds. Piano, strings, guitar and the quiet
  vibraphone layer use the existing extended bank. Native timing/control and
  loop branches are preserved; the supplied description's mastered audio mix
  is not part of the MIDI and has not been reproduced or listened to here.

## Doors

- Ordinary hinged doors open and complete their passage at 1.2 times vanilla
  animation speed, including their warp-door entry and exit. Mario's passage
  animation, the closing sounds, warp trigger and standard-door camera stages
  share that speed. Ordinary push/pull passages release Mario five gameplay
  updates before the animation endpoint; warp passages retain their normal
  timing at that speed. Star
  doors and key/star unlocking sequences keep their timing.
- Starting an ordinary, warp, key or star-door interaction requires at least
  50% raw stick strength (32 of 64). Mario's facing and intended movement must
  both be within 45 degrees toward the door, from either side. Neutral coasting,
  turning away and passing sideways do not open doors or trigger locked-door text.
- Committed key/star unlock sequences and the 70-star post-dialog continuation
  retain their existing behavior. Door contact still supplies the range check,
  and push/pull selection and warp flags stay unchanged.

## Ledge grabs

- Z is the only voluntary ledge-drop input. Holding the stick backward keeps
  Mario hanging; forward-stick and A climbs keep their existing timing. Lost
  support, unusable ledge slopes and damaging interruptions still force release.

- The ledge-grab "Whoa!" plays once only when Mario was falling at least
  24 units per gameplay update before the grab. Slower downward drift, rising
  and level catches use a quiet "Hrmm!" grunt instead;
  gravity after the collision step does not determine sound eligibility.

- Ledge hanging skips animation frame 0 once on entry. Forward-stick slow climb
  becomes available on the ninth grab update; A still fast-climbs immediately.
  Z release, support/clearance checks and climb-down timing are unchanged.

- The fast A-button ledge climb has no entry grunt; its landing sound remains.

- Upward slow and fast ledge climbs skip animation frames 0–2 once on entry.
  Slow-climb grunt and cancel timing follow the shortened animation; camera
  interpolation and climb-down playback keep their existing timing.

- Intentional ledge grabs from above play Mario's "Hrmm!" lifting grunt once when the
  climb-down starts, across walking, crawling and crouch-slide entries. The
  subsequent hang does not replay it or add a "Whoa!". Lost support before the
  climb-down starts still releases immediately without a grab cue.

- Walking, crawling and crouch-slide climb-down probes validate the final anchor's
  floor and headroom before entering the ledge state. The floor must support an
  ordinary ground step (within 100 vertical units), be gentle enough to hang from,
  and have at least 160 units of headroom. The selected wall, position and floor
  are committed together; invalid seam candidates cannot erase a valid candidate.

- Mario grabs a ledge only while the stick is held toward its wall and no more
  than 90 degrees from his facing. A neutral or backward stick does not grab.
- A live buffered A wall kick takes priority when a contact also permits a ledge
  grab. It retains the existing speed, strong-stick lookback and wall-angle gates;
  expired buffers and ineligible kicks leave the normal grab available. Collision
  stepping stops at the selected kick contact so a later substep cannot replace it
  with a grab. Water-exit jumps retain their existing wall-kick startup lock, and
  landing, ceiling and burning-wall reactions retain priority.

## Debug level selection

- Debug free movement clears the squish timer, squish input and flattened model
  scale each update, including before leaving the mode. Ordinary platform
  crushing and the debug movement collision limits remain unchanged.
- L opens the debug page menu only with the stick inside its normal deadzone
  and Mario in a non-airborne, non-swimming action. Treading water and underwater
  shell swimming also block opening; Metal underwater walking stays eligible.
  L closes an open chooser or returns from Level Select, RAM and Better Reverb
  pages regardless of stick or action. L + D-pad Up always disables an active
  debug overlay and returns to General, releasing any page that freezes gameplay.
  Opening from an ordinary overlay page retains the entry restrictions.
- Ordinary menu selections step once per stick tilt. Holding a direction does not
  repeat; releasing or reversing the stick permits another step. Existing selection
  limits, wrapping and confirmations remain.
- The debug boot level selector steps immediately, repeats after 10 held updates,
  then every 5 updates. Fresh input wraps at the ends; held repeats stop there.
  Direction changes and release restart the delay.
- Debug pages place Collision directly below General, Minimal (when enabled)
  directly below Collision, and Level Select directly below Log. Coverage is removed.
- The debug page menu also repeats held up/down navigation after 10 updates,
  then every 5 updates. Only D-pad inputs navigate debug menus; analogue-stick
  movement is ignored. Fresh D-pad inputs wrap at the ends; held
  repeats stop there. Releasing or changing
  directions restarts the delay. Opening, closing or disabling the menu resets
  repeat timing so a held direction does not carry rapid scrolling into another page.
- The in-game selector accepts an immediate D-pad step, then repeats after
  10 held updates and every 5 updates thereafter. Up/down browse levels and
  left/right browse destinations. Fresh inputs wrap at the ends; held repeats
  stop there. Releasing or
  changing directions restarts the delay; leaving the visible selector or
  opening the debug page menu clears its repeat state. A remains a fresh-press
  warp confirmation. Debug-page directional navigation shares the same 10-update delay and
  five-update repeat, including character/view/reverb choices and the RAM page
  (3 pixels per step within its bounds). Confirmations and toggles remain fresh-press actions.
- In-game and boot-time level selectors display readable course, castle, secret
  stage names. Bowser stages and their separate boss fights have
  distinct labels. Both selectors follow castle progression: courses 1–15 stay
  in course order, secret stages sit near their castle district, each Bowser fight
  follows its approach stage. The list ends with the third Bowser fight;
  End Cake and Physics Lab are omitted from debug browsing.
  Display order is separate from level IDs. Unused level slots are omitted from browsing.
- In-game destination numbers run from 1 through the number of areas with usable
  Mario entrances. Each choice reaches a distinct area; missing areas and areas
  without entrances are omitted. Tall, Tall Mountain's choices 1–3 reach areas
  1, 2 and 4. Left/right wrap within the selected level's destinations; switching
  to a shorter list resets an out-of-range choice before handling further input.
  Levels with only one valid destination omit the destination number; selected
  levels with multiple destinations retain the numbered suffix.
- The in-game panel accommodates the full names and selected-destination suffix. The
  boot selector wraps long names at word boundaries so they fit on screen.
- The in-game level list keeps at least two levels above and below the
  highlighted entry when available, scrolling in either direction only when that
  margin runs out. The first and final nine-entry pages stay full.

- Every gameplay level in the debug menu uses an entrance backed by an active
  spawn object and a warp node. A valid selected area is preserved; node `0x0A`
  is preferred, otherwise an existing entrance in that area is reused. Missing
  areas or entrances fall back to the level's authored starting area, then
  another valid area. Death/knockback arrivals are not chosen as fallbacks.
  Debug selections reload even the current level, cancel pending delayed
  warps, and resolve entrances after the destination script filters objects
  for the selected act. Ordinary gameplay warps retain their existing routing.
- A star-exit results screen without a completed course or star displays
  "YOU SHOULDN'T HAVE DONE THAT." instead of looking up an invalid star name. Normal star
  and key results retain their existing text.
- Normal credits load the interactive ending cake through the level dispatcher,
  showing the usual cake screen and ending sound while permitting later level
  requests. Direct cake requests remain supported even though debug browsing
  omits the cake. The incoming request is consumed on entry; another level
  request unloads the cake and uses the usual destination area and entry node.
  Requesting cake again safely reloads it.
  Its static graph camera does not use the gameplay pause overview when no
  gameplay camera exists, even if an outdoor area's overview mask remains from
  before the transition. Pause and normal departure controls remain available.

## Ending and completed files

- Collecting the final Grand Star in Bowser Fight 3 saves game completion on
  that file. Other files are unaffected. Completion survives reloading and
  copying that save and clears when it is erased; it does not add any stars.
  Demos and credits cannot mark a file complete.
- A completed file defaults to Physics Lab area 1, act 1 on file select when
  cheats/debug do not block secrets. A blocked file keeps the castle start.
  Explicit debug level selections and the existing HE THICC intro/TAS shortcut
  start rules retain their priority. Uncompleted files keep their normal start.
- The Peach ending is pauseable once Mario has finished the flight in Bowser's
  arena and reached the scene outside the castle. The flight itself retains its
  pause lock. Pausing freezes the scene and caption timing; resuming continues it.
  Its pause menu offers Continue and Exit Course without changing the scripted camera.
- Exit Course during the Peach scene enters Physics Lab area 1 at node `0x0A`
  for a clean visit; a visit flagged for cheats/debug enters castle grounds
  area 1 at node `0x32` instead. Turning cheats/debug off during the same visit
  does not clear the flag. The Grand Star, Peach and ending-screen reloads retain
  the Bowser visit's flag even if cheats/debug were turned off before collection
  or departure. An ordinary course reload resets and reevaluates it.
  Finishing the Peach scene also enters the Lab instead of starting the credits
  montage, using the same cheats/debug destination check. Both routes clear
  credits/subtitle state and restore gameplay sounds and the file's character
  on arrival. Debug level selection does not override these ending destinations.
- The cake screen offers the same Continue/Exit Course menu despite having no
  Mario or camera. Exit Course uses the same clean-Lab/blocked-castle destination.
  After the thank-you line, 300
  unpaused updates (10 seconds) automatically start the same departure; pausing
  preserves the countdown, and re-entering cake starts a fresh countdown.

## Physics Lab layout

- A repeatable ordinary wooden sign beside the spawn welcomes Mario. Full-star
  files receive the completion message; other files receive the developer/early
  access message. Each supplied sentence has its own dialogue box. Debug
  mode always receives the developer/early access message, even at full stars.

- Its background music is TheNail's Mii Channel — Wii port, using the Inside
  Castle sound bank. It has its own sequence slot; cap and event music keep
  their existing priority and restore the Lab track afterward.
- The Physics Lab's normal entrance is area 1 at warp node `0x0A`.
  Its existing death/respawn warps return to that entry; Exit Course keeps
  returning outside the castle.
- Related fixtures share walking routes and supply points. The main runway and
  promenade remain clear of placed ground enemies and unrelated obstacles.
- Four districts surround the commons: motion, moving platforms, collision
  and water. A broad flush forecourt joins the starting area, water edge and
  Y=120 chamfered stopping terrace beside the launch tower. An inset rear
  curb rises 120 units above the terrace's back edge; the former isolated pad,
  tall post and crown are removed. The runway stays flat and clear. An uphill motion shoulder,
  angled collision cheeks and offset platform landing forms distinguish the
  approaches without entering comparison fixtures or spanning their entrances.
- The launch tower keeps all six switchback ramp profiles, turn landings and
  its Y=3000 deck. Its raised sections have closed slab undersides and outer
  columns, leaving ground passages open beneath them. The ground-level first
  ramp and separate lower drop shelves retain their original solid forms.
  Four corner posts replace the lift's side wall, outside its full travel.
  Its roof remains at Y=950 underneath and Y=1120 on top, preserving 130 units
  of clearance at the lift's high stop.
- Four flush directory panels on the forecourt use larger lettering and
  separated arrows: motion/platforms in the north pair, collision/water in
  the south pair. They point west, east, southwest and southwest respectively
  when read facing north. The rear curb remains low and has no lettering.
  Solid neutral directory and junction panels retain exact floor collision;
  the measuring grid keeps its phase around them and through the entrance pads.
  Garden and hill directions use clear western ground approaches; the aquatic
  direction leads east along its northern shore. Neutral aprons and ten
  120-unit corner markers frame both western Bully forecourts, shallow west
  entry, cold north entry and the aquatic south walking exit. The Bully
  reference lines, open entrances, ramps, supply crossing and promenade stay clear.
- One broad stepped landmark south of the western ground link marks the pair
  of Bully courts. Its five low sections peak at Y=360, outside their matched
  interiors, entry/recovery markings and the walking link at Z=5400–6000.
- A native 3200-unit pole at X=1020/Z=-2100 provides direct access beside
  the launch deck's west edge,
  clear of its underside and columns. The six-ramp walking route remains.
  Single start bars and a shared paired finish mark the four slope lanes;
  each balance beam has its own staging marks. These are flush paint outside
  the measured ramps and beam spans, preserving collision, friction and widths.
- The widened northern terrace joins the matched-slope landing and both
  platform docks; a front ramp returns directly from that landing to the
  commons. Closed undersides and columns outside the ground routes keep those
  routes usable below. The rocking bridge and disk share a thin Y=350 gallery;
  its side link preserves the disk's original ground ramp and the eastern
  promenade. A western beam return reaches their start deck at Y=250 without
  descending, and a Y=240 side crossing connects that deck to the shallow rim.
  Each route also has a walking ground return.
- Separate nonslippery dry quays follow the shallow and cold pool rims, with
  independent ground approaches. The supply crossing between them remains at
  Y=0, and the adjacent ground promenade retains its width. Three inward
  chamfered shoulders fill dry gaps beyond the pool rims at X=5710–5850;
  their nonslippery tops stay below the adjacent quay ramps. Original ramp
  profiles and widths, rims, entrance markers and water volumes remain exact.
  Flush district,
  surface, width and depth labels change rendering only; their original floor
  collision, friction and world-aligned scale grid remain intact. The Platforms
  entrance label stays clear of its boarding ramp. The two low BLJ access ramps
  replace the ground rendering beneath their footprints, retaining their exact
  ramp profiles and all ground collision.
- The water tests and open aquatic enemy basin form one southern district;
  cold-water drain remains confined to the cold pool's actual bounds. The main
  pool and water-annex divider is 240 units thick, retaining its west edge and
  the full 1,000-unit connecting opening.
- The rolling boulder stays in its own walled lane beyond the BLJ exit, away
  from the pools and promenade. On the lab's flat ground it regains its initial
  rolling speed so the test keeps running; the ramps retain normal slowing and
  reversal. Boulders in other courses retain their ordinary behavior.
- Objects retain wall-solver separation even when an edge or containment surface
  supplies no interactive wall contact. Such a push reflects inward horizontal
  travel using the push direction; outward travel keeps its heading. Reported
  walls retain their existing first-wall reflection.
- Normal, slippery, very slippery and nonslippery sculptures each have full,
  half and quarter-size examples. The ice and Vanish Cap sets are removed,
  including their collision and smaller copies. Remaining specimens keep
  their original positions, collision and the smaller copies' 180-degree turn.
- Layout cleanup preserves test slopes, widths, clearances, momentum runouts,
  BLJ step dimensions, native enemy behaviors and the full non-boss roster.
- The added empirical bays preserve the existing fixtures. A and B use one
  translated seam template: smooth control, coplanar irregular triangulation,
  an 8-unit lip and a normal-to-ice boundary, with matched starts/recovery.
- Impact bay C compares dry, 60-deep and 600-deep water beds with entry plane
  Y=0 and release decks at Y=1700/3600. Water beds have walking exits; a native
  3800-unit pole reaches their boarding arms. Actual shallow-floor contact is
  60 units below the dry floor, so measurements must use the actual contact.
- Bullet Bill and the water-bomb spawner retain their separate lanes in bay D.
  The authorized enemy-layout pass orders Goombas tiny through huge and aligns
  the Koopa variants; explosive/fire targets use neighboring firing bays with
  an open western side corridor and a 1500-unit eastern backstop. The long grey
  western barrier is removed from both rendering and collision. Their native activation, aiming,
  spawning and interactions remain.
- Chuckya and Heave-Ho share the clear strip north of mover boarding. Heave-Ho's
  west approach leaves a long eastward throw recovery; other approach angles
  retain native throws. Both Bullies use matching dry courts, preserving their
  behavior while removing ice as a comparison variable. Whomp, Thwomp and
  Grindel form a northern row beside the unchanged Tox Box sweep.
- Western air targets have a separate Lakitu/Spiny spawning floor; wind targets
  face away from the main runway. Haunted targets stay with Swoop's ceiling,
  while homing Amps use a dedicated southwest shock court. Their native
  1500-unit home chase ranges clear the impact beds and main walking routes.
  The three existing water-test circling Amps and aquatic habitat stay in place.
  Enemy staging never changes AI ranges or adds sleeping/culling behavior.
- Bay E is a fixed animation observation mark. Flush A–E entrances and 1000-unit
  stopping-distance marks provide physical references; northern course paint
  preserves its original collision and friction. Reset, calibrated release,
  frame-step and measurement controls remain separate future instrumentation.
- Every cap supply contains one each of Wing, Metal and Vanish, placed together.
- Cap and shell boxes have flat ground under their whole shadow footprint,
  including its expansion when the box is hit.
- Their shadows stay on that ground when hit and after the boxes respawn.
- Wing, Metal and Vanish Cap pickups in the playground each grant 120 seconds
  (3600 gameplay frames), including repeat pickups. Other courses keep their
  existing durations and the shared cap timer retains its normal pause rules.
- While Vanish is active, its power-up music takes priority when another cap is
  picked up and when area music restarts.
- Moving fixtures have room for their complete travel and rotation; boarding
  ramps and neighboring sections stay outside those swept volumes. Intentional
  low-roof, closing-wall and bad-geometry tests retain their tight clearances.
- Visuals use a shared ground grid, neutral structures and courts, and matching
  colors for equivalent physics surfaces. The lab's opaque ice receives decal
  shadows; transparent water keeps its normal shadow rendering.
- Playground rendering uses prebuilt terrain lists: one ground pass and six
  ordered structure runs. Near and distant structure meshes use consecutive
  child groups with padded RSP bounds, up to 256 triangles and an 8,192-unit
  X/Z span. Vertex uploads retain the 24-entry cache limit and primitive order.
  Disjoint checker floor rectangles may merge across former list boundaries;
  world-aligned UVs, colours, openings and collision remain exact.
- Static structure rendering removes precisely covered interior wall portions
  at adjoining boxes, slabs and sealed wedges, preventing dotted end-wall seams
  across slope/landing joins. Exposed walls, materials, collision and antialiasing
  remain intact; open wedges, Vanish surfaces and separate LOD caps cannot hide neighbors.
  Exact opposing sloped end faces of sealed solids are removed as internal joins,
  including the precision ramp's return lane; exposed tops and collision stay exact.
  Buried-base wedges with above-ground tops also qualify when ground seals their
  full footprint, removing the half-pipe's internal end-wall seams.
  Hidden checker ground beneath the half-pipe is omitted from rendering so it
  cannot bleed through the zero-height trough; its collision stays intact.
  Flat opaque floors at other heights can seal static boxes too, including the
  underwater shaft, tunnel and steps. Floor gaps and Vanish/LOD floors cannot.
  The connecting pool banks render only the side walls above the adjoining
  floor profiles. Their shared integer vertices prevent floor-to-wall cracks;
  floor portions hidden inside the banks are omitted from rendering only.
  Both dry and underwater treasure-chest pairs face 180 degrees away from their
  original headings, retaining their positions and native opening behavior.
- Checker floor vertices use normal clipping by default; merged patches can
  cross the camera plane in close views and leave holes under a wider guard.
  Only the validated aerial stream may use a two-times floor guard, when the
  complete geometry-derived floor bounds are more than 4,096 world units in
  front of the camera plane, with a finite depth no greater than 65,536.
  Both floor streams share all geometry and restore the configured baseline.
  Scope a two-times clipping guard to the shared structure/sculpture material;
  restore the configured baseline afterward.
  A single face can exceed the terrain group's 8,192-unit X/Z span. Those
  oversized groups use normal clipping before vertex transformation to prevent
  long walls warping near the camera plane. Restore the two-times guard in the
  parent after each child, including when the child returns early after culling.
  Twenty complete flat label footprints use opaque point-sampled RGBA32
  textures derived from the existing block font and palette. Split checker
  labels at world grid boundaries and preserve the integer colour product.
  Incomplete footprints retain their geometry. Cull labels before loading
  their texture, and restore texture, filter and shading state outside each child.
- Northern command streams share the complete meshes' original vertices and
  padded bounds. Retain northern edge faces; omit only +Z planar faces at or
  beyond Z=-12,000 when the camera is north of every omitted plane by 32 units.
  A high camera in the central strip may also omit proven downward/outward
  backfaces. Choose one whole-mesh stream; preserve original LOD decisions.
  Stale, invalid, scaled, reflected or transformed contexts retain complete
  lists. Do not run per-section CPU frustum tests, wall occlusion volumes or
  animated object/shadow envelope tests. Keep ordinary engine object culling.
  Retain render-only coplanar merging and removal of completely buried opaque
  faces. Preserve pool holes, partially covered faces, Vanish fixtures and all
  collision; clipping and textured lettering may change edge rounding and antialiasing.
  Flight-tunnel roof joins omit buried slab caps and trim only the covered
  portions of the adjoining side strips; exposed roofs and tunnel walls remain.
  Slide-circuit straight/turn joins omit the four buried straight end walls
  from rendering. Both turn LODs retain shared riding edges; collision and the
  exposed ends at the jump gap remain unchanged.
  The BLJ runway's exit/side-ramp joins and the outer wall corners likewise omit
  only buried wall portions, retaining exposed walls and all collision.
  The cannon recess uses distinct flat tones on its three interior walls to
  make its depth visible; its opening, floor and exit ramp remain unchanged.
- All 12 static sculptures share one transform and material wrapper. Nearby
  meshes use 92 triangles and distant meshes use 50: merge the plinth top
  and remove only faces proven fully buried in another solid at generation.
  Keep exposed fins, central ramp, low overhang and nearby facet colors.
  Distant meshes share similar incident corner shades only when each color
  channel's range is at most 8/255, retaining every distant triangle, outline
  and stronger facet contrast. Nearby shading remains exact. Preserve
  each remaining specimen's original 126 collision triangles, surface properties
  and the small copies' rotation and scale. Retain RSP bounding-box culling in
  each list, with material restoration outside the culled child.
  Banked turns and uneven hills may use half-density distant meshes. Use
  prebuilt lists, camera zoom and 10% hysteresis; terrain switches around
  5,000 units to its nearest bound, statues around 3.75 times their largest
  span (minimum 2,250). No per-frame mesh simplification is performed.
- Selected callback-free enemy models stop drawing past 3,500 camera-depth
  units, and five lab mover models past 6,000; zoom extends these ranges.
  A bounded model-policy cache stores only these simple limits and resets on
  level geometry creation. Unknown/attached models and compatibility contexts
  retain ordinary rendering. Render limits never change AI or collision ranges.
  Visible enemies, friendly NPCs, and cap/shell boxes in the lab keep their
  shadows at any camera depth while their models are drawn. Other non-player
  lab shadows fade between 2,000 and 3,000 camera-depth units and stop
  generating beyond that. Preserve Mario, mirrored Mario, held-object shadows,
  model-child traversal and all other levels' shadow behavior.
  Render rejection preserves animation clocks, sound positions and cleanup;
  ordinary lab objects can reject before building their root transform.
- Physics Lab defaults to aggressive distance sleeping. Ordinary enemies and
  moving fixtures freeze beyond 2,500 horizontal units or 1,000 units above/below
  Mario from their current position, even while visible, attacking, airborne, recovering or
  changing actions. Scripts, animation clocks, cooldowns and expiry timers pause.
  This intentionally changes offscreen gameplay and hazard timing. Moving Lab
  fixtures and checkerboard elevators also participate. Settled small/metal boxes, permanent ground
  shells and the added fish/Buddy families use the idle-state rules above instead.
  Chest assemblies, mole holes, trees, poles, coins, loose caps and 1-ups keep their
  normal updates so puzzle resets, cooldowns and collectible expiry can finish.
  Carried Boo coins may follow a sleeping owner; released ones keep updating.
  Moving collision objects retain a wake bound of at least their collision
  distance plus 600 units; surfaces still
  publish collision while sleeping. Mario, initialization, held objects, current
  contacts, Mario's supporting platform and time-stop handling remain live.
  Attachments also wake near their owner. Lab movers use a common home envelope
  extended by 900 units, keeping paired closing walls synchronized.
  Sleeping objects are marked visited and have animation/blink flags cleared.
  Distance and room visibility still refresh every update. Enrolled enemies and
  their enrolled attachments do not render while asleep, even inside drawing
  range. Lab enemy display uses an independent history bit: entry uses the
  existing wake cylinder, while exit adds 300 horizontal and 150 vertical units.
  Valid attachments share their owner's display range, including Pokey stacks.
  Mandatory collision/combat updates outside that range remain live without
  revealing a distant model for one update; native hiding, room and drawing
  limits still apply. Other levels retain their existing wake/reveal behavior.
  Idle Bob-ombs use stable wake/display history in every level: enter at 2,500
  horizontal / 1,000 vertical units; after appearing, stay awake and visible up
  to 2,800 horizontal / 1,150 vertical units. Beyond the exit boundary, ordinary
  idle sleeping resumes. Mandatory contact updates do not flash a hidden idle
  model. Fuse-lit, held and non-patrol states retain native visibility, including
  room and drawing-distance limits; the Lab's existing aggressive visibility
  policy remains unchanged for non-idle states.
  Sleeping fixtures and supplies retain native distance/room visibility.
  Collision publication cannot reveal a sleeping enemy. Active-from-afar behavior
  flags remain intact.
  Display history uses one object-flag bit, reset on behavior initialization;
  no extra object fields or per-object sleep cache are introduced.
- Distant Lab projectiles and cosmetic objects are discarded beyond 4,000 units
  in 3D rather than retaining frozen lifetimes. This includes disposable-particle
  objects, Snufit shots, Mr. I shots, mole rocks, flame effects, released snowballs,
  water bombs and Lakitu-spawned released Spinies. Held projectiles stay with
  their owner. Removal clears Lakitu's Spiny count and the water-bomb spawner's
  active-bomb flag; normal object teardown still releases their resources.
  Other levels retain their existing updates. The previous conservative enemy
  sleep policy remains available with PHYSICS_LAB_AGGRESSIVE_SLEEP=0.
  The debug profiler shows updated/sleeping object counts and separates object-pair
  checks, platform setup/carry and cleanup separately from Behavior. Behavior
  includes spawner, surface and other non-Mario object updates, with measured
  terrain-query time excluded across each complete update interval. CPU timings
  are emulated microseconds and depend on the emulator's clock model.
  These diagnostics sit below the right-hand collision counters, preserving
  the left column's full RSP display.
  Skipped main-thread phases contribute zero samples, so pause flushes their
  rolling averages after 15 frames without resetting audio/RSP or FPS clocks.
  This includes platform setup and object cleanup samples.
  `play`/`aplay` monitor general performance using a separate capture ROM;
  `--no-capture` selects the normal ROM. Detailed collision candidate counts are
  opt-in with `--collision` because those hooks add overhead. The recorder saves
  15-frame pacing windows, CPU phase averages/peaks, audio and graphics-processor
  timings, Mario's location/action and capture cost without requiring an open debug
  overlay. Only the optional collision mode exports every frame. See
  `docs/performance-capture.md` for measurement limits.
  The following enemy carry details apply only when the optional
  `ENABLE_ENEMY_PLATFORM_MOTION` flag is enabled.
  Objects outside the enemy-platform policies skip provider, carry and support-history
  work during their native update. A script change still initializes or clears that
  history in the same update. Ordinary updates keep the cold stun handler separate
  so unstunned objects do not save its floating-point registers on the stack.
  Motion resets retain behavior-keyed classification; script changes and area
  reloads still invalidate it. King Bob-omb's inactive home-hold phase does not
  request carrier collision; dialogue, combat and stun retain platform eligibility.
  Wooden posts use automatic mesh-based collision distance when enabled, keeping
  their native drawing distance and avoiding remote post-to-post collision requests.
  Chain Chomp searches eligible riders for its post before tether reconciliation.
  Enemies without support or departure motion skip carrier searches in cells
  containing no dynamic floors. Existing support, inherited motion and static
  landings retain their full support query; new floors are acquired immediately.
  The displacement solver saves its working registers only when an object has
  a carrier or departure motion to apply.
  Pose snapshots retain only terrain published in the preceding frame. Additional
  enemy collision requests reject empty space outside the transformed mesh's local
  vertex box, including rotated corners, with body/next-step/inertia padding.
  Offscreen requests derive the loader's pending transform without changing object
  state; unsupported data, overflow and pending carry retain the coarse fallback.
  Existing support always requests publication. Visit status shares classification
  padding, and full support searches keep their register setup off the empty-cell path.
  Stable supply boxes can skip redundant setup outside their existing collision
  radius while preserving attacks, visibility and respawn timers; waiting
  Amps/plants/Swoops can skip unused angle calculations.
  With opt-in COLLISION_RESULT_CACHES=1, far-away/different-room objects whose
  movement is already suspended can reuse
  unchanged static floor support through a bounded 32-entry cache (1,024 target
  bytes). Scripts and clocks keep running. Position/support changes, terrain
  reloads, changed grate eligibility, special collision flags and any dynamic
  floor list in the queried cell force a fresh lookup. Reactivated objects and
  Mario always query normally; rendering visibility alone never gates physics.
  Only eligible suspended objects with static support populate this cache;
  active objects cannot evict their entries.
  Particle replacement stores reviewed disposable candidates without sorting
  or shifting on insertion/removal. Selection explicitly preserves pool-index
  ties and scans only those candidates, preserving
  inactive preference, priority, birth order, protected objects and teardown.
  Painting ripple distances are shared by identical mesh/geometry parameters;
  live wave timing and equal-distance vertex reuse keep their existing math.
  In Physics Lab, Skeeter/shark cosmetic waves emit every second area update
  within 3,000 horizontal units of Mario and every fourth farther away. Ordinary
  levels retain full emission cadence for nearby or potentially visible waves.
  Only distant awake emitters request an ordinary-level camera snapshot; missing
  or cross-level/area views retain emission until a fresh snapshot is available.
  Distant offscreen waves may be suppressed with a generously padded view test; roll, stale
  views and compatibility contexts fall back to emission. Ordinary-level
  suppression affects only cosmetic waves and never gates enemy updates or terrain.
  Damaging projectiles, moving fixtures and the rolling boulder remain active.

## Pause menu

- Pause-menu scrolling shares the coin sound bank and overlaps the pause and
  unpause tones on the menu bank, allowing those tones to finish independently.
- The D-pad navigates pause options and castle scores with the same single-step
  controls as the analog stick. Hold Z and press D-pad left/right to change camera
  speed; the pause page displays this control, and those presses do not move the
  camera-mode selection.
  Aspect ratio switches only with Z held and a fresh L press; its label shows
  the Z + L shortcut.
- An eligible Start press opens pause before Mario, other objects, physics,
  camera or gameplay timers update that frame. Simultaneous action buttons
  cannot start actions or create fresh action buffers. Existing pause restrictions
  and pending transition/demo-exit priority still apply; rejected pause presses
  leave ordinary gameplay input handling active. Resume behavior is unchanged.
- Pause keeps the current song playing at 20% of its normal volume without
  restarting or freezing it. Volume fades down alongside the three intermediate
  screen-blend frames, then remains at 20%; resuming fades it back up with the
  closing blend. Gameplay sound effects remain muted while paused. Existing music
  fades and volume constraints remain independent of this pause multiplier.
- With pause music disabled, each eligible pause rolls one exact 1/8 chance
  for the supplied Dunkey pause-theme quote. A winning pause chooses a random
  delay from 1 through 4 minutes of uninterrupted idle time, measured after
  releasing the opening Start press. Any further button or active stick input
  cancels that pause's opportunity; it does not reroll while paused. Menu exit,
  resume and warps cancel pending/playing audio. Demos and credits do not qualify.
  The quote is uncompressed 16-bit mono PCM matching the output rate (30 kHz
  during the audio-quality trial, 32 kHz with the trial disabled), played through both speakers
  without unmuting the normal sequences. Triggering immediately saves a consumed
  marker for the active file, including its backup; restarting or copying the
  file cannot replay it. Erasing the file resets it. The current US/JP/EU engine
  supports playback; Shindou cannot trigger it.
- The pause page appears on the update that accepts Start. The opening Start
  press cannot also close the page, even if held. Closing is allowed only after
  8 frames have elapsed, in both course and castle menus. Releasing and pressing
  Start again during the delay arms resume while Start stays held, allowing
  unpause on the first available frame. Releasing it cancels this request.
  A new press after pause opens similarly arms selection of the highlighted option
  while A stays held; releasing A before the delay ends cancels it. A held during
  gameplay or pressed on the opening update cannot arm selection. Navigation and
  settings remain available during the wait.
  Every new pause starts a fresh delay, regardless of repeated render passes.
- Physics Lab area 1 supports the ordinary pause-camera overview. Pausing while
  still or moving no faster than 0.5 uses the zoomed-out view; pausing while moving
  faster keeps the gameplay view. Resuming restores the normal camera.
- Pause draws its shading and menu at their normal opacity. The complete screen
  blends with the image from before the transition over three intermediate frames
  when pause opens and when play resumes: one-quarter, one-half, then three-quarters
  of the new view. This includes the background camera, Mario, HUD and menu, and adds no
  input delay. Subsequent frames are unblended. Camera speed and widescreen
  labels remain in the closing pause image to blend out with the other text;
  their settings cannot change on that closing frame.
  The RDP reads the previous framebuffer; when an emulator reuses the same one,
  it first preserves that image in an existing spare framebuffer. No additional
  screen buffer is allocated. The original image remains intact through all three
  blend frames; the first blended result is not reused as the source. Warps,
  area/level changes, missing frame history, special viewports and unavailable
  display-list space skip the blend.
  Six-row RGBA16 strips fill the screen exactly and stay within texture memory;
  the copy fallback and the blend use the same strip coverage.
- On Parallel Launcher only, the world image resolves to native 1× pixels while
  paused, before HUD and menu drawing. This keeps the paused world at the same
  coarse resolution as the framebuffer fade. Gameplay resumes normal enhanced
  rendering after unpausing; the emulator’s global upscaling setting is unchanged.
  Other emulators, fade timing, warps and special viewports retain their existing
  behavior. Insufficient graphics space skips this extra pass safely.
- The outdoor pause camera overview appears on that same opening frame only
  when Mario was still before pause (actual movement is at most 0.5 units per
  update). If Mario was moving, the pause page retains the gameplay
  camera view. This movement value stays frozen while paused, so repeated render
  passes do not change the choice; resuming restores the gameplay view.
  The existing per-area overview eligibility and framing remain unchanged.
- Collected red-coin icons remain visible at 96/255 opacity (about 38%) after
  their star spawns, including after pickup. Before that, they stay fully opaque.
  Spinning frames and collected counts are unchanged, including the numeric
  display for more than nine red coins. World coins retain their usual material.
- Start resumes play regardless of the highlighted pause-menu option. A alone
  selects that option; Start takes priority if both buttons are pressed together.
- In a playable course, including secret and Bowser courses, Exit Course is
  available whenever the game permits pausing, including while Mario is
  airborne. The existing exit destination still applies.

## Camera and silhouettes

- Swimming (including idle swimming) and Wing Cap flight show a circular shadow
  on the nearest solid wall or slope ahead of Mario's body, following his yaw and pitch. Its range
  is 600 units plus twelve times his forward speed magnitude (twelve updates of travel).
  It fades to zero at the current range limit and sharpens as he approaches.
  The cue sits slightly above the surface with translucent depth testing, so Mario and
  closer geometry hide it. It checks floor, wall and ceiling partitions so slope
  classification does not hide the cue, respects Mario's wall filters including
  Vanish-passable walls, keeps his usual ground shadow, and is absent during
  intangible actions. Other objects and mirror Mario do not create this extra wall cue.

- World-space box culling retains terrain and particle groups during screen roll;
  the unrotated camera transform cannot prove that the rotated view hides them.

- Reonucam continuous rotation speeds are 2, 3, 4, 6 and 8 degrees per update
  for Slowest, Slow, Medium, Fast and Fastest. Boot starts at Medium. Each save
  file retains its own speed; unset, existing and erased files default to Medium.
  Copying a file copies its speed preference. Pause-menu speed changes apply immediately
  in memory and update the label on the same frame. The final preference saves when
  leaving pause, including Exit Course and the TAS shortcut; unchanged preferences
  do not trigger a save.
  R-modified 45-degree snaps and R-tap timing retain their existing behavior.
  Flight, Wing triple-jump takeoff and cannon shots do not count R presses as
  ground-camera taps. An R hold carried out of flight must be released before
  another ground tap can count, preserving the chosen ground camera style.

- Persistent life, coin, star and key counters hide after their entry/reveal
  period and reappear for 90 gameplay updates after collecting a coin. The
  camera icon has its own reveal period after an actual Mario/Lakitu switch or
  entering/leaving C-Up mode.
  Both groups reappear after 60 idle gameplay updates and hide again when Mario
  becomes active. Both groups fade in and out over six gameplay updates (0.2
  seconds); camera rotation does not restart or interrupt a fade-out. They are
  fully visible on the pause menu and stay visible for 90 gameplay updates
  after unpausing. Health and course timers remain visible.
  The cap timer shows only remaining seconds at bottom left with input display off.
  With input display on, it centers beneath the health meter. Toggling the overlay
  updates the timer position in the same displayed frame. It uses white vanilla dialogue lettering
  with a dark shadow and a thin underline beneath the countdown. The underline
  is green above 10 seconds, amber from 5 through 10 seconds, and red below
  5 seconds, for every cap type. It shrinks toward the next threshold, then
  refills for the new colour. Green starts full at pickup or a timer extension;
  amber drains from 10 to 5 seconds and red drains to expiry. Its full length
  stays fixed as the digit count changes. All cap combinations share this one timer. Remaining
  whole seconds round up, use a lowercase s suffix, and align to a fixed units
  column within the health-centered timer container so the countdown does not shift
  when its digit count changes. The underline follows this alignment. Countdown digits and the suffix
  share one shadow pass and one foreground pass. While green, the cap timer follows
  the counters' reveal and idle rules with the same six-update fade. Each successful
  special-cap pickup also reveals this timer for five seconds (150 gameplay updates),
  refreshed by another pickup even when the shared countdown is already longer.
  Coin extensions do not restart this pickup reveal. At 10 seconds
  or less (amber/red), it fades in and stays visible. An extension back into green
  returns it to the counter visibility rules. Numbers, shadow and underline fade
  together; fully hidden timers submit no drawing. The timer is fully visible on
  the pause menu, and its fade clock freezes during time stops.
- HE THICC uses cyan (RGB 0, 255, 255) in the existing through-wall silhouette
  pass, fully opaque. Both appearances and mirror Mario share the colour. Standard Mario and held objects retain their original
  silhouette colours; normal visible rendering and layer eligibility are unchanged.
- Mario and mirror Mario retain animated through-wall silhouettes across body
  appearances and cap changes. Their wall-visible copies are suppressed during
  dim damage-blink frames and restored on bright frames; the ordinary body's
  25% dim opacity, cap warnings and held-object silhouettes remain unchanged.
  Silhouettes use the exact geometry and detail
  level selected for the visible model, with a shared 31/32 scale around each
  animated part's pivot to tuck the opaque silhouette inside the visible outline.
  Vanish/warp copies instead use the exact ordinary matrices, including wings,
  so their cutout pixels line up. Ordinary Mario, animation pivots and held-object
  sizes stay unchanged.
  Standard Mario's through-wall silhouette
  is light gray (RGB 208/208/208); the cyan character variant retains its color.
  Vanish/warp wall-visible copies use a fixed pale gray (RGB 208/208/208),
  with the cyan variant retaining its hue at the same brightness. Accepted
  pixels replace the wall colour, so overlapping triangles cannot brighten
  them repeatedly. Dither gaps remain transparent; no solid backing is added. Vanish/warp Mario and his wall-visible copy
  share a fixed screen-aligned dither pattern on console/native RDP emulators.
  Mupen-based emulators use their standard screen-space noise comparison with
  full-range opacity because GLideN64 tests alpha before adding the ordered
  pattern. Both passes select the same comparison. A separate two-cycle alpha
  comparison controls Vanish opacity; accepted body pixels cover the white outline
  completely. Accepted silhouette dither pixels draw
  without the old coverage-overflow rejection, so joints and overlapping
  body triangles do not leave extra holes behind walls.
  Every body part, including limbs with inherited materials, uses that comparison.
  Both passes use the same single coverage sample; accepted ordinary pixels,
  including dim phases, do not blend white silhouette coverage at triangle edges.
  The material restores one-cycle rendering and alpha comparison after each part;
  the renderer restores the layer's blender before subsequent particles draw.
  Ordinary visible models, collectible and held-object silhouette colours
  and textured wing masks are retained. Mario body silhouettes use opaque rendering without framebuffer reads or overlap
  blending; collectible and held-object silhouettes retain their opacity.
- Ordinary native idle, walking, tiptoe and crouch/crawl poses receive a render-only
  correction for shoe burial, capped at 8 world units. It uses the displayed
  animation frame and the worst shoe depth across all three model details. The
  correction tapers continuously to zero at a 15-degree floor incline. It moves
  the body after shadow placement, so visible geometry, silhouettes and hand
  attachments stay together while movement, collision, camera and shadow anchors
  remain unchanged. Airborne poses, quicksand sinking, squishing, active foot
  scaling, custom/blended poses and existing terrain-aligned transforms retain
  their original rendering. This reduces clipping; it does not fully conform
  Mario's feet to slopes.
- Mario and mirror Mario restore alpha comparison after their model draw, so
  Vanish/teleport dithering does not leak into translucent scenery or the
  Physics Lab poison volume. Their own fading and silhouettes remain active.
- C-Up enters first person only when the stick is neutral at the press and
  stays neutral until entry. Steering before entry cancels the request instead
  of entering later after Mario stops. Once Mario enters the first-person action,
  stick movement looks around without exiting, including during the zoom-in.
  The C-Up camera sound plays once when the camera actually begins zooming in,
  not when C-Up is pressed or while entry is pending. Leaving first person plays the
  C-Up unzoom sound once on camera-button or forced exits; canceling a
  pending request before entry does not play the unzoom sound.
- A fresh C-Up press that cannot enter first person plays the camera error buzz
  once when no other C button or R is held. Holding C-Up does not repeat the
  buzz. Valid zoom-in and first-person exit presses do not buzz; dialogue and
  cutscene input locks retain their control over the camera.
- The camera HUD arrow reflects the active camera mode after camera input is
  processed. A pending C-Up request does not show the up arrow; C-Down can show
  its down arrow during that wait. First-person shows only the up arrow, and
  cutscenes show neither arrow.
- HackerSM64's camera proximity opacity for flagged objects, mainly trees,
  fades from fully opaque at 1,024 to fully transparent at 320 camera-depth
  units. Ease both ends of the fade while keeping the existing object selection.
- Respawn camera initialization resets the actual/default Mario and Lakitu modes,
  selection state and R-tap history together, including same-area respawns.
- Mounting a ground shell clears any expiry blink immediately, and ridden shells
  stop running the unused shell blink/expiry routine. Held underwater shells
  follow Mario in world space as well as the rendered hand transform, so distance
  and shadow queries no longer use their spawn point. Underwater shell swimming
  has no timed expiry: travel distance and elapsed riding time never release it.
  Throwing, damage, surfacing and cap handoffs retain their existing behavior.
- A Koopa shell has a silhouette only while Mario rides or holds that shell.
  Unattended ordinary and underwater shells do not; other silhouette objects
  retain their existing flag behavior.
- Yellow, blue and red coin pickups emit two sparkles, one per update, each lasting
  five updates. Floating-number effects retain their original counts and
  lifetimes. Continuous 1-up sparkle emission targets two live
  sparkles per mushroom with five-update lifetimes; stationary 1-ups remain
  particle-free. Existing models, random placement and scale are retained.
- Ordinary shell sparkle emission targets seven live sparkles per shell during
  continuous idle or riding emission, retaining their nine-update animation,
  random placement and scale. Water trails and lava flames are unchanged.
- Active whirlpools request 30 environment bubbles instead of 60. Bubble size,
  spiral motion, visibility range, water sound and whirlpool interaction remain
  unchanged. JRB jets use 30 particle slots with randomized 19-24 active-update
  lifetimes, 0-6 update startup delays and 0-3 update respawn delays. Waiting
  particles are hidden; their motion, size, current, sound and star behavior remain unchanged.
  Other levels retain their native jet particle count and lifetime.
- Ground-pound impact mist emits eight evenly spaced puffs on a fixed 35-unit
  circle, with equal outward speed and scale, lasting fifteen updates. Its star
  burst emits six evenly spaced stars. Particle-budget limits still apply;
  unrelated mist and the separate underwater impulse ring retain their effects.
- Red and blue coins, 1-ups, and collectible stars have a wall-visible
  silhouette within an oval range of 600 horizontal and 300 vertical units
  from Mario. It reaches 127/255 opacity (just under 50%) within the inner 20%
  of that range and fades to zero at its edge. Red coins have a soft red tint,
  blue coins blue, 1-ups green, and stars yellow. Their ordinary models and collection
  behavior stay intact.
  Collectibles draw once in the silhouette pass and once in their ordinary
  layer. Silhouette copies never redraw as ordinary models. The ordinary solid
  draw uses the existing silhouette-covering coverage mode and restores it
  before the next object. Solid 1-ups, including the poison mushroom, use a 128/255 texture-alpha cutout with
  full pixel coverage and no polygon antialiasing, retaining depth checks and
  writes at every distance. All 1-up sprite passes use point sampling to keep
  filtered edge colours out of the visible texels, restoring bilinear filtering
  before the next draw. Their cutout settings reset before the next draw;
  dim draws keep their 20% blend and texture cutouts.
  The green 1-up wall copy also uses the 128/255 texture-alpha cutout and full
  pixel coverage, blending its existing fog tint/opacity without polygon AA or
  coverage-overlap gating. It remains visible through walls and restores the
  layer mode and alpha comparison before the next draw.
  Coin and 1-up silhouette copies use a shared 15/16 scale in the sprite plane
  to inset their filtered edges inside the visible sprite. Their centers, depth,
  textures and ordinary sprite sizes stay unchanged; stars retain their
  existing silhouette sizes. Mario uses the separate body inset above.

## Mario's shadow

- Mario's floor shadow, including his mirror image, and his front projected
  shadow never draw on death-plane surfaces. Water above a death plane retains
  its normal shadow. Ordinary and held-object shadows keep their existing rules.
- Shared shadow materials disable polygon antialiasing while drawing, then restore
  it for the next draw. Filtered texture alpha keeps the edges soft without the
  uneven coverage fringe at native resolution. This includes Mario and power-box
  shadows; textures, size, opacity, render order and depth policy stay unchanged.
- Mario's shadow, including his mirror image, has an offset along the receiving
  surface's normal: 2 units through 1,000 camera-depth units, increasing smoothly
  to 6 units at 4,000 and beyond. Without a camera it retains the 2-unit offset.
  It draws in the final shadow layer with decal depth testing. Floors, decals and
  transparent surfaces draw first; closer opaque geometry can still hide it. Shadow size, fading,
  animation rules and surface selection are unchanged. Held-object and other
  object shadows retain their usual rendering.

## Object culling

- Project64, Parallel Launcher and Mupen retain ordinary object camera culling
  with a 4:3 projection. Their widescreen projection retains the compatibility
  bypass; depth protection still applies in both modes. Use the active projection,
  including screens that temporarily use 4:3, rather than only the saved setting.
  Behavior, collision and animation update policies are unchanged. External
  emulator widescreen enhancements must be disabled when using the game's 4:3 mode.
- Vertical object culling covers fixed, shadow-free white/sand/pebble particles,
  liquid droplets, and audited bubble/purple-marble, sparkle and explosion frame
  switches. It rejects only graphics wholly beyond the top or bottom bounds,
  using conservative bounds containing every drawing frame and absolute scale.
  Valid frame selection is preserved; invalid frame states use the native callback
  and its reset. Camera-position calculations are shared with subsequent drawing.
  Unknown layouts, bones, shadows, attachments, custom transforms, silhouettes,
  debug hitboxes and screen roll retain their current rendering. The emulator
  widescreen bypass still applies; gameplay and collision updates remain live.
- JRB area 1 also vertically culls native, shadow-free seaweed and clams using
  conservative 300- and 448-unit bounds before building their bone matrices.
  Native animation clocks and sound positions still advance. Attachments, unknown
  animations, custom transforms, silhouettes, screen roll and unsupported
  projection contexts retain ordinary drawing.
- Distant native seaweed whose conservative bounds fit within 32 screen pixels
  at camera depth greater than 6,000 keeps its animated base and draws its three
  upper segments together in a fixed native pose. All four textures, transparency
  and triangles remain. Nearby, large, attached and unknown-animation plants use
  the original fully animated mesh; animation clocks continue in both forms.
- JRB area 1's five opaque terrain lists use 13 coarse RSP bounding boxes.
  Bounds enclose every loaded vertex; rejected sections skip only geometry.
  Original materials, triangle order, water, mist and transparent terrain remain
  intact, with lighting restored before a section can return early.
- JRB area 1's dry treasure cave and its objects draw only when Mario or the camera
  approaches the underwater tunnel or is inside the cave. The tunnel itself stays
  visible, and the cave turns on before its mouth. Leaving turns it off once both
  Mario and the camera are outside the approach. Collision, behaviors, animation
  clocks and sound positions keep updating; area 2's ship interior is unchanged.
- JRB area 1 also rejects native seaweed and static rocks wholly hidden behind
  three audited opaque shoreline/seabed triangles. Conservative model spheres
  include every native pose; partial cover, attachments, custom transforms,
  unknown animations and debug hitboxes retain drawing. Geometry checks preserve
  gameplay, animation clocks and camera-relative sound positions.
- Deep inside JRB's treasure cave, the main bay terrain and its objects stop
  drawing only when both Mario and the camera are inside and a padded tunnel-exit
  box is wholly outside the camera view. Turning toward the exit restores the
  bay. The tunnel, cave, water animation and skybox retain their normal paths.
- Within JRB's central cave/tunnel camera corridor, the bay's three opaque
  terrain roots omit ten fully hidden sections in the cave and six in the tunnel.
  Complete portal separation preserves materials and retained geometry; the
  native floor/ceiling envelope limits activation. Full terrain returns at every
  region boundary and before the exit, or for unsupported rendering contexts.
  The existing whole-bay gate, objects, collision, water and skybox are unchanged.
- Deep JRB seaweed bundles below or at Y=-4000 use three independently animated
  leaves. Shallow JRB bundles and every other level retain four.
- The two fixed sunken-ship collision meshes reuse retained geometry and cell
  membership. Range, room and time-stop gates, warp faces and native list order
  remain intact. Capacity or duplicate-publication failures use the ordinary
  loader; the rocking ship keeps its existing loader. Enabling the optional
  general mesh cache does not retry or duplicate a successful ship publication.
- JRB opaque terrain batches consecutive triangles into the native 32-vertex
  cache. Vertex attributes, primitive order, materials, fog, section bounds and
  collision remain unchanged.
- Native JRB area 1 ship parts retain full detail below camera depth 7,500,
  use a lightly simplified mesh from 7,500, the distant mesh from 10,000, and
  the coarsest mesh from 20,000. Thresholds scale upward for enlarged objects;
  zoomed cameras, attachments and unsupported contexts keep full detail. All
  parts switch together. Rocking, collision, opacity, materials and texture
  references remain native. Distant meshes use ordered 32-entry vertex batches.
  Full-detail meshes share identical vertices in 32-entry batches. Ordered triangles,
  UVs and normals/colors stay native; tiny edge/shading rounding differences are
  accepted for the measured graphics saving.
- Closed ordinary polished wooden doors may select their existing distant mesh from
  camera depth 2,500 when their projected bounds fit within 40 screen pixels.
  Opening doors, other door families and signs retain their ranges. AUTO_LOD's
  emulator policy, door animation, collision and interaction remain unchanged.
- Ordinary circle/square shadows bounded to at most 16 screen pixels use 32x32
  quarter textures. Larger, player/mirror, held and unusual shadows retain HD
  textures, as do unsupported projection contexts and optional texture reuse.
  Floor resolution, shape, scale, opacity, priority and depth policy stay native.
- Tree vertical culling and separate regular-shadow footprint culling are enabled
  by default in the base game (`TREE_VERTICAL_CULLING`, `SHADOW_VIEW_CULLING`).
  Trees use all opaque/transparent mesh
  alternatives within their scaled 800-unit bound and preserve native transparency
  and frame-selection callbacks whenever ordinary traversal would reach them.
  Shadow rejection follows native floor, water/lava/carpet, slope, size and opacity
  resolution, and leaves body-child drawing intact. Mario, mirrored Mario, held
  objects, unusual shadow forms and unsupported camera contexts retain fallback.
- With opt-in ORDINARY_SPRITE_BATCHING=1, white, cold-water and lava small
  droplets share ordered sprite batches. Normal object sprites draw individually.
  Each opt-in batch loads its shared texture once and stores each droplet's
  tint in its vertices. Dim-blinking, attached, animated and unusual-scale sprites retain
  their ordinary rendering fallback.

- Consecutive identical coin frames reuse their resident IA8 texture without
  reordering draws. Large water splashes use lossless CI8 texels and RGBA16
  palettes for all eight frames; consecutive identical splash frames reuse the
  palette while still uploading their texels. Unsupported materials draw directly;
  reuse tracking stores only changed state. Other materials, layers, blinking,
  silhouette passes and unsupported owners retain full uploads. Colors, alpha,
  filtering, geometry, animation and draw order remain unchanged.

- Consecutive identical mist, sparkle and small splash frames reuse their
  resident texture. Each particle still applies its own opacity or liquid tint.
  Other drawing, frame changes, layer boundaries, dim blinking and silhouette
  passes end reuse; geometry, animation and draw order stay unchanged.

## Aquarium rendering

- JRB mission 1 performs no jet-stream bubble rendering setup. Later missions
  skip this setup while the emitter requests zero bubbles, retaining active
  emission and normal area cleanup. Initial mist performs no drawing allocation
  outside mission 1 or while the camera is underwater. Mission objectives,
  hazards, collectibles and shared scenery retain their existing availability.
- Jolly Roger Bay may omit terrain, water quads and jet-stream bubble draw
  groups only when their padded bounds are wholly outside the camera view.
  Bubble simulation and water texture phase continue while hidden. Offscreen
  object animation clocks retain their native update and pause behavior.
  Camera-edge geometry, uncertain bounds and debug hitbox views retain drawing.
- Castle lobby tanks spawn six fish each, for 24 across the four tanks. Room-based
  spawning/despawning, swimming, animation and tank geometry remain unchanged.
- Secret Aquarium fish and bubbles keep their existing counts, spawning, movement,
  animation clocks and lifetimes. Levels and their geometry are unchanged.
- Shadow-free aquarium decorations may skip drawing outside conservative
  horizontal and vertical camera bounds, including on emulators. Shadowed
  objects retain the existing culling policy; screen roll retains depth-only
  decoration culling.
- With opt-in ORDINARY_SPRITE_BATCHING=1, consecutive ordinary aquarium bubble
  sprites share drawing work in groups of up to 16. Preserve the sprite texture,
  UVs, color, layer and draw order, using
  1/16-unit vertex precision. Custom geometry/transforms and debug hitboxes use
  the ordinary renderer. Split batches before coordinate overflow. Compact
  explosion particles retain their dedicated batch renderer in normal builds.
- Objects without Metal-contact or stun state use their normal update directly.
  Active enemy state still advances only on eligible object updates, including
  the 15-update Metal grace period and time-stop rules.
- Scene transforms retain their normal float calculations and animation updates.
  Prepare a fixed drawing matrix only on the first actual draw at a stack entry;
  reuse it for that unchanged entry and invalidate it when a sibling reuses the
  depth. A visible aquarium fish needs two fixed matrices, for body and tail,
  rather than five including its transform-only nodes. Drawn transforms remain
  identical to the previous renderer.

## Dialogue

- Below the current obtainable total of 120 stars, Yoshi's rooftop spawn is a
  sign reading "Sorry, Mario. I can't give credit.\nCome back when you're a
  little...\nMmmmm...\nRicher!", with a separate dialogue box for each phrase.
  Debug level select, UNLOCK_ALL and COMPLETE_SAVE_FILE use the completed-star
  Yoshi path without changing the save's star count.
  Closing this sign spawns a purple poison mushroom with the normal 1-Up
  emergence motion, appearance sound and sparkles, then the mushroom homes
  toward Mario using the normal pole 1-Up steering and collision movement.
  The sign fades away with a castle warp sound. Mushroom contact
  kills Mario, clears queued healing and forces the ordinary Game Over route
  regardless of lives or debug level select, without the Yoshi coin cake or
  HE THICC unlock.
  Poison-mushroom death permanently enables the castle rooftop Wing Cap box
  for the current save. Leaving the area by any route after the sign has fully
  faded enables that box too; leaving before the fade finishes does not.
  The completed fade moves the sign permanently into a hidden room behind
  the upper castle window. Yoshi still requires full star completion.
  This local unlock does not activate Wing Cap boxes in other levels or change
  the star count. The three regular rooftop 1-Ups require full star completion
  and secret eligibility. If cheating disqualifies the visit after they spawn,
  remove them before pickup; a clean course restart restores eligibility.
  Ordinary 1-Ups retain their normal pickup behavior.
- Castle-secret progress does not share cannon-unlock bits. Older saves retain
  their cannon unlocks and rooftop progress evidenced by the local Wing unlock
  or relocated sign's introduction. An ambiguous old pool-floor-sign marker is
  ignored, so that sign can be read once again after upgrading.
- If Mario breaks the window while the credit sign is still on the roof, its
  next read uses the ten-page angry intruder dialogue. After closing, it jumps
  and tips forward like a Whomp while Mario is locked in place, then kills him regardless
  of caps or invincibility. The normal cake screen follows, then Game Over.
  A 60-update fallback applies the same lethal outcome if the slam misses its
  impact state. The movement lock is released on death. The sign disappears,
  relocates to its room, and unlocks the castle Wing Cap box for that save,
  like the poison-mushroom route. Progress is flushed on level exit/Game Over.
- The intact upper castle window remains visible in distant castle views; its
  object draw range is 30,000 units so the opening stays concealed until broken.
- Bonking the upper castle window shatters it and reveals a small room with a
  portrait, even before Mario reads the rooftop poison sign.
  Without the room sign, it uses the supplied ordinary Lady Bow image. With
  the relocated sign, it uses smug Lady Bow; after the sign steps out, Lady
  Bow disappears while the frame and dark purple portrait backing remain. Full star completion takes priority and uses
  the existing Physics Lab painting, including debug completion.
  The room sign appears only after the rooftop sign has faded and relocated;
  full star completion also makes it appear, including debug completion.
  Completed saves retain Yoshi outside and use the nine-box
  "SUPER PLAYER" room message in preference to all encounter dialogue stages.
  Reading or pounding this completion sign does not advance encounter progress.
  Lil Board renders fully opaque until its fade begins, then uses transparency.
  After that dialogue closes, Lil Board fades away and stays absent across
  area/level exits until a fresh game boot or reload of that save file. This
  dismissal is temporary and never written to SRAM. Other save files retain
  their own sign availability.
  The room sign uses ordinary wooden-sign sinking in every dialogue state:
  two distinct ground pounds lower it 30 units each over five updates, up to
  60 units total, with the shared collision, reading-height and impact sound
  behavior. Sinking resets when the room objects reload.
  With full completion, hopping into the Physics Lab painting from inside the
  revealed room warps to Physics Lab area 1 at its normal entrance. It uses
  the native castle painting mesh, proximity/entry ripples, special floor
  regions and delayed white painting transition. Its entry camera targets the
  portrait in world space, with floor clearance below the overhead roof. Its
  camera approach stays inside the room and discards exterior camera smoothing.
  The room's visibility bound encloses its walls and portrait, keeping them drawn
  when the painting-entry camera moves behind the room object's doorway origin. The portrait
  cannot trigger a warp before the window breaks or from outside the tower.
  The room and every object inside remain unspawned and invisible until the
  window breaks. The window resets to unbroken on every area entry; the room
  and its contents are hidden again until Mario breaks it on that visit.
  The relocated sign first says "Please don't tell anyone how I live.", then
  cannot be read. One ground pound enables "Ow." once; another, after reading
  "Ow.", enables "Why?" once. Each line disables reading again when closed.
  Further pounds do not repeat the dialogue. Leaving the area by any means
  after reading "Please don't tell anyone how I live." permanently replaces the room dialogue with
  "Stepped out for a bit! - Ms. Board". This note can be read repeatedly and
  further ground pounds never change it. The note persists per save.
  Leaving before reading the hidden-room introduction keeps it
  available on the next visit. Older saves with a premature departure note also
  require reading the introduction and then leaving again.
  Encounter changes stay in RAM during play. SRAM writes are deferred until
  level teardown or Game Over, before the saved backup is restored. Same-level
  area changes update the note without writing SRAM.
  The original poison encounter,
  Game Over route, Wing Cap unlock and regular rooftop 1-Up gates are preserved.
- Rooftop Yoshi wears angular black sunglasses with white lens highlights and a
  black tank top with a scoop neckline, muted brown trim and two pale chest
  accents suggesting the reference's white print without lettering. The tank
  ends at the saddle, with the lower body and hips exposed.
  His dialogue is "GTA VI came out before Return to Yoshi's
  Island 64." After the dialogue he throws one collectible yellow coin toward
  Mario, pauses in place for 45 updates (1.5 seconds), then walks and jumps off
  the roof through his existing departure.
  Collecting his coin adds one coin but costs one health wedge instead of
  healing. Ordinary coins retain their usual healing.
  If that wedge makes Mario's pending damage fatal, the death transition leads
  to the ending cake, then the usual game-over Mario head and file-select route.
  The game-over departure reloads the saved file. Healing that
  saves Mario cancels this trigger; other deaths do not show the cake.
  This cake visit plays the cropped "I lost the run to RNG" clip in place of
  normal ending audio and disables the pause menu. The usual cake visits retain
  their normal audio and pause menu; departure waits for the clip to finish.
  This replaces the lives and special-triple-jump reward. His roaming, credits
  pose and departed-until-reset behavior remain unchanged.
- Yoshi's walking footsteps play only within 1,000 units of Mario, including
  exactly 1,000, during both roaming and departure.
- Yoshi's talk-start puzzle jingle plays once on conversation entry, including
  when he already faces Mario. Turning and waiting for Mario to face him do not
  restart it; a new conversation can play it again.

- Holding A through a Wing, Metal or Vanish Cap switch prompt automatically
  chooses Yes on its final save page. A fresh A/B press still confirms the
  selected response, including No; holding Z retains backward-page priority.
- A Bob-omb Buddy plays the talk-start sound once when it begins turning toward
  Mario, rather than replaying it every update until the turn finishes.
- Ordinary freestanding wooden signs sink in two stages when ground-pounded
  on top: 30 units per distinct impact, up to 60 units total. Each stage lowers
  the model and collision over five updates and plays the wooden-post impact
  sound. Held landing contact does not count again. The sign remains readable
  at either depth and stays buried until the area's objects reload. Wall signs
  and the special Toad-room sign retain their existing behavior.
- Starting sign or NPC dialogue clears horizontal and slide velocity once.
  Dialogue alignment and vertical/platform support retain their existing rules.
- Reading any sign with Mario's feet below the water surface requires Metal or
  Vanish power and ground contact. The normal facing, movement and B-button
  conditions still apply. Dry signs and NPC dialogue retain their existing rules.
- The Physics Lab's hidden underwater shaft sign sits in the rear corner,
  facing diagonally inward to keep its reading camera clear of the walls.
  Its model and B prompt are hidden beyond 150 units from Mario and visible
  at or within 150 units. Its normal solid collision and sinking remain active.
- Sign, NPC, automatic and cap-switch dialogue block manual camera input.
  Buttons held through dialogue must be released before becoming usable again;
  releases during dialogue count. Scripted camera movement remains active.

## Purple floor switches

- A qualified step onto a purple floor switch activates it once. Mario must
  step off before another press can activate it again; remaining on the switch
  does not refresh the effect.
- Bowser's Castle stairs and other switch animated structures continue their
  own countdown after Mario leaves the switch. A later press refreshes their
  timer without collapsing an already open structure. Standing on the switch
  is not required to keep the structure active.
- Hidden boxes remain active for their timed appearance after Mario steps off.
  A new press refreshes visible boxes and restores broken boxes; merely
  releasing the switch does not restore them. Grill gates stay open once
  activated.
- Switch-spawned breakable boxes and hidden platforms blink on alternating
  updates for the final three seconds (90 gameplay updates) of their existing
  lifetime. The switch's warning ticking uses the same three-second window.
  Invisible warning frames remain solid; expiry and breaking remove collision
  immediately. A new press restores visibility and resets the countdown.

## Moving 1-ups

- Walking, fleeing, sliding, and revealed 1-ups check wall and floor contact
  at their final position after each movement update. If a wall turn or slope
  changes their path, they are pushed out of walls and raised onto a nearby
  floor instead of remaining embedded. Collection and appearance timing stay
  the same.
- A sliding 1-up awards its pickup and feedback once, including while it
  flickers before disappearing. Its movement and disappearance timing are unchanged.

## Dab

- An accepted dab shortcut plays Mario's voice immediately on action entry.
  Each accepted repeat plays it once again; rejected chords remain silent.
  The initial wind-up and repeat animation starting frame remain unchanged.
- After the normal wind-up, holding both L and Z gives inverse analogue dab
  control: neutral holds the full dab, and forward stick pressure unwinds toward
  the relaxed wind-up pose. Forward means toward Mario's facing direction;
  partial pressure selects intermediate poses without turning or accelerating.
  This does not repeat the voice or increment the dab/sleep count. Releasing Z
  with a neutral stick settles into held breathing; releasing both buttons
  settles into recovery. While L+Z remains held, stick movement, A/B and C-Up
  cannot cancel dabbing, including during wind-up. Sideways/backward stick holds
  the full dab. Releasing either button restores ordinary voluntary controls;
  damage and forced exits still apply.
- Waking from seated or lying sleep plays at 50% speed when no cap flags are
  present at wake entry, with 40 recovery updates instead of the capped 20.
  Cap changes during recovery do not alter its pace. Stomps, floor loss and
  sliding still interrupt immediately; snore/music cleanup happens once.
  Earlier capless sleep entry remains unchanged.
- Falling asleep after repeated dabs ignores A, B, Z, stick, and C-Up inputs
  for 110 gameplay updates from sleep entry. The remaining lock carries into
  the sleeping action if the animation finishes early. Damage and forced floor
  exits still apply; ordinary idle sleep keeps its usual controls.
- After five completed updates holding L during a dab, a damaging enemy hit
  can knock off Mario's ordinary cap. Releasing L ends that vulnerability.

## Star select

- D-pad left/right and the analog stick both navigate selectable stars, one
  step per press or tilt. Holding does not repeat; release or reversal permits
  another step. Existing selectable-star filtering and endpoint limits remain.
- Holding A through the star-select screen confirms the currently highlighted
  selectable star once the menu's opening delay ends. A fresh press after the
  delay selects that same highlighted star.

## Code map for movement and combat

- `mario_step.c` owns ground and air movement, slope-aware step counts, gravity,
  and the response after each collision check. `mario_collision.c` owns Mario's
  wall corridor, cached wall queries, and crossed-wall fallback.
- `mario_actions_stationary.c`, `mario_actions_moving.c`,
  `mario_actions_airborne.c`, and `mario_actions_automatic.c` own their action
  transitions, including crouching, crawling, jumping, and pole movement.
- `mario_actions_submerged.c` owns swimming controls and stroke speed response;
  `mario_water_collision.c` owns water substeps, surface crossing checks, and
  stroke contact projection. `mario_actions_object.c` owns grabs and throws.
- `mario_punch.c` owns the punch and breakdance sequence and its drag and
  lunge. `interaction.c` owns object, wall, bully, cap, and damage contacts.

## Focused verification

Run from the repository root:

```sh
python3 tests/run_regressions.py
```

Requires Python 3 and a host C compiler (`cc`, or the `CC` environment variable).
It compiles temporary host test executables, not a ROM. Use `--suite punch` for
just the punch cases, or `--case punch.standing.pairs` to rerun one failure.
See [the regression guide](tests/README.md) for diagnostics, coverage, and the
animation-resumption and sound-cancellation suites. The runner extracts the
actual punch updater, standing/moving action handlers, action setter, idle
cancels, grab eligibility, and hit-record functions. Animation bounds come from
the current animation assets. There is no separate copy of the punch algorithm
in the tests.

The checks cover multiple repeat cycles, recovery timing, release, interruption,
fresh sequences, cancellation, lunge strength/eligibility, and hit-record resets.
Use `--source-root /path/to/older/checkout` to run the same checks against an older
implementation. The prior committed implementation passes the single-repeat
checks but fails repeated pairs; the subsequent generic-buffer rewrite fails
both repeat modes.

Most engine services are stubbed. The swimming suite additionally exercises
actual wall/floor/ceiling queries and raycasts against synthetic triangles and
Jolly Roger Bay static triangles, plus the common swimming movement and pitch
response; use `--suite swimming` for those cases. It does not run level loading,
moving-object updates, or action dispatch. These checks do not validate complete level geometry, rendering,
sound playback, target performance, or gameplay feel. A ROM playtest should
check all three input sequences while standing and moving, then release/cancel
them, repeat hits on an enemy/wall, and grab an object during a repeated lunge.
Follow the project's build policy in `AGENTS.md`.

### Character palettes, appearance and text

- HE THICC has baked fuller body meshes at every detail level, including Simple
  Mario, Metal and transparent variants. In bone-local coordinates, torso scale
  is (1.00, 1.22, 1.18), butt (1.05, 1.25, 1.22), and thighs (1.00, 1.22, 1.25).
  Buttons follow the torso and normals are corrected for the changed proportions.
  Bones, joint positions, animations, collision and movement stay unchanged;
  Standard Mario retains the original meshes. Regenerate these assets with
  `tools/generate_he_thicc_body.py`; topology and texture coordinates are preserved.

- Standard Mario is the default. Dying from shallow-water health drain while
  dabbing in the L is Real courtyard fountain forces a game over and earns a
  pending HE THICC unlock. Starting a fresh File C claims it and plays the entire
  Peach intro, even with debug level/file-select/intro skips enabled. An occupied
  C keeps its progress and the unlock waits until it is erased and started fresh.
  HE THICC is saved only on C, named HE THICC in file select, until C is deleted.
  Copying a save cannot transfer the unlock to another slot. An interrupted intro
  restarts on the next C entry; its pending flag clears only after the finale.
- File select and the pause menu also accept a buttons-only TAS shortcut: alternate
  C-Left + C-Right and C-Up + C-Down for one game input update each, 32 pairs
  (64 consecutive updates), with no other buttons. Stick position is ignored.
  Gaps, held/repeated chords and extra buttons break the sequence; a first chord
  can begin a fresh attempt immediately. Each menu owns its progress; leaving
  pause or reopening file select clears it. Ordinary menu controls remain active.
  Completion converts File C to HE THICC in place, preserving its stars, coins
  and other progress, and immediately starts C at the castle grounds front
  entrance without the Peach intro. This works even from the grounds pause menu.
  The shortcut save is named TAS THICC throughout file select and its score
  pages; fountain-unlocked saves retain HE THICC. The TAS name persists with C,
  is removed by erasing C, and cannot be copied into another slot.
- The fountain secret requires every ordinary courtyard Boo to be defeated if
  present. Invisible, distant and bounced/stunned Boos still count as alive.
  A fatal hit counts as defeated during the death animation. The large courtyard
  cage carrier must be present, alive and visible; he must not be defeated.
  Visible uses Boo's own appearing threshold (opacity above 180 with target255),
  so a vanished or disappearing Boo does not count. A missing,
  dying or deleted cage carrier blocks the secret, including before the Boos
  initially spawn. Other Big Boo types cannot substitute for him. Re-entering
  and respawning ordinary Boos requires clearing them again before the fatal
  dab-drowning damage.
- Mario must already be capless when the fatal water damage occurs: no cap on
  his head or in his hand. The test precedes drowning damage's cap knock-off
  handling and the sink animation's cap loss. Those later losses cannot award
  the secret retroactively.
- File C's occupied-file face uses a turquoise cap, the model's pink T badge,
  matching peach skin and smooth purple eyebrows/hair/moustache, with fuller
  cheeks when that save has HE THICC active. The portrait is edited from a
  credited 256x128 Render96 source, keeping its rendered shading, then downscaled
  once to the game's 64x32 texture. Main, score, copy and erase
  menus select the icon from the save flag rather than the last gameplay
  palette. Ordinary saves and empty-file carvings keep their existing faces.
- The debug Character page offers normal Mario or the lowest-detail Mario mesh
  at every distance, within the current file's palette. Simple Mario and his
  mirror do not switch detail levels; held objects and ordinary appearances
  retain their existing AUTO_LOD policy. All four combinations share the
  existing skeleton and animation set. D-pad left/right switches appearance
  immediately, including during actions, without changing palette, movement,
  animation state or the fast-walk trial. The selected appearance survives normal
  respawns. Demos and credits temporarily
  use Standard, preserving the requested choice for the next ordinary spawn.
- HE THICC has a subtly deeper Mario voice (97% pitch), shared by both
  appearances. Existing distance/vibrato pitch variation remains; Standard Mario,
  Peach dialogue, music and other effects retain their pitch.
- Standard Mario's footsteps use 59.5% of their original volume, across terrain,
  tiptoeing, quicksand, Metal and underwater Metal steps. Both appearances share
  the reduction; terrain differences, cadence, pitch and particles remain native.
  Standard Mario's jump and landing effects use 85% volume, including terrain,
  heavy landings, Metal, underwater Metal and water jumps. Gains apply only to
  Mario's sound source. HE THICC, NPC sounds, hanging hands, voices and music
  keep their existing volume.
- Ordinary jump/freefall landing sounds and terrain particles trigger at floor
  contact, before remaining ground travel or recovery cancels. The landing action's
  sound flag prevents replay on its next update. Damage and burial keep their own
  impact effects. Jump effects bring their second foot sound forward by three
  tatums (about 31 ms at 120 BPM), including terrain, shallow water and Metal;
  pitch, volume, voices, movement and animation timing remain unchanged.
- Footstep samples trim the quiet opening near a zero crossing: Metal 20.938 ms,
  snow 9.250 ms, spooky floor 1.375 ms, sand 1.188 ms and shallow-water splash
  3.312 ms at their native sample rates. Each trimmed asset starts with a 0.5 ms
  fade to avoid a hard cut, retaining the rest of the waveform, peak, sample rate
  and ADPCM codebook. The sound banks use separate `_trimmed` assets; extracted
  originals remain available. Shared walking/NPC steps and splash effects using
  these samples also start sooner. Immediate-onset default, grass, stone and ice
  samples retain their native openings.
- Nine additional effects trim their quiet openings using a -40 dBFS threshold;
  four voices and five instruments use -50 dBFS, measured in approximately 1 ms
  RMS windows. Cuts retain at least 3 ms before the detected onset and use a
  0.5 ms fade-in. Separate `_trimmed` assets preserve the originals, native sample
  rates, ADPCM codebooks and remaining waveforms. Instrument loop markers shift
  by the removed frames, preserving loop lengths; bank tuning and envelopes stay
  unchanged.
- HE THICC's low melodic music notes receive a 10% volume boost (about
  0.83 dB) at or below the octave below middle C. Sample tuning is accounted for;
  pitch, tempo, percussion, sound effects and voices retain their existing behavior.
  Both appearances share the mix; Standard, demos and credits use the normal mix.
  Active-note gain is derived fresh and release tails preserve it without accumulation.
- Standard Mario and HE THICC share the same custom movement profile: walking
  target 40, acceleration 1.375, jump height scale 1.05, retention 0.9, and a
  0.20 speed contribution for ordinary/double/unladen Metal-water jumps. Both
  retain the full custom moveset; HE THICC has no vanilla-style movement override.
  The profile only replaces the prior walking and jump-tuning sites; other custom
  mechanics remain in place.
  Model detail has no effect on those properties. The 127 fast-walk trial still
  works and clears only when committing a different ordinary character choice.
- HE THICC carries a dab pose through his special moves: the head, shoulders, arms and
  hands start from dab frame 7, with movement variations described below, while
  the current move keeps its root, hips and leg motion. Gameplay animation clocks,
  collision and attack timing stay native.
  Both appearances, Metal/transparent variants and mirror Mario share the pose;
  unrelated/held objects and Standard Mario keep their own animation data.
  During inverse analogue dabbing and its release settling, his upper body uses
  the selected dab pose so that stick control remains visible.
- HE THICC has a damped jiggle spring driven by changes in vertical speed and clap
  impulses. It adds bounded squash/stretch and hip/torso sway once per gameplay
  update; extra render passes and pause do not advance it. Native damage/crushing
  squash owns its scale, and spawning or cutscenes clear the spring.
  Ground travel claps every 120 units, with a six-update hip/thigh squeeze;
  running uses its animation-timed bass footfalls instead. Heavy
  landings, wavedashes, timed bounces and real wall kicks also clap. The clap uses
  a separate details sound channel so footsteps and impact audio can play together.
- While descending within 160 units of a safe floor, fresh Z plus a stick direction
  starts a diagonal-down wavedash: 32 downward speed, at least 48 horizontal speed,
  incoming absolute speed plus 16, capped at 100. A/B and L chords retain priority;
  neutral Z keeps the ordinary ground pound. Carrying/riding, damage reactions,
  water, lava, quicksand, death planes, warps and steep floors cannot start it.
  Collision remains native; walls bonk, lava walls boost, and floor contact becomes
  a fast belly slide with jiggle and clap feedback.
- An unladen forward dive (`ACT_DIVE`, positive forward speed) descending at least
  600 units from its peak and landing at downward speed 55+ on a safe floor becomes
  a heavy belly slide instead of fall damage/burial. Other air moves and backward
  dives retain their native landing and fall-damage checks. Minimum slide
  speed is 56; retained absolute horizontal speed gains one quarter of downward
  speed, capped at 100. Contact
  produces the existing strong shake/AOE stun wave once. Held/ridden objects,
  hazardous/wet/steep floors, deliberate pounds and enemy/scripted knockback keep
  their native reactions. Native slide friction, steering and wall/ledge response
  continue to run. A fresh A during the first six completed slide updates launches
  a forward rollout at 48 upward speed and at least 80 forward speed (current speed
  plus 24, capped at 100). Late A or B uses ordinary rollout; held A adds no boost.
- HE THICC's deliberate ground-pound descent starts each update at 100 downward
  speed, twice the ordinary 50, retaining submerged Metal resistance and normal
  startup collision/timing. The existing 800-unit AOE stun and strong screen shake
  occur once at floor impact. A fresh A during its first six landing updates gives
  a big bounce: triple-jump movement at 84 upward speed and at least 48 forward
  speed, with clap, jiggle and stars. Stomps/floor loss retain priority.
  The repeated ground-pound request is one stronger move, with one impact event.
- Real HE THICC wall kicks launch in a seated butt-first pose with a clap and jiggle
  impulse. Wall-contact eligibility, launch physics and chain slowdown stay native;
  pole jumps sharing the same action retain their ordinary animation.
- HE THICC uses graded rumble throughout active moves, including airborne travel
  and free swimming. Rising jumps use 4/8 duty, falling 1/8, walking/crawling/climbing
  2/8, running at absolute speed 24+ and swimming 3/8, punches/dab entry 4/8,
  twirl/slides/wavedashes/dives/heavy throws 5/8, landings and pound descent 6/8,
  and heavy ground impacts/bonks 8/8. Held dab/twirl poses soften to 1/8; idle/sleep,
  stationary attachment holds, first-person, cutscenes, demos and credits remain
  quiet. Existing impact effects may add stronger feedback; ordinary travel claps
  preserve light walking rumble. Pause/reset cancels the overlay and it expires
  after six video interrupts without gameplay refresh. Standard Mario's rumble
  stays unchanged; both HE THICC appearances share feedback.
- HE THICC adds a brief small camera shake for floor landings at downward travel
  speed 40+, bonks at absolute forward speed 32+, heavy object bounces and connected
  punches/kicks/trips/fast attacks. Ordinary travel does not refresh camera shake.
  Contact pitch/roll amplitudes are 0x40/0x20, decaying over eight updates.
  His ground-pound floor impact instead produces a much stronger pitch/roll shake
  (0x180/0xC0 amplitudes, 24 updates of decay) and an 800-unit horizontal stun radius,
  with a 200-unit vertical window around the floor (enemy hitboxes must overlap
  that window, measured from the hitbox bottom including its downward offset).
  Both ordinary ground pound and underwater down-burst use this one impact event.
  Eligible mobile enemies, including larger mobile variants, receive a small
  harmless hop and 60 updates of grounded stun via the existing stun lifecycle.
  A repeat impact refreshes a stun without restarting its flight or duplicating
  stars. Boos retain their native harmless bounce; scripted bosses/rooted immune
  enemies and racing Koopa remain unaffected. Walls, different rooms, inactive,
  intangible, held, already-attacked and committed-defeat objects are excluded.
  Direct ground-pound damage and ordinary Mario's original impact remain unchanged.
- Every actual light and severe fall-damage event triggers HE THICC's same ground-pound
  stun radius and strong screen shake at landing, without changing fall damage,
  squash, knockback, cap loss or held-object release. Falls prevented by water,
  Metal or spawn protection do not trigger this automatic wave. A deliberate
  ground pound that also causes fall damage emits its normal single shockwave.
- HE THICC's dry-ground, lava and water-surface slide-kick bounces each emit one
  existing 800-unit ground-pound stun wave, with the tuned impact shake. Ordinary
  settling after a bounce does not emit another wave; bounce physics and input
  queues remain unchanged.
- Each HE THICC punch, ground kick and jump kick emits one explosion on its first
  confirmed attack contact with an object or wall. Standing/moving punches and
  repeated singles/pairs receive fresh blasts for each connected strike. Misses,
  grabs, rejected attacks and breakdance do not emit blasts. Multiple targets and
  wall triangles share one blast per strike, including the jump kick's recovery.
  The native strike retains its normal damage; the blast adds the existing
  800-unit harmless enemy stun,
  explosion sound, mist, stars and clap. Its tagged Mario-owned visual explosion
  remains intangible, so it cannot hurt its owner or cause a second damage event.
  Native damage and recoil are resolved before the object-contact blast, so its
  stun cannot make that contact safe. Ordinary explosions retain their native damage.
  A full object pool can omit the visual explosion but still applies the stun
  and sound/particle feedback.
- Walking/running/tiptoe, idle, ordinary single/double/triple jumps, freefall and
  their landings retain native head/arm poses, including held-object variants,
  braking, shivering and sleep/wake clips. Jiggle and butt-clap deformation remain.
  Special-move dabs animate the head/arms with flips/long jumps,
  slides/dives/rollouts/pounds, swimming and crawling. Native root/leg motion and
  gameplay animation clocks are preserved. Mirrors and extra render passes sample
  the same pose. Inverse-stick dabbing keeps its controlled pose; tired/sleep
  animations keep their native head and arms so the yawn is visible.
- Wavedash entry adds sparkles and a spin sound; belly-slide landings add mist,
  dust and horizontal stars; timed belly launches and pound/slide-kick bounces add
  star feedback and a rising bass bounce sound. Actual wall kicks add stars.
  Bounce sounds use the general-effects channel alongside the details-channel clap.
- Running at absolute speed 24+ on supported ground adds short, low bass notes
  to each native animation-timed footfall. From speed 24 to 48, the added thump's
  gain rises smoothly from 25% to 75% of its previous level while its pitch falls
  from 135% to 110%; faster speeds retain that bounded mix. Slow running is lighter
  and faster running is bassier, with native terrain sounds and cadence preserved.
  Walking claps retain their existing cadence at lower speeds; running footfalls
  own that details channel. Quicksand deeper than 50 units, demos and credits do
  not add the running thump.
  Butt claps have priority 0xA0, above the running thump and more details-channel
  sounds; their pitch and volume remain unchanged.
- Each actual breaststroke, free or while holding an object, emits one expanding
  ring of white water particles at the stroke's sound phase. It uses the existing
  particle-budgeted ring effect, with no collectible/health/mission interactions.
  Interrupted strokes that do not reach that phase emit no ring.
- Away from the surface, HE THICC's swimming buoyancy has an extra 3 units of
  downward speed: normal idle settles at -5, level swimming at -3, and native
  Metal sinking at -21. Swimming upward still works; surface flotation and
  invulnerable water reactions keep their existing values.
  Submerged Metal/Vanish land actions also receive 25% stronger gravity through
  the shared gravity path, including jumps, slide kicks and twirls. Their native
  descent limits, short-hop rules and Metal's water time scale still apply.
- After more than three seconds of uninterrupted ordinary idle (91 gameplay
  updates), HE THICC begins the tired/yawn phase of falling asleep, with normal
  wake controls and no dab sleep-input lock. Both cap states and cold terrain
  follow this timing. Input restarts the wait; hazards, low-health panting,
  unstable floors and unsafe sleep geometry retain priority. Pause does not
  advance the wait. Standard Mario retains his existing sleep/shiver delays.
- While HE THICC is active and "Disable THICC text" is off, displayed occurrences of Mario's name become
  `HE THICC` in dialogue, non-pause menus, HUD labels and captions. Complete names match
  regardless of case, including possessives; source strings stay unchanged.
  Width calculations and dialogue page offsets use the expanded name. Peach's
  opening letter retains its original text, without HE THICC text changes.
- With "Disable THICC text" off, HE THICC changes displayed `ck` to `cc` and `se` to `ss`, preserving case, across signs,
  dialogue, non-pause menus, HUD and other game text. Pause, Options and Statistics
  use literal text on every file, including confirmation screens and width calculations.
  It uses the original previous letter
  (`ckk` becomes `cck`; `see` becomes `sse`), and removes word-final `d`/`D`
  (`red` becomes `re`; `END!` becomes `EN!`; `odd` becomes `od`). An apostrophe
  followed by a letter keeps the preceding d (`dad's` stays intact). It preserves
  colour controls and other tokens, and never
  rewrites the source strings or dialogue IDs. Each open dialogue keeps its
  opening text style; omitted glyphs compact displayed widths without
  changing dialogue page offsets. File select has no active character and uses
  literal slot names. PuppyPrint diagnostics remain literal.
- The old build-only palette option is replaced by runtime selection. HE THICC
  retains the supplied Vinesauce references and approved balanced preview:
  richer turquoise cap and overalls; purple hair; rose-pink shirt; warm peach
  skin; golden-orange gloves and darker warm-brown boots. Main materials use 45%
  ambient fill for deeper shadows; hair retains its matching half-brightness
  texture shading. All Mario model detail levels
  and the held/dropped cap use the same palette and a pink Mario-style T badge.
  The eyes use grey irises and dark-grey pupils across every open/half-closed
  expression, preserving white highlights and dark eye outlines. Eyebrows and
  moustache are matching purple. The hair mesh, shaded sideburn texture and
  shaded moustache use the same purple highlight/shadow range. Sideburns
  retain their baked detail, and both texture silhouettes stay unchanged.
  The moustache uses a broad, smooth highlight without a speckled upper rim.
  Closed/dead expressions retain their shapes
  with the same purple brows. Metal and wing materials keep their original
  colors. Standard uses the original palette and M badge. Enemy-held caps also
  follow the active palette. The Standard choice remains the default when debug
  controls are disabled; choices are not saved to SRAM.

### Audio optimization trial

- The power-up song (Wing Cap, Vanish Cap and shell variant) uses dedicated
  half-rate normal guitar, bass and normal/high string samples in bank 17,
  with exactly half the former tuning to preserve pitch. Its sequence,
  arrangement, note timing, attacks and held-note envelopes remain intact.
  Low strings, other instruments and percussion retain their original samples.
  Bank 17's guitar instrument uses release rate 24 and both string instruments
  use 18 for modestly shorter, smooth note-off fades. Bass and other releases
  retain their original rates. Other banks retain their original samples and
  release rates. These bank choices apply independently of the audio engine
  trial selector.
- Audio sequence channels group frequently accessed volume, pan, frequency,
  player and layer fields together before vibrato and instrument metadata.
  Channel storage size is unchanged. This layout is enabled for ongoing
  playtesting; sound preservation remains unverified after generated sample
  differences in matched captures.
- US/JP builds default to the accepted combined trial, `AUDIO_TRIAL=10`.
  EU/SH retain `AUDIO_TRIAL=0`. Explicit `AUDIO_TRIAL=0` selects normal audio,
  with the older quality and linear trials also off: 32 kHz output, original exponential envelopes, every sound tail and
  original US/JP/EU microcode. Shindou retains its region-specific microcode.
- `AUDIO_TRIAL=1` through `9` select independent experiments, each in its own
  `_audio_trial_N` directory. Trials 1–7 change only microcode instructions;
  trial 8 tests released-tail retirement at 32 kHz with original microcode;
  trial 9 tests 30 kHz alone with original envelopes, tails and microcode.
  `AUDIO_TRIAL=10` combines all nine in a separate US/JP build, with shared
  decoder arithmetic and separate dry/wet mixer loops that fit RSP instruction memory. It retains the quiet
  tail's final fade and uses the matching 30 kHz pause quote.
  US/JP pitch compensation caches the output-rate ratio at session initialization;
  note updates retain the same multiplication and normal-rate bypass.
  The numbered trials cannot be combined with older quality/linear experiments.
  Normal ROM paths, `play`/`aplay`, voice limits, sound triggers, trimmed samples
  and the accepted scheduler remain unchanged. See `tests/README.md` for the list.
- The tail trial retains held notes and starts, requires both current and target
  gains at most one Q1.15 step, and renders a final eight-sample fade before
  retirement on the next synthesis update. Normal note-list cleanup remains.
  Trial 9 uses the existing 30 kHz pause quote to retain its pitch and duration.
- The native descriptor backend is parked outside the checkout in
  `/path/to/workspace/audio-block-integration-2026-10-03/parked-2026-10-03`.
  The accepted trial-10 audio engine remains active.
- Normal US/JP builds use `AUDIO_LOW_LATENCY=1` and start audio processing after
  the current frame's sample DMAs
  complete, without waiting for the next VBlank. Playback still queues the oldest
  completed buffer at VBlank with the original buffering target. This removes
  one VBlank of pipeline latency when tasks meet their deadline. An overdue
  completion wakes the sound thread to queue that buffer without waiting another
  VBlank; it does not synthesize again. Rendering and queued buffers remain owned
  until their consumers finish, and busy-device/dispatch retries retain order.
  Explicit `AUDIO_LOW_LATENCY=0` restores the original two-VBlank path in a separate
  `_audio_buffered` build directory. EU/SH retain their native scheduling.
- US/JP use aligned fractional sample budgeting on detected Mupen emulators
  (`AUDIO_MUPEN_SAMPLE_CLOCK=1`). It follows the actual output frequency over
  60 refreshes per second, preserving the faster scheduler and existing buffer
  length limits. Only a successful preparation spends the budget; blocked
  preparation retains bounded demand. Frequency/counter resets and gaps longer
  than one second begin a fresh budget without an extra startup reserve.
  Console and other detected emulator classes retain their remaining-AI calculation.
  Explicit `AUDIO_MUPEN_SAMPLE_CLOCK=0` restores that calculation everywhere in a
  separate `_audio_clock_off` directory. The Mupen detection also includes older
  ParallelN64; playback evidence currently covers RMG 0.9.0, not every such core.
  JRB music, reverb, sound effects, fish and bubbles retain their accepted settings.
- US/JP music resets hold the load lock through sound-player and bank/reverb
  initialization, after preparation, dispatch retries and running audio tasks
  finish. Queued PCM is allowed to finish playback. Neither scheduler mode
  clears playback-owned buffers during a music reset.
- The sound-request queue retains up to 255 pending effects. Further requests
  are discarded until consumption makes room; overflow never erases the queued
  burst. Cancellation and source identity still apply to retained requests.
- US/JP/EU sample transfers use bounded batches of 64 accepted requests, draining
  completions before reusing descriptors. Rejected PI submissions are retried
  and are not counted as transfers requiring completion. The immediate task's
  final sample fence remains in place, with no additional VBlank delay.
- The opt-in quality trial uses 30 kHz, released-tail trimming, and optimized
  assembled microcode. The experimental linear mixer also uses assembled
  microcode. These quality experiments remain excluded from the normal ROM.
  Footstep animation triggers and accepted volume scaling stay unchanged.
- Host checks and a ROM build do not establish audible quality or emulator latency.

- Ground-pound windups draw smoothly across the existing startup duration,
  including the four updates formerly holding the completed spin pose. Underwater
  ground pounds draw between native animation frames at their existing 0.75x rate.
  This affects the rendered pose only: animation clocks, descent, impact timing,
  recovery, and controls retain their existing behavior.

## Ceiling-grab momentum

- A valid ceiling grab retains half of signed incoming horizontal speed, capped
  at 24 units/update in either direction. The startup pose coasts along the
  hangable ceiling and slows by 2 per gameplay update until stationary.
- Existing stick-start timing and release/ground-pound inputs remain unchanged.
  Speed above the normal hanging target approaches that target by 2 per update
  instead of snapping to it. Ordinary hanging acceleration and steering remain.
- Coasting uses hanging floor/ceiling/wall checks; a wall stops the carry, a
  ceiling edge releases Mario, and invalid clearance blocks movement. Metal's
  existing underwater movement scale applies to travel. No persistent momentum
  state survives a new grab; each entry samples the new incoming speed once.

## Distant object sound requests

- Horizontal distance from the camera limits selected requests: platform motors
  at 2,500 units, waterfall/sand/volcano/carousel ambience at 4,000, Amp buzzing and
  flamethrower/Tweester loops at 2,500, and ordinary
  Goomba/Bob-omb/Koopa/Bully/Skeeter footsteps at 1,000.
  Separate world-height limits match each horizontal range: 2,500, 4,000,
  2,500 and 1,000 units respectively, above or below the camera. Both limits
  include their exact boundary; camera pitch does not change them.
  These replace distant playback with silence; nearby timing and volume stay native.
- The castle grounds waterfall bypasses these horizontal/vertical request limits
  and the custom SFX source budget, preserving native attenuation and continuous
  playback there. Waterfall sounds in other levels retain the custom limits.
- Continuous sounds refresh every normal update in range and expire through the
  native freshness mechanism out of range. Walls and camera direction do not mute
  them. Music, player feedback, enemy alerts/attacks, bosses, rumble and unlisted
  sounds retain their existing paths.

## Big Boo's Haunt carousel activity

- The enclosed basement carousel pauses rotation, dynamic collision, its Boo
  manager/minions/boss, carried Boo coins and flamethrower/fire while Mario is
  outside. Behavior timers, fight progress and native visibility resume on entry.
  Doorway passages activate the enclosure before crossing the inner threshold.
- Interior terrain and the rotating platform are hidden outside; exterior walls,
  doorways and surrounding water remain visible. Static terrain collision stays
  available. Dropped coins and the reward star retain their ordinary lifetime.
- Basement music and outside wind continue their existing location-based updates.
  [Matched Ares tests](../../bbh-carousel-performance-2026-10-06/README.md)
  reduce exterior-water CPU/RSP cost, but increase CPU cost inside, at the
  doorway and courtyard entry. The current gate is not a general performance win;
  all tested scenes remain at 30 FPS and console performance is unmeasured.

## Big Boo's Haunt basement audio

- Inside the carousel enclosure, the haunted-house track fades out and stops
  through native sequence/voice cleanup. It fades back in on exit, restarting if
  its fade-out completed; a quick exit cancels the pending stop. Music queue
  priority, carousel music, and temporary fanfares retain their existing paths.
- Carousel creaking participates in the ambient distance gate and low-impact
  source budget. BBH creaking, flamethrower loops and roomed elevator motors
  refresh only in their room or an adjacent doorway room. Dynamic floors use the
  underlying static room; unknown room data retains playback.
- Howling wind follows the outside room, including outdoor dynamic platforms,
  and stops refreshing immediately on entering an indoor room.

## Audio note budget

- Console-class systems and emulators with detected CPU cache emulation,
  including ares, use a 20-note budget shared by music and sound effects.
  Other detected emulators retain 40 notes. The ROM selects this budget during
  audio initialization in every region. This is a performance trial; audible
  note loss and target timing remain to be assessed.

## Lava fireball trails

- Bouncing lava fireball emitters retain their native damaging leading flame,
  collision physics, burst count and cooldown. The remaining flames are intangible
  trails with simple arcs and bounces at the emission height. Trails fade after
  80 updates in their current stage and expire after 100, without floor or wall
  queries. This applies to the volcano and equivalent emitters in the Physics Lab;
  Bowser's flame behaviors retain their existing rules.

## Decorative rendering budget

- All Physics Lab static structure runs, including distant LODs, use consecutive
  groups with padded RSP bounds. Preserve ordered vertex attributes, coverage
  and material restoration. Complete flat lettering may use the equivalent
  block-font texture; alternate command streams may omit only faces proven
  back-facing throughout their camera region. Collision and LOD choices remain
  unchanged. Culled children return so later groups and restoration still run.


- Blue and cyan fish load their texture once per fish; tails and fins reuse the
  body texture in TMEM. Geometry, lighting, animation and draw order stay unchanged.
- Large blue and cyan fish schools spawn 12 fish; small schools keep five.
  JRB and DDD schools keep 80% of those counts, rounded to whole fish: four per
  small school and ten per large school. JRB's four schools total at most 16 fish.
  The full-water flag is excluded from school selection and retained on each
  spawned fish, so flagged schools (including the Aquarium blue school) spawn normally.
  Steering, turning, movement, wall collision, animation and distance checks
  run every update; spawning distance and school lifecycle remain unchanged.
- Normal and blizzard weather snow cap at 110 flakes. Underwater snow keeps its
  30-flake cap. Snow draws up to ten flakes per vertex load and pairs their
  triangles, including partial batches during the normal density ramp.
- Sushi sharks request wakes on three out of every four updates when their
  existing water/proximity checks permit them. Swimming, damage and shark sounds
  retain their existing behavior; the resulting trails are sparser.
- Castle portrait ripples use precomputed groups for the two shared texture maps,
  preserving their triangles, textures, normals and ripple timing. Other painting
  maps retain the existing renderer. The Goomba/Floomba model shares identical
  vertices within each material/animation part and preserves triangle order.
- Shadows use the existing small texture through a projected size of 24 pixels,
  keeping the existing exclusions, transparency, placement and player priority.
- Mario's detail ranges remain 600/1600. The 500/1400 trial and its measurements
  are retained in `/path/to/workspace/optimization-trials-2026-10-04`.

## SFX source budget

- `SFX_BUDGET_TRIAL=1` enables a six-source budget on console-class systems,
  including accurate emulators that pass the console hardware check. Music stays
  outside this source budget and still shares the separate 20-note console pool.
- Native banks choose their winners first. Unclassified effects and global-source
  feedback are protected and may exceed six sources. Only audited motors,
  waterfall/sand/volcano ambience, Amp/fire/Tweester loops, ordinary enemy steps,
  and quiet bubbles can be dropped, using existing distance-aware priorities.
- Playing loops receive a 512-point priority preference to reduce source churn.
  Rejected playback uses the native script stop signal. Interrupted one-shots are
  deleted; pending requests keep native expiry and requested loops can resume.
- Set `SFX_BUDGET_TRIAL=0` to restore the original bank-by-bank playback path.
  Host checks establish selection/state behavior. Matched native Ares trials
  measured audio CPU/RSP savings for six versus eight selected sources; console
  timing and listening quality remain unverified.

## Particle appearance

- Shifting Sand Land's pyramid-top fragments use gravity-driven motion without
  terrain collision or bouncing. Their spawn count, size, spin and 60-frame
  lifetime remain unchanged; the pyramid top itself retains solid collision.
- Original pre-trial particle textures, transparency and opacity fades are restored.
- Both opaque and aggressive texture trials are parked; their source snapshots,
  derived assets and comparisons remain available outside the active game assets.


## Jolly Roger Bay SimpleFlips encounter

- During Plunder in the Sunken Ship (act 1), entering the alcove window inside
  the ship warps only while its water is draining or already drained, to JRB
  area 3: an enclosed black room with a black background
  and one sign reading "Shout-outs to SimpleFlips". The room and sign are absent
  from the other areas, including their collision. Music starts silent; only
  the requested Physics Lab track may start after read 55. Ordinary sound effects remain available. During an eligible encounter, Start
  cannot pause it, even during death.
- All dialogue in this room requires a fresh A or B press to advance or close;
  holding A does not advance pages automatically. Z retains backward scrolling.
- The B interaction indicator disappears immediately when Mario starts talking
  to MIPS or Motos, including while he turns toward them.
- Mario cannot leave the box through movement, caps, debug free movement or
  debug/course warps. A boundary safeguard keeps his body inside the walls,
  floor and ceiling if collision fails. Mario keeps full health and pending
  damage is cleared until Toad punches him; ordinary health drain is skipped.
  Ordinary death and exit warps are blocked. Only Toad's forced
  game over or the intentional crash can end the encounter.
- A blocked debug/cheat visit has no active encounter NPCs and permits ordinary
  pausing and exit warps. After Toad makes contact, pausing and exits remain
  locked so the forced game-over route can finish.
- Arriving in this room restores all eight health wedges, clears pending damage
  and healing, and resets health-regeneration progress. Health is then protected
  throughout the encounter; Toad's punch can still
  kill Mario afterward.
- Entry also restores Mario's normal cap on his head, clears cap-recovery locks
  and saved lost-cap locations, and preserves temporary cap powers. If Mario
  later becomes truly capless, beta Motos appears at the opposite corner from
  MIPS, far from the sign. He uses the supplied patch's model and idle animation
  with Vanish-Cap-style dithered transparency confined to his own model; Mario's
  dust retains normal transparency. He is stationary and cannot grab
  or damage Mario. Talking to him shows the requested twelve sentences as
  twelve pages in one three-line dialogue box. The final scream fills two
  120-pixel lines. After the final page closes,
  normal dialogue cleanup releases Mario, then Motos disappears in a water
  splash with splashing sound effects. He cannot be talked to again.
- The first arrival is saved immediately. The window stays inactive on that
  save file afterward, including after game over or restarting the game.
  Other files retain their own first visit. Copying a file copies its visit
  status; erasing a file allows a new visit.
- A close diagonal camera follows Mario inside the box so the sign and punch
  remain visible. Before the final read 128, ground-pounding the sign's top
  transforms it into a visible Toad, stops the music, restores the black room,
  and removes MIPS if present. Talking uses the normal B interaction and
  displays one word per page:
  "Now You Done Messed Up I Will Count To One Hundred And When I End I Will
  Count To One Hundred More Are You Ready One Two Oatmeal."
- After the last word, Toad adopts Mario's animation joints and first-punch
  animation. Mario stays in the conversation action until contact, then loses
  all health and receives an airborne backward knockback. Half a second after
  contact, Toad returns to his normal model and animation, facing the direction
  he punched after compensating for the native wave pose's facing offset.
  This death forces a game over regardless of lives or debug level select, without granting the
  separate HE THICC unlock. This game-over screen replaces Goddard with Toad's
  head. Move the hand with the stick, hold A over the face or cap and move the
  stick to stretch it; releasing A lets it return to its original shape.
  Face picking and horizontal drag follow the selected 4:3 or 16:9 projection.
  C buttons rotate the head; grab/release and hand appearance use Goddard's
  sound effects. Z enables the normal face-tile Easter egg with Toad.
  These controls deform Toad's character mesh. Leaving this screen restores
  ordinary title and game-over behavior.
- Each committed sign reading counts once, locally to this encounter. Milestone
  text is defined in `include/simpleflips_messages.inc.h`; other reads retain
  the original shout-out. Every sign message uses the same two-line dialogue
  box and screen position, with lines wrapped to fit the native font. All
  current sign messages fit on one page. Effects begin after dialogue closes.
  Read 26 makes the room slightly red (red tint 36/255); read 55 starts the
  existing Physics Lab Mii Channel music. Read 70 spawns a stationary, ungrabbable MIPS who can say
  "I have no paws, but I must game." once, then releases Mario through normal
  dialogue cleanup. Read 95 causes a harmless explosion:
  the sign disappears with no collision for five seconds, then fades back in.
  Read 105 spawns a normal collectible 1-up above the sign. It performs the
  ordinary spawn twirl for 38 updates before homing to Mario, with the normal
  spawn jingle and sparkles.
- Read 128 says "You may now ground pound the sign.", removes MIPS, restores the
  black room and silence, and disables further sign talking. Ground-pounding
  afterward intentionally halts the game and opens the assertion/debug screen
  with the supplied Nathanial scuttle bug monologue. C-Up/C-Down scroll the
  assertion text; Z speeds up scrolling. L and R move one debug page left or
  right per press, with no repeat while held; Z does not change pages.
  Immediately before this intentional crash, a pending Toad game-over marker
  is written to the current save file and its backup. The next boot or soft
  reset consumes that marker and starts on Toad's game-over screen once,
  including his existing head controls and sound effects. Pre-NMI shutdown
  prevents new RSP tasks and retries the SDK's RSP program-counter cleanup.
  A task that has not finished after a shared 100 ms grace period is halted
  before retrying PC cleanup; the hardware-count deadline tolerates wraparound.
  A timer-driven fallback also observes the shutdown latch independently of the
  scheduler and crash screen. After 100 ms without scheduler retrace progress,
  it stops the scheduler, halts the RSP and retries PC cleanup until NMI;
  a responsive scheduler retains its normal reset bars and notifications.
  Audio startup clears retained task,
  output-buffer and sample-clock ownership after a soft reset; ordinary in-game
  sound resets still wait for running tasks and preserve queued playback.
  Once pre-NMI begins, the crash-screen thread stops starting serial transfers
  and ordinary controller/motor DMA starts check shutdown atomically. Failed
  DMA starts do not enter completion waits or consume stale controller input.
  Sound shutdown bounds its list traversal and rejects invalid links so corrupt
  audio lists cannot strand reset cleanup. The crash-screen thread
  also retries reset cleanup. Its delays and peripheral waits observe reset;
  an unfinished controller DMA retains its locks and buffer ownership rather
  than allowing another caller to reuse the buffer.
  Rumble shutdown attempts one final motor stop only when both controller locks
  and the serial interface are available. Motor DMA starts are checked and each
  completion wait is bounded to 20 ms; unfinished DMA retains both locks.
  Accessory detection and the rumble thread stop during reset.
  A scheduler fault or breakpoint redirects pre-NMI to the crash screen,
  including a later scheduler failure after another crash page has opened.
  Already queued notifications are preserved; the SDK's sticky shutdown state
  also detects reset when event delivery was dropped. Both reset paths share
  RSP cleanup, while retrace notifications continue during scheduler shutdown. Crash drawing
  never waits for a controller lock held by a faulted thread; failed controller
  reads release acquired ownership without consuming stale input when no DMA
  remains outstanding. Controller waits time out after 20 ms, keeping the menu
  visible; symbol reads use bounded ROM I/O and fall back to numeric addresses
  if PI is busy, reset is pending or symbol data is invalid. Crash delays use
  elapsed hardware counts without changing the system clock.
  Fault and breakpoint discovery handles the SDK's NULL-terminated active list,
  including system threads. Assert pages decode the exception field independently
  of interrupt and branch-delay flags. The assertion page is #01; overview, context,
  stack trace, optional log and disassembly follow in numerical order. Crashes without
  an assertion skip #01 when navigating. Formatted messages are bounded; filenames,
  conditions and logs are literal text, with long assertion paths scrollable.
  Disassembly validates every aligned word against loaded text and installed
  RAM, preserves the PC/RA highlight on redraw and shows unsupported instructions
  safely. Jump targets follow the R4300 encoding.
  Stack tracing accepts only aligned pointers into installed RAM and scans at
  most 100 words without crossing the RAM boundary. The first indirect-call
  entry needs no previous frame. Crash logs show every retained entry newest
  first, treating percent signs literally; trace text has room for the entire
  frame-name buffer plus its address and line number.
  Reading message 128
  and the ordinary Toad punch do not arm this marker. The save's visited flag
  remains set, so the encounter stays unavailable afterward.
  Before this final reading, ground-pounding still starts the Toad encounter.


## Boot splash presentation

- Ordinary boot shows the SUPER MARIO 64 logo splash, Goddard and file select.
  The splash has no Floombas. Its credit reads
  "Devin's Movement Rewrite 0.1.0" on two centered lines, retaining the native
  cyan-to-blue tint and TM mark. The internal ROM name is `DevinMovementRewrite`.

- Castle exterior roof and northwest cliff support follows the visible triangles,
  so movement across their joins cannot lose support or hit raised invisible ledges.
  Roof grip, cliff slipperiness, tower walls and the existing outer edge remain.
  The waterfall bank also follows its visible slopes, including the downhill join
  reached after exiting Metal Cap Cave, so slides can move into the pool.
- Hidden roof collision bridges the upper tower seam to its inset walls, so
  walking into the stone cannot drop Mario through a gap in floor support.
- Castle upper white tower collision stays vertical and is inset to its visible
  upper roof rim, inside the slightly tapered stone. It remains wall-kickable
  without extending invisible walls beside the top roof.

## Dialogue box sizing

- Every standard dialogue page uses the height needed by its last visible text
  line, with a minimum of one line. Trailing blank padding does not enlarge the
  box; intentional gaps between text lines remain. Page breaks, scrolling timing,
  text, responses and opening/closing behavior stay unchanged. During a page
  scroll, the box fits both pages. Continue and response arrows follow the fitted
  height. This applies to both black and white dialogue boxes.

## Player statistics

- Counters and records do not increase while the shared cheating state is active,
  including Debug Mode and debug flight. Saved statistics remain readable and
  preserved. Resuming eligible play starts fresh movement sequences and attack
  credit, excluding movement and defeats from the ineligible interval. Cheat
  toggles preserve the course visit and never count as entries or exits. Actual
  warps and deaths still update visit identity while recording is suspended;
  entries during suspension are not counted later when play resumes. Visit enemy
  streaks restart, and a visit spanning suspension cannot earn damage-free stars.

- Track Missed Bowser Throws in Combat. Each player throw counts once
  when Bowser settles or begins off-stage recovery without an accepted bomb hit.
  Weak throws count; drops and timed automatic escapes do not. Existing saved
  totals migrate to the current statistics version, with absent counters starting at zero.
- Track Enemy Bounce Streak in Combat as the highest consecutive successful
  enemy rebounds without ground contact, including twirl and ground-pound rebounds.
  Boxes and scenery do not count. Wall kicks preserve the streak; ground contact,
  death, warps, level/file changes and ineligible play end it.
- Track Seconds Soaked in Damage and Recovery during eligible gameplay with
  the soaked effect and no water contact. Display whole seconds; pauses and
  contact at or below the water surface do not count. Preserve earlier saved
  statistics when upgrading the catalog; absent entries start at zero.
- Track Defeated by Bowser independently for Fights 1, 2 and 3. A death, fatal
  arena fall or pause-menu exit counts once per visit; winning and dying in the
  approach course do not count. Preserve existing saved totals when migrating
  to the expanded statistics catalog.
- Track the longest damage-free gameplay streak, damage-free stars, most enemy
  defeats in one course visit, breakdance defeats, thrown-object defeats, time
  holding objects, horizontal distance while soaked outside water, farthest
  cannon flight, and separate Wing/Metal/Vanish cap times. Preserve earlier
  statistics when upgrading to version 7; all eleven new entries start at zero.
  Health loss ends the current damage-free streak and disqualifies that visit's
  stars even after healing; pending hurt damage also disqualifies star collection.
  Clean warps preserve the streak; death, file changes and ineligible play end it.
  Area changes preserve course-visit damage and defeat totals. Attribute defeats
  to the last credited attack, without counting stuns or repeated callbacks.
  Cannon distance runs from launch to first ground landing or water entry;
  interrupted flights produce no record. Activity timers count eligible Mario
  updates, excluding pauses and cheats; simultaneous cap powers count separately.
  Keep new titles short enough to fit beside input history at natural font width,
  preserving the existing five-row statistics layout.
- Count statistics independently for each of the four save files. The pause menu
  exposes Statistics for the current file, in both course and castle pauses.
  The camera-angle selector is removed; the existing camera default is unchanged.
  A selects Statistics; castle Up/Down chooses Continue, Statistics or Options.
  Left/Right browses course scores. The view groups all 159 counters into seven
  ordinary categories plus HE THICC when eligible, with five rows per page.
  Display order is independent of saved counter IDs. Keep time totals together,
  stars with collectibles, object handling with world interactions, and lives
  lost/game overs with damage and recovery. Pair horizontal and 3D distances
  with their related records; group custom move variants and Bowser results.
  L/R changes category once per press, without repeating while held; holding both together does nothing.
  Shoulders held when Statistics opens do not change category; release and press again to browse.
  stick and D-pad left/right do nothing. Stick and D-pad up/down browse silently,
  moving immediately on a fresh input, then repeating after 10 frames and every
  5 frames thereafter. Fresh inputs wrap between the first and last row in the
  category; held repeats stop at the ends. Main pause navigation remains one step
  per press or tilt.
  Only category changes
  play the selection sound. B returns and Start resumes. Start has priority over A/B; held opening
  A does not resume. Statistics controls do not alter camera speed or aspect ratio.
  Display hours:minutes:seconds, game-unit distances/speeds, wedges, full counts
  and selected-record level name without an area suffix. Browsing does not count
  playtime or save data.
  Any statistic that reaches its 64-bit maximum, including an addition that would
  exceed it, displays "Lots!" in place of the numeric value and units.
  Preserve original IDs 1–100, plus dialogue time, separate
  horizontal/3D distance totals and the hack's twirl/breakdance events.
- Also count accepted dabs and dab-induced sleeps, dab/held-twirl-pose time,
  separate ground/air twirl wall rebounds and their longest continuation chain,
  supported water-walk time/horizontal/3D distance and longest stretch, slide-kick
  liquid bounces, completed downward swimming bursts, new twirl stun episodes,
  confirmed passive Metal follow-up defeats, breakdance-shed power caps, powers
  removed by damage and Bowser's timed escapes. Combined caps count each removed
  power. First Metal stuns, harmless Boo bounces and Wing-twirl follow-up defeats
  do not count as passive Metal kills.
- Add a HE THICC category: active character playtime, butt claps, wavedashes,
  heavy belly slams, timed belly bounces, ground-pound bounces, real butt wall
  kicks, slide-kick bounces, connected explosive strikes and swimming strokes.
  Count only committed HE THICC gameplay, for both appearances; requested choices,
  an unlocked save, demos and credits do not qualify. Use accepted event boundaries;
  repeated animation/render updates and rejected inputs do not add move counts.
  Show the HE THICC category only on a committed HE THICC save file, even in debug
  mode. Retain its saved totals when hidden; switching appearance does not change
  save eligibility.
  Earlier generic statistics and ordinary Physics Lab course timing remain.
  Add no custom-location or custom-secret counters.
- Playtime includes dialogue (including frozen course-entry messages), cutscenes
  and cannons; pause, demos and credits are excluded. Dialogue has a separate
  overlapping timer. Physics Lab counts as a course.
- A visit ends on death, course exit or a different-level warp. A 100-coin star
  does not end it; death counts an exit without a star if none was collected.
- Respawn refills do not count as health restored. Survived-fall eligibility follows
  actual queued healing and damage. TAS metadata is not a castle star; held-object
  swimming strokes and completed one-time Toad dialogue count normally.
- Wiggler's final dialogue preserves the actual attack method for its confirmed
  defeat, even after a long read; missing or cheating-discarded credit is not restored.
- Bully collision feedback preserves an explicit player's attack method. A later
  ordinary body push replaces it with unclassified contact credit.
- Ground-pound defeat credit includes its accepted landing hit. Twirls, water
  bursts and ordinary fall impacts do not become ground-pound defeats.
- Long-jump records survive twirl interruptions. Survived falls include water
  landings and enemy bounces. Unassisted airtime excludes both wearing Wing Cap
  and actual flight. Warps/respawns, including every star-exit animation,
  contribute no movement distance.
- Persist to 32 KB SRAM at ordinary saves, deaths/exits and 60 gameplay-second
  checkpoints. Initial statistics read failures block implicit writes until a
  successful reload or explicit copy/erase of that slot. Copy and erase follow
  the slot. Unread base progress/statistics or blocked settings/timing loads
  (including unsupported settings/timing versions) prevent the entire copy before
  any destination changes. Rejected copies retain destination models, the copy
  selection phase and its prompt, with buzz/bounce feedback. Readable empty files remain
  valid copy sources; statistics retain their existing format-recovery policy.
  Resets retain the last successful
  checkpoint, with subsequent unsaved counting lost. See `docs/player_stats.md`
  for the exact catalog, backup format, event-credit limits and migration policies.
  Version-1 saved totals and record locations migrate automatically to version 2;
  new counters begin at zero.


## Intentional wall hugging

- Walking wall contacts enter the pushing/sidling pose and 6-speed cap only
  with nonzero stick input within 60 degrees of directly into the contacted
  wall. Glancing, parallel and away inputs keep normal walking animation and
  do not impose that cap. Wall collision itself remains active.
- Contact with an ordinary, big-star or key door that would qualify for wall
  hugging instead attempts to open that contacted door immediately. Use its
  normal opening/unlock action or locked-door message and preserve star/key
  requirements. This contact also works with light stick input and sidling
  angles; ordinary approach checks remain unchanged before wall contact.

- The game samples controller input after audio and graphics-pool setup, then
  waits for that read before gameplay consumes it. This avoids using a sample
  taken before setup; stick deadzones, button edge handling and one gameplay
  update per frame remain unchanged. Input arriving after the sample still waits
  until the next update; polling-phase variability is not eliminated. Reset and
  failed DMA starts skip input consumption.

## Input display

- The game shows the approved native-resolution input history and controller
  overlay independently of debug mode. It samples the physical controller input
  before gameplay remaps or attract-demo replacement, once per game-loop frame.
  It never changes those inputs.
- Raw stick arrows use eight 45-degree sectors, including movements ignored by
  gameplay's deadzone. The continuous raw stick angle has two
  decimal places, with right at zero degrees, and power has two decimal places as a
  percentage of the gameplay stick's 64-unit maximum. Deadzone movement can therefore
  show a direction with zero gameplay power. Both readouts use five numeric digits,
  including leading zeros: `000.00` degrees and `000.00%`. The center mark moves with
  the stick cap and stays visible at every angle.
- D-pad and C-button directions each have separate indicators and held-frame counters.
  Their arrows have broad heads, align vertically with the letter centers, and
  leave a clear pixel after the letter outline. C/D arrow borders match the gray
  power/degree outlines, independently of their fill colours. Analogue-stick arrows
  retain their original white fill and black borders.
  Held-frame numbers alone use black outlines; power and degree readouts retain gray.
  All stick arrows use symmetric pixel steps, sharp heads, and matching shaft thickness.
  The history A glyph omits its stray upper-right pixel.
  D-pad indicators do not affect movement. A is blue, B green, C yellow, Start red,
  and the other digital inputs gray. Controller highlights follow each held button.
- Each input's hold count starts at one, persists through other inputs joining or
  leaving, and resets on its release. Stick hold counts reset when the displayed
  direction changes. Brackets group simultaneous inputs; large groups use two columns.
  History shows up to 10 rows with a Large controller, 12 with Small, or 15 with
  Controller Display Off. Only complete history groups are shown.
  Neither column displays an FR heading; frame counts remain visible in both columns.
  Z sits two pixels higher in both its idle and pressed states.
- Counters retain unsigned 32-bit frame counts and saturate safely. All counts display
  their exact decimal value, including holds above 999. Groups with long counts use
  one column to keep digits clear of other inputs. History is transient and is never written into save data.
- The display stays live across title/file menus, gameplay, pause, conversations,
  demonstrations, credits, cutscenes, transitions, and ending screens, even when inputs
  have no effect there. Hold counts continue across these states and level changes.
  It follows the physical controller during attract demos, not the demo recording.
  Disconnected/failed controller samples appear neutral. History resets only with the
  game process. Sampling adds no button remaps or menu ownership. Hold counts measure
  samples,
  so loading stalls without game-loop samples do not add held frames.
- Input Display and Controller Display start Off per file. Input Display controls
  history; Controller Display selects Off, Large, or Small. Switching Input Display
  from Off to On promotes an Off controller to Large, preserving Small. The
  controller can then be turned Off while history stays On; both preferences
  survive save/reload. Turning history Off preserves the controller preference.
  Analogue stick values is a separate Off/On preference, default On, displaying
  angle/degrees and power below either controller size. It does not enable a
  hidden controller. Small uses the approved 50-by-35 pixel
  artwork at one screen pixel per texel, including its held-button highlights,
  and keeps the stick cap on whole pixels. It stays near the lower-left corner
  and shifts six pixels up when numbers are shown to keep them clear. Raw sampling continues
  regardless of which drawings are enabled.
- Input history is controlled through Options, without a paused Z+C-Up shortcut
  or shortcut reminder. Preferences are saved per file on leaving pause; hold
  history remains transient and unsaved.
- The pause transition blends a clean saved background underneath the input strip
  using the same weight as the rest of the scene, then draws live inputs. Two small
  background buffers also preserve interrupted fades without caching input glyphs.
  Its allocator reserves graphics space for the live overlay and background copy.
  Debug menus, profiler output, and deferred text draw above the input display.
- The main file menu's bottom button row, pause, Statistics, and dialogue content
  leave room beside the input strip while enabled and restore their original
  layout while hidden. The colorful PAUSE heading is centered within the remaining
  pause-menu area. Authored dialogue lines and page breaks are preserved; unusually wide dialogue scales
  horizontally to fit the remaining space.
- `INPUT_DISPLAY` in `config_graphics.h` controls compilation and enables its required
  PuppyPrint font support, including builds with debug disabled.


Cheat Wing/Metal/Vanish caps default Off and require Cheats enabled for the current
playing file. Each supplies its power indefinitely without replacing the natural
cap timer. Collected/scripted powers have separate ownership; their ordinary timer,
warning and music expiry continue. Natural expiry, shock/depowering and breakdance
release retain forced powers. Breakdance releases only naturally owned powers;
its ordinary cosmetic cap remains uncollectible. Disabling a selection or the
master removes only forced ownership, preserving natural powers/time. Cap-loss statistics count natural powers only, and forced-only depowering does
not repeatedly stop music. Normal cap save/theft state is never granted by forcing
a power. Spawn/file initialization
clears temporary ownership, and demos/credits cannot activate these overrides.

Permanent Soaked defaults Off and requires Cheats enabled. It overrides Gameplay
Soaked Off and Current Vanish's dry status; ordinary cap immunities still follow
their selected presets. Wind, fire and electrical consumption cannot dry this
forced wet status. It extinguishes burning and supports ordinary wet particles.
Natural wet timers continue independently, and disabling the cheat returns to
ordinary status rules without leaving an invented wet tail.


### Options shell retention and Mario silhouette

- Prevent shell loss requires Cheats enabled and an active owned ground or
  underwater shell. Harmful reactions and forced drops retain it; ground-wall
  collisions stop forward movement. Health costs remain. Manual dismount, throw,
  successful water exits and forced star/warp cleanup still release ownership
  and music. Ordinary held objects receive no shell protection.
- Seamless instant-area travel retains carried/ridden objects and shell music,
  moving their coordinates and required anchors with Mario before the source area unloads.
- Through-wall silhouette defaults On. Off removes Mario's and mirror Mario's
  through-wall passes while preserving normal visibility, opacity and damage
  blinking. Held objects and other silhouettes keep their own rules.
- Analogue stick values is the separate Off/On readout preference directly below
  Controller Display, default On. It controls the bottom angle/degrees and power
  readouts for both Large and Small; it never enables a hidden controller.
- Remove engine speed caps defaults Off and requires Cheats enabled for the
  active playing file. On bypasses the shared 128-unit cap and hard movement
  ceilings for slides, sideflips, swimming/rolls, flight/twirls, carrying/throwing,
  hanging, falling and HE THICC moves. Acceleration targets, drag, friction,
  braking, collisions and ordinary action timing still apply. Scripted star and
  death-return descent and Bowser's angular spinning limit stay unchanged.
  Each signed movement component retains a 16384-unit numerical guard; ground,
  air and water use additional collision samples at high speeds. Animation and
  pose conversions remain bounded. Disabling the option or Cheats restores
  ordinary ceilings on the next relevant movement update.

- Course and Session timers default Off and may display together. A single row
  uses y=22; paired rows use y=29 for Course and y=15 for Session, centered on
  the lower HUD's y=22 baseline with 14-pixel spacing. They count
  gameplay updates, excluding pause, dialogue, fades/loading, death, scripted
  intangible actions, Mario-frozen time stops, demos and credits. Player-controlled
  cannon aiming counts once the cannon is ready. The existing slide/course challenge
  timer retains its separate rules. A cancelled Secret Slide timer cannot award
  the fast-slide star at the finish; a new run must start normally. Session time
  resets on file entry and survives area/course travel; course time resets on
  entry/re-entry, death or exit, while travel between areas of the same level retains it.
- Best times per star records genuine course/star identities independently of
  timer visibility. Keys and the Grand Star are excluded. Each star submits at
  most once per course visit; an improved time from a new visit replaces it.
  Enabling Cheats or Debug Mode makes the whole current attempt ineligible,
  including enabling then disabling while paused. New course entry starts a
  fresh attempt, eligible only when neither permission nor residual debug state
  is active. Best Times uses Left/Right for courses and B to return.
  Main courses list six missions and 100 coins; the Secret Slide lists its box
  and fast-slide stars; other bonus courses list only their red-coin star.
  Nonexistent star slots cannot submit times.
- Times use common 1/150-second storage units across NTSC/PAL, with saturation
  invalidating the attempt. Per-file CRC/generation backups use retired legacy
  statistics banks only after both current statistics copies are readback-
  verified. Initial read failures or unsupported record versions block implicit
  replacement. Explicit file copy/erase replace both timing banks; failed writes
  retain dirty state for retry. Existing save, options and current statistics
  allocations remain separate.
