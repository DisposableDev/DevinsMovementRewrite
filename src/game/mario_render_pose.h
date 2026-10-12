#ifndef MARIO_RENDER_POSE_H
#define MARIO_RENDER_POSE_H

#include "types.h"

enum MarioPoseRootSpace {
    MARIO_POSE_NATIVE_ROOT,
    MARIO_POSE_RENDER_ROOT,
};

// Describes the displayed pose only. Native roots still need animation flags
// and rootScale; render roots already contain their final translation.
// frame indexes values/index and need not equal the gameplay animation frame.
struct MarioRenderPose {
    s16 *values;
    u16 *index;
    s16 frame;
    f32 rootScale;
    enum MarioPoseRootSpace rootSpace;
};

#endif
