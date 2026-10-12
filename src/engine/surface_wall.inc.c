// Compile the same wall-push algorithm with ordinary and height-group cursors.
// Separate hot loops keep short lists, copied candidates and moving walls cheap.
// WALL_HEIGHT_GROUP_SCAN is local to surface_collision.c.
// Keep the grouped loop out of cell dispatch so fallback scans do not fetch it.
/** Iterate through the walls and apply each applicable wall push. */
#if WALL_HEIGHT_GROUP_SCAN
static __attribute__((noinline)) s32 find_wall_collisions_from_height_groups(
    struct SurfaceQueryIterator candidates, struct WallCollisionData *data,
    const struct WallCollisionCache *cache, s32 *leftCache, const struct StaticWallHeightGroup *groups,
    s32 groupCount) {
    s32 groupRemaining = 0;
#else
static s32 find_wall_collisions_from_list(struct SurfaceQueryIterator candidates,
                                          struct WallCollisionData *data,
                                          const struct WallCollisionCache *cache, s32 *leftCache) {
#endif
    const f32 corner_threshold = -0.9f;
    struct Surface *surf;
    f32 offset;
    f32 radius = data->radius;

    Vec3f pos = { data->x, data->y + data->offsetY, data->z };
    Vec3f v0, v1, v2;
    f32 d00, d01, d11, d20, d21;
    TerrainData type = SURFACE_DEFAULT;
    s32 numCols = 0;

    f32 margin_radius = radius - 1.0f;

    // Stay in this loop until out of walls.
#if WALL_HEIGHT_GROUP_SCAN
    while ((surf = surface_query_next_wall(&candidates, &groups, &groupCount, &groupRemaining, pos[1]))
           != NULL) {
#else
    while ((surf = surface_query_next(&candidates)) != NULL) {
#endif
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_WALL_NODES);
#endif
        type = surf->type;

        // Exclude a large number of walls immediately to optimize.
        if (pos[1] < surf->lowerY || pos[1] > surf->upperY) {
            continue;
        }

        if (type == SURFACE_MARIO_ONLY_GRIPPY && surface_is_passable_vanish(surf)) {
            continue;
        }

        // Determine if checking for the camera or not.
        if (gCollisionFlags & COLLISION_FLAG_CAMERA) {
            if (surf->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                continue;
            }
        } else {
            // Ignore camera only surfaces.
            if (type == SURFACE_CAMERA_BOUNDARY) {
                continue;
            }

            if (surface_is_passable_vanish(surf)) {
                continue;
            }
        }

        // Dot of normal and pos, + origin offset
        offset = (surf->normal.x * pos[0]) + (surf->normal.y * pos[1]) + (surf->normal.z * pos[2])
                 + surf->originOffset;

        // Exclude surfaces outside of the radius.
        if (offset < -radius || offset > radius) {
            continue;
        }

        // Walls sharing a plane may be far apart in a large partition cell.
        // The edge solver's margin can grow after earlier wall pushes.
        f32 reach = MAX(radius, margin_radius) + 1.0f;
        if (pos[0] < MIN(surf->vertex1[0], MIN(surf->vertex2[0], surf->vertex3[0])) - reach
            || pos[0] > MAX(surf->vertex1[0], MAX(surf->vertex2[0], surf->vertex3[0])) + reach
            || pos[2] < MIN(surf->vertex1[2], MIN(surf->vertex2[2], surf->vertex3[2])) - reach
            || pos[2] > MAX(surf->vertex1[2], MAX(surf->vertex2[2], surf->vertex3[2])) + reach) {
            continue;
        }

        vec3_diff(v0, surf->vertex2, surf->vertex1);
        vec3_diff(v1, surf->vertex3, surf->vertex1);
        vec3_diff(v2, pos, surf->vertex1);

        // Face
        d00 = vec3_dot(v0, v0);
        d01 = vec3_dot(v0, v1);
        d11 = vec3_dot(v1, v1);
        d20 = vec3_dot(v2, v0);
        d21 = vec3_dot(v2, v1);

        f32 mult = (d00 * d11) - (d01 * d01);
        if (check_wall_vw(d00, d01, d11, d20, d21, mult)) {
            if (offset < 0) {
                continue;
            }

            // Edge 1-2
            f32 invDenom;
            if (check_wall_edge(v0, v2, &d00, &d01, &invDenom, &offset, margin_radius)) {
                // Edge 1-3
                if (check_wall_edge(v1, v2, &d00, &d01, &invDenom, &offset, margin_radius)) {
                    vec3_diff(v1, surf->vertex3, surf->vertex2);
                    vec3_diff(v2, pos, surf->vertex2);
                    // Edge 2-3
                    if (check_wall_edge(v1, v2, &d00, &d01, &invDenom, &offset, margin_radius)) {
                        continue;
                    }
                }
            }

            // Check collision
            if (FLT_IS_NONZERO(invDenom)) {
                invDenom = (offset / invDenom);
            }

            // Update pos
            pos[0] += (d00 *= invDenom);
            pos[2] += (d01 *= invDenom);
            margin_radius += 0.01f;

            if (cache != NULL
                && (pos[0] < cache->minX || pos[0] > cache->maxX || pos[2] < cache->minZ
                    || pos[2] > cache->maxZ)) {
                *leftCache = TRUE;
                return numCols;
            }

            if ((d00 * surf->normal.x) + (d01 * surf->normal.z) < (corner_threshold * offset)) {
                continue;
            }
        } else {
            // Update pos
            pos[0] += surf->normal.x * (radius - offset);
            pos[2] += surf->normal.z * (radius - offset);

            if (cache != NULL
                && (pos[0] < cache->minX || pos[0] > cache->maxX || pos[2] < cache->minZ
                    || pos[2] > cache->maxZ)) {
                *leftCache = TRUE;
                return numCols;
            }
        }

        // Containment pushes position without supplying a wall for Mario actions.
        if (type == SURFACE_NON_INTERACTIVE_BOUNDARY) {
            continue;
        }

        // Has collision
        if (data->numWalls < MAX_REFERENCED_WALLS) {
            data->walls[data->numWalls++] = surf;
        }
        numCols++;

        if (gCollisionFlags & COLLISION_FLAG_RETURN_FIRST) {
            break;
        }
    }

    data->x = pos[0];
    data->z = pos[2];
    return numCols;
}
