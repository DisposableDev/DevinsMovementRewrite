#include <ultra64.h>

#include "sm64.h"
#include "config.h"
#include "engine/math_util.h"
#include "area.h"
#include "emutest.h"
#include "level_table.h"
#include "memory.h"
#include "physics_lab.h"
#include "rendering_graph_node.h"
#include "levels/physics_lab/header.h"

Gfx *geo_physics_lab_sculptures(s32 callContext, struct GraphNode *node, Mat4 model) {
    if (callContext != GEO_CONTEXT_RENDER || gCurrLevelNum != LEVEL_PHYSICS_LAB) {
        return NULL;
    }
    SET_GRAPH_NODE_LAYER(node->flags, LAYER_OPAQUE);
    Gfx *fallback = segmented_to_virtual(physics_lab_sculptures_dl);
    if (gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL) {
        return fallback;
    }
#ifndef CULLING_ON_EMULATOR
    if (gEmulator & (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)) {
        return fallback;
    }
#endif
    // One common translation/scale preserves quarter-unit vertices without
    // overflowing s16. Generated world bounds are valid only in this basis.
    for (s32 row = 0; row < 4; row++) {
        for (s32 col = 0; col < 4; col++) {
            f32 expected = 0.0f;
            if (row == col) {
                expected = row == 3 ? 1.0f : LAB_SCULPTURE_SCALE;
            }
            if (row == 3 && col == 0) {
                expected = LAB_SCULPTURE_ORIGIN_X;
            }
            if (row == 3 && col == 2) {
                expected = LAB_SCULPTURE_ORIGIN_Z;
            }
            if (model[row][col] != expected) {
                return fallback;
            }
        }
    }
    const struct PhysicsLabSculpture *sculptures = segmented_to_virtual(physics_lab_sculptures);
    u32 count = *(u32 *) segmented_to_virtual(&physics_lab_sculpture_count);
    Gfx *list = alloc_display_list((count + 3) * sizeof(*list));
    if (list == NULL) {
        return fallback;
    }
    Gfx *head = list;
    gSPDisplayList(head++, physics_lab_terrain_begin);
    for (u32 i = 0; i < count; i++) {
        const struct PhysicsLabSculpture *s = &sculptures[i];
        gSPDisplayList(head++, physics_lab_use_distant_mesh(&s->bounds, s->slot, 0.0f)
                                   ? s->distant
                                   : s->bounds.displayList);
    }
    gSPDisplayList(head++, physics_lab_terrain_end);
    gSPEndDisplayList(head);
    return list;
}
