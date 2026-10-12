// Real surface queries/raycast traversal and water movement; synthetic geometry
// and minimal Mario/graphics state. Current generation is a controlled fixture.
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include "test_support.h"
#define bzero(p, n) memset(p, 0, n)
typedef float f32;
typedef double f64;
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int TerrainData;
typedef int SurfaceType;
typedef float Vec3f[3];
typedef float Mat4[4][4];
#define MTXF_END(m)                                                                                    \
    do {                                                                                               \
        (m)[0][3] = (m)[1][3] = (m)[2][3] = 0;                                                         \
        (m)[3][3] = 1;                                                                                 \
    } while (0)
#define vec3_add(a, b)                                                                                 \
    do {                                                                                               \
        for (int axis_ = 0; axis_ < 3; axis_++)                                                        \
            (a)[axis_] += (b)[axis_];                                                                  \
    } while (0)
typedef int16_t Vec3s[3];
typedef int32_t Vec3i[3];
#define UNUSED
#define COND_BIT(test, bits, mask)                                                                     \
    do {                                                                                               \
        if (test)                                                                                      \
            (bits) |= (mask);                                                                          \
        else                                                                                           \
            (bits) &= ~(mask);                                                                         \
    } while (0)
#define TRUE 1
#define FALSE 0
#define ALWAYS_INLINE static inline
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) MIN(MAX(x, a), b)
#define sqr(x) ((x) * (x))
#define absf fabsf
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define NEAR_ZERO __FLT_EPSILON__
#define NEAR_ONE (1.0f - __FLT_EPSILON__)
#define FLT_IS_NONZERO(x) (absf(x) > NEAR_ZERO)
#define DEGREES(x) ((s16) ((x) * 65536.0f / 360.0f))
#define MAX_REFERENCED_WALLS 4
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() ((void) 0)
#define PUPPYPRINT_ADD_COUNTER(x) (collisionQueryCount++)
#define profiler_collision_update(x) (collisionTimedQueries++)
struct Normal {
    f32 x, y, z;
};
struct Surface {
    TerrainData type;
    s16 flags, lowerY, upperY, room;
    Vec3s vertex1, vertex2, vertex3;
    struct Normal normal;
    f32 originOffset;
    struct Object *object;
};
#define oPosVec location[0]
#define oPosX location[0]
#define oPosY location[1]
#define oPosZ location[2]
struct SurfaceNode {
    struct SurfaceNode *next;
    struct Surface *surface;
};
struct Object {
    int activeFlags;
    Vec3f location;
    f32 oWallHitboxRadius, hitboxHeight, oVelX, oVelY, oVelZ, oForwardVel;
    f32 oDragStrength, oGravity, oBuoyancy, oBounciness, oFloorHeight;
    s16 oMoveAngleYaw, oFloorType, oFloorRoom, oWallAngle, oFaceAngleYaw;
    f32 hitboxRadius, oGraphYOffset;
    s32 oGroundTwirlStunTimer, oGroundTwirlContactRebound, oGroundTwirlStunDuration;
    u32 metalContactPendingDefeat, metalContactCooldown, oInteractStatus;
    int behavior, oAction, oTimer, oDeathSound, oRoom, oMontyMoleHoleCooldown, oBehParams2ndByte;
    f32 oMontyMoleHeightRelativeToFloor;
    s16 oFaceAnglePitch, oAngleToMario;
    struct Object *prevObj, *oMontyMoleCurrentHole;
    u32 oMoveFlags;
    struct Surface *oFloor;
    struct {
        struct {
            Vec3f pos, scale;
            Vec3s angle;
            struct {
                u32 flags;
            } node;
        } gfx;
    } header;
};
#define A_BUTTON 0x8000
struct Controller {
    f32 stickX, stickY;
    u16 buttonDown;
};
struct BodyState {
    Vec3s headAngle;
};
struct MarioState {
    f32 peakHeight;
    u16 actionState;
    u8 spawnFallProtection;
    u8 waterWalkCooldown;
    u8 waterWalkDisabled;
    u8 waterWalkTwirl, rolloutFrames, airTwirlFlightRollFrame;
    Vec3f pos, vel;
    Vec3f prevPos;
    Vec3s faceAngle, angleVel;
    s16 wallYaw;
    f32 forwardVel, slideVelX, slideVelZ, floorHeight, ceilHeight, waterLevel;
    u32 flags, action, input, particleFlags;
    u32 prevAction;
    struct Surface *floor;
    struct Surface *ceil;
    struct Surface *wall;
    struct Object *marioObj;
    struct Object *heldObj;
    struct Controller *controller;
    struct BodyState *marioBodyState;
};
#include "swimming_definitions.inc"
#define LEVEL_BBH 4
#define LEVEL_BOWSER_3 34
static s16 gCurrLevelNum = LEVEL_BBH;
static int sPalette;
static int mario_character_palette(void) {
    return sPalette;
}
static struct SurfaceNode *gStaticSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
static struct SurfaceNode *gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
#if STATIC_SURFACE_INDEX
static struct StaticSurfaceIndex gStaticSurfaceIndex;
#endif
#if STATIC_SURFACE_INDEX || STATIC_WALL_HEIGHT_GROUPS
static union {
    void *alignment;
    u8 bytes[0x200000];
} staticIndexPool;
static void *gCurrStaticSurfacePool = staticIndexPool.bytes;
static void *gCurrStaticSurfacePoolEnd = staticIndexPool.bytes;
#endif
#if STATIC_WALL_HEIGHT_GROUPS
static struct StaticWallHeightIndex gStaticWallHeightIndex;
#endif
#if DYNAMIC_SURFACE_INDEX
static struct DynamicSurfaceIndex gDynamicSurfaceIndex = { .generation = 1 };
static union {
    struct Surface *alignment;
    u8 bytes[DYNAMIC_SURFACE_INDEX_BYTES];
} sDynamicSurfaceIndexData;
#endif
static struct MarioState mario, *gMarioState = &mario;
static struct Object object, *gMarioObject = &object, *o = &object;
static struct Object *gCurrentObject = &object;
// Provider history executes in platform.floor_rise; control its result here
// while exercising native floor queries against the shipped elevator mesh.
static f32 verifiedFloorRise;
static f32 object_platform_motion_floor_rise_at(struct Object *obj, const Vec3f point,
                                                const Vec3f normal) {
    (void) point;
    (void) normal;
    return obj == &object ? verifiedFloorRise : 0.0f;
}
static int nativeEnemyUpdates, nativeMoleAI, waterDeathEligible;
void bhv_monty_mole_update(void);
static const int bhvHeaveHo = 1, bhvMontyMole = 2, bhvSnufit = 3, bhvSwoop = 4;
static s32 mario_ground_twirl_is_enemy(struct Object *obj) {
    return obj == &object;
}
static s32 obj_has_behavior(struct Object *obj, int behavior) {
    return obj->behavior == behavior;
}
static s32 find_water_level(s32 x, s32 z);
static s32 ground_twirl_enemy_needs_water_update(struct Object *obj) {
    return waterDeathEligible && find_water_level(obj->oPosX, obj->oPosZ) >= obj->oPosY;
}
static void cur_obj_defeat_metal_contact_enemy(void) {
    fprintf(stderr, "HARNESS ERROR: unexpected Metal defeat dispatch\n");
    exit(2);
}
static void cur_obj_update(void) {
    nativeEnemyUpdates++;
    if (nativeMoleAI) {
        bhv_monty_mole_update();
    }
}
static s16 gCollisionFlags;
static int gNumFindFloorMisses;
static u32 gGlobalTimer, gStaticSurfaceGeneration;
static u32 floorNodeVisits, collisionQueryCount, collisionTimedQueries;
static struct {
    f32 surfaceExitVelocityY;
    int burstRecovering;
    u8 rollFrame;
    s32 rollStartAngle;
    Vec3f rollFloorNormal;
    u8 rollFloorContact;
    s16 rollWallYaw;
    u8 rollWallContact;
    u8 rollBounceCount;
} sSwimming;
static Vec3f current;
static int currentCalls;
static struct Controller controller;
static struct BodyState body;
static s16 sBobTimer, sBobIncrement;
static f32 sBobHeight;
#define vec3f_set(a, x, y, z)                                                                          \
    do {                                                                                               \
        (a)[0] = (x);                                                                                  \
        (a)[1] = (y);                                                                                  \
        (a)[2] = (z);                                                                                  \
    } while (0)
#define vec3s_set vec3f_set
#define vec3_diff(a, b, c)                                                                             \
    do {                                                                                               \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] = (b)[j_] - (c)[j_];                                                               \
    } while (0)
#define vec3f_diff vec3_diff
#define vec3f_sum(a, b, c)                                                                             \
    do {                                                                                               \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] = (b)[j_] + (c)[j_];                                                               \
    } while (0)
#define vec3_dot(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define vec3f_dot vec3_dot
#define vec3_scale_dest(a, b, s)                                                                       \
    do {                                                                                               \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] = (b)[j_] * (s);                                                                   \
    } while (0)
#define vec3_mag(a) sqrtf(vec3_dot(a, a))
#define vec3f_normalize(a)                                                                             \
    do {                                                                                               \
        f32 mag_ = vec3_mag(a);                                                                        \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] /= mag_;                                                                           \
    } while (0)
#define vec3s_to_vec3f(a, b)                                                                           \
    do {                                                                                               \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] = (b)[j_];                                                                         \
    } while (0)
static void vec3f_cross(Vec3f a, Vec3f b, Vec3f c) {
    vec3f_set(a, b[1] * c[2] - b[2] * c[1], b[2] * c[0] - b[0] * c[2], b[0] * c[1] - b[1] * c[0]);
}
static int signum_positive(f32 x) {
    return x < 0 ? -1 : 1;
}
static f32 sins(s16 x) {
    return sinf(x * (6.28318530718f / 65536.0f));
}
static f32 coss(s16 x) {
    return cosf(x * (6.28318530718f / 65536.0f));
}
static s16 atan2s(f32 y, f32 x) {
    return (s16) (atan2f(x, y) * (65536.0f / 6.28318530718f));
}
static s32 approach_s32(s32 x, s32 target, s32 inc, s32 dec) {
    return x < target ? MIN(x + inc, target) : MAX(x - dec, target);
}
static f32 approach_f32(f32 x, f32 target, f32 inc, f32 dec) {
    return x < target ? MIN(x + inc, target) : MAX(x - dec, target);
}
static void mario_play_wall_bonk_sound(struct MarioState *m) {
    (void) m;
}
// Surface particles and sound do not alter the submerged movement under test.
static void set_swimming_at_surface_particles(struct MarioState *m, u32 flag) {
    (void) m;
    (void) flag;
}
static void set_mario_floor(struct MarioState *m, struct Surface *floor, f32 height) {
    m->floor = floor;
    m->floorHeight = height;
}
static void apply_water_current(struct MarioState *m, Vec3f step) {
    (void) m;
    currentCalls++;
    for (int i = 0; i < 3; i++) {
        step[i] += current[i];
    }
}
f32 find_floor(f32, f32, f32, struct Surface **);
static f32 enemyWater = -10000;
static s32 find_water_level(s32 x, s32 z) {
    return enemyWater;
}
static s32 clear_move_flag(u32 *bits, s32 flag);
static s16 abss(s16 value) {
    return value < 0 ? -value : value;
}
s32 find_wall_collisions(struct WallCollisionData *);
#define SURFACE_YAW(s) atan2s((s)->normal.z, (s)->normal.x)
f32 moving_body_support(struct MovingBodyProfile *, Vec3f, Vec3f);
static u32 stunnedCastCount, stunnedLinearSolveCount, stunnedTriangleNormalCount;
static u32 waterRaycastCount, waterEdgePlaneCount;
static u32 waterRayTriangleCount, waterVerticalNodeCount, waterRayNodeCount;
static s32 bypassStaticGroups, bypassWaterEdgeGroups, bypassDynamicIndex;
static u32 dynamicIndexBuildCount, dynamicIndexReferenceCount;
static u32 waterEdgeNodeCount;
static s32 bypassWaterRayBounds, bypassWaterContacts, bypassWaterQueries;
static s32 counted_stunned_triangle_cast(struct MovingBodyProfile *, Vec3f, Vec3f, Vec3f[3], Vec3f[3],
                                         f32 *, Vec3f);
// Native Mole action/movement logic runs below. Animation events and attack
// eligibility are controlled services; defeat cleanup is tested by metal.enemy_roster.
#define SOUND_OBJ_DYING_ENEMY1 1
#define SOUND_OBJ_MONTY_MOLE_ATTACK 2
#define MODEL_PEBBLE 3
static const int bhvMontyMoleRock = 3;
static int sMontyMoleHitbox, moleHidden;
static struct Object moleHole, moleRock;
static void cur_obj_init_animation_with_sound(int animation) {
    (void) animation;
}
static void cur_obj_init_anim_extend(int animation) {
    (void) animation;
}
static int cur_obj_check_if_near_animation_end(void) {
    return FALSE;
}
static int cur_obj_init_anim_and_check_if_end(int animation) {
    (void) animation;
    return FALSE;
}
static int cur_obj_init_anim_check_frame(int animation, int frame) {
    (void) animation;
    (void) frame;
    return FALSE;
}
static int obj_is_near_to_and_facing_mario(f32 distance, int angle) {
    (void) distance;
    (void) angle;
    return FALSE;
}
static void cur_obj_play_sound_2(int sound) {
    (void) sound;
}
static struct Object *spawn_object(struct Object *parent, int model, int behavior) {
    (void) parent;
    (void) model;
    (void) behavior;
    fprintf(stderr, "HARNESS ERROR: unexpected Mole rock spawn\n");
    exit(2);
}
static void monty_mole_act_select_hole(void) {
    fprintf(stderr, "HARNESS ERROR: unexpected hole selection\n");
    exit(2);
}
static void monty_mole_defeat(void) {
    fprintf(stderr, "HARNESS ERROR: unexpected Mole defeat\n");
    exit(2);
}
static int obj_check_attacks(void *hitbox, int action) {
    (void) hitbox;
    (void) action;
    return FALSE;
}
static void monty_mole_spawn_dirt_particles(int offset, int speed) {
    (void) offset;
    (void) speed;
}
static void cur_obj_become_intangible(void) {
}
static void cur_obj_hide(void) {
    moleHidden = TRUE;
}
static s32 approach_f32_ptr(f32 *v, f32 target, f32 step) {
    *v = *v < target ? MIN(*v + step, target) : MAX(*v - step, target);
    return *v == target;
}
// Air-quarter tests run dives, long jumps, freefall, rollouts and ground pounds
// on non-wind terrain. Action changes, hang/ledge grabs and cap-box targets
// are outside this fixture.
static struct Surface gWaterSurfacePseudoFloor;
void mario_set_forward_vel(struct MarioState *m, f32 speed);
static s32 mario_is_low_land_pose(struct MarioState *m) {
    assert(m->action == ACT_FREEFALL || m->action == ACT_STOMACH_SLIDE);
    return m->action == ACT_STOMACH_SLIDE;
}
static s32 mario_can_wall_kick_backward_jump(struct MarioState *m) {
    assert(m->action == ACT_DIVE || m->action == ACT_FREEFALL || m->action == ACT_LONG_JUMP
           || m->action == ACT_FORWARD_ROLLOUT || m->action == ACT_BACKWARD_ROLLOUT
           || m->action == ACT_GROUND_POUND);
    return FALSE;
}
static void set_mario_wall(struct MarioState *m, struct Surface *surface) {
    m->wall = surface;
    m->wallYaw = surface ? SURFACE_YAW(surface) : 0;
}
static s32 mario_restrict_air_uphill(struct MarioState *m, struct Surface *floor) {
    assert(m->action == ACT_DIVE || m->action == ACT_FREEFALL || m->action == ACT_LONG_JUMP
           || m->action == ACT_FORWARD_ROLLOUT || m->action == ACT_BACKWARD_ROLLOUT
           || m->action == ACT_GROUND_POUND);
    return FALSE;
}
static void constrain_wind_descent(struct MarioState *m, Vec3f next, struct Surface *floor,
                                   f32 height) {
    assert(floor == NULL || floor->type != SURFACE_VERTICAL_WIND);
}
static void mario_attack_cap_box_surface(struct MarioState *m, struct Surface *floor, u32 attack) {
    if (attack == INT_HIT_FROM_BELOW) {
        // Gallery and bridge undersides use the native rising-ceiling callback.
        // These Lab surfaces own no cap box, so the interaction has no effect.
        assert(m->vel[1] > 0.0f);
        assert(floor != NULL && floor->normal.y < 0.0f);
        assert(floor->object == NULL || (floor->object == &object && floor->type == SURFACE_DEFAULT));
        return;
    }
    assert(m->action == ACT_GROUND_POUND && floor->type == SURFACE_DEFAULT
           && attack == INT_GROUND_POUND_OR_TWIRL);
}
static s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return FALSE;
}
static struct Surface *check_ledge_grab(struct MarioState *m, struct Surface *wall,
                                        struct Surface *surface, Vec3f intended, Vec3f next,
                                        Vec3f ledge, struct Surface **floor) {
    fprintf(stderr, "HARNESS ERROR: unexpected air-step ledge grab\n");
    exit(2);
}
static void bowser_arena_recover_enemy(struct Object *obj) {
    (void) obj;
}
static s32 bowser_arena_phase_in(struct Object *obj) {
    (void) obj;
    return FALSE;
}
#include "swimming_sources.inc"

static s32 counted_stunned_triangle_cast(struct MovingBodyProfile *profile, Vec3f start, Vec3f end,
                                         Vec3f before[3], Vec3f after[3], f32 *fraction, Vec3f normal) {
    stunnedCastCount++;
    return moving_triangle_cast(profile, start, end, before, after, fraction, normal);
}

static struct Surface surfaces[1100];
static struct SurfaceNode nodes[1100 * NUM_CELLS * NUM_CELLS];
static int surfaceCount, nodeCount;
// Insert into every cell deliberately: lookup order, cell traversal, triangle
// bounds and static/dynamic selection are still the production implementations.
static struct Surface *triangle(Vec3s a, Vec3s b, Vec3s c, int dynamic, int type) {
#if DYNAMIC_SURFACE_INDEX
    if (dynamic) {
        invalidate_dynamic_surface_index();
    }
#endif
    struct Surface *s = &surfaces[surfaceCount++];
    Vec3f ab, ac, n;
    memcpy(s->vertex1, a, sizeof(Vec3s));
    memcpy(s->vertex2, b, sizeof(Vec3s));
    memcpy(s->vertex3, c, sizeof(Vec3s));
    vec3_diff(ab, b, a);
    vec3_diff(ac, c, a);
    vec3f_cross(n, ab, ac);
    vec3f_normalize(n);
    s->normal = (struct Normal){ n[0], n[1], n[2] };
    s->originOffset = -(n[0] * a[0] + n[1] * a[1] + n[2] * a[2]);
    s->lowerY = MIN(a[1], MIN(b[1], c[1])) - 5;
    s->upperY = MAX(a[1], MAX(b[1], c[1])) + 5;
    s->type = type;
    int kind = n[1] > .01f    ? SPATIAL_PARTITION_FLOORS
               : n[1] < -.01f ? SPATIAL_PARTITION_CEILS
                              : SPATIAL_PARTITION_WALLS;
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            struct SurfaceNode **head =
                dynamic ? &gDynamicSurfacePartition[z][x][kind] : &gStaticSurfacePartition[z][x][kind];
            // Production floor lists are sorted highest first.
            while (*head && kind == SPATIAL_PARTITION_FLOORS && (*head)->surface->upperY >= s->upperY) {
                head = &(*head)->next;
            }
            struct SurfaceNode *node = &nodes[nodeCount++];
            node->surface = s;
            node->next = *head;
            *head = node;
        }
    }
    return s;
}
static void floor_at(int height) {
    triangle((Vec3s){ -4000, height, -4000 }, (Vec3s){ 0, height, 4000 },
             (Vec3s){ 4000, height, -4000 }, 0, SURFACE_DEFAULT);
}
static void wall_at(int x, int low, int high, int dynamic, int type) {
    // Keep the flat-wall probes far from triangle edges, whose existing radial
    // push can deliberately add a tangential component.
    triangle((Vec3s){ x, low, -4000 }, (Vec3s){ x, low, 4000 }, (Vec3s){ x, high, 0 }, dynamic, type);
}
static void reset(void) {
#if DYNAMIC_SURFACE_INDEX
    invalidate_dynamic_surface_index();
#endif
    bypassDynamicIndex = FALSE;
#if STATIC_SURFACE_INDEX
    gStaticSurfaceIndex.offsets = NULL;
#endif
#if STATIC_SURFACE_INDEX || STATIC_WALL_HEIGHT_GROUPS
    gCurrStaticSurfacePoolEnd = staticIndexPool.bytes;
#endif
    invalidate_static_wall_height_index();
    bypassWaterRayBounds = bypassWaterContacts = bypassWaterQueries = bypassStaticGroups = FALSE;
    bypassWaterEdgeGroups = FALSE;
    gGlobalTimer++;
    gStaticSurfaceGeneration++;
    floorNodeVisits = collisionQueryCount = collisionTimedQueries = 0;
    verifiedFloorRise = 0.0f;
    sPalette = MARIO_PALETTE_STANDARD;
    enemyWater = -10000;
    gMarioObject = &object;
    nativeEnemyUpdates = nativeMoleAI = waterDeathEligible = moleHidden = 0;
    memset(&moleHole, 0, sizeof(moleHole));
    memset(&moleRock, 0, sizeof(moleRock));
    memset(&mario, 0, sizeof(mario));
    memset(&object, 0, sizeof(object));
    memset(&sSwimming, 0, sizeof(sSwimming));
    memset(&controller, 0, sizeof(controller));
    memset(&body, 0, sizeof(body));
    sBobTimer = sBobIncrement = 0;
    sBobHeight = 0;
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
    memset(gDynamicSurfacePartition, 0, sizeof(gDynamicSurfacePartition));
    memset(surfaces, 0, sizeof(surfaces));
    surfaceCount = nodeCount = currentCalls = gCollisionFlags = 0;
    vec3f_set(current, 0, 0, 0);
    mario.marioObj = &object;
    mario.waterLevel = 2000;
    mario.action = ACT_FLAG_SWIMMING;
    mario.controller = &controller;
    mario.marioBodyState = &body;
    floor_at(-1000);
    mario.floor = &surfaces[0];
    mario.floorHeight = -1000;
}
#define CHECK(x) EXPECT_EQ(!!(x), 1)
#define NEAR(a, b)                                                                                     \
    do {                                                                                               \
        f32 got_ = (a), want_ = (b);                                                                   \
        if (!(fabsf(got_ - want_) < 0.02f)) {                                                          \
            fprintf(stderr, "%s:%d: %s: %s expected %.4f, observed %.4f\n", __FILE__, __LINE__,        \
                    sCurrentCase, #a, want_, got_);                                                    \
            return 0;                                                                                  \
        }                                                                                              \
    } while (0)
static int thicc_water_weight(void) {
    for (int moving = 0; moving < 2; moving++) {
        reset();
        mario.action = moving ? ACT_BREASTSTROKE : ACT_WATER_IDLE;
        f32 ordinary = get_buoyancy(&mario);
        sPalette = MARIO_PALETTE_HE_THICC;
        NEAR(get_buoyancy(&mario), ordinary - 3);
        mario.pos[1] = mario.waterLevel - 80;
        NEAR(get_buoyancy(&mario), 1.25f);
        mario.pos[1] = 0;
        mario.flags = MARIO_METAL_CAP;
        NEAR(get_buoyancy(&mario), -21);
        mario.action = ACT_WATER_SHOCKED;
        NEAR(get_buoyancy(&mario), -2);
    }
    reset();
    mario.action = ACT_BREASTSTROKE;
    mario.forwardVel = 24;
    mario.faceAngle[0] = 0x2000;
    sPalette = MARIO_PALETTE_HE_THICC;
    common_swimming_step(&mario, 200, 0);
    CHECK(mario.vel[1] > 10); // Swimming upward remains useful against the extra weight.
    return 1;
}

static int free_movement(void) {
    reset();
    resolve_mario_geometry_walls(&mario);
    vec3f_set(mario.vel, 64, 32, -24);
    mario.forwardVel = 64;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_NONE);
    NEAR(mario.pos[0], 64);
    NEAR(mario.pos[1], 32);
    NEAR(mario.pos[2], -24);
    NEAR(mario.forwardVel, 64);
    EXPECT_EQ(currentCalls, 1);
    return 1;
}
static int fast_wall(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        reset();
        wall_at(8, -500, 500, dynamic, SURFACE_DEFAULT);
        vec3f_set(mario.vel, 128, 0, 48);
        mario.forwardVel = 128;
        EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
        CHECK(mario.pos[0] < 8);
        NEAR(mario.pos[2], 48);
        NEAR(mario.forwardVel, 128);
        NEAR(mario.vel[0], 0);
        NEAR(mario.vel[2], 48);
    }
    return 1;
}
static int roll_wall_rebound(void) {
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    sSwimming.rollFrame = 1;
    mario.forwardVel = 32;
    mario.faceAngle[1] = 0x4000;
    vec3f_set(mario.vel, 32, 0, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    NEAR(mario.forwardVel, 53.3f);
    CHECK(mario.vel[0] < 0);
    CHECK(mario.particleFlags & PARTICLE_VERTICAL_STAR);
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    set_mario_wall(&mario, &surfaces[surfaceCount - 1]);
    sSwimming.rollFrame = 1;
    mario.faceAngle[1] = 0x4000;
    bounce_swimming_roll(&mario, (Vec3f){ 0, 0, 0 }, WATER_STEP_HIT_WALL, (Vec3f){ 0, 0, 0 });
    NEAR(mario.forwardVel, 11.7f);
    CHECK(mario.vel[0] < 0);
    CHECK(mario.particleFlags & PARTICLE_VERTICAL_STAR);
    for (s32 dynamic = 0; dynamic < 2; dynamic++) {
        for (s32 early = 0; early < 2; early++) {
            reset();
            wall_at(8, -500, 500, dynamic, SURFACE_DEFAULT);
            sSwimming.rollFrame = early ? 1 : 8;
            mario.forwardVel = 32;
            mario.faceAngle[0] = 0x1000;
            mario.faceAngle[1] = atan2s(8, 32);
            s16 yaw = mario.faceAngle[1];
            vec3f_set(mario.vel, 32, 4, 8);
            EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
            NEAR(mario.forwardVel, (32 + (early ? 9 : 8)) * 1.3f);
            EXPECT_EQ(mario.faceAngle[1], (s16) -yaw);
            EXPECT_EQ(mario.faceAngle[0], 0x1000);
            CHECK(mario.vel[0] < 0 && mario.vel[2] > 0);
            NEAR(mario.vel[1], 4);
            EXPECT_EQ(sSwimming.rollFrame, 1);
            EXPECT_EQ(sSwimming.rollBounceCount, 1);
            CHECK(early || sSwimming.rollStartAngle > 0);
            // Staying against the same wall cannot grant a second boost.
            f32 speed = mario.forwardVel;
            bounce_swimming_roll(&mario, (Vec3f){ 32, 4, 8 }, WATER_STEP_HIT_WALL, (Vec3f){ 0, 0, 0 });
            NEAR(mario.forwardVel, speed);
            EXPECT_EQ(sSwimming.rollBounceCount, 1);
            // Move away: the collision report clears the contact before returning.
            perform_water_step(&mario);
            CHECK(mario.wall == NULL);
            EXPECT_EQ(sSwimming.rollWallContact, FALSE);
        }
    }
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    struct Surface *wall = &surfaces[surfaceCount - 1];
    sSwimming.rollFrame = 1;
    // Side/rear overlap uses the flight roll's facing-preserving boost.
    set_mario_wall(&mario, wall);
    mario.faceAngle[1] = -0x4000;
    mario.forwardVel = 60;
    bounce_swimming_roll(&mario, (Vec3f){ -60, 0, 0 }, WATER_STEP_HIT_WALL, (Vec3f){ 0, 0, 0 });
    EXPECT_EQ(mario.faceAngle[1], -0x4000);
    NEAR(mario.forwardVel, 64);
    for (s32 hit = 2; hit <= 18; hit++) {
        sSwimming.rollWallContact = FALSE;
        mario.forwardVel = 32;
        bounce_swimming_roll(&mario, (Vec3f){ -32, 0, 0 }, WATER_STEP_HIT_WALL, (Vec3f){ 0, 0, 0 });
        f32 expected = hit == 18 ? 0 : 40 * 1.3f * (1 - (hit - 1) * 0.06f);
        NEAR(mario.forwardVel, expected);
        EXPECT_EQ(sSwimming.rollBounceCount, hit);
    }
    return TRUE;
}

static int roll_floor_rebound(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int slope = 0; slope < 3; slope++) {
            for (int early = 0; early < 2; early++) {
                reset();
                struct Surface *floor;
                if (slope == 2) {
                    floor = triangle((Vec3s){ 0, -300, -1000 }, (Vec3s){ 0, -300, 1000 },
                                     (Vec3s){ 12, 300, 0 }, dynamic, SURFACE_DEFAULT);
                } else {
                    int rise = slope ? 1000 : 0;
                    floor = triangle((Vec3s){ -2000, -rise, -2000 }, (Vec3s){ 0, 0, 2000 },
                                     (Vec3s){ 2000, rise, -2000 }, dynamic, SURFACE_DEFAULT);
                }
                mario.pos[1] = 10;
                mario.forwardVel = 32;
                mario.faceAngle[0] = -0x2000;
                mario.faceAngle[1] = 0x4000;
                sSwimming.rollFrame = early ? 1 : 8;
                Vec3f incoming = { 24, -24, 8 };
                vec3f_copy(mario.vel, incoming);
                EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
                CHECK(mario.floor == floor);
                f32 speed = (32 + (early ? 9 : 8)) * 1.3f;
                NEAR(mario.forwardVel, speed);
                NEAR(sqrtf(vec3f_dot(mario.vel, mario.vel)), speed);
                CHECK(mario.vel[0] * floor->normal.x + mario.vel[1] * floor->normal.y
                          + mario.vel[2] * floor->normal.z
                      > 0);
                CHECK(mario.pos[0] * floor->normal.x + mario.pos[1] * floor->normal.y
                          + mario.pos[2] * floor->normal.z + floor->originOffset
                      >= -0.05f);
                EXPECT_EQ(sSwimming.rollBounceCount, 1);
                EXPECT_EQ(sSwimming.rollFrame, 1);
                CHECK(mario.particleFlags & PARTICLE_VERTICAL_STAR);
                // The same plane, including a neighboring triangle, cannot boost twice.
                struct Surface neighboring = *floor;
                mario.floor = &neighboring;
                bounce_swimming_roll(
                    &mario, incoming, WATER_STEP_HIT_FLOOR,
                    (Vec3f){ mario.floor->normal.x, mario.floor->normal.y, mario.floor->normal.z });
                NEAR(mario.forwardVel, speed);
                EXPECT_EQ(sSwimming.rollBounceCount, 1);
                // Separating from the bottom rearms a genuine later impact.
                mario.floor = floor;
                EXPECT_EQ(perform_water_step(&mario), WATER_STEP_NONE);
                EXPECT_EQ(sSwimming.rollFloorContact, FALSE);
                sSwimming.rollFrame = 8;
                mario.forwardVel = 32;
                bounce_swimming_roll(
                    &mario, incoming, WATER_STEP_HIT_FLOOR,
                    (Vec3f){ mario.floor->normal.x, mario.floor->normal.y, mario.floor->normal.z });
                NEAR(mario.forwardVel, 40 * 1.3f * 0.94f);
                EXPECT_EQ(sSwimming.rollBounceCount, 2);
            }
        }
    }
    // Head-on downward and stopped downward-facing contacts both launch upward.
    for (int stopped = 0; stopped < 2; stopped++) {
        reset();
        floor_at(0);
        mario.floor = &surfaces[1];
        mario.faceAngle[0] = -0x4000;
        mario.forwardVel = stopped ? 0 : 60;
        sSwimming.rollFrame = 1;
        bounce_swimming_roll(
            &mario, (Vec3f){ 0, stopped ? 0 : -60, 0 }, WATER_STEP_HIT_FLOOR,
            (Vec3f){ mario.floor->normal.x, mario.floor->normal.y, mario.floor->normal.z });
        CHECK(mario.vel[1] > 0);
        NEAR(mario.forwardVel, stopped ? 11.7f : 64);
        CHECK(abs(mario.faceAngle[0] - 0x4000) <= 1);
    }
    // Ordinary swimming still clips downward movement without a roll boost.
    reset();
    floor_at(0);
    mario.pos[1] = 10;
    mario.forwardVel = 32;
    vec3f_set(mario.vel, 0, -32, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    NEAR(mario.vel[1], 0);
    NEAR(mario.forwardVel, 32);
    EXPECT_EQ(sSwimming.rollBounceCount, 0);
    // Actual swim steering and automatic floor assistance preserve the rebound pitch.
    reset();
    floor_at(0);
    mario.pos[1] = 10;
    mario.forwardVel = 32;
    mario.faceAngle[0] = -0x2000;
    controller.stickY = 64;
    sSwimming.rollFrame = 1;
    common_swimming_step(&mario, 320, 0);
    CHECK(mario.vel[1] > 0);
    CHECK(mario.faceAngle[0] > 0);
    return TRUE;
}

static int thin_wall_crossing(void) {
    reset();
    wall_at(8, 0, 2, 0, SURFACE_DEFAULT);
    mario.pos[1] = -18;
    vec3f_set(mario.vel, 16, 16, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    CHECK(mario.pos[0] < 8);
    NEAR(mario.pos[1], -2);
    return 1;
}
static int fast_floor(void) {
    reset();
    floor_at(0);
    mario.pos[1] = 10;
    vec3f_set(mario.vel, 0, -128, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    NEAR(mario.pos[1], 0);
    CHECK(mario.floor == &surfaces[1]);
    return 1;
}
static int steep_slope(void) {
    reset();
    struct Surface *s = triangle((Vec3s){ 0, -300, -1000 }, (Vec3s){ 0, -300, 1000 },
                                 (Vec3s){ 12, 300, 0 }, 0, SURFACE_DEFAULT);
    vec3f_set(mario.vel, 128, 0, 0);
    mario.forwardVel = 64;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    CHECK(s->normal.x * mario.pos[0] + s->normal.y * mario.pos[1] + s->originOffset >= -.05f);
    CHECK(mario.pos[1] > 0);
    NEAR(mario.forwardVel, 64);
    return 1;
}
static int ceiling(void) {
    reset();
    triangle((Vec3s){ -4000, 180, -4000 }, (Vec3s){ 4000, 180, -4000 }, (Vec3s){ 0, 180, 4000 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.vel, 64, 128, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_CEILING);
    NEAR(mario.pos[1], 20);
    NEAR(mario.pos[0], 64);
    return 1;
}
static int thin_ceiling(void) {
    reset();
    triangle((Vec3s){ 4, 164, -20 }, (Vec3s){ 12, 164, -20 }, (Vec3s){ 8, 164, 20 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.vel, 16, 8, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_CEILING);
    CHECK(mario.pos[1] <= 4);
    NEAR(mario.pos[0], 16);
    return 1;
}
static int body_edge_crossing(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int ceiling = 0; ceiling < 2; ceiling++) {
            reset();
            Vec3s a = { 4, 80, -20 }, b = { 4, 80, 20 }, c = { 12, 80, 0 };
            triangle(a, ceiling ? c : b, ceiling ? b : c, dynamic, SURFACE_DEFAULT);
            Vec3f movement = { 16, 0, 0 };
            Vec3f velocity = { 16, 0, 0 };
            u32 expected = ceiling ? WATER_STEP_HIT_CEILING : WATER_STEP_HIT_FLOOR;
            EXPECT_EQ(mario_water_collision_step(&mario, movement, velocity), expected);
            CHECK(mario.pos[0] < 4.01f);
            NEAR(mario.pos[1], 0);
            CHECK(velocity[0] < 0.01f);
            // The temporary edge must not constrain movement after leaving it.
            EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ -16, 0, 0 }, NULL), WATER_STEP_NONE);
            CHECK(mario.pos[0] < -11.9f);
        }
    }
    return 1;
}
static int ddd_cage_escape(void) {
    // Cross each closed side of the actual cage at two heights, then try re-entry.
    const Vec3f directions[] = { { 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 } };
    for (unsigned side = 0; side < ARRAY_COUNT(directions); side++) {
        for (int height = -3900; height <= -3600; height += 300) {
            for (int vanish = 0; vanish <= 1; vanish++) {
                reset();
                floor_at(-4087);
                mario.floor = &surfaces[surfaceCount - 1];
                mario.floorHeight = -4087;
                for (unsigned face = 0; face < ARRAY_COUNT(dddCageTriangles); face++) {
                    triangle(dddCageTriangles[face].vertices[0], dddCageTriangles[face].vertices[1],
                             dddCageTriangles[face].vertices[2], 0, dddCageTriangles[face].type);
                }
                mario.flags = vanish ? MARIO_VANISH_CAP : 0;
                vec3f_set(mario.pos, 1967, height, -2807);
                Vec3f movement = { directions[side][0] * 16, 0, directions[side][2] * 16 };
                for (int frame = 0; frame < 40; frame++) {
                    mario_water_collision_step(&mario, movement, NULL);
                }
                f32 distance = (mario.pos[0] - 1967) * directions[side][0]
                               + (mario.pos[2] + 2807) * directions[side][2];
                CHECK(distance > 1000);
                movement[0] = -movement[0];
                movement[2] = -movement[2];
                for (int frame = 0; frame < 40; frame++) {
                    mario_water_collision_step(&mario, movement, NULL);
                }
                distance = (mario.pos[0] - 1967) * directions[side][0]
                           + (mario.pos[2] + 2807) * directions[side][2];
                if (vanish) {
                    CHECK(absf(distance) < 1);
                } else {
                    CHECK(distance > 400);
                }
            }
        }
    }
    return 1;
}

static int currents(void) {
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    vec3f_set(mario.vel, 12, 0, 0);
    vec3f_set(current, 116, 0, 40);
    mario.forwardVel = 12;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    CHECK(mario.pos[0] < 8);
    NEAR(mario.pos[2], 40);
    NEAR(mario.forwardVel, 12);
    EXPECT_EQ(currentCalls, 1);
    return 1;
}
static int surface_exit(void) {
    reset();
    mario.waterLevel = 100;
    mario.vel[1] = 64;
    perform_water_step(&mario);
    NEAR(mario.pos[1], 20);
    NEAR(mario.vel[1], 0);
    NEAR(sSwimming.surfaceExitVelocityY, 64);
    return 1;
}
static int stroke_wall(void) {
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    mario.faceAngle[1] = 0x4000;
    mario.forwardVel = 24;
    mario.vel[0] = 24;
    perform_water_step(&mario);
    NEAR(mario.forwardVel, 24);
    NEAR(mario.vel[0], 0);
    return 1;
}
static int stroke_glance(void) {
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    mario.faceAngle[1] = 0x2000;
    mario.forwardVel = 24;
    vec3f_set(mario.vel, 24 * sins(0x2000), 0, 24 * coss(0x2000));
    perform_water_step(&mario);
    NEAR(mario.forwardVel, 24);
    CHECK(mario.pos[2] > 16);
    return 1;
}
static int sustained_wall_glance(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING,
                            ACT_WATER_PLUNGE, ACT_WATER_DOWN_BURST };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int dynamic = 0; dynamic < 2; dynamic++) {
            for (int direction = -1; direction <= 1; direction += 2) {
                for (int cycle = 0; cycle < 3; cycle++) {
                    reset();
                    wall_at(8, -500, 500, dynamic, SURFACE_DEFAULT);
                    mario.action = actions[action];
                    mario.forwardVel = 24;
                    // Rebuild requested velocity each update, just like the swim controls.
                    // A 20-degree glance used to multiply speed by cos(20) every frame.
                    for (int frame = 0; frame < 12; frame++) {
                        f32 z = mario.pos[2];
                        vec3f_set(mario.vel, mario.forwardVel * 0.34202014f, 0,
                                  direction * mario.forwardVel * 0.93969262f);
                        perform_water_step(&mario);
                        NEAR(mario.forwardVel, 24);
                        CHECK(mario.pos[0] < 8);
                        CHECK(direction * (mario.pos[2] - z) > 22.0f);
                    }
                    // Turning away keeps the retained glance speed but never adds speed.
                    f32 x = mario.pos[0];
                    vec3f_set(mario.vel, -24, 0, 0);
                    perform_water_step(&mario);
                    NEAR(mario.forwardVel, 24);
                    CHECK(mario.pos[0] < x - 23);
                }
            }
        }
    }
    // Near-head-on contact still permits sliding and retains stored speed.
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    mario.forwardVel = 24;
    vec3f_set(mario.vel, 24 * sins(DEGREES(80)), 0, 24 * coss(DEGREES(80)));
    perform_water_step(&mario);
    NEAR(mario.forwardVel, 24);
    CHECK(mario.pos[0] < 8);
    CHECK(mario.pos[2] > 0);
    // Direct impacts block actual travel but retain stored speed.
    reset();
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    for (int frame = 0; frame < 8; frame++) {
        mario.forwardVel = 24;
        vec3f_set(mario.vel, 24, 0, 0);
        perform_water_step(&mario);
        NEAR(mario.forwardVel, 24);
        NEAR(mario.vel[0], 0);
    }
    return 1;
}
static int turning_wall_speed(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int dynamic = 0; dynamic < 2; dynamic++) {
            for (int direction = -1; direction <= 1; direction += 2) {
                for (int cycle = 0; cycle < 3; cycle++) {
                    reset();
                    wall_at(8, -2000, 2000, dynamic, SURFACE_DEFAULT);
                    mario.action = actions[action];
                    mario.heldObj = action ? &object : NULL;
                    mario.forwardVel = 24;
                    mario.faceAngle[1] = 0x4000 - direction * DEGREES(5);
                    controller.stickX = direction * 64;
                    for (int frame = 0; frame < 50; frame++) {
                        resolve_mario_geometry_walls(&mario);
                        common_swimming_step(&mario, 200, 0);
                        NEAR(mario.forwardVel, 24);
                        CHECK(mario.pos[0] < 8);
                        EXPECT_EQ(mario.faceAngle[0], 0);
                    }
                    CHECK(direction * mario.pos[2] > 300);
                }
            }
        }
    }
    return 1;
}
static int wall_slide_controls(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int dynamic = 0; dynamic < 2; dynamic++) {
            for (int direction = -1; direction <= 1; direction += 2) {
                for (int cycle = 0; cycle < 3; cycle++) {
                    reset();
                    wall_at(8, -500, 500, dynamic, SURFACE_DEFAULT);
                    mario.action = actions[action];
                    mario.heldObj = action ? &object : NULL;
                    mario.forwardVel = 24;
                    mario.faceAngle[1] = direction > 0 ? 0x2000 : 0x6000;
                    for (int frame = 0; frame < 16; frame++) {
                        resolve_mario_geometry_walls(&mario);
                        f32 z = mario.pos[2];
                        common_swimming_step(&mario, 200, 0);
                        EXPECT_EQ(mario.faceAngle[0], 0);
                        CHECK(mario.pos[0] < 8);
                        NEAR(mario.forwardVel, 24);
                        CHECK(direction * (mario.pos[2] - z) > 16.9f);
                    }
                    // Turning away follows facing immediately, with no retained travel yaw.
                    mario.faceAngle[1] = -0x4000;
                    common_swimming_step(&mario, 200, 0);
                    NEAR(mario.vel[0], -24);
                    NEAR(mario.vel[2], 0);
                    NEAR(mario.forwardVel, 24);

                    // A tiny tangent from an almost head-on stroke must stay tiny over time.
                    reset();
                    wall_at(8, -2000, 2000, dynamic, SURFACE_DEFAULT);
                    mario.action = actions[action];
                    mario.heldObj = action ? &object : NULL;
                    mario.forwardVel = 48;
                    mario.faceAngle[1] = 0x4000 + direction * 0x0058;
                    for (int frame = 0; frame < 24; frame++) {
                        f32 previousZ = mario.pos[2];
                        common_swimming_step(&mario, 200, 0);
                        CHECK(fabsf(mario.vel[2]) < 1.0f);
                        CHECK(fabsf(mario.pos[2] - previousZ) < 1.0f);
                        CHECK(mario.pos[0] < 8);
                        NEAR(mario.forwardVel, 48);
                    }
                }
            }
        }
    }
    return 1;
}
static int wall_slide_reversal(void) {
    const u32 actions[] = { ACT_BREASTSTROKE, ACT_HOLD_BREASTSTROKE, ACT_WATER_SHELL_SWIMMING };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int dynamic = 0; dynamic < 2; dynamic++) {
            for (int direction = -1; direction <= 1; direction += 2) {
                reset();
                wall_at(8, -2000, 2000, dynamic, SURFACE_DEFAULT);
                mario.action = actions[action];
                mario.heldObj = action ? &object : NULL;
                mario.forwardVel = 50.8f;
                mario.faceAngle[1] = direction > 0 ? 0x2000 : 0x6000;
                for (int frame = 0; frame < 4; frame++) {
                    common_swimming_step(&mario, 200, 0);
                }

                // A turn across the wall normal must reverse the tangent now.
                mario.faceAngle[1] = direction > 0 ? 0x6000 : 0x2000;
                for (int frame = 0; frame < 8; frame++) {
                    f32 previousZ = mario.pos[2];
                    common_swimming_step(&mario, 200, 0);
                    CHECK(direction * (mario.pos[2] - previousZ) < -10.0f);
                    CHECK(mario.pos[0] < 8);
                    NEAR(mario.forwardVel, 50.8f);
                }

                // Real steering through another turn must not retain backward travel.
                controller.stickX = direction * 64;
                for (int frame = 0; frame < 60; frame++) {
                    resolve_mario_geometry_walls(&mario);
                    common_swimming_step(&mario, 200, 0);
                    f32 forwardTravel = mario.vel[0] * sins(mario.faceAngle[1])
                                        + mario.vel[2] * coss(mario.faceAngle[1]);
                    CHECK(forwardTravel >= -0.05f);
                    CHECK(mario.pos[0] < 8);
                    NEAR(mario.forwardVel, 50.8f);
                }
            }
        }
    }
    return 1;
}

// A tiny sideways component must keep its travel without repeating the same
// inward wall separation during every adaptive step.
static int wall_contact_work(void) {
    reset();
    wall_at(500, -500, 500, 0, SURFACE_DEFAULT);
    vec3f_set(mario.pos, 390, 0, 500);
    vec3f_set(mario.vel, 51.5984f, 0, 0.4057f);
    mario.forwardVel = 51.6f;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    NEAR(mario.pos[0], 390);
    NEAR(mario.pos[2], 500.4057f);
    NEAR(mario.vel[0], 0);
    NEAR(mario.vel[2], 0.4057f);
    // The original solver makes 54 requests for this four-step wall glide.
    // Final clearance and redirected-path queries remain within this budget.
    CHECK(collisionQueryCount <= 33);

    // Contact ownership ends with the call; turning away must leave the wall.
    vec3f_set(mario.vel, -51.6f, 0, 0.4057f);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_NONE);
    NEAR(mario.pos[0], 338.4f);
    NEAR(mario.pos[2], 500.8114f);
    return TRUE;
}

static int bbh_wall_slide_release(void) {
    reset();
    // The basement wall near (2291, -2925, 2137), translated to that origin.
    // These two faces come from levels/bbh/areas/1/collision.inc.c.
    Vec3s a = { 64, 1492, 423 }, b = { 64, -44, 423 };
    Vec3s c = { -345, -44, -89 }, d = { -345, 1492, -89 };
    struct Surface *wall = triangle(a, b, c, 0, SURFACE_DEFAULT);
    triangle(a, c, d, 0, SURFACE_DEFAULT);
    mario.action = ACT_BREASTSTROKE;
    mario.forwardVel = 50.8f;
    mario.waterLevel = 417;
    mario.pos[0] = -140.5f + 110.0f * wall->normal.x;
    mario.pos[2] = 167.0f + 110.0f * wall->normal.z;
    mario.faceAngle[1] = -0x4000;
    for (int frame = 0; frame < 4; frame++) {
        common_swimming_step(&mario, 200, 0);
    }

    // The reported facing points away from this wall, opposite the old slide.
    mario.faceAngle[1] = 0x314f;
    for (int frame = 0; frame < 8; frame++) {
        Vec3f previous;
        vec3f_copy(previous, mario.pos);
        common_swimming_step(&mario, 200, 0);
        f32 forwardTravel = (mario.pos[0] - previous[0]) * sins(mario.faceAngle[1])
                            + (mario.pos[2] - previous[2]) * coss(mario.faceAngle[1]);
        CHECK(forwardTravel > 45.0f);
        NEAR(mario.forwardVel, 50.8f);
    }
    return 1;
}

static void load_jrb(void);
static int jrb_wall_pitch(void) {
    int checked = 0;
    load_jrb();
    // Exercise original JRB wall triangles in isolation, so each trial has a
    // known contact rather than silently testing open water beside the mesh.
    for (unsigned i = 0; i < ARRAY_COUNT(jrbTriangles); i++) {
        Vec3s a, b, c;
        memcpy(a, jrbVertices[jrbTriangles[i][0]], sizeof(a));
        memcpy(b, jrbVertices[jrbTriangles[i][1]], sizeof(b));
        memcpy(c, jrbVertices[jrbTriangles[i][2]], sizeof(c));
        reset();
        struct Surface *wall = triangle(a, b, c, 0, SURFACE_DEFAULT);
        if (fabsf(wall->normal.y) > .001f || wall->upperY - wall->lowerY < 500) {
            continue;
        }
        f32 nx = wall->normal.x, nz = wall->normal.z;
        vec3f_set(mario.pos, (a[0] + b[0] + c[0]) / 3.0f + nx * 112, (a[1] + b[1] + c[1]) / 3.0f - 10,
                  (a[2] + b[2] + c[2]) / 3.0f + nz * 112);
        mario.waterLevel = 20000;
        mario.forwardVel = 24;
        mario.faceAngle[1] = atan2s(-nz + nx, -nx - nz);
        f32 distance = 0;
        int contact = 0;
        for (int frame = 0; frame < 8; frame++) {
            Vec3f before;
            vec3f_copy(before, mario.pos);
            common_swimming_step(&mario, 200, 0);
            EXPECT_EQ(mario.faceAngle[0], 0);
            distance += sqrtf(sqr(mario.pos[0] - before[0]) + sqr(mario.pos[2] - before[2]));
            contact += fabsf(mario.vel[0] * nx + mario.vel[2] * nz) < 0.01f
                       && sqr(mario.vel[0]) + sqr(mario.vel[2]) > 0.01f;
        }
        if (contact) {
            CHECK(distance > 80);
            checked++;
        }
    }
    CHECK(checked > 0);
    return 1;
}
static int filters(void) {
    for (int which = 0; which < 4; which++) {
        reset();
        wall_at(8, -500, 500, 0, which == 0 ? SURFACE_CAMERA_BOUNDARY : SURFACE_VANISH_CAP_WALLS);
        if (which == 1) {
            mario.flags = MARIO_VANISH_CAP;
        }
        if (which == 2) {
            object.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
        }
        mario.vel[0] = 128;
        perform_water_step(&mario);
        if (which < 3) {
            NEAR(mario.pos[0], 128);
        } else {
            CHECK(mario.pos[0] < 8);
        }
    }
    reset();
    struct Surface *s = triangle((Vec3s){ 0, -300, -1000 }, (Vec3s){ 0, -300, 1000 },
                                 (Vec3s){ 12, 300, 0 }, 0, SURFACE_INTANGIBLE);
    mario.vel[0] = 128;
    perform_water_step(&mario);
    NEAR(mario.pos[0], 128);
    s->type = SURFACE_DEFAULT;
    s->flags = SURFACE_FLAG_NO_CAM_COLLISION;
    vec3f_set(mario.pos, 0, 0, 0);
    perform_water_step(&mario);
    CHECK(mario.pos[0] < 12);
    return 1;
}
static int floor_contact_retained(void) {
    reset();
    triangle((Vec3s){ -2, 0, -40 }, (Vec3s){ 6, 0, 40 }, (Vec3s){ 14, 0, -40 }, 0, SURFACE_DEFAULT);
    vec3f_set(mario.pos, 0, 2, 0);
    vec3f_set(mario.vel, 64, -16, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    CHECK(mario.pos[0] > 14);
    return 1;
}
static int vanish_sculpture_faces(void) {
    int floors = 0, ceilings = 0;
    for (int size = 0; size < 3; size++) {
        for (int i = 0; i < 126; i++) {
            reset();
            Vec3s v[3];
            for (int p = 0; p < 3; p++) {
                for (int axis = 0; axis < 3; axis++) {
                    v[p][axis] = (s16) lrintf(vanishSculpture[i][p][axis] / (f32) (1 << size));
                }
            }
            struct Surface *s = triangle(v[0], v[1], v[2], i % 2, SURFACE_VANISH_CAP_WALLS);
            if (fabsf(s->normal.y) < .01f) {
                continue;
            }
            s32 x = (v[0][0] + v[1][0] + v[2][0]) / 3, z = (v[0][2] + v[1][2] + v[2][2]) / 3;
            f32 y = get_surface_height_at_location(x, z, s);
            // Extremely thin triangles may have no integer point near the centroid.
            if (s->normal.y > 0 ? !check_within_floor_triangle_bounds(x, z, s)
                                : !check_within_ceil_triangle_bounds(x, z, s, 1.5f)) {
                continue;
            }
            if (s->normal.y > 0) {
                floors++;
            } else {
                ceilings++;
            }
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int phase = 0; phase < 3; phase++) {
                    struct Surface *hit = NULL;
                    mario.flags = phase == 1 ? MARIO_VANISH_CAP : 0;
                    if (s->normal.y > 0) {
                        find_floor(x, y + 150, z, &hit);
                    } else {
                        find_ceil(x, y - 150, z, &hit);
                    }
                    CHECK(phase == 1 ? hit != s : hit == s);
                }
            }
            // Camera queries still see the surface while Mario is intangible.
            mario.flags = MARIO_VANISH_CAP;
            gCollisionFlags = COLLISION_FLAG_CAMERA;
            struct Surface *hit = NULL;
            if (s->normal.y > 0) {
                find_floor(x, y + 150, z, &hit);
            } else {
                find_ceil(x, y - 150, z, &hit);
            }
            CHECK(hit == s);
            // Another object does not inherit Mario's cap, but its own grate flag
            // does apply equally to sloped floors and undersides.
            struct Object other = { 0 };
            o = &other;
            gCollisionFlags = 0;
            CHECK(!surface_is_passable_vanish(s));
            other.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
            CHECK(surface_is_passable_vanish(s));
            o = &object;
        }
    }
    CHECK(floors > 100);
    CHECK(ceilings > 30);
    return 1;
}
static int vanish_swimming_faces(void) {
    for (int ceiling = 0; ceiling < 2; ceiling++) {
        for (int active = 0; active < 2; active++) {
            reset();
            mario.flags = active ? MARIO_VANISH_CAP : 0;
            if (ceiling) {
                triangle((Vec3s){ -1000, 200, -1000 }, (Vec3s){ 1000, 200, -1000 },
                         (Vec3s){ 0, 200, 1000 }, 0, SURFACE_VANISH_CAP_WALLS);
                vec3f_set(mario.vel, 0, 128, 0);
            } else {
                triangle((Vec3s){ -1000, -40, -1000 }, (Vec3s){ 0, -40, 1000 },
                         (Vec3s){ 1000, -40, -1000 }, 0, SURFACE_VANISH_CAP_WALLS);
                vec3f_set(mario.vel, 0, -128, 0);
            }
            perform_water_step(&mario);
            if (active) {
                NEAR(mario.pos[1], ceiling ? 128 : -128);
            } else {
                CHECK(ceiling ? mario.pos[1] < 100 : mario.pos[1] > -100);
            }
        }
    }
    return 1;
}
static int no_floor(void) {
    reset();
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
    mario.vel[0] = 128;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_CANCELLED);
    NEAR(mario.pos[0], 0);
    return 1;
}
static int corner(void) {
    reset();
    wall_at(120, -500, 500, 0, SURFACE_DEFAULT);
    triangle((Vec3s){ -1000, -500, 120 }, (Vec3s){ 1000, 500, 120 }, (Vec3s){ 1000, -500, 120 }, 0,
             SURFACE_DEFAULT);
    triangle((Vec3s){ -1000, -500, 120 }, (Vec3s){ -1000, 500, 120 }, (Vec3s){ 1000, 500, 120 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.vel, 128, 0, 128);
    perform_water_step(&mario);
    CHECK(mario.pos[0] < 120 && mario.pos[2] < 120);
    return 1;
}
static int repeated_slope(void) {
    reset();
    triangle((Vec3s){ -1000, -1000, -2000 }, (Vec3s){ -1000, -1000, 2000 }, (Vec3s){ 1000, 1000, 0 }, 0,
             SURFACE_DEFAULT);
    for (int frame = 0; frame < 8; frame++) {
        vec3f_set(mario.vel, 64, -32, 0);
        EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
        CHECK(mario.pos[1] >= mario.pos[0] - .02f);
    }
    // On y=x, projecting (64,-32) along the slope gives (16,16),
    // not the old upward snap that kept all 64 horizontal units.
    CHECK(fabsf(mario.pos[0] - 128) < .2f);
    return 1;
}
static int thin_floor(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        reset();
        triangle((Vec3s){ 4, 0, -20 }, (Vec3s){ 8, 0, 20 }, (Vec3s){ 12, 0, -20 }, dynamic,
                 SURFACE_DEFAULT);
        vec3f_set(mario.pos, 0, 4, 0);
        vec3f_set(mario.vel, 16, -8, 0);
        EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
        CHECK(mario.pos[1] >= 0);
        NEAR(mario.pos[0], 16);
    }
    return 1;
}
static int surface_under_ceiling(void) {
    reset();
    mario.waterLevel = 100;
    triangle((Vec3s){ -4000, 170, -4000 }, (Vec3s){ 4000, 170, -4000 }, (Vec3s){ 0, 170, 4000 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.vel, 64, 128, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_CEILING);
    CHECK(mario.pos[1] < mario.waterLevel - 80);
    NEAR(mario.pos[0], 64);
    return 1;
}
static int cell_boundary(void) {
    reset();
    wall_at(1028, 0, 2, 1, SURFACE_DEFAULT);
    vec3f_set(mario.pos, 1020, -18, 0);
    vec3f_set(mario.vel, 16, 16, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    CHECK(mario.pos[0] < 1028);
    NEAR(mario.pos[1], -2);
    return 1;
}
static int cancelled_stroke(void) {
    reset();
    floor_at(0);
    triangle((Vec3s){ -4000, 150, -4000 }, (Vec3s){ 4000, 150, -4000 }, (Vec3s){ 0, 150, 4000 }, 0,
             SURFACE_DEFAULT);
    mario.forwardVel = 24;
    mario.vel[2] = 24;
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_CANCELLED);
    NEAR(mario.pos[2], 0);
    NEAR(mario.forwardVel, 24);
    return 1;
}
static int idle_and_reverse(void) {
    reset();
    wall_at(100, -500, 500, 0, SURFACE_DEFAULT);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_WALL);
    CHECK(mario.pos[0] <= -10);
    vec3f_set(mario.vel, -128, 0, 0);
    perform_water_step(&mario);
    CHECK(mario.pos[0] <= -138);
    return 1;
}
// A downward approach must reach the slope, not ratchet upward or keep the
// original clearance after contact. Fractional X exposes floor-query rounding.
static int downward_slope_contact(void) {
    reset();
    triangle((Vec3s){ -500, -1000, -2000 }, (Vec3s){ -500, -1000, 2000 }, (Vec3s){ 500, 1000, 0 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.pos, 0, 20, 0);
    for (int frame = 0; frame < 4; frame++) {
        f32 oldY = mario.pos[1];
        vec3f_set(mario.vel, 37, -41, 0);
        EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
        CHECK(mario.pos[1] <= oldY + .1f);
        CHECK(fabsf(mario.pos[1] - 2 * mario.pos[0]) < .2f);
    }
    CHECK(mario.pos[1] < 0);
    return 1;
}
static int contact_remainder(void) {
    reset();
    floor_at(0);
    vec3f_set(mario.pos, 0, 4, 0);
    vec3f_set(mario.vel, 12, -12, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    NEAR(mario.pos[0], 12);
    NEAR(mario.pos[1], WATER_COLLISION_SKIN);
    return 1;
}
static int downward_slope_distance(void) {
    reset();
    triangle((Vec3s){ -500, -2000, -2000 }, (Vec3s){ -500, -2000, 2000 }, (Vec3s){ 500, 2000, 0 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.pos, 0, 20, 0);
    vec3f_set(mario.vel, 40, -40, 0);
    EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_FLOOR);
    CHECK(mario.pos[1] <= 20.1f);
    CHECK(sqr(mario.pos[0]) + sqr(mario.pos[1] - 20) <= 3200.1f);
    return 1;
}
static int downhill_seam(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int reverse = 0; reverse < 2; reverse++) {
            reset();
            triangle((Vec3s){ 0, 0, -2000 }, (Vec3s){ 0, 0, 2000 }, (Vec3s){ 1000, -1000, 0 }, dynamic,
                     SURFACE_DEFAULT);
            triangle((Vec3s){ -1000, 0, 0 }, (Vec3s){ 0, 0, 2000 }, (Vec3s){ 0, 0, -2000 }, dynamic,
                     SURFACE_DEFAULT);
            vec3f_set(mario.pos, reverse ? 11.5f : -12.5f, .01f, 0);
            for (int frame = 0; frame < 10; frame++) {
                f32 oldX = mario.pos[0];
                u32 result =
                    mario_water_collision_step(&mario, (Vec3f){ reverse ? -12 : 12, 0, 0 }, NULL);
                CHECK(result != WATER_STEP_CANCELLED);
                NEAR(mario.pos[0], oldX + (reverse ? -12 : 12));
                NEAR(mario.pos[1], .01f);
            }
        }
    }
    return 1;
}
static int overlap_escape(void) {
    for (int direction = 0; direction < 3; direction++) {
        reset();
        floor_at(0);
        vec3f_set(mario.pos, 0, -20, 0);
        Vec3f delta = { 0, 0, 0 };
        delta[direction] = 12;
        for (int frame = 0; frame < 10; frame++) {
            CHECK(mario_water_collision_step(&mario, delta, NULL) != WATER_STEP_CANCELLED);
        }
        CHECK(mario.pos[1] >= 0);
        CHECK(mario.pos[direction] >= 119.9f);
    }
    return 1;
}
static int overlap_escape_progress(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            triangle((Vec3s){ -4000, 0, -4000 }, (Vec3s){ 0, 0, 4000 }, (Vec3s){ 4000, 0, -4000 },
                     dynamic, SURFACE_DEFAULT);
            mario.pos[1] = -50;
            // Neither idle nor inward movement may teleport or deepen the overlap,
            // including when the floor would leave FIND_FLOOR_BUFFER afterward.
            mario_water_collision_step(&mario, (Vec3f){ 0, 0, 0 }, NULL);
            NEAR(mario.pos[1], -50);
            EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ 0, -64, 0 }, NULL),
                      WATER_STEP_CANCELLED);
            NEAR(mario.pos[1], -50);
            mario.pos[1] = -70;
            EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ 0, -16, 0 }, NULL),
                      WATER_STEP_CANCELLED);
            NEAR(mario.pos[1], -70);
            mario.pos[1] = -50;
            for (int frame = 0; frame < 6; frame++) {
                f32 oldY = mario.pos[1];
                resolve_mario_geometry_walls(&mario);
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 16, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
                CHECK(mario.pos[1] > oldY);
                CHECK(mario.pos[1] - oldY <= 16 + WATER_COLLISION_MAX_RECOVERY + .02f);
            }
            CHECK(mario.pos[1] > 0);
            // The back face can initially be outside the usual 78-unit query.
            mario.pos[1] = -100;
            for (int frame = 0; frame < 8; frame++) {
                resolve_mario_geometry_walls(&mario);
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 16, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
            }
            CHECK(mario.pos[1] > 0);
            reset();
            triangle((Vec3s){ -4000, 200, -4000 }, (Vec3s){ 4000, 200, -4000 }, (Vec3s){ 0, 200, 4000 },
                     dynamic, SURFACE_DEFAULT);
            mario.pos[1] = 100;
            EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ 0, 64, 0 }, NULL),
                      WATER_STEP_CANCELLED);
            NEAR(mario.pos[1], 100);
            for (int frame = 0; frame < 6; frame++) {
                f32 oldY = mario.pos[1];
                resolve_mario_geometry_walls(&mario);
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, -16, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
                CHECK(mario.pos[1] < oldY);
            }
            CHECK(mario.pos[1] < 40);
            mario.pos[1] = 190;
            EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ 0, 16, 0 }, NULL),
                      WATER_STEP_CANCELLED);
            NEAR(mario.pos[1], 190);
            mario.pos[1] = 220; // Ceiling itself initially lies below the foot probe.
            for (int frame = 0; frame < 16; frame++) {
                resolve_mario_geometry_walls(&mario);
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, -16, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
            }
            CHECK(mario.pos[1] < 40);
        }
    }
    return 1;
}
static int overlap_escape_obstacles(void) {
    reset();
    floor_at(0);
    wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
    mario.pos[1] = -50;
    for (int frame = 0; frame < 6; frame++) {
        CHECK(mario_water_collision_step(&mario, (Vec3f){ 16, 16, 0 }, NULL) != WATER_STEP_CANCELLED);
        CHECK(mario.pos[0] < 8);
    }
    CHECK(mario.pos[1] > 0);
    reset();
    floor_at(0);
    triangle((Vec3s){ -4000, 160, -4000 }, (Vec3s){ 4000, 160, -4000 }, (Vec3s){ 0, 160, 4000 }, 0,
             SURFACE_DEFAULT);
    mario.pos[1] = -50;
    for (int frame = 0; frame < 8; frame++) {
        mario_water_collision_step(&mario, (Vec3f){ 0, 16, 0 }, NULL);
        CHECK(mario.pos[1] <= .02f);
    }
    // An unrelated, newly reached floor may not be treated as an old overlap.
    reset();
    triangle((Vec3s){ 8, 70, -1000 }, (Vec3s){ 8, 70, 1000 }, (Vec3s){ 1000, 70, 0 }, 0,
             SURFACE_DEFAULT);
    CHECK(mario_water_collision_step(&mario, (Vec3f){ 16, 1, 0 }, NULL) == WATER_STEP_CANCELLED);
    NEAR(mario.pos[0], 0);
    NEAR(mario.pos[1], 0);
    return 1;
}
static int wall_separation_overlap(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING,
                            ACT_WATER_PLUNGE, ACT_WATER_DOWN_BURST };
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
            reset();
            mario.action = actions[a];
            triangle((Vec3s){ -1000, 1000, -2000 }, (Vec3s){ -1000, 1000, 2000 },
                     (Vec3s){ 1000, -1000, 0 }, dynamic, SURFACE_DEFAULT);
            wall_at(100, -30, 500, dynamic, SURFACE_DEFAULT);
            vec3f_set(mario.pos, 70, -70, 0);
            // The high probe's wall push must stop at the slope without embedding.
            resolve_mario_geometry_walls(&mario);
            NEAR(mario.pos[0], 70);
            NEAR(mario.pos[1], -70);
            CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 0, 12 }, NULL)
                  != WATER_STEP_CANCELLED);
            CHECK(mario.pos[1] >= -mario.pos[0] - .02f);
        }
    }
    // Separation cannot cross the finite edge of an existing overlap.
    reset();
    triangle((Vec3s){ 0, 50, -1000 }, (Vec3s){ 0, 50, 1000 }, (Vec3s){ 1000, 50, 0 }, 0,
             SURFACE_DEFAULT);
    wall_at(100, -500, 500, 0, SURFACE_DEFAULT);
    vec3f_set(mario.pos, 1, 0, 0);
    resolve_water_wall_push(&mario, mario.pos, 10, 110, NULL, NULL, NULL, NULL, NULL);
    NEAR(mario.pos[0], 1);
    NEAR(mario.pos[1], 0);
    // Metal walking retains its ordinary wall separation.
    reset();
    wall_at(100, -500, 500, 0, SURFACE_DEFAULT);
    mario.action = ACT_METAL_WATER_WALKING;
    vec3f_set(mario.pos, 70, 0, 0);
    resolve_mario_geometry_walls(&mario);
    NEAR(mario.pos[0], 50);
    return 1;
}
static int ceiling_seam(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int reverse = 0; reverse < 2; reverse++) {
            reset();
            triangle((Vec3s){ 0, 160, -2000 }, (Vec3s){ 1000, 1160, 0 }, (Vec3s){ 0, 160, 2000 },
                     dynamic, SURFACE_DEFAULT);
            triangle((Vec3s){ -1000, 160, 0 }, (Vec3s){ 0, 160, -2000 }, (Vec3s){ 0, 160, 2000 },
                     dynamic, SURFACE_DEFAULT);
            vec3f_set(mario.pos, reverse ? 11.5f : -12.5f, -.01f, 0);
            for (int frame = 0; frame < 10; frame++) {
                f32 oldX = mario.pos[0];
                CHECK(mario_water_collision_step(&mario, (Vec3f){ reverse ? -12 : 12, 0, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
                NEAR(mario.pos[0], oldX + (reverse ? -12 : 12));
                NEAR(mario.pos[1], -.01f);
            }
        }
    }
    return 1;
}
static int overlap_recovery_bounds(void) {
    reset();
    floor_at(0);
    triangle((Vec3s){ -4000, 200, -4000 }, (Vec3s){ 4000, 200, -4000 }, (Vec3s){ 0, 200, 4000 }, 0,
             SURFACE_DEFAULT);
    vec3f_set(mario.pos, 0, 60, 0);
    CHECK(mario_water_collision_step(&mario, (Vec3f){ 12, 0, 0 }, NULL) != WATER_STEP_CANCELLED);
    NEAR(mario.pos[0], 12);
    NEAR(mario.pos[1], 40 - WATER_COLLISION_SKIN);
    reset();
    floor_at(0);
    mario.pos[1] = -100;
    mario_water_collision_step(&mario, (Vec3f){ 0, 0, 0 }, NULL);
    NEAR(mario.pos[1], -100);
    reset();
    floor_at(0);
    triangle((Vec3s){ -4000, 150, -4000 }, (Vec3s){ 4000, 150, -4000 }, (Vec3s){ 0, 150, 4000 }, 0,
             SURFACE_DEFAULT);
    mario.pos[1] = -20;
    EXPECT_EQ(mario_water_collision_step(&mario, (Vec3f){ 12, 0, 0 }, NULL), WATER_STEP_CANCELLED);
    NEAR(mario.pos[0], 0);
    NEAR(mario.pos[1], -20);
    return 1;
}
static int wall_separation_crossing(void) {
    reset();
    wall_at(20, 50, 70, 0, SURFACE_DEFAULT);
    triangle((Vec3s){ -10, 50, -4000 }, (Vec3s){ -10, 70, 0 }, (Vec3s){ -10, 50, 4000 }, 0,
             SURFACE_DEFAULT);
    // The high probe's push must stop before the other wall.
    resolve_mario_geometry_walls(&mario);
    CHECK(mario.pos[0] > -10 && mario.pos[0] < 20);
    return 1;
}
static int air_floor_edge_crossings(void) {
    const s16 shifts[] = { -5000, -1, 0, 1, 5000 };
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (unsigned shift = 0; shift < sizeof(shifts) / sizeof(shifts[0]); shift++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                reset();
                triangle((Vec3s){ -8000, -300, -8000 }, (Vec3s){ -8000, -300, 8000 },
                         (Vec3s){ 8000, -300, -8000 }, 0, SURFACE_DEFAULT);
                triangle((Vec3s){ 8000, -300, -8000 }, (Vec3s){ -8000, -300, 8000 },
                         (Vec3s){ 8000, -300, 8000 }, 0, SURFACE_DEFAULT);
                s16 x = shifts[shift];
                struct Surface *rail = triangle((Vec3s){ x, 0, -1000 }, (Vec3s){ x, 0, 1000 },
                                                (Vec3s){ x + 1, 0, -1000 }, dynamic, SURFACE_DEFAULT);
                struct Surface *railB = triangle((Vec3s){ x + 1, 0, -1000 }, (Vec3s){ x, 0, 1000 },
                                                 (Vec3s){ x + 1, 0, 1000 }, dynamic, SURFACE_DEFAULT);
                struct Surface *hit;
                Vec3f from = { x + 0.25f, -4.0f, 0.0f };
                Vec3f next = { x + 0.25f, -8.0f, 0.0f };
                NEAR(find_floor(next[0], next[1], next[2], &hit), 0.0f);
                CHECK(hit == rail || hit == railB);
                NEAR(find_floor_for_air_step(from, next, &hit), -300.0f);
                CHECK(hit != rail && hit != railB && hit != NULL);
                // A real downward crossing still lands on the narrow top.
                from[1] = 1.0f;
                next[1] = -1.0f;
                NEAR(find_floor_for_air_step(from, next, &hit), 0.0f);
                CHECK(hit == rail || hit == railB);
                // Exact contact remains supported, without a width restriction.
                from[1] = next[1] = 0.0f;
                NEAR(find_floor_for_air_step(from, next, &hit), 0.0f);
                CHECK(hit == rail || hit == railB);
            }
        }
    }
    reset();
    floor_at(-300);
    struct Surface *slope = triangle((Vec3s){ -1000, -100, -1000 }, (Vec3s){ 0, 0, 1000 },
                                     (Vec3s){ 1000, 100, -1000 }, 0, SURFACE_DEFAULT);
    struct Surface *hit;
    Vec3f from = { 0.0f, 4.0f, 0.0f }, next = { 50.0f, 2.0f, 0.0f };
    NEAR(find_floor_for_air_step(from, next, &hit), 5.0f);
    CHECK(hit == slope);
    from[1] = -1.0f;
    next[1] = -2.0f;
    NEAR(find_floor_for_air_step(from, next, &hit), -300.0f);
    CHECK(hit != slope);
    return TRUE;
}

static int bitfs_rising_lift(void) {
    const s16 positions[][2] = { { 0, 0 }, { -190, -190 }, { 190, 190 }, { -190, 190 }, { 190, -190 } };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 1; frame <= 12; frame++) {
            reset();
            memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
            memset(gDynamicSurfacePartition, 0, sizeof(gDynamicSurfacePartition));
            surfaceCount = nodeCount = 0;
            triangle((Vec3s){ -8000, -2000, -8000 }, (Vec3s){ -8000, -2000, 8000 },
                     (Vec3s){ 8000, -2000, -8000 }, 0, SURFACE_DEFAULT);
            triangle((Vec3s){ 8000, -2000, -8000 }, (Vec3s){ -8000, -2000, 8000 },
                     (Vec3s){ 8000, -2000, 8000 }, 0, SURFACE_DEFAULT);
            s16 liftY = -1279 + 10 * frame;
            for (unsigned face = 0; face < ARRAY_COUNT(bitfsLiftTriangles); face++) {
                Vec3s vertices[3];
                for (int vertex = 0; vertex < 3; vertex++) {
                    vertices[vertex][0] = bitfsLiftTriangles[face][vertex][0] + 2867;
                    vertices[vertex][1] = bitfsLiftTriangles[face][vertex][1] + liftY;
                    vertices[vertex][2] = bitfsLiftTriangles[face][vertex][2] + 307;
                }
                struct Surface *s = triangle(vertices[0], vertices[1], vertices[2], 1, SURFACE_DEFAULT);
                s->object = &object;
            }
            f32 top = liftY + 154;
            verifiedFloorRise = 10;
            for (unsigned sample = 0; sample < ARRAY_COUNT(positions); sample++) {
                struct Surface *hit;
                Vec3f start = { 2867 + positions[sample][0], top - 9, 307 + positions[sample][1] };
                Vec3f next = { start[0], start[1] - 4, start[2] };
                NEAR(find_floor_for_air_step(start, next, &hit), top);
                CHECK(hit != NULL && hit->object == &object && hit->normal.y > .99f);
                // The accepted landing becomes a normal supported contact next update.
                start[1] = top;
                next[1] = top - 4;
                NEAR(find_floor_for_air_step(start, next, &hit), top);
                // Being below the previous top must not become a side upwarp.
                start[1] = top - 11;
                next[1] = top - 15;
                CHECK(find_floor_for_air_step(start, next, &hit) < top - 20);
                start[1] = top - 9;
                next[1] = top - 13;
                verifiedFloorRise = 0;
                CHECK(find_floor_for_air_step(start, next, &hit) < top - 20);
                verifiedFloorRise = 10;
                gCollisionFlags = COLLISION_FLAG_EXCLUDE_DYNAMIC;
                NEAR(find_floor_for_air_step(start, next, &hit), -2000);
                EXPECT_EQ(gCollisionFlags, 0);
            }
            struct Surface *hit;
            Vec3f start = { 2867 + 210, top - 9, 307 }, next = { 2867 + 210, top - 13, 307 };
            NEAR(find_floor_for_air_step(start, next, &hit), -2000);
        }
    }
    return TRUE;
}

static void lab_bridge_collision(int frame, int yaw, Mat4 transform) {
    reset();
    floor_at(0);
    s16 roll = LAB_BRIDGE_ROLL_AMPLITUDE * sins_bhaskara((frame & 255) * 0x100);
    Vec3s angles = { 0, yaw * 0x4000, roll };
    lab_bridge_matrix(transform, (Vec3f){ 0, 350, 0 }, angles);
    for (unsigned face = 0; face < ARRAY_COUNT(labBridgeTriangles); face++) {
        Vec3s vertices[3];
        for (int vertex = 0; vertex < 3; vertex++) {
            linear_mtxf_mul_vec3_and_translate(transform, vertices[vertex],
                                               labBridgeTriangles[face][vertex]);
        }
        triangle(vertices[0], vertices[1], vertices[2], 1, SURFACE_DEFAULT)->object = &object;
    }
}

static int lab_bridge_edge_landings(void) {
    const f32 speeds[] = { MARIO_MAX_FALL_SPEED, HE_THICC_MAX_FALL_SPEED };
    const u32 caps[] = { 0, MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_WING_CAP };
    const int lengths[] = { -1050, 0, 1050 };
    for (int yaw = 0; yaw < 4; yaw++) {
        for (int frame = 0; frame < 256; frame++) {
            for (unsigned length = 0; length < ARRAY_COUNT(lengths); length++) {
                for (int edge = -1; edge <= 1; edge += 2) {
                    Mat4 oldTransform, transform;
                    lab_bridge_collision(frame - 1, yaw, oldTransform);
                    f32 x = oldTransform[0][0] * lengths[length] + oldTransform[2][0] * edge * 249;
                    f32 z = oldTransform[0][2] * lengths[length] + oldTransform[2][2] * edge * 249;
                    struct Surface *floor;
                    f32 oldHeight = find_floor_precise(x, 1000, z, &floor);
                    CHECK(floor != NULL && floor->object == &object);
                    for (unsigned cap = 0; cap < ARRAY_COUNT(caps); cap++) {
                        for (unsigned speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
                            lab_bridge_collision(frame, yaw, transform);
                            f32 height = find_floor_precise(x, 1000, z, &floor);
                            CHECK(floor != NULL && floor->object == &object);
                            // Provider pose/rounding is covered by platform.floor_rise.
                            verifiedFloorRise = height > oldHeight ? height - oldHeight + 1 : 0;
                            mario.flags = caps[cap];
                            mario.action = ACT_FREEFALL;
                            mario.waterLevel = -11000;
                            mario.floor = &surfaces[0];
                            mario.floorHeight = 0;
                            vec3f_set(mario.pos, x, oldHeight + 0.1f, z);
                            vec3f_set(mario.vel, 0, -speeds[speed], 0);
                            Vec3f next = { x, mario.pos[1] - speeds[speed] / 4, z };
                            int shouldLand = next[1] <= height;
                            int result = perform_air_quarter_step(&mario, next, 0, TRUE);
                            EXPECT_EQ(result == AIR_STEP_LANDED, shouldLand);
                            if (shouldLand) {
                                CHECK(mario.floor != NULL && mario.floor->object == &object);
                                CHECK(fabsf(mario.pos[1] - height) < 1.0f);
                            }
                        }
                    }
                }
            }
        }
    }
    return TRUE;
}

static int castle_outdoor_barriers(void) {
    for (int side = -1; side <= 1; side += 2) {
        reset();
        for (unsigned face = 0; face < ARRAY_COUNT(castleOuterRoofTriangles); face++) {
            Vec3s *v = castleOuterRoofTriangles[face].vertices;
            triangle(v[0], v[1], v[2], 0, castleOuterRoofTriangles[face].type);
        }
        mario.action = ACT_FREEFALL;
        mario.waterLevel = -11000;
        mario.faceAngle[1] = side > 0 ? (s16) 0x8888 : 0x7778;
        vec3f_set(mario.pos, side * 1468, 3100, -3295);
        mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
        CHECK(mario.floor != NULL);
        mario.pos[1] = mario.floorHeight;
        // Cross the pictured displaced edge, and its west-side counterpart.
        // Native floor/wall queries must retain support instead of inventing a wall.
        for (int step = 0; step < 32; step++) {
            Vec3f next = { mario.pos[0] + 32 * sins(mario.faceAngle[1]) / 4, mario.pos[1],
                           mario.pos[2] + 32 * coss(mario.faceAngle[1]) / 4 };
            EXPECT_EQ(perform_ground_quarter_step(&mario, next, TRUE), GROUND_STEP_NONE);
            NEAR(mario.pos[0], next[0]);
            NEAR(mario.pos[2], next[2]);
            CHECK(mario.floor != NULL);
            CHECK(mario.wall == NULL);
        }
    }
    return TRUE;
}

static int castle_waterfall_slide_exit(void) {
    // The Metal Cap Cave exit can land here. Every travel direction must have
    // support across the visible bank, including the formerly missing join.
    for (int heading = 0; heading < 0x10000; heading += 0x1000) {
        reset();
        for (unsigned face = 0; face < ARRAY_COUNT(castleCliffTriangles); face++) {
            Vec3s *v = castleCliffTriangles[face].vertices;
            triangle(v[0], v[1], v[2], 0, castleCliffTriangles[face].type);
        }
        mario.action = ACT_STOMACH_SLIDE;
        mario.waterLevel = -800;
        mario.faceAngle[1] = (s16) 0x802C;
        vec3f_set(mario.pos, -5741, -803, -5138);
        mario.floorHeight = find_floor(mario.pos[0], mario.pos[1] + 10, mario.pos[2], &mario.floor);
        CHECK(mario.floor != NULL);
        CHECK(mario.floor->type == SURFACE_VERY_SLIPPERY);
        mario.pos[1] = mario.floorHeight;
        Vec3f next = { mario.pos[0] + 25 * sins(heading), mario.pos[1],
                       mario.pos[2] + 25 * coss(heading) };
        CHECK(perform_ground_quarter_step(&mario, next, TRUE) == GROUND_STEP_NONE);
        NEAR(mario.pos[0], -5741 + 25 * sins(heading));
        NEAR(mario.pos[2], -5138 + 25 * coss(heading));
        CHECK(mario.floor != NULL);
        CHECK(mario.floor->type == SURFACE_VERY_SLIPPERY);
        CHECK(mario.wall == NULL);
        if (heading == 0x3000) {
            // Continue downhill far enough to enter the ordinary swim depth.
            for (int step = 0; step < 8 && mario.pos[1] >= -880; step++) {
                vec3f_set(next, mario.pos[0] + 25 * sins(heading), mario.pos[1],
                          mario.pos[2] + 25 * coss(heading));
                CHECK(perform_ground_quarter_step(&mario, next, TRUE) == GROUND_STEP_NONE);
            }
            CHECK(mario.pos[1] < -880);
        }
    }
    return TRUE;
}

static int castle_cliff_support(void) {
    reset();
    for (unsigned face = 0; face < ARRAY_COUNT(castleCliffTriangles); face++) {
        Vec3s *v = castleCliffTriangles[face].vertices;
        triangle(v[0], v[1], v[2], 0, castleCliffTriangles[face].type);
    }
    for (unsigned face = 0; face < ARRAY_COUNT(castleCliffTriangles); face++) {
        Vec3s *v = castleCliffTriangles[face].vertices;
        Vec3f a, b, n;
        vec3_diff(a, v[1], v[0]);
        vec3_diff(b, v[2], v[0]);
        vec3f_cross(n, a, b);
        vec3f_normalize(n);
        if (n[1] <= .01f) {
            continue;
        }
        f32 x = (v[0][0] + v[1][0] + v[2][0]) / 3.0f;
        f32 y = (v[0][1] + v[1][1] + v[2][1]) / 3.0f;
        f32 z = (v[0][2] + v[1][2] + v[2][2]) / 3.0f;
        struct Surface *floor;
        NEAR(find_floor_precise(x, y + 1, z, &floor), y);
        CHECK(floor != NULL);
    }
    return TRUE;
}

static int air_thin_slope_top(void) {
    // Pictured upper Lab ramp: diagonal descent near the 1200-unit plateau.
    // Nearby side probes must not push a genuine top contact through its slope.
    const s16 headings[] = { (s16) 0xE000, (s16) 0xEAE4, (s16) 0xF000 };
    int contacts = 0;
    for (unsigned heading = 0; heading < ARRAY_COUNT(headings); heading++) {
        for (int x = -13300; x <= -13260; x += 5) {
            for (int z = -12110; z <= -12080; z += 5) {
                for (int y = 1150; y <= 1190; y += 5) {
                    reset();
                    for (unsigned face = 0; face < ARRAY_COUNT(labUpperRampTriangles); face++) {
                        Vec3s *v = labUpperRampTriangles[face].vertices;
                        triangle(v[0], v[1], v[2], 0, labUpperRampTriangles[face].type);
                    }
                    mario.action = ACT_FREEFALL;
                    mario.waterLevel = -11000;
                    mario.forwardVel = 38.8f;
                    mario.faceAngle[1] = headings[heading];
                    vec3f_set(mario.pos, x, y, z);
                    vec3f_set(mario.vel, 38.8f * sins(headings[heading]), -8,
                              38.8f * coss(headings[heading]));
                    Vec3f next = { x + mario.vel[0] / 4, y - 2, z + mario.vel[2] / 4 };
                    Vec3f contact;
                    struct Surface *hit;
                    s32 crossed = find_air_step_floor_contact(mario.pos, next, contact, &hit, 0);
                    s32 result = perform_air_quarter_step(&mario, next, 0, TRUE);
                    if (crossed && y - floor_height_at_xz(x, z, hit) > .01f) {
                        contacts++;
                        // Moving away from the top can stay airborne. A wall
                        // push underneath its plane cannot erase the landing.
                        CHECK(result == AIR_STEP_LANDED
                              || mario.pos[1]
                                     >= floor_height_at_xz(mario.pos[0], mario.pos[2], hit) - .01f);
                        if (headings[heading] == (s16) 0xEAE4 && x == -13275 && y == 1170
                            && z == -12105) {
                            EXPECT_EQ(result, AIR_STEP_LANDED);
                            NEAR(mario.pos[0], contact[0]);
                            NEAR(mario.pos[1], contact[1]);
                            NEAR(mario.pos[2], contact[2]);
                        }
                    }
                }
            }
        }
    }
    CHECK(contacts > 0);
    return TRUE;
}

static int air_top_contact_obstructions(void) {
    reset();
    floor_at(350);
    triangle((Vec3s){ 0, 0, -4000 }, (Vec3s){ 0, 1000, 0 }, (Vec3s){ 0, 0, 4000 }, 0, SURFACE_DEFAULT);
    mario.action = ACT_FREEFALL;
    mario.waterLevel = -11000;
    vec3f_set(mario.pos, 100, 400, 0);
    vec3f_set(mario.vel, -200, -400, 0);
    // The uncorrected top crossing lies on the intervening wall. Landing
    // must retain its blocking side instead of restoring that crossing.
    EXPECT_EQ(perform_air_quarter_step(&mario, (Vec3f){ -100, 300, 0 }, 0, TRUE), AIR_STEP_LANDED);
    CHECK(mario.pos[0] >= 0);

    // A nearby wall still separates an ordinary flat-floor landing.
    vec3f_set(mario.pos, 30, 351, 0);
    vec3f_set(mario.vel, 0, -8, 0);
    EXPECT_EQ(perform_air_quarter_step(&mario, (Vec3f){ 30, 349, 0 }, 0, TRUE), AIR_STEP_LANDED);
    NEAR(mario.pos[0], 50);
    NEAR(mario.pos[1], 350);

    Mat4 transform;
    lab_bridge_collision(0, 0, transform);
    mario.action = ACT_FREEFALL;
    mario.waterLevel = -11000;
    mario.floor = &surfaces[0];
    mario.floorHeight = 0;
    vec3f_set(mario.pos, 0, 50, 0);
    vec3f_set(mario.vel, 0, 200, 0);
    CHECK(perform_air_quarter_step(&mario, (Vec3f){ 0, 100, 0 }, 0, TRUE) != AIR_STEP_LANDED);
    NEAR(mario.pos[1], 50);
    NEAR(mario.vel[1], 0);

    vec3f_set(mario.pos, 0, 340, 0);
    vec3f_set(mario.vel, 0, -127, 0);
    CHECK(perform_air_quarter_step(&mario, (Vec3f){ 0, 310, 0 }, 0, TRUE) != AIR_STEP_LANDED);
    return TRUE;
}

static int ttc_stopped_platform_entry(void) {
    for (int yaw = 0; yaw < 4; yaw++) {
        for (int gap = 1; gap <= 24; gap++) {
            reset();
            Mat4 transform;
            Vec3f position = { -1179, 445, 1413 };
            Vec3s angles = { 0, DEGREES(45) + yaw * 0x4000, 0 };
            mtxf_rotate_zxy_and_translate(transform, position, angles);
            for (unsigned face = 0; face < ARRAY_COUNT(ttcPlatformTriangles); face++) {
                Vec3s vertices[3];
                for (int vertex = 0; vertex < 3; vertex++) {
                    linear_mtxf_mul_vec3_and_translate(transform, vertices[vertex],
                                                       ttcPlatformTriangles[face][vertex]);
                }
                triangle(vertices[0], vertices[1], vertices[2], 1, SURFACE_NOT_SLIPPERY)->object =
                    &object;
            }
            mario.action = ACT_FREEFALL;
            mario.faceAngle[1] = angles[1];
            mario.forwardVel = 29;
            Vec3f start = { 0.25f, -gap, -132.0f }, end = { .25f, -gap - 1, -120.0f };
            for (int axis = 0; axis < 3; axis++) {
                mario.pos[axis] = position[axis];
                for (int local = 0; local < 3; local++) {
                    mario.pos[axis] += transform[local][axis] * start[local];
                }
            }
            Vec3f next;
            for (int axis = 0; axis < 3; axis++) {
                next[axis] = position[axis];
                for (int local = 0; local < 3; local++) {
                    next[axis] += transform[local][axis] * end[local];
                }
            }
            EXPECT_EQ(perform_air_quarter_step(&mario, next, 0, TRUE), AIR_STEP_LANDED);
            NEAR(mario.pos[1], position[1]);
            CHECK(mario.floor != NULL && mario.floor->object == &object);
        }
    }
    return TRUE;
}

static int air_riser_entry_boundaries(void) {
    for (int boundary = 0; boundary < 5; boundary++) {
        reset();
        s16 width = boundary == 1 ? 1 : 200;
        triangle((Vec3s){ 0, 0, -200 }, (Vec3s){ 0, 0, 200 }, (Vec3s){ width, 0, -200 }, 0,
                 SURFACE_DEFAULT);
        triangle((Vec3s){ width, 0, -200 }, (Vec3s){ 0, 0, 200 }, (Vec3s){ width, 0, 200 }, 0,
                 SURFACE_DEFAULT);
        if (boundary != 2) {
            wall_at(0, -100, 0, 0, SURFACE_DEFAULT);
        }
        if (boundary == 3) {
            triangle((Vec3s){ -200, 150, -200 }, (Vec3s){ 200, 150, -200 }, (Vec3s){ 0, 150, 200 }, 0,
                     SURFACE_DEFAULT);
        }
        mario.action = ACT_FREEFALL;
        mario.faceAngle[1] = 0x4000;
        mario.forwardVel = 16;
        vec3f_set(mario.pos, -2, -4, 0);
        Vec3f next = { .25f, -5, 0 };
        if (boundary == 4) {
            // The integer query includes the edge; the actual feet are outside.
            vec3f_set(mario.pos, -.25f, -4, -190);
            vec3f_set(next, 200.25f, -5, -190);
        }
        int result = perform_air_quarter_step(&mario, next, 0, TRUE);
        if (boundary == 0) {
            EXPECT_EQ(result, AIR_STEP_LANDED);
            NEAR(mario.pos[1], 0);
        } else {
            CHECK(result != AIR_STEP_LANDED);
            CHECK(mario.pos[1] < 0);
        }
    }
    return TRUE;
}

static int bowser_rising_tilt_landing(void) {
    int checked = 0;
    const s32 actions[] = { ACT_FREEFALL, ACT_FORWARD_ROLLOUT, ACT_BACKWARD_ROLLOUT, ACT_GROUND_POUND };
    // The first tilt phase reaches 39 * 10 angle units per update.
    const int tilts[] = { 40, 80, 160, 390 };
    for (unsigned tilt = 0; tilt < ARRAY_COUNT(tilts); tilt++) {
        for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
            int angle = tilts[tilt];
            for (int heading = 0; heading < 4; heading++) {
                reset();
                Mat4 transform;
                Vec3f position = { 0, 0, 0 };
                Vec3s angles = { angle, heading * 0x4000, 0 };
                mtxf_rotate_zxy_and_translate(transform, position, angles);
                for (unsigned face = 0; face < ARRAY_COUNT(bowserArenaTriangles); face++) {
                    Vec3s vertices[3];
                    for (int vertex = 0; vertex < 3; vertex++) {
                        linear_mtxf_mul_vec3_and_translate(transform, vertices[vertex],
                                                           bowserArenaTriangles[face][vertex]);
                    }
                    triangle(vertices[0], vertices[1], vertices[2], 1, SURFACE_DEFAULT)->object =
                        &object;
                }
                f32 x = -2700 * sins(angles[1]), z = -2700 * coss(angles[1]);
                struct Surface *floor;
                f32 top = find_floor_precise(x, 1600, z, &floor);
                CHECK(floor != NULL && floor->object == &object);
                verifiedFloorRise = top - 1229;
                CHECK(verifiedFloorRise > 0);
                mario.action = actions[action];
                mario.forwardVel = 16;
                vec3f_set(mario.pos, x, 1230, z);
                f32 descent = mario.action == ACT_GROUND_POUND ? -100 : -32;
                vec3f_set(mario.vel, 16 * coss(angles[1]), descent, -16 * sins(angles[1]));
                Vec3f next = { x + mario.vel[0] / 4, 1230 + descent / 4, z + mario.vel[2] / 4 };
                EXPECT_EQ(perform_air_quarter_step(&mario, next, 0, TRUE), AIR_STEP_LANDED);
                f32 landedHeight = find_floor_precise(mario.pos[0], 1600, mario.pos[2], &floor);
                NEAR(mario.pos[1], landedHeight);
                NEAR(mario.vel[0], 16 * coss(angles[1]));
                NEAR(mario.vel[2], -16 * sins(angles[1]));
                CHECK(mario.floor->object == &object);
                f32 rise = verifiedFloorRise;
                // Missing history and a stationary/falling top cannot grant recovery.
                const f32 rejectedRises[] = { 0, -rise };
                for (unsigned rejected = 0; rejected < ARRAY_COUNT(rejectedRises); rejected++) {
                    verifiedFloorRise = rejectedRises[rejected];
                    vec3f_set(mario.pos, x, 1230, z);
                    vec3f_set(next, x, 1230 + descent / 4, z);
                    CHECK(perform_air_quarter_step(&mario, next, 0, TRUE) != AIR_STEP_LANDED);
                }
                verifiedFloorRise = rise;
                // Dynamic exclusion also applies to this recovered contact.
                Vec3f start = { x, 1230, z }, contact;
                vec3f_set(next, x, 1230 + descent / 4, z);
                CHECK(!find_air_step_floor_contact(start, next, contact, &floor,
                                                   COLLISION_FLAG_EXCLUDE_DYNAMIC));
                // Mario below the previous top must still pass underneath.
                vec3f_set(mario.pos, x, 1227, z);
                next[1] = 1227 + descent / 4;
                CHECK(perform_air_quarter_step(&mario, next, 0, TRUE) != AIR_STEP_LANDED);
                CHECK(mario.pos[1] < 1229);
                checked++;
            }
        }
    }
    EXPECT_EQ(checked, 64);
    return TRUE;
}

static int ttm_box_cave_departure(void) {
    const s16 headings[] = { 0x8E33, 0x9F03, 0xA3A4, 0xA000 };
    for (unsigned heading = 0; heading < ARRAY_COUNT(headings); heading++) {
        reset();
        for (unsigned i = 0; i < ARRAY_COUNT(ttmCaveTriangles); i++) {
            Vec3s *v = ttmCaveTriangles[i].vertices;
            triangle(v[0], v[1], v[2], 0, ttmCaveTriangles[i].type);
        }
        Vec3s vertices[ARRAY_COUNT(caveBoxVertices)];
        // Actual hidden box placement from the level's macro-object list.
        for (unsigned i = 0; i < ARRAY_COUNT(vertices); i++) {
            vertices[i][0] = 1950 + caveBoxVertices[i][0] * coss(DEGREES(30))
                             + caveBoxVertices[i][2] * sins(DEGREES(30));
            vertices[i][1] = 880 + caveBoxVertices[i][1];
            vertices[i][2] = 1321 - caveBoxVertices[i][0] * sins(DEGREES(30))
                             + caveBoxVertices[i][2] * coss(DEGREES(30));
        }
        for (unsigned i = 0; i < ARRAY_COUNT(caveBoxFaces); i++) {
            s16 *face = caveBoxFaces[i];
            triangle(vertices[face[0]], vertices[face[1]], vertices[face[2]], 1,
                     SURFACE_NO_CAM_COLLISION)
                ->object = &object;
        }
        mario.action = ACT_FREEFALL;
        mario.faceAngle[1] = headings[heading];
        mario.forwardVel = 29;
        vec3f_set(mario.pos, 1971, 1080, 1239);
        mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
        NEAR(mario.floorHeight, 1080);
        vec3f_set(mario.vel, 29 * sins(mario.faceAngle[1]), 0, 29 * coss(mario.faceAngle[1]));
        int airborne = FALSE, touchedCave = FALSE;
        for (int frame = 0; frame < 20; frame++) {
            for (int step = 0; step < 4; step++) {
                Vec3f next = { mario.pos[0] + mario.vel[0] / 4, mario.pos[1] + mario.vel[1] / 4,
                               mario.pos[2] + mario.vel[2] / 4 };
                int result;
                if (airborne) {
                    result = perform_air_quarter_step(&mario, next, 0, TRUE);
                    if (result == AIR_STEP_LANDED) {
                        airborne = FALSE;
                        mario.vel[1] = 0;
                    }
                } else {
                    result = perform_ground_quarter_step(&mario, next, TRUE);
                    airborne = result == GROUND_STEP_LEFT_GROUND;
                }
                if (mario.floor != NULL && mario.floor->object == NULL && mario.floorHeight == 1081) {
                    touchedCave = TRUE;
                    if (mario.pos[1] < 1080) {
                        fprintf(stderr,
                                "TTM heading=%04x frame=%d result=%d feet=%.3f "
                                "x=%.3f z=%.3f\n",
                                (u16) headings[heading], frame, result, mario.pos[1], mario.pos[0],
                                mario.pos[2]);
                        return FALSE;
                    }
                }
            }
            if (airborne) {
                mario.vel[1] -= 4;
            }
            if (mario.pos[1] < 1000) {
                fprintf(stderr, "TTM fell below cave heading=%04x x=%.3f y=%.3f z=%.3f\n",
                        (u16) headings[heading], mario.pos[0], mario.pos[1], mario.pos[2]);
                return FALSE;
            }
        }
        CHECK(touchedCave);
    }
    return TRUE;
}

static int lab_backward_long_jump_stairs(void) {
    for (int approach = 0; approach <= 160; approach += 8) {
        for (int speed = 24; speed <= 128; speed += 8) {
            for (int quarter = 4; quarter <= 6; quarter++) {
                reset();
                for (unsigned i = 0; i < ARRAY_COUNT(labStairTriangles); i++) {
                    triangle(labStairTriangles[i][0], labStairTriangles[i][1], labStairTriangles[i][2],
                             0, SURFACE_DEFAULT);
                }
                mario.action = ACT_LONG_JUMP;
                mario.faceAngle[1] = 0;
                mario.forwardVel = -speed;
                vec3f_set(mario.pos, 14400.25f, 20, 1004.25f + approach);
                mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
                CHECK(mario.floor != NULL && mario.floorHeight == 20);
                vec3f_set(mario.vel, 0, 25, -speed);
                for (int frame = 0; frame < 12; frame++) {
                    int result = AIR_STEP_NONE;
                    for (int step = 0; step < quarter; step++) {
                        Vec3f next = { mario.pos[0], mario.pos[1] + mario.vel[1] / quarter,
                                       mario.pos[2] + mario.vel[2] / quarter };
                        result = perform_air_quarter_step(&mario, next, 0, TRUE);
                        struct Surface *support;
                        f32 top = find_floor_precise(mario.pos[0], 2000, mario.pos[2], &support);
                        if (mario.pos[1] < top - 1) {
                            fprintf(stderr,
                                    "stairs speed=%d steps=%d frame=%d result=%d feet=%.6f "
                                    "top=%.6f z=%.6f\n",
                                    speed, quarter, frame, result, mario.pos[1], top, mario.pos[2]);
                            return FALSE;
                        }
                        if (result == AIR_STEP_LANDED) {
                            break;
                        }
                    }
                    if (result == AIR_STEP_LANDED) {
                        mario.vel[1] = 25;
                    } else {
                        mario.vel[1] -= 2.4f;
                    }
                }
            }
        }
    }
    return TRUE;
}
static int air_floor_grounded_dives(void) {
    reset();
    mario.action = ACT_DIVE;
    for (unsigned i = 0; i < ARRAY_COUNT(labSlopeTriangles); i++) {
        Vec3s *v = labSlopeTriangles[i].vertices;
        triangle(v[0], v[1], v[2], 0, labSlopeTriangles[i].type);
    }
    // All three grades, fractional grounded positions and repeated departures.
    for (int grade = 0; grade < 3; grade++) {
        for (int sample = 0; sample < 8; sample++) {
            vec3f_set(mario.pos, -5700.25f, 2000, -1800.11f - grade * 700 - sample * .11f);
            for (int cycle = 0; cycle < 3; cycle++) {
                mario.floorHeight =
                    find_floor(mario.pos[0], mario.pos[1] + 10, mario.pos[2], &mario.floor);
                mario.pos[1] = mario.floorHeight;
                Vec3f origin;
                vec3f_copy(origin, mario.pos);
                mario.forwardVel = 45;
                mario.faceAngle[1] = (s16) 0x8000;
                vec3f_set(mario.vel, 0, 25, -45);
                int result = AIR_STEP_NONE;
                for (int frame = 0; frame < 24 && result != AIR_STEP_LANDED; frame++) {
                    for (int quarter = 0; quarter < 4; quarter++) {
                        Vec3f next = { mario.pos[0], mario.pos[1] + mario.vel[1] / 4,
                                       mario.pos[2] + mario.vel[2] / 4 };
                        result = perform_air_quarter_step(&mario, next, 0, TRUE);
                        CHECK(result == AIR_STEP_NONE || result == AIR_STEP_LANDED);
                        struct Surface *floor;
                        f32 ramp = find_floor_precise(mario.pos[0], 2000, mario.pos[2], &floor);
                        // Native grounded heights can round by one unit. A dive
                        // must not keep travelling through the ramp underneath.
                        CHECK(mario.pos[1] >= ramp - 1.01f);
                        if (result == AIR_STEP_LANDED) {
                            break;
                        }
                    }
                    mario.vel[1] -= 4;
                }
                EXPECT_EQ(result, AIR_STEP_LANDED);
                CHECK(mario.pos[2] < origin[2] - 1);
            }
        }
    }
    return TRUE;
}

static int air_floor_lab_ramp_sides(void) {
    const s16 headings[] = { 0x3000, 0x4000, 0x5000, 0x6888 };
    const s16 starts[] = { -3706, -3330, -3300, -3285, -3255, -3225 };
    const s16 speeds[] = { 32, 48, 96, 128 };
    const s16 lifts[] = { 25, 38, 45 };
    for (unsigned z = 0; z < ARRAY_COUNT(starts); z++) {
        for (unsigned heading = 0; heading < ARRAY_COUNT(headings); heading++) {
            for (unsigned speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
                for (unsigned lift = 0; lift < ARRAY_COUNT(lifts); lift++) {
                    reset();
                    for (unsigned face = 0; face < ARRAY_COUNT(labSideRampTriangles); face++) {
                        Vec3s *v = labSideRampTriangles[face].vertices;
                        triangle(v[0], v[1], v[2], 0, labSideRampTriangles[face].type);
                    }
                    mario.action = ACT_DIVE;
                    mario.faceAngle[1] = headings[heading];
                    mario.forwardVel = speeds[speed];
                    vec3f_set(mario.pos, 11482, 0, starts[z]);
                    mario.floorHeight =
                        find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
                    vec3f_set(mario.vel, speeds[speed] * sins(headings[heading]), lifts[lift],
                              speeds[speed] * coss(headings[heading]));
                    int stopped = FALSE;
                    for (int frame = 0; frame < 25 && !stopped; frame++) {
                        int checkWalls =
                            mario_wall_collision_begin(&mario, 4, 30, 150, mario.vel[1], 0);
                        set_mario_wall(&mario, NULL);
                        for (int step = 0; step < 4; step++) {
                            Vec3f next = { mario.pos[0] + mario.vel[0] / 4,
                                           mario.pos[1] + mario.vel[1] / 4,
                                           mario.pos[2] + mario.vel[2] / 4 };
                            int result = perform_air_quarter_step(&mario, next, 0, checkWalls);
                            f32 top = (-3200 - mario.pos[2]) / 3;
                            if (mario.pos[0] > 11550.1f && mario.pos[0] < 12049.9f
                                && mario.pos[2] > -4099.9f && mario.pos[2] < -3200.1f) {
                                CHECK(mario.pos[1] >= top - 1.01f);
                            }
                            if (mario.wall != NULL) {
                                checkWalls = TRUE;
                            }
                            // Ordinary wall results do not stop the remaining substeps.
                            stopped |= result == AIR_STEP_HIT_WALL;
                            if (result == AIR_STEP_LANDED) {
                                stopped = TRUE;
                                break;
                            }
                        }
                        mario.vel[1] -= 4;
                    }
                }
            }
        }
    }
    return TRUE;
}

static int air_floor_side_filters_and_travel(void) {
    for (int mode = 0; mode < 5; mode++) {
        reset();
        floor_at(0);
        triangle((Vec3s){ 0, 100, -100 }, (Vec3s){ 0, 0, 100 }, (Vec3s){ 200, 0, 100 }, 0,
                 SURFACE_NOT_SLIPPERY);
        triangle((Vec3s){ 0, 100, -100 }, (Vec3s){ 200, 0, 100 }, (Vec3s){ 200, 100, -100 }, 0,
                 SURFACE_NOT_SLIPPERY);
        struct Surface *side =
            triangle((Vec3s){ 0, 0, 100 }, (Vec3s){ 0, 100, -100 }, (Vec3s){ 0, 0, -100 }, 0,
                     mode == 1 || mode == 2 ? SURFACE_VANISH_CAP_WALLS : SURFACE_DEFAULT);
        if (mode == 1) {
            mario.flags = MARIO_VANISH_CAP;
        }
        if (mode == 2) {
            object.activeFlags |= ACTIVE_FLAG_MOVE_THROUGH_GRATE;
        }
        if (mode == 3) {
            side->flags |= SURFACE_FLAG_NO_CAM_COLLISION;
        }
        if (mode == 4) {
            triangle((Vec3s){ -200, 10, -200 }, (Vec3s){ 0, 10, 200 }, (Vec3s){ 200, 10, -200 }, 1,
                     SURFACE_DEFAULT);
            gCollisionFlags = COLLISION_FLAG_EXCLUDE_DYNAMIC;
        }
        mario.action = ACT_DIVE;
        mario.faceAngle[1] = 0x4000;
        mario.forwardVel = 32;
        vec3f_set(mario.pos, -2, 20, 0);
        mario.floorHeight = 0;
        Vec3f next = { 2, 20, 12 };
        int result = perform_air_quarter_step(&mario, next, 0, FALSE);
        NEAR(mario.pos[2], 12);
        NEAR(mario.pos[1], 20);
        if (mode == 1 || mode == 2) {
            NEAR(mario.pos[0], 2);
            EXPECT_EQ(result, AIR_STEP_NONE);
            CHECK(mario.wall == NULL);
        } else {
            CHECK(mario.pos[0] <= 0);
            EXPECT_EQ(result, AIR_STEP_HIT_WALL);
            CHECK(mario.wall == side);
            // The ordinary wall pass consumes EXCLUDE_DYNAMIC before floor queries.
            NEAR(mario.floorHeight, mode == 4 ? 10 : 0);
        }
        EXPECT_EQ(gCollisionFlags, 0);
    }
    return TRUE;
}

static int air_floor_crest_takeoff(void) {
    for (int downhill = 0; downhill < 2; downhill++) {
        for (int base = 0; base < 2; base++) {
            for (int sign = -1; sign <= 1; sign += 2) {
                reset();
                memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
                surfaceCount = nodeCount = 0;
                // A continuous ramp joins a flat or gently descending crest.
                const Vec3s vertices[][3] = {
                    { { -1000, 0, -1000 }, { -1000, 0, 1000 }, { 0, 500, 1000 } },
                    { { -1000, 0, -1000 }, { 0, 500, 1000 }, { 0, 500, -1000 } },
                    { { 0, 500, -1000 }, { 0, 500, 1000 }, { 1000, 500, 1000 } },
                    { { 0, 500, -1000 }, { 1000, 500, 1000 }, { 1000, 500, -1000 } }
                };
                for (unsigned face = 0; face < ARRAY_COUNT(vertices); face++) {
                    Vec3s v[3];
                    for (int point = 0; point < 3; point++) {
                        vec3s_set(v[point], sign * vertices[face][point][0], vertices[face][point][1],
                                  sign * vertices[face][point][2]);
                        if (downhill && ABS(v[point][0]) == 1000 && face >= 2) {
                            v[point][1] = 450;
                        }
                    }
                    triangle(v[0], v[1], v[2], 0, SURFACE_DEFAULT);
                }
                if (base) {
                    floor_at(-1000);
                }
                mario.action = ACT_DIVE;
                vec3f_set(mario.pos, sign * -5.25f, 2000, sign * 0.25f);
                mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
                mario.pos[1] = mario.floorHeight;
                mario.faceAngle[1] = sign * 0x4000;
                mario.forwardVel = 45;
                vec3f_set(mario.vel, sign * 45, 25, 0);
                int result = AIR_STEP_NONE;
                for (int frame = 0; frame < 24 && result != AIR_STEP_LANDED; frame++) {
                    for (int quarter = 0; quarter < 4; quarter++) {
                        Vec3f next = { mario.pos[0] + mario.vel[0] / 4, mario.pos[1] + mario.vel[1] / 4,
                                       mario.pos[2] };
                        result = perform_air_quarter_step(&mario, next, 0, TRUE);
                        CHECK(result == AIR_STEP_NONE || result == AIR_STEP_LANDED);
                        CHECK(mario.wall == NULL);
                        NEAR(mario.pos[0], next[0]);
                        if (frame == 0 && quarter == 0) {
                            EXPECT_EQ(result, AIR_STEP_NONE);
                            struct Surface *floor;
                            f32 height = find_floor_precise(next[0], next[1], next[2], &floor);
                            CHECK(mario.floor == floor);
                            NEAR(mario.floorHeight, height);
                        }
                        if (result == AIR_STEP_LANDED) {
                            break;
                        }
                    }
                    mario.vel[1] -= 4;
                }
                EXPECT_EQ(result, AIR_STEP_LANDED);
                CHECK(sign * mario.pos[0] > 100);
                NEAR(mario.forwardVel, 45);
            }
        }
    }
    return TRUE;
}

static int air_floor_bob_takeoffs(void) {
    reset();
    mario.action = ACT_DIVE;
    for (unsigned i = 0; i < ARRAY_COUNT(bobTriangles); i++) {
        Vec3s *v = bobTriangles[i].vertices;
        triangle(v[0], v[1], v[2], 0, bobTriangles[i].type);
    }
    int checked = 0, tinySlopes = 0, roundedBelow = 0;
    for (int i = 1; i < surfaceCount; i++) {
        struct Surface *surface = &surfaces[i];
        if (surface->normal.y < .94f || surface->normal.y > .9999f) {
            continue;
        }
        f32 slope = sqrtf(sqr(surface->normal.x) + sqr(surface->normal.z));
        for (int sample = 0; sample < 10; sample++) {
            Vec3f pos = { (surface->vertex1[0] + surface->vertex2[0] + surface->vertex3[0]) / 3.0f
                              + sample * .11f,
                          (surface->vertex1[1] + surface->vertex2[1] + surface->vertex3[1]) / 3.0f + 10,
                          (surface->vertex1[2] + surface->vertex2[2] + surface->vertex3[2]) / 3.0f
                              + sample * .17f };
            struct Surface *floor;
            f32 height = find_floor(pos[0], pos[1], pos[2], &floor);
            struct Surface *uncachedFloor;
            NEAR(height, uncached_find_floor(pos[0], pos[1], pos[2], &uncachedFloor));
            CHECK(floor == uncachedFloor);
            if (floor != surface) {
                continue;
            }
            pos[1] = height;
            vec3f_copy(mario.pos, pos);
            mario.floor = floor;
            mario.floorHeight = height;
            mario.wall = NULL;
            mario.flags = 0;
            mario.forwardVel = 45;
            vec3f_set(mario.vel, -45 * surface->normal.x / slope, 25, -45 * surface->normal.z / slope);
            mario.faceAngle[1] = atan2s(mario.vel[2], mario.vel[0]);
            Vec3f next = { pos[0] + mario.vel[0] / 4, pos[1] + 6.25f, pos[2] + mario.vel[2] / 4 };
            struct WallCollisionData lower, upper;
            mario_resolve_air_step_walls(&mario, next, TRUE, &lower, &upper);
            struct Surface *ceil;
            f32 ceilHeight = find_mario_ceil(next, height, &ceil);
            if (lower.numWalls || upper.numWalls || next[1] + 160 >= ceilHeight) {
                continue; // Real walls and low ceilings are separate contacts.
            }
            checked++;
            tinySlopes += surface->normal.y > .99f;
            roundedBelow += height < floor_height_at_xz(pos[0], pos[2], surface);
            int result = perform_air_quarter_step(&mario, next, 0, TRUE);
            CHECK(result == AIR_STEP_NONE || result == AIR_STEP_LANDED);
            CHECK(mario.wall == NULL);
            CHECK(sqr(mario.pos[0] - pos[0]) + sqr(mario.pos[2] - pos[2]) > 1);
            if (check_within_surface_triangle_bounds_f32(next[0], next[2], surface)) {
                CHECK(fabsf(mario.floorHeight - floor_height_at_xz(next[0], next[2], surface)) < 1);
            }
        }
    }
    CHECK(checked > 1000 && tinySlopes > 10 && roundedBelow > 100);
    return TRUE;
}

static int air_floor_support_boundaries(void) {
    reset();
    struct Surface *slope = triangle((Vec3s){ -1000, 500, -1000 }, (Vec3s){ 0, 500, 1000 },
                                     (Vec3s){ 1000, 1500, -1000 }, 0, SURFACE_DEFAULT);
    Vec3f pos = { -200.75f, 2000, -200.25f }, start;
    struct Surface *floor;
    f32 height = find_floor(pos[0], pos[1], pos[2], &floor);
    CHECK(floor == slope);
    pos[1] = height;
    get_air_step_floor_start(pos, slope, height, start);
    NEAR(start[1], MAX(height, floor_height_at_xz(pos[0], pos[2], slope)));
    NEAR(pos[1], height);
    pos[0] = 200.75f;
    height = find_floor(pos[0], 2000, pos[2], &floor);
    pos[1] = height;
    get_air_step_floor_start(pos, floor, height, start);
    CHECK(start[1] > pos[1]);
    NEAR(start[1], floor_height_at_xz(pos[0], pos[2], floor));
    // A remembered floor is not ground contact once Mario has fallen below it.
    pos[1] = floor_height_at_xz(pos[0], pos[2], slope) - .05f;
    get_air_step_floor_start(pos, slope, height, start);
    NEAR(start[1], pos[1]);
    Vec3f next = { pos[0] + 11.25f, pos[1] - 1, pos[2] }, contact;
    CHECK(find_floor_for_air_step(start, next, &floor) < height - 20);
    CHECK(!find_air_step_floor_contact(start, next, contact, &floor, 0));
    // Do not extend a stale support plane beyond its finite triangle.
    vec3f_set(pos, 2000.75f, height, 0);
    get_air_step_floor_start(pos, slope, height, start);
    NEAR(start[1], pos[1]);
    get_air_step_floor_start(pos, NULL, height, start);
    NEAR(start[1], pos[1]);
    return TRUE;
}

static int air_floor_grounded_seam(void) {
    const Vec3s vertices[][3] = { { { -1000, 500, -1000 }, { 0, 500, 1000 }, { 1000, 1500, -1000 } },
                                  { { 1000, 1500, -1000 }, { 0, 500, 1000 }, { 1000, 1000, 1000 } } };
    for (int sign = -1; sign <= 1; sign += 2) {
        reset();
        for (int face = 0; face < 2; face++) {
            Vec3s v[3];
            for (int point = 0; point < 3; point++) {
                vec3s_set(v[point], sign * vertices[face][point][0], vertices[face][point][1],
                          sign * vertices[face][point][2]);
            }
            triangle(v[0], v[1], v[2], 0, SURFACE_DEFAULT);
        }
        mario.action = ACT_DIVE;
        vec3f_set(mario.pos, sign * 500.5f, 2000, sign * .5f);
        mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
        mario.pos[1] = mario.floorHeight;
        // Rounded coordinates select one face; the actual footprint has just
        // crossed the shared diagonal onto its coplanar neighbor.
        CHECK(!check_within_surface_triangle_bounds_f32(mario.pos[0], mario.pos[2], mario.floor));
        mario.forwardVel = 45;
        mario.faceAngle[1] = sign * 0x4000;
        vec3f_set(mario.vel, sign * 45, 25, 0);
        Vec3f next = { mario.pos[0] + sign * 11.25f, mario.pos[1] + 6.25f, mario.pos[2] };
        EXPECT_EQ(perform_air_quarter_step(&mario, next, 0, TRUE), AIR_STEP_NONE);
        CHECK(mario.floor != NULL && mario.wall == NULL);
        NEAR(mario.pos[0], next[0]);
        struct Surface *support;
        f32 expected = find_floor_precise(next[0], 2000, next[2], &support);
        CHECK(mario.floor == support);
        NEAR(mario.floorHeight, expected);
    }
    return TRUE;
}

static int shallow_pool_edge_entry(void) {
    for (int side = 0; side < 2; side++) {
        for (int speed = 8; speed <= 64; speed *= 2) {
            for (int z = 7900; z <= 9400; z += 100) {
                reset();
                for (unsigned i = 0; i < ARRAY_COUNT(labPoolTriangles); i++) {
                    triangle(labPoolTriangles[i][0], labPoolTriangles[i][1], labPoolTriangles[i][2], 0,
                             SURFACE_DEFAULT);
                }
                mario.action = ACT_FREEFALL;
                mario.waterLevel = 180;
                mario.faceAngle[1] = side ? -0x4000 : 0x4000;
                mario_set_forward_vel(&mario, speed);
                vec3f_set(mario.pos, side ? 5701 : 3299, 240, z);
                mario.floorHeight = find_floor(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
                f32 fallSpeed = 0;
                int landed = FALSE;
                for (int frame = 0; frame < 30 && !landed; frame++) {
                    mario.vel[1] = fallSpeed;
                    for (int step = 0; step < 4 && !landed; step++) {
                        Vec3f next = { mario.pos[0] + mario.vel[0] / 4, mario.pos[1] + fallSpeed / 4,
                                       mario.pos[2] };
                        u32 result = perform_air_quarter_step(&mario, next, 0, TRUE);
                        CHECK(result == AIR_STEP_NONE || result == AIR_STEP_LANDED);
                        NEAR(mario.forwardVel, speed);
                        NEAR(mario.vel[0], side ? -speed : speed);
                        NEAR(mario.vel[2], 0);
                        landed = result == AIR_STEP_LANDED;
                    }
                    fallSpeed = MAX(fallSpeed - 4, -75);
                }
                CHECK(landed);
            }
        }
    }
    return TRUE;
}

static int air_floor_lab_dives(void) {
    reset();
    for (unsigned i = 0; i < ARRAY_COUNT(labUnevenTriangles); i++) {
        triangle(labUnevenTriangles[i][0], labUnevenTriangles[i][1], labUnevenTriangles[i][2], 0,
                 SURFACE_DEFAULT);
    }
    int missedEndpoints = 0, lowerEndpoints = 0;
    const f32 directions[][2] = {
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }, { 0.7071f, 0.7071f }, { -0.7071f, -0.7071f }
    };
    const f32 weights[][3] = { { .333333f, .333333f, .333334f },
                               { .98f, .01f, .01f },
                               { .01f, .98f, .01f },
                               { .01f, .01f, .98f },
                               { .49f, .49f, .02f },
                               { .49f, .02f, .49f },
                               { .02f, .49f, .49f } };
    for (unsigned i = 0; i < ARRAY_COUNT(labUnevenTriangles); i++) {
        for (unsigned sample = 0; sample < ARRAY_COUNT(weights); sample++) {
            Vec3f center = { 0, 0, 0 };
            for (int vertex = 0; vertex < 3; vertex++) {
                for (int axis = 0; axis < 3; axis++) {
                    center[axis] += labUnevenTriangles[i][vertex][axis] * weights[sample][vertex];
                }
            }
            for (unsigned direction = 0; direction < ARRAY_COUNT(directions); direction++) {
                for (int travel = 8; travel <= 64; travel *= 2) {
                    Vec3f start = { center[0], center[1] + 4.0f, center[2] };
                    Vec3f delta = { directions[direction][0] * travel, -8,
                                    directions[direction][1] * travel };
                    Vec3f next, contact;
                    vec3f_sum(next, start, delta);
                    struct Surface *crossed, *floor;
                    find_surface_on_ray(start, delta, &crossed, contact,
                                        RAYCAST_FIND_FLOOR | RAYCAST_MARIO_WALL
                                            | RAYCAST_IGNORE_INTANGIBLE);
                    f32 height = find_floor_for_air_step(start, next, &floor);
                    if (crossed != NULL && (floor == NULL || next[1] > height)) {
                        missedEndpoints++;
                    }
                    if (crossed != NULL && floor != NULL && next[1] <= height) {
                        f32 offset = floor->normal.x * (contact[0] - floor->vertex1[0])
                                     + floor->normal.y * (contact[1] - floor->vertex1[1])
                                     + floor->normal.z * (contact[2] - floor->vertex1[2]);
                        if (offset > .01f) {
                            lowerEndpoints++;
                        }
                    }
                    Vec3f repairedContact;
                    struct Surface *repairedFloor;
                    s32 hit =
                        find_air_step_floor_contact(start, next, repairedContact, &repairedFloor, 0);
                    if (crossed != NULL) {
                        EXPECT_EQ(hit, TRUE);
                    }
                    if (hit) {
                        if (crossed != NULL) {
                            for (int axis = 0; axis < 3; axis++) {
                                NEAR(repairedContact[axis], contact[axis]);
                            }
                        }
                        struct Surface *support;
                        NEAR(find_floor_precise(repairedContact[0], repairedContact[1] + 1.0f,
                                                repairedContact[2], &support),
                             repairedContact[1]);
                        CHECK(support != NULL);
                        CHECK(repairedContact[1] <= start[1]);
                    }
                }
            }
        }
    }
    CHECK(missedEndpoints > 0);
    CHECK(lowerEndpoints > 0);
    return TRUE;
}

// Rebuild only the seam walls using their real cell bounds rather than the
// intentionally over-inclusive partitions used by the older water fixtures.
static void partition_seam_walls(void) {
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            gStaticSurfacePartition[z][x][SPATIAL_PARTITION_WALLS] = NULL;
            gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_WALLS] = NULL;
        }
    }
    for (int i = 1; i < surfaceCount; i++) {
        struct Surface *wall = &surfaces[i];
        int minX = GET_CELL_COORD(MIN(wall->vertex1[0], MIN(wall->vertex2[0], wall->vertex3[0])));
        int maxX = GET_CELL_COORD(MAX(wall->vertex1[0], MAX(wall->vertex2[0], wall->vertex3[0])));
        int minZ = GET_CELL_COORD(MIN(wall->vertex1[2], MIN(wall->vertex2[2], wall->vertex3[2])));
        int maxZ = GET_CELL_COORD(MAX(wall->vertex1[2], MAX(wall->vertex2[2], wall->vertex3[2])));
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                struct SurfaceNode **head =
                    wall->flags & SURFACE_FLAG_DYNAMIC
                        ? &gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_WALLS]
                        : &gStaticSurfacePartition[z][x][SPATIAL_PARTITION_WALLS];
                struct SurfaceNode *node = &nodes[nodeCount++];
                node->surface = wall;
                node->next = *head;
                *head = node;
            }
        }
    }
}

static int wall_seam_crossing(void) {
    for (int mode = 0; mode < 8; mode++) {
        const s32 inAir = mode & 1, dynamic = (mode / 2) & 1;
        for (int angle = 5; angle < 180; angle += 5) {
            for (int order = 0; order < 2; order++) {
                for (int trajectory = -12; trajectory <= 12; trajectory++) {
                    reset();
                    f32 radians = angle * 3.14159265358979323846f / 180.0f;
                    s16 x = -1000 * sinf(radians), z = 1000 * cosf(radians);
                    Vec3s walls[2][4] = {
                        { { 0, 0, 0 }, { 0, 1000, 0 }, { 0, 1000, 1000 }, { 0, 0, 1000 } },
                        { { 0, 0, 0 }, { x, 0, z }, { x, 1000, z }, { 0, 1000, 0 } }
                    };
                    if (mode >= 4) {
                        // Nearly vertical faces still belong to the wall partition.
                        walls[0][1][0] = walls[0][2][0] = -8;
                        walls[1][2][0] = x - 8;
                        walls[1][3][0] = -8;
                    }
                    for (int wall = 0; wall < 2; wall++) {
                        int index = wall ^ order;
                        struct Surface *face = triangle(walls[index][0], walls[index][2],
                                                        walls[index][1], dynamic, SURFACE_DEFAULT);
                        face->flags |= dynamic ? SURFACE_FLAG_DYNAMIC : 0;
                        face = triangle(walls[index][0], walls[index][3], walls[index][2], dynamic,
                                        SURFACE_DEFAULT);
                        face->flags |= dynamic ? SURFACE_FLAG_DYNAMIC : 0;
                    }
                    partition_seam_walls();
                    // Start on the front of both planes and repeatedly approach their joining seam.
                    vec3f_set(mario.pos, -600 * sinf(radians / 2), 400, 600 * cosf(radians / 2));
                    for (int frame = 0; frame < 120; frame++) {
                        vec3f_set(mario.vel, 16 * sinf(radians / 2) + trajectory * cosf(radians / 2),
                                  (frame < 60 ? 2 : -2),
                                  -16 * cosf(radians / 2) + trajectory * sinf(radians / 2));
                        Vec3f next, delta, origin, contact;
                        vec3f_sum(next, mario.pos, mario.vel);
                        struct WallCollisionData lower, upper;
                        s32 check = mario_wall_collision_begin(&mario, order ? 4 : 1, 30,
                                                               inAir ? 150 : 60, 2, 0);
                        if (inAir) {
                            mario_resolve_air_step_walls(&mario, next, check, &lower, &upper);
                        } else {
                            mario_resolve_ground_step_walls(&mario, next, check, FALSE, &lower, &upper);
                        }
                        vec3f_diff(delta, next, mario.pos);
                        for (int probe = 30; probe <= (inAir ? 150 : 60); probe += (inAir ? 120 : 30)) {
                            vec3f_copy(origin, mario.pos);
                            origin[1] += probe;
                            struct Surface *crossed;
                            find_surface_on_ray(origin, delta, &crossed, contact,
                                                RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL);
                            if (crossed != NULL) {
                                f32 endDistance = crossed->normal.x * next[0]
                                                  + crossed->normal.y * (next[1] + probe)
                                                  + crossed->normal.z * next[2] + crossed->originOffset;
                                if (endDistance < -0.01f) {
                                    fprintf(stderr,
                                            "Wall seam crossing angle %d order %d trajectory %d frame "
                                            "%d: from %.3f %.3f to %.3f %.3f, wall distance %.3f\n",
                                            angle, order, trajectory, frame, mario.pos[0], mario.pos[2],
                                            next[0], next[2], endDistance);
                                    return FALSE;
                                }
                            }
                        }
                        vec3f_copy(mario.pos, next);
                    }
                }
            }
        }
    }
    return TRUE;
}

static int wall_seam_release(void) {
    for (int inAir = 0; inAir < 2; inAir++) {
        for (int cached = 0; cached < 2; cached++) {
            reset();
            wall_at(0, -1000, 1000, 0, SURFACE_DEFAULT);
            mario.action = ACT_SIDE_FLIP;
            vec3f_set(mario.pos, -50, 100, 100);
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int update = 0; update < 12; update++) {
                    f32 oldZ = mario.pos[2], oldY = mario.pos[1];
                    vec3f_set(mario.vel, 8, update < 6 ? 2 : -2, 16);
                    Vec3f next;
                    vec3f_sum(next, mario.pos, mario.vel);
                    struct WallCollisionData lower, upper;
                    s32 check =
                        mario_wall_collision_begin(&mario, cached ? 4 : 1, 30, inAir ? 150 : 60, 2, 0);
                    if (inAir) {
                        mario_resolve_air_step_walls(&mario, next, check, &lower, &upper);
                    } else {
                        mario_resolve_ground_step_walls(&mario, next, check, FALSE, &lower, &upper);
                    }
                    NEAR(next[0], MIN(mario.pos[0] + 8, -50));
                    NEAR(next[1], oldY + mario.vel[1]);
                    NEAR(next[2], oldZ + 16);
                    vec3f_copy(mario.pos, next);
                }
                Vec3f next;
                vec3f_set(mario.vel, -16, 2, 0);
                vec3f_sum(next, mario.pos, mario.vel);
                struct WallCollisionData lower, upper;
                s32 check =
                    mario_wall_collision_begin(&mario, cached ? 4 : 1, 30, inAir ? 150 : 60, 2, 0);
                if (inAir) {
                    mario_resolve_air_step_walls(&mario, next, check, &lower, &upper);
                } else {
                    mario_resolve_ground_step_walls(&mario, next, check, FALSE, &lower, &upper);
                }
                NEAR(next[0], mario.pos[0] - 16);
                NEAR(next[1], mario.pos[1] + 2);
                vec3f_copy(mario.pos, next);
            }
        }
    }
    for (int grate = 0; grate < 2; grate++) {
        reset();
        wall_at(0, -1000, 1000, 0, SURFACE_VANISH_CAP_WALLS);
        mario.flags = grate ? 0 : MARIO_VANISH_CAP;
        object.activeFlags = grate ? ACTIVE_FLAG_MOVE_THROUGH_GRATE : 0;
        vec3f_set(mario.pos, -10, 100, 100);
        vec3f_set(mario.vel, 40, 2, 0);
        Vec3f next;
        vec3f_sum(next, mario.pos, mario.vel);
        struct WallCollisionData lower, upper;
        s32 check = mario_wall_collision_begin(&mario, 4, 30, 150, 2, 0);
        mario_resolve_air_step_walls(&mario, next, check, &lower, &upper);
        NEAR(next[0], 30);
        NEAR(next[1], 102);
    }
    return TRUE;
}

static int air_floor_sweep_boundaries(void) {
    reset();
    floor_at(0);
    struct Surface *dynamic = triangle((Vec3s){ -1000, 20, -1000 }, (Vec3s){ 0, 20, 1000 },
                                       (Vec3s){ 1000, 20, -1000 }, 1, SURFACE_DEFAULT);
    struct Surface *intangible = triangle((Vec3s){ -1000, 30, -1000 }, (Vec3s){ 0, 30, 1000 },
                                          (Vec3s){ 1000, 30, -1000 }, 0, SURFACE_INTANGIBLE);
    Vec3f start = { 0.5f, 40, 0.5f }, next = { 0.5f, -5, 0.5f }, contact;
    struct Surface *hit;
    const s16 flags[] = { 0, COLLISION_FLAG_EXCLUDE_DYNAMIC, COLLISION_FLAG_INCLUDE_INTANGIBLE };
    for (int flag = 0; flag < 3; flag++) {
        gCollisionFlags = COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_CAMERA;
        CHECK(find_air_step_floor_contact(start, next, contact, &hit, flags[flag]));
        f32 expectedHeight;
        if (flag == 1) {
            expectedHeight = 0;
        } else if (flag == 2) {
            expectedHeight = 30;
        } else {
            expectedHeight = 20;
        }
        NEAR(contact[1], expectedHeight);
        CHECK(flag == 1 || hit == (flag == 2 ? intangible : dynamic));
        EXPECT_EQ(gCollisionFlags, COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_CAMERA);
    }
    start[1] = -1;
    next[1] = -5;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    CHECK(hit == NULL);
    next[1] = 5;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    start[1] = next[1] = 0;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    vec3f_set(start, 5000, 40, 5000);
    vec3f_set(next, 5000, -5, 5000);
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    reset();
    struct Surface *vanish = triangle((Vec3s){ -1000, 0, -1000 }, (Vec3s){ 0, 0, 1000 },
                                      (Vec3s){ 1000, 0, -1000 }, 0, SURFACE_VANISH_CAP_WALLS);
    vec3f_set(start, .5f, 5, .5f);
    vec3f_set(next, .5f, -5, .5f);
    CHECK(find_air_step_floor_contact(start, next, contact, &hit, 0));
    CHECK(hit == vanish);
    mario.flags = MARIO_VANISH_CAP;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    mario.flags = 0;
    object.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    object.activeFlags = 0;
    vanish->flags |= SURFACE_FLAG_NO_CAM_COLLISION;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, COLLISION_FLAG_CAMERA));
    vanish->flags = 0;
    vanish->type = SURFACE_CAMERA_BOUNDARY;
    CHECK(!find_air_step_floor_contact(start, next, contact, &hit, 0));
    CHECK(find_air_step_floor_contact(start, next, contact, &hit, COLLISION_FLAG_CAMERA));
    return TRUE;
}

static int air_floor_edge_flags(void) {
    for (int flag = 0; flag < 3; flag++) {
        reset();
        floor_at(0);
        struct Surface *dynamic = triangle((Vec3s){ -1000, 20, -1000 }, (Vec3s){ 0, 20, 1000 },
                                           (Vec3s){ 1000, 20, -1000 }, 1, SURFACE_DEFAULT);
        struct Surface *intangible = triangle((Vec3s){ -1000, 30, -1000 }, (Vec3s){ 0, 30, 1000 },
                                              (Vec3s){ 1000, 30, -1000 }, 0, SURFACE_INTANGIBLE);
        triangle((Vec3s){ -1000, 50, -1000 }, (Vec3s){ 0, 50, 1000 }, (Vec3s){ 1000, 50, -1000 }, 0,
                 SURFACE_DEFAULT);
        Vec3f from = { 0.5f, 49.0f, 0.5f }, next = { 0.5f, 45.0f, 0.5f };
        struct Surface *hit;
        gCollisionFlags = flag == 1   ? COLLISION_FLAG_EXCLUDE_DYNAMIC
                          : flag == 2 ? COLLISION_FLAG_INCLUDE_INTANGIBLE
                                      : 0;
        f32 expectedHeight;
        if (flag == 1) {
            expectedHeight = 0.0f;
        } else if (flag == 2) {
            expectedHeight = 30.0f;
        } else {
            expectedHeight = 20.0f;
        }
        NEAR(find_floor_for_air_step(from, next, &hit), expectedHeight);
        CHECK(flag == 1 || hit == (flag == 2 ? intangible : dynamic));
        EXPECT_EQ(gCollisionFlags, 0);
    }
    return TRUE;
}

static int precise_filters_and_flags(void) {
    reset();
    floor_at(0);
    struct Surface *dynamic = triangle((Vec3s){ -1000, 10, -1000 }, (Vec3s){ 0, 10, 1000 },
                                       (Vec3s){ 1000, 10, -1000 }, 1, SURFACE_DEFAULT);
    struct Surface *intangible = triangle((Vec3s){ -1000, 20, -1000 }, (Vec3s){ 0, 20, 1000 },
                                          (Vec3s){ 1000, 20, -1000 }, 0, SURFACE_INTANGIBLE);
    struct Surface *hit;
    NEAR(find_floor_precise(.5f, 50, .5f, &hit), 10);
    CHECK(hit == dynamic);
    gCollisionFlags = COLLISION_FLAG_EXCLUDE_DYNAMIC;
    NEAR(find_floor_precise(.5f, 50, .5f, &hit), 0);
    EXPECT_EQ(gCollisionFlags, 0);
    gCollisionFlags = COLLISION_FLAG_INCLUDE_INTANGIBLE;
    NEAR(find_floor_precise(.5f, 50, .5f, &hit), 20);
    CHECK(hit == intangible);
    EXPECT_EQ(gCollisionFlags, 0);
    intangible->type = SURFACE_CAMERA_BOUNDARY;
    NEAR(find_floor_precise(.5f, 50, .5f, &hit), 10);
    gCollisionFlags = COLLISION_FLAG_CAMERA;
    NEAR(find_floor_precise(.5f, 50, .5f, &hit), 20);
    EXPECT_EQ(gCollisionFlags, COLLISION_FLAG_CAMERA);
    gCollisionFlags = 0;
    struct Surface *ceil = triangle((Vec3s){ -1000, 200, -1000 }, (Vec3s){ 1000, 200, -1000 },
                                    (Vec3s){ 0, 200, 1000 }, 1, SURFACE_DEFAULT);
    triangle((Vec3s){ -1000, 300, -1000 }, (Vec3s){ 1000, 300, -1000 }, (Vec3s){ 0, 300, 1000 }, 0,
             SURFACE_DEFAULT);
    NEAR(find_ceil_precise(.5f, 50, .5f, &hit), 200);
    CHECK(hit == ceil);
    gCollisionFlags = COLLISION_FLAG_EXCLUDE_DYNAMIC;
    NEAR(find_ceil_precise(.5f, 50, .5f, &hit), 300);
    EXPECT_EQ(gCollisionFlags, 0);
    // Recovery may inspect geometry but must leave one-shot flags for the
    // ordinary first wall query, even when recovery is unnecessary/impossible.
    const s16 flags = COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC
                      | COLLISION_FLAG_INCLUDE_INTANGIBLE;
    for (int y = -100; y <= 100; y += 100) {
        mario.pos[1] = y;
        gCollisionFlags = flags;
        recover_water_overlap(&mario, NULL, NULL, NULL);
        EXPECT_EQ(gCollisionFlags, flags);
        resolve_mario_geometry_walls(&mario);
        EXPECT_EQ(gCollisionFlags, 0);
    }
    return 1;
}
static int moving_floor_recovery(void) {
    reset();
    struct Surface *floor = triangle((Vec3s){ -1000, 0, -1000 }, (Vec3s){ 0, 0, 1000 },
                                     (Vec3s){ 1000, 0, -1000 }, 1, SURFACE_DEFAULT);
    for (int frame = 0; frame < 20; frame++) {
        // Move the dynamic triangle between frames, then run the real query /
        // recovery / swimming sequence. Object transforms are outside this fixture.
        int lift = frame < 10 ? 4 : -4;
        floor->vertex1[1] += lift;
        floor->vertex2[1] += lift;
        floor->vertex3[1] += lift;
        floor->lowerY += lift;
        floor->upperY += lift;
        floor->originOffset -= lift;
        resolve_mario_geometry_walls(&mario);
        CHECK(mario_water_collision_step(&mario, (Vec3f){ 12, -6, 0 }, NULL) != WATER_STEP_CANCELLED);
        NEAR(mario.pos[0], 12 * (frame + 1));
        NEAR(mario.pos[1], floor->vertex1[1] + WATER_COLLISION_SKIN);
    }
    return 1;
}
static void load_jrb(void) {
    reset();
    surfaceCount = nodeCount = 0;
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
    for (int i = 0; i < (int) (sizeof(jrbTriangles) / sizeof(jrbTriangles[0])); i++) {
        int *t = jrbTriangles[i];
        triangle(jrbVertices[t[0]], jrbVertices[t[1]], jrbVertices[t[2]], 0, t[3]);
    }
    mario.waterLevel = 1024;
}
static void load_jrb_object_rock(unsigned placement) {
    load_jrb();
    const s32 *p = swimmingRockPlacements[placement];
    s16 yaw = (s16) (p[3] * 65536 / 360);
    Vec3s vertices[12];
    for (unsigned i = 0; i < ARRAY_COUNT(vertices); i++) {
        // Production object transforms truncate rotated local coordinates
        // before adding the integer placement, not the world-space sum.
        vertices[i][0] =
            (s16) (swimmingRockVertices[i][0] * coss(yaw) + swimmingRockVertices[i][2] * sins(yaw))
            + p[0];
        vertices[i][1] = swimmingRockVertices[i][1] + p[1];
        vertices[i][2] =
            (s16) (-swimmingRockVertices[i][0] * sins(yaw) + swimmingRockVertices[i][2] * coss(yaw))
            + p[2];
    }
    for (unsigned i = 0; i < ARRAY_COUNT(swimmingRockTriangles); i++) {
        int *t = swimmingRockTriangles[i];
        triangle(vertices[t[0]], vertices[t[1]], vertices[t[2]], 1, SURFACE_DEFAULT)->flags |=
            SURFACE_FLAG_DYNAMIC;
    }
}
static void reanchor_ship_fixture(Vec3f point) {
    Mat4 before, after;
    Vec3f delta, local;
    ship_fixture_reference_matrix(before, swimmingShipPosition, swimmingShipRotation);
    mtxf_rotate_zxy_and_translate(after, swimmingShipPosition, swimmingShipRotation);
    for (int axis = 0; axis < 3; axis++) {
        delta[axis] = point[axis] - swimmingShipPosition[axis];
    }
    for (int axis = 0; axis < 3; axis++) {
        local[axis] =
            delta[0] * before[axis][0] + delta[1] * before[axis][1] + delta[2] * before[axis][2];
    }
    // Keep the original height above the static seabed.
    for (int axis = 0; axis < 3; axis += 2) {
        point[axis] = local[0] * after[0][axis] + local[1] * after[1][axis] + local[2] * after[2][axis]
                      + swimmingShipPosition[axis];
    }
}

static void load_jrb_sunken_ship(void) {
    load_jrb();
    Mat4 transform;
    Vec3s vertices[ARRAY_COUNT(swimmingShipVertices)];
    mtxf_rotate_zxy_and_translate(transform, swimmingShipPosition, swimmingShipRotation);
    for (unsigned i = 0; i < ARRAY_COUNT(vertices); i++) {
        Vec3f local;
        vec3s_to_vec3f(local, swimmingShipVertices[i]);
        linear_mtxf_mul_vec3_and_translate(transform, vertices[i], local);
    }
    for (unsigned i = 0; i < ARRAY_COUNT(swimmingShipTriangles); i++) {
        int *t = swimmingShipTriangles[i];
        triangle(vertices[t[0]], vertices[t[1]], vertices[t[2]], 1, t[3])->flags |=
            SURFACE_FLAG_DYNAMIC;
    }
}

static int jrb_object_rock_escape(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING,
                            ACT_WATER_PLUNGE, ACT_WATER_DOWN_BURST };
    int checked = 0;
    for (unsigned placement = 0; placement < ARRAY_COUNT(swimmingRockPlacements); placement++) {
        if (swimmingRockPlacements[placement][1] >= 0) {
            continue;
        }
        load_jrb_object_rock(placement);
        struct Surface *cap = &surfaces[surfaceCount - 20];
        Vec3f center;
        for (int axis = 0; axis < 3; axis++) {
            center[axis] = (cap->vertex1[axis] + cap->vertex2[axis] + cap->vertex3[axis]) / 3.0f;
        }
        struct Surface *floor;
        f32 height = find_floor_precise(center[0], center[1], center[2], &floor);
        CHECK(floor == cap);
        for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                vec3f_copy(mario.pos, center);
                mario.pos[1] = height - 50;
                mario.action = actions[action];
                mario.floor = cap;
                mario.floorHeight = height;
                mario.forwardVel = 64;
                gCollisionFlags = 0;
                // The former full-clearance check rejects this first 16-unit escape.
                resolve_mario_geometry_walls(&mario);
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 16, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
                CHECK(mario.pos[1] > height - 50);
                for (int frame = 0; frame < 8; frame++) {
                    resolve_mario_geometry_walls(&mario);
                    CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 64, 0 }, NULL)
                          != WATER_STEP_CANCELLED);
                }
                CHECK(mario.pos[1] > height + 100);
                NEAR(mario.forwardVel, 64);
                // Once outside, a fast return must still stop on the same rock.
                CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, -128, 0 }, NULL)
                      != WATER_STEP_CANCELLED);
                CHECK(mario.pos[1] >= height - .02f);
                checked++;
            }
        }
        // Exercise steering after embedding with the ordinary held-A target.
        vec3f_copy(mario.pos, center);
        mario.pos[1] = height - 50;
        mario.action = ACT_FLUTTER_KICK;
        mario.forwardVel = 64;
        mario.faceAngle[0] = 0;
        mario.faceAngle[1] = atan2s(cap->normal.z, cap->normal.x) + 0x8000;
        vec3s_set(mario.angleVel, 0, 0, 0);
        controller.stickY = -64;
        for (int frame = 0; frame < 96; frame++) {
            resolve_mario_geometry_walls(&mario);
            mario.forwardVel = approach_f32(mario.forwardVel, A_SWIM_SPEED, A_SWIM_ACCEL, 0);
            common_swimming_step(&mario, 200, 0);
        }
        CHECK(mario.pos[1] > height);
    }
    CHECK(checked >= 300);
    return 1;
}
static int jrb_seam_travel(void) {
    const Vec3f starts[] = { { -2927.245361f, 512.010010f, 5258.087891f },
                             { -2195, 512.010010f, 5688.666504f },
                             { -1941.018677f, 512.010010f, 5837.066895f } };
    Vec3f travel[] = { { -16.970562f, -24, -16.970562f },
                       { 0, -24, -24 },
                       { 16.970562f, -24, -16.970562f } };
    for (int i = 0; i < 3; i++) {
        load_jrb();
        vec3f_copy(mario.pos, starts[i]);
        for (int frame = 0; frame < 10; frame++) {
            Vec3f before;
            vec3f_copy(before, mario.pos);
            resolve_mario_geometry_walls(&mario);
            CHECK(mario_water_collision_step(&mario, travel[i], NULL) != WATER_STEP_CANCELLED);
            CHECK(sqr(mario.pos[0] - before[0]) + sqr(mario.pos[2] - before[2]) > 1);
            CHECK(mario.pos[1] <= before[1] + .1f);
        }
        f32 oldY = mario.pos[1];
        CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, 24, 0 }, NULL) != WATER_STEP_CANCELLED);
        CHECK(mario.pos[1] > oldY + 20);
    }
    return 1;
}
static int jrb_downward_approaches(void) {
    load_jrb();
    int checked = 0, contacts = 0;
    for (int i = 0; i < surfaceCount; i++) {
        struct Surface *s = &surfaces[i];
        if (s->normal.y < .01f) {
            continue;
        }
        f32 x = (s->vertex1[0] + s->vertex2[0] + s->vertex3[0]) / 3.0f;
        f32 y = (s->vertex1[1] + s->vertex2[1] + s->vertex3[1]) / 3.0f;
        f32 z = (s->vertex1[2] + s->vertex2[2] + s->vertex3[2]) / 3.0f;
        if (y > 850 || x < -6304 || x > 7992 || z < -669 || z > 7814) {
            continue;
        }
        for (int heading = 0; heading < 4; heading++) {
            vec3f_set(mario.pos, x, y + 20, z);
            mario.floorHeight = find_floor(x, y + 20, z, &mario.floor);
            if (mario.floor != s) {
                continue;
            }
            mario.forwardVel = 56;
            vec3f_set(mario.vel, 37 * sins(heading * 0x4000), -41, 37 * coss(heading * 0x4000));
            u32 result = perform_water_step(&mario);
            if (mario.pos[1] > y + 20.1f) {
                fprintf(stderr, "JRB triangle %d, heading %d: requested downward, rose %.3f units\n", i,
                        heading, mario.pos[1] - (y + 20));
                return 0;
            }
            checked++;
            contacts += result == WATER_STEP_HIT_FLOOR;
        }
    }
    CHECK(checked >= 700);
    CHECK(contacts > 500);
    return 1;
}
static int downward_controls(void) {
    const f32 speeds[] = { 12, 24, 56.5685f };
    for (int held = 0; held < 2; held++) {
        for (int rise = 0; rise <= 2000; rise += 1000) {
            for (int speed = 0; speed < 3; speed++) {
                reset();
                // Keep the same gradients but leave room for the full smooth turn
                // upward at stroke speed; the old small floor ended mid-recovery.
                triangle((Vec3s){ -3000, -6 * rise, -6000 }, (Vec3s){ -3000, -6 * rise, 6000 },
                         (Vec3s){ 3000, 6 * rise, 0 }, 0, SURFACE_DEFAULT);
                mario.action = held ? ACT_HOLD_FLUTTER_KICK : ACT_FLUTTER_KICK;
                mario.heldObj = held ? &object : NULL;
                controller.stickY = 64;
                mario.forwardVel = speeds[speed];
                mario.faceAngle[0] = -0x2000;
                mario.faceAngle[1] = 0x4000;
                vec3f_set(mario.pos, 0, 20, 0);
                // Match the existing downward-pitch visual offset on entry.
                f32 oldGfxY = 50;
                for (int frame = 0; frame < 8; frame++) {
                    f32 oldY = mario.pos[1];
                    common_swimming_step(&mario, 200, 0);
                    CHECK(mario.faceAngle[0] < 0);
                    CHECK(mario.pos[1] <= oldY + .1f);
                    // Pitch rendering can move by a few units per update, but
                    // must not turn collision into a visible upward launch.
                    CHECK(object.header.gfx.pos[1] <= oldGfxY + 3.0f);
                    oldGfxY = object.header.gfx.pos[1];
                }
                CHECK(fabsf(mario.pos[1] - (rise / 500.0f) * mario.pos[0]) < .2f);
                // Steering up after contact still escapes the floor normally.
                controller.stickY = -64;
                for (int frame = 0; frame < 70; frame++) {
                    mario.forwardVel = approach_f32(mario.forwardVel, A_SWIM_SPEED, A_SWIM_ACCEL, 0);
                    common_swimming_step(&mario, 200, 0);
                }
                CHECK(mario.faceAngle[0] > 0);
                if (rise == 0) {
                    CHECK(mario.pos[1] > 20);
                }
            }
        }
    }
    return 1;
}
static int jrb_downward_controls(void) {
    load_jrb();
    // A confirmed steep floor from the reported level, not a synthetic plane.
    struct Surface *floor = &surfaces[25];
    CHECK(jrbTriangles[25][0] == 69 && jrbTriangles[25][1] == 66 && jrbTriangles[25][2] == 67);
    f32 x = (floor->vertex1[0] + floor->vertex2[0] + floor->vertex3[0]) / 3.0f;
    f32 y = (floor->vertex1[1] + floor->vertex2[1] + floor->vertex3[1]) / 3.0f;
    f32 z = (floor->vertex1[2] + floor->vertex2[2] + floor->vertex3[2]) / 3.0f;
    vec3f_set(mario.pos, x, y + 20, z);
    mario.floor = floor;
    mario.floorHeight = y;
    mario.action = ACT_FLUTTER_KICK;
    mario.forwardVel = 56.5685f;
    mario.faceAngle[0] = -0x2000;
    mario.faceAngle[1] = -0x4000;
    controller.stickY = 64;
    for (int frame = 0; frame < 8; frame++) {
        f32 oldY = mario.pos[1];
        common_swimming_step(&mario, 200, 0);
        CHECK(mario.pos[1] <= oldY + .1f);
        CHECK(mario.faceAngle[0] < 0);
    }
    CHECK(mario.pos[1] < y);
    return 1;
}
static int slope_floor_assist(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING };
    const f32 speeds[] = { 12, 24, 56 };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (unsigned speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
            for (int grade = -1; grade <= 1; grade++) {
                for (int dynamic = 0; dynamic < 2; dynamic++) {
                    for (int cycle = 0; cycle < 3; cycle++) {
                        reset();
                        mario.action = actions[action];
                        mario.heldObj = action ? &object : NULL;
                        mario.waterLevel = 6000;
                        mario.faceAngle[1] = 0x4000;
                        triangle((Vec3s){ -2000, -2000 * grade, -6000 },
                                 (Vec3s){ -2000, -2000 * grade, 6000 },
                                 (Vec3s){ 2000, 2000 * grade, 0 }, dynamic, SURFACE_DEFAULT);
                        mario.forwardVel = speeds[speed];
                        mario.faceAngle[0] = -DEGREES(60);
                        vec3f_set(mario.pos, 0, 1, 0);
                        // Downward input still works; neutral release restores bottom assistance.
                        controller.stickY = 64;
                        common_swimming_step(&mario, 200, 0);
                        CHECK(mario.faceAngle[0] < -DEGREES(59));
                        controller.stickY = 0;
                        for (int frame = 0; frame < 48; frame++) {
                            s16 previousPitch = mario.faceAngle[0];
                            common_swimming_step(&mario, 200, 0);
                            CHECK(mario.faceAngle[0] - previousPitch <= 0x200);
                            CHECK(mario.pos[1] >= mario.floorHeight - .02f);
                        }
                        CHECK(mario.faceAngle[0] >= -DEGREES(46));
                        if (grade >= 0) {
                            CHECK(mario.faceAngle[0] >= -DEGREES(1));
                        }
                        if (grade > 0) {
                            CHECK(mario.faceAngle[0] > DEGREES(30));
                        }
                        // Keep travelling and explicitly steer upward out of floor contact.
                        Vec3f before;
                        vec3f_copy(before, mario.pos);
                        controller.stickY = -64;
                        for (int frame = 0; frame < 32; frame++) {
                            mario.forwardVel = approach_f32(mario.forwardVel, 24, .6f, 0);
                            resolve_mario_geometry_walls(&mario);
                            common_swimming_step(&mario, 200, 0);
                            CHECK(mario.pos[1] >= mario.floorHeight - .02f);
                        }
                        CHECK(mario.pos[0] > before[0] + 10);
                        CHECK(mario.faceAngle[0] > 0);
                        CHECK(mario.pos[1] > mario.floorHeight + 1);
                    }
                }
            }
        }
    }
    return 1;
}
static int stunned_motion(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        wall_at(40, -500, 500, cycle % 2, SURFACE_DEFAULT);
        object.oPosY = 20;
        object.oWallHitboxRadius = 10;
        object.hitboxHeight = 30;
        object.oForwardVel = 120;
        object.oMoveAngleYaw = 0x4000;
        object.oGravity = -4;
        object.oVelY = 8;
        cur_obj_move_stunned();
        CHECK(object.oPosX <= 30.05f);
        NEAR(object.oPosY, 24);
        NEAR(object.oVelY, 4);
        NEAR(object.oForwardVel, 0);
        EXPECT_EQ(!!(object.oMoveFlags & OBJ_MOVE_HIT_WALL), TRUE);
        reset();
        floor_at(0);
        object.oPosY = 20;
        object.hitboxHeight = 30;
        object.oWallHitboxRadius = 10;
        object.oVelY = -74;
        object.oGravity = -4;
        object.oBounciness = 0;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 0);
        NEAR(object.oVelY, 0);
        EXPECT_EQ(!!(object.oMoveFlags & OBJ_MOVE_LANDED), TRUE);
        cur_obj_move_stunned();
        NEAR(object.oPosY, 0);
        EXPECT_EQ(!!(object.oMoveFlags & OBJ_MOVE_ON_GROUND), TRUE);
    }
    return 1;
}
static void stunned_setup(f32 y) {
    object.oPosY = y;
    object.hitboxHeight = 30;
    object.oWallHitboxRadius = 10;
    object.oMoveAngleYaw = 0x4000;
}
static void mole_setup(f32 y) {
    stunned_setup(y);
    object.behavior = bhvMontyMole;
    object.hitboxHeight = 50;
    object.hitboxRadius = 70;
    object.oWallHitboxRadius = 30;
    object.oGraphYOffset = -60;
    object.oGravity = 0;
    object.oBounciness = -.5f;
    object.oGroundTwirlStunDuration = 12;
    object.oMontyMoleCurrentHole = &moleHole;
    object.prevObj = &moleRock;
}
static int stunned_mole_clearance(void) {
    const f32 heights[] = { 0, 25, 50, 160 };
    for (unsigned height = 0; height < ARRAY_COUNT(heights); height++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            floor_at(0);
            mole_setup(heights[height]);
            object.oGroundTwirlStunTimer = -8;
            object.oVelY = 30;
            object.oForwardVel = 15;
            object.metalContactCooldown = 15;
            update_object_with_ground_twirl_stun();
            // Launching from a partly buried pose must still rise normally.
            NEAR(object.oPosY, heights[height] + 26);
            NEAR(object.oVelY, 26);
            EXPECT_EQ(object.oGroundTwirlStunTimer, -7);
            int frames = 1;
            while (object.oGroundTwirlStunTimer < 0 && frames++ < 100) {
                update_object_with_ground_twirl_stun();
            }
            CHECK(frames < 100);
            NEAR(object.oPosY, 50);
            NEAR(object.header.gfx.pos[1], -10);
            NEAR(object.oFloorHeight, 0);
            CHECK(object.oFloor != NULL);
            NEAR(object.oVelY, 0);
            NEAR(object.oGravity, 0);
            NEAR(object.oWallHitboxRadius, 30);
            EXPECT_EQ(object.oGroundTwirlStunTimer, 12);
            int elapsed = 0;
            f32 endX = object.oPosX;
            while (object.oGroundTwirlStunTimer > 0) {
                update_object_with_ground_twirl_stun();
                elapsed++;
                NEAR(object.oPosY, 50);
                NEAR(object.header.gfx.pos[1], -10);
                NEAR(object.oPosX, endX);
                EXPECT_EQ(nativeEnemyUpdates, 0);
            }
            EXPECT_EQ(elapsed, 12);
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(nativeEnemyUpdates, 1);
            // Deep incoming descent must land at exposed height despite native bounce.
            mole_setup(60);
            object.oGroundTwirlStunTimer = -8;
            object.oVelY = -74;
            update_object_with_ground_twirl_stun();
            NEAR(object.oPosY, 50);
            NEAR(object.oVelY, 0);
            EXPECT_EQ(object.oGroundTwirlStunTimer, 12);
            // Rebounding an already stunned Mole uses the same support, then releases.
            object.oGroundTwirlStunTimer = 0;
            object.oGroundTwirlContactRebound = 24;
            object.oVelY = 12;
            frames = 0;
            while (object.oGroundTwirlContactRebound > 0 && frames++ < 30) {
                update_object_with_ground_twirl_stun();
            }
            CHECK(frames < 30);
            NEAR(object.oPosY, 50);
            NEAR(object.header.gfx.pos[1], -10);
            EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
        }
    }
    return 1;
}
static int stunned_snufit_clearance(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        floor_at(0);
        stunned_setup(100);
        object.behavior = bhvSnufit;
        object.oGroundTwirlStunTimer = -8;
        object.oGroundTwirlStunDuration = 12;
        object.oVelY = -20;
        int frames = 0;
        while (object.oGroundTwirlStunTimer < 0 && frames++ < 100) {
            update_object_with_ground_twirl_stun();
        }
        CHECK(frames < 100);
        NEAR(object.oPosY, 50);
        NEAR(object.header.gfx.pos[1], 50);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 12);
        update_object_with_ground_twirl_stun();
        NEAR(object.oPosY, 50);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 11);
        // A lowered floor starts a fall without refreshing the remaining stun.
        memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
        floor_at(-100);
        update_object_with_ground_twirl_stun();
        CHECK(object.oPosY < 50);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 10);
        while (object.oGroundTwirlStunTimer > 0) {
            update_object_with_ground_twirl_stun();
        }
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(nativeEnemyUpdates, 1);
    }
    return 1;
}
static int stunned_mole_support_changes(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        floor_at(0);
        mole_setup(50);
        object.oGroundTwirlStunTimer = 10;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 9);
        // Replace the real floor: no fake raised floor metadata and no stun refresh.
        memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
        floor_at(-40);
        int frames = 0;
        while (object.oPosY > 10 && frames++ < 20) {
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(object.oGroundTwirlStunTimer, 9 - frames);
        }
        CHECK(frames < 20);
        NEAR(object.oPosY, 10);
        NEAR(object.oFloorHeight, -40);
        NEAR(object.header.gfx.pos[1], -50);
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 8 - frames);
        floor_at(-20);
        update_object_with_ground_twirl_stun();
        NEAR(object.oPosY, 30);
        NEAR(object.oFloorHeight, -20);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 7 - frames);
        // A disappeared floor releases support and resumes real gravity.
        memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
        update_object_with_ground_twirl_stun();
        NEAR(object.oPosY, 26);
        NEAR(object.oVelY, -4);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 6 - frames);
        EXPECT_EQ(object.oFloor, NULL);
        object.oPosY = FLOOR_LOWER_LIMIT_MISC - 1;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
        // A sloping support is sampled at the knockback endpoint.
        reset();
        triangle((Vec3s){ -200, -100, -200 }, (Vec3s){ 0, 0, 200 }, (Vec3s){ 200, 100, -200 }, 0,
                 SURFACE_DEFAULT);
        mole_setup(60);
        object.oForwardVel = 20;
        object.oVelY = -50;
        object.oGroundTwirlStunTimer = -8;
        update_object_with_ground_twirl_stun();
        NEAR(object.oPosY, object.oFloorHeight + 50);
        NEAR(object.header.gfx.pos[1], object.oFloorHeight - 10);
        CHECK(object.oPosX > 0);
    }
    return 1;
}
static int stunned_mole_hazards(void) {
    const int types[] = { SURFACE_BURNING, SURFACE_DEATH_PLANE, SURFACE_VERTICAL_WIND };
    for (unsigned type = 0; type < ARRAY_COUNT(types); type++) {
        reset();
        floor_at(0);
        surfaces[surfaceCount - 1].type = types[type];
        mole_setup(60);
        object.oGroundTwirlStunTimer = -8;
        object.oVelY = -74;
        object.oBounciness = 0;
        update_object_with_ground_twirl_stun();
        NEAR(object.oPosY, 0);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
        EXPECT_EQ(object.oGroundTwirlContactRebound, 0);
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(nativeEnemyUpdates, 1);
    }
    reset();
    floor_at(0);
    mole_setup(60);
    enemyWater = 100;
    waterDeathEligible = TRUE;
    object.oGroundTwirlStunTimer = -8;
    object.oVelY = -20;
    update_object_with_ground_twirl_stun();
    CHECK(object.oMoveFlags & OBJ_MOVE_MASK_IN_WATER);
    CHECK(object.oPosY < 50);
    EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
    update_object_with_ground_twirl_stun();
    EXPECT_EQ(nativeEnemyUpdates, 1);
    // Explicit attacks bypass support correction before native attack handling.
    for (int grounded = 0; grounded < 2; grounded++) {
        reset();
        floor_at(0);
        mole_setup(25);
        object.oGroundTwirlStunTimer = grounded ? 8 : -8;
        object.oInteractStatus = INT_STATUS_WAS_ATTACKED;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(nativeEnemyUpdates, 1);
        NEAR(object.oPosY, 25);
        EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
        CHECK(object.oInteractStatus & INT_STATUS_WAS_ATTACKED);
    }
    return 1;
}
static int stunned_mole_native_resume(void) {
    const int actions[] = { MONTY_MOLE_ACT_RISE_FROM_HOLE,
                            MONTY_MOLE_ACT_SPAWN_ROCK,
                            MONTY_MOLE_ACT_THROW_ROCK,
                            MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE,
                            MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE,
                            MONTY_MOLE_ACT_JUMP_INTO_HOLE,
                            MONTY_MOLE_ACT_HIDE };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            floor_at(0);
            mole_setup(60);
            object.oAction = actions[action];
            object.oTimer = 17;
            object.oGroundTwirlStunTimer = -8;
            object.oVelY = -74;
            object.oForwardVel = 15;
            if (actions[action] == MONTY_MOLE_ACT_JUMP_INTO_HOLE) {
                object.oGravity = -6;
            }
            if (actions[action] == MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE) {
                object.oGravity = -4;
            }
            nativeMoleAI = TRUE;
            update_object_with_ground_twirl_stun();
            NEAR(object.oPosY, 50);
            f32 endX = object.oPosX;
            while (object.oGroundTwirlStunTimer > 0) {
                update_object_with_ground_twirl_stun();
                EXPECT_EQ(object.oAction, actions[action]);
                EXPECT_EQ(object.oTimer, 17);
                EXPECT_EQ(object.prevObj, &moleRock);
                EXPECT_EQ(object.oMontyMoleCurrentHole, &moleHole);
                EXPECT_EQ(moleHole.oMontyMoleHoleCooldown, 0);
            }
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(nativeEnemyUpdates, 1);
            EXPECT_EQ(object.oGroundTwirlStunDuration, 0);
            NEAR(object.oPosX, endX);
            EXPECT_EQ(object.prevObj, &moleRock);
            EXPECT_EQ(object.oTimer, 17);
            if (actions[action] == MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE) {
                EXPECT_EQ(object.oAction, MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE);
                NEAR(object.oPosY, 50);
                NEAR(object.oGravity, 0);
                NEAR(object.oVelY, 0);
            } else if (actions[action] == MONTY_MOLE_ACT_JUMP_INTO_HOLE
                       || actions[action] == MONTY_MOLE_ACT_HIDE) {
                CHECK(object.oPosY < 50);
                EXPECT_EQ(moleHidden, FALSE);
                int frames = 0;
                while (object.oAction != MONTY_MOLE_ACT_SELECT_HOLE && frames++ < 150) {
                    update_object_with_ground_twirl_stun();
                }
                CHECK(frames < 150);
                EXPECT_EQ(moleHidden, TRUE);
                EXPECT_EQ(moleHole.oMontyMoleHoleCooldown, 30);
                NEAR(object.oPosY, 0);
            } else {
                EXPECT_EQ(object.oAction, actions[action]);
                NEAR(object.oPosY, 50);
            }
        }
    }
    // Without a resume marker the jump-out threshold retains its native boundary.
    reset();
    floor_at(0);
    mole_setup(50);
    object.oGroundTwirlStunDuration = 0;
    object.oAction = MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE;
    object.oGravity = -4;
    nativeMoleAI = TRUE;
    update_object_with_ground_twirl_stun();
    EXPECT_EQ(object.oAction, MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE);
    NEAR(object.oPosY, 46);
    return 1;
}
static int stunned_fixed_triangle_cast(void) {
    const Vec3f triangles[][3] = { { { 40, -500, -500 }, { 40, -500, 500 }, { 40, 500, 0 } },
                                   { { -200, 0, -200 }, { 0, 0, 200 }, { 200, 0, -200 } },
                                   { { -200, 50, -200 }, { 200, 50, -200 }, { 0, 50, 200 } },
                                   { { -200, -100, -200 }, { 0, 0, 200 }, { 200, 100, -200 } },
                                   { { 40, 9, -200 }, { 40, 9, 200 }, { 40, 14, 0 } },
                                   { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } };
    const Vec3f starts[] = { { 0, 20, 0 },  { 40, 20, 0 }, { 80, 20, 0 },   { 30, 0, 0 },
                             { 30, 80, 0 }, { 0, -40, 0 }, { 40, 30, 200 }, { 40, 30, -200 } };
    const Vec3f travel[] = { { 120, 0, 0 },  { -120, 0, 0 },  { 0, 60, 0 }, { 0, -78, 0 },
                             { 80, -60, 0 }, { 0, -20, 120 }, { 0, 0, 0 } };
    for (unsigned shape = 0; shape < ARRAY_COUNT(triangles); shape++) {
        for (int shifted = 0; shifted < 2; shifted++) {
            for (int wide = 0; wide < 2; wide++) {
                for (unsigned from = 0; from < ARRAY_COUNT(starts); from++) {
                    for (unsigned move = 0; move < ARRAY_COUNT(travel); move++) {
                        struct MovingBodyProfile profile = { 30,
                                                             { 10, 10 },
                                                             { wide ? 10 : 0, wide ? 10 : 0 } };
                        Vec3f vertices[3], copy[3], start, end, fastNormal, referenceNormal;
                        for (int vertex = 0; vertex < 3; vertex++) {
                            for (int axis = 0; axis < 3; axis++) {
                                vertices[vertex][axis] = copy[vertex][axis] =
                                    triangles[shape][vertex][axis] + (shifted ? 37 : 0);
                            }
                        }
                        for (int axis = 0; axis < 3; axis++) {
                            start[axis] = starts[from][axis] + (shifted ? 37 : 0);
                            end[axis] = start[axis] + travel[move][axis];
                        }
                        f32 fastFraction = -1, referenceFraction = -1;
                        stunnedTriangleNormalCount = 0;
                        s32 fast = moving_triangle_cast(&profile, start, end, vertices, vertices,
                                                        &fastFraction, fastNormal);
                        EXPECT_EQ(stunnedTriangleNormalCount, 1);
                        // Separate but identical endpoints exercise the general moving-triangle
                        // path as an independent reference for the fixed-geometry shortcut.
                        s32 reference = moving_triangle_cast(&profile, start, end, vertices, copy,
                                                             &referenceFraction, referenceNormal);
                        EXPECT_EQ(fast, reference);
                        if (fast == MOVING_CAST_HIT) {
                            CHECK(fabsf(fastFraction - referenceFraction) < .000001f);
                            for (int axis = 0; axis < 3; axis++) {
                                CHECK(fabsf(fastNormal[axis] - referenceNormal[axis]) < .000001f);
                            }
                        }
                    }
                }
            }
        }
    }
    // Real vertex motion still uses the general path.
    struct MovingBodyProfile profile = { 30, { 10, 10 }, { 10, 10 } };
    Vec3f before[3], after[3], normal, start = { 0, 20, 0 }, end = { 0, 20, 0 };
    f32 fraction;
    for (int vertex = 0; vertex < 3; vertex++) {
        for (int axis = 0; axis < 3; axis++) {
            before[vertex][axis] = triangles[0][vertex][axis];
            after[vertex][axis] = before[vertex][axis] - (axis == 0 ? 60 : 0);
        }
    }
    EXPECT_EQ(moving_triangle_cast(&profile, start, end, before, after, &fraction, normal),
              MOVING_CAST_HIT);
    CHECK(fraction > .49f && fraction < .51f);
    EXPECT_EQ(moving_triangle_cast(&profile, start, end, before, before, &fraction, normal),
              MOVING_CAST_CLEAR);
    return 1;
}
static int stunned_finite_edge_clearance(void) {
    const struct MovingBodyProfile profiles[] = { { 60, { 10, 10 }, { 30, 30 } },
                                                  { 60, { 10, 50 }, { 30, 30 } },
                                                  { 60, { 10, 50 }, { 15, 30 } },
                                                  { 60, { 10, 50 }, { 30, 15 } } };
    for (unsigned shape = 0; shape < ARRAY_COUNT(profiles); shape++) {
        for (int shifted = 0; shifted < 2; shifted++) {
            struct MovingBodyProfile profile = profiles[shape];
            Vec3f vertices[3] = { { 0, 0, 0 }, { 0, 1000, 0 }, { 0, 0, 500 } };
            Vec3f position = { 80, 200, -60 }, normal;
            Vec3f shift = { shifted ? 2000 : 0, shifted ? -500 : 0, shifted ? 1600 : 0 };
            for (int axis = 0; axis < 3; axis++) {
                position[axis] += shift[axis];
                for (int vertex = 0; vertex < 3; vertex++) {
                    vertices[vertex][axis] += shift[axis];
                }
            }
            // The horizontal radius is bounded by 30. A largest disk reaches
            // the closest finite edge at this height, attaining distance 70.
            f32 distance = moving_triangle_distance(&profile, position, vertices, normal);
            CHECK(fabsf(distance - 70) < .003f);
            CHECK(fabsf(normal[0] - .8f) < .003f);
            CHECK(fabsf(normal[1]) < .003f);
            CHECK(fabsf(normal[2] + .6f) < .003f);
            Vec3f start = { 100 + shift[0], 200 + shift[1], -20 + shift[2] };
            Vec3f end = { -100 + shift[0], 200 + shift[1], -20 + shift[2] };
            f32 fraction;
            EXPECT_EQ(moving_triangle_cast(&profile, start, end, vertices, vertices, &fraction, normal),
                      MOVING_CAST_HIT);
            f32 x = start[0] + fraction * (end[0] - start[0]) - shift[0];
            f32 radial = sqrtf(x * x + 400);
            CHECK(radial >= 30 - .003f && radial <= 30 + MOVING_CONTACT_SKIN + .003f);
            CHECK(normal[0] > 0 && normal[2] < 0);
            // The edge's extension must not turn the cone above its top into
            // a cylinder. Both endpoint disks are above the finite triangle.
            start[1] = end[1] = 1000 + shift[1];
            EXPECT_EQ(moving_triangle_cast(&profile, start, end, vertices, vertices, &fraction, normal),
                      MOVING_CAST_CLEAR);
        }
    }
    // Check accepted travel through the caller, including a second separating
    // step. The swept body's side stays clear of the edge and keeps falling.
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        reset();
        floor_at(0);
        triangle((Vec3s){ 0, 0, 0 }, (Vec3s){ 0, 1000, 0 }, (Vec3s){ 0, 0, 500 }, dynamic,
                 SURFACE_DEFAULT);
        struct Object body = { .hitboxHeight = 60 };
        Vec3f start = { 32, 200, -20 }, target = { 16, 196, -20 };
        CHECK(sweep_object_motion(&body, start, target, NULL, 30) & OBJ_MOVE_HIT_WALL);
        CHECK(sqrtf(target[0] * target[0] + target[2] * target[2]) >= 30 - .003f);
        NEAR(target[1], 196);
        CHECK(target[2] < -20);
        vec3f_copy(start, target);
        vec3f_set(target, start[0] + 16, start[1] - 4, start[2] - 8);
        EXPECT_EQ(sweep_object_motion(&body, start, target, NULL, 30), 0);
        NEAR(target[0], start[0] + 16);
        NEAR(target[1], start[1] - 4);
        NEAR(target[2], start[2] - 8);
    }
    return TRUE;
}

static int stunned_simplex_distance(void) {
    const f32 points[][4][3] = { { { 2, 3, 6 } },
                                 { { -2, 3, 4 }, { 2, 3, 4 } },
                                 { { 2, 0, 5 }, { -1, 2, 5 }, { -1, -2, 5 } },
                                 { { 1, 1, 1 }, { 1, -1, -1 }, { -1, 1, -1 }, { -1, -1, 1 } },
                                 { { -1, -1, 5 }, { 1, -1, 5 }, { 1, 1, 5 }, { -1, 1, 5 } },
                                 { { 2, 3, 6 }, { 2, 3, 6 }, { 2, 3, 6 }, { 2, 3, 6 } } };
    const int counts[] = { 1, 2, 3, 4, 4, 4 };
    const f32 expected[][3] = { { 2, 3, 6 }, { 0, 3, 4 }, { 0, 0, 5 },
                                { 0, 0, 0 }, { 0, 0, 5 }, { 2, 3, 6 } };
    for (unsigned shape = 0; shape < ARRAY_COUNT(points); shape++) {
        for (int reverse = 0; reverse < 2; reverse++) {
            Vec3f simplex[4], closest;
            s32 count = counts[shape];
            for (int i = 0; i < count; i++) {
                for (int axis = 0; axis < 3; axis++) {
                    simplex[i][axis] = points[shape][reverse ? count - 1 - i : i][axis];
                }
            }
            stunnedLinearSolveCount = 0;
            CHECK(moving_closest_simplex(simplex, &count, closest));
            CHECK(count >= 1 && count <= counts[shape]);
            for (int axis = 0; axis < 3; axis++) {
                CHECK(isfinite(closest[axis]));
                NEAR(closest[axis], expected[shape][axis]);
            }
            EXPECT_EQ(stunnedLinearSolveCount, 0);
        }
    }
    return 1;
}
static int stunned_simplex_progress(void) {
    // Both coordinates grow away from B, so B is the nearest endpoint.
    // Permitting a small negative weight invents a point outside this segment.
    Vec3f segment[4] = { { 1000, 2, 0 }, { .005f, 1, 0 } }, closest;
    s32 count = 2;
    CHECK(moving_closest_simplex(segment, &count, closest));
    EXPECT_EQ(count, 1);
    CHECK(fabsf(closest[0] - .005f) < .0000001f);
    NEAR(closest[1], 1);
    // This interior projection is valid despite an extremely skinny triangle.
    Vec3f skinny[4] = { { -4000, -.001f, 1 }, { 4000, -.001f, 1 }, { 0, .001f, 1 } };
    count = 3;
    CHECK(moving_closest_simplex(skinny, &count, closest));
    CHECK(fabsf(closest[0]) < .000001f);
    CHECK(fabsf(closest[1]) < .000001f);
    NEAR(closest[2], 1);
    // The midpoint is closer even when squared float distances round equal.
    Vec3f rounded[4] = { { .0001f, 0, 1 }, { -.0001f, 0, 1 } };
    count = 2;
    CHECK(moving_closest_simplex(rounded, &count, closest));
    CHECK(fabsf(closest[0]) < .000001f);
    NEAR(closest[2], 1);
    return 1;
}
static int stunned_contact_release(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            floor_at(0);
            stunned_setup(0);
            object.oGravity = -4;
            object.oVelY = 30;
            cur_obj_move_stunned();
            NEAR(object.oPosY, 26);
            // A small overlap can remain after the native rounded wall/edge push.
            // It must not consume the downward travel needed to finish landing.
            reset();
            floor_at(0);
            wall_at(100, -500, 500, dynamic, SURFACE_DEFAULT);
            stunned_setup(80);
            object.oPosX = 56;
            object.oWallHitboxRadius = 45;
            object.oGravity = -4;
            cur_obj_move_stunned();
            NEAR(object.oPosY, 76);
            for (int frame = 0; frame < 20 && object.oPosY > 0; frame++) {
                cur_obj_move_stunned();
            }
            NEAR(object.oPosY, 0);
            CHECK(object.oMoveFlags & OBJ_MOVE_MASK_ON_GROUND);
            object.oPosY = 40;
            object.oForwardVel = 80;
            object.oMoveAngleYaw = -0x4000;
            object.oVelY = 0;
            f32 before = object.oPosX;
            cur_obj_move_stunned();
            NEAR(object.oPosX, before - 80);
        }
    }
    return 1;
}
static int stunned_wall_lifecycle(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int corner = 0; corner < 2; corner++) {
            reset();
            floor_at(0);
            wall_at(100, -500, 500, dynamic, SURFACE_DEFAULT);
            if (corner) {
                triangle((Vec3s){ -200, 0, 100 }, (Vec3s){ 200, 0, 100 }, (Vec3s){ 0, 300, 100 },
                         dynamic, SURFACE_DEFAULT);
            }
            for (int cycle = 0; cycle < 3; cycle++) {
                vec3f_set(object.location, 0, cycle == 1 ? 60 : 0, corner ? 80 : 0);
                object.oForwardVel = 80;
                object.oMoveAngleYaw = 0x4000;
                object.oVelY = 30;
                object.oGravity = 2;
                object.oWallHitboxRadius = 30;
                object.hitboxRadius = 65;
                object.hitboxHeight = 113;
                object.oGroundTwirlStunTimer = -8;
                object.oGroundTwirlStunDuration = 12;
                object.metalContactCooldown = 15;
                nativeEnemyUpdates = 0;
                int frame;
                for (frame = 0; frame < 100 && nativeEnemyUpdates == 0; frame++) {
                    object.header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
                    stunnedLinearSolveCount = 0;
                    update_object_with_ground_twirl_stun();
                    EXPECT_EQ(stunnedLinearSolveCount, 0);
                    NEAR(object.oGravity, 2);
                    NEAR(object.oWallHitboxRadius, 30);
                    if (frame < 14) {
                        EXPECT_EQ(nativeEnemyUpdates, 0);
                    }
                }
                CHECK(frame < 100);
                EXPECT_EQ(nativeEnemyUpdates, 1);
                NEAR(object.oPosY, 0);
                EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
                EXPECT_EQ(object.metalContactCooldown, 0);
                CHECK(object.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
                stunnedCastCount = 0;
                for (int idle = 0; idle < 5; idle++) {
                    update_object_with_ground_twirl_stun();
                }
                EXPECT_EQ(nativeEnemyUpdates, 6);
                EXPECT_EQ(stunnedCastCount, 0);
            }
        }
    }
    return 1;
}
static int stunned_sculpture_lifecycle(void) {
    reset();
    floor_at(0);
    for (unsigned i = 0; i < ARRAY_COUNT(vanishSculpture); i++) {
        triangle(vanishSculpture[i][0], vanishSculpture[i][1], vanishSculpture[i][2], 0,
                 SURFACE_DEFAULT);
    }
    // 156 launches toward the actual sculpture from outside its bounds. Two
    // of these stayed at timer -1 beyond 180 updates with the old wall solver.
    for (int side = 0; side < 4; side++) {
        for (int y = 50; y <= 850; y += 400) {
            for (int lateral = -900; lateral <= 900; lateral += 150) {
                memset(&object, 0, sizeof(object));
                nativeEnemyUpdates = 0;
                if (side < 2) {
                    vec3f_set(object.location, side == 0 ? -1050 : 1050, y, lateral);
                    object.oMoveAngleYaw = side == 0 ? 0x4000 : -0x4000;
                } else {
                    vec3f_set(object.location, lateral, y, side == 2 ? -1050 : 1050);
                    object.oMoveAngleYaw = side == 2 ? 0 : 0x8000;
                }
                object.oForwardVel = 80;
                object.oVelY = 30;
                object.oGravity = -4;
                object.oWallHitboxRadius = 45;
                object.hitboxRadius = 60;
                object.hitboxHeight = 113;
                object.oGroundTwirlStunTimer = -8;
                object.oGroundTwirlStunDuration = 12;
                object.metalContactCooldown = 15;
                int frame;
                for (frame = 0; frame < 90 && !nativeEnemyUpdates; frame++) {
                    update_object_with_ground_twirl_stun();
                }
                if (!nativeEnemyUpdates) {
                    fprintf(stderr,
                            "stuck launch: side=%d y=%d lateral=%d pos=%.3f,%.3f,%.3f timer=%d\n", side,
                            y, lateral, object.oPosX, object.oPosY, object.oPosZ,
                            object.oGroundTwirlStunTimer);
                }
                CHECK(frame < 90);
                EXPECT_EQ(nativeEnemyUpdates, 1);
                EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
                CHECK(object.oMoveFlags & OBJ_MOVE_MASK_ON_GROUND);
                NEAR(object.oPosY, object.oFloorHeight);
            }
        }
    }
    return 1;
}
static int stunned_court_lifecycle(void) {
    const s16 xs[] = { 1350, 1400, 1450, 1540, 1550, 1600, 1650, 2000, 2550,
                       2600, 2650, 2660, 2700, 2740, 3000, 3860, 3900, 3940,
                       4000, 5060, 5100, 5140, 6260, 6300, 6350, 6400, 6450 };
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        reset();
        for (unsigned i = 0; i < ARRAY_COUNT(enemyCourtTriangles); i++) {
            triangle(enemyCourtTriangles[i].vertices[0], enemyCourtTriangles[i].vertices[1],
                     enemyCourtTriangles[i].vertices[2], dynamic, enemyCourtTriangles[i].type);
        }
        // Koopa's 1.5-scaled native hitbox and wall radius, including fractional
        // positions, diagonal launches, ledge tops, wall seams and court corners.
        for (unsigned xi = 0; xi < ARRAY_COUNT(xs); xi++) {
            for (int y = 0; y <= 320; y += 40) {
                for (int yaw = -1; yaw <= 1; yaw++) {
                    for (int speed = 0; speed <= 80; speed += 40) {
                        memset(&object, 0, sizeof(object));
                        nativeEnemyUpdates = 0;
                        vec3f_set(object.location, xs[xi] + .3f, y, -10620.3f);
                        object.oMoveAngleYaw = 0x8000 + yaw * 0x1000;
                        object.oForwardVel = speed;
                        object.oVelY = 30;
                        object.oGravity = -4;
                        object.oWallHitboxRadius = 50;
                        object.hitboxRadius = 90;
                        object.hitboxHeight = 60;
                        object.oGroundTwirlStunTimer = -8;
                        object.oGroundTwirlStunDuration = 12;
                        object.metalContactCooldown = 15;
                        int frame;
                        for (frame = 0; frame < 100 && !nativeEnemyUpdates; frame++) {
                            object.header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
                            update_object_with_ground_twirl_stun();
                            NEAR(object.oGravity, -4);
                            NEAR(object.oWallHitboxRadius, 50);
                            if (frame < 14) {
                                EXPECT_EQ(nativeEnemyUpdates, 0);
                            }
                        }
                        if (!nativeEnemyUpdates) {
                            fprintf(stderr,
                                    "stuck court launch: dynamic=%d x=%d y=%d yaw=%d speed=%d "
                                    "pos=%.6f,%.6f,%.6f timer=%d\n",
                                    dynamic, xs[xi], y, yaw, speed, object.oPosX, object.oPosY,
                                    object.oPosZ, object.oGroundTwirlStunTimer);
                        }
                        CHECK(frame < 100);
                        EXPECT_EQ(nativeEnemyUpdates, 1);
                        EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
                        EXPECT_EQ(object.metalContactCooldown, 0);
                        CHECK(object.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
                        CHECK(object.oMoveFlags & OBJ_MOVE_MASK_ON_GROUND);
                        NEAR(object.oPosY, object.oFloorHeight);
                        stunnedCastCount = 0;
                        for (int idle = 0; idle < 5; idle++) {
                            update_object_with_ground_twirl_stun();
                        }
                        EXPECT_EQ(nativeEnemyUpdates, 6);
                        EXPECT_EQ(stunnedCastCount, 0);
                    }
                }
            }
        }
    }
    return 1;
}
static int stunned_candidate_work(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int dense = 0; dense < 2; dense++) {
            reset();
            stunned_setup(20);
            object.oForwardVel = 120;
            wall_at(40, -500, 500, dynamic, SURFACE_DEFAULT);
            if (dense) {
                for (int i = 0; i < 96; i++) {
                    wall_at(300 + i * 3, -500, 500, dynamic, SURFACE_DEFAULT);
                    triangle((Vec3s){ 40, 0, 300 + i * 3 }, (Vec3s){ 40, 0, 310 + i * 3 },
                             (Vec3s){ 40, 200, 300 + i * 3 }, dynamic, SURFACE_DEFAULT);
                }
            }
            stunnedCastCount = 0;
            cur_obj_move_stunned();
            CHECK(object.oPosX <= 30.05f);
            CHECK(object.oMoveFlags & OBJ_MOVE_HIT_WALL);
            // Two real wall faces, at most once per slide, despite four overlapping
            // cell lists and 288 distant faces at the same height in the same cell.
            EXPECT_EQ(stunnedCastCount <= 4, TRUE);
        }
    }
    // Slanted face bounds overlap the travel, but its plane is well below it.
    reset();
    stunned_setup(500);
    object.oForwardVel = 120;
    object.oVelY = 120;
    triangle((Vec3s){ -1000, -1000, -1000 }, (Vec3s){ 0, 0, 1000 }, (Vec3s){ 1000, 1000, -1000 }, 0,
             SURFACE_DEFAULT);
    stunnedCastCount = 0;
    cur_obj_move_stunned();
    NEAR(object.oPosX, 120);
    NEAR(object.oPosY, 620);
    EXPECT_EQ(stunnedCastCount, 0);
    return 1;
}
static int stunned_candidate_overflow(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            stunned_setup(20);
            object.oForwardVel = 120;
            // Insert the nearest wall first so it is encountered after more than
            // 32 candidates. A full duplicate cache must never omit that wall.
            wall_at(40, -500, 500, dynamic, SURFACE_DEFAULT);
            for (int i = 0; i < 40; i++) {
                wall_at(60 + i, -500, 500, dynamic, SURFACE_DEFAULT);
            }
            stunnedCastCount = 0;
            cur_obj_move_stunned();
            CHECK(stunnedCastCount > 32);
            CHECK(object.oPosX <= 30.05f);
            CHECK(object.oMoveFlags & OBJ_MOVE_HIT_WALL);
        }
    }
    return 1;
}
static int stunned_distant_faces(void) {
    // Sculpture faces that previously made the iterative solver uncertain and
    // stopped an unrelated slide. All four are outside the swept body bounds.
    Vec3s faces[4][3] = { { { 500, 40, -890 }, { 310, 1100, -420 }, { 430, 40, -520 } },
                          { { -310, 40, -950 }, { 310, 40, -317 }, { 310, 40, -950 } },
                          { { -280, 80, 780 }, { -280, 40, 950 }, { 280, 40, 950 } },
                          { { -220, 450, -100 }, { -220, 600, -100 }, { 330, 600, -100 } } };
    Vec3f starts[4] = { { 939.506f, 37.920f, -800 },
                        { -948.759f, 39.748f, 0 },
                        { 400, 40.030f, 550.050f },
                        { -803.904f, 442.151f, -259.991f } };
    Vec3f ends[4] = { { 885.324f, 49.960f, -800 },
                      { -948.772f, 26.217f, 1.433f },
                      { 400, 40.030f, 480 },
                      { -822.041f, 449.446f, -312.797f } };
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int i = 0; i < 4; i++) {
            reset();
            object.hitboxHeight = 113;
            object.oWallHitboxRadius = 45;
            triangle(faces[i][0], faces[i][1], faces[i][2], dynamic, SURFACE_DEFAULT);
            Vec3f target;
            vec3f_copy(target, ends[i]);
            stunnedCastCount = 0;
            EXPECT_EQ(sweep_object_motion(&object, starts[i], target, NULL, object.oWallHitboxRadius),
                      0);
            for (int axis = 0; axis < 3; axis++) {
                NEAR(target[axis], ends[i][axis]);
            }
            EXPECT_EQ(stunnedCastCount, 0);
        }
    }
    return 1;
}
static int stunned_thin_geometry(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            stunned_setup(40);
            object.oForwardVel = 80;
            object.oVelY = -60;
            triangle((Vec3s){ 48, 0, -40 }, (Vec3s){ 54, 0, 40 }, (Vec3s){ 60, 0, -40 }, dynamic,
                     SURFACE_DEFAULT);
            cur_obj_move_stunned();
            CHECK(object.oPosY >= -.05f);
            NEAR(object.oPosX, 80);
            reset();
            stunned_setup(0);
            object.oForwardVel = 80;
            object.oVelY = 60;
            triangle((Vec3s){ 20, 50, -40 }, (Vec3s){ 32, 50, -40 }, (Vec3s){ 26, 50, 40 }, dynamic,
                     SURFACE_DEFAULT);
            cur_obj_move_stunned();
            CHECK(object.oPosY <= 20.05f);
            NEAR(object.oPosX, 80);
            NEAR(object.oVelY, 0);
            reset();
            stunned_setup(0);
            object.oForwardVel = 120;
            wall_at(40, 9, 14, dynamic, SURFACE_DEFAULT);
            cur_obj_move_stunned();
            CHECK(object.oPosX <= 30.05f);
            CHECK(object.oMoveFlags & OBJ_MOVE_HIT_WALL);
        }
    }
    return 1;
}
static int stunned_water_and_filters(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        stunned_setup(20);
        floor_at(0);
        enemyWater = 30;
        object.oVelY = -60;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 0);
        CHECK(object.oMoveFlags & OBJ_MOVE_UNDERWATER_ON_GROUND);
        reset();
        stunned_setup(20);
        enemyWater = 30;
        object.oVelY = -10;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 10);
        CHECK(object.oMoveFlags & OBJ_MOVE_ENTERED_WATER);
        object.oBuoyancy = 4;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 4);
        NEAR(object.oVelY, -6);
        reset();
        stunned_setup(29);
        enemyWater = 30;
        object.oMoveFlags = OBJ_MOVE_UNDERWATER_OFF_GROUND;
        object.oVelY = 4;
        object.oBuoyancy = 1;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 30);
        NEAR(object.oVelY, 0);
        CHECK(object.oMoveFlags & OBJ_MOVE_AT_WATER_SURFACE);
        object.oVelY = 8;
        object.oGravity = -4;
        object.oBuoyancy = 100;
        cur_obj_move_stunned();
        NEAR(object.oPosY, 34);
        NEAR(object.oVelY, 4);
        CHECK(!(object.oMoveFlags & OBJ_MOVE_MASK_IN_WATER));
        // An ordinary enemy is unaffected by Mario's Vanish cap; only the
        // enemy's own grate flag bypasses its Vanish surfaces.
        for (int bypass = 0; bypass < 2; bypass++) {
            reset();
            stunned_setup(20);
            struct Object marioOnly = { 0 };
            gMarioObject = &marioOnly;
            mario.flags = MARIO_VANISH_CAP;
            object.oForwardVel = 120;
            if (bypass) {
                object.activeFlags |= ACTIVE_FLAG_MOVE_THROUGH_GRATE;
            }
            wall_at(40, -500, 500, cycle % 2, SURFACE_VANISH_CAP_WALLS);
            cur_obj_move_stunned();
            if (bypass) {
                NEAR(object.oPosX, 120);
            } else {
                CHECK(object.oPosX <= 30.05f);
            }
        }
        reset();
        stunned_setup(20);
        object.oForwardVel = 120;
        wall_at(40, -500, 500, 0, SURFACE_INTANGIBLE);
        cur_obj_move_stunned();
        NEAR(object.oPosX, 120);
    }
    reset();
    return 1;
}
#define RUN(name, fn)                                                                                  \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)

static int blocked_swim_momentum(void) {
    const u32 actions[] = { ACT_FLUTTER_KICK, ACT_HOLD_FLUTTER_KICK, ACT_WATER_SHELL_SWIMMING,
                            ACT_WATER_PLUNGE, ACT_WATER_DOWN_BURST,  ACT_WATER_IDLE };
    for (unsigned action = 0; action < ARRAY_COUNT(actions); action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            reset();
            mario.action = actions[action];
            triangle((Vec3s){ -4000, 180, -4000 }, (Vec3s){ 4000, 180, -4000 }, (Vec3s){ 0, 180, 4000 },
                     0, SURFACE_DEFAULT);
            mario.forwardVel = 64;
            vec3f_set(mario.vel, 0, 64, 0);
            EXPECT_EQ(perform_water_step(&mario), WATER_STEP_HIT_CEILING);
            NEAR(mario.forwardVel, 64);
            NEAR(mario.vel[1], 0);
            for (int frame = 0; frame < 8; frame++) {
                // Repeated thrust cannot pass the ceiling or drain stored speed.
                vec3f_set(mario.vel, 0, mario.forwardVel, 0);
                perform_water_step(&mario);
                NEAR(mario.forwardVel, 64);
                NEAR(mario.vel[1], 0);
                CHECK(mario.pos[1] <= 20.01f);
            }
            Vec3f before;
            vec3f_copy(before, mario.pos);
            vec3f_set(mario.vel, mario.forwardVel, 0, 0);
            perform_water_step(&mario);
            NEAR(mario.pos[0] - before[0], 64);
            // Currents can push into a wall while Mario is swimming away from it.
            reset();
            wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
            mario.forwardVel = 12;
            vec3f_set(mario.vel, -12, 0, 0);
            vec3f_set(current, 40, 0, 0);
            perform_water_step(&mario);
            NEAR(mario.forwardVel, 12);
            NEAR(mario.vel[0], -12);
            // An intervening ceiling blocks upward launch, but not stored swim speed.
            reset();
            mario.waterLevel = 100;
            triangle((Vec3s){ -4000, 170, -4000 }, (Vec3s){ 4000, 170, -4000 }, (Vec3s){ 0, 170, 4000 },
                     0, SURFACE_DEFAULT);
            mario.forwardVel = 64;
            vec3f_set(mario.vel, 0, 64, 0);
            perform_water_step(&mario);
            NEAR(sSwimming.surfaceExitVelocityY, 0);
            NEAR(mario.forwardVel, 64);
        }
    }
    return 1;
}
static int jrb_underside_travel(void) {
    int checked = 0;
    for (int mesh = -1; mesh < (int) ARRAY_COUNT(swimmingRockPlacements); mesh++) {
        if (mesh < 0) {
            load_jrb_sunken_ship();
        } else {
            if (swimmingRockPlacements[mesh][1] >= 0) {
                continue;
            }
            load_jrb_object_rock(mesh);
        }
        int first = (int) ARRAY_COUNT(jrbTriangles);
        for (int i = first; i < surfaceCount; i++) {
            struct Surface *s = &surfaces[i];
            if (s->normal.y >= -.05f) {
                continue;
            }
            for (int angle = 0; angle < 9; angle++) {
                for (int cycle = 0; cycle < 2; cycle++) {
                    Vec3f start = { (s->vertex1[0] + s->vertex2[0] + s->vertex3[0]) / 3.0f,
                                    (s->vertex1[1] + s->vertex2[1] + s->vertex3[1]) / 3.0f - 160.02f,
                                    (s->vertex1[2] + s->vertex2[2] + s->vertex3[2]) / 3.0f };
                    struct Surface *floor;
                    float height;
                    vec3f_copy(mario.pos, start);
                    if (!water_position_has_clearance(mario.pos, &floor, &height, NULL)) {
                        continue;
                    }
                    Vec3f movement = { angle == 8 ? 0 : 32 * sins(angle * 0x2000), 32,
                                       angle == 8 ? 0 : 32 * coss(angle * 0x2000) };
                    for (int frame = 0; frame < 16; frame++) {
                        resolve_mario_geometry_walls(&mario);
                        u32 result = mario_water_collision_step(&mario, movement, NULL);
                        if (result == WATER_STEP_CANCELLED) {
                            fprintf(stderr, "underside mesh=%d triangle=%d angle=%d frame=%d\n", mesh,
                                    i, angle, frame);
                            return 0;
                        }
                    }
                    // The separate overlap cases cover partially embedded escape.
                    // Once clear, steering away from sustained contact stays available.
                    if (water_position_has_clearance(mario.pos, &floor, &height, NULL)) {
                        CHECK(mario_water_collision_step(&mario, (Vec3f){ 0, -32, 0 }, NULL)
                              != WATER_STEP_CANCELLED);
                        CHECK(water_position_has_clearance(mario.pos, &floor, &height, NULL));
                    }
                    checked++;
                }
            }
        }
    }
    EXPECT_EQ(checked, 450);
    return 1;
}

static int closed_swimming_corner(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        floor_at(0);
        // Sloped roof and seabed form a crease along Z; the third face closes it.
        triangle((Vec3s){ -4000, 4164, -4000 }, (Vec3s){ 4000, -3836, -4000 }, (Vec3s){ 0, 164, 4000 },
                 1, SURFACE_DEFAULT);
        triangle((Vec3s){ -4000, -500, -3992 }, (Vec3s){ 0, 500, 8 }, (Vec3s){ 4000, -500, 4008 }, 1,
                 SURFACE_DEFAULT);
        vec3f_set(mario.pos, 0, 1, 0);
        Vec3f target = { 40, -9, 40 };
        Vec3f velocity = { 40, -10, 40 };
        EXPECT_EQ(sweep_water_movement(&mario, mario.pos, target, velocity, TRUE, 10, NULL, NULL),
                  WATER_STEP_HIT_FLOOR);
        CHECK(target[0] <= 4.01f);
        CHECK(target[1] >= -0.01f);
        CHECK(target[2] <= 12.01f);
        NEAR(vec3f_dot(velocity, velocity), 0);
        struct Surface *floor;
        f32 height;
        CHECK(water_position_has_clearance(target, &floor, &height, NULL));
        Vec3f start;
        vec3f_copy(start, target);
        target[0] -= 40;
        target[1] += 10;
        target[2] -= 40;
        EXPECT_EQ(sweep_water_movement(&mario, start, target, NULL, TRUE, 10, NULL, NULL),
                  WATER_STEP_NONE);
        CHECK(target[0] < start[0] - 39);
        CHECK(target[1] > start[1] + 9);
        CHECK(target[2] < start[2] - 39);
    }
    return 1;
}

static int jrb_ship_contact_work(void) {
    for (int pitch = -2; pitch <= 0; pitch++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            load_jrb_sunken_ship();
            vec3f_set(mario.pos, 4395, -5118, 2677);
            reanchor_ship_fixture(mario.pos);
            mario.faceAngle[1] = 0x2C59;
            mario.faceAngle[0] = pitch * 0x1000;
            Vec3f movement = {
                49.8f * coss(mario.faceAngle[0]) * sins(mario.faceAngle[1]),
                49.8f * sins(mario.faceAngle[0]),
                49.8f * coss(mario.faceAngle[0]) * coss(mario.faceAngle[1]),
            };
            for (int frame = 0; frame < 16; frame++) {
                Vec3f start, velocity;
                vec3f_copy(start, mario.pos);
                vec3f_copy(velocity, movement);
                waterRaycastCount = waterEdgePlaneCount = 0;
                resolve_mario_geometry_walls(&mario);
                EXPECT_EQ(mario_water_collision_step(&mario, movement, velocity), WATER_STEP_HIT_FLOOR);
                CHECK(waterRaycastCount <= 36);
                // Full body-path coverage also checks faces between clear endpoints.
                CHECK(waterEdgePlaneCount <= 180);
                CHECK(mario.pos[2] - start[2] > 6.0f);
                CHECK(mario.pos[1] >= -5119.01f);
                CHECK(velocity[1] >= -0.0001f);
                struct Surface *floor;
                f32 height;
                CHECK(water_position_has_clearance(mario.pos, &floor, &height, NULL));
            }
            // Constraints expire with the sweep: turning away must move freely.
            Vec3f start;
            vec3f_copy(start, mario.pos);
            CHECK(mario_water_collision_step(&mario, (Vec3f){ -40, 20, 0 }, NULL)
                  != WATER_STEP_CANCELLED);
            CHECK(mario.pos[0] < start[0] - 30);
            CHECK(mario.pos[1] > start[1] + 15);
        }
    }
    return 1;
}

// Match production cell membership instead of the fixture's deliberate all-cell
// insertion, so operation counts describe the local ship/terrain candidate lists.
static void use_native_surface_cells(void) {
#if DYNAMIC_SURFACE_INDEX
    invalidate_dynamic_surface_index();
#endif
    for (s32 z = 0; z < NUM_CELLS; z++) {
        for (s32 x = 0; x < NUM_CELLS; x++) {
            for (s32 kind = 0; kind < NUM_SPATIAL_PARTITIONS; kind++) {
                for (s32 dynamic = 0; dynamic < 2; dynamic++) {
                    struct SurfaceNode **node = dynamic ? &gDynamicSurfacePartition[z][x][kind]
                                                        : &gStaticSurfacePartition[z][x][kind];
                    while (*node != NULL) {
                        struct Surface *s = (*node)->surface;
                        s32 minX = MIN(s->vertex1[0], MIN(s->vertex2[0], s->vertex3[0]));
                        s32 maxX = MAX(s->vertex1[0], MAX(s->vertex2[0], s->vertex3[0]));
                        s32 minZ = MIN(s->vertex1[2], MIN(s->vertex2[2], s->vertex3[2]));
                        s32 maxZ = MAX(s->vertex1[2], MAX(s->vertex2[2], s->vertex3[2]));
                        if (x < lower_cell_index(minX) || x > upper_cell_index(maxX)
                            || z < lower_cell_index(minZ) || z > upper_cell_index(maxZ)) {
                            *node = (*node)->next;
                        } else {
                            node = &(*node)->next;
                        }
                    }
                }
            }
        }
    }
#if STATIC_SURFACE_INDEX
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    assert(gStaticSurfaceIndex.offsets != NULL);
#endif
}

#if STATIC_SURFACE_INDEX
static int static_index_lifecycle(void) {
    reset();
    floor_at(0);
    floor_at(40);
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticSurfaceIndex.offsets != NULL);
    struct Surface *hit;
    NEAR(find_floor(0, 80, 0, &hit), 40);
    EXPECT_EQ(hit, &surfaces[surfaceCount - 1]);
    floor_at(80);
    gStaticSurfaceGeneration++;
    // A live list addition must supersede the stale packed list and cached result.
    NEAR(find_floor(0, 80, 0, &hit), 80);
    EXPECT_EQ(hit, &surfaces[surfaceCount - 1]);
    // Reuse the same index allocation for the next area's different geometry.
    reset();
    floor_at(100);
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticSurfaceIndex.offsets != NULL);
    NEAR(find_floor(0, 120, 0, &hit), 100);
    EXPECT_EQ(hit, &surfaces[surfaceCount - 1]);
    return TRUE;
}

static int indexed_collision_equivalence(void) {
    load_jrb_sunken_ship();
    use_native_surface_cells();
    u32 *offsets = gStaticSurfaceIndex.offsets;
    u32 references = offsets[NUM_CELLS * NUM_CELLS * NUM_SPATIAL_PARTITIONS];
    printf("INDEX JRB: %u references; %u target pool bytes plus 16 global bytes\n", references,
           (u32) ((NUM_CELLS * NUM_CELLS * NUM_SPATIAL_PARTITIONS + 1) * 4 + references * 4
                  + ((references + STATIC_SURFACE_GROUP_SIZE - 1) / STATIC_SURFACE_GROUP_SIZE)
                        * sizeof(struct SurfaceQueryBounds)));
    const s16 flags[] = { 0,
                          COLLISION_FLAG_CAMERA,
                          COLLISION_FLAG_RETURN_FIRST,
                          COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_INCLUDE_INTANGIBLE,
                          COLLISION_FLAG_CAMERA | COLLISION_FLAG_RETURN_FIRST
                              | COLLISION_FLAG_EXCLUDE_DYNAMIC };
    for (s32 i = 0; i < surfaceCount; i++) {
        struct Surface *surface = &surfaces[i];
        f32 x = (surface->vertex1[0] + surface->vertex2[0] + surface->vertex3[0]) / 3.0f;
        f32 y = (surface->vertex1[1] + surface->vertex2[1] + surface->vertex3[1]) / 3.0f;
        f32 z = (surface->vertex1[2] + surface->vertex2[2] + surface->vertex3[2]) / 3.0f;
        for (u32 flag = 0; flag < ARRAY_COUNT(flags); flag++) {
            struct Surface *floor[4], *ceil[4];
            f32 floorHeight[4], ceilHeight[4];
            s16 floorFlags[4], ceilFlags[4], wallFlags[2];
            s32 wallHits[2];
            struct WallCollisionData walls[2] = { 0 };
            for (s32 packed = 0; packed < 2; packed++) {
                gStaticSurfaceIndex.offsets = packed ? offsets : NULL;
                gCollisionFlags = flags[flag];
                floorHeight[packed] = find_floor(x, y + 200, z, &floor[packed]);
                floorFlags[packed] = gCollisionFlags;
                gCollisionFlags = flags[flag];
                ceilHeight[packed] = find_ceil(x, y - 200, z, &ceil[packed]);
                ceilFlags[packed] = gCollisionFlags;
                gCollisionFlags = flags[flag];
                floorHeight[packed + 2] = find_floor_precise(x, y + 200, z, &floor[packed + 2]);
                floorFlags[packed + 2] = gCollisionFlags;
                gCollisionFlags = flags[flag];
                ceilHeight[packed + 2] = find_ceil_precise(x, y - 200, z, &ceil[packed + 2]);
                ceilFlags[packed + 2] = gCollisionFlags;
                walls[packed].x = x + surface->normal.x * 20;
                walls[packed].y = y - 30;
                walls[packed].z = z + surface->normal.z * 20;
                walls[packed].offsetY = 30;
                walls[packed].radius = 50;
                gCollisionFlags = flags[flag];
                wallHits[packed] = find_wall_collisions(&walls[packed]);
                wallFlags[packed] = gCollisionFlags;
            }
            for (s32 precise = 0; precise <= 2; precise += 2) {
                EXPECT_EQ(floor[precise], floor[precise + 1]);
                EXPECT_EQ(ceil[precise], ceil[precise + 1]);
                CHECK(memcmp(&floorHeight[precise], &floorHeight[precise + 1], sizeof(f32)) == 0);
                CHECK(memcmp(&ceilHeight[precise], &ceilHeight[precise + 1], sizeof(f32)) == 0);
                EXPECT_EQ(floorFlags[precise], floorFlags[precise + 1]);
                EXPECT_EQ(ceilFlags[precise], ceilFlags[precise + 1]);
            }
            EXPECT_EQ(wallFlags[0], wallFlags[1]);
            EXPECT_EQ(wallHits[0], wallHits[1]);
            CHECK(memcmp(&walls[0].x, &walls[1].x, sizeof(f32)) == 0);
            CHECK(memcmp(&walls[0].z, &walls[1].z, sizeof(f32)) == 0);
            EXPECT_EQ(walls[0].numWalls, walls[1].numWalls);
            for (s32 wall = 0; wall < walls[0].numWalls; wall++) {
                EXPECT_EQ(walls[0].walls[wall], walls[1].walls[wall]);
            }
        }
    }
    gStaticSurfaceIndex.offsets = offsets;
    return TRUE;
}

enum JrbScanReference {
    JRB_LINKED_SCANS,
    JRB_UNGROUPED_SCANS,
    JRB_UNGROUPED_EDGE_SCANS,
    JRB_UNINDEXED_DYNAMIC_SCANS
};

static int compare_jrb_indexed_travel(enum JrbScanReference reference) {
    const Vec3f starts[] = { { 4395, -5118, 2677 }, { 4330, -5118, 2912 }, { 4990, -5119, 3929 } };
    const s16 yaws[] = { 0x2C59, 0x3A24, (s16) 0x9E79 };
    const f32 speeds[] = { 41.9f, 53.3f };
    u32 updates = 0, referenceRays = 0, filteredRays = 0;
    u32 referenceVertical = 0, filteredVertical = 0;
    u32 referenceEdges = 0, filteredEdges = 0;
    u32 builds = 0, packedReferences = 0, peakIndexBytes = 0;
    for (u32 placement = 0; placement < ARRAY_COUNT(starts); placement++) {
        for (s32 pitch = -3; pitch <= 0; pitch++) {
            for (u32 speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
                load_jrb_sunken_ship();
                use_native_surface_cells();
                u32 *offsets = gStaticSurfaceIndex.offsets;
                vec3f_copy(mario.pos, starts[placement]);
                for (s32 frame = 0; frame < 20; frame++) {
                    Vec3f movement = { speeds[speed] * coss(pitch * 0x1000) * sins(yaws[placement]),
                                       speeds[speed] * sins(pitch * 0x1000),
                                       speeds[speed] * coss(pitch * 0x1000) * coss(yaws[placement]) };
                    if (frame >= 16) {
                        vec3f_set(movement, -40, 20, 0);
                    }
                    struct MarioState before = mario;
                    s16 beforeFlags = gCollisionFlags;
                    Vec3f velocity, linkedVelocity;
                    vec3f_copy(velocity, movement);
                    gStaticSurfaceIndex.offsets = offsets;
                    bypassDynamicIndex = reference != JRB_UNINDEXED_DYNAMIC_SCANS;
#if DYNAMIC_SURFACE_INDEX
                    invalidate_dynamic_surface_index();
#endif
                    dynamicIndexBuildCount = dynamicIndexReferenceCount = 0;
                    waterRayNodeCount = waterVerticalNodeCount = waterEdgeNodeCount = 0;
                    resolve_mario_geometry_walls(&mario);
                    u32 result = mario_water_collision_step(&mario, movement, velocity);
                    filteredRays += waterRayNodeCount;
                    filteredVertical += waterVerticalNodeCount;
                    filteredEdges += waterEdgeNodeCount;
                    builds += dynamicIndexBuildCount;
                    packedReferences += dynamicIndexReferenceCount;
#if DYNAMIC_SURFACE_INDEX
                    peakIndexBytes = MAX(peakIndexBytes, gDynamicSurfaceIndex.used);
#endif
                    struct MarioState packed = mario;
                    s16 packedFlags = gCollisionFlags;
                    mario = before;
                    gCollisionFlags = beforeFlags;
                    vec3f_copy(linkedVelocity, movement);
                    bypassStaticGroups =
                        reference == JRB_LINKED_SCANS || reference == JRB_UNGROUPED_SCANS;
                    bypassDynamicIndex = TRUE;
#if DYNAMIC_SURFACE_INDEX
                    invalidate_dynamic_surface_index();
#endif
                    bypassWaterEdgeGroups = reference == JRB_UNGROUPED_EDGE_SCANS;
                    gStaticSurfaceIndex.offsets = reference == JRB_LINKED_SCANS ? NULL : offsets;
                    waterRayNodeCount = waterVerticalNodeCount = waterEdgeNodeCount = 0;
                    resolve_mario_geometry_walls(&mario);
                    EXPECT_EQ(mario_water_collision_step(&mario, movement, linkedVelocity), result);
                    referenceRays += waterRayNodeCount;
                    referenceVertical += waterVerticalNodeCount;
                    referenceEdges += waterEdgeNodeCount;
                    bypassStaticGroups = FALSE;
                    bypassWaterEdgeGroups = FALSE;
                    gStaticSurfaceIndex.offsets = offsets;
                    CHECK(memcmp(mario.pos, packed.pos, sizeof(Vec3f)) == 0);
                    CHECK(memcmp(linkedVelocity, velocity, sizeof(Vec3f)) == 0);
                    CHECK(memcmp(&mario.floorHeight, &packed.floorHeight, sizeof(f32)) == 0);
                    EXPECT_EQ(mario.floor, packed.floor);
                    EXPECT_EQ(gCollisionFlags, packedFlags);
                    updates++;
                }
                gStaticSurfaceIndex.offsets = offsets;
            }
        }
    }
    printf("%s JRB: %u paired contact/escape updates matched exactly\n",
           reference == JRB_LINKED_SCANS ? "INDEX" : "GROUPS", updates);
    printf("WORK groups: rays %u -> %u; vertical %u -> %u candidates\n", referenceRays, filteredRays,
           referenceVertical, filteredVertical);
    printf("WORK edge groups: %u -> %u candidates\n", referenceEdges, filteredEdges);
    if (reference == JRB_UNGROUPED_EDGE_SCANS) {
        EXPECT_EQ(filteredRays, referenceRays);
        EXPECT_EQ(filteredVertical, referenceVertical);
    } else {
        CHECK(filteredRays < referenceRays);
        CHECK(filteredVertical < referenceVertical);
    }
    CHECK(filteredEdges < referenceEdges);
    if (reference == JRB_UNINDEXED_DYNAMIC_SCANS) {
        CHECK(builds > 0);
        printf("WORK dynamic index: %u cell builds, %u packed references, %u peak host arena bytes\n",
               builds, packedReferences, peakIndexBytes);
    }
    return TRUE;
}
static int jrb_indexed_travel(void) {
    return compare_jrb_indexed_travel(JRB_LINKED_SCANS);
}

static int jrb_grouped_travel(void) {
    return compare_jrb_indexed_travel(JRB_UNGROUPED_SCANS);
}

#if DYNAMIC_SURFACE_INDEX
static int jrb_dynamic_index_travel(void) {
    return compare_jrb_indexed_travel(JRB_UNINDEXED_DYNAMIC_SCANS);
}
#endif

#endif

static int jrb_ship_contact_reuse(void) {
    const Vec3f starts[] = { { 4395, -5118, 2677 }, { 4330, -5118, 2912 } };
    const s16 yaws[] = { 0x2C59, 0x3A24 };
    u32 reusedRays = 0, repeatedRays = 0;
    for (u32 placement = 0; placement < ARRAY_COUNT(starts); placement++) {
        for (s32 pitch = -3; pitch <= 0; pitch++) {
            load_jrb_sunken_ship();
            use_native_surface_cells();
            vec3f_copy(mario.pos, starts[placement]);
            reanchor_ship_fixture(mario.pos);
            mario.forwardVel = 53.3f;
            for (s32 frame = 0; frame < 20; frame++) {
                Vec3f movement = { 53.3f * coss(pitch * 0x1000) * sins(yaws[placement]),
                                   53.3f * sins(pitch * 0x1000),
                                   53.3f * coss(pitch * 0x1000) * coss(yaws[placement]) };
                if (frame >= 16) {
                    vec3f_set(movement, -40, 20, 0);
                }
                struct MarioState before = mario;
                s16 beforeFlags = gCollisionFlags;
                Vec3f velocity, referenceVelocity;
                vec3f_copy(velocity, movement);
                bypassWaterContacts = FALSE;
                waterRaycastCount = 0;
                resolve_mario_geometry_walls(&mario);
                u32 result = mario_water_collision_step(&mario, movement, velocity);
                struct MarioState optimized = mario;
                s16 optimizedFlags = gCollisionFlags;
                reusedRays += waterRaycastCount;
                mario = before;
                gCollisionFlags = beforeFlags;
                vec3f_copy(referenceVelocity, movement);
                bypassWaterContacts = TRUE;
                waterRaycastCount = 0;
                resolve_mario_geometry_walls(&mario);
                u32 reference = mario_water_collision_step(&mario, movement, referenceVelocity);
                repeatedRays += waterRaycastCount;
                EXPECT_EQ(result, reference);
                EXPECT_EQ(optimized.floor, mario.floor);
                EXPECT_EQ(optimizedFlags, gCollisionFlags);
                NEAR(optimized.forwardVel, 53.3f);
                for (s32 axis = 0; axis < 3; axis++) {
                    // Reusing a touching plane skips only its tiny skin gap.
                    CHECK(absf(optimized.pos[axis] - mario.pos[axis]) < 0.03f);
                    CHECK(absf(velocity[axis] - referenceVelocity[axis]) < 0.001f);
                }
                if (frame < 16) {
                    EXPECT_EQ(result, WATER_STEP_HIT_FLOOR);
                    CHECK(optimized.pos[1] <= before.pos[1] + 0.02f);
                } else {
                    CHECK(optimized.pos[0] < before.pos[0] - 30);
                    CHECK(optimized.pos[1] > before.pos[1] + 15);
                }
                mario = optimized;
                gCollisionFlags = optimizedFlags;
                struct Surface *floor;
                f32 height;
                CHECK(water_position_has_clearance(mario.pos, &floor, &height, NULL));
            }
        }
    }
    bypassWaterContacts = FALSE;
    printf("WORK ship contact reuse: %u -> %u raycasts\n", repeatedRays, reusedRays);
    CHECK(reusedRays * 4 < repeatedRays * 3);
    return TRUE;
}

static int contact_reuse_finite_edges(void) {
    for (s32 obstacle = 0; obstacle < 2; obstacle++) {
        for (s32 cycle = 0; cycle < 3; cycle++) {
            reset();
            floor_at(-1000);
            triangle((Vec3s){ -40, 0, -100 }, (Vec3s){ -40, 0, 100 }, (Vec3s){ 40, 0, 0 }, cycle % 2,
                     SURFACE_DEFAULT);
            if (obstacle) {
                wall_at(55, -1000, 1000, cycle % 2, SURFACE_DEFAULT);
            }
            vec3f_set(mario.pos, -32, 8, 0);
            mario.forwardVel = 96;
            Vec3f movement = { 96, -32, 0 }, velocity;
            vec3f_copy(velocity, movement);
            CHECK(mario_water_collision_step(&mario, movement, velocity) != WATER_STEP_CANCELLED);
            CHECK(mario.pos[1] < -2);
            if (obstacle) {
                CHECK(mario.pos[0] <= 55.02f);
            } else {
                CHECK(mario.pos[0] > 60);
            }
            // The next call has no retained contacts, including after reversal.
            Vec3f before;
            vec3f_copy(before, mario.pos);
            CHECK(mario_water_collision_step(&mario, (Vec3f){ -48, 48, 0 }, NULL)
                  != WATER_STEP_CANCELLED);
            CHECK(mario.pos[0] < before[0] - 40);
            CHECK(mario.pos[1] > before[1] + 40);
            NEAR(mario.forwardVel, 96);
        }
    }
    // Leave a sloped face before reaching its plane. Reusing the infinite
    // slope would invent an upward slide after its finite tip has ended.
    reset();
    floor_at(-1000);
    struct Surface *slope = triangle((Vec3s){ -40, -40, -100 }, (Vec3s){ -40, -40, 100 },
                                     (Vec3s){ 40, 40, 0 }, 0, SURFACE_DEFAULT);
    struct WaterContactState contacts = { { slope }, { WATER_STEP_HIT_FLOOR }, 1 };
    Vec3f start = { 39.99999f, 40.02f, 0 }, target = { 47.99999f, 40.02f, 0 };
    EXPECT_EQ(sweep_water_movement(&mario, start, target, NULL, TRUE, 10, &contacts, NULL),
              WATER_STEP_NONE);
    NEAR(target[0], 47.99999f);
    NEAR(target[1], 40.02f);
    return TRUE;
}

static int jrb_narrow_gap_query_work(void) {
    const f32 speeds[] = { 41.9f, 53.3f, 64 };
    u32 cachedNodes = 0, repeatedNodes = 0;
    for (u32 speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
        for (s32 pitch = -3; pitch <= 0; pitch++) {
            load_jrb_sunken_ship();
            use_native_surface_cells();
            vec3f_set(mario.pos, 4990, -5119, 3929);
            mario.forwardVel = speeds[speed];
            for (s32 frame = 0; frame < 20; frame++) {
                Vec3f movement = { speeds[speed] * coss(pitch * 0x1000) * sins(0x9E79),
                                   speeds[speed] * sins(pitch * 0x1000),
                                   speeds[speed] * coss(pitch * 0x1000) * coss(0x9E79) };
                if (frame >= 16) {
                    vec3f_set(movement, 40, 20, 0);
                }
                struct MarioState before = mario;
                s16 beforeFlags = gCollisionFlags;
                Vec3f velocity, referenceVelocity;
                vec3f_copy(velocity, movement);
                bypassWaterQueries = FALSE;
                waterVerticalNodeCount = collisionQueryCount = 0;
                int misses = gNumFindFloorMisses;
                resolve_mario_geometry_walls(&mario);
                u32 result = mario_water_collision_step(&mario, movement, velocity);
                struct MarioState optimized = mario;
                s16 optimizedFlags = gCollisionFlags;
                u32 optimizedQueries = collisionQueryCount;
                int optimizedMisses = gNumFindFloorMisses - misses;
                cachedNodes += waterVerticalNodeCount;
                mario = before;
                gCollisionFlags = beforeFlags;
                vec3f_copy(referenceVelocity, movement);
                bypassWaterQueries = TRUE;
                waterVerticalNodeCount = collisionQueryCount = 0;
                misses = gNumFindFloorMisses;
                resolve_mario_geometry_walls(&mario);
                u32 reference = mario_water_collision_step(&mario, movement, referenceVelocity);
                repeatedNodes += waterVerticalNodeCount;
                EXPECT_EQ(result, reference);
                EXPECT_EQ(optimized.floor, mario.floor);
                EXPECT_EQ(optimizedFlags, gCollisionFlags);
                EXPECT_EQ(optimizedQueries, collisionQueryCount);
                EXPECT_EQ(optimizedMisses, gNumFindFloorMisses - misses);
                CHECK(memcmp(optimized.pos, mario.pos, sizeof(Vec3f)) == 0);
                CHECK(memcmp(velocity, referenceVelocity, sizeof(Vec3f)) == 0);
                CHECK(memcmp(&optimized.floorHeight, &mario.floorHeight, sizeof(f32)) == 0);
                NEAR(optimized.forwardVel, speeds[speed]);
                if (frame >= 16) {
                    CHECK(optimized.pos[0] > before.pos[0] + 30);
                    CHECK(optimized.pos[1] > before.pos[1] + 15);
                }
                mario = optimized;
                gCollisionFlags = optimizedFlags;
            }
        }
    }
    bypassWaterQueries = FALSE;
    printf("WORK narrow ship gap: %u -> %u vertical node visits\n", repeatedNodes, cachedNodes);
    // This floor/ceiling gap produces no wall correction. The adaptive cache
    // stays inactive here; angled-wall cases separately require query reuse.
    EXPECT_EQ(cachedNodes, repeatedNodes);
    return TRUE;
}

// Preserve the reported diagonal-to-corner travel while avoiding duplicate
// vertical searches. These four faces are BBH basement faces 428 through 431.
static int angled_wall_query_reuse(s16 level) {
    reset();
    gCurrLevelNum = level;
    triangle((Vec3s){ 2150, -2457, 1126 }, (Vec3s){ 2150, -2661, -613 }, (Vec3s){ 2150, -2457, -613 },
             0, SURFACE_DEFAULT);
    triangle((Vec3s){ 2150, -2457, 1126 }, (Vec3s){ 2150, -2969, 1126 }, (Vec3s){ 2150, -2661, -613 },
             0, SURFACE_DEFAULT);
    triangle((Vec3s){ 1638, -2457, 1638 }, (Vec3s){ 1638, -2969, 1638 }, (Vec3s){ 2150, -2969, 1126 },
             0, SURFACE_DEFAULT);
    triangle((Vec3s){ 1638, -2457, 1638 }, (Vec3s){ 2150, -2969, 1126 }, (Vec3s){ 2150, -2457, 1126 },
             0, SURFACE_DEFAULT);
    triangle((Vec3s){ 1500, -3000, 0 }, (Vec3s){ 1500, -3000, 2000 }, (Vec3s){ 3000, -3000, 0 }, 0,
             SURFACE_DEFAULT);
    triangle((Vec3s){ 3000, -3000, 0 }, (Vec3s){ 1500, -3000, 2000 }, (Vec3s){ 3000, -3000, 2000 }, 0,
             SURFACE_DEFAULT);
    mario.waterLevel = -2508;
    vec3f_set(mario.pos, 2177, -2588, 1254);
    u32 cachedNodes = 0, repeatedNodes = 0;
    for (s32 frame = 0; frame < 40; frame++) {
        // Repeat the previous query positions with a newly loaded moving floor.
        if (frame == 1) {
            vec3f_set(mario.pos, 2177, -2588, 1254);
            triangle((Vec3s){ 1500, -2700, 0 }, (Vec3s){ 1500, -2700, 2000 },
                     (Vec3s){ 3000, -2700, 2000 }, 1, SURFACE_DEFAULT);
        }
        Vec3f movement = { 50.6f * sins((s16) 0x9990), 0, 50.6f * coss((s16) 0x9990) };
        Vec3f velocity, referenceVelocity;
        struct MarioState before = mario;
        s16 beforeFlags = gCollisionFlags;
        vec3f_copy(velocity, movement);
        bypassWaterQueries = FALSE;
        waterVerticalNodeCount = collisionQueryCount = collisionTimedQueries = 0;
        int misses = gNumFindFloorMisses;
        u32 result = mario_water_collision_step(&mario, movement, velocity);
        struct MarioState optimized = mario;
        s16 optimizedFlags = gCollisionFlags;
        u32 queries = collisionQueryCount, timedQueries = collisionTimedQueries;
        int expectedMisses = gNumFindFloorMisses - misses;
        cachedNodes += waterVerticalNodeCount;
        mario = before;
        gCollisionFlags = beforeFlags;
        vec3f_copy(referenceVelocity, movement);
        bypassWaterQueries = TRUE;
        waterVerticalNodeCount = collisionQueryCount = collisionTimedQueries = 0;
        misses = gNumFindFloorMisses;
        EXPECT_EQ(mario_water_collision_step(&mario, movement, referenceVelocity), result);
        repeatedNodes += waterVerticalNodeCount;
        CHECK(memcmp(optimized.pos, mario.pos, sizeof(Vec3f)) == 0);
        CHECK(memcmp(velocity, referenceVelocity, sizeof(Vec3f)) == 0);
        EXPECT_EQ(optimized.floor, mario.floor);
        EXPECT_EQ(optimized.ceil, mario.ceil);
        CHECK(memcmp(&optimized.floorHeight, &mario.floorHeight, sizeof(f32)) == 0);
        CHECK(memcmp(&optimized.ceilHeight, &mario.ceilHeight, sizeof(f32)) == 0);
        EXPECT_EQ(gCollisionFlags, optimizedFlags);
        EXPECT_EQ(collisionQueryCount, queries);
        EXPECT_EQ(collisionTimedQueries, timedQueries);
        EXPECT_EQ(gNumFindFloorMisses - misses, expectedMisses);
        mario = optimized;
        gCollisionFlags = optimizedFlags;
    }
    bypassWaterQueries = FALSE;
    printf("WORK angled wall (level %d): %u -> %u vertical node visits\n", level, repeatedNodes,
           cachedNodes);
    CHECK(cachedNodes < repeatedNodes);
    CHECK(mario.pos[0] > 2240);
    CHECK(mario.pos[2] < 1126);
    return TRUE;
}

static int bbh_angled_wall_query_reuse(void) {
    return angled_wall_query_reuse(LEVEL_BBH);
}

static int shared_angled_wall_query_reuse(void) {
    s16 previousLevel = gCurrLevelNum;
    int result = angled_wall_query_reuse(0);
    gCurrLevelNum = previousLevel;
    return result;
}

static int precise_query_cache_contract(void) {
    reset();
    floor_at(0);
    triangle((Vec3s){ -800, 80, -800 }, (Vec3s){ 0, 80, 800 }, (Vec3s){ 800, 80, -800 }, 1,
             SURFACE_VANISH_CAP_WALLS);
    triangle((Vec3s){ -800, 200, -800 }, (Vec3s){ 800, 200, -800 }, (Vec3s){ 0, 200, 800 }, 1,
             SURFACE_DEFAULT);
    surfaces[surfaceCount - 1].flags |= SURFACE_FLAG_NO_CAM_COLLISION;
    floor_at(100);
    surfaces[surfaceCount - 1].type = SURFACE_INTANGIBLE;
    const s16 flags[] = { 0,
                          COLLISION_FLAG_CAMERA,
                          COLLISION_FLAG_RETURN_FIRST,
                          COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_INCLUDE_INTANGIBLE,
                          COLLISION_FLAG_CAMERA | COLLISION_FLAG_RETURN_FIRST
                              | COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE };
    const Vec3f positions[] = { { 0.9f, 300, -0.9f },
                                { 0, -40, 0 },
                                { 0, -2000, 0 },
                                { 799.9f, 50.9f, -799.9f },
                                { -799.9f, 100, -799.9f },
                                { 810, 300, 810 },
                                { 0, 0, 0 },
                                { -0.0f, 0, -0.0f },
                                { LEVEL_BOUNDARY_MAX + 1, 300, 0 } };
    for (s32 eligibility = 0; eligibility < 3; eligibility++) {
        mario.flags = eligibility == 1 ? MARIO_VANISH_CAP : 0;
        object.activeFlags = eligibility == 2 ? ACTIVE_FLAG_MOVE_THROUGH_GRATE : 0;
        struct SurfaceQueryCache cache = { .count = 0, .next = 0 };
        for (u32 point = 0; point < ARRAY_COUNT(positions); point++) {
            for (u32 flag = 0; flag < ARRAY_COUNT(flags); flag++) {
                for (s32 ceiling = 0; ceiling < 2; ceiling++) {
                    struct Surface *expected, *observed;
                    gCollisionFlags = flags[flag];
                    int misses = gNumFindFloorMisses;
                    f32 expectedHeight =
                        ceiling ? find_ceil_precise(positions[point][0], positions[point][1],
                                                    positions[point][2], &expected)
                                : find_floor_precise(positions[point][0], positions[point][1],
                                                     positions[point][2], &expected);
                    int expectedMisses = gNumFindFloorMisses - misses;
                    s16 expectedFlags = gCollisionFlags;
                    for (s32 repeat = 0; repeat < 2; repeat++) {
                        gCollisionFlags = flags[flag];
                        misses = gNumFindFloorMisses;
                        u32 queries = collisionQueryCount, timedQueries = collisionTimedQueries;
                        f32 height =
                            ceiling
                                ? find_ceil_precise_cached(positions[point][0], positions[point][1],
                                                           positions[point][2], &observed, &cache)
                                : find_floor_precise_cached(positions[point][0], positions[point][1],
                                                            positions[point][2], &observed, &cache);
                        CHECK(memcmp(&height, &expectedHeight, sizeof(f32)) == 0);
                        EXPECT_EQ(observed, expected);
                        EXPECT_EQ(gCollisionFlags, expectedFlags);
                        EXPECT_EQ(gNumFindFloorMisses - misses, expectedMisses);
                        EXPECT_EQ(collisionQueryCount - queries, 1);
                        EXPECT_EQ(collisionTimedQueries - timedQueries, 1);
                    }
                }
            }
        }
    }
    return TRUE;
}

static int jrb_ship_ray_bounds(void) {
    const f32 speeds[] = { 24, 49.8f, 64 };
    u32 optimizedTotal = 0, referenceTotal = 0;
    for (s32 pitch = -4; pitch <= 0; pitch++) {
        for (u32 speed = 0; speed < ARRAY_COUNT(speeds); speed++) {
            load_jrb_sunken_ship();
            use_native_surface_cells();
            vec3f_set(mario.pos, 4395, -5118, 2677);
            reanchor_ship_fixture(mario.pos);
            for (s32 frame = 0; frame < 20; frame++) {
                f32 strokeSpeed = speeds[speed] + (frame % 4) * 0.5f;
                Vec3f movement = { strokeSpeed * coss(pitch * 0x1000) * sins(0x2C59),
                                   strokeSpeed * sins(pitch * 0x1000),
                                   strokeSpeed * coss(pitch * 0x1000) * coss(0x2C59) };
                if (frame >= 16) {
                    vec3f_set(movement, -40, 20, 0);
                }
                struct MarioState before = mario;
                s16 beforeFlags = gCollisionFlags;
                Vec3f velocity, referenceVelocity;
                vec3f_copy(velocity, movement);
                bypassWaterRayBounds = FALSE;
                waterRayTriangleCount = 0;
                resolve_mario_geometry_walls(&mario);
                u32 result = mario_water_collision_step(&mario, movement, velocity);
                struct MarioState optimized = mario;
                s16 optimizedFlags = gCollisionFlags;
                optimizedTotal += waterRayTriangleCount;
                mario = before;
                gCollisionFlags = beforeFlags;
                vec3f_copy(referenceVelocity, movement);
                bypassWaterRayBounds = TRUE;
                waterRayTriangleCount = 0;
                resolve_mario_geometry_walls(&mario);
                u32 reference = mario_water_collision_step(&mario, movement, referenceVelocity);
                referenceTotal += waterRayTriangleCount;
                EXPECT_EQ(result, reference);
                CHECK(memcmp(mario.pos, optimized.pos, sizeof(Vec3f)) == 0);
                CHECK(memcmp(referenceVelocity, velocity, sizeof(Vec3f)) == 0);
                EXPECT_EQ(mario.floor, optimized.floor);
                CHECK(memcmp(&mario.floorHeight, &optimized.floorHeight, sizeof(f32)) == 0);
                EXPECT_EQ(gCollisionFlags, optimizedFlags);
                CHECK(result != WATER_STEP_CANCELLED);
                if (frame >= 16) {
                    CHECK(mario.pos[0] < before.pos[0] - 30);
                    CHECK(mario.pos[1] > before.pos[1] + 15);
                }
            }
        }
    }
    bypassWaterRayBounds = FALSE;
    printf("WORK ship movement rays: %u -> %u detailed triangle tests\n", referenceTotal,
           optimizedTotal);
    CHECK(optimizedTotal * 2 < referenceTotal);
    return TRUE;
}

static int movement_ray_bounds_equivalence(void) {
    load_jrb_sunken_ship();
    use_native_surface_cells();
    s32 checked = 0, hits = 0;
    const s32 flags[] = { RAYCAST_FIND_FLOOR, RAYCAST_FIND_WALL, RAYCAST_FIND_CEIL,
                          RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL | RAYCAST_FIND_CEIL
                              | RAYCAST_IGNORE_INTANGIBLE | RAYCAST_IGNORE_VANISH_CAP_WALL };
    for (s32 i = 0; i < surfaceCount; i++) {
        struct Surface *s = &surfaces[i];
        Vec3s *vertices[] = { &s->vertex1, &s->vertex2, &s->vertex3 };
        for (s32 point = 0; point < 4; point++) {
            Vec3f center;
            for (s32 axis = 0; axis < 3; axis++) {
                center[axis] = point < 3
                                   ? (*vertices[point])[axis]
                                   : (s->vertex1[axis] + s->vertex2[axis] + s->vertex3[axis]) / 3.0f;
            }
            for (u32 kind = 0; kind < ARRAY_COUNT(flags); kind++) {
                Vec3f origin = { center[0] + s->normal.x * 24, center[1] + s->normal.y * 24,
                                 center[2] + s->normal.z * 24 };
                Vec3f direction = { -s->normal.x * 48, -s->normal.y * 48, -s->normal.z * 48 };
                Vec3f hit, referenceHit;
                struct Surface *surface, *referenceSurface;
                s32 filter = flags[kind] | RAYCAST_MARIO_WALL;
                bypassWaterRayBounds = bypassStaticGroups = FALSE;
                f32 distance = find_surface_on_ray(origin, direction, &surface, hit, filter);
                bypassWaterRayBounds = bypassStaticGroups = TRUE;
                f32 reference =
                    find_surface_on_ray(origin, direction, &referenceSurface, referenceHit, filter);
                EXPECT_EQ(surface, referenceSurface);
                CHECK(memcmp(&distance, &reference, sizeof(f32)) == 0);
                CHECK(memcmp(hit, referenceHit, sizeof(Vec3f)) == 0);
                checked++;
                hits += surface != NULL;
            }
        }
    }
    bypassWaterRayBounds = bypassStaticGroups = FALSE;
    CHECK(checked > 4000);
    CHECK(hits > 1000);
    return TRUE;
}

static int water_geometry_handoff(void) {
    const Vec3f positions[] = {
        { 4395, -5118, 2677 }, { 4330, -5118, 2912 }, { 4990, -5119, 3929 }, { 23000, 0, 0 }
    };
    const s16 flags[] = { 0, COLLISION_FLAG_RETURN_FIRST, COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_INCLUDE_INTANGIBLE };
    u32 cachedNodes = 0, repeatedNodes = 0;
    for (u32 point = 0; point < ARRAY_COUNT(positions); point++) {
        for (u32 kind = 0; kind < ARRAY_COUNT(flags); kind++) {
            load_jrb_sunken_ship();
            use_native_surface_cells();
            vec3f_copy(mario.pos, positions[point]);
            vec3f_set(object.header.gfx.pos, 4395, -5118, 2677);
            struct MarioState before = mario;
            gCollisionFlags = flags[kind];
            waterVerticalNodeCount = collisionQueryCount = 0;
            u32 misses = gNumFindFloorMisses;
            mario_water_collision_prepare_geometry(&mario);
            cachedNodes += waterVerticalNodeCount;
            struct MarioState cached = mario;
            u32 cachedQueries = collisionQueryCount, cachedMisses = gNumFindFloorMisses - misses;
            s16 cachedFlags = gCollisionFlags;
            mario = before;
            gCollisionFlags = flags[kind];
            waterVerticalNodeCount = collisionQueryCount = 0;
            misses = gNumFindFloorMisses;
            // The geometry-input sequence before the handoff.
            mario_water_collision_prepare(&mario);
            mario.floorHeight =
                find_floor_precise(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
            if (mario.floor == NULL) {
                vec3f_copy(mario.pos, object.header.gfx.pos);
                mario.floorHeight =
                    find_floor_precise(mario.pos[0], mario.pos[1], mario.pos[2], &mario.floor);
            }
            mario.ceilHeight =
                find_ceil_precise(mario.pos[0], mario.pos[1] + 3, mario.pos[2], &mario.ceil);
            repeatedNodes += waterVerticalNodeCount;
            CHECK(memcmp(mario.pos, cached.pos, sizeof(Vec3f)) == 0);
            EXPECT_EQ(mario.floor, cached.floor);
            EXPECT_EQ(mario.ceil, cached.ceil);
            CHECK(memcmp(&mario.floorHeight, &cached.floorHeight, sizeof(f32)) == 0);
            CHECK(memcmp(&mario.ceilHeight, &cached.ceilHeight, sizeof(f32)) == 0);
            EXPECT_EQ(gCollisionFlags, cachedFlags);
            s32 reusedGeometry = flags[kind] == 0 && cached.floor != NULL
                                 && memcmp(before.pos, cached.pos, sizeof(Vec3f)) == 0;
            EXPECT_EQ(collisionQueryCount, cachedQueries + (reusedGeometry ? 2 : 0));
            EXPECT_EQ(gNumFindFloorMisses - misses, cachedMisses);
        }
    }
    printf("WORK geometry handoff: %u -> %u vertical node visits\n", repeatedNodes, cachedNodes);
    CHECK(cachedNodes < repeatedNodes);
    reset();
    struct Surface *platform = triangle((Vec3s){ -4000, 10, -4000 }, (Vec3s){ 0, 10, 4000 },
                                        (Vec3s){ 4000, 10, -4000 }, 1, SURFACE_DEFAULT);
    platform->flags |= SURFACE_FLAG_DYNAMIC;
    for (s32 frame = 0; frame < 3; frame++) {
        vec3f_set(mario.pos, 0, 50, 0);
        mario_water_collision_prepare_geometry(&mario);
        EXPECT_EQ(mario.floor, platform);
        NEAR(mario.floorHeight, 10 + frame * 5);
        // A later call must see new object geometry at the same position.
        platform->vertex1[1] += 5;
        platform->vertex2[1] += 5;
        platform->vertex3[1] += 5;
        platform->lowerY += 5;
        platform->upperY += 5;
        platform->originOffset -= 5;
    }
    return TRUE;
}

static int stunned_lost_support(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        floor_at(0);
        stunned_setup(60);
        object.oGroundTwirlStunTimer = 9;
        object.oGroundTwirlStunDuration = 30;
        int frames = 0;
        while (object.oPosY > 0 && frames < 8) {
            update_object_with_ground_twirl_stun();
            frames++;
            EXPECT_EQ(object.oGroundTwirlStunTimer, 9 - frames);
            EXPECT_EQ(nativeEnemyUpdates, 0);
        }
        NEAR(object.oPosY, 0);
        CHECK(frames < 8);
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 8 - frames);
        // Short stun expires in the air without a new launch timer or reload.
        reset();
        floor_at(-1000);
        stunned_setup(60);
        object.oGroundTwirlStunTimer = 2;
        object.oGroundTwirlStunDuration = 30;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 1);
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(object.oGroundTwirlStunTimer, 0);
        CHECK(object.oPosY > 0);
        f32 y = object.oPosY, velocity = object.oVelY;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(nativeEnemyUpdates, 1);
        NEAR(object.oPosY, y);
        NEAR(object.oVelY, velocity);
    }
    return TRUE;
}
static int platform_collision(void) {
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        reset();
        stunned_setup(20);
        object.hitboxRadius = 10;
        wall_at(50, -500, 500, dynamic, SURFACE_DEFAULT);
        Vec3f start = { 0, 20, 0 }, target = { 120, 20, 25 };
        object_resolve_platform_motion(&object, start, target, NULL);
        CHECK(target[0] <= 40.05f);
        CHECK(target[0] >= 39.9f);
        NEAR(target[2], 25);
        // An enemy's own published body cannot block its carrier transport.
        for (int i = 0; i < surfaceCount; i++) {
            surfaces[i].object = &object;
        }
        vec3f_set(target, 120, 20, 25);
        object_resolve_platform_motion(&object, start, target, NULL);
        NEAR(target[0], 120);
        NEAR(target[2], 25);
    }
    return TRUE;
}
static int wall_candidate_cache_equivalence(void) {
    reset();
    wall_at(50, -500, 500, 0, SURFACE_DEFAULT);
    wall_at(40, -500, 500, 1, SURFACE_VANISH_CAP_WALLS);
    wall_at(65, -500, 500, 0, SURFACE_CAMERA_BOUNDARY);
    for (int i = 0; i < 12; i++) {
        wall_at(50, 1000 + i * 100, 1050 + i * 100, i & 1, SURFACE_DEFAULT);
    }
#if STATIC_SURFACE_INDEX
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticSurfaceIndex.offsets != NULL);
#endif
    Vec3f origin = { 35, 0, 0 };
    mario_reset_wall_candidate_cache(TRUE, 30, 60, 0, 0);
    mario_build_wall_candidate_cache(origin);
    CHECK(sMarioWallCandidateCache.valid);
    const s16 flags[] = { 0,
                          COLLISION_FLAG_CAMERA,
                          COLLISION_FLAG_RETURN_FIRST,
                          COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_INCLUDE_INTANGIBLE,
                          COLLISION_FLAG_CAMERA | COLLISION_FLAG_RETURN_FIRST
                              | COLLISION_FLAG_EXCLUDE_DYNAMIC };
    for (int fallback = 0; fallback < 2; fallback++) {
        // A push out of the filtered region must redo the original full scan.
        sMarioWallCandidateCache.view.minX = fallback ? 34.0f : -15.0f;
        for (int vanish = 0; vanish < 2; vanish++) {
            mario.flags = vanish ? MARIO_VANISH_CAP : 0;
            for (unsigned i = 0; i < ARRAY_COUNT(flags); i++) {
                struct WallCollisionData native = { 0 }, cached = { 0 };
                native.x = 35.25f;
                native.y = 0.5f;
                native.z = -0.75f;
                native.offsetY = 30;
                native.radius = 50;
                cached = native;
                gCollisionFlags = flags[i];
                s32 nativeHits = find_wall_collisions(&native);
                s16 nativeFlags = gCollisionFlags;
                gCollisionFlags = flags[i];
                EXPECT_EQ(find_wall_collisions_with_cache(&cached, &sMarioWallCandidateCache.view),
                          nativeHits);
                EXPECT_EQ(gCollisionFlags, nativeFlags);
                CHECK(memcmp(&cached.x, &native.x, sizeof(f32)) == 0);
                CHECK(memcmp(&cached.y, &native.y, sizeof(f32)) == 0);
                CHECK(memcmp(&cached.z, &native.z, sizeof(f32)) == 0);
                EXPECT_EQ(cached.numWalls, native.numWalls);
                for (s32 wall = 0; wall < cached.numWalls; wall++) {
                    EXPECT_EQ(cached.walls[wall], native.walls[wall]);
                }
            }
        }
    }
    return TRUE;
}

static int swimming_wall_height_filter(void) {
    reset();
    wall_at(50, -500, 500, 0, SURFACE_DEFAULT);
    wall_at(40, -500, 500, 1, SURFACE_VANISH_CAP_WALLS);
    wall_at(65, -500, 500, 0, SURFACE_CAMERA_BOUNDARY);
    for (s32 i = 0; i < 12; i++) {
        wall_at(50, 1000 + i * 100, 1050 + i * 100, i & 1, SURFACE_DEFAULT);
    }
    use_native_surface_cells();
    struct WaterWallCandidates candidates;
    Vec3f origin = { 35, 0, 0 };
    Vec3f travel = { 0, 0, 0 };
    struct WallCollisionCache *cache = prepare_water_wall_candidates(&candidates, origin, travel);
    CHECK(cache != NULL);
    CHECK(candidates.nodeCount < 12);
    const s16 flags[] = { 0, COLLISION_FLAG_CAMERA, COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_RETURN_FIRST, COLLISION_FLAG_INCLUDE_INTANGIBLE };
    const f32 points[][3] = { { 35.25f, 0.5f, -0.75f }, { 35, -32, 0 }, { 35, 92, 0 }, { 35, 93, 0 },
                              { 35, 1000, 0 },          { 500, 0, 0 },  { 1100, 0, 0 } };
    for (s32 leave = 0; leave < 2; leave++) {
        cache->minX = leave ? 34.0f : origin[0] - 112.0f;
        for (s32 vanish = 0; vanish < 2; vanish++) {
            mario.flags = vanish ? MARIO_VANISH_CAP : 0;
            for (u32 point = 0; point < ARRAY_COUNT(points); point++) {
                for (u32 flag = 0; flag < ARRAY_COUNT(flags); flag++) {
                    struct WallCollisionData native = { 0 }, filtered = { 0 };
                    native.x = points[point][0];
                    native.y = points[point][1] - 10;
                    native.z = points[point][2];
                    native.radius = 110;
                    native.offsetY = 10;
                    filtered = native;
                    gCollisionFlags = flags[flag];
                    s32 hits = find_wall_collisions(&native);
                    s16 remainingFlags = gCollisionFlags;
                    gCollisionFlags = flags[flag];
                    EXPECT_EQ(find_wall_collisions_with_cache(&filtered, cache), hits);
                    EXPECT_EQ(gCollisionFlags, remainingFlags);
                    CHECK(memcmp(&filtered.x, &native.x, sizeof(f32)) == 0);
                    CHECK(memcmp(&filtered.z, &native.z, sizeof(f32)) == 0);
                    EXPECT_EQ(filtered.numWalls, native.numWalls);
                    for (s32 wall = 0; wall < filtered.numWalls; wall++) {
                        EXPECT_EQ(filtered.walls[wall], native.walls[wall]);
                    }
                }
            }
        }
    }
    // A new publication must rebuild, and overflowing storage must fail open.
    wall_at(55, -500, 500, 1, SURFACE_DEFAULT);
    use_native_surface_cells();
    s32 previousCount = candidates.nodeCount;
    CHECK(prepare_water_wall_candidates(&candidates, origin, travel) != NULL);
    CHECK(candidates.nodeCount > previousCount);
    reset();
    for (s32 i = 0; i <= WATER_COLLISION_CACHE_NODES; i++) {
        wall_at(50, -500, 500, i & 1, SURFACE_DEFAULT);
    }
    CHECK(prepare_water_wall_candidates(&candidates, origin, travel) == NULL);
    vec3f_set(origin, 20000, 0, 0);
    CHECK(prepare_water_wall_candidates(&candidates, origin, travel) == NULL);
    return TRUE;
}

static int water_top_bottom_scan(void) {
    reset();
    struct Surface *low = triangle((Vec3s){ -800, 100, -800 }, (Vec3s){ 0, 100, 800 },
                                   (Vec3s){ 800, 100, -800 }, 0, SURFACE_NEW_WATER);
    struct Surface *high = triangle((Vec3s){ -800, 250, -800 }, (Vec3s){ 0, 250, 800 },
                                    (Vec3s){ 800, 250, -800 }, 0, SURFACE_NEW_WATER);
    struct Surface *bottom = triangle((Vec3s){ -800, 150, -800 }, (Vec3s){ 800, 150, -800 },
                                      (Vec3s){ 0, 150, 800 }, 0, SURFACE_NEW_WATER_BOTTOM);
    struct SurfaceNode waterNodes[] = { { &waterNodes[1], high },
                                        { &waterNodes[2], bottom },
                                        { NULL, low } };
    s32 cell = GET_CELL_COORD(0);
    gStaticSurfacePartition[cell][cell][SPATIAL_PARTITION_WATER] = waterNodes;
#if STATIC_SURFACE_INDEX
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticSurfaceIndex.offsets != NULL);
#endif
    struct Surface *hit = NULL;
    // The bottom pass caps the top search, whose cursor must restart at the head.
    NEAR(find_water_floor(0, 0, 0, &hit), 100);
    EXPECT_EQ(hit, low);
    NEAR(find_water_floor(0, 1000, 0, &hit), 250);
    EXPECT_EQ(hit, high);
    // Native misses leave the caller's output untouched, including empty cells.
    NEAR(find_water_floor(2000, 0, 2000, &hit), FLOOR_LOWER_LIMIT);
    EXPECT_EQ(hit, high);
    gStaticSurfacePartition[cell][cell][SPATIAL_PARTITION_WATER] = NULL;
    gStaticSurfaceGeneration++;
    NEAR(find_water_floor(0, 0, 0, &hit), FLOOR_LOWER_LIMIT);
    EXPECT_EQ(hit, high);
    return TRUE;
}

static int cached_floor_matches(f32 x, f32 y, f32 z, s16 flags) {
    struct Surface *expected, *observed;
    gCollisionFlags = flags;
    int referenceMisses = gNumFindFloorMisses;
    f32 expectedHeight = uncached_find_floor(x, y, z, &expected);
    int expectedMisses = gNumFindFloorMisses - referenceMisses;
    s16 expectedFlags = gCollisionFlags;
    for (int repeat = 0; repeat < 2; repeat++) {
        gCollisionFlags = flags;
        int misses = gNumFindFloorMisses;
        f32 observedHeight = find_floor(x, y, z, &observed);
        CHECK(memcmp(&expectedHeight, &observedHeight, sizeof(f32)) == 0);
        EXPECT_EQ(observed, expected);
        EXPECT_EQ(gCollisionFlags, expectedFlags);
        EXPECT_EQ(gNumFindFloorMisses - misses, expectedMisses);
    }
    return TRUE;
}

static int static_floor_cache_equivalence(void) {
    reset();
    floor_at(0);
    triangle((Vec3s){ -800, 30, -800 }, (Vec3s){ 0, 60, 800 }, (Vec3s){ 800, 90, -800 }, 0,
             SURFACE_DEFAULT)
        ->flags = SURFACE_FLAG_NO_CAM_COLLISION;
    floor_at(100);
    surfaces[surfaceCount - 1].type = SURFACE_VANISH_CAP_WALLS;
    floor_at(150);
    surfaces[surfaceCount - 1].type = SURFACE_INTANGIBLE;
    floor_at(200);
    surfaces[surfaceCount - 1].type = SURFACE_CAMERA_BOUNDARY;
    struct Surface *dynamic = triangle((Vec3s){ -800, 80, -800 }, (Vec3s){ 0, 80, 800 },
                                       (Vec3s){ 800, 80, -800 }, 1, SURFACE_DEFAULT);
    struct Object other = { 0 };
    const s16 flags[] = { 0,
                          COLLISION_FLAG_CAMERA,
                          COLLISION_FLAG_RETURN_FIRST,
                          COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_INCLUDE_INTANGIBLE,
                          COLLISION_FLAG_CAMERA | COLLISION_FLAG_RETURN_FIRST
                              | COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE };
    const f32 positions[][3] = { { 0.9f, 300, -0.9f },      { 0, -40, 0 },
                                 { 0, -2000, 0 },           { 799.9f, 50.9f, -799.9f },
                                 { -799.9f, 100, -799.9f }, { 810, 300, 810 },
                                 { 4001, 300, 4001 },       { LEVEL_BOUNDARY_MAX + 1, 300, 0 } };
#if STATIC_SURFACE_INDEX
    build_static_surface_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticSurfaceIndex.offsets != NULL);
#endif
    for (int cycle = 0; cycle < 3; cycle++) {
        gGlobalTimer++;
        for (int eligibility = 0; eligibility < 5; eligibility++) {
            if (eligibility == 3) {
                o = &other;
            } else if (eligibility == 4) {
                o = NULL;
            } else {
                o = &object;
            }
            mario.flags = eligibility == 1 ? MARIO_VANISH_CAP : 0;
            object.activeFlags = eligibility == 2 ? ACTIVE_FLAG_MOVE_THROUGH_GRATE : 0;
            other.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
            for (unsigned flag = 0; flag < ARRAY_COUNT(flags); flag++) {
                for (unsigned point = 0; point < ARRAY_COUNT(positions); point++) {
                    CHECK(cached_floor_matches(positions[point][0], positions[point][1],
                                               positions[point][2], flags[flag]));
                }
            }
        }
        // Dynamic publication can change repeatedly within the same frame.
        o = &object;
        mario.flags = object.activeFlags = 0;
        for (int height = 300; height >= -100; height -= 25) {
            dynamic->originOffset = -height;
            dynamic->lowerY = height - 5;
            dynamic->upperY = height + 5;
            dynamic->vertex1[1] = dynamic->vertex2[1] = dynamic->vertex3[1] = height;
            CHECK(cached_floor_matches(0, 300, 0, 0));
        }
    }
    o = &object;
    return TRUE;
}

static int static_floor_cache_lifecycle(void) {
    reset();
    wall_at(50, -500, 500, 0, SURFACE_MARIO_ONLY_GRIPPY);
    struct Object wallOther = { 0 };
    for (int actor = 0; actor < 3; actor++) {
        o = actor == 1 ? &wallOther : gMarioObject;
        gCollisionFlags = actor == 2 ? COLLISION_FLAG_CAMERA : 0;
        struct WallCollisionData wall = { 0 };
        wall.x = 35;
        wall.radius = 110;
        EXPECT_EQ(find_wall_collisions(&wall) > 0, actor == 0);
    }
    o = gMarioObject;
    reset();
    floor_at(0);
    struct Surface *pillar = triangle((Vec3s){ -1000, 50, -1000 }, (Vec3s){ 0, 50, 1000 },
                                      (Vec3s){ 1000, 50, -1000 }, 0, SURFACE_MARIO_ONLY_GRIPPY);
    struct Surface *pillarHit;
    struct Object other = { 0 };
    gStaticSurfaceGeneration++;
    NEAR(find_floor(0, 100, 0, &pillarHit), 50);
    EXPECT_EQ(pillarHit, pillar);
    o = &other;
    NEAR(find_floor(0, 100, 0, &pillarHit), 0);
    o = gMarioObject;
    NEAR(find_floor(0, 100, 0, &pillarHit), 50);
    gCollisionFlags = COLLISION_FLAG_CAMERA;
    NEAR(find_floor(0, 100, 0, &pillarHit), 0);
    reset();
    floor_at(10);
    struct Surface *staticFloor = &surfaces[surfaceCount - 1], *hit;
    NEAR(find_floor(0, 30, 0, &hit), 10);
    EXPECT_EQ(hit, staticFloor);
    // A generation change must invalidate even a retained list head/address.
    staticFloor->originOffset = -20;
    staticFloor->lowerY = 15;
    staticFloor->upperY = 25;
    gStaticSurfaceGeneration++;
    NEAR(find_floor(0, 30, 0, &hit), 20);
    // A fresh frame cannot reuse a previous result or surface pointer.
    staticFloor->originOffset = -25;
    staticFloor->lowerY = 20;
    staticFloor->upperY = 30;
    gGlobalTimer++;
    NEAR(find_floor(0, 30, 0, &hit), 25);
    struct Surface *grate = triangle((Vec3s){ -1000, 40, -1000 }, (Vec3s){ 0, 40, 1000 },
                                     (Vec3s){ 1000, 40, -1000 }, 0, SURFACE_VANISH_CAP_WALLS);
    gStaticSurfaceGeneration++;
    NEAR(find_floor(0, 30, 0, &hit), 40);
    mario.flags = MARIO_VANISH_CAP;
    NEAR(find_floor(0, 30, 0, &hit), 25);
    mario.flags = 0;
    NEAR(find_floor(0, 30, 0, &hit), 40);
    object.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    NEAR(find_floor(0, 30, 0, &hit), 25);
    object.activeFlags = 0;
    NEAR(find_floor(0, 30, 0, &hit), 40);
    EXPECT_EQ(hit, grate);
    struct Surface *dynamic = triangle((Vec3s){ -1000, 50, -1000 }, (Vec3s){ 0, 50, 1000 },
                                       (Vec3s){ 1000, 50, -1000 }, 1, SURFACE_DEFAULT);
    gGlobalTimer++;
    NEAR(find_floor(0, 30, 0, &hit), 50);
    dynamic->originOffset = -5;
    dynamic->lowerY = 0;
    dynamic->upperY = 10;
    NEAR(find_floor(0, 30, 0, &hit), 40);
    EXPECT_EQ(hit, grate);
    // Same-height moving supports retain dynamic priority and current identity.
    dynamic->originOffset = -40;
    dynamic->lowerY = 35;
    dynamic->upperY = 45;
    NEAR(find_floor(0, 30, 0, &hit), 40);
    EXPECT_EQ(hit, dynamic);
    struct Surface *replacement = triangle((Vec3s){ -1000, 40, -1000 }, (Vec3s){ 0, 40, 1000 },
                                           (Vec3s){ 1000, 40, -1000 }, 1, SURFACE_DEFAULT);
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_FLOORS]->surface = replacement;
        }
    }
    NEAR(find_floor(0, 30, 0, &hit), 40);
    EXPECT_EQ(hit, replacement);
    // Cached misses still count, and late static publication makes them hits.
    for (int repeat = 0; repeat < 2; repeat++) {
        NEAR(find_floor(0, -2000, 0, &hit), FLOOR_LOWER_LIMIT);
        CHECK(hit == NULL);
    }
    floor_at(-2000);
    gStaticSurfaceGeneration++;
    NEAR(find_floor(0, -2000, 0, &hit), -2000);
    return TRUE;
}

static int static_floor_cache_work(void) {
    reset();
    for (unsigned i = 0; i < ARRAY_COUNT(labUnevenTriangles); i++) {
        triangle(labUnevenTriangles[i][0], labUnevenTriangles[i][1], labUnevenTriangles[i][2], 0,
                 SURFACE_DEFAULT);
    }
    // Retain only nodes published in these cells by the real loader's bounds.
    // Other water fixtures deliberately insert each triangle in every cell.
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            struct SurfaceNode **node = &gStaticSurfacePartition[z][x][SPATIAL_PARTITION_FLOORS];
            while (*node != NULL) {
                struct Surface *surface = (*node)->surface;
                int minX = MIN(surface->vertex1[0], MIN(surface->vertex2[0], surface->vertex3[0]));
                int maxX = MAX(surface->vertex1[0], MAX(surface->vertex2[0], surface->vertex3[0]));
                int minZ = MIN(surface->vertex1[2], MIN(surface->vertex2[2], surface->vertex3[2]));
                int maxZ = MAX(surface->vertex1[2], MAX(surface->vertex2[2], surface->vertex3[2]));
                if (x < lower_cell_index(minX) || x > upper_cell_index(maxX)
                    || z < lower_cell_index(minZ) || z > upper_cell_index(maxZ)) {
                    *node = (*node)->next;
                } else {
                    node = &(*node)->next;
                }
            }
        }
    }
    u32 cachedWork = 0, referenceWork = 0;
    for (unsigned i = 0; i < ARRAY_COUNT(labUnevenTriangles); i++) {
        f32 x = 0, z = 0;
        for (int vertex = 0; vertex < 3; vertex++) {
            x += labUnevenTriangles[i][vertex][0] / 3.0f;
            z += labUnevenTriangles[i][vertex][2] / 3.0f;
        }
        gGlobalTimer++;
        for (int repeat = 0; repeat < 8; repeat++) {
            struct Surface *hit, *expected;
            floorNodeVisits = 0;
            f32 reference = uncached_find_floor(x, 3000, z, &expected);
            referenceWork += floorNodeVisits;
            floorNodeVisits = 0;
            f32 observed = find_floor(x, 3000, z, &hit);
            cachedWork += floorNodeVisits;
            EXPECT_EQ(hit, expected);
            CHECK(memcmp(&observed, &reference, sizeof(f32)) == 0);
        }
    }
    CHECK(referenceWork > 1000);
    EXPECT_EQ(cachedWork * 8, referenceWork);
    EXPECT_EQ(collisionQueryCount, ARRAY_COUNT(labUnevenTriangles) * 16);
    EXPECT_EQ(collisionTimedQueries, collisionQueryCount);
    printf("WORK static floor cache: %u -> %u node visits for repeated Lab queries\n", referenceWork,
           cachedWork);
    // Aligned nearby coordinates must not all evict the same cache entry.
    reset();
    floor_at(0);
    floor_at(-10);
    struct Surface *hit;
    for (int x = 0; x < 32; x += 8) {
        NEAR(find_floor(x, 30, 0, &hit), 0);
    }
    floorNodeVisits = 0;
    for (int repeat = 0; repeat < 3; repeat++) {
        for (int x = 0; x < 32; x += 8) {
            NEAR(find_floor(x, 30, 0, &hit), 0);
        }
    }
    EXPECT_EQ(floorNodeVisits, 0);
    return TRUE;
}

static int platform_support_query(void) {
    reset();
    floor_at(0);
    struct Object carrier = { 0 }, attachment = { 0 };
    struct Surface *self = triangle((Vec3s){ -1000, 20, -1000 }, (Vec3s){ 0, 20, 1000 },
                                    (Vec3s){ 1000, 20, -1000 }, 1, SURFACE_DEFAULT);
    struct Surface *other = triangle((Vec3s){ -1000, 10, -1000 }, (Vec3s){ 0, 10, 1000 },
                                     (Vec3s){ 1000, 10, -1000 }, 1, SURFACE_DEFAULT);
    self->object = &object;
    other->object = &carrier;
    struct Surface *hit = NULL;
    NEAR(find_floor_excluding_objects(0, 20, 0, &hit, &object, NULL), 10);
    EXPECT_EQ(hit, other);
    other->object = &attachment;
    NEAR(find_floor_excluding_objects(0, 20, 0, &hit, &object, &attachment), 0);
    other->object = &carrier;
    other->type = SURFACE_VANISH_CAP_WALLS;
    object.activeFlags = 0;
    NEAR(find_floor_excluding_objects(0, 20, 0, &hit, &object, NULL), 10);
    object.activeFlags = ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    NEAR(find_floor_excluding_objects(0, 20, 0, &hit, &object, NULL), 0);
    EXPECT_EQ(collisionQueryCount, 4);
    EXPECT_EQ(collisionTimedQueries, 4);
    NEAR(find_floor_excluding_objects(LEVEL_BOUNDARY_MAX + 1, 20, 0, &hit, &object, NULL),
         FLOOR_LOWER_LIMIT);
    CHECK(hit == NULL);
    EXPECT_EQ(collisionQueryCount, 5);
    EXPECT_EQ(collisionTimedQueries, 5);
    return TRUE;
}
static int inactive_object_floor_work(void) {
    reset();
    floor_at(0);
    gMarioObject = &moleHole;
    object.activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_FAR_AWAY;
    object.oPosY = 20;
    cur_obj_update_floor_and_walls();
    struct Surface *floor = object.oFloor;
    u32 queries = collisionQueryCount;
    for (int frame = 0; frame < 30; frame++) {
        gGlobalTimer++;
        cur_obj_update_floor_and_walls();
        CHECK(object.oFloor == floor);
        NEAR(object.oFloorHeight, 0);
    }
    EXPECT_EQ(collisionQueryCount, queries);
    object.oFloorType = SURFACE_BURNING;
    object.oFloorRoom = 9;
    cur_obj_update_floor_and_walls();
    EXPECT_EQ(object.oFloorType, SURFACE_DEFAULT);
    EXPECT_EQ(object.oFloorRoom, 0);
    EXPECT_EQ(collisionQueryCount, queries);
    object.activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_IN_DIFFERENT_ROOM;
    cur_obj_update_floor_and_walls();
    EXPECT_EQ(collisionQueryCount, queries);
    for (int cycle = 0; cycle < 3; cycle++) {
        object.activeFlags = ACTIVE_FLAG_ACTIVE;
        cur_obj_update_floor_and_walls();
        EXPECT_EQ(collisionQueryCount, ++queries);
        object.activeFlags |= ACTIVE_FLAG_FAR_AWAY;
        cur_obj_update_floor_and_walls();
        EXPECT_EQ(collisionQueryCount, queries);
    }
    // An active object sharing the dormant object's slot cannot evict it.
    struct Object objects[INACTIVE_OBJECT_FLOOR_CACHE_SIZE + 1] = { 0 };
    o = &objects[0];
    o->activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_FAR_AWAY;
    o->oPosY = 20;
    cur_obj_update_floor();
    queries = collisionQueryCount;
    o = &objects[INACTIVE_OBJECT_FLOOR_CACHE_SIZE];
    o->activeFlags = ACTIVE_FLAG_ACTIVE;
    o->oPosY = 20;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    o = &objects[0];
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, queries);
    o = &object;
    return 1;
}

static int inactive_object_floor_refresh(void) {
    reset();
    floor_at(0);
    gMarioObject = &moleHole;
    object.activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_FAR_AWAY;
    object.oPosY = 20;
    cur_obj_update_floor();
    u32 queries = collisionQueryCount;
    object.oPosX += 0.25f;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    object.oPosY += 0.25f;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    object.oPosZ += 0.25f;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    gStaticSurfaceGeneration++;
    floor_at(10);
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    NEAR(object.oFloorHeight, 10);
    object.activeFlags |= ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    gCollisionFlags = COLLISION_FLAG_EXCLUDE_DYNAMIC;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    EXPECT_EQ(gCollisionFlags, 0);
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    object.oFloor = NULL;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    gMarioObject = &object;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    return 1;
}

static int inactive_object_dynamic_floor(void) {
    reset();
    floor_at(0);
    gMarioObject = &moleHole;
    object.activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_FAR_AWAY;
    object.oPosY = 50;
    cur_obj_update_floor();
    u32 queries = collisionQueryCount;
    struct Surface *platform = triangle((Vec3s){ -4000, 10, -4000 }, (Vec3s){ 0, 10, 4000 },
                                        (Vec3s){ 4000, 10, -4000 }, 1, SURFACE_DEFAULT);
    platform->flags |= SURFACE_FLAG_DYNAMIC;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    CHECK(object.oFloor == platform);
    platform->originOffset = -30;
    object.oFloor = NULL; // The frame's dynamic-reference cleanup.
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    NEAR(object.oFloorHeight, 30);
    memset(gDynamicSurfacePartition, 0, sizeof(gDynamicSurfacePartition));
    object.oFloor = NULL;
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, ++queries);
    NEAR(object.oFloorHeight, 0);
    cur_obj_update_floor();
    EXPECT_EQ(collisionQueryCount, queries);
    return 1;
}

static struct Object settled_flame(void) {
    return (struct Object){ .activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_IGNORE_ENV_BOXES,
                            .oAction = 1,
                            .oRoom = -1,
                            .location = { 100, 0, -100 },
                            .oVelX = 12,
                            .oVelZ = -7,
                            .oDragStrength = 10,
                            .oMoveAngleYaw = 0x1234,
                            .oMoveFlags = OBJ_MOVE_ON_GROUND | OBJ_MOVE_HIT_EDGE };
}

static int flame_movement_matches(const struct Object *control) {
    CHECK(object.oPosX == control->oPosX);
    CHECK(object.oPosY == control->oPosY);
    CHECK(object.oPosZ == control->oPosZ);
    CHECK(object.oVelX == control->oVelX);
    CHECK(object.oVelY == control->oVelY);
    CHECK(object.oVelZ == control->oVelZ);
    CHECK(object.oForwardVel == control->oForwardVel);
    CHECK(object.oFloorHeight == control->oFloorHeight);
    EXPECT_EQ(object.oFloor, control->oFloor);
    EXPECT_EQ(object.oFloorType, control->oFloorType);
    EXPECT_EQ(object.oFloorRoom, control->oFloorRoom);
    EXPECT_EQ(object.oMoveFlags, control->oMoveFlags);
    EXPECT_EQ(object.oWallAngle, control->oWallAngle);
    EXPECT_EQ(object.activeFlags, control->activeFlags);
    return TRUE;
}

static int compare_flame_movement(struct Object initial, s16 queryFlags, int savedQueries) {
    object = initial;
    gCollisionFlags = queryFlags;
    collisionQueryCount = 0;
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
    struct Object control = object;
    u32 controlQueries = collisionQueryCount;
    s16 controlFlags = gCollisionFlags;

    object = initial;
    gCollisionFlags = queryFlags;
    collisionQueryCount = 0;
    bowser_flame_update_movement();
    CHECK(flame_movement_matches(&control));
    EXPECT_EQ(gCollisionFlags, controlFlags);
    EXPECT_EQ(collisionQueryCount + savedQueries, controlQueries);
    return TRUE;
}

static int settled_flame_movement(void) {
    const struct {
        s16 height;
        SurfaceType type;
        s32 dynamic;
    } floorCases[] = { { 0, SURFACE_DEFAULT, FALSE }, { 4, SURFACE_DEFAULT, TRUE },
                       { -8, SURFACE_DEFAULT, TRUE }, { FLOOR_LOWER_LIMIT, SURFACE_DEFAULT, FALSE },
                       { 0, SURFACE_BURNING, FALSE }, { 0, SURFACE_DEATH_PLANE, FALSE } };
    const u32 moveFlags[] = { 0,
                              OBJ_MOVE_LANDED,
                              OBJ_MOVE_ON_GROUND,
                              OBJ_MOVE_ON_GROUND | OBJ_MOVE_HIT_EDGE,
                              OBJ_MOVE_IN_AIR,
                              OBJ_MOVE_LEFT_GROUND,
                              OBJ_MOVE_UNDERWATER_ON_GROUND,
                              OBJ_MOVE_AT_WATER_SURFACE };
    const u32 activeFlags[] = {
        ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_IGNORE_ENV_BOXES,
        ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_IGNORE_ENV_BOXES | ACTIVE_FLAG_FAR_AWAY,
        ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_IGNORE_ENV_BOXES | ACTIVE_FLAG_IN_DIFFERENT_ROOM
    };
    int comparisons = 0;
    for (unsigned floorCase = 0; floorCase < ARRAY_COUNT(floorCases); floorCase++) {
        reset();
        // Remove reset's default floor so missing support is a real NULL result.
        memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
        int height = floorCases[floorCase].height;
        if (height != FLOOR_LOWER_LIMIT) {
            triangle((Vec3s){ -4000, height, -4000 }, (Vec3s){ 0, height, 4000 },
                     (Vec3s){ 4000, height, -4000 }, floorCases[floorCase].dynamic,
                     floorCases[floorCase].type);
        }
        for (int action = 0; action < 2; action++) {
            for (int speed = 0; speed < 4; speed++) {
                for (unsigned motion = 0; motion < ARRAY_COUNT(moveFlags); motion++) {
                    for (unsigned active = 0; active < ARRAY_COUNT(activeFlags); active++) {
                        for (int room = 0; room < 2; room++) {
                            for (int special = 0; special < 2; special++) {
                                struct Object initial = settled_flame();
                                initial.oAction = action;
                                initial.oForwardVel = speed == 1 ? 10 : 0;
                                initial.oVelY = speed == 2 ? -1 : 0;
                                initial.oGravity = speed == 3 ? -1 : 0;
                                initial.oMoveFlags = moveFlags[motion];
                                initial.activeFlags = activeFlags[active];
                                initial.oRoom = room ? 1 : -1;
                                int saved = action && speed == 0 && active == 0 && !room && !special
                                            && height != FLOOR_LOWER_LIMIT
                                            && !(moveFlags[motion] & OBJ_MOVE_MASK_IN_WATER);
                                CHECK(compare_flame_movement(
                                    initial, special ? COLLISION_FLAG_EXCLUDE_DYNAMIC : 0, saved));
                                comparisons++;
                            }
                        }
                    }
                }
            }
        }
    }
    EXPECT_EQ(comparisons, 4608);
    // Entering water must still run vertical integration and update its flags.
    reset();
    floor_at(0);
    enemyWater = 10;
    struct Object initial = settled_flame();
    initial.activeFlags &= ~ACTIVE_FLAG_IGNORE_ENV_BOXES;
    CHECK(compare_flame_movement(initial, 0, 1));
    CHECK(object.oMoveFlags & OBJ_MOVE_ENTERED_WATER);
    return TRUE;
}

static int settled_flame_floor_changes(void) {
    const int heights[] = { 0, 4, -8, FLOOR_LOWER_LIMIT, 0 };
    for (int cycle = 0; cycle < 3; cycle++) {
        struct Object initial = settled_flame();
        for (unsigned step = 0; step < ARRAY_COUNT(heights); step++) {
            reset();
            memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
            struct Surface *support = NULL;
            int height = heights[step];
            if (height != FLOOR_LOWER_LIMIT) {
                support = triangle((Vec3s){ -4000, height, -4000 }, (Vec3s){ 0, height, 4000 },
                                   (Vec3s){ 4000, height, -4000 }, 1, SURFACE_DEFAULT);
            }
            CHECK(compare_flame_movement(initial, 0, support != NULL));
            EXPECT_EQ(object.oFloor, support);
            CHECK(object.oFloorHeight == height);
            CHECK(object.oPosY == MAX(initial.oPosY, height));
            initial = object;
        }
    }
    return TRUE;
}

static int settled_flame_fixed_arena(void) {
    reset();
    floor_at(307);
    gCurrLevelNum = LEVEL_BOWSER_3;
    object = settled_flame();
    object.oPosY = 307;
    cur_obj_update_floor_and_walls();
    struct Object initial = object;
    // Repeated resting updates retain all native movement flags and water work.
    for (int frame = 0; frame < 100; frame++) {
        CHECK(compare_flame_movement(initial, 0, 2));
        initial = object;
    }
    // Leaving the fixed center, special queries and moving floors refresh support.
    initial.oPosX = 1000;
    CHECK(compare_flame_movement(initial, 0, 1));
    initial.oPosX = 100;
    // The first query consumes the flag; unchanged static support then saves
    // the movement query while the full state comparison remains required.
    CHECK(compare_flame_movement(initial, COLLISION_FLAG_EXCLUDE_DYNAMIC, 1));
    initial.oFloor->flags |= SURFACE_FLAG_DYNAMIC;
    CHECK(compare_flame_movement(initial, 0, 1));
    initial.oFloor->flags &= ~SURFACE_FLAG_DYNAMIC;
    initial.oWallHitboxRadius = 50;
    CHECK(compare_flame_movement(initial, 0, 1));
    gCurrLevelNum = LEVEL_BBH;
    CHECK(compare_flame_movement(initial, 0, 1));
    return TRUE;
}

static int settled_flame_work(void) {
    reset();
    floor_at(0);
    struct Object initial = settled_flame();
    object = initial;
    for (int frame = 0; frame < 100; frame++) {
        cur_obj_update_floor_and_walls();
        cur_obj_move_standard(78);
    }
    struct Object control = object;
    EXPECT_EQ(collisionQueryCount, 200);
    object = initial;
    collisionQueryCount = 0;
    for (int frame = 0; frame < 100; frame++) {
        bowser_flame_update_movement();
    }
    CHECK(flame_movement_matches(&control));
    EXPECT_EQ(collisionQueryCount, 100);
    return TRUE;
}

#if STATIC_WALL_HEIGHT_GROUPS
static int static_wall_ray_height_equivalence(void) {
    reset();
    // Several storeys share the basement cells. Include special walls and a
    // moving wall at the same distance to preserve equal-hit ownership.
    for (s32 storey = 0; storey < 8; storey++) {
        for (s32 face = 0; face < 12; face++) {
            s16 type = SURFACE_DEFAULT;
            if (face == 3) {
                type = SURFACE_VANISH_CAP_WALLS;
            } else if (face == 5) {
                type = SURFACE_CAMERA_BOUNDARY;
            }
            wall_at(face * 8, -3000 + storey * 500, -2700 + storey * 500, FALSE, type);
        }
    }
    wall_at(0, -3000, -2700, TRUE, SURFACE_DEFAULT);
    use_native_surface_cells();
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticWallHeightIndex.cells != NULL);
    struct StaticWallHeightCell *cells = gStaticWallHeightIndex.cells;
    const f32 heights[] = { -3006, -3005, -3004, -2800, -2695, -2694, -1000, 0, 1000 };
    const Vec3f directions[] = {
        { -512, 0, 0.5f }, { 512, 0, -0.5f }, { -512, 1, 0.5f }, { -512, -1, 0.5f }, { 0, 512, 0 }
    };
    const s32 filters[] = { RAYCAST_FIND_WALL, RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL,
                            RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL | RAYCAST_IGNORE_VANISH_CAP_WALL,
                            RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL | RAYCAST_FIND_FLOOR
                                | RAYCAST_FIND_CEIL };
    u32 ordinaryNodes = 0, groupedNodes = 0;
    for (u32 height = 0; height < ARRAY_COUNT(heights); height++) {
        for (u32 direction = 0; direction < ARRAY_COUNT(directions); direction++) {
            for (u32 filter = 0; filter < ARRAY_COUNT(filters); filter++) {
                Vec3f origin = { directions[direction][0] < 0 ? 256 : -256, heights[height], 0 };
                Vec3f ray, expectedPosition = { 0 }, actualPosition = { 0 };
                struct Surface *expected, *actual;
                vec3f_copy(ray, (f32 *) directions[direction]);
                gStaticWallHeightIndex.cells = NULL;
                waterRayNodeCount = 0;
                f32 expectedDistance =
                    find_surface_on_ray(origin, ray, &expected, expectedPosition, filters[filter]);
                ordinaryNodes += waterRayNodeCount;
                gStaticWallHeightIndex.cells = cells;
                waterRayNodeCount = 0;
                f32 actualDistance =
                    find_surface_on_ray(origin, ray, &actual, actualPosition, filters[filter]);
                groupedNodes += waterRayNodeCount;
                EXPECT_EQ(actual, expected);
                CHECK(memcmp(&actualDistance, &expectedDistance, sizeof(f32)) == 0);
                CHECK(memcmp(actualPosition, expectedPosition, sizeof(Vec3f)) == 0);
                if (direction >= 2 || !(filters[filter] & RAYCAST_MARIO_WALL)) {
                    // Pitched rays and camera rays use the ordinary scan.
                    gStaticWallHeightIndex.cells = NULL;
                    u32 nodes = waterRayNodeCount;
                    waterRayNodeCount = 0;
                    find_surface_on_ray(origin, ray, &expected, expectedPosition, filters[filter]);
                    EXPECT_EQ(waterRayNodeCount, nodes);
                }
            }
        }
    }
    CHECK(groupedNodes < ordinaryNodes);
    printf("WORK multistorey wall rays: %u -> %u node visits\n", ordinaryNodes, groupedNodes);
    // A stale generation must never consult the retained terrain pointers.
    gStaticWallHeightIndex.cells = cells;
    gStaticSurfaceGeneration++;
    Vec3f origin = { 256, -2800, 0 }, ray = { -512, 0, 0 }, hit;
    struct Surface *surface;
    waterRayNodeCount = 0;
    find_surface_on_ray(origin, ray, &surface, hit, RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL);
    u32 staleNodes = waterRayNodeCount;
    gStaticWallHeightIndex.cells = NULL;
    waterRayNodeCount = 0;
    find_surface_on_ray(origin, ray, &surface, hit, RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL);
    EXPECT_EQ(waterRayNodeCount, staleNodes);
    return TRUE;
}

static int static_wall_height_equivalence(void) {
    reset();
    for (int i = 0; i < 60; i++) {
        int low = -3000 + (i / 10) * 500;
        wall_at((i % 5) * 20, low, low + 300, i % 9 == 0,
                i % 7 == 0    ? SURFACE_VANISH_CAP_WALLS
                : i % 11 == 0 ? SURFACE_CAMERA_BOUNDARY
                              : SURFACE_DEFAULT);
    }
    use_native_surface_cells();
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticWallHeightIndex.cells != NULL);
    struct StaticWallHeightCell *cells = gStaticWallHeightIndex.cells;
    const s16 flags[] = { 0, COLLISION_FLAG_CAMERA, COLLISION_FLAG_EXCLUDE_DYNAMIC,
                          COLLISION_FLAG_RETURN_FIRST, COLLISION_FLAG_INCLUDE_INTANGIBLE };
    const f32 ys[] = { -3006, -3005, -3004, -2800, -2695, -2694, -1000, 0, 1000, 3000 };
    const f32 xs[] = { -100, -0.25f, 0, 35.25f, 100, 1100 };
    const f32 zs[] = { -0.5f, 0, 500, 1024, 3999 };
    const f32 radii[] = { 24, 50, 110 };
    for (int vanish = 0; vanish < 2; vanish++) {
        mario.flags = vanish ? MARIO_VANISH_CAP : 0;
        for (u32 flag = 0; flag < ARRAY_COUNT(flags); flag++) {
            for (u32 y = 0; y < ARRAY_COUNT(ys); y++) {
                for (u32 x = 0; x < ARRAY_COUNT(xs); x++) {
                    for (u32 z = 0; z < ARRAY_COUNT(zs); z++) {
                        for (u32 radius = 0; radius < ARRAY_COUNT(radii); radius++) {
                            struct WallCollisionData plain = { 0 };
                            plain.x = xs[x];
                            plain.y = ys[y];
                            plain.z = zs[z];
                            plain.radius = radii[radius];
                            struct WallCollisionData grouped = plain;
                            gStaticWallHeightIndex.cells = NULL;
                            gCollisionFlags = flags[flag];
                            s32 hits = find_wall_collisions(&plain);
                            s16 after = gCollisionFlags;
                            gStaticWallHeightIndex.cells = cells;
                            gCollisionFlags = flags[flag];
                            EXPECT_EQ(find_wall_collisions(&grouped), hits);
                            EXPECT_EQ(gCollisionFlags, after);
                            CHECK(grouped.x == plain.x);
                            CHECK(grouped.z == plain.z);
                            EXPECT_EQ(grouped.numWalls, plain.numWalls);
                            for (s32 w = 0; w < plain.numWalls; w++) {
                                EXPECT_EQ(grouped.walls[w], plain.walls[w]);
                            }
                        }
                    }
                }
            }
        }
    }
    return TRUE;
}

static int static_wall_height_lifecycle(void) {
    reset();
    // Short lists allocate nothing, even with enough pool capacity.
    wall_at(0, -500, -100, FALSE, SURFACE_DEFAULT);
    void *end = gCurrStaticSurfacePoolEnd;
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    EXPECT_EQ(gStaticWallHeightIndex.cells, NULL);
    EXPECT_EQ(gCurrStaticSurfacePoolEnd, end);
    for (int i = 1; i < 24; i++) {
        wall_at(0, i < 8 ? -500 : 100, i < 8 ? -100 : 500, FALSE, SURFACE_DEFAULT);
    }
    use_native_surface_cells();
    end = gCurrStaticSurfacePoolEnd;
    memset(end, 0xA5, 0x4000);
    build_static_wall_height_index((u8 *) end - staticIndexPool.bytes);
    EXPECT_EQ(gStaticWallHeightIndex.cells, NULL);
    EXPECT_EQ(gCurrStaticSurfacePoolEnd, end);
    for (int i = 0; i < 0x4000; i++) {
        EXPECT_EQ(((u8 *) end)[i], 0xA5);
    }
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    CHECK(gStaticWallHeightIndex.cells != NULL);
    CHECK((u8 *) gCurrStaticSurfacePoolEnd - (u8 *) end <= STATIC_WALL_HEIGHT_MAX_BYTES);
    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(((u8 *) gCurrStaticSurfacePoolEnd)[i], 0xA5);
    }
    s32 count;
    s32 cell = GET_CELL_COORD(0);
    CHECK(static_wall_height_groups(cell, cell, 0, &count) != NULL);
    EXPECT_EQ(count, 3);
    EXPECT_EQ(static_wall_height_groups(cell, cell, 200, &count), NULL);
    EXPECT_EQ(count, 0);
    CHECK(static_wall_height_groups(cell, cell, -200, &count) != NULL);
    // A late static addition invalidates bounds before touching pooled metadata.
    wall_at(0, -50, 50, FALSE, SURFACE_DEFAULT);
    gStaticSurfaceGeneration++;
    EXPECT_EQ(static_wall_height_groups(cell, cell, 0, &count), NULL);
    EXPECT_EQ(count, 0);
    struct WallCollisionData hit = { .x = 20, .y = 0, .z = 0, .radius = 50 };
    CHECK(find_wall_collisions(&hit) > 0);
    invalidate_static_wall_height_index();
    EXPECT_EQ(gStaticWallHeightIndex.cells, NULL);
    // Reuse the allocation with replacement collision and no old Y bounds.
    reset();
    for (int i = 0; i < 17; i++) {
        wall_at(0, i < 8 ? 600 : 1200, i < 8 ? 900 : 1500, FALSE, SURFACE_DEFAULT);
    }
    use_native_surface_cells();
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    const struct StaticWallHeightGroup *groups = static_wall_height_groups(cell, cell, 0, &count);
    CHECK(groups != NULL);
    EXPECT_EQ(count, 3);
    EXPECT_EQ(groups[2].count, 1);
    CHECK(groups[0].lowerY >= 595);
    CHECK(groups[0].lowerY <= groups[0].first->surface->lowerY);
    CHECK(groups[0].upperY >= groups[0].first->surface->upperY);
    // A dense list at one height retains the ordinary fast scan.
    struct SurfaceNode *head = gStaticSurfacePartition[cell][cell][SPATIAL_PARTITION_WALLS];
    for (struct SurfaceNode *node = head; node != NULL; node = node->next) {
        node->surface->lowerY = 600;
        node->surface->upperY = 900;
    }
    end = gCurrStaticSurfacePoolEnd;
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    EXPECT_EQ(gStaticWallHeightIndex.cells, NULL);
    EXPECT_EQ(gCurrStaticSurfacePoolEnd, end);
    for (int i = 0; head != NULL && i < 8; i++, head = head->next) {
        head->surface->lowerY = -500;
        head->surface->upperY = -100;
    }
    // A terrain whose metadata exceeds the cap retains the original pool end.
    head = gStaticSurfacePartition[cell][cell][SPATIAL_PARTITION_WALLS];
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            gStaticSurfacePartition[z][x][SPATIAL_PARTITION_WALLS] = head;
        }
    }
    end = gCurrStaticSurfacePoolEnd;
    build_static_wall_height_index(sizeof(staticIndexPool.bytes));
    EXPECT_EQ(gStaticWallHeightIndex.cells, NULL);
    EXPECT_EQ(gCurrStaticSurfacePoolEnd, end);
    return TRUE;
}
#endif

static int unlimited_speed(void) {
    for (int master = 0; master < 2; master++) {
        for (int enabled = 0; enabled < 2; enabled++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_NO_SPEED_LIMIT] = enabled;
            reset();
            mario.forwardVel = 300.0f;
            update_swimming_speed(&mario, 30.0f, 1.0f);
            NEAR(mario.forwardVel, master && enabled ? 299.0f : 63.0f);
            reset();
            wall_at(8, -500, 500, 0, SURFACE_DEFAULT);
            struct Surface *wall = &surfaces[surfaceCount - 1];
            sSwimming.rollFrame = 1;
            mario.wall = wall;
            mario.vel[0] = mario.forwardVel = 300.0f;
            mario.faceAngle[1] = 0x4000;
            bounce_swimming_roll(&mario, (Vec3f){ 300, 0, 0 }, WATER_STEP_HIT_WALL, (Vec3f){ 0, 0, 0 });
            if (master && enabled) {
                CHECK(absf(mario.forwardVel) > 300.0f);
            } else {
                NEAR(absf(mario.forwardVel), 64.0f);
            }
            EXPECT_EQ(sSwimming.rollBounceCount, 1);
            if (master && enabled) {
                reset();
                wall_at(128, -500, 500, 0, SURFACE_DEFAULT);
                mario.vel[0] = mario.forwardVel = 600.0f;
                perform_water_step(&mario);
                CHECK(mario.pos[0] < 128.0f);
                CHECK(mario.wall != NULL);
            }
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_NO_SPEED_LIMIT] = 0;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN("swimming.unlimited_speed", unlimited_speed);
#if STATIC_WALL_HEIGHT_GROUPS
    RUN("swimming.static_wall_ray_height_equivalence", static_wall_ray_height_equivalence);
    RUN("swimming.static_wall_height_equivalence", static_wall_height_equivalence);
    RUN("swimming.static_wall_height_lifecycle", static_wall_height_lifecycle);
#endif
    RUN("swimming.settled_flame_movement", settled_flame_movement);
    RUN("swimming.settled_flame_floor_changes", settled_flame_floor_changes);
    RUN("swimming.settled_flame_work", settled_flame_work);
    RUN("swimming.settled_flame_fixed_arena", settled_flame_fixed_arena);
    RUN("swimming.inactive_object_floor_work", inactive_object_floor_work);
    RUN("swimming.inactive_object_floor_refresh", inactive_object_floor_refresh);
    RUN("swimming.inactive_object_dynamic_floor", inactive_object_dynamic_floor);
#if STATIC_SURFACE_INDEX
    RUN("swimming.static_index_lifecycle", static_index_lifecycle);
    RUN("swimming.indexed_collision_equivalence", indexed_collision_equivalence);
    RUN("swimming.jrb_indexed_travel", jrb_indexed_travel);
    RUN("swimming.jrb_grouped_travel", jrb_grouped_travel);
#if DYNAMIC_SURFACE_INDEX
    RUN("swimming.jrb_dynamic_index_travel", jrb_dynamic_index_travel);
#endif
#endif
    RUN("swimming.wall_candidate_cache_equivalence", wall_candidate_cache_equivalence);
    RUN("swimming.wall_height_filter", swimming_wall_height_filter);
    RUN("swimming.water_top_bottom_scan", water_top_bottom_scan);
    RUN("swimming.static_floor_cache_equivalence", static_floor_cache_equivalence);
    RUN("swimming.static_floor_cache_lifecycle", static_floor_cache_lifecycle);
    RUN("swimming.static_floor_cache_work", static_floor_cache_work);
    RUN("swimming.lab_backward_long_jump_stairs", lab_backward_long_jump_stairs);
    RUN("swimming.ttm_box_cave_departure", ttm_box_cave_departure);
    RUN("swimming.lab_bridge_edge_landings", lab_bridge_edge_landings);
    RUN("swimming.castle_cliff_support", castle_cliff_support);
    RUN("swimming.castle_waterfall_slide_exit", castle_waterfall_slide_exit);
    RUN("swimming.castle_outdoor_barriers", castle_outdoor_barriers);
    RUN("swimming.air_thin_slope_top", air_thin_slope_top);
    RUN("swimming.air_top_contact_obstructions", air_top_contact_obstructions);
    RUN("swimming.ttc_stopped_platform_entry", ttc_stopped_platform_entry);
    RUN("swimming.air_riser_entry_boundaries", air_riser_entry_boundaries);
    RUN("swimming.bowser_rising_tilt_landing", bowser_rising_tilt_landing);
    RUN("swimming.air_floor_grounded_dives", air_floor_grounded_dives);
    RUN("swimming.air_floor_lab_ramp_sides", air_floor_lab_ramp_sides);
    RUN("swimming.air_floor_side_filters_and_travel", air_floor_side_filters_and_travel);
    RUN("swimming.air_floor_crest_takeoff", air_floor_crest_takeoff);
    RUN("swimming.air_floor_bob_takeoffs", air_floor_bob_takeoffs);
    RUN("swimming.air_floor_support_boundaries", air_floor_support_boundaries);
    RUN("swimming.air_floor_grounded_seam", air_floor_grounded_seam);
    RUN("swimming.platform_collision", platform_collision);
    RUN("swimming.stunned_lost_support", stunned_lost_support);
    RUN("swimming.platform_support_query", platform_support_query);
    RUN("swimming.blocked_momentum", blocked_swim_momentum);
    RUN("swimming.jrb_underside_travel", jrb_underside_travel);
    RUN("swimming.jrb_ship_contact_work", jrb_ship_contact_work);
    RUN("swimming.jrb_ship_contact_reuse", jrb_ship_contact_reuse);
    RUN("swimming.contact_reuse_finite_edges", contact_reuse_finite_edges);
    RUN("swimming.jrb_ship_ray_bounds", jrb_ship_ray_bounds);
    RUN("swimming.movement_ray_bounds_equivalence", movement_ray_bounds_equivalence);
    RUN("swimming.water_geometry_handoff", water_geometry_handoff);
    RUN("swimming.closed_corner", closed_swimming_corner);
    RUN("swimming.stunned_motion", stunned_motion);
    RUN("swimming.stunned_snufit_clearance", stunned_snufit_clearance);
    RUN("swimming.stunned_mole_clearance", stunned_mole_clearance);
    RUN("swimming.stunned_mole_support_changes", stunned_mole_support_changes);
    RUN("swimming.stunned_mole_hazards", stunned_mole_hazards);
    RUN("swimming.stunned_mole_native_resume", stunned_mole_native_resume);
    RUN("swimming.stunned_fixed_triangle_cast", stunned_fixed_triangle_cast);
    RUN("swimming.stunned_finite_edge_clearance", stunned_finite_edge_clearance);
    RUN("swimming.stunned_simplex_distance", stunned_simplex_distance);
    RUN("swimming.stunned_simplex_progress", stunned_simplex_progress);
    RUN("swimming.stunned_contact_release", stunned_contact_release);
    RUN("swimming.stunned_wall_lifecycle", stunned_wall_lifecycle);
    RUN("swimming.stunned_sculpture_lifecycle", stunned_sculpture_lifecycle);
    RUN("swimming.stunned_court_lifecycle", stunned_court_lifecycle);
    RUN("swimming.stunned_candidate_work", stunned_candidate_work);
    RUN("swimming.stunned_candidate_overflow", stunned_candidate_overflow);
    RUN("swimming.stunned_distant_faces", stunned_distant_faces);
    RUN("swimming.stunned_thin_geometry", stunned_thin_geometry);
    RUN("swimming.stunned_water_and_filters", stunned_water_and_filters);
    RUN("swimming.free", free_movement);
    RUN("swimming.fast_wall", fast_wall);
    RUN("swimming.roll_wall_rebound", roll_wall_rebound);
    RUN("swimming.roll_floor_rebound", roll_floor_rebound);
    RUN("swimming.thin_wall", thin_wall_crossing);
    RUN("swimming.fast_floor", fast_floor);
    RUN("swimming.steep_slope", steep_slope);
    RUN("swimming.ceiling", ceiling);
    RUN("swimming.body_edge_crossing", body_edge_crossing);
    RUN("swimming.thin_ceiling", thin_ceiling);
    RUN("swimming.currents", currents);
    RUN("swimming.thicc_water_weight", thicc_water_weight);
    RUN("swimming.surface_exit", surface_exit);
    RUN("swimming.stroke_wall", stroke_wall);
    RUN("swimming.stroke_glance", stroke_glance);
    RUN("swimming.sustained_wall_glance", sustained_wall_glance);
    RUN("swimming.turning_wall_speed", turning_wall_speed);
    RUN("swimming.wall_slide_controls", wall_slide_controls);
    RUN("swimming.jrb_wall_pitch", jrb_wall_pitch);
    RUN("swimming.filters", filters);
    RUN("swimming.wall_slide_reversal", wall_slide_reversal);
    RUN("swimming.bbh_wall_slide_release", bbh_wall_slide_release);
    RUN("swimming.wall_contact_work", wall_contact_work);
    RUN("swimming.floor_contact_retained", floor_contact_retained);
    RUN("swimming.no_floor", no_floor);
    RUN("swimming.corner", corner);
    RUN("swimming.repeated_slope", repeated_slope);
    RUN("swimming.thin_floor", thin_floor);
    RUN("swimming.surface_under_ceiling", surface_under_ceiling);
    RUN("swimming.cell_boundary", cell_boundary);
    RUN("swimming.cancelled_stroke", cancelled_stroke);
    RUN("swimming.idle_and_reverse", idle_and_reverse);
    RUN("swimming.downward_slope_contact", downward_slope_contact);
    RUN("swimming.downward_slope_distance", downward_slope_distance);
    RUN("swimming.downhill_seam", downhill_seam);
    RUN("swimming.overlap_escape", overlap_escape);
    RUN("swimming.overlap_escape_progress", overlap_escape_progress);
    RUN("swimming.overlap_escape_obstacles", overlap_escape_obstacles);
    RUN("swimming.wall_separation_overlap", wall_separation_overlap);
    RUN("swimming.ceiling_seam", ceiling_seam);
    RUN("swimming.overlap_recovery_bounds", overlap_recovery_bounds);
    RUN("swimming.wall_separation_crossing", wall_separation_crossing);
    RUN("swimming.precise_filters_and_flags", precise_filters_and_flags);
    RUN("swimming.precise_query_cache_contract", precise_query_cache_contract);
    RUN("swimming.bbh_angled_wall_query_reuse", bbh_angled_wall_query_reuse);
    RUN("swimming.shared_angled_wall_query_reuse", shared_angled_wall_query_reuse);
    RUN("swimming.jrb_narrow_gap_query_work", jrb_narrow_gap_query_work);
    RUN("swimming.air_floor_edge_crossings", air_floor_edge_crossings);
    RUN("swimming.bitfs_rising_lift", bitfs_rising_lift);
    RUN("swimming.shallow_pool_edge_entry", shallow_pool_edge_entry);
    RUN("swimming.air_floor_lab_dives", air_floor_lab_dives);
    RUN("swimming.wall_seam_crossing", wall_seam_crossing);
    RUN("swimming.wall_seam_release", wall_seam_release);
    RUN("swimming.air_floor_sweep_boundaries", air_floor_sweep_boundaries);
    RUN("swimming.air_floor_edge_flags", air_floor_edge_flags);
    RUN("swimming.moving_floor_recovery", moving_floor_recovery);
    RUN("swimming.jrb_seam_travel", jrb_seam_travel);
    RUN("swimming.jrb_object_rock_escape", jrb_object_rock_escape);
    RUN("swimming.jrb_downward_approaches", jrb_downward_approaches);
    RUN("swimming.downward_controls", downward_controls);
    RUN("swimming.jrb_downward_controls", jrb_downward_controls);
    RUN("swimming.slope_floor_assist", slope_floor_assist);
    RUN("swimming.contact_remainder", contact_remainder);
    RUN("swimming.vanish_sculpture_faces", vanish_sculpture_faces);
    RUN("swimming.vanish_swimming_faces", vanish_swimming_faces);
    RUN("swimming.ddd_cage_escape", ddd_cage_escape);
    return test_finish();
}
