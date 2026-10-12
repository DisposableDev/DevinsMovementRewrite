#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
typedef uintptr_t BehaviorScript;
typedef u16 ModelID16;
typedef u32 ModelID32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef f32 Mat4[4][4];
#define IS_64_BIT 0
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define SET_BPARAM2(p, v) ((p) = ((p) & 0xFF00FFFFu) | ((u32) (v) << 16))
#include "object_fields.h"
#include "particle_definitions.inc"

#define MARIO_VANISH_CAP (1 << 1)
struct MarioState {
    u32 flags;
    Vec3s faceAngle;
    Vec3f pos;
    struct Object *marioObj;
};
static struct MarioState sMarioState, *gMarioState = &sMarioState;
static s32 sHeadUnderwater;
static s32 sSecretsBlocked;
static s32 game_options_secrets_blocked(void) {
    return sSecretsBlocked;
}
static s32 mario_is_head_underwater(UNUSED struct MarioState *m) {
    return sHeadUnderwater;
}

static struct Object gObjectPool[OBJECT_POOL_CAPACITY];
static struct ObjectNode listHeads[NUM_OBJ_LISTS], gFreeObjectList;
static struct ObjectNode *gObjectLists = listHeads;
static struct Object *gCurrentObject, *gMarioObject;
#define o gCurrentObject
static struct GraphNode gObjParentGraphNode;
static void *gLoadedGraphNodes[1024];
static Vec3f gVec3fZero;
static Vec3s gVec3sZero;
static u32 gTimeStopState;
static u32 gGlobalTimer;
static const BehaviorScript *gCurBhvCommand;
static const void *segmentedAlias;
static const BehaviorScript *segmentedTarget;
static int stoppedSounds, animations, updates, failures;
static f32 suppliedWaterLevel;
static s32 waterQueries;
static s32 floorQueries, suppliedFloorPresent;
static f32 suppliedFloorHeight;
// The policy harness exercises the full-pool fallback. Compact storage has
// its own lifecycle fixture and deliberately reports full capacity here.
static void compact_explosion_bubbles_reset(void) {
}
static s32 compact_explosion_bubble_spawn(UNUSED struct Object *parent) {
    return FALSE;
}
static const void *segmented_to_virtual(const void *p) {
    return p == segmentedAlias ? segmentedTarget : p;
}
#include "particle_policy.inc"

// Supplied engine services: graphics relinking, model initialization, random
// draws, water height, audio cleanup and the per-object behavior callback.
static void geo_remove_child(UNUSED struct GraphNode *node) {
}
static void geo_add_child(UNUSED struct GraphNode *parent, UNUSED struct GraphNode *node) {
}
static void stop_sounds_from_source(UNUSED f32 *pos) {
    stoppedSounds++;
}
static void mtxf_identity(Mat4 m) {
    memset(m, 0, sizeof(Mat4));
    for (int i = 0; i < 4; i++) {
        m[i][i] = 1;
    }
}
static void vec3_same(Vec3f v, f32 value) {
    for (int i = 0; i < 3; i++) {
        v[i] = value;
    }
}
static ModelID32 physics_lab_actor_model(UNUSED const BehaviorScript *b, ModelID32 model) {
    return model;
}
static void geo_obj_init(struct GraphNodeObject *node, void *model, Vec3f pos, Vec3s angle) {
    node->sharedChild = model;
    node->node.flags |= GRAPH_RENDER_ACTIVE;
    memcpy(node->pos, pos, sizeof(Vec3f));
    memcpy(node->angle, angle, sizeof(Vec3s));
    for (int i = 0; i < 3; i++) {
        node->scale[i] = 1;
    }
}
static void obj_copy_pos_and_angle(struct Object *dst, struct Object *src) {
    dst->oPosX = src->oPosX;
    dst->oPosY = src->oPosY;
    dst->oPosZ = src->oPosZ;
    dst->oFaceAngleYaw = src->oFaceAngleYaw;
    dst->oMoveAngleYaw = src->oMoveAngleYaw;
}
static void obj_copy_pos(struct Object *dst, struct Object *src) {
    dst->oPosX = src->oPosX;
    dst->oPosY = src->oPosY;
    dst->oPosZ = src->oPosZ;
}
static void obj_scale(struct Object *obj, f32 scale) {
    for (int i = 0; i < 3; i++) {
        obj->header.gfx.scale[i] = scale;
    }
}
static f32 random_float(void) {
    return 0.25f;
}
static u16 random_u16(void) {
    return 0;
}
static f32 random_f32_around_zero(UNUSED f32 range) {
    return 0;
}
static f32 sins(s16 a) {
    return sinf(a * 3.14159265359f / 32768.0f);
}
static f32 coss(s16 a) {
    return cosf(a * 3.14159265359f / 32768.0f);
}
static s32 find_water_level(UNUSED f32 x, UNUSED f32 z) {
    waterQueries++;
    return suppliedWaterLevel;
}
struct Surface {
    s32 unused;
};
static f32 find_floor(UNUSED f32 x, UNUSED f32 y, UNUSED f32 z, struct Surface **floor) {
    static struct Surface surface;
    floorQueries++;
    *floor = suppliedFloorPresent ? &surface : NULL;
    return suppliedFloorPresent ? suppliedFloorHeight : FLOOR_LOWER_LIMIT;
}
static const void *blue_fish_seg3_anims_0301C2B0;
static void obj_init_animation_with_sound(UNUSED struct Object *obj, UNUSED const void *anims,
                                          UNUSED s32 index) {
    animations++;
}
static s32 cur_obj_has_model(UNUSED ModelID32 model) {
    return FALSE;
}
static void cur_obj_scale(f32 scale) {
    obj_scale(o, scale);
}
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}
static void update_object_with_ground_twirl_stun(void) {
    updates++;
}
static void obj_and_int(struct Object *obj, u8 field, s32 mask) {
    obj->rawData.asU32[field] &= mask;
}
#include "particle_params.inc"
// Platform motion is covered by its integration suite; allocator tests observe native updates.
// Persistent collision lifecycle is covered by its own fixture.
static void persistent_surfaces_forget(struct Object *obj) {
    (void) obj;
}
static void persistent_surfaces_reset(void) {
}
static void object_platform_motion_allocated(struct Object *obj) {
    (void) obj;
}
static void object_platform_motion_freed(struct Object *obj) {
    (void) obj;
}
static void object_platform_motion_reset(struct Object *obj) {
    (void) obj;
}
static void update_object_with_platform_motion(void) {
    update_object_with_ground_twirl_stun();
}
static struct Object *forgottenAttackObject;
static void mario_forget_attack_object(struct Object *obj) {
    forgottenAttackObject = obj;
}
static void mario_forget_pickup_object(struct Object *obj) {
    (void) obj;
}

// Observe the production splash callback's graphics commands; rasterization is not simulated.
typedef struct {
    u8 red, green, blue, alpha;
    s32 ended;
} Gfx;
struct GraphNodeGenerated {
    struct {
        struct GraphNode node;
    } fnNode;
};
#define GEO_CONTEXT_RENDER 1
#define LAYER_TRANSPARENT 5
#define SET_GRAPH_NODE_LAYER(flags, layer) ((flags) = (layer))
#define gCurGraphNodeObjectNode gCurrentObject
#define gDPSetEnvColor(dl, r, g, b, a) (*(dl) = (Gfx){ (r), (g), (b), (a), 0 })
#define gSPEndDisplayList(dl) ((dl)->ended = 1)
static Gfx sSplashCommands[2];
static s32 sNoDisplayMemory;
static Gfx *alloc_display_list(UNUSED size_t size) {
    return sNoDisplayMemory ? NULL : sSplashCommands;
}
static void cur_obj_compute_vel_xz(void) {
    o->oVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);
}
static void cur_obj_move_using_vel_and_gravity(void) {
    o->oPosX += o->oVelX;
    o->oPosY += o->oVelY;
    o->oPosZ += o->oVelZ;
    o->oVelY += o->oGravity;
}
static void cur_obj_apply_drag_xz(UNUSED f32 drag) {
}
// Statistics and active Mario attack queries are outside this particle-budget
// fixture; its supplied Mario is neutral and the gameplay suites cover attacks.
static void player_stats_object_reset(UNUSED struct Object *obj) {
}
static s32 mario_ground_twirl_is_active(UNUSED struct MarioState *m) {
    return FALSE;
}
static s32 mario_air_twirl_is_active(UNUSED struct MarioState *m) {
    return FALSE;
}
static s32 mario_swimming_roll_is_active(void) {
    return FALSE;
}

#include "particle_functions.inc"

#define CHECK(c)                                                                                       \
    do {                                                                                               \
        if (!(c)) {                                                                                    \
            fprintf(stderr, "FAIL %s:%d: %s\n", __func__, __LINE__, #c);                               \
            failures++;                                                                                \
            return;                                                                                    \
        }                                                                                              \
    } while (0)

static void reset(void) {
    memset(gObjectPool, 0, sizeof(gObjectPool));
    init_free_object_list();
    clear_object_lists(listHeads);
    gCurrentObject = NULL;
    gMarioObject = NULL;
    gTimeStopState = 0;
    memset(&sMarioState, 0, sizeof(sMarioState));
    sHeadUnderwater = FALSE;
    sSecretsBlocked = FALSE;
    segmentedAlias = NULL;
    segmentedTarget = NULL;
    stoppedSounds = animations = updates = 0;
    suppliedWaterLevel = 100;
    waterQueries = 0;
    floorQueries = 0;
    suppliedFloorPresent = FALSE;
    suppliedFloorHeight = 0;
    sNoDisplayMemory = FALSE;
    gMarioObject = create_object(bhvMario);
    gCurrentObject = gMarioObject;
    sMarioState.marioObj = gMarioObject;
}
static void fill_to_free(int freeSlots) {
    while (particle_budget_free_slots() > freeSlots) {
        create_object(bhvGoomba);
    }
}
static int free_list_count(void) {
    int count = 0;
    for (struct ObjectNode *p = gFreeObjectList.next; p; p = p->next) {
        assert(++count <= OBJECT_POOL_CAPACITY);
    }
    return count;
}
static int behavior_count(const BehaviorScript *behavior) {
    int count = 0;
    for (int list = 0; list < NUM_OBJ_LISTS; list++) {
        struct ObjectNode *head = &listHeads[list];
        for (struct ObjectNode *p = head->next; p != head; p = p->next) {
            if (((struct Object *) p)->behavior == behavior) {
                count++;
            }
        }
    }
    return count;
}
static struct Object *visual(const BehaviorScript *behavior, enum ParticlePriority priority) {
    struct Object *obj = try_spawn_particle(gMarioObject, MODEL_MIST, behavior, priority);
    assert(obj);
    return obj;
}

static void test_live_count_and_reset(void) {
    CHECK(OBJECT_POOL_CAPACITY == 240);
    reset();
    CHECK(particle_budget_free_slots() == OBJECT_POOL_CAPACITY - 1);
    for (int cycle = 0; cycle < 3; cycle++) {
        struct Object *p = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
        CHECK(free_list_count() == particle_budget_free_slots());
        unload_object(p);
        CHECK(particle_budget_free_slots() == OBJECT_POOL_CAPACITY - 1);
        struct Object *enemy = create_object(bhvGoomba);
        CHECK(enemy == p);
        CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
        unload_object(enemy);
        CHECK(free_list_count() == OBJECT_POOL_CAPACITY - 1);
    }
    reset();
    CHECK(free_list_count() == OBJECT_POOL_CAPACITY - 1);
}
static void test_order_and_ties(void) {
    reset();
    struct Object *bubble = visual(bhvBobombExplosionBubble, PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    struct Object *older = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION);
    struct Object *newer = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == older);
    unload_object(older);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == newer);
    unload_object(newer);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == bubble);
    // Native timers cannot override actual creation order.
    bubble->oTimer = 1000;
    struct Object *younger = visual(bhvBobombExplosionBubble, PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    younger->oTimer = 0;
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == bubble);
    unload_object(bubble);
    unload_object(younger);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    // Reusing a lower pool slot must not preserve its previous particle's age.
    reset();
    struct Object *reusedSlot = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_DEBRIS);
    struct Object *survivor = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_DEBRIS);
    unload_object(reusedSlot);
    struct Object *replacement = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_DEBRIS);
    CHECK(replacement == reusedSlot);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == survivor);
    // Reverse registration and reprioritization preserve pool-order birth ties.
    reset();
    older = create_object(bhvBreakBoxTriangle);
    newer = create_object(bhvBreakBoxTriangle);
    particle_budget_set_priority(newer, PARTICLE_PRIORITY_DEBRIS);
    particle_budget_set_priority(older, PARTICLE_PRIORITY_DEBRIS);
    particle_budget_set_priority(older, PARTICLE_PRIORITY_CELEBRATION);
    particle_budget_set_priority(older, PARTICLE_PRIORITY_DEBRIS);
    sParticleBirthOrder[newer - gObjectPool] = sParticleBirthOrder[older - gObjectPool];
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == older);
    unload_object(older);
    struct Object *enemy = create_object(bhvGoomba);
    CHECK(enemy == older);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == newer);
}
static void test_protected_and_segmented(void) {
    reset();
    struct Object outsider = { 0 };
    outsider.behavior = bhvBreakBoxTriangle;
    particle_budget_set_priority(NULL, PARTICLE_PRIORITY_DEBRIS);
    particle_budget_init_priority(NULL, PARTICLE_PRIORITY_DEBRIS);
    particle_budget_set_priority(&outsider, PARTICLE_PRIORITY_DEBRIS);
    particle_budget_init_priority(&outsider, PARTICLE_PRIORITY_DEBRIS);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    const BehaviorScript *protected[] = { bhvMario,
                                          bhvGoomba,
                                          bhvExplosion,
                                          bhvKoopaShellFlame,
                                          bhvWaterAirBubble,
                                          bhvStrongWindParticle,
                                          bhvBubbleParticleSpawner,
                                          bhvIdleWaterWave,
                                          bhvSparkleSpawn };
    for (unsigned i = 0; i < sizeof(protected) / sizeof(protected[0]); i++) {
        struct Object *p = create_object(protected[i]);
        particle_budget_set_priority(p, PARTICLE_PRIORITY_CELEBRATION);
        CHECK(particle_budget_priority(protected[i]) == PARTICLE_PRIORITY_PROTECTED);
    }
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    struct Object *hazard = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    hazard->oInteractType = INTERACT_DAMAGE;
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    hazard->oInteractType = 0;
    hazard->collisionData = hazard;
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    unload_object(hazard);
    segmentedAlias = (const void *) (uintptr_t) 0x13001000;
    segmentedTarget = bhvBobombExplosionBubble;
    CHECK(particle_budget_priority(segmentedAlias) == PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    struct Object *p = try_spawn_particle(gMarioObject, MODEL_MIST, segmentedAlias,
                                          PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    CHECK(p != NULL && p->behavior == bhvBobombExplosionBubble);
}
static void test_reserve_and_replacement(void) {
    reset();
    fill_to_free(31);
    struct Object *low = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION);
    CHECK(particle_budget_free_slots() == 30);
    CHECK(
        try_spawn_particle(gMarioObject, MODEL_MIST, bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION)
        == NULL);
    struct Object *high =
        try_spawn_particle(gMarioObject, MODEL_BUBBLE, bhvSmallWaterWave, PARTICLE_PRIORITY_FEEDBACK);
    CHECK(high == low);
    CHECK(particle_budget_free_slots() == 30);
    CHECK(try_spawn_particle(gMarioObject, MODEL_BUBBLE, bhvSmallWaterWave, PARTICLE_PRIORITY_FEEDBACK)
          == NULL);
    CHECK(high->behavior == bhvSmallWaterWave);
    // Required objects may use the reserve and then reclaim feedback at exhaustion.
    fill_to_free(0);
    struct Object *enemy = spawn_object(gMarioObject, MODEL_NONE, bhvGoomba);
    CHECK(enemy == high && particle_budget_free_slots() == 0);
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
    CHECK(try_spawn_particle(gMarioObject, MODEL_MIST, bhvWhitePuffExplosion, PARTICLE_PRIORITY_MIST)
          == NULL);
}
static void test_active_parent_and_list_safety(void) {
    reset();
    struct Object *current = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION);
    struct Object *parent = visual(bhvBreakBoxTriangle, PARTICLE_PRIORITY_CELEBRATION);
    struct Object *next = visual(bhvBobombExplosionBubble, PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    gCurrentObject = current;
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, parent) == next);
    fill_to_free(0);
    struct Object *child = spawn_object(parent, MODEL_NONE, bhvGoomba);
    CHECK(child == next);
    CHECK(child->parentObj == parent);
    CHECK(current->behavior == bhvBreakBoxTriangle && parent->behavior == bhvBreakBoxTriangle);
    CHECK(current->header.next == &parent->header);
    parent->respawnInfo = parent;
    CHECK(particle_budget_select(PARTICLE_PRIORITY_REQUIRED, NULL) == NULL);
}
static void test_inactive_reclaim(void) {
    reset();
    struct Object *feedback = visual(bhvSmallWaterWave, PARTICLE_PRIORITY_FEEDBACK);
    fill_to_free(30);
    feedback->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    struct Object *debris =
        try_spawn_particle(gMarioObject, MODEL_MIST, bhvBreakBoxTriangle, PARTICLE_PRIORITY_DEBRIS);
    CHECK(debris == feedback && particle_budget_free_slots() == 30);
    CHECK(stoppedSounds == 1);
}
static void test_bursts_and_projection(void) {
    reset();
    fill_to_free(72);
    spawn_triangle_break_particles_with_priority(60, MODEL_CARTOON_STAR, 0.3f, 0,
                                                 PARTICLE_PRIORITY_CELEBRATION);
    CHECK(behavior_count(bhvBreakBoxTriangle) == 12);
    CHECK(particle_budget_free_slots() == 60);
    struct SpawnParticlesInfo info = { .count = 20, .model = MODEL_MIST, .sizeBase = 30 };
    cur_obj_spawn_particles_with_priority(&info, PARTICLE_PRIORITY_MIST);
    CHECK(behavior_count(bhvWhitePuffExplosion) == 6);
    CHECK(particle_budget_free_slots() == 54);
    bhv_shallow_water_splash_spawn_droplets();
    CHECK(behavior_count(bhvWaterDroplet) == 6);
    CHECK(free_list_count() == particle_budget_free_slots());
    // A burst that would cross the soft threshold is reduced before it starts.
    reset();
    fill_to_free(131);
    spawn_triangle_break_particles_with_priority(60, MODEL_CARTOON_STAR, 0.3f, 0,
                                                 PARTICLE_PRIORITY_CELEBRATION);
    CHECK(behavior_count(bhvBreakBoxTriangle) == 12);
    reset();
    fill_to_free(132);
    spawn_triangle_break_particles_with_priority(60, MODEL_CARTOON_STAR, 0.3f, 0,
                                                 PARTICLE_PRIORITY_CELEBRATION);
    CHECK(behavior_count(bhvBreakBoxTriangle) == 60 && particle_budget_free_slots() == 72);
    reset();
    fill_to_free(31);
    spawn_triangle_break_particles(30, MODEL_DIRT_ANIMATION, 3, 0);
    CHECK(behavior_count(bhvBreakBoxTriangle) == 1 && particle_budget_free_slots() == 30);
    CHECK(behavior_count(bhvGoomba) == OBJECT_POOL_CAPACITY - 32);
}
static void test_underwater_explosion(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        struct Object *explosion = spawn_object(gMarioObject, MODEL_EXPLOSION, bhvExplosion);
        fill_to_free(72);
        gCurrentObject = explosion;
        explosion->oTimer = 9;
        explosion->oOpacity = 255;
        bhv_explosion_loop();
        CHECK(behavior_count(bhvBobombExplosionBubble) == 8);
        CHECK(particle_budget_free_slots() == 64);
        CHECK(explosion->activeFlags == ACTIVE_FLAG_DEACTIVATED);
        struct Object *bubble = (struct Object *) listHeads[OBJ_LIST_DEFAULT].next;
        CHECK(bubble->behavior == bhvBobombExplosionBubble);
        CHECK(!(bubble->activeFlags & ACTIVE_FLAG_UNIMPORTANT));
    }
    reset();
    struct Object *explosion = spawn_object(gMarioObject, MODEL_EXPLOSION, bhvExplosion);
    gCurrentObject = explosion;
    explosion->oTimer = 9;
    explosion->oOpacity = 255;
    suppliedWaterLevel = -100;
    bhv_explosion_loop();
    CHECK(behavior_count(bhvBobombExplosionBubble) == 0);
    CHECK(behavior_count(bhvBobombBullyDeathSmoke) == 1);
}
static void clear_command(struct Object *owner, u32 flags) {
    static BehaviorScript commands[2];
    commands[0] = (uintptr_t) (&gMarioObject->oActiveParticleFlags - gMarioObject->rawData.asU32) << 16;
    commands[1] = flags;
    gCurrentObject = owner;
    gCurBhvCommand = commands;
    bhv_cmd_parent_bit_clear();
}
static void test_flag_ownership_and_recovery(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        spawn_particle(ACTIVE_PARTICLE_WAVE_TRAIL, MODEL_WAVE_TRAIL, bhvWaveTrail);
        struct Object *old = (struct Object *) listHeads[OBJ_LIST_UNIMPORTANT].next;
        CHECK(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_WAVE_TRAIL);
        clear_command(old, ACTIVE_PARTICLE_WAVE_TRAIL);
        gCurrentObject = gMarioObject;
        spawn_particle(ACTIVE_PARTICLE_WAVE_TRAIL, MODEL_WAVE_TRAIL, bhvWaveTrail);
        struct Object *newer = (struct Object *) listHeads[OBJ_LIST_UNIMPORTANT].prev;
        CHECK(newer != old);
        clear_command(old, ACTIVE_PARTICLE_WAVE_TRAIL);
        CHECK(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_WAVE_TRAIL);
        unload_object(old);
        CHECK(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_WAVE_TRAIL);
        unload_object(newer);
        CHECK(!(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_WAVE_TRAIL));
        gCurrentObject = gMarioObject;
        spawn_particle(ACTIVE_PARTICLE_WAVE_TRAIL, MODEL_WAVE_TRAIL, bhvWaveTrail);
        CHECK(behavior_count(bhvWaveTrail) == 1);
    }
    reset();
    fill_to_free(30);
    spawn_particle(ACTIVE_PARTICLE_BUBBLE, MODEL_NONE, bhvBubbleParticleSpawner);
    CHECK(!(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_BUBBLE));
    CHECK(behavior_count(bhvBubbleParticleSpawner) == 0);
    struct Object *enemy = (struct Object *) listHeads[OBJ_LIST_PUSHABLE].next;
    unload_object(enemy);
    spawn_particle(ACTIVE_PARTICLE_BUBBLE, MODEL_NONE, bhvBubbleParticleSpawner);
    CHECK(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_BUBBLE);
    struct Object *owner = (struct Object *) listHeads[OBJ_LIST_DEFAULT].next;
    CHECK(owner->behavior == bhvBubbleParticleSpawner);
    clear_command(owner, ACTIVE_PARTICLE_BUBBLE);
    CHECK(!(gMarioObject->oActiveParticleFlags & ACTIVE_PARTICLE_BUBBLE));
}
static void test_script_children_and_time_stop(void) {
    reset();
    struct Object *owner = spawn_object(gMarioObject, MODEL_NONE, bhvBubbleParticleSpawner);
    static BehaviorScript commands[3];
    commands[0] = 0;
    commands[1] = MODEL_BUBBLE;
    commands[2] = (uintptr_t) bhvSmallWaterWave;
    gCurrentObject = owner;
    gCurBhvCommand = commands;
    CHECK(bhv_cmd_spawn_child() == BHV_PROC_CONTINUE);
    CHECK(behavior_count(bhvSmallWaterWave) == 1);
    struct Object *bubble = visual(bhvBobombExplosionBubble, PARTICLE_PRIORITY_EXPLOSION_BUBBLE);
    (void) bubble;
    gTimeStopState = TIME_STOP_ACTIVE;
    updates = 0;
    update_objects_during_time_stop(&listHeads[OBJ_LIST_DEFAULT], listHeads[OBJ_LIST_DEFAULT].next);
    CHECK(updates == 0);
    update_objects_during_time_stop(&listHeads[OBJ_LIST_UNIMPORTANT],
                                    listHeads[OBJ_LIST_UNIMPORTANT].next);
    CHECK(updates == 1);
    gTimeStopState = TIME_STOP_ACTIVE | TIME_STOP_ALL_OBJECTS;
    updates = 0;
    update_objects_during_time_stop(&listHeads[OBJ_LIST_UNIMPORTANT],
                                    listHeads[OBJ_LIST_UNIMPORTANT].next);
    CHECK(updates == 0);
    // Optional child failure must still advance the behavior command.
    reset();
    owner = spawn_object(gMarioObject, MODEL_NONE, bhvBubbleParticleSpawner);
    fill_to_free(30);
    gCurrentObject = owner;
    gCurBhvCommand = commands;
    CHECK(bhv_cmd_spawn_child() == BHV_PROC_CONTINUE && gCurBhvCommand == commands + 3);
    CHECK(behavior_count(bhvSmallWaterWave) == 0);
}
static void test_droplet_secondary_and_null_fish(void) {
    reset();
    struct Object *drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    fill_to_free(30);
    gCurrentObject = drop;
    drop->oVelY = -10;
    drop->oPosY = 0;
    bhv_water_droplet_loop();
    CHECK(drop->activeFlags == ACTIVE_FLAG_DEACTIVATED);
    CHECK(behavior_count(bhvWaterDropletSplash) == 0);
    reset();
    fill_to_free(30);
    bhv_shallow_water_splash_init();
    CHECK(animations == 0);
}
static void test_rising_droplet_water_queries(void) {
    reset();
    struct Object *drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    gCurrentObject = drop;
    drop->oPosY = 200;
    drop->oVelY = 20;
    drop->oTimer = 0;
    bhv_water_droplet_loop();
    CHECK(waterQueries == 1 && drop->oVelY == 16);
    for (s32 timer = 1; timer <= 3; timer++) {
        drop->oTimer = timer;
        bhv_water_droplet_loop();
    }
    CHECK(waterQueries == 1 && drop->oVelY == 4);
    drop->oTimer = 4;
    bhv_water_droplet_loop();
    CHECK(waterQueries == 1 && drop->oVelY == 0);
    drop->oTimer = 5;
    bhv_water_droplet_loop();
    CHECK(waterQueries == 2 && drop->oVelY == -4);
}

static void test_shallow_water_fish_eligibility(void) {
    for (s32 blocked = FALSE; blocked <= TRUE; blocked++) {
        reset();
        sSecretsBlocked = blocked;
        // The supplied random draw always selects the rare fish event.
        bhv_shallow_water_splash_init();
        CHECK(animations == (blocked ? 0 : 1));
        CHECK(behavior_count(bhvWaterDroplet) == (blocked ? 0 : 1));

        reset();
        sSecretsBlocked = blocked;
        bhv_shallow_water_splash_spawn_droplets();
        CHECK(behavior_count(bhvWaterDroplet) == 18);
        CHECK(animations == 0);
    }
}

static void test_slide_kick_liquid_bursts(void) {
    static s32 lavaModel, coldModel;
    gLoadedGraphNodes[MODEL_LAVA_DROPLET] = &lavaModel;
    gLoadedGraphNodes[MODEL_COLD_WATER_DROPLET] = &coldModel;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int material = LIQUID_PARTICLE_COLD_WATER; material <= LIQUID_PARTICLE_LAVA; material++) {
            reset();
            // Mario's action position can be ahead of his object's last update.
            gMarioObject->oPosX = -100;
            gMarioObject->oPosY = -50;
            gMarioObject->oPosZ = -200;
            sMarioState.pos[0] = 300;
            sMarioState.pos[1] = 500;
            sMarioState.pos[2] = 700;
            spawn_slide_kick_liquid_particles(&sMarioState, 123, material);
            CHECK(behavior_count(bhvWaterDroplet) == 18);
            CHECK(waterQueries == 0);
            for (int i = 0; i < OBJECT_POOL_CAPACITY; i++) {
                struct Object *drop = &gObjectPool[i];
                if (drop->activeFlags == ACTIVE_FLAG_DEACTIVATED || drop->behavior != bhvWaterDroplet) {
                    continue;
                }
                CHECK(drop->oPosX == 300 && drop->oPosY == 123 && drop->oPosZ == 700);
                CHECK(drop->parentObj == gMarioObject);
                CHECK(drop->oAction == WATER_DROPLET_ACT_SLIDE_KICK);
                CHECK(drop->oBehParams2ndByte == material);
                CHECK(drop->oForwardVel == 2.75f && drop->oVelY == 25);
                CHECK(drop->header.gfx.scale[0] == (material == LIQUID_PARTICLE_LAVA ? 1.5f : 0.75f));
                ModelID32 model =
                    material == LIQUID_PARTICLE_LAVA ? MODEL_LAVA_DROPLET : MODEL_COLD_WATER_DROPLET;
                CHECK(drop->header.gfx.sharedChild == gLoadedGraphNodes[model]);
            }
        }
    }
}

static void test_slide_kick_liquid_lifecycle(void) {
    for (int material = LIQUID_PARTICLE_COLD_WATER; material <= LIQUID_PARTICLE_LAVA; material++) {
        reset();
        suppliedWaterLevel = material == LIQUID_PARTICLE_LAVA ? FLOOR_LOWER_LIMIT : 0;
        suppliedFloorPresent = TRUE;
        struct Object *drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
        drop->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
        drop->oBehParams2ndByte = material;
        drop->oPosY = 0;
        drop->oVelY = 20;
        gCurrentObject = drop;
        // Moving Mario cannot drag these world-space drops along with him.
        gMarioObject->oPosX = 900;
        gMarioObject->oPosY = 300;
        for (int frame = 0; frame < 8; frame++) {
            drop->oTimer = frame;
            bhv_water_droplet_loop();
            CHECK(drop->activeFlags != ACTIVE_FLAG_DEACTIVATED);
            CHECK(drop->oPosX == 0);
            CHECK(behavior_count(bhvWaterDropletSplash) == 0);
        }
        drop->oTimer = 8;
        bhv_water_droplet_loop();
        CHECK(drop->activeFlags == ACTIVE_FLAG_DEACTIVATED);
        CHECK(behavior_count(bhvWaterDropletSplash) == 1);
        CHECK(material != LIQUID_PARTICLE_LAVA || waterQueries == 0);
        CHECK(material != LIQUID_PARTICLE_COLD_WATER || floorQueries == 0);
        for (int i = 0; i < OBJECT_POOL_CAPACITY; i++) {
            struct Object *splash = &gObjectPool[i];
            if (splash->activeFlags != ACTIVE_FLAG_DEACTIVATED
                && splash->behavior == bhvWaterDropletSplash) {
                CHECK(splash->oPosY == 0 && splash->oAction == WATER_DROPLET_ACT_SLIDE_KICK);
                CHECK(splash->oBehParams2ndByte == material);
            }
        }
    }

    reset();
    struct Object *drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    drop->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
    drop->oBehParams2ndByte = LIQUID_PARTICLE_LAVA;
    drop->oPosY = 100;
    drop->oVelY = -50;
    gCurrentObject = drop;
    // No supporting floor: bounded expiry, without a fabricated water splash.
    for (int frame = 0; frame <= 21; frame++) {
        drop->oTimer = frame;
        bhv_water_droplet_loop();
    }
    CHECK(drop->activeFlags == ACTIVE_FLAG_DEACTIVATED);
    CHECK(behavior_count(bhvWaterDropletSplash) == 0 && waterQueries == 0);

    reset();
    drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    drop->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
    drop->oBehParams2ndByte = LIQUID_PARTICLE_COLD_WATER;
    drop->oVelY = 20;
    suppliedWaterLevel = FLOOR_LOWER_LIMIT;
    gCurrentObject = drop;
    bhv_water_droplet_loop();
    CHECK(drop->activeFlags == ACTIVE_FLAG_DEACTIVATED);
    CHECK(behavior_count(bhvWaterDropletSplash) == 0);
}

static void test_slide_kick_liquid_budget(void) {
    reset();
    fill_to_free(PARTICLE_SOFT_RESERVE);
    spawn_slide_kick_liquid_particles(&sMarioState, 0, LIQUID_PARTICLE_LAVA);
    CHECK(behavior_count(bhvWaterDroplet) == 6);
    reset();
    fill_to_free(PARTICLE_SLOT_RESERVE + 3);
    spawn_slide_kick_liquid_particles(&sMarioState, 0, LIQUID_PARTICLE_LAVA);
    CHECK(behavior_count(bhvWaterDroplet) == 3);
    reset();
    fill_to_free(PARTICLE_SLOT_RESERVE);
    spawn_slide_kick_liquid_particles(&sMarioState, 0, LIQUID_PARTICLE_LAVA);
    CHECK(behavior_count(bhvWaterDroplet) == 0);
    CHECK(gMarioObject->activeFlags != ACTIVE_FLAG_DEACTIVATED);
    CHECK(waterQueries == 0);

    reset();
    struct Object *drop = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    drop->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
    drop->oBehParams2ndByte = LIQUID_PARTICLE_LAVA;
    drop->oPosY = 20;
    drop->oVelY = -30;
    suppliedFloorPresent = TRUE;
    gCurrentObject = drop;
    fill_to_free(PARTICLE_SLOT_RESERVE);
    bhv_water_droplet_loop();
    CHECK(drop->activeFlags == ACTIVE_FLAG_DEACTIVATED);
    CHECK(behavior_count(bhvWaterDropletSplash) == 0);
}

static void test_liquid_splash_material(void) {
    reset();
    struct Object *splash = visual(bhvWaterDropletSplash, PARTICLE_PRIORITY_DROPLET);
    gCurrentObject = splash;
    struct GraphNodeGenerated node = { 0 };
    const u8 expected[][3] = { { 255, 255, 255 }, { 128, 224, 255 }, { 255, 192, 64 } };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int action = WATER_DROPLET_ACT_NORMAL; action <= WATER_DROPLET_ACT_SLIDE_KICK; action++) {
            for (int material = LIQUID_PARTICLE_WATER; material <= LIQUID_PARTICLE_LAVA; material++) {
                splash->oAction = action;
                splash->oBehParams2ndByte = material;
                splash->oOpacity = 216 - cycle * 30;
                CHECK(geo_update_small_water_splash_opacity(GEO_CONTEXT_RENDER, &node.fnNode.node, NULL)
                      == sSplashCommands);
                const u8 *rgb = expected[action == WATER_DROPLET_ACT_SLIDE_KICK ? material : 0];
                CHECK(sSplashCommands[0].red == rgb[0] && sSplashCommands[0].green == rgb[1]);
                CHECK(sSplashCommands[0].blue == rgb[2]
                      && sSplashCommands[0].alpha == splash->oOpacity);
                CHECK(sSplashCommands[1].ended && node.fnNode.node.flags == LAYER_TRANSPARENT);
            }
        }
    }
    CHECK(geo_update_small_water_splash_opacity(0, &node.fnNode.node, NULL) == NULL);
    sNoDisplayMemory = TRUE;
    CHECK(geo_update_small_water_splash_opacity(GEO_CONTEXT_RENDER, &node.fnNode.node, NULL) == NULL);
}
static void test_native_initializer(void) {
    reset();
    struct Object *p = visual(bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET);
    p->metalContactCooldown = 55;
    p->metalContactPendingDefeat = 1;
    p->prevObj = p;
    p->platform = p;
    p->oHealth = 5;
    p->oActiveParticleFlags = ~0u;
    forgottenAttackObject = NULL;
    unload_object(p);
    CHECK(forgottenAttackObject == p);
    struct Object *enemy = create_object(bhvGoomba);
    CHECK(enemy == p);
    CHECK(enemy->metalContactCooldown == 0 && enemy->metalContactPendingDefeat == 0);
    CHECK(enemy->prevObj == NULL && enemy->platform == NULL && enemy->oHealth == 2048);
    CHECK(enemy->oActiveParticleFlags == 0 && enemy->oIntangibleTimer == -1);
}
static void test_feedback_ring(void) {
    reset();
    struct Object *owner = spawn_object(gMarioObject, MODEL_NONE, bhvBubbleParticleSpawner);
    gCurrentObject = owner;
    owner->parentObj = gMarioObject;
    bhv_water_down_mist_init();
    CHECK(behavior_count(bhvWhitePuffExplosion) == 6);
    CHECK(particle_budget_free_slots() == OBJECT_POOL_CAPACITY - 8);
    s32 directionMask = 0;
    for (struct ObjectNode *p = listHeads[OBJ_LIST_UNIMPORTANT].next;
         p != &listHeads[OBJ_LIST_UNIMPORTANT]; p = p->next) {
        struct Object *particle = (struct Object *) p;
        CHECK(particle->oBehParams2ndByte == 6 && particle->header.gfx.scale[0] == 0.75f);
        for (s32 i = 0; i < 6; i++) {
            if ((u16) particle->oMoveAngleYaw == (u16) ((i * 65536) / 6)) {
                directionMask |= 1 << i;
            }
        }
    }
    CHECK(directionMask == 0x3F);
    reset();
    owner = spawn_object(gMarioObject, MODEL_NONE, bhvVanishWaterRingSpawner);
    owner->parentObj = gMarioObject;
    owner->oBehParams2ndByte = 1;
    gCurrentObject = owner;
    gMarioState->faceAngle[1] = 0x2000;
    bhv_water_down_mist_init();
    CHECK(behavior_count(bhvWhitePuffExplosion) == 11);
    CHECK(particle_budget_free_slots() == OBJECT_POOL_CAPACITY - 13);
    s32 ring = 0, flecks = 0;
    for (struct ObjectNode *p = listHeads[OBJ_LIST_UNIMPORTANT].next;
         p != &listHeads[OBJ_LIST_UNIMPORTANT]; p = p->next) {
        struct Object *particle = (struct Object *) p;
        if (particle->oBehParams2ndByte == 4) {
            ring++;
        }
        if (particle->oBehParams2ndByte == 2) {
            flecks++;
        }
    }
    CHECK(ring == 8 && flecks == 3);
}

static void test_breath_bubble_reserve(void) {
    reset();
    struct Object *owner = spawn_object(gMarioObject, MODEL_NONE, bhvBubbleParticleSpawner);
    owner->oPosX = 10;
    gMarioObject->oPosX = 100;
    gMarioState->flags = MARIO_VANISH_CAP;
    sHeadUnderwater = TRUE;
    gCurrentObject = owner;
    bhv_spawn_mario_breath_bubble();
    CHECK(behavior_count(bhvSmallWaterWave) == 1);
    struct Object *bubble = (struct Object *) listHeads[OBJ_LIST_UNIMPORTANT].next;
    CHECK(bubble->oPosX == 100 && bubble->oBehParams2ndByte == 1);
    reset();
    owner = spawn_object(gMarioObject, MODEL_NONE, bhvBubbleParticleSpawner);
    gMarioState->flags = MARIO_VANISH_CAP;
    sHeadUnderwater = TRUE;
    gCurrentObject = owner;
    fill_to_free(PARTICLE_SLOT_RESERVE);
    bhv_spawn_mario_breath_bubble();
    CHECK(behavior_count(bhvSmallWaterWave) == 0);
    CHECK(particle_budget_free_slots() == PARTICLE_SLOT_RESERVE);
}
static void test_swimming_particle_rates(void) {
    reset();
    struct Object *owner = spawn_object(gMarioObject, MODEL_NONE, bhvWaterEntryBubble);
    gCurrentObject = owner;
    owner->oBehParams2ndByte = 2;
    bhv_water_waves_init();
    CHECK(behavior_count(bhvSmallParticle) == 2);
    owner->oBehParams2ndByte = 0;
    bhv_water_waves_init();
    CHECK(behavior_count(bhvSmallParticle) == 5);

    reset();
    owner = spawn_object(gMarioObject, MODEL_NONE, bhvWaterSplash);
    gCurrentObject = owner;
    const s32 totals[] = { 2, 4, 5, 7, 9, 9 };
    for (s32 timer = 0; timer < 6; timer++) {
        owner->oTimer = timer;
        bhv_water_splash_spawn_droplets();
        CHECK(behavior_count(bhvWaterDroplet) == totals[timer]);
    }

    for (s32 phase = 0; phase < 6; phase++) {
        for (s32 submerged = 0; submerged < 2; submerged++) {
            reset();
            owner = spawn_object(gMarioObject, MODEL_NONE, bhvWaveTrail);
            owner->oPosY = submerged ? -100.0f : 100.0f;
            gCurrentObject = owner;
            gGlobalTimer = phase;
            bhv_wave_trail_shrink();
            CHECK((owner->activeFlags != ACTIVE_FLAG_DEACTIVATED) == (submerged || phase % 3 == 0));
        }
    }
}

static void test_impulse_puff_lifetime_and_floor(void) {
    for (s32 hasFloor = 0; hasFloor < 2; hasFloor++) {
        reset();
        struct Object *puff = spawn_object(gMarioObject, MODEL_MIST, bhvWhitePuffExplosion);
        gCurrentObject = puff;
        puff->oBehParams2ndByte = 6;
        puff->oPosY = 20.0f;
        puff->oVelY = -13.0f;
        suppliedFloorPresent = hasFloor;
        for (s32 timer = 0; timer < 18; timer++) {
            puff->oTimer = timer;
            bhv_white_puff_exploding_loop();
            if (hasFloor && timer == 1) {
                CHECK(puff->activeFlags == ACTIVE_FLAG_DEACTIVATED);
                break;
            }
            CHECK(puff->activeFlags != ACTIVE_FLAG_DEACTIVATED);
        }
        if (!hasFloor) {
            puff->oTimer = 18;
            bhv_white_puff_exploding_loop();
            CHECK(puff->activeFlags == ACTIVE_FLAG_DEACTIVATED);
        }
    }
}

int main(void) {
    struct {
        const char *name;
        void (*run)(void);
    } cases[] = { { "live_count_and_reset", test_live_count_and_reset },
                  { "order_and_ties", test_order_and_ties },
                  { "protected_and_segmented", test_protected_and_segmented },
                  { "reserve_and_replacement", test_reserve_and_replacement },
                  { "active_parent_and_list_safety", test_active_parent_and_list_safety },
                  { "inactive_reclaim", test_inactive_reclaim },
                  { "bursts_and_projection", test_bursts_and_projection },
                  { "underwater_explosion", test_underwater_explosion },
                  { "flag_ownership_and_recovery", test_flag_ownership_and_recovery },
                  { "script_children_and_time_stop", test_script_children_and_time_stop },
                  { "droplet_secondary_and_null_fish", test_droplet_secondary_and_null_fish },
                  { "rising_droplet_water_queries", test_rising_droplet_water_queries },
                  { "shallow_water_fish_eligibility", test_shallow_water_fish_eligibility },
                  { "slide_kick_liquid_bursts", test_slide_kick_liquid_bursts },
                  { "slide_kick_liquid_lifecycle", test_slide_kick_liquid_lifecycle },
                  { "slide_kick_liquid_budget", test_slide_kick_liquid_budget },
                  { "liquid_splash_material", test_liquid_splash_material },
                  { "native_initializer", test_native_initializer },
                  { "feedback_ring", test_feedback_ring },
                  { "breath_bubble_reserve", test_breath_bubble_reserve },
                  { "swimming_particle_rates", test_swimming_particle_rates },
                  { "impulse_puff_lifetime_and_floor", test_impulse_puff_lifetime_and_floor } };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        cases[i].run();
        if (failures) {
            return 1;
        }
        printf("PASS particles.%s\n", cases[i].name);
    }
    printf("%zu particle cases passed.\n", sizeof(cases) / sizeof(cases[0]));
    return 0;
}
