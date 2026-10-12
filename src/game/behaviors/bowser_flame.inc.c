static s32 bowser_flames_use_original_fights(void) {
    return game_options_get(GO_BOWSER_FIGHTS) == 1
           && (gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2
               || gCurrLevelNum == LEVEL_BOWSER_3);
}

static void original_bowser_flame_despawn(void);
static s32 original_bowser_flame_should_despawn(s32 maxTime);
static void original_bhv_flame_bowser_init(void);
static void original_bhv_flame_large_burning_out_init(void);
static void original_bhv_flame_bowser_loop(void);
static void original_bhv_flame_moving_forward_growing_init(void);
static void original_bhv_flame_moving_forward_growing_loop(void);
static void original_bhv_flame_floating_landing_init(void);
static void original_bhv_flame_floating_landing_loop(void);
static void original_bhv_blue_bowser_flame_init(void);
static void original_bhv_blue_bowser_flame_loop(void);
static void original_bhv_flame_bouncing_init(void);
static void original_bhv_flame_bouncing_loop(void);
static void original_bhv_blue_flames_group_loop(void);

// bowser_flame.inc.c

struct ObjectHitbox sGrowingBowserFlameHitbox = {
    /* interactType:      */ INTERACT_FLAME,
    /* downOffset:        */ 20,
    /* damageOrCoinValue: */ 1,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 12,
    /* height:            */ 40,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

struct ObjectHitbox sBowserFlameHitbox = {
    /* interactType:      */ INTERACT_FLAME,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 1,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 12,
    /* height:            */ 40,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

void bowser_flame_despawn(void) {
    if (bowser_flames_use_original_fights()) {
        original_bowser_flame_despawn();
        return;
    }

    obj_mark_for_deletion(o);
    spawn_object_with_scale(o, MODEL_NONE, bhvBlackSmokeUpward, 1.0f);
    if (random_float() < (gCurrLevelNum == LEVEL_BOWSER_3 ? 0.1f : 0.05f)) {
        spawn_object(o, MODEL_YELLOW_COIN, bhvTemporaryYellowCoin);
    }
}

static s32 bowser_blue_flame_lifetime(void) {
    switch (gCurrLevelNum) {
        case LEVEL_BOWSER_1:
            return 120;
        case LEVEL_BOWSER_2:
            return 180;
        case LEVEL_BOWSER_3:
            return 240;
        default:
            return BOWSER_FLAME_AIR_TIMEOUT_FRAMES;
    }
}

s32 bowser_flame_should_despawn(s32 maxTime) {
    if (bowser_flames_use_original_fights()) {
        return original_bowser_flame_should_despawn(maxTime);
    }

    if (o->oBehParams & BOWSER_FLAME_DOUBLE_LIFETIME) {
        maxTime *= 2;
    }
    if (o->oTimer >= maxTime) {
        return TRUE;
    }

    // Flames should despawn if they fall off the arena.
    if ((o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->oFloorType == SURFACE_BURNING) {
        return TRUE;
    }
    if ((o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->oFloorType == SURFACE_DEATH_PLANE) {
        return TRUE;
    }

    return FALSE;
}

static f32 bowser_red_ground_flame_speed(void) {
    if (gCurrLevelNum == LEVEL_BOWSER_3) {
        return BOWSER_FIGHT_3_RED_GROUND_SPEED;
    }
    return gCurrLevelNum == LEVEL_BOWSER_1 ? 5.0f : 10.0f;
}

static s16 bowser_blue_flame_turn_speed(void) {
    return gCurrLevelNum == LEVEL_BOWSER_3 ? BOWSER_FIGHT_3_BLUE_FLAME_TURN_SPEED
                                           : BOWSER_BLUE_FLAME_TURN_SPEED;
}

void bhv_flame_bowser_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_bowser_init();
        return;
    }

    o->oAnimState = (s32) (random_float() * 10.0f);
    if (o->oBehParams & BOWSER_FLAME_GROUND_BREATH) {
        // Breath fire keeps its incoming heading and follows the floor immediately.
        o->oAction = 1;
        o->oVelY = 0.0f;
        o->oGravity = 0.0f;
        o->oForwardVel = bowser_red_ground_flame_speed();
        o->oFlameScale = random_float() * 2.0f + 6.0f;
        return;
    }
    o->oMoveAngleYaw = random_u16();
    o->oVelY = random_float() < 0.2f ? 80.0f : 20.0f;
    o->oForwardVel = 10.0f;
    o->oGravity = -0.8f - random_float() * 0.4f;
    o->oFlameScale = random_float() + 1.0f;
}

void bhv_flame_large_burning_out_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_large_burning_out_init();
        return;
    }

    o->oAnimState = (s32) (random_float() * 10.0f);
    o->oMoveAngleYaw = random_u16();
    o->oGravity = -0.8f - random_float() * 0.4f;
    o->oVelY = 10.0f;
    o->oForwardVel = 0.0f;
    o->oFlameScale = 7.0f;
}

void bowser_flame_move(void) {
    s32 timer = ((o->oFlameSpeedTimerOffset + gGlobalTimer) & 0x3F) << 10;
    o->oPosX += sins(o->oMoveAngleYaw) * sins(timer) * 4.0f;
    o->oPosZ += coss(o->oMoveAngleYaw) * sins(timer) * 4.0f;
}

// The 1,000-unit center disk lies wholly inside the static star and outside
// every falling piece, including its walls. Vertical/water updates still run.
static s32 bowser_flame_has_fixed_arena_support(void) {
    return gCurrLevelNum == LEVEL_BOWSER_3 && gCollisionFlags == 0 && o->oAction != 0
           && o->oForwardVel == 0.0f && o->oVelY == 0.0f && o->oGravity == 0.0f && o->oRoom == -1
           && o->oWallHitboxRadius <= 0.1f && o->oFloor != NULL
           && !(o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
           && !(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) && !(o->oFloor->flags & SURFACE_FLAG_DYNAMIC)
           && o->oFloor->normal.y == 1.0f && o->oFloorType == SURFACE_DEFAULT
           && o->oFloorHeight == 307.0f && o->oPosY == 307.0f
           && sqr(o->oPosX) + sqr(o->oPosZ) < sqr(1000.0f);
}

static s32 bowser_flame_has_fixed_arena_floor(void) {
    return gCurrLevelNum == LEVEL_BOWSER_3 && gCollisionFlags == 0 && o->oRoom == -1
           && o->oWallHitboxRadius <= 0.1f && o->oFloor != NULL
           && !(o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
           && !(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) && !(o->oFloor->flags & SURFACE_FLAG_DYNAMIC)
           && o->oFloor->normal.y == 1.0f && o->oFloorType == SURFACE_DEFAULT
           && o->oFloorHeight == 307.0f && o->oPosY >= 307.0f
           && sqr(o->oPosX) + sqr(o->oPosZ) < sqr(1000.0f);
}

static void bowser_flame_update_floor(void) {
    if (gCurrLevelNum != LEVEL_BOWSER_3 || gCollisionFlags != 0 || o->oWallHitboxRadius > 0.1f
        || (o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))) {
        cur_obj_update_floor_and_walls();
        return;
    }
    // These flames never react to the steep-floor wall flag. Horizontal movement
    // still performs its native edge and slope checks after velocity/drag.
    o->oMoveFlags &= ~OBJ_MOVE_HIT_WALL;
    if (!bowser_flame_has_fixed_arena_floor()) {
        o->oFloor = cur_obj_update_floor_height_and_get_floor();
    }
    o->oFloorType = o->oFloor != NULL ? o->oFloor->type : SURFACE_DEFAULT;
    o->oFloorRoom = o->oFloor != NULL ? o->oFloor->room : 0;
    if (o->oPosY > o->oFloorHeight) {
        o->oMoveFlags |= OBJ_MOVE_IN_AIR;
    }
}

static s32 bowser_flame_move_on_fixed_arena_floor(void) {
    if (!bowser_flame_has_fixed_arena_floor()) {
        return FALSE;
    }
    cur_obj_compute_vel_xz();
    cur_obj_apply_drag_xz(o->oDragStrength);
    f32 intendedX = o->oPosX + o->oVelX;
    f32 intendedZ = o->oPosZ + o->oVelZ;
    if (sqr(intendedX) + sqr(intendedZ) >= sqr(1000.0f)) {
        return FALSE;
    }
    // Both endpoints and their segment are wholly inside the fixed flat disk.
    // Match move_standard(78): drag, horizontal travel, vertical update and speed.
    o->oMoveFlags &= ~OBJ_MOVE_HIT_EDGE;
    o->oPosX = intendedX;
    o->oPosZ = intendedZ;
    cur_obj_move_y(o->oGravity, o->oBounciness, o->oBuoyancy);
    s32 negativeSpeed = o->oForwardVel < 0.0f;
    o->oForwardVel = sqrtf(sqr(o->oVelX) + sqr(o->oVelZ));
    if (negativeSpeed) {
        o->oForwardVel = -o->oForwardVel;
    }
    return TRUE;
}

static void bowser_flame_update_movement(void) {
    if (bowser_flame_has_fixed_arena_support()) {
        o->oVelX = 0.0f;
        o->oVelZ = 0.0f;
        o->oMoveFlags &= ~(OBJ_MOVE_HIT_WALL | OBJ_MOVE_HIT_EDGE);
        cur_obj_move_y(o->oGravity, o->oBounciness, o->oBuoyancy);
        return;
    }
    // The first floor query consumes special flags; its result is reusable only
    // when both queries would have used the ordinary collision rules.
    s32 ordinaryQuery = gCollisionFlags == 0;
    bowser_flame_update_floor();

    if (ordinaryQuery && o->oAction != 0 && o->oForwardVel == 0.0f && o->oVelY == 0.0f
        && o->oGravity == 0.0f && o->oRoom == -1 && o->oFloor != NULL
        && o->oFloorHeight >= FLOOR_LOWER_LIMIT_MISC
        && !(o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
        && !(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
        // Settled fire has no horizontal travel. Keep the fresh support query
        // and native vertical update for floor changes, ground flags and water.
        o->oVelX = 0.0f;
        o->oVelZ = 0.0f;
        o->oMoveFlags &= ~OBJ_MOVE_HIT_EDGE;
        cur_obj_move_y(o->oGravity, o->oBounciness, o->oBuoyancy);
    } else if (!bowser_flame_move_on_fixed_arena_floor()) {
        cur_obj_move_standard(78);
    }
}

void bhv_flame_bowser_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_bowser_loop();
        return;
    }

    if (o->oAction != 0) {
        o->oForwardVel = bowser_red_ground_flame_speed();
    }
    if (o->oForwardVel > 0.0f) {
        s16 turnSpeed = BOWSER_RED_FLAME_TURN_SPEED;
        if (o->oAction != 0 && gCurrLevelNum == LEVEL_BOWSER_3) {
            turnSpeed = BOWSER_FIGHT_3_RED_GROUND_TURN_SPEED;
        }
        cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), turnSpeed);
    }
    bowser_flame_update_movement();

    // Keep red flames staggered even after they reach their falling-speed limit.
    f32 fallSpeedLimit = o->oGravity * 4.0f;
    if (o->oAction == 0 && o->oVelY < fallSpeedLimit) {
        o->oVelY = fallSpeedLimit;
    }

    if (o->oAction == 0) {
        cur_obj_become_tangible();
        bowser_flame_move();
        if (o->oTimer >= BOWSER_FLAME_AIR_TIMEOUT_FRAMES) {
            obj_mark_for_deletion(o);
        }

        if (o->oMoveFlags & OBJ_MOVE_LANDED) {
            o->oAction++;
            if (cur_obj_has_behavior(bhvFlameLargeBurningOut)) {
                o->oFlameScale = 8.0f;
            } else {
                o->oFlameScale = random_float() * 2 + 6.0f;
            }
            o->oForwardVel = 0;
            o->oVelY = 0;
            o->oGravity = 0;
        }
    } else {
        cur_obj_become_tangible();

        f32 lifetimeScale = o->oBehParams & BOWSER_FLAME_DOUBLE_LIFETIME ? 2.0f : 1.0f;
        if (o->oTimer / lifetimeScale > o->oFlameScale * 10 + 5.0f) {
            o->oFlameScale -= 0.15f / lifetimeScale;
            if (o->oFlameScale <= 0) {
                bowser_flame_despawn();
            }
        }
    }

    cur_obj_scale(o->oFlameScale);
    o->oGraphYOffset = o->header.gfx.scale[1] * 14.0f;
    obj_set_hitbox(o, &sBowserFlameHitbox);
}

void bhv_flame_moving_forward_growing_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_moving_forward_growing_init();
        return;
    }

    o->oForwardVel = 30.0f;
    obj_translate_xz_random(o, 80.0f);
    o->oAnimState = (s32)(random_float() * 10.0f);
    o->oFlameScale = 3.0f;
}

void bhv_flame_moving_forward_growing_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_moving_forward_growing_loop();
        return;
    }

    cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), BOWSER_RED_FLAME_TURN_SPEED);
    obj_set_hitbox(o, &sGrowingBowserFlameHitbox);
    o->oFlameScale = MIN(o->oFlameScale + 0.5f, 30.0f);
    cur_obj_scale(o->oFlameScale);

    if (o->oMoveAnglePitch > 0x800) {
        o->oMoveAnglePitch -= 0x200;
    }

    cur_obj_set_pos_via_transform();
    cur_obj_update_floor_height();

    if (o->oTimer >= BOWSER_FLAME_AIR_TIMEOUT_FRAMES) {
        obj_mark_for_deletion(o);
    }

    if (o->oPosY < o->oFloorHeight) {
        o->oPosY = o->oFloorHeight;
        struct Object *flame = spawn_object(o, MODEL_RED_FLAME, bhvFlameBowser);
        flame->oMoveAngleYaw = o->oMoveAngleYaw;
        flame->oBehParams |= BOWSER_FLAME_GROUND_BREATH;
        obj_mark_for_deletion(o);
    }
}

void bhv_flame_floating_landing_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_floating_landing_init();
        return;
    }

    o->oAnimState = (s32) (random_float() * 10.0f);
    o->oMoveAngleYaw = random_u16();
    if (o->oBehParams2ndByte != 0) {
        o->oForwardVel = random_float() * 5.0f;
    } else {
        o->oForwardVel = random_float() * 70.0f;
    }
    o->oVelY = random_float() * 20.0f;
    if (o->oBehParams2ndByte == 0) {
        o->oGravity = -0.8f - random_float() * 0.4f;
    } else {
        o->oGravity = -1.0f;
    }
    o->oFlameSpeedTimerOffset = random_float() * 64.0f;
}

f32 sFlameFloatingYLimit[] = { -8.0f, -6.0f, -3.0f };

void bhv_flame_floating_landing_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_floating_landing_loop();
        return;
    }

    s16 turnSpeed =
        o->oBehParams2ndByte == 0 ? BOWSER_RED_FLAME_TURN_SPEED : bowser_blue_flame_turn_speed();
    cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), turnSpeed);
    obj_set_hitbox(o, &sBowserFlameHitbox);
    cur_obj_become_tangible();
    bowser_flame_update_movement();
    bowser_flame_move();

    if (bowser_flame_should_despawn(o->oBehParams2ndByte == 0 ? BOWSER_FLAME_AIR_TIMEOUT_FRAMES
                                                              : bowser_blue_flame_lifetime())) {
        obj_mark_for_deletion(o);
    }

    f32 fallSpeedLimit = sFlameFloatingYLimit[o->oBehParams2ndByte];
    if (o->oBehParams2ndByte == 0) {
        fallSpeedLimit *= -o->oGravity;
    }
    if (o->oVelY < fallSpeedLimit) {
        o->oVelY = fallSpeedLimit;
    }

    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        if (o->oBehParams2ndByte == 0) {
            struct Object *flame = spawn_object(o, MODEL_RED_FLAME, bhvFlameLargeBurningOut);
            flame->oBehParams |= o->oBehParams & BOWSER_FLAME_DOUBLE_LIFETIME;
        } else {
            spawn_object(o, MODEL_NONE, bhvBlueFlamesGroup); //? wonder if they meant MODEL_BLUE_FLAME?
        }
        obj_mark_for_deletion(o);
    }

    o->oGraphYOffset = o->header.gfx.scale[1] * 14.0f;
}

void bhv_blue_bowser_flame_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_blue_bowser_flame_init();
        return;
    }

    obj_translate_xz_random(o, 80.0f);
    o->oAnimState = (s32) (random_float() * 10.0f);
    o->oVelY = 7.0f;
    o->oForwardVel = o->oBehParams & BOWSER_FLAME_NO_SPLIT ? BOWSER_SPIT_FLAME_SPEED : 35.0f;
    o->oFlameScale = 3.0f;
    o->oFlameUnusedRand = random_float() * 0.5f;
    o->oGravity = -1.0f;
    o->oFlameSpeedTimerOffset = (s32) (random_float() * 64.0f);
}

void bhv_blue_bowser_flame_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_blue_bowser_flame_loop();
        return;
    }

    s32 i;

    s16 turnSpeed =
        o->oBehParams2ndByte == 0 ? BOWSER_RED_FLAME_TURN_SPEED : bowser_blue_flame_turn_speed();
    if (o->oBehParams & BOWSER_FLAME_NO_SPLIT) {
        o->oForwardVel = BOWSER_SPIT_FLAME_SPEED;
        if (o->oAction != 0) {
            // The first update after touchdown has action timer zero.
            if (gCurrLevelNum == LEVEL_BOWSER_3) {
                o->oForwardVel = MAX(BOWSER_FIGHT_3_SPIT_GROUND_SPEED,
                                     BOWSER_FIGHT_3_SPIT_LANDING_SPEED
                                         - (o->oTimer + 1) * BOWSER_FIGHT_3_SPIT_DECEL);
                turnSpeed = bowser_blue_flame_turn_speed();
            } else {
                o->oForwardVel = MAX(BOWSER_SPIT_FLAME_GROUND_SPEED,
                                     BOWSER_SPIT_FLAME_LANDING_SPEED - (o->oTimer + 1));
                turnSpeed = BOWSER_SPIT_FLAME_GROUND_TURN_SPEED;
            }
        }
    }
    cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), turnSpeed);
    obj_set_hitbox(o, &sGrowingBowserFlameHitbox);

    if (o->oFlameScale < 16.0f) {
        o->oFlameScale = o->oFlameScale + 0.5f;
    }

    cur_obj_scale(o->oFlameScale);
    bowser_flame_update_movement();

    if (o->oBehParams & BOWSER_FLAME_NO_SPLIT) {
        if (o->oAction == 0 && (o->oMoveFlags & OBJ_MOVE_LANDED)) {
            // Keep this projectile as the ground hazard instead of spawning children.
            o->oAction = 1;
            o->oForwardVel = gCurrLevelNum == LEVEL_BOWSER_3 ? BOWSER_FIGHT_3_SPIT_LANDING_SPEED
                                                             : BOWSER_SPIT_FLAME_LANDING_SPEED;
            o->oVelY = 0.0f;
            o->oGravity = -1.0f;
            o->oBounciness = 0.0f;
        } else if (o->oTimer >= bowser_blue_flame_lifetime()) {
            obj_mark_for_deletion(o);
            if (random_float() < (gCurrLevelNum == LEVEL_BOWSER_3 ? 0.1f : 0.05f)) {
                spawn_object(o, MODEL_YELLOW_COIN, bhvTemporaryYellowCoin);
            }
        }
        return;
    }

    if (o->oTimer > 20) {
        if (o->oBehParams2ndByte == 0) {
            s32 flameCount = gCurrLevelNum == LEVEL_BOWSER_3 ? 6 : 3;
            for (i = 0; i < flameCount; i++) {
                spawn_object_relative_with_scale(0, 0, 0, 0, 5.0f, o, MODEL_RED_FLAME,
                                                 bhvFlameFloatingLanding);
            }
        } else {
            spawn_object_relative_with_scale(1, 0, 0, 0, 8.0f, o, MODEL_BLUE_FLAME,
                                             bhvFlameFloatingLanding);
            if (gCurrLevelNum != LEVEL_BOWSER_3) {
                spawn_object_relative_with_scale(2, 0, 0, 0, 8.0f, o, MODEL_BLUE_FLAME,
                                                 bhvFlameFloatingLanding);
            }
        }
        obj_mark_for_deletion(o);
    }
}

void bhv_flame_bouncing_init(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_bouncing_init();
        return;
    }

    o->oAnimState = (s32) (random_float() * 10.0f);
    o->oGravity = -1.0f;
    o->oVelY = 30.0f;
    o->oForwardVel = 20.0f;
    o->oFlameScale = o->header.gfx.scale[0];
    o->oFlameSpeedTimerOffset = (s32) (random_float() * 64.0f);
}

void bhv_flame_bouncing_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_flame_bouncing_loop();
        return;
    }

    cur_obj_rotate_yaw_toward(obj_angle_to_object(o, gMarioObject), bowser_blue_flame_turn_speed());
    o->oForwardVel = gCurrLevelNum == LEVEL_BOWSER_3 ? BOWSER_FIGHT_3_BOUNCING_FLAME_SPEED : 15.0f;
    o->oBounciness = -1.0f;
    cur_obj_scale(o->oFlameScale);
    obj_set_hitbox(o, &sGrowingBowserFlameHitbox);
    bowser_flame_update_movement();

    if (bowser_flame_should_despawn(bowser_blue_flame_lifetime())) {
        obj_mark_for_deletion(o);
    }
}

void bhv_blue_flames_group_loop(void) {
    if (bowser_flames_use_original_fights()) {
        original_bhv_blue_flames_group_loop();
        return;
    }

    if (o->oTimer == 0) {
        o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
    }

    s32 flameIndex = (o->oTimer / BOWSER_BLUE_GROUP_SPAWN_INTERVAL) * 3;
    s32 flameCount = gCurrLevelNum == LEVEL_BOWSER_3 ? BOWSER_BLUE_GROUP_FLAME_COUNT / 2
                                                     : BOWSER_BLUE_GROUP_FLAME_COUNT;
    if (flameIndex >= flameCount) {
        obj_mark_for_deletion(o);
        return;
    }
    if (o->oTimer % BOWSER_BLUE_GROUP_SPAWN_INTERVAL == 0) {
        // Emit each ring together in its three radial directions.
        o->oBlueFlameNextScale = 5.0f - (flameIndex / 3) * 0.5f;
        for (s32 i = 0; i < 3; i++) {
            struct Object *flame = spawn_object(o, MODEL_BLUE_FLAME, bhvFlameBouncing);
            if (flame != NULL) {
                flame->oMoveAngleYaw += i * DEGREES(120);
                flame->header.gfx.scale[0] = o->oBlueFlameNextScale;
            }
        }
    }
}

#include "bowser_flame_original.inc.c"
