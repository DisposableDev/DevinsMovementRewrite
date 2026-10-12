#ifndef SURFACE_QUERY_H
#define SURFACE_QUERY_H

#include "config.h"
#include "macros.h"
#include "surface_load.h"

enum SurfaceQueryStorage { SURFACE_QUERY_STATIC, SURFACE_QUERY_DYNAMIC, SURFACE_QUERY_EMPTY };

#define STATIC_SURFACE_GROUP_SIZE 8

// Consecutive linked references retain the original wall-push order.
#define STATIC_WALL_HEIGHT_GROUP_SIZE 8
#define STATIC_WALL_HEIGHT_MIN_NODES 16
#define STATIC_WALL_HEIGHT_MAX_BYTES 0x4000

struct StaticWallHeightGroup {
    struct SurfaceNode *first;
    s16 lowerY, upperY;
    u16 count;
};

struct StaticWallHeightCell {
    u16 start, count;
};

struct StaticWallHeightIndex {
    struct StaticWallHeightCell *cells;
    struct StaticWallHeightGroup *groups;
    u32 generation;
};

#if STATIC_WALL_HEIGHT_GROUPS
extern struct StaticWallHeightIndex gStaticWallHeightIndex;
#endif

struct SurfaceQueryBounds {
    Vec3s minimum;
    Vec3s maximum;
};

struct StaticSurfaceIndex {
    u32 *offsets;
    struct Surface **surfaces;
    struct SurfaceQueryBounds *bounds;
    u32 generation;
};

#if STATIC_SURFACE_INDEX
extern struct StaticSurfaceIndex gStaticSurfaceIndex;
#endif

#if DYNAMIC_SURFACE_INDEX
#define DYNAMIC_SURFACE_INDEX_CELLS 32
#define DYNAMIC_SURFACE_INDEX_BYTES 0x2000

struct DynamicSurfaceIndexCell {
    u32 generation;
    u16 cell;
    u16 queries;
    s16 count; // 0: warming; -1: linked fallback; positive: packed references.
    struct Surface **surfaces;
    struct SurfaceQueryBounds *bounds;
};

struct DynamicSurfaceIndex {
    u32 generation;
    u32 used;
    s32 active;
    struct DynamicSurfaceIndexCell cells[DYNAMIC_SURFACE_INDEX_CELLS];
};

extern struct DynamicSurfaceIndex gDynamicSurfaceIndex;
void invalidate_dynamic_surface_index(void);
void build_dynamic_surface_index_cell(struct DynamicSurfaceIndexCell *cell, struct SurfaceNode *head);
#endif

// One ordered cell/partition scan. Geometry must remain fixed while it is consumed;
// never retain this cursor across object collision publication or terrain loads.
struct SurfaceQueryIterator {
    union {
        struct SurfaceNode *node;
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
        struct Surface **surfaces;
#endif
    } next;
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    // Negative for linked lists; otherwise the number of packed entries left.
    s32 remaining;
#endif
#if DYNAMIC_SURFACE_INDEX
    struct Surface **base;
    struct SurfaceQueryBounds *bounds;
#endif
};

// Wrap caller-owned filtered lists without exposing their links to the resolver.
static ALWAYS_INLINE struct SurfaceQueryIterator surface_query_list(struct SurfaceNode *head) {
    struct SurfaceQueryIterator iterator;
    iterator.next.node = head;
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    iterator.remaining = -1;
#endif
#if DYNAMIC_SURFACE_INDEX
    iterator.base = NULL;
    iterator.bounds = NULL;
#endif
    return iterator;
}

static ALWAYS_INLINE s32 surface_query_is_empty(struct SurfaceQueryIterator iterator) {
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    if (iterator.remaining >= 0) {
        return iterator.remaining == 0;
    }
#endif
    return iterator.next.node == NULL;
}

// Zero/one-candidate floor searches bypass caching because a scan is cheaper.
static ALWAYS_INLINE s32 surface_query_has_multiple(struct SurfaceQueryIterator iterator) {
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    if (iterator.remaining >= 0) {
        return iterator.remaining > 1;
    }
#endif
    return iterator.next.node != NULL && iterator.next.node->next != NULL;
}

// Identity of an unconsumed scan, for cache keys only. It must be paired with
// terrain generation and the cache's existing frame/query eligibility rules.
static ALWAYS_INLINE const void *surface_query_identity(struct SurfaceQueryIterator iterator) {
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    if (iterator.remaining >= 0) {
        return iterator.remaining == 0 ? NULL : iterator.next.surfaces;
    }
#endif
    return iterator.next.node;
}

// Presence checks use the retained roots without warming or building an index.
static ALWAYS_INLINE s32 surface_query_cell_is_empty(s32 cellX, s32 cellZ, s32 partition,
                                                     enum SurfaceQueryStorage storage) {
    if (storage == SURFACE_QUERY_STATIC) {
        return gStaticSurfacePartition[cellZ][cellX][partition] == NULL;
    }
    if (storage == SURFACE_QUERY_DYNAMIC) {
        return gDynamicSurfacePartition[cellZ][cellX][partition] == NULL;
    }
    return TRUE;
}

// Coordinates must be inside the grid unless storage is EMPTY. Surface filters,
// static/dynamic order and early exits belong to the caller's exact collision test.
static ALWAYS_INLINE struct SurfaceQueryIterator surface_query_cell(s32 cellX, s32 cellZ, s32 partition,
                                                                    enum SurfaceQueryStorage storage) {
    struct SurfaceQueryIterator iterator = surface_query_list(NULL);
    if (storage == SURFACE_QUERY_STATIC) {
#if STATIC_SURFACE_INDEX
        if (gStaticSurfaceIndex.offsets != NULL
            && gStaticSurfaceIndex.generation == gStaticSurfaceGeneration) {
            s32 cell = (cellZ * NUM_CELLS + cellX) * NUM_SPATIAL_PARTITIONS + partition;
            u32 start = gStaticSurfaceIndex.offsets[cell];
            iterator.next.surfaces = gStaticSurfaceIndex.surfaces + start;
            iterator.remaining = gStaticSurfaceIndex.offsets[cell + 1] - start;
#if DYNAMIC_SURFACE_INDEX
            iterator.base = gStaticSurfaceIndex.surfaces;
            iterator.bounds = gStaticSurfaceIndex.bounds;
#endif
            return iterator;
        }
#endif
        iterator.next.node = gStaticSurfacePartition[cellZ][cellX][partition];
    } else if (storage == SURFACE_QUERY_DYNAMIC) {
        iterator.next.node = gDynamicSurfacePartition[cellZ][cellX][partition];
#if DYNAMIC_SURFACE_INDEX
        // Empty/single-node lists stay cheap. Pack only on the fourth query so
        // occasional searches avoid rebuild work. Hash conflicts retain links.
        if (iterator.next.node != NULL && iterator.next.node->next != NULL) {
            u32 cell = (cellZ * NUM_CELLS + cellX) * NUM_SPATIAL_PARTITIONS + partition;
            struct DynamicSurfaceIndexCell *entry =
                &gDynamicSurfaceIndex.cells[(cell ^ (cell >> 5)) % DYNAMIC_SURFACE_INDEX_CELLS];
            gDynamicSurfaceIndex.active = TRUE;
            if (entry->generation != gDynamicSurfaceIndex.generation) {
                entry->generation = gDynamicSurfaceIndex.generation;
                entry->cell = cell;
                entry->queries = 1;
                entry->count = 0;
            } else if (entry->cell == cell) {
                if (entry->count == 0 && ++entry->queries == 4) {
                    build_dynamic_surface_index_cell(entry, iterator.next.node);
                }
                if (entry->count > 0) {
                    iterator.next.surfaces = entry->surfaces;
                    iterator.remaining = entry->count;
                    iterator.base = entry->surfaces;
                    iterator.bounds = entry->bounds;
                }
            }
        }
#endif
    }
    return iterator;
}

// Preserves list order and repeated references; NULL marks the end of the scan.
static ALWAYS_INLINE struct Surface *surface_query_next(struct SurfaceQueryIterator *iterator) {
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    if (iterator->remaining >= 0) {
        if (iterator->remaining == 0) {
            return NULL;
        }
        iterator->remaining--;
        return *iterator->next.surfaces++;
    }
#endif
    struct SurfaceNode *node = iterator->next.node;
    if (node == NULL) {
        return NULL;
    }
    iterator->next.node = node->next;
    return node->surface;
}

// A height group belongs only to an immutable static wall list, never a copied
// candidate list or live moving mesh. Check generation before touching pool data.
static ALWAYS_INLINE const struct StaticWallHeightGroup *static_wall_height_groups(s32 cellX, s32 cellZ,
                                                                                   f32 y, s32 *count) {
    *count = 0;
#if STATIC_WALL_HEIGHT_GROUPS
    if (gStaticWallHeightIndex.cells != NULL
        && gStaticWallHeightIndex.generation == gStaticSurfaceGeneration) {
        const struct StaticWallHeightCell *cell =
            &gStaticWallHeightIndex.cells[cellZ * NUM_CELLS + cellX];
        const struct StaticWallHeightGroup *groups = gStaticWallHeightIndex.groups + cell->start;
        s32 rejected = 0;
        // Group bookkeeping pays off when it removes substantial traversal.
        // Use half the maximum list length, with at least one full group saved.
        s32 minimumRejected =
            MAX(STATIC_WALL_HEIGHT_GROUP_SIZE, cell->count * STATIC_WALL_HEIGHT_GROUP_SIZE / 2);
        for (s32 i = 0; i < cell->count; i++) {
            if (y < groups[i].lowerY || y > groups[i].upperY) {
                rejected += groups[i].count;
                if (rejected >= minimumRejected) {
                    *count = cell->count;
                    return groups;
                }
            }
        }
    }
#else
    (void) cellX;
    (void) cellZ;
    (void) y;
#endif
    return NULL;
}

// The wall solver pushes X/Z only; its probe height stays fixed throughout a
// list. A rejected group contains no wall that could pass the original Y test.
static ALWAYS_INLINE struct Surface *surface_query_next_wall(struct SurfaceQueryIterator *iterator,
                                                             const struct StaticWallHeightGroup **group,
                                                             s32 *groupCount, s32 *remaining, f32 y) {
#if STATIC_WALL_HEIGHT_GROUPS
    if (*group != NULL) {
        while (*groupCount > 0) {
            if (*remaining == 0) {
                if (y < (*group)->lowerY || y > (*group)->upperY) {
                    (*group)++;
                    (*groupCount)--;
                    continue;
                }
                // Also reset the packed iterator's tag when using linked groups.
                *iterator = surface_query_list((*group)->first);
                *remaining = (*group)->count;
            }
            struct Surface *surface = surface_query_next(iterator);
            if (--(*remaining) == 0) {
                (*group)++;
                (*groupCount)--;
            }
            return surface;
        }
        return NULL;
    }
#else
    (void) group;
    (void) groupCount;
    (void) remaining;
    (void) y;
#endif
    return surface_query_next(iterator);
}

// Skip only complete, aligned groups outside an immutable query region. Short
// lists and partial end groups keep the ordinary scan; surviving order is intact.
static ALWAYS_INLINE struct Surface *surface_query_next_in_bounds(struct SurfaceQueryIterator *iterator,
                                                                  const Vec3f minimum,
                                                                  const Vec3f maximum) {
#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
    while (iterator->remaining >= STATIC_SURFACE_GROUP_SIZE) {
#if DYNAMIC_SURFACE_INDEX
        u32 offset = iterator->next.surfaces - iterator->base;
#else
        u32 offset = iterator->next.surfaces - gStaticSurfaceIndex.surfaces;
#endif
        if (offset % STATIC_SURFACE_GROUP_SIZE != 0) {
            break;
        }
#if DYNAMIC_SURFACE_INDEX
        const struct SurfaceQueryBounds *bounds = &iterator->bounds[offset / STATIC_SURFACE_GROUP_SIZE];
#else
        const struct SurfaceQueryBounds *bounds =
            &gStaticSurfaceIndex.bounds[offset / STATIC_SURFACE_GROUP_SIZE];
#endif
        if (!(bounds->minimum[0] > maximum[0] || bounds->maximum[0] < minimum[0]
              || bounds->minimum[1] > maximum[1] || bounds->maximum[1] < minimum[1]
              || bounds->minimum[2] > maximum[2] || bounds->maximum[2] < minimum[2])) {
            break;
        }
        iterator->next.surfaces += STATIC_SURFACE_GROUP_SIZE;
        iterator->remaining -= STATIC_SURFACE_GROUP_SIZE;
    }
#else
    (void) minimum;
    (void) maximum;
#endif
    return surface_query_next(iterator);
}

#endif
