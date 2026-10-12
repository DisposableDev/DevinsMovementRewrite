"""Secret pool geometry and room-local C-Up cake routing."""
from collections import Counter
from pathlib import Path
import math
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from regression_support import Source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
AREA = ROOT / 'levels/castle_inside/areas/4'


def triples(text, macro):
    return [tuple(map(int, values)) for values in re.findall(
        macro + r'\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)', text)]


def normal(a, b, c):
    u = [b[i] - a[i] for i in range(3)]
    v = [c[i] - a[i] for i in range(3)]
    return (u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0])


class SecretLobbyTests(unittest.TestCase):
    def test_generator_preserves_current_lobby_assets(self):
        with tempfile.TemporaryDirectory(prefix='secret-lobby-generator-') as directory:
            root = Path(directory)
            for relative in ('tools/generate_secret_lobby.py',
                             'levels/castle_inside/areas/1/2/model.inc.c',
                             'src/game/level_update.h'):
                target = root / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(ROOT / relative, target)
            area = root / AREA.relative_to(ROOT)
            area.mkdir(parents=True)
            subprocess.run([sys.executable, str(root / 'tools/generate_secret_lobby.py')],
                           check=True, capture_output=True, text=True)
            for name in ('collision.inc.c', 'movtext.inc.c', 'platform.inc.c',
                         'pool.inc.c', 'sunbeam.inc.c'):
                self.assertEqual((area / name).read_bytes(), (AREA / name).read_bytes(), name)

    def test_rejection_sign_stops_only_music(self):
        level = Source(ROOT / 'src/game/level_update.c')
        audio = Source(ROOT / 'src/game/sound_init.c')
        branch = re.search(r'if \(castle_secret_lobby_rejection_dialog\(\) >= 0\) \{.*?\n            \}',
                           level.function('init_level'), re.S)[0]
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
typedef short s16;
#define SEQ_PLAYER_LEVEL 0
#define SEQ_PLAYER_ENV 1
#define SEQ_PLAYER_SFX 2
#define MUSIC_NONE -1
static int enabled[3]={1,1,1},volume[3]={100,100,100};
static int sCurrentMusic=10,sCurrentShellMusic=11,sCurrentCapMusic=12;
static int castle_secret_lobby_rejection_dialog(void) {return 1;}
static void seq_player_fade_out(int player,int frames) {
 assert(frames==1 && player!=SEQ_PLAYER_SFX);enabled[player]=0;
}
void fadeout_music(int frames) {
 (void)frames;enabled[0]=enabled[1]=enabled[2]=0;volume[2]=0;
}
/* MUSIC HELPER */
static void enter_rejected_lobby(void) {
/* ENTRY BRANCH */
}
int main(void) {
 enter_rejected_lobby();
 assert(!enabled[SEQ_PLAYER_LEVEL] && !enabled[SEQ_PLAYER_ENV]);
 assert(enabled[SEQ_PLAYER_SFX] && volume[SEQ_PLAYER_SFX]==100);
 assert(sCurrentMusic==MUSIC_NONE && sCurrentShellMusic==MUSIC_NONE && sCurrentCapMusic==MUSIC_NONE);
}
'''
        code = code.replace('/* MUSIC HELPER */', audio.function('fadeout_level_music'))
        code = code.replace('/* ENTRY BRANCH */', branch)
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=undefined',
                            str(folder / 'test.c'), '-o', str(folder / 'test')], check=True)
            subprocess.run([str(folder / 'test')], check=True)

    def test_boundary_pushes_without_action_contacts(self):
        collision = Source(ROOT / 'src/engine/surface_collision.c')
        mario_collision = Source(ROOT / 'src/game/mario_collision.c')
        header = Source(ROOT / 'include/surface_terrains.h')
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
#include <math.h>
#include <stddef.h>
#include <string.h>
typedef float f32;
typedef int s32;
typedef int TerrainData;
typedef float Vec3f[3];
#define TRUE 1
#define FALSE 0
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define sqr(x) ((x)*(x))
#define FLT_IS_NONZERO(x) ((x)!=0)
#define MAX_REFERENCED_WALLS 4
#define COLLISION_FLAG_CAMERA 1
#define COLLISION_FLAG_RETURN_FIRST 2
#define MARIO_VANISH_CAP 1
#define ACTIVE_FLAG_MOVE_THROUGH_GRATE 1
#define RAYCAST_FIND_WALL 1
#define RAYCAST_MARIO_WALL 2
#define RAYCAST_IGNORE_VANISH_CAP_WALL 4
#define WALL_HEIGHT_GROUP_SCAN 0
/* SURFACES */
struct Surface {int type,flags,lowerY,upperY; Vec3f vertex1,vertex2,vertex3;
 struct {float x,y,z;} normal; float originOffset;};
struct WallCollisionData {float x,y,z,offsetY,radius; int numWalls; struct Surface *walls[4];};
struct WallCollisionCache {float minX,maxX,minZ,maxZ;};
struct SurfaceQueryIterator {struct Surface *next;};
struct Object {int activeFlags;};
struct MarioState {Vec3f pos; int flags; struct Object *marioObj;};
static int gCollisionFlags;
static struct Surface wall;
static struct Surface *surface_query_next(struct SurfaceQueryIterator *it) {
 struct Surface *s=it->next;it->next=NULL;return s;
}
static int surface_is_passable_vanish(struct Surface *s) {(void)s;return FALSE;}
static void vec3_diff(Vec3f out,Vec3f a,Vec3f b) {for(int i=0;i<3;i++)out[i]=a[i]-b[i];}
static void vec3f_diff(Vec3f out,Vec3f a,Vec3f b) {vec3_diff(out,a,b);}
static void vec3f_copy(Vec3f out,Vec3f in) {memcpy(out,in,sizeof(Vec3f));}
static float vec3_dot(Vec3f a,Vec3f b) {return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
static float find_surface_on_ray(Vec3f origin,Vec3f direction,struct Surface **hit,Vec3f point,int flags) {
 (void)flags;assert(origin[0]==0 && direction[0]==100);
 *hit=&wall;point[0]=50;point[1]=origin[1];point[2]=origin[2];return 50;
}
static void mario_reject_crossed_wall_push(struct MarioState *m,Vec3f p,float low,float high) {
 (void)m;(void)p;(void)low;(void)high;
}
/* PRODUCTION */
int main(void) {
 wall=(struct Surface){.normal={-1,0,0},.originOffset=50,.lowerY=-500,.upperY=500,
  .vertex1={50,-500,-4000},.vertex2={50,-500,4000},.vertex3={50,500,0}};
 for(int boundary=0;boundary<2;boundary++) {
  wall.type=boundary?SURFACE_NON_INTERACTIVE_BOUNDARY:SURFACE_DEFAULT;
  wall.flags=boundary?SURFACE_FLAG_NO_CAM_COLLISION:0;
  struct WallCollisionData data={.x=35,.radius=50};
  int count=find_wall_collisions_from_list((struct SurfaceQueryIterator){&wall},&data,NULL,NULL);
  assert(fabsf(data.x)<.001f && count==!boundary && data.numWalls==!boundary);
  struct Object object={0};struct MarioState m={.marioObj=&object};
  Vec3f next={100,0,0};struct Surface *lower,*upper;
  assert(raycast_mario_crossed_wall(&m,next,30,150,&lower,&upper,TRUE));
  assert(fabsf(next[0])<.001f);
  assert((lower!=NULL)==!boundary && (upper!=NULL)==!boundary);
  next[0]=100;data.numWalls=0;
  assert(mario_resolve_air_step_floor_side(&m,next,&data));
  assert(fabsf(next[0])<.001f && data.numWalls==!boundary);
  data=(struct WallCollisionData){.x=35,.radius=50};gCollisionFlags=COLLISION_FLAG_CAMERA;
  count=find_wall_collisions_from_list((struct SurfaceQueryIterator){&wall},&data,NULL,NULL);
  assert(count==!boundary && fabsf(data.x-(boundary?35:0))<.001f);
  gCollisionFlags=0;
 }
}
'''
        definitions = header.declaration('enum', 'SurfaceTypes')
        definitions += header.declaration('enum', 'SurfaceFlags')
        functions = collision.function('check_wall_vw') + collision.function('check_wall_edge')
        functions += read_source(ROOT / 'src/engine/surface_wall.inc.c')
        functions += mario_collision.function('raycast_mario_crossed_wall')
        functions += mario_collision.function('mario_resolve_air_step_floor_side')
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            text = code.replace('/* SURFACES */', definitions)
            (folder / 'test.c').write_text(text.replace('/* PRODUCTION */', functions))
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=undefined',
                            str(folder / 'test.c'), '-lm', '-o', str(folder / 'test')], check=True)
            subprocess.run([str(folder / 'test')], check=True)

    def test_closed_thick_boundary_and_trimmed_platform(self):
        collision = read_source(AREA / 'collision.inc.c')
        vertices = triples(collision, 'COL_VERTEX')
        faces = triples(collision, 'COL_TRI')
        self.assertEqual(len(faces), 168)
        edges = Counter()
        boundary = [face for i in range(8) for face in faces[24+i*9:32+i*9]]
        for face in boundary:
            for i in range(3):
                edges[tuple(sorted((face[i], face[(i+1)%3])))] += 1
        self.assertTrue(all(count == 2 for count in edges.values()))
        for i in range(8):
            sector = faces[24+i*9:32+i*9]
            for j, face in enumerate(sector):
                a, b, c = [vertices[k] for k in face]
                n = normal(a, b, c)
                if j < 4:
                    inward = n[0]*(-1023-(a[0]+b[0]+c[0])/3) + n[2]*(717-(a[2]+b[2]+c[2])/3)
                    self.assertGreater(inward if j < 2 else -inward, 0)
                else:
                    self.assertGreater(n[1], 0)  # Bottom is one-way for upward re-entry.
            self.assertEqual(min(vertices[k][1] for f in sector for k in f), -5600)
            self.assertEqual(max(vertices[k][1] for f in sector for k in f), 19900)
        groups = re.findall(r'COL_TRI_INIT\((\w+), (\d+)\)', collision)
        self.assertEqual(groups, [('SURFACE_DEFAULT', '24')]
                         + [('SURFACE_NON_INTERACTIVE_BOUNDARY', '4'), ('SURFACE_DEFAULT', '5')] * 8
                         + [('SURFACE_NON_INTERACTIVE_BOUNDARY', '8')]
                         + [('SURFACE_NON_INTERACTIVE_BOUNDARY', '4'), ('SURFACE_DEFAULT', '4')] * 8)
        for face in faces[96:104]:
            self.assertLess(normal(*(vertices[k] for k in face))[1], 0)
            self.assertEqual({vertices[k][1] for k in face}, {19900})
        # The basin bottom is upward-facing only, so swimmers below can rise through.
        for i in range(8):
            bottom = faces[32 + i * 9]
            self.assertGreater(normal(*(vertices[k] for k in bottom))[1], 0)
            self.assertEqual({vertices[k][1] for k in bottom}, {-2800})
        model = read_source(AREA / 'platform.inc.c')
        positions = [tuple(map(int, p)) for p in re.findall(r'\{\s*\{\s*\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\}', model)]
        self.assertEqual(len(positions), 72)
        self.assertEqual(positions, [vertices[k] for f in faces[:24] for k in f])
        for x, y, z in positions:
            dx, dz = x+1023, z-717
            self.assertLessEqual(max(abs(dx), abs(dz)), 2048 + 2.04*y + 2)
            self.assertLessEqual(max(abs(dx+dz), abs(dx-dz)), 2868 + 2.74*y + 2)
        pool = read_source(AREA / 'pool.inc.c')
        self.assertEqual(len(re.findall(r'\{\s*\{\s*\{', pool)), 24)  # Floor only: bounds remain invisible.
        for line in pool.splitlines():
            if '{{{' not in line:
                continue
            x, y, z, flag, s, t, *color = map(int, re.findall(r'-?\d+', line))
            self.assertTrue(-32768 <= s <= 32767 and -32768 <= t <= 32767)
            self.assertEqual((s - (x + 1023) * 4) % 1024, 0)
            self.assertEqual((t - (z - 717) * 4) % 1024, 0)
        self.assertIn('jrb_textures.02800', pool)

        platform_walls = faces[104:]
        edges = Counter()
        for face in platform_walls:
            for i in range(3):
                edges[tuple(sorted((face[i], face[(i + 1) % 3])))] += 1
        self.assertTrue(all(count == 2 for count in edges.values()))
        for sector in range(8):
            for j, face in enumerate(platform_walls[sector * 8:sector * 8 + 4]):
                a, b, c = [vertices[k] for k in face]
                n = normal(a, b, c)
                inward = n[0] * (-1023 - a[0]) + n[2] * (717 - a[2])
                self.assertGreater(inward if j < 2 else -inward, 0)
                self.assertEqual({vertices[k][1] for k in face}, {-5600, 19900})
                if j < 2:
                    for x, _, z in (a, b, c):
                        self.assertEqual(max(abs(x + 1023), abs(z - 717)), 2048)
                        self.assertEqual(abs(x + 1023) + abs(z - 717), 2868)

    def test_carpet_matches_platform_floor_and_beam_precedes_objects(self):
        geo = read_source(AREA / 'geo.inc.c')
        scale = int(re.search(r'GEO_SCALE\(0x00, (\d+)\)', geo)[1]) / 65536
        native = read_source(ROOT / 'levels/castle_inside/areas/1/6/model.inc.c')
        carpet_y = int(re.search(r'\{\{\{\s*-757,\s*(-?\d+),', native)[1]) * scale
        self.assertEqual(carpet_y, -200)
        self.assertEqual(scale, 4)
        platform = geo.index('GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_lobby_platform_dl)')
        beam = geo.index('GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_lobby_sunbeam_dl)')
        self.assertLess(platform, beam)
        self.assertLess(beam, geo.index('GEO_RENDER_OBJ()'))
        material = read_source(AREA / 'sunbeam.inc.c')
        modes = re.findall(r'gsDPSetRenderMode\(([^)]+)\)', material)
        self.assertEqual(modes, ['G_RM_PASS, G_RM_AA_ZB_XLU_SURF2',
                                 'G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2'])

    def test_water_and_mist_leave_dry_platform_clear(self):
        text = read_source(AREA / 'movtext.inc.c')
        points = [tuple(map(int, p[:3])) for p in re.findall(
            r'MOV_TEX_TRIS\((-?\d+), (-?\d+), (-?\d+), (-?\d+), (-?\d+)\)', text)]
        self.assertEqual(len(points), 25)
        self.assertEqual({p[1] for p in points[:9]}, {-420})
        self.assertEqual({p[1] for p in points[9:]}, {0})
        camera_low = -200 + 125 - 250*math.sin(math.pi/4)
        self.assertLess(-420, camera_low)
        mist = points[9:]
        for i in range(8):
            j = (i+1)%8
            for face in [(i, i+8, j), (j, i+8, j+8)]:
                self.assertGreater(normal(*(mist[k] for k in face))[1], 0)
        for x, _, z in mist[8:]:
            self.assertEqual(max(abs(x+1023), abs(z-717)), 2048)
            self.assertEqual(abs(x+1023)+abs(z-717), 2868)

    def test_beam_has_visible_center_and_soft_edges(self):
        text = read_source(AREA / 'sunbeam.inc.c')
        vertices = [tuple(int(n) for n in re.findall(r'-?\d+', line))
                    for line in text.splitlines() if re.search(r'\{\s*\{\s*\{', line)]
        self.assertEqual(len(vertices), 24)
        self.assertIn('gsSPSetGeometryMode(G_SHADING_SMOOTH)', text)
        for start in (0, 12):
            left, center, right = vertices[start:start+3]
            self.assertEqual((left[5], center[5], right[5]), (0, 990, 0))
            self.assertGreaterEqual(center[-1], 190)
            self.assertEqual(center[1], -180)
            self.assertEqual(vertices[start+10][-1], 0)

    def test_rejection_star_priority_and_game_over_fade(self):
        level = Source(ROOT / 'src/game/level_update.c')
        functions = level.function('castle_secret_lobby_active')
        functions += level.function('castle_secret_lobby_rejection_dialog')
        functions += level.function('secret_lobby_rejection_fade_update')
        functions += level.function('level_reject_secret_lobby')
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
#include <stddef.h>
typedef int s32;
typedef short s16;
#define LEVEL_CASTLE 6
#define COURSE_MIN 1
#define COURSE_MAX 25
#define CASTLE_ROOF_COMPLETION_STARS 120
#define DIALOG_CASTLE_LOBBY_UNEARNED_ENDING 200
#define DIALOG_CASTLE_LOBBY_DEBUG_STARS 201
#define MODEL_STATE_MASK 255
#define MODEL_STATE_ALPHA 256
#define MARIO_CAP_BLINK_NONE 0
#define WARP_OP_NONE 0
#define game_timing_end_course() ((void)0)
#define WARP_SPECIAL_MARIO_HEAD_DIZZY -3
#define SOUND_ACTION_TELEPORT 0x0457C081
#define CLAMP(x,l,h) ((x)<(l)?(l):((x)>(h)?(h):(x)))
#define TRUE 1
#define FALSE 0
struct MarioBodyState {int modelState,capBlinkBlend;};
struct MarioState {struct MarioBodyState *marioBodyState;int health;int respawnHealCounter;};
static struct MarioState *gMarioState;
static int gCurrLevelNum=6,gCurrAreaIndex=4,gCurrSaveFileNum=1,gDebugLevelSelect;
static int stars,reads,sDelayedWarpOp=99,soundCount,destination,resetCount,reloads;
static int healthUpdates, deaths;
static float gGlobalSoundSource[3];
static void (*transition)(s16*);
static int frames;
static int save_file_get_saved_star_count(int file) {
 assert(file==0);reads++;return stars;
}
static void player_stats_health(int before, int after) {
 assert(before == 0x880 && after == 0 && gMarioState->health == before);
 healthUpdates++;
}
static void player_stats_death(int gameOver, int pit) {
 assert(gameOver && !pit && gMarioState->health == 0);
 assert(healthUpdates == 1 && reloads == 0);
 deaths++;
}
static void save_file_reload(void) {
 assert(gMarioState->health == 0 && healthUpdates == 1 && deaths == 1);
 reloads++;
}
static void reset_dialog_render_state(void) {resetCount++;}
static void play_sound(int sound,float *pos) {
 assert(sound==SOUND_ACTION_TELEPORT && pos==gGlobalSoundSource);soundCount++;
}
static void warp_special(int target) {destination=target;}
static void level_set_transition(int length,void(*callback)(s16*)) {
 frames=length;transition=callback;
}
/* PRODUCTION */
int main(void) {
 for(int debug=0;debug<=1;debug++) {
  gDebugLevelSelect=debug;
  for(stars=0;stars<120;stars++) {
   assert(castle_secret_lobby_rejection_dialog()==DIALOG_CASTLE_LOBBY_UNEARNED_ENDING);
  }
  stars=120;
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
  assert(castle_secret_lobby_rejection_dialog()==DIALOG_CASTLE_LOBBY_DEBUG_STARS);
#else
  assert(castle_secret_lobby_rejection_dialog()==(debug?DIALOG_CASTLE_LOBBY_DEBUG_STARS:-1));
#endif
 }
 int before=reads;gCurrAreaIndex=1;assert(castle_secret_lobby_rejection_dialog()==-1 && reads==before);
 gCurrAreaIndex=4;gCurrLevelNum=7;assert(castle_secret_lobby_rejection_dialog()==-1 && reads==before);
 gCurrLevelNum=6;stars=119;
 struct MarioBodyState body={512,2};struct MarioState mario={&body,0x880,0};gMarioState=&mario;
 level_reject_secret_lobby();
 assert(destination==-3 && frames==32 && soundCount==1 && resetCount==1 && sDelayedWarpOp==0);
 assert(mario.health==0x880 && reloads==0 && healthUpdates==0 && deaths==0);
 int previous=256;
 for(s16 timer=32;timer>=0;timer--) {
  transition(&timer);
  int alpha=body.modelState & 255;
  assert(alpha==255*timer/32 && alpha<=previous);previous=alpha;
  assert((body.modelState & 512) && (body.modelState & 256) && body.capBlinkBlend==0);
  assert(mario.health==(timer?0x880:0) && reloads==(timer?0:1));
  assert(healthUpdates == (timer ? 0 : 1) && deaths == (timer ? 0 : 1));
 }
 assert(previous==0 && healthUpdates==1 && deaths==1);
}
'''
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code.replace('/* PRODUCTION */', functions))
            for define in (None, 'UNLOCK_ALL', 'COMPLETE_SAVE_FILE'):
                command = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                           '-fsanitize=undefined', str(folder / 'test.c'), '-o', str(folder / 'test')]
                if define:
                    command.append('-D' + define)
                subprocess.run(command, check=True)
                subprocess.run([str(folder / 'test')], check=True)

    def test_native_sun_aim_and_ordinary_cake_route(self):
        stationary = Source(ROOT / 'src/game/mario_actions_stationary.c')
        level = Source(ROOT / 'src/game/level_update.c')
        header = Source(ROOT / 'src/game/level_update.h')
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() (gDebugLevelSelect)
#include <math.h>
#include <stddef.h>
typedef int s32;
typedef short s16;
#define FALSE 0
#define LEVEL_CASTLE 6
#define LEVEL_ENDING 25
#define WARP_NODE_MAIN_ENTRY 10
#define WARP_OP_NONE 0
#define game_timing_end_course() ((void)0)
#define WARP_FLAGS_NONE 0
#define WARP_FLAG_SECRET_LOBBY_CAKE 64
#define WARP_TRANSITION_FADE_INTO_COLOR 1
#define CAM_MOVING_INTO_MODE 256
#define MODEL_STATE_MASK 255
#define MODEL_STATE_ALPHA 256
#define MARIO_CAP_BLINK_NONE 0
#define CLAMP(x,l,h) ((x)<(l)?(l):((x)>(h)?(h):(x)))
struct MarioBodyState {int modelState,capBlinkBlend;};
static int gCurrLevelNum=6,gCurrAreaIndex=4,gCameraMovementFlags;
static int castle_secret_lobby_rejection_dialog(void) {return -1;}
int castle_secret_lobby_active(void);
static int sunReady=1;
static int castle_lobby_finale_sun_ready(void) {return castle_secret_lobby_active() && sunReady;}
static struct {float pos[3],focus[3];} gLakituState;
struct CameraState {short headRotation[3];};
struct MarioState {struct CameraState *statusForCamera;struct MarioBodyState *marioBodyState;};
static struct MarioState *gMarioState;
static void secret_lobby_cake_fade_update(s16 *timer);
static void *gCurrCreditsEntry,*sDispCreditsEntry;
static int sDelayedWarpOp=99,warps;
static s16 atan2s(float z,float x) {return (s16)(atan2(x,z)*65536/6.283185307179586);}
static void reset_dialog_render_state(void) {}
static void initiate_warp(int l,int a,int n,int f) {
 assert(l==25 && a==1 && n==10 && f==64);warps++;
}
static void play_transition(int t,int frames,int r,int g,int b) {
 assert(t==1 && frames==90 && r==255 && g==255 && b==255);
}
static void level_set_transition(int f,void(*callback)(s16*)) {assert(f==90 && callback==secret_lobby_cake_fade_update);}
/* PRODUCTION */
static void aim(int yaw) {
 double angle=yaw*6.283185307179586/65536;
 gLakituState.focus[0]=sin(angle)*100;gLakituState.focus[2]=cos(angle)*100;
}
int main(void) {
 struct CameraState camera={{-0x1800,0,0}};struct MarioBodyState body={512,2};struct MarioState m={&camera,&body};gMarioState=&m;
 aim(CASTLE_SECRET_LOBBY_SUN_YAW);assert(castle_secret_lobby_looking_at_sun(&m));
 sunReady=0;assert(!castle_secret_lobby_looking_at_sun(&m));level_warp_to_secret_lobby_cake();assert(warps==0);sunReady=1;
 aim(CASTLE_SECRET_LOBBY_SUN_YAW+0x700);assert(castle_secret_lobby_looking_at_sun(&m));
 aim(CASTLE_SECRET_LOBBY_SUN_YAW+0x900);assert(!castle_secret_lobby_looking_at_sun(&m));
 aim(CASTLE_SECRET_LOBBY_SUN_YAW);camera.headRotation[0]=-0x1700;assert(!castle_secret_lobby_looking_at_sun(&m));
 camera.headRotation[0]=-0x1800;gCameraMovementFlags=256;assert(!castle_secret_lobby_looking_at_sun(&m));
 gCameraMovementFlags=0;gCurrAreaIndex=1;assert(!castle_secret_lobby_looking_at_sun(&m));
 gCurrAreaIndex=4;gCurrLevelNum=7;assert(!castle_secret_lobby_looking_at_sun(&m));
 gCurrLevelNum=6;level_warp_to_secret_lobby_cake();assert(warps==1 && sDelayedWarpOp==0);
 int previous=256;
 for(s16 timer=90;timer>=0;timer--) {
  secret_lobby_cake_fade_update(&timer);
  int alpha=body.modelState & 255;
  assert(alpha<=previous && alpha==255*timer/90);previous=alpha;
  assert((body.modelState & 512) && (body.modelState & 256) && body.capBlinkBlend==0);
 }
 assert(previous==0);
}
'''
        functions = header.macros('CASTLE_SECRET_LOBBY_SUN_YAW')
        functions += level.function('castle_secret_lobby_active')
        functions += stationary.function('castle_secret_lobby_looking_at_sun')
        functions += level.function('secret_lobby_cake_fade_update')
        functions += level.function('level_warp_to_secret_lobby_cake')
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code.replace('/* PRODUCTION */', functions))
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=undefined',
                            str(folder / 'test.c'), '-lm', '-o', str(folder / 'test')], check=True)
            subprocess.run([str(folder / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
