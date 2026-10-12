#pragma once

/******************
 * DEBUG SETTINGS *
 ******************/

/**
 * Enables most debug options, including PUPPYPRINT_DEBUG.
 */
// #define DEBUG_ALL

/**
 * Release preset: disables developer shortcuts, automatic unlocks, capture hooks,
 * uncapped FPS and assertions. Keeps player-controlled debug tools and crash diagnostics.
 * Explicit debug, benchmark and capture builds retain their developer configuration.
 */
#if !defined(DISABLE_ALL) && !defined(DEBUG) && !defined(DEBUG_ALL)                                    \
    && !defined(ENABLE_CREDITS_BENCHMARK) && !PERFORMANCE_CAPTURE && !COLLISION_PROFILING              \
    && !GCC_PROFILE
#define DISABLE_ALL
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Enables a comprehensive standalone profiler. Automatically enabled by PUPPYPRINT_DEBUG.
 * If not using PUPPYPRINT_DEBUG, press L + D-Pad Up to toggle the profiler.
 */
#define USE_PROFILER

/**
 * -- TEST LEVEL --
 * Uncomment TEST_LEVEL and set TEST_ACT to boot straight into that course and act.
 * This allows you to quickly test the level you're working on.
 * If you want the game to boot normally, just comment out the define again.
 */
// #define TEST_LEVEL LEVEL_PHYSICS_LAB
// #define TEST_ACT 1

/**
 * Enables debug level select. Hold L while the game boots to turn it on.
 */
#define DEBUG_LEVEL_SELECT

/**
 * Enables debug free move (D-pad up to enter, A to exit).
 */
#define ENABLE_DEBUG_FREE_MOVE

/**
 * Enables a custom, enhanced performance profiler. (Enables PUPPYPRINT by default in config_safeguards).
 */
#define PUPPYPRINT_DEBUG

// Emulator-only frame export. The launcher selects a separate diagnostic build.
#ifndef PERFORMANCE_CAPTURE
#define PERFORMANCE_CAPTURE 0
#endif

/**
 * Enables detailed collision capture for diagnostic builds. The counter hooks add
 * per-candidate overhead even with capture closed, so normal builds keep them out.
 * Set to 1 and open the Collision debug page, or use play/aplay --collision.
 * The standard profiler
 * remains available at 0. Requires PUPPYPRINT_DEBUG and USE_PROFILER.
 */
#ifndef COLLISION_PROFILING
#define COLLISION_PROFILING 0
#endif

/**
 * Uses cycles instead of microseconds in Puppyprint debug output.
 */
// #define PUPPYPRINT_DEBUG_CYCLES

/**
 * A vanilla style debug mode. It doesn't rely on a text engine, but it's much less powerful that PUPPYPRINT_DEBUG.
 * Press D-pad left to show the debug UI.
 */
// #define VANILLA_STYLE_CUSTOM_DEBUG

/**
 * Visual debug enables some collision visuals. Tapping Right on the D-pad will cycle between visual hitboxes, visual surfaces, both, and neither.
 * If puppyprint is enabled, then this can be cycled only while the screen is active.
 */
// #define VISUAL_DEBUG

/**
 * Opens all courses and doors. Used for debugging purposes to unlock all content.
 */
// #define UNLOCK_ALL

/**
 * Same as above, but also reads all save file flags as complete.
 * This will not overwrite existing save file data unless you save over it.
 */
// #define COMPLETE_SAVE_FILE

/**
 * Removes the limit on FPS.
 */
// #define UNLOCK_FPS

/**
 * Includes vanilla debug functionality.
 */
// #define VANILLA_DEBUG

/**
 * Intentionally crash the game whenever a runtime assertion fails (also invoked by the DEBUG define in the Makefile).
 */
#define DEBUG_ASSERTIONS
