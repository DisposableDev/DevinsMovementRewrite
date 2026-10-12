#ifndef COMPACT_EXPLOSION_BUBBLES_H
#define COMPACT_EXPLOSION_BUBBLES_H

#include "types.h"

#define COMPACT_EXPLOSION_BUBBLE_CAPACITY 128

void compact_explosion_bubbles_reset(void);
// TRUE allocated a bubble; FALSE means capacity was exhausted, without an RNG draw.
s32 compact_explosion_bubble_spawn(struct Object *parent);
// Advance once after the default-object list completes; skipped during time stop.
void compact_explosion_bubbles_update(void);
s32 compact_explosion_bubbles_count(void);
// index is a storage slot, not an ordinal among count() active bubbles.
// TRUE writes world position, render scale, and area; FALSE leaves outputs unchanged.
s32 compact_explosion_bubble_view(s32 index, Vec3f pos, Vec3f scale, s16 *areaIndex);

#endif
