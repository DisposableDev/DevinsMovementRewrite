#pragma once

/*******************
 * CAMERA SETTINGS *
 *******************/

/**
 * Forces the camera mode to your choice (except when swimming or flying).
 * It does this by setting the area's camera->defMode to this mode.
 */
#define FORCED_CAMERA_MODE        CAMERA_MODE_8_DIRECTIONS

/**
 * Changes hardcoded camera mode reverting to instead use the area's default mode (defMode).
 * If you're using a FORCED_CAMERA_MODE, this must be on for it to work.
 */
#define USE_COURSE_DEFAULT_MODE

/***** Movement specific camera modes: *****/
/**
 * Camera mode when Mario is less than 280 units below the water surface.
 * The default is CAMERA_MODE_WATER_SURFACE.
 */
#define WATER_SURFACE_CAMERA_MODE CAMERA_MODE_WATER_SURFACE
/**
 * Camera mode when Mario is more than 480 units below the water surface.
 * The default is CAMERA_MODE_BEHIND_MARIO.
 */
#define DEEP_WATER_CAMERA_MODE    CAMERA_MODE_BEHIND_MARIO
/**
 * Camera mode when Mario is flying (default is CAMERA_MODE_BEHIND_MARIO).
 */
#define FLYING_CAMERA_MODE        CAMERA_MODE_BEHIND_MARIO
/*******************************************/

/**
 * Makes the camera approach Mario's height much more quickly.
 */
#define FAST_VERTICAL_CAMERA_MOVEMENT

/**
 * Enables "parallel lakitu camera" or "aglab cam" which lets you move the camera smoothly with the D-pad.
 */
#define PARALLEL_LAKITU_CAM

// Enables the Reonucam camera controls.
#define REONUCAM

/**********************************/
/***** Vanilla config options *****/
/**********************************/

/**
 * Allow course specific camera processing.
 * You will likely want this disabled in non-vanilla hacks.
 * Enabling this will automatically disable FORCED_CAMERA_MODE and USE_COURSE_DEFAULT_MODE.
 */
// #define ENABLE_VANILLA_CAM_PROCESSING

// Reonucam requires the eight-direction camera.
#ifdef REONUCAM
#undef FORCED_CAMERA_MODE
#define FORCED_CAMERA_MODE CAMERA_MODE_8_DIRECTIONS
#ifndef USE_COURSE_DEFAULT_MODE
#define USE_COURSE_DEFAULT_MODE
#endif
#undef ENABLE_VANILLA_CAM_PROCESSING
#endif
