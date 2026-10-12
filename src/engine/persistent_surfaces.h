#ifndef PERSISTENT_SURFACES_H
#define PERSISTENT_SURFACES_H

#include "config.h"
#include "types.h"

#if (PERSISTENT_DYNAMIC_SURFACES || DORRIE_STATIONARY_SURFACES || JRB_STATIONARY_SURFACES              \
     || BOWSER_STATIONARY_SURFACES)                                                                    \
    && defined(ALL_SURFACES_HAVE_FORCE)
// Called after the dynamic grid is cleared, never during time stop.
void persistent_surfaces_begin_frame(void);
void persistent_surfaces_reset(void);
void persistent_surfaces_forget(struct Object *object);
// FALSE leaves the grid untouched so the ordinary loader can handle this publication.
s32 persistent_surfaces_publish(struct Object *object, const TerrainData *data, Mat4 transform,
                                RoomData room);
u32 persistent_surfaces_reserved_bytes(void);
u32 persistent_surfaces_used_bytes(void);
u32 persistent_surfaces_published_meshes(void);
#else
#define persistent_surfaces_begin_frame() ((void) 0)
#define persistent_surfaces_reset() ((void) 0)
#define persistent_surfaces_forget(object) ((void) 0)
#define persistent_surfaces_publish(object, data, transform, room) FALSE
#define persistent_surfaces_reserved_bytes() 0u
#define persistent_surfaces_used_bytes() 0u
#define persistent_surfaces_published_meshes() 0u
#endif

#if DORRIE_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
// Only an unchanged pose attempts reuse; FALSE requires ordinary publication.
s32 persistent_surfaces_publish_dorrie(struct Object *object, const TerrainData *data, Mat4 transform,
                                       Vec3f scale);
#else
#define persistent_surfaces_publish_dorrie(object, data, transform, scale) FALSE
#endif

#if JRB_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
s32 persistent_surfaces_publish_jrb_ship(struct Object *object, const TerrainData *data, Mat4 transform,
                                         Vec3f scale);
#else
#define persistent_surfaces_publish_jrb_ship(object, data, transform, scale) FALSE
#endif

#if BOWSER_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
s32 persistent_surfaces_publish_bowser_platform(struct Object *object, const TerrainData *data,
                                                Mat4 transform, Vec3f scale);
#else
#define persistent_surfaces_publish_bowser_platform(object, data, transform, scale) FALSE
#endif

#endif
