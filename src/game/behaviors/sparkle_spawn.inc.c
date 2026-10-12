
/**
 * Behavior for bhvSparkleSpawn.
 *
 * Invisible spawner used by various objects. It attempts one child on each
 * update with oTimer 0, 1 and 2, then deletes itself after the final attempt.
 * Successful children receive world-position jitter and random scale before
 * bhvSparkle takes over their animation and lifetime. Failed allocation skips
 * those random draws and still advances the spawner toward deletion.
 */

void bhv_sparkle_spawn_loop(void) {
    struct Object *sparkle = try_to_spawn_object(0, 1.0f, o, MODEL_SPARKLES_ANIMATION, bhvSparkle);
    if (sparkle != NULL) {
        obj_translate_xyz_random(sparkle, 90.0f);
        obj_scale_random(sparkle, 1.0f, 0.0f);
    }
    if (o->oTimer > 1) {
        obj_mark_for_deletion(o);
    }
}
