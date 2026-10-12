#pragma once

// Keep hinged doors, Mario's passage and their camera/sounds on one clock.
#define STANDARD_DOOR_CONTROL_RELEASE_FRAMES 5
#define STANDARD_DOOR_SPEED_NUMERATOR 12
#define STANDARD_DOOR_SPEED_DENOMINATOR 10
#define STANDARD_DOOR_ANIM_SPEED ((f32) STANDARD_DOOR_SPEED_NUMERATOR / STANDARD_DOOR_SPEED_DENOMINATOR)
#define STANDARD_DOOR_ANIM_ACCEL                                                                       \
    (0x10000 * STANDARD_DOOR_SPEED_NUMERATOR / STANDARD_DOOR_SPEED_DENOMINATOR)
#define STANDARD_DOOR_DURATION(frames)                                                                 \
    (((frames) * STANDARD_DOOR_SPEED_DENOMINATOR + STANDARD_DOOR_SPEED_NUMERATOR - 1)                  \
     / STANDARD_DOOR_SPEED_NUMERATOR)

/*********************
 * MOVEMENT SETTINGS *
 *********************/

/**
 * Changes Mario's ground turn radius by making it dependent on the analog stick magnitude and speed.
 */
#define VELOCITY_BASED_TURN_SPEED

/**
 * Allows Mario to easily side flip when moving forwards at any speed.
 */
#define SIDE_FLIP_AT_LOW_SPEEDS

/**
 * Allows Mario to aim towards a new direction at the end of turning around,
 * and allows Mario to turn around multiple times in a row.
 */
#define RESET_DIRECTION_WHEN_TURNING_AROUND

/**
 * Improved hanging:
 * - Doesn't require holding down the A button.
 * - Precise turning control.
 * - Prevents falling from the edges.
 */
#define BETTER_HANGING

/**
 * Change the movement speed when hanging from a ceiling (the vanilla value is 4.0f, has no effect if BETTER_HANGING is enabled).
 */
#define HANGING_SPEED 4.0f

/**
 * Prevents Mario from falling asleep while idle.
 */
// #define NO_SLEEP

// Eligible idle updates before sleep startup (30 updates/second).
#define IDLE_SLEEP_DELAY_CAPPED_FRAMES (90 * 30)
#define IDLE_SLEEP_DELAY_CAPLESS_FRAMES (30 * 30)
#define IDLE_SHIVER_DELAY_CAPPED_FRAMES (30 * 30)
#define IDLE_SHIVER_DELAY_CAPLESS_FRAMES (3 * 30)

/**
 * Disables fall damage.
 */
// #define NO_FALL_DAMAGE

/**
 * Disables the scream that mario makes when falling off a great height (this is separate from actual fall damage).
 */
// #define NO_FALL_DAMAGE_SOUND

/**
 * Fall height for normal fall damage. Vanilla is 1150.0f.
 */
#define FALL_DAMAGE_HEIGHT_SMALL 1000.0f

/**
 * Fall height for double fall damage. Vanilla is 3000.0f.
 */
#define FALL_DAMAGE_HEIGHT_LARGE 2100.0f

/**
 * Disables Mario getting stuck in snow and sand when falling.
 */
// #define NO_GETTING_BURIED

/**
 * Prevents hands-free holding.
 */
#define HANDS_FREE_HOLDING_FIX

/**
 * Prevents Mario from losing his cap.
 */
// #define PREVENT_CAP_LOSS

/**
 * Uses Shindou's pole behavior.
 */
#define SHINDOU_POLES

/**
 * If A and Z are pressed on the same frame while walking with a strong stick,
 * Mario will long jump instead of ground pound.
 */
#define EASIER_LONG_JUMPS

/**
 * Enables the ability to hold Z while twirling to descend faster.
 */
#define Z_TWIRL

/**
 * Disables bonks when ground pounding next to a wall.
 */
#define DISABLE_GROUNDPOUND_BONK

/**
 * Allows Mario to jump kick on steep surfaces that are set to be non slippery, instead of being forced to dive.
 */
#define JUMP_KICK_FIX

/**
 * Allows Mario to grab hangable ceilings from any state.
 */
#define HANGING_FIX

/**
 * The maximum angle the player can wall kick, in degrees. 0..90. To allow 45 degree wall kicks, you must supply `46` to allow 45 and under.
 */
#define WALL_KICK_DEGREES 60

#define WALL_KICK_GRACE_FRAMES 6

// Fraction of impact speed used for bonk recoil, before the existing minimum.
#define WALL_BONK_MOMENTUM_RETENTION 0.5f

/**
 * Makes Mario unable to ledge grab steep slopes to prevent false ledge grabs.
 */
#define DONT_LEDGE_GRAB_STEEP_SLOPES

/**
 * Buffers an A input if you jump off a slope during the landing lag
 */
#define SLOPE_BUFFER

/**
 * Disables BLJs and crushes SimpleFlips's dreams.
 */
// The global speed cap bounds backwards long jumps without disabling their chain.
// #define DISABLE_BLJ

/**
 * Re-enables upwarping when entering water. Forces you to only enter water from the top.
 */
// #define WATER_PLUNGE_UPWARP

/**
 * This bug allows mario to move when initiating a dialog,
 * just as long as mario doesn't enter an action that can
 * be interrupted with text
 * Uncomment this to fix this bug, and frustrate speedrunners
 */
#define BUGFIX_DIALOG_TIME_STOP

/**
 * Inertia defines; allow Mario to preserve his momemtum when leaving moving platforms.
 */
#define MARIO_INERTIA_UPWARD
#define MARIO_INERTIA_LATERAL

// Custom enemy carry and departure momentum; Mario and native Bowser carry remain.
#ifndef ENABLE_ENEMY_PLATFORM_MOTION
#define ENABLE_ENEMY_PLATFORM_MOTION 0
#endif

/*************************
 * GROUND AND AIR TUNING *
 *************************/

// Speeds are world units per gameplay frame. The debug walking trial switches
// the full-stick target between 40 and 127 without changing the hard cap.
// The global limit applies to each signed velocity component independently.
#define MARIO_GLOBAL_SPEED_CAP 128.0f
// Keep fixed-point animation conversion and collision subdivision representable.
// This is a numerical guard for the speed cheat, not an action speed target.
#define MARIO_SPEED_SAFETY_CAP 16384.0f
#define MARIO_ACTIVE_SPEED_CAP                                                                         \
    (game_options_cheat_active(GO_NO_SPEED_LIMIT) ? MARIO_SPEED_SAFETY_CAP : MARIO_GLOBAL_SPEED_CAP)
#define MARIO_UNLIMITED_COLLISION_MAX_STEPS 2048
// Use four ground/air checks; faster travel uses larger increments.
// Ground checks include travel along the current slope.
#define MARIO_GROUND_AIR_COLLISION_MIN_STEPS 4
#define MARIO_GROUND_AIR_COLLISION_MAX_STEPS 4
#define MARIO_GROUND_AIR_COLLISION_STEP_DISTANCE 16.0f
#define MARIO_MAX_FALL_SPEED 100.0f
#define HE_THICC_MAX_FALL_SPEED 127.0f
#define HE_THICC_WATER_GRAVITY_SCALE 1.25f
// Wind resistance begins once Mario falls faster than vanilla's 70-unit cap.
#define FALL_DRAG_START_SPEED 70.0f
#define FALL_DRAG_GRAVITY_REDUCTION_PER_UNIT 0.035f
#define WALK_TARGET_SPEED 40.0f
#define WALK_WING_TARGET_SPEED 48.0f
#define WALK_SLOW_TERRAIN_TARGET_SPEED 32.0f
#define WALK_DEBUG_TARGET_SPEED 127.0f
#define WALK_STICK_NEGATIVE_DECEL 6.0f
#define WALK_WALL_HUG_MAX_ANGLE DEGREES(60)
#define GROUND_NEUTRAL_BRAKE_DECEL 2.0f
#define GROUND_NEUTRAL_COAST_DECEL 1.0f
#define GROUND_NEUTRAL_ROLLOUT_BRAKE_DECEL 2.5f
#define GROUND_NEUTRAL_ROLLOUT_COAST_DECEL 1.5f
#define HOLD_LIGHT_TARGET_SPEED 20.0f
#define HOLD_HEAVY_TARGET_SPEED 8.0f
#define CAPLESS_CAP_PICKUP_AIR_DRAG 1.0f

/** Full-stick crawl speed on level ground. */
#define CRAWL_TARGET_SPEED 9.0f
#define MARIO_STRONG_STICK_THRESHOLD (64.0f * 0.85f)

// The shared jump scale also applies to backflips and flying triple jumps.
#define JUMP_HEIGHT_VELOCITY_SCALE 1.05f
// Retain this fraction of the actual apex for a fresh ground A+B belly dive.
#define GROUND_DIVE_HEIGHT_RETENTION 0.9f
#define JUMP_RELEASE_UPWARD_DECEL 15.0f
#define JUMP_FORWARD_SPEED_RETENTION 0.90f
#define TRIPLE_JUMP_SPEED_THRESHOLD 10.0f

#define LONG_JUMP_SPEED_MULTIPLIER 1.2f
#define LONG_JUMP_BOOST_CAP 52.0f
#define FORWARD_DIVE_SPEED_BOOST 5.0f
#define FORWARD_DIVE_BASE_AIR_DRAG 0.8f
#define FORWARD_DIVE_EXTRA_DRAG_THRESHOLD 48.0f
#define BACKWARD_AIR_VANILLA_DRAG_THRESHOLD -16.0f
#define BACKWARD_AIR_DRAG_BLEND_START 20.0f
#define BACKWARD_AIR_DRAG_THRESHOLD -32.0f
#define BACKWARD_AIR_VANILLA_EXTRA_DRAG 2.0f
#define BACKWARD_AIR_EXTRA_DRAG 6.0f
#define BACKWARD_AIR_HELD_GAIN_AT_BLEND_START 0.5f
#define BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG 2.0f
#define BACKWARD_JUMP_KICK_UPHILL_DECEL 20.0f
#define SLIDE_KICK_BASE_AIR_DRAG 0.28f
#define SLIDE_KICK_EXTRA_DRAG_THRESHOLD 64.0f
#define SLIDE_KICK_AIR_ACCEL 0.5f
#define SIDE_FLIP_AIR_ACCEL 2.0f
#define SIDE_FLIP_HORIZONTAL_SPEED_CAP 40.0f

/*********************
 * SWIMMING SETTINGS *
 *********************/

// Metal's ordinary actions advance more slowly as his body enters water.
// Scaling travel and gravity together preserves the moves' basic jump arcs.
// The sinking limit is in world units/update, after water resistance scales
// movement. Foot-only wading receives no slowdown.
#define METAL_WATER_TIME_SCALE 0.65f
#define METAL_WATER_MAX_FALL_SPEED 75.0f
#define METAL_WATER_RESISTANCE_START_DEPTH 50.0f
#define METAL_WATER_RESISTANCE_FULL_DEPTH 140.0f

/**
 * Speeds are world units per gameplay frame; acceleration/drag changes speed each frame.
 * Durations count Mario updates (normally 30 per second), excluding pause and time stop.
 * MIN_SWIM_SPEED is the base threshold above which ordinary swimming applies drag.
 * SWIM_SOFT_SPEED_LIMIT caps the drag threshold earned by chaining breaststrokes.
 * MAX_SWIM_SPEED is the hard limit on swimming forward speed.
 */
#define MIN_SWIM_SPEED 20.0f
#define WING_FLIGHT_MIN_CRUISE_SPEED 32.0f
#define WING_FLIGHT_CRUISE_SPEED 64.0f
#define WING_FLIGHT_MAX_SPEED 100.0f
#define WING_FLIGHT_SPEED_APPROACH 1.0f

#define SWIM_SOFT_SPEED_LIMIT 48.0f
#define MAX_SWIM_SPEED 64.0f
#define SWIM_DRAG 0.5f

/**
 * A restores its cruising speed and preserves higher speeds up to the soft limit.
 * B accelerates or brakes toward its own target. When both are held, the last
 * pressed button takes priority.
 * These controls require empty hands.
 */
// Initial A swimming starts immediately; release within six updates for a stroke.
// Presses within an existing A-swimming sequence retain their normal timing.
#define A_SWIM_STROKE_TAP_FRAMES 6
#define A_SWIM_SPEED 24.0f
#define A_SWIM_ACCEL 0.6f
#define B_SWIM_SPEED 12.0f
#define B_SWIM_ACCEL 0.6f
#define B_SWIM_DECEL 0.6f

/**
 * Empty-handed breaststroke gains: frames 1-5, then 9-13. Animation timing is unchanged.
 * Stroke drag increases above the swimming soft limit.
 * Recovery applies SWIM_RECOVERY_DECEL plus SWIM_DRAG when above the current threshold.
 * Held-object strokes retain their original acceleration and timing.
 */
#define BREASTSTROKE_INITIAL_ACCEL 0.75f
#define BREASTSTROKE_FINISH_ACCEL 2.25f
#define BREASTSTROKE_DRAG 0.17f
#define BREASTSTROKE_ABOVE_SOFT_DRAG 0.75f
#define SWIM_RECOVERY_DECEL 0.25f

/**
 * Idle drag scales both the base 2-unit braking and extra drag above MIN_SWIM_SPEED.
 * This includes held-object idle. Plunge drag scales vertical recovery (up 2, down 1).
 * Entry divisors apply once; forward plunge drag applies on every plunge update.
 */
#define WATER_IDLE_DRAG_SCALE 0.6f
#define WATER_PLUNGE_DRAG_SCALE 0.6f
#define WATER_PLUNGE_FORWARD_DRAG 0.0f
#define WATER_ENTRY_FORWARD_DIVISOR 1.5f
#define WATER_ENTRY_VERTICAL_DIVISOR 1.9f

/**
 * Target yaw velocity per unit of horizontal stick input, in angle units per frame.
 * 65536 angle units = 360 degrees; 14 at stick magnitude 64 gives about 4.92 degrees/frame.
 */
#define SWIM_YAW_SPEED 14.0f
#define B_SWIM_YAW_SPEED 20.0f // Horizontal turning while B is held below A_SWIM_SPEED.
#define SWIM_MAX_PITCH_DEGREES 75.0f
#define SWIM_PITCH_SPEED_DEGREES 70.0f // Degrees per second at 30 updates/second.

// Automatic water exits add these boosts, then scale both velocity components.
#define WATER_JUMP_MIN_UPWARD_SPEED 25.0f
#define WATER_JUMP_FORWARD_BOOST 1.2f
#define WATER_JUMP_UPWARD_BOOST 9.0f
#define WATER_JUMP_SPEED_SCALE 1.2f
#define WATER_JUMP_WALL_KICK_LOCK_FRAMES 10

// Forward-only air acceleration without draining water-exit momentum.
#define WATER_JUMP_AIR_ACCEL 1.5f
#define WATER_JUMP_AIR_SPEED 32.0f

// Help a steep, low-speed water jump keep moving toward a nearby pool rim.
#define WATER_JUMP_RIM_ASSIST_PITCH DEGREES(45)
#define WATER_JUMP_RIM_ASSIST_STICK_Y -48.0f
#define WATER_JUMP_RIM_ASSIST_ACCEL 3.0f
#define WATER_JUMP_RIM_ASSIST_SPEED 20.0f
// A forward dive out of a steep water jump needs horizontal travel as well as lift.
#define WATER_JUMP_DIVE_FORWARD_RATIO 0.6f
#define WATER_JUMP_DIVE_FORWARD_CAP 36.0f

/**
 * Release B within this many updates after pressing it to attack, with A released.
 * Longer holds only swim. A tap during attack recovery/cooldown queues one attack;
 * holding A or pressing B again discards that queue in favor of swimming.
 * The final WATER_PUNCH_SWIM_CANCEL_FRAMES animation frames can cancel into swimming;
 * the same number of gameplay frames must pass after recovery ends before another attack.
 */
#define B_SWIM_ATTACK_TAP_FRAMES 6
#define AIR_ENTRY_SWIM_STROKE_TAP_FRAMES 6
#define WATER_PUNCH_SWIM_CANCEL_FRAMES 8

/**
 * Z plays the startup animation at 75% speed, waits, then gives one downward impulse.
 * Z selects full or reduced initial strength. Sinking only brakes toward its target speed.
 * Z also extends the pose. Ending it eases vertical velocity back into ordinary swimming.
 */
#define WATER_DOWN_ANIM_SPEED 0.75f
// The 11-frame startup at 0.75x reaches its last frame on gameplay update 16.
#define WATER_DOWN_IMPACT_COOLDOWN_FRAMES 16
#define WATER_DOWN_WEAK_STRENGTH 0.7f        // Released-Z strength relative to the held burst.
#define WATER_DOWN_BURST_SPEED 65.0f         // Initial downward speed; preserves faster descents.
#define WATER_DOWN_BURST_BRAKE 1.5f          // Forward braking throughout startup, delay, and sinking.
#define WATER_DOWN_BURST_VERTICAL_BRAKE 3.5f // Vertical recovery rate after the pose ends.
#define WATER_DOWN_SINK_DECEL 1.5f           // Braking from the impulse toward constant sinking speed.
#define WATER_DOWN_HOLD_SPEED 12.0f          // Constant downward target during the falling pose.
#define WATER_DOWN_BURST_FRAMES 12 // Shared minimum falling-pose duration, including the impulse frame.
#define WATER_DOWN_BURST_MAX_HOLD_FRAMES 450 // Hold cap after startup, including the delay.
#define WATER_DOWN_BURST_DELAY_FRAMES 4      // Updates after startup ends before the impulse.
