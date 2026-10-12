#pragma once

/**********************
 * BENCHMARK SETTINGS *
 **********************/

/**
 * Enabling this will enable a set of defines in order to run a benchmark on the game.
 * Currently this just starts the credits, and enables PUPPYPRINT_DEBUG.
 */
// #define ENABLE_CREDITS_BENCHMARK

#if defined(ENABLE_CREDITS_BENCHMARK) && !defined(DISABLE_ALL)
#define DEBUG_ALL
#define ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
#define TEST_LEVEL LEVEL_CASTLE_GROUNDS
#endif
