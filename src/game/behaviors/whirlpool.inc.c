// Whirlpool visuals/interaction and shared bubble-emitter configuration for whirlpools and jets.

// Environment bubbles are drawn in groups of five.
#define WHIRLPOOL_BUBBLE_COUNT 30

static struct ObjectHitbox sWhirlpoolHitbox = {
    /* interactType:      */ INTERACT_WHIRLPOOL,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 200,
    /* height:            */ 500,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

// Keep placed pitch/roll for bubble rotation before clearing the visual angles.
void bhv_whirlpool_init(void) {
    o->oWhirlpoolInitFacePitch = o->oFaceAnglePitch;
    o->oWhirlpoolInitFaceRoll = o->oFaceAngleRoll;
    o->oFaceAnglePitch = 0;
    o->oFaceAngleRoll = 0;
}

void whirlpool_set_hitbox(void) {
    obj_set_hitbox(o, &sWhirlpoolHitbox);
}

void whirlpool_orient_graph(void) {
    f32 cosPitch = coss(o->oFaceAnglePitch);
    f32 sinPitch = sins(o->oFaceAnglePitch);
    f32 cosRoll = coss(o->oFaceAngleRoll);
    f32 sinRoll = sins(o->oFaceAngleRoll);
    f32 normalX = sinRoll * cosPitch;
    f32 normalY = cosPitch * cosRoll;
    f32 normalZ = sinPitch;
    obj_orient_graph(o, normalX, normalY, normalZ);
}

// Distance gates bubble emission and visuals; hitbox and sound requests continue each update.
void bhv_whirlpool_loop(void) {
    if (o->oDistanceToMario < 5000.0f) {
        o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;

        gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT] = WHIRLPOOL_BUBBLE_COUNT;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_X] = o->oPosX;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z] = o->oPosZ;
        gEnvFxBubbleConfig[ENVFX_STATE_DEST_X] = o->oPosX;
        gEnvFxBubbleConfig[ENVFX_STATE_DEST_Y] = o->oPosY;
        gEnvFxBubbleConfig[ENVFX_STATE_DEST_Z] = o->oPosZ;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] = o->oPosY + 800.0f;
        gEnvFxBubbleConfig[ENVFX_STATE_PITCH] = o->oWhirlpoolInitFacePitch;
        // The legacy YAW slot carries the whirlpool's placed roll.
        gEnvFxBubbleConfig[ENVFX_STATE_YAW] = o->oWhirlpoolInitFaceRoll;

        whirlpool_orient_graph();

        o->oFaceAngleYaw += 8000;
    } else {
        o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT] = 0;
    }

    cur_obj_play_sound_1(SOUND_ENV_WATER);

    whirlpool_set_hitbox();
}

// Publish the shared jet source/count in range; distant jets clear only the count.
void bhv_jet_stream_loop(void) {
    if (o->oDistanceToMario < 5000.0f) {
        gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT] =
            gCurrLevelNum == LEVEL_JRB ? JRB_JETSTREAM_BUBBLE_COUNT : 60;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_X] = o->oPosX;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] = o->oPosY;
        gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z] = o->oPosZ;
    } else {
        gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT] = 0;
    }

    cur_obj_play_sound_1(SOUND_ENV_WATER);
}
