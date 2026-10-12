"""Exercise native loader eligibility and visibility with observed publication calls."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class TTCSpinnerCollisionRangeTests(unittest.TestCase):
    def test_nearby_contacts_and_distant_loading(self):
        loading = Source(ROOT / "src/engine/surface_load.c")
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <math.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t TerrainData;
typedef float f32;
typedef f32 Vec3f[3];
#define TRUE 1
#define FALSE 0
#define AUTO_COLLISION_DISTANCE
#define PERSISTENT_DYNAMIC_SURFACES 0
#define COLLISION_PROFILING 0
#define TIME_STOP_ACTIVE 1
#define OBJ_FLAG_DONT_CALC_COLL_DIST 1
#define ACTIVE_FLAG_IN_DIFFERENT_ROOM 1
#define GRAPH_RENDER_ACTIVE 1
#define TERRAIN_LOAD_CONTINUE 255
#define sqr(x) ((x)*(x))
#define absf(x) fabsf(x)
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() u32 first=0
#define profiler_collision_update(x) ((void)(x))
#define COND_BIT(condition,field,bit) do { \
    if (condition) (field)|=(bit); else (field)&=~(bit); \
} while (0)
struct Object {
    union { Vec3f oPosVec; struct { f32 oPosX,oPosY,oPosZ; }; };
    f32 oCollisionDistance,oDrawingDistance,oDistanceToMario;
    u32 oFlags,activeFlags;
    const void *behavior;
    TerrainData *collisionData;
    struct { struct { struct { u32 flags; } node; } gfx; } header;
};
struct MarioState { Vec3f vel; } marioState;
struct Object platform,marioObject;
static struct Object *gCurrentObject=&platform,*gMarioObject=&marioObject;
static struct MarioState *gMarioState=&marioState;
#define o gCurrentObject
static const u32 bhvTTCSpinner[]={1},otherBehavior[]={2};
static u32 gTimeStopState;
static int needed,transforms,publications,visibilityQueries,distanceCalculations;
static TerrainData sVertexData[600],collisionData[]={0,20,0,TERRAIN_LOAD_CONTINUE};
static void *segmented_to_virtual(const void *p) { return (void *)p; }
static void vec3_diff(Vec3f out,const void *aa,const void *bb) {
    const f32 *a=aa,*b=bb;
    for (int i=0;i<3;i++) out[i]=a[i]-b[i];
}
static void get_optimal_coll_dist(struct Object *obj) {
    distanceCalculations++;
    obj->oCollisionDistance=450;
    obj->oFlags|=OBJ_FLAG_DONT_CALC_COLL_DIST;
}
static int object_platform_motion_needs_collision(struct Object *obj) {
    (void)obj;
    return needed;
}
static void object_platform_motion_refresh_transform(struct Object *obj) { (void)obj; }
static void transform_object_vertices(TerrainData **data,TerrainData *vertices) {
    (void)vertices;
    transforms++;
    *data=collisionData+2;
}
static void load_object_surfaces(TerrainData **data,TerrainData *vertices,u32 dynamic) {
    (void)vertices;
    assert(dynamic==TRUE);
    *data=collisionData+3;
}
static void object_platform_motion_publish(struct Object *obj) {
    (void)obj;
    publications++;
}
static f32 dist_between_objects(struct Object *a,struct Object *b) {
    return sqrtf(sqr(a->oPosX-b->oPosX)+sqr(a->oPosY-b->oPosY)+sqr(a->oPosZ-b->oPosZ));
}
static f32 cur_obj_drawing_distance(f32 distance) {
    (void)distance;
    visibilityQueries++;
    return o->oDrawingDistance;
}
/* LOADER */
static void reset(f32 height,f32 speed) {
    platform=(struct Object){0};
    marioObject=(struct Object){0};
    marioState=(struct MarioState){0};
    platform.behavior=bhvTTCSpinner;
    platform.collisionData=collisionData;
    platform.oCollisionDistance=450;
    platform.oDrawingDistance=3000;
    platform.oDistanceToMario=19000;
    platform.oFlags=OBJ_FLAG_DONT_CALC_COLL_DIST;
    marioObject.oPosY=height;
    marioState.vel[1]=speed;
    needed=transforms=publications=visibilityQueries=distanceCalculations=0;
    gTimeStopState=0;
}
static void check_load(int expected) {
    load_object_collision_model();
    assert(transforms==expected && publications==expected);
    assert(visibilityQueries==1);
}
int main(void) {
    // The old allowance rebuilds spinners over two thousand units below Mario.
    reset(2200,-74);
    check_load(0);
    assert(platform.header.gfx.node.flags&GRAPH_RENDER_ACTIVE);
    reset(700,-74);
    check_load(1);
    reset(700,74);
    check_load(1);
    reset(730,-74);
    check_load(0);
    // Speed padding retains collision before a faster descent reaches the mesh.
    reset(800,-200);
    check_load(1);
    reset(800,200);
    check_load(1);
    // Existing below-platform/headroom and horizontal eligibility stay active.
    reset(-449,40);
    check_load(1);
    reset(-450,40);
    check_load(0);
    reset(0,0);
    marioObject.oPosX=450;
    check_load(0);
    // Other objects retain the original vertical allowance.
    reset(2200,-74);
    platform.behavior=otherBehavior;
    check_load(1);
    // Provider requirements override Mario distance without changing frozen/room rules.
    reset(2200,-74);
    needed=1;
    check_load(1);
    reset(0,0);
    gTimeStopState=TIME_STOP_ACTIVE;
    check_load(0);
    reset(0,0);
    platform.activeFlags=ACTIVE_FLAG_IN_DIFFERENT_ROOM;
    check_load(0);
    // Auto radius, drawing-distance extension and visibility still run when loading is skipped.
    reset(2200,-74);
    platform.oFlags=0;
    platform.oCollisionDistance=1;
    platform.oDrawingDistance=1;
    check_load(0);
    assert(distanceCalculations==1 && platform.oDrawingDistance==450);
    assert(!(platform.header.gfx.node.flags&GRAPH_RENDER_ACTIVE));
    return 0;
}
'''
        loader = loading.function("load_object_collision_model")
        # Bowser-only consumers are disabled in this native TTC loader fixture.
        loader = loader.replace("s32 onlyTop = FALSE;", "s32 onlyTop = FALSE; (void) onlyTop;")
        code = code.replace("/* LOADER */", loader)
        with tempfile.TemporaryDirectory(prefix="ttc-spinner-range-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code)
            subprocess.run([
                os.environ.get("CC", "cc"), "-std=gnu17", "-O1", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                str(path / "test.c"), "-lm", "-o", str(path / "test"),
            ], check=True)
            subprocess.run([str(path / "test")], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
