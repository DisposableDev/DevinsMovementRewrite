#ifndef MARIO_H
#define MARIO_H

#include <PR/ultratypes.h>

#include "macros.h"
#include "types.h"

#define MARIO_LANDING_JUMP_BUFFER_FRAMES 6
#define MARIO_BACKFLIP_LANDING_BUFFER_FRAMES 6
#define MARIO_WALL_KICK_INPUT_BUFFER_FRAMES 3
#define MARIO_BURN_COIN_JUMP_BUFFER_FRAMES 6
#define MARIO_BACKWARD_LONG_JUMP_CROUCH_FRAMES 5
#define MARIO_BURN_TIMER_LIMIT 160
#define MARIO_BURN_DAMAGE_TOTAL (3 * 0x100)
#define MARIO_TWIRL_COOLDOWN_FRAMES 30
#define MARIO_WALL_TWIRL_SOUND_GAP_FRAMES 6
#define MARIO_TWIRL_WALL_BOOST_MULTIPLIER 1.15f

enum MarioSpawnFallProtection {
    MARIO_SPAWN_FALL_PROTECTION_NONE,
    MARIO_SPAWN_FALL_PROTECTION_ACTIVE,
    MARIO_SPAWN_FALL_PROTECTION_LANDED,
};

s32 mario_is_knockback_recovery(u32 action);
s32 mario_move_buffer_family(u32 action);
s32 mario_is_buffered_landing(u32 action);
void mario_update_move_buffers(struct MarioState *m);
s32 is_anim_at_end(struct MarioState *m);
s32 is_anim_past_end(struct MarioState *m);
s16 set_mario_animation(struct MarioState *m, s32 targetAnimID);
s16 set_mario_anim_with_accel(struct MarioState *m, s32 targetAnimID, s32 accel);
void set_anim_to_frame(struct MarioState *m, s16 animFrame);
s32 is_anim_past_frame(struct MarioState *m, s16 animFrame);
s16 find_mario_anim_flags_and_translation(struct Object *obj, s32 yaw, Vec3s translation);
void update_mario_pos_for_anim(struct MarioState *m);
s16 return_mario_anim_y_translation(struct MarioState *m);
void play_sound_if_no_flag(struct MarioState *m, u32 soundBits, u32 flags);
void play_mario_jump_sound(struct MarioState *m);
void adjust_sound_for_speed(struct MarioState *m);
void play_sound_and_spawn_particles(struct MarioState *m, u32 soundBits, u32 waveParticleType);
void play_mario_action_sound(struct MarioState *m, u32 soundBits, u32 waveParticleType);
void play_mario_landing_sound(struct MarioState *m, u32 soundBits);
void play_mario_landing_sound_once(struct MarioState *m, u32 soundBits);
void play_mario_heavy_landing_sound(struct MarioState *m, u32 soundBits);
void play_mario_heavy_landing_sound_once(struct MarioState *m, u32 soundBits);
void play_mario_sound(struct MarioState *m, s32 primarySoundBits, s32 scondarySoundBits);
void mario_limit_velocity_vector(Vec3f velocity);
void mario_limit_velocity(struct MarioState *m);
void mario_set_forward_vel(struct MarioState *m, f32 speed);
void set_mario_y_vel_based_on_fspeed(struct MarioState *m, f32 initialVelY, f32 multiplier);
s32 mario_get_floor_class(struct MarioState *m);
u32 mario_get_terrain_sound_addend(struct MarioState *m);
s32 mario_facing_downhill(struct MarioState *m, s32 turnYaw);
u32 mario_floor_is_slippery(struct MarioState *m);
s32 mario_floor_is_slope(struct MarioState *m);
s32 mario_floor_is_steep(struct MarioState *m);
f32 find_floor_height_relative_polar(struct MarioState *m, s16 angleFromMario, f32 distFromMario);
s16 find_floor_slope(struct MarioState *m, s16 yawOffset);
Bool32 set_mario_wall(struct MarioState *m, struct Surface *wall);
Bool32 set_mario_ceil(struct MarioState *m, struct Surface *ceil,  f32 ceilHeight);
Bool32 set_mario_floor(struct MarioState *m, struct Surface *floor, f32 floorHeight);
void update_mario_sound_and_camera(struct MarioState *m);
void set_steep_jump_action(struct MarioState *m);
u32 set_mario_action(struct MarioState *m, u32 action, u32 actionArg);
s32 set_jump_from_landing(struct MarioState *m);
s32 set_jumping_action(struct MarioState *m, u32 action, u32 actionArg);
s32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 actionArg);
s32 hurt_and_set_mario_action(struct MarioState *m, u32 action, u32 actionArg, s16 hurtCounter);
s32 check_common_action_exits(struct MarioState *m);
s32 check_common_hold_action_exits(struct MarioState *m);
void mario_stop_underwater_shell(struct MarioState *m);
s32 transition_submerged_to_walking(struct MarioState *m);
u32 landing_jump_action(struct MarioState *m, u32 ordinaryAction);
s32 check_landing_attack_or_crouch(struct MarioState *m);
s32 transition_submerged_to_airborne(struct MarioState *m);
s32 mario_has_current_metal_cap(struct MarioState *m);
s32 mario_has_current_vanish_cap(struct MarioState *m);
s32 mario_has_water_damage_protection(struct MarioState *m);
s32 mario_has_land_movement_underwater(struct MarioState *m);
s32 mario_keeps_shell(struct MarioState *m);
s32 mario_blocks_harmful_knockback(struct MarioState *m);
s32 mario_is_riding_underwater_shell(struct MarioState *m);
s32 mario_transition_underwater_shell_to_riding(struct MarioState *m);
s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m);
s32 mario_is_low_land_pose(struct MarioState *m);
s32 mario_is_head_underwater(struct MarioState *m);
s32 mario_has_water_contact(struct MarioState *m, f32 waterLevel);
void mario_update_forced_caps(struct MarioState *m);
void mario_add_cap_power(struct MarioState *m, u32 flags);
void mario_clear_cap_flags(struct MarioState *m);
u32 mario_natural_cap_powers(struct MarioState *m);
void mario_update_soaked_status(struct MarioState *m);
s32 mario_is_soaked(struct MarioState *m);
void mario_apply_soaked_status(struct MarioState *m);
void mario_consume_soaked_status(struct MarioState *m);
s32 mario_action_locks_camera(u32 action);
s32 set_water_plunge_action(struct MarioState *m);
void mario_ground_twirl_init(void);
void mario_air_twirl_init(struct MarioState *m);
void mario_air_twirl_reset_on_coin(struct MarioState *m);
s32 mario_air_twirl_is_active(struct MarioState *m);
s32 mario_try_wall_air_twirl(struct MarioState *m);
s32 mario_air_twirl_flying_wall_bounce(struct MarioState *m);
void mario_air_twirl_flying_action(struct MarioState *m);
void mario_ground_twirl_update_cooldown(void);
void mario_air_twirl_transition(struct MarioState *m, u32 action);
// Once per Mario update before interactions: consume queued inputs, advance timers,
// and attempt air entry or held-L ground continuation. TRUE means a twirl started.
s32 mario_try_air_twirl(struct MarioState *m);
s32 mario_start_air_twirl(struct MarioState *m, s32 continuation);
s32 mario_air_twirl_action(struct MarioState *m);
f32 mario_air_twirl_gravity_scale(struct MarioState *m);
f32 mario_air_twirl_control_scale(struct MarioState *m);
void mario_air_twirl_limit_control_speed(struct MarioState *m);
void mario_play_wall_bonk_sound(struct MarioState *m);
void mario_play_air_twirl_sound(struct MarioState *m);
s32 mario_start_ground_twirl(struct MarioState *m, s32 continuation);
void mario_twirl_begin_contacts(void);
// Process all eligible contacts before applying at most one Mario rebound.
void mario_twirl_push_enemies(struct MarioState *m);
s32 mario_twirl_can_stun_enemy(struct Object *obj);
s32 mario_try_ground_twirl(struct MarioState *m);
void mario_ground_twirl_cancel_queued_input(void);
void mario_ground_twirl_reset_wall_bounces(void);
s32 mario_ground_twirl_action(struct MarioState *m);
s32 mario_ground_twirl_is_active(struct MarioState *m);
s32 mario_ground_twirl_is_held_pose(struct MarioState *m);
s32 mario_ground_twirl_loses_cap_on_damage(struct MarioState *m);
s32 mario_ground_twirl_is_enemy(struct Object *obj);
s32 mario_ground_twirl_is_oversized_enemy(struct Object *obj);
s32 mario_ground_twirl_is_immune_enemy(struct Object *obj);
s32 mario_ground_twirl_is_large_enemy(struct Object *obj);
s32 mario_ground_twirl_rebound_from_stun(struct MarioState *m, struct Object *obj);
void mario_metal_stun_enemy_on_contact(struct MarioState *m, struct Object *obj);
void mario_ground_pound_impact(struct MarioState *m);
void mario_he_thicc_contact_shake(struct MarioState *m);
s32 mario_has_rumble_contact(struct MarioState *m);
void mario_ground_twirl_reset_cooldown(void);
s32 execute_mario_action(UNUSED struct Object *obj);
void init_mario(void);
s32 mario_has_cap_for_damage(struct MarioState *m);
void mario_reset_health_regeneration(struct MarioState *m);
s32 mario_health_after_pending_damage(struct MarioState *m);
void mario_queue_respawn_healing(struct MarioState *m);
// Apply this update's burn damage after advancing oMarioBurnTimer by timerStep.
void mario_apply_burning_damage(struct MarioState *m, s32 timerStep);
void init_mario_from_save_file(void);

#endif // MARIO_H
