"""Earned lobby guest sequencing, one-shot conversations, dialogue and fade assets."""
from pathlib import Path
import json
import re
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class CastleLobbyFinaleTests(unittest.TestCase):
    def test_all_conversations_and_arrivals(self):
        feature = (ROOT / 'src/game/behaviors/castle_lobby_finale.inc.c').read_text()
        feature = re.sub(r'^#include.*\n', '', feature, flags=re.M)
        animations = set(re.findall(r'\b(?:castle_lobby_\w+anims_\w+|castle_lobby_motos_anime|toad_seg6_anims_\w+|mips_seg6_anims_\w+)\b', feature))
        declarations = ''.join('static const struct Animation *const ' + name + '[] = {NULL};\n'
                               for name in sorted(animations))
        native_growth = Source(ROOT / 'src/game/behaviors/piranha_plant.inc.c').function(
            'piranha_plant_act_respawn')
        sign_source = Source(ROOT / 'src/game/behaviors/message_panel.inc.c')
        native_sign = sign_source.macros('SIGNPOST_.*') + sign_source.function('bhv_message_panel_loop')
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "dialog_ids.h"
#include "object_constants.h"
typedef int s32;
typedef uint32_t u32;
#define UNUSED __attribute__((unused))
typedef float f32;
typedef short s16;
#define TRUE 1
#define FALSE 0
#define BITMASK(n) ((1u << (n))-1)
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define STATIC_ASSERT(c,m) _Static_assert(c,m)
#define INTERACT_NONE 0
#define INTERACT_TEXT 1
#define INT_SUBTYPE_SIGN 1
#define INT_SUBTYPE_NPC 2
#define INT_STATUS_NONE 0
#define INT_STATUS_INTERACTED 1
#define MARIO_DIALOG_LOOK_UP 1
#define MARIO_DIALOG_LOOK_FRONT 2
#define MARIO_DIALOG_LOOK_DOWN 3
#define DIALOG_FLAG_TURN_TO_MARIO 2
#define CUTSCENE_DIALOG 3
#define PS_TOAD_CONVERSATIONS 99
#define SOUND_GENERAL_POUND_WOOD_POST 15
#define SOUND_ACTION_READ_SIGN 12
#define SOUND_OBJ_BOWSER_WALK 13
#define SOUND_OBJ2_BOWSER_ROAR 14
#define SHAKE_ENV_BOWSER_JUMP 1
#define DIALOG_FLAG_TEXT_DEFAULT 1
#define SOUND_ACTION_TELEPORT 10
#define SOUND_OBJ_BOWSER_LAUGH 11
#define CASTLE_SECRET_MOTOS_TALKED 256
#define CASTLE_LOBBY_CONVERSATION_COUNT 10
#define MODEL_CASTLE_LOBBY_PEACH 6
#define MODEL_CASTLE_LOBBY_BOBOMB 7
#define MODEL_CASTLE_LOBBY_KOOPA 8
#define MODEL_YOSHI_CREDIT_SIGN 9
#define MODEL_CASTLE_LOBBY_BOWSER 10
#define MODEL_CASTLE_LOBBY_PIRANHA 11
#define MODEL_CASTLE_LOBBY_MOTOS 12
#define MODEL_TOAD 13
#define MODEL_CASTLE_LOBBY_MIPS 14
struct Animation {int unused;};
/* ANIMATIONS */
struct Object {
 int oAction,oSubAction,oTimer,oOpacity,oIntangibleTimer,oInteractType,oInteractStatus;
 uint32_t oBehParams;
 int oBehParams2ndByte,oInteractionSubtype,oFaceAngleYaw,oMoveAngleYaw;
 float oPosX,oPosY,oPosZ,oVelY,oPiranhaPlantScale,scale,hitboxRadius,hitboxHeight;
 float oSignpostSinkDepth,oSignpostTargetSinkDepth;
 int oSignpostMarioPounding;
 void *oAnimations,*collisionData;
 const void *behavior;
};
struct MarioState {int unused;};
static struct MarioState mario;
static struct Object controller,guests[10],*o;
static const int bhvCastleLobbyGuest[1],wooden_signpost_seg3_collision_0302DD80[1];
static int active=1,rejection=-1,deleted,spawnCount,exhausted,teleports,laughs,dialogCalls,dialogFinished;
static int lastDialog,nativeGrowths,collisionLoads,gCurrSaveFileNum=1;
static int savedProgress,saveWrites,pushCalls,animation=-1,animationFrame;
static int talkSounds,impacts,roars,shakes,dust,toadConversations,pounding;
static uint32_t savedSecrets;
static float gGlobalSoundSource[3];
static int castle_secret_lobby_active(void) {return active;}
static int castle_secret_lobby_rejection_dialog(void) {return rejection;}
static void obj_mark_for_deletion(struct Object *obj) {assert(obj==o);deleted++;}
static struct Object *try_to_spawn_object(int param,float scale,struct Object *parent,int model,const void *behavior) {
 (void)model;assert(param==0 && parent==&controller && behavior==bhvCastleLobbyGuest);
 if(exhausted)return NULL;
 struct Object *obj=&guests[spawnCount++];*obj=(struct Object){.scale=scale,.behavior=behavior};return obj;
}
static int atan2s(float z,float x) {return (short)(atan2(x,z)*65536/6.283185307179586);}
#define segmented_to_virtual(p) ((void*)(p))
static void cur_obj_scale(float scale) {o->scale=scale;}
static void cur_obj_init_animation(int anim) {
 assert(anim>=0 && anim<40);
 if(animation!=anim) {animation=anim;animationFrame=0;}
}
static void cur_obj_init_animation_with_sound(int anim) {cur_obj_init_animation(anim);}
static void cur_obj_become_intangible(void) {o->oIntangibleTimer=-1;}
static void cur_obj_play_sound_2(int sound) {
 switch(sound) {case 10:teleports++;break;case 12:talkSounds++;break;
 case 13:impacts++;break;case 14:roars++;break;case 15:break;default:assert(0);}
}
static int cur_obj_check_if_near_animation_end(void) {return animationFrame>=20;}
static void set_environmental_camera_shake(int shake) {assert(shake==1);shakes++;}
static void spawn_mist_particles_variable(int a,int b,float size) {assert(!a && !b && size==60);dust++;}
static void play_sound(int sound,float *pos) {assert(sound==11 && pos==gGlobalSoundSource);laughs++;}
static int obj_has_behavior(struct Object *obj,const void *behavior) {return obj->behavior==behavior;}
static int cur_obj_is_mario_ground_pounding_platform(void) {return pounding;}
static void load_object_collision_model(void) {collisionLoads++;}
static void player_stats_add(int stat,int count) {assert(stat==99 && count==1);toadConversations++;}
static int cur_obj_update_dialog_with_cutscene(int action,int flags,int cutscene,int dialog) {
 assert(action>=1 && action<=3 && flags==2 && cutscene==3);
 o->oMoveAngleYaw+=0x800;
 dialogCalls++;lastDialog=dialog;return dialogFinished;
}
static uint32_t save_file_get_castle_secret(int file) {assert(file==0);return savedSecrets;}
static int save_file_get_castle_lobby_progress(int file) {assert(file==0);return savedProgress;}
static void save_file_set_castle_lobby_progress(int file,int progress) {
 assert(file==0 && progress==savedProgress+1 && progress<=10);savedProgress=progress;saveWrites++;
}
static void player_stats_object_reset(struct Object *obj) {assert(obj==o);nativeGrowths++;}
static u32 check_npc_talk(struct MarioState *m,struct Object *obj) {
 assert(m==&mario);obj->oInteractStatus=INT_STATUS_INTERACTED;return TRUE;
}
static u32 check_read_sign(struct MarioState *m,struct Object *obj) {
 assert(m==&mario && obj==o);return TRUE;
}
static void push_mario_out_of_object(struct MarioState *m,struct Object *obj,float pad) {
 assert(m==&mario && obj==o && pad==2);pushCalls++;
}
/* PRODUCTION */
static struct Object *spawn_guest(int index) {
 o=&controller;int before=spawnCount;bhv_castle_lobby_finale_loop();
 assert(controller.oSubAction==1 && spawnCount==before+1);
 struct Object *guest=&guests[before];o=guest;bhv_castle_lobby_guest_init();
 animation=-1;animationFrame=0;
 if(index==CASTLE_LOBBY_TOAD)assert(guest->oFaceAngleYaw==guest->oMoveAngleYaw-0x1555);
 const struct CastleLobbyGuestInfo *info=&sCastleLobbyGuests[index];
 assert(guest->oPosX==-1023+info->x && guest->oPosZ==717+info->z);
 assert(guest->hitboxRadius==info->hitboxRadius && guest->hitboxHeight==info->hitboxHeight);
 if(index==CASTLE_LOBBY_TOAD || index==CASTLE_LOBBY_MIPS || index==CASTLE_LOBBY_BOBOMB)
  assert(guest->hitboxRadius<=60 && guest->hitboxHeight<=120);
 assert(GET_BPARAM1(guest->oBehParams)==(unsigned)index);
 int soundCount=teleports;
 for(int frame=0;guest->oAction==CASTLE_LOBBY_GUEST_ARRIVING && frame<120;frame++) {
  guest->oTimer=frame;bhv_castle_lobby_guest_loop();
  if(index==CASTLE_LOBBY_TOAD)assert(guest->oFaceAngleYaw==guest->oMoveAngleYaw-0x1555);
  if(index!=CASTLE_LOBBY_BOWSER && index!=CASTLE_LOBBY_PIRANHA) {
   assert(guest->oOpacity==MIN(255,255*(frame+1)/32));
  }
  if(index==CASTLE_LOBBY_BOWSER) {
   assert(guest->oVelY>=-60 && guest->oPosY>=-200);
   if(guest->oSubAction==0)assert(animation==BOWSER_ANIM_JUMP && animationFrame==frame);
   else assert(animation==BOWSER_ANIM_JUMP_STOP);
 }
  animationFrame++;
  if(guest->oAction==CASTLE_LOBBY_GUEST_ARRIVING)assert(guest->oInteractType==0 && guest->oIntangibleTimer==-1);
 }
 assert(guest->oAction==CASTLE_LOBBY_GUEST_READY && guest->oInteractType==1);
 assert(guest->oPosY==-200 && guest->oOpacity==255 && teleports==soundCount);
 if(index==CASTLE_LOBBY_PIRANHA)assert(guest->scale==1 && guest->oPiranhaPlantScale==1);
 o=&controller;before=spawnCount;bhv_castle_lobby_finale_loop();assert(spawnCount==before);
 return guest;
}
static void finish_guest(struct Object *guest,int index) {
 o=guest;bhv_castle_lobby_guest_loop();int before=dialogCalls;int soundBefore=talkSounds;
 assert(interact_text(&mario,INTERACT_TEXT,guest));
 if(index==CASTLE_LOBBY_SIGN) {
  assert(guest->oBehParams2ndByte==DIALOG_CASTLE_LOBBY_SIGN);
  castle_lobby_finale_sign_read(guest);
 } else {
  guest->oInteractStatus=1;dialogFinished=0;bhv_castle_lobby_guest_loop();
  assert(guest->oAction==CASTLE_LOBBY_GUEST_TALKING && controller.oAction==index);
  dialogFinished=1;bhv_castle_lobby_guest_loop();assert(dialogCalls>before);
  assert(guest->oFaceAngleYaw==guest->oMoveAngleYaw-(index==CASTLE_LOBBY_TOAD?0x1555:0));
 }
 assert(controller.oAction==index+1 && controller.oSubAction==0);
 assert(guest->oAction==CASTLE_LOBBY_GUEST_DONE && guest->oInteractType==1 && guest->oIntangibleTimer==0);
 before=dialogCalls;
 for(int i=0;i<10;i++) {guest->oInteractStatus=1;bhv_castle_lobby_guest_loop();}
 assert(dialogCalls==before && controller.oAction==index+1);
 assert(talkSounds==soundBefore+(index!=CASTLE_LOBBY_SIGN));
 if(index==CASTLE_LOBBY_TOAD)assert(guest->oFaceAngleYaw==guest->oMoveAngleYaw-0x1555);
 int pushes=pushCalls;assert(!interact_text(&mario,INTERACT_TEXT,guest));assert(pushCalls==pushes+1);
}
int main(void) {
 for(int known=0;known<=1;known++) {
  controller=(struct Object){0};spawnCount=teleports=laughs=nativeGrowths=0;
  talkSounds=impacts=roars=shakes=dust=toadConversations=pounding=0;savedProgress=saveWrites=0;
  savedSecrets=known?256:0;deleted=0;active=1;rejection=-1;o=&controller;bhv_castle_lobby_finale_init();
  assert(!castle_lobby_finale_sun_ready());
  for(int timer=0;timer<450;timer++) {controller.oTimer=timer;bhv_castle_lobby_finale_loop();assert(spawnCount==0);}
  controller.oTimer=450;exhausted=1;bhv_castle_lobby_finale_loop();assert(spawnCount==0 && controller.oSubAction==0 && teleports==0);exhausted=0;
  for(int index=0;index<8;index++) {
   struct Object *guest=spawn_guest(index);assert(!castle_lobby_finale_sun_ready());
   finish_guest(guest,index);
   if(index==6)assert(lastDialog==(known?DIALOG_CASTLE_LOBBY_MOTOS_KNOWN:DIALOG_CASTLE_LOBBY_MOTOS_NEW));
  }
  assert(spawnCount==8 && nativeGrowths==1 && laughs==1 && teleports==6);
  assert(impacts==1 && roars==1 && shakes==1 && dust==1);
  o=&controller;bhv_castle_lobby_finale_loop();assert(spawnCount==8 && controller.oSubAction==1);
  assert(GET_BPARAM1(guests[0].oBehParams)==8 && guests[0].oAction==1);
  finish_guest(&guests[0],8);assert(lastDialog==DIALOG_CASTLE_LOBBY_PEACH_FINALE);
  struct Object *mips=spawn_guest(9);assert(mips->oFaceAngleYaw==0x8000 && mips->oPosX==-1023 && mips->oPosZ==717);
  assert(!castle_lobby_finale_sun_ready());finish_guest(mips,9);assert(castle_lobby_finale_sun_ready());
  assert(lastDialog==DIALOG_CASTLE_LOBBY_MIPS && teleports==7);
  // Native post sinking must keep the resized sign's top at the same world height.
  o=&guests[CASTLE_LOBBY_SIGN];float signTop=o->oPosY+o->hitboxHeight;
  for(int pound=0;pound<3;pound++) {
   pounding=1;bhv_castle_lobby_guest_loop();pounding=0;
   for(int frame=0;frame<10;frame++)bhv_castle_lobby_guest_loop();
   assert(o->oPosY+o->hitboxHeight==signTop);
   assert(o->hitboxHeight==150+MIN(30*(pound+1),60));
  }
  assert(castle_lobby_finale_sun_alpha()==0);
  o=&controller;
  for(int frame=1;frame<=40;frame++) {
   bhv_castle_lobby_finale_loop();
   assert(castle_lobby_finale_sun_alpha()==255*MIN(frame,30)/30);
  }
  assert(savedProgress==10 && saveWrites==10);
  o=&controller;bhv_castle_lobby_finale_loop();assert(spawnCount==9);
  active=0;assert(!castle_lobby_finale_sun_ready());active=1;rejection=1;assert(!castle_lobby_finale_sun_ready());
  o=&controller;bhv_castle_lobby_finale_init();assert(deleted==1 && !castle_lobby_finale_sun_ready());
  rejection=-1;bhv_castle_lobby_finale_init();assert(castle_lobby_finale_sun_ready());
 }
 // Every completed prefix survives re-entry; restoration is silent and never repeats a speech.
 for(int progress=0;progress<=10;progress++) {
  controller=(struct Object){0};savedProgress=progress;spawnCount=teleports=saveWrites=0;
  o=&controller;bhv_castle_lobby_finale_init();assert(controller.oAction==progress);
  assert(castle_lobby_finale_sun_ready()==(progress==10));
  int restored=progress==10?9:MIN(progress,8);
  for(int i=0;i<restored;i++) {
   o=&controller;int before=spawnCount;exhausted=1;bhv_castle_lobby_finale_loop();
   assert(spawnCount==before);exhausted=0;bhv_castle_lobby_finale_loop();assert(spawnCount==before+1);
   o=&guests[before];bhv_castle_lobby_guest_init();bhv_castle_lobby_guest_loop();
   assert(o->oAction==CASTLE_LOBBY_GUEST_DONE && o->oOpacity==255 && o->oIntangibleTimer==0);
   assert(o->oInteractionSubtype==0 && o->oInteractType==INTERACT_TEXT && o->oPosY==-200);
   int index=GET_BPARAM1(o->oBehParams);
   assert(o->hitboxRadius==sCastleLobbyGuests[index].hitboxRadius);
   assert(o->hitboxHeight==sCastleLobbyGuests[index].hitboxHeight);
   if(index==CASTLE_LOBBY_TOAD)assert(o->oFaceAngleYaw==o->oMoveAngleYaw-0x1555);
   assert(!interact_text(&mario,INTERACT_TEXT,o));
  }
  assert(teleports==0 && saveWrites==0 && controller.oAction==progress);
  assert(toadConversations==1);
  if(progress==8) {
   o=&controller;bhv_castle_lobby_finale_loop();assert(guests[0].oAction==CASTLE_LOBBY_GUEST_READY);
   o=&guests[0];bhv_castle_lobby_guest_loop();assert(o->oInteractionSubtype==INT_SUBTYPE_NPC);
  }
 }
}
'''
        code = code.replace('/* ANIMATIONS */', declarations)
        dispatch = Source(ROOT / "src/game/interaction.c").function("interact_text")
        code = code.replace('/* PRODUCTION */', dispatch + native_growth + native_sign + feature)
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=undefined',
                            '-I', str(ROOT / 'include'), str(folder / 'test.c'), '-lm',
                            '-o', str(folder / 'test')], check=True)
            subprocess.run([str(folder / 'test')], check=True)

    def test_saved_progress_and_real_star_gate(self):
        save = Source(ROOT / 'src/game/save_file.c')
        header = Source(ROOT / 'src/game/save_file.h')
        functions = header.macros('NUM_SAVE_FILES|SAVE_CAKE_.*|CASTLE_LOBBY_CONVERSATION_COUNT|SAVE_FLAG_TO_STAR_FLAG')
        functions += header.declaration('enum', 'SaveFileIndex')
        functions += header.declaration('enum', 'SaveProgressFlags')
        functions += re.search(r'#ifdef COMPLETE_SAVE_FILE\ns32 save_file_get_course_star_count.*?(?=// Read saved star bits)', save.text, re.S)[0] + '\n'
        functions += re.search(r'#ifdef COMPLETE_SAVE_FILE\nu32 save_file_get_star_flags.*?#endif', save.text, re.S)[0] + '\n'
        for name in ('mark_save_file_modified', 'save_file_get_saved_star_count', 'save_file_get_castle_lobby_progress',
                     'save_file_set_castle_lobby_progress', 'save_file_copy', 'save_file_erase'):
            functions += save.function(name)
        functions += Source(ROOT / 'src/game/level_update.c').function('castle_secret_lobby_rejection_dialog')
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "course_table.h"
typedef int s32;
typedef uint32_t u32;
typedef unsigned char u8;
#define UNUSED __attribute__((unused))
#define TRUE 1
#define FALSE 0
#define DIALOG_CASTLE_LOBBY_UNEARNED_ENDING 17
#define DIALOG_CASTLE_LOBBY_DEBUG_STARS 18
#define CASTLE_ROOF_COMPLETION_STARS 120
#define bcopy(src,dst,size) memcpy(dst,src,size)
#define bzero(dst,size) memset(dst,0,size)
struct SaveFile {u32 flags;u8 courseStars[COURSE_COUNT];};
static struct {struct SaveFile files[4][2];} gSaveBuffer;
static struct SaveFile disk[4];
static void *gCurrDemoInput,*gCurrCreditsEntry;
static int gSaveFileModified,writes,gCurrSaveFileNum=1,gDebugLevelSelect;
static u8 sModifiedFiles;
static u8 sUnreadFiles;
static u8 sFailedSaveFiles;
static int castle_secret_lobby_active(void) {return 1;}
static int save_file_write_progress(int file) {
 assert(file>=0 && file<4);
 if(sUnreadFiles & (1u << file))return FALSE;
 if(sModifiedFiles & (1u << file)) {
  writes++;disk[file]=gSaveBuffer.files[file][1]=gSaveBuffer.files[file][0];
  sModifiedFiles &= ~(1u << file);sFailedSaveFiles &= ~(1u << file);
  gSaveFileModified=sModifiedFiles != 0;
 }
 return TRUE;
}
static void save_file_do_save(int file) {
 assert(save_file_write_progress(file));
}
static void touch_high_score_ages(int file) {assert(file>=0 && file<4);}
#define game_timing_copy(src,dst) ((void)0)
#define game_options_copy(src,dst) ((void)0)
#define game_options_can_copy(src) ((src) >= 0 && (src) < 4)
#define game_timing_can_copy(src) ((src) >= 0 && (src) < 4)
#define player_stats_can_copy(src) ((src) >= 0 && (src) < 4)
#define game_timing_erase(file) ((void)0)
#define game_options_erase(file) ((void)0)
static void player_stats_copy(int src,int dst) {assert(src>=0 && dst<4);}
static void player_stats_erase(int file) {assert(file>=0 && file<4);}
u32 save_file_get_star_flags(s32 fileIndex,s32 courseIndex);
/* PRODUCTION */
int main(void) {
 assert(save_file_get_saved_star_count(-1)==0 && save_file_get_saved_star_count(4)==0);
 assert(save_file_get_castle_lobby_progress(-1)==0 && save_file_get_castle_lobby_progress(4)==0);
 gSaveBuffer.files[0][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_CAKE_END)]=0x80;
 for(int stage=1;stage<=10;stage++) {
  save_file_set_castle_lobby_progress(0,stage);assert(writes==stage);
  assert(save_file_get_castle_lobby_progress(0)==stage && save_file_get_saved_star_count(0)==0);
  assert((disk[0].courseStars[COURSE_NUM_TO_INDEX(COURSE_CAKE_END)] & 0x80)!=0);
  assert(castle_secret_lobby_rejection_dialog()==DIALOG_CASTLE_LOBBY_UNEARNED_ENDING);
#ifndef COMPLETE_SAVE_FILE
  assert(save_file_get_total_star_count(0,0,COURSE_MAX-1)==0);
#endif
  gSaveBuffer.files[0][0]=(struct SaveFile){0};
  gSaveBuffer.files[0][0]=gSaveBuffer.files[0][1];assert(save_file_get_castle_lobby_progress(0)==stage);
  save_file_set_castle_lobby_progress(0,stage);save_file_set_castle_lobby_progress(0,stage-1);
  assert(writes==stage);assert(save_file_get_castle_lobby_progress(1)==0);
 }
 save_file_set_castle_lobby_progress(0,11);save_file_set_castle_lobby_progress(-1,10);assert(writes==10);
 gSaveBuffer.files[0][0].flags=0xBF000001u; // Five stars, character metadata and completed-game bit.
 for(int i=0;i<15;i++)gSaveBuffer.files[0][0].courseStars[i]=0x7f;
 for(int i=15;i<24;i++)gSaveBuffer.files[0][0].courseStars[i]=0x81;
 gSaveBuffer.files[0][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_PSS)]|=2;
 assert(save_file_get_saved_star_count(0)==120);
#if defined(COMPLETE_SAVE_FILE) || defined(UNLOCK_ALL)
 assert(castle_secret_lobby_rejection_dialog()==DIALOG_CASTLE_LOBBY_DEBUG_STARS);
#else
 assert(castle_secret_lobby_rejection_dialog()==-1);
#endif
 gDebugLevelSelect=1;assert(castle_secret_lobby_rejection_dialog()==DIALOG_CASTLE_LOBBY_DEBUG_STARS);
 save_file_copy(0,1);assert(save_file_get_castle_lobby_progress(1)==10);
 assert(save_file_get_saved_star_count(1)==120 && save_file_get_castle_lobby_progress(0)==10);
 save_file_erase(1);assert(save_file_get_castle_lobby_progress(1)==0 && save_file_get_saved_star_count(1)==0);
 gCurrDemoInput=(void*)1;save_file_set_castle_lobby_progress(1,1);assert(save_file_get_castle_lobby_progress(1)==0);
 gCurrDemoInput=NULL;gCurrCreditsEntry=(void*)1;save_file_set_castle_lobby_progress(1,1);assert(save_file_get_castle_lobby_progress(1)==0);
}
'''
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code.replace('/* PRODUCTION */', functions))
            for flags in ([], ['-DCOMPLETE_SAVE_FILE'], ['-DUNLOCK_ALL']):
                subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=undefined',
                                '-I', str(ROOT / 'include'), '-I', str(ROOT), *flags,
                                str(folder / 'test.c'), '-o', str(folder / 'test')], check=True)
                subprocess.run([str(folder / 'test')], check=True)

    def test_requested_dialogue_pages_fit(self):
        expected = json.loads((ROOT / 'tests/castle_lobby_finale_dialogs.json').read_text())
        text = (ROOT / 'text/simpleflips_dialogs.h').read_text()
        table = (ROOT / 'src/game/ingame_menu.c').read_text().split('u8 gDialogCharWidths[256] = {')[1].split('\n};')[0]
        table = re.sub(r'#ifdef VERSION_EU\n.*?#else\n(.*?)#endif', r'\1', table, flags=re.S)
        widths = [int(n) for n in re.findall(r'\b\d+\b', re.sub(r'//[^\n]*', '', table))]
        chars = {m[1]: int(m[2], 16) for line in (ROOT / 'charmap.txt').read_text().splitlines()
                 if (m := re.match(r"'(.*?)' = (0x[\dA-Fa-f]+)", line)) and len(m[1]) == 1}
        chars["'"] = 0x3E
        for name, sentences in expected.items():
            body = re.search(r'DEFINE_DIALOG\(DIALOG_CASTLE_LOBBY_' + name + r', 1, 3, 30, 200,\s*_\((.*?)\)\)', text, re.S)[1]
            lines = ''.join(json.loads(literal) for literal in re.findall(r'"(?:\\.|[^"\\])*"', body)).split('\n')
            pages = [' '.join(line for line in lines[i:i+3] if line) for i in range(0, len(lines), 3)]
            self.assertEqual(pages, sentences, name)
            for line in lines:
                self.assertLessEqual(sum(widths[chars[c]] for c in line), 120, (name, line))

    def test_guest_assets_are_current_and_preserve_joints(self):
        subprocess.run(['python3', str(ROOT / 'tools/generate_castle_lobby_guests.py'), '--check'], check=True)
        generated = (ROOT / 'levels/castle_inside/areas/4/guest_geos.inc.c').read_text()
        self.assertEqual(generated.count('GEO_ANIMATED_PART('), 2 * sum(
            (ROOT / 'actors' / actor / 'geo.inc.c').read_text().count('GEO_ANIMATED_PART(')
            for actor in ('peach', 'bobomb', 'koopa', 'bowser', 'piranha_plant', 'motos', 'mips')))
        for body in re.findall(r'const GeoLayout \w+_fade\[\] = \{(.*?)\n\};', generated, re.S):
            self.assertEqual(set(re.findall(r'\bLAYER_\w+\b', body)) - {'LAYER_TRANSPARENT'}, set())
        self.assertIn('GEO_BRANCH(1, castle_lobby_mips_geo_opaque)', generated)
        self.assertIn('GEO_BRANCH(1, castle_lobby_mips_geo_fade)', generated)
        layouts = dict(re.findall(r'const GeoLayout (\w+)\[\] = \{(.*?)\n\};', generated, re.S))
        for name, body in layouts.items():
            if 'geo_switch_castle_lobby_guest_opacity' not in body:
                continue
            children = []
            depth = 0

            def expand(layout):
                nonlocal depth
                for command, args in re.findall(r'GEO_(\w+)\(([^()\n]*)\)', layout):
                    if command == 'OPEN_NODE':
                        depth += 1
                    elif command == 'CLOSE_NODE':
                        depth -= 1
                    elif command == 'BRANCH':
                        expand(layouts[args.split(',')[1].strip()])
                    elif command not in ('END', 'RETURN') and depth == 1:
                        children.append(command)

            expand(body)
            # Each switch child must contain both its model and its alpha cleanup.
            self.assertEqual(children, ['NODE_START', 'NODE_START'], name)
            self.assertEqual(depth, 0, name)


if __name__ == '__main__':
    unittest.main()
