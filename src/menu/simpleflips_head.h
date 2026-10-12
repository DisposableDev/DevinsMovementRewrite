#ifndef SIMPLEFLIPS_HEAD_H
#define SIMPLEFLIPS_HEAD_H

#include "types.h"
#include "engine/graph_node.h"

// These match the game-over head's camera and geometry scale.
#define SIMPLEFLIPS_HEAD_SCALE 1.5f
#define SIMPLEFLIPS_HEAD_GEO_SCALE 98304
#define SIMPLEFLIPS_HEAD_CAMERA_DISTANCE 1000.0f
#define SIMPLEFLIPS_HEAD_FOCAL_LENGTH 289.70563f

// Immutable segmented source vertices, paired with flattened G_VTX loads in command order.
struct SimpleFlipsHeadVertexGroup {
    const Vtx *vertices;
    u16 count;
};

struct SimpleFlipsHeadMesh {
    const Gfx *displayList;
    const struct SimpleFlipsHeadVertexGroup *groups;
    u16 commandCount;
    u16 groupCount;
    u16 vertexCount;
};

// Create resets session controls; render owns input sampling and frame-owned deformation.
Gfx *geo_simpleflips_head(s32 callContext, struct GraphNode *node, void *context);
// Runs after the head callback and reads its current screen-pixel cursor/grab state.
Gfx *geo_simpleflips_head_cursor(s32 callContext, struct GraphNode *node, void *context);

#endif
