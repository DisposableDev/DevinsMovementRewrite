"""Extract native effect callbacks and check inactive/active mission transitions."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class MissionEffects(unittest.TestCase):
    def test_effect_lifecycle(self):
        callbacks = Source(ROOT / 'src/game/level_geo.c').function('geo_envfx_main')
        callbacks += Source(ROOT / 'src/game/moving_texture.c').function('geo_movtex_draw_water_regions')
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef int s32; typedef unsigned u32; typedef short s16;
typedef float Vec3f[3]; typedef short Vec3s[3]; typedef float Mat4[4][4];
typedef struct { int command; } Gfx; typedef struct { int value[16]; } Mtx;
struct GraphNode { int flags; };
struct GraphNodeGenerated { struct { struct GraphNode node; } fnNode; u32 parameter; };
struct Camera { Vec3f focus, pos; } camera, *gCurGraphNodeCamera = &camera;
struct Player { Vec3f pos; } player, *gPlayerCameraState = &player;
struct { Vec3f goalPos; } gLakituState;
s32 gAreaUpdateCounter, gCurrLevelNum, gCurrActNum, updates, conversions, allocations, quads;
s16 gEnvFxBubbleConfig[10], regions[7] = {1, 0, 0, 0, 0, 0, 1024}, *gEnvironmentRegions = regions;
s16 gMovtexVtxColor, gMovetexLastTextureId;
Vec3s gVec3sZero;
Gfx output[100], dl_waterbox_end[1];
#define GEO_CONTEXT_RENDER 1
#define GEO_CONTEXT_AREA_INIT 2
#define ENVFX_MODE_NONE 0
#define ENVFX_JETSTREAM_BUBBLES 12
#define ENVFX_STATE_PARTICLECOUNT 7
#define LEVEL_JRB 12
#define JRB_MOVTEX_INITIAL_MIST 2
#define HMC_MOVTEX_TOXIC_MAZE_MIST 3
#define SSL_MOVTEX_TOXBOX_QUICKSAND_MIST 4
#define MOVTEX_VTX_COLOR_DEFAULT 0
#define MOVTEX_VTX_COLOR_YELLOW 1
#define MOVTEX_VTX_COLOR_RED 2
#define LAYER_OCCLUDE_SILHOUETTE_ALPHA 1
#define LAYER_TRANSPARENT_INTER 2
#define UNUSED
#define GET_HIGH_U16_OF_32(x) ((x) >> 16)
#define GET_LOW_U16_OF_32(x) ((x) & 65535)
#define SET_HIGH_U16_OF_32(x,v) ((x) = ((x) & 65535) | ((u32)(v) << 16))
#define SET_GRAPH_NODE_LAYER(x,v) ((x) = (v))
#define VIRTUAL_TO_PHYSICAL(x) (x)
#define G_MTX_MODELVIEW 1
#define G_MTX_LOAD 2
#define G_MTX_NOPUSH 4
#define gSPMatrix(p,a,b) ((p)->command = 1)
#define gSPBranchList(p,a) ((p)->command = 2)
#define gSPDisplayList(p,a) ((p)->command = 3)
#define gSPEndDisplayList(p) ((p)->command = 4)
void vec3f_to_vec3s(Vec3s dst, Vec3f src) { conversions++; for(int i=0;i<3;i++)dst[i]=src[i]; }
void vec3s_copy(Vec3s dst, Vec3s src) { for(int i=0;i<3;i++)dst[i]=src[i]; }
void *envfx_update_particles(int mode, Vec3s p, Vec3s c, Vec3s f) { updates++; return mode ? output : NULL; }
void *alloc_display_list(size_t n) { allocations++; return output; }
void mtxf_to_mtx(Mtx *dst, Mat4 src) {}
void *get_quad_collection_from_id(int id) { return output; }
void movtex_change_texture_format(int id, Gfx **p) {}
Gfx *movtex_gen_quads_id(int id, int y, void *collection) { quads++; return output; }
'''
        checks = r'''
int main(void) {
 Mat4 matrix = {{0}};
 struct GraphNodeGenerated node = {0};
 gCurrLevelNum = LEVEL_JRB;
 // Mission 1 stays inactive even if a previous area's request was nonzero.
 for(int act=1;act<=6;act++) {
  gCurrActNum=act; gEnvFxBubbleConfig[7]=act==1 ? 60 : 0;
  for(int frame=1;frame<=90;frame++) {
   node.parameter=ENVFX_JETSTREAM_BUBBLES; gAreaUpdateCounter=frame;
   assert(!geo_envfx_main(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
   assert(GET_HIGH_U16_OF_32(node.parameter)==frame);
  }
 }
 assert(updates==0 && conversions==0 && allocations==0);
 // Each enabled mission resumes emission; duplicate render passes stay idle.
 for(int act=2;act<=6;act++) {
  gCurrActNum=act; gEnvFxBubbleConfig[7]=60;
  node.parameter=ENVFX_JETSTREAM_BUBBLES; gAreaUpdateCounter=100+act;
  assert(geo_envfx_main(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
  assert(!geo_envfx_main(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 }
 assert(updates==5 && conversions==15 && allocations==10);
 geo_envfx_main(GEO_CONTEXT_AREA_INIT,(struct GraphNode *)&node,matrix);
 assert(updates==6); // Native cleanup remains reachable after every mission.
 gCurrLevelNum=9; node.parameter=ENVFX_JETSTREAM_BUBBLES; gAreaUpdateCounter++;
 gEnvFxBubbleConfig[7]=0;
 assert(geo_envfx_main(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 assert(updates==7); // Other levels retain their own effect policy.
 allocations=quads=0; node.parameter=JRB_MOVTEX_INITIAL_MIST;
 for(int act=2;act<=6;act++) {
  gCurrActNum=act; gLakituState.goalPos[1]=2000;
  assert(!geo_movtex_draw_water_regions(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 }
 gCurrActNum=1; gLakituState.goalPos[1]=1023;
 assert(!geo_movtex_draw_water_regions(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 assert(allocations==0 && quads==0);
 gLakituState.goalPos[1]=1024;
 assert(geo_movtex_draw_water_regions(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 assert(allocations==1 && quads==1);
 node.parameter=1; gCurrActNum=6; gLakituState.goalPos[1]=0;
 assert(geo_movtex_draw_water_regions(GEO_CONTEXT_RENDER,(struct GraphNode *)&node,matrix));
 assert(allocations==2 && quads==2); // Ordinary water remains enabled.
 return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='jrb-mission-') as directory:
            source = Path(directory) / 'fixture.c'
            binary = Path(directory) / 'fixture'
            source.write_text(fixture + callbacks + checks)
            subprocess.run(['cc', '-std=c99', '-O1', '-fsanitize=address,undefined',
                            str(source), '-o', str(binary)], check=True)
            subprocess.run([str(binary)], check=True,
                           env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))


if __name__ == '__main__':
    unittest.main()
