"""Native painting trigger/coordinate checks; no RDP or emulator rendering is exercised."""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]


class CastlePaintingTests(unittest.TestCase):
    def test_room_remains_visible_during_the_interior_camera_dolly(self):
        room = ROOT / "levels/castle_grounds/areas/1/secret_room"
        layout = read_source(room / "geo.inc.c").split(
            "const GeoLayout castle_secret_room_geo[] = {", 1)[1]
        root = re.match(r"\s*(?://[^\n]*\n\s*)*GEO_CULLING_RADIUS\((\d+)\)", layout)
        graphics = read_source(ROOT / "include/config/config_graphics.h")
        default = int(re.search(r"#define DEFAULT_CULLING_RADIUS (\d+)", graphics)[1])
        radius = int(root[1]) if root else default
        model = read_source(room / "model.inc.c")
        for name, body in re.findall(r"const Vtx (\w+)\[\] = \{(.*?)\n\};", model, re.S):
            if "facade" in name or "window" in name:
                continue
            vertices = re.findall(r"\{\s*\{\s*\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)", body)
            self.assertTrue(vertices, name)
            for vertex in vertices:
                self.assertLessEqual(sum(int(axis) ** 2 for axis in vertex), radius ** 2, name)

        renderer = Source(ROOT / "src/game/rendering_graph_node.c")
        fixture = r'''
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
typedef float f32;
typedef int32_t s32;
typedef int16_t s16;
#define TRUE 1
#define FALSE 0
#define CULLING_ON_EMULATOR 1
#define DEFAULT_CULLING_RADIUS DEFAULT_RADIUS
#define GRAPH_NODE_TYPE_CULLING_RADIUS 1
#define LEVEL_JRB 7
#define LEVEL_PHYSICS_LAB 32
#define absf fabsf
struct GraphNode { int type; };
struct GraphNodeCullingRadius { struct GraphNode node; s16 cullingRadius; };
struct GraphNodeObject { struct GraphNode *sharedChild; f32 cameraToObject[3], pos[3]; int areaIndex; };
static int gCurrLevelNum=16, sJrbBayVisible=1, sJrbCaveVisible=1;
static struct { f32 halfFovHorizontal; } frustum={0.6f}, *gCurGraphNodeCamFrustum=&frustum;
static int geo_jrb_object_behind_terrain(struct GraphNodeObject *n) { (void)n; return 0; }
static int physics_lab_object_in_view(struct GraphNodeObject *n) { (void)n; return -1; }
/* FUNCTION */
int main(void) {
    struct GraphNodeCullingRadius geo={{GRAPH_NODE_TYPE_CULLING_RADIUS}, ROOM_RADIUS};
    struct GraphNodeObject room={.sharedChild=&geo.node};
    // The doorway origin moves behind the view during the actual room-entry dolly.
    for (int depth=0; depth<=600; depth+=50) {
        room.cameraToObject[2]=depth;
        for (int lateral=-200; lateral<=200; lateral+=50) {
            room.cameraToObject[0]=lateral;
            assert(obj_is_in_view(&room));
        }
    }
    room.cameraToObject[0]=0;
    room.cameraToObject[2]=1200;
    assert(!obj_is_in_view(&room));
    room.cameraToObject[2]=300;
    room.sharedChild=0;
    assert(!obj_is_in_view(&room));
}
'''.replace("/* FUNCTION */", renderer.function("obj_is_in_view"))
        fixture = fixture.replace("DEFAULT_RADIUS", str(default)).replace("ROOM_RADIUS", str(radius))
        with tempfile.TemporaryDirectory(prefix="castle-room-culling-") as directory:
            folder = Path(directory)
            (folder / "test.c").write_text(fixture)
            subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", str(folder / "test.c"), "-lm",
                            "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_native_entry_camera_targets_the_revealed_room(self):
        paintings = Source(ROOT / "src/game/paintings.c")
        camera = Source(ROOT / "src/game/camera.c")
        functions = paintings.function("painting_get_world_position")
        functions += camera.function("offset_rotated") + camera.function("cutscene_enter_painting")
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
typedef float f32;
typedef int32_t s32;
typedef float Vec3f[3];
typedef int16_t Vec3s[3];
#define SURFACE_PAINTING_WOBBLE_A6 0xA6
#define SURFACE_PAINTING_WARP_F9 0xF9
#define CUTSCENE_ENTER_PAINTING 1
#define CUTSCENE_STOP -1
#define CAMERA_MODE_CLOSE 1
#define CAM_FOV_APP_20 1
#define CAM_FLAG_SMOOTH_MOVEMENT 1
#define CAM_FLAG_START_TRANSITION 2
#define CLAMP(x, low, high) fminf(fmaxf((x), (low)), (high))
#define TRUE 1
#define FALSE 0
#define segmented_to_virtual(p) (p)
struct Painting { f32 yaw, size, posX, posY, posZ; } castle_secret_lab_painting;
static struct Painting *gRipplingPainting=&castle_secret_lab_painting;
static Vec3f sCastleSecretPaintingRoomPos={0,5780,-3421};
struct Camera { Vec3f focus, pos; int cutscene, mode; };
struct Surface { int type; } surface={0xD4};
static struct { Vec3f pos; } mario={{0,5950,-4129}}, *sMarioCamState=&mario;
static int sStatusFlags,gCutsceneTimer,missingFloor,interpolate;
static struct { int framesLeft; } sModeTransition;
static void vec3f_copy(Vec3f v,Vec3f from) { for(int i=0;i<3;i++) v[i]=from[i]; }
static void vec3f_add(Vec3f v, Vec3f delta) { for(int i=0;i<3;i++) v[i]+=delta[i]; }
static f32 sins(int16_t angle) { return sinf(angle * (6.28318530718f/65536)); }
static f32 coss(int16_t angle) { return cosf(angle * (6.28318530718f/65536)); }
static void cutscene_enter_painting_stub(struct Camera *c) { (void)c; }
static void cutscene_event(void (*f)(struct Camera *),struct Camera *c,int a,int b) { (void)f;(void)c;(void)a;(void)b; }
static void set_fov_function(int f) { assert(f==CAM_FOV_APP_20); }
static void approach_vec3f_asymptotic(Vec3f v,Vec3f target,f32 x,f32 y,f32 z) {
    f32 rates[3]={x,y,z};
    for(int i=0;i<3;i++) v[i]=interpolate?v[i]+rates[i]*(target[i]-v[i]):target[i];
}
static f32 find_floor(f32 x,f32 y,f32 z,struct Surface **floor) {
    // Both the camera and Mario queries must remain near the rooftop room.
    assert(fabsf(x)<1000 && y>5700 && z < -3000 && z > -5000);
    *floor=missingFloor?NULL:&surface;
    // The roof is above the intended camera: the old +500 query catches it.
    return y >= 6297 ? 6297 : 5780;
}
/* FUNCTIONS */
int main(void) {
    castle_secret_lab_painting=(struct Painting){0,228,-114,124,-695};
    struct Camera c={.cutscene=CUTSCENE_ENTER_PAINTING};
    cutscene_enter_painting(&c);
    assert(c.focus[0]==0 && c.focus[1]==6018 && c.focus[2]==-4116);
    assert(c.pos[0]==0 && c.pos[1]==6018);
    assert(fabsf(c.pos[2]-(-4116+228*500.0f/307))<0.01f);
    assert(c.cutscene==CUTSCENE_ENTER_PAINTING);
    // Actual interpolation from views beyond each room wall and ceiling.
    const Vec3f starts[]={{-1500,5950,-3700},{1500,5950,-3700},
                         {0,6800,-3700},{0,5500,-3700},
                         {0,5950,-4600},{0,5950,-2800}};
    interpolate=1;
    for(unsigned i=0;i<sizeof(starts)/sizeof(starts[0]);i++) {
        for(int axis=0;axis<3;axis++) c.pos[axis]=c.focus[axis]=starts[i][axis];
        c.cutscene=CUTSCENE_ENTER_PAINTING;
        sStatusFlags=CAM_FLAG_SMOOTH_MOVEMENT|CAM_FLAG_START_TRANSITION;
        sModeTransition.framesLeft=20;
        for(int frame=0;frame<40;frame++) {
            cutscene_enter_painting(&c);
            assert(c.pos[0]>=-215 && c.pos[0]<=215);
            assert(c.pos[1]>=5810 && c.pos[1]<=6170);
            assert(c.pos[2]>=-3966 && c.pos[2]<=-3451);
            assert(c.focus[0]==0 && c.focus[1]==6018 && c.focus[2]==-4116);
            assert(sModeTransition.framesLeft==0);
            assert(!(sStatusFlags&(CAM_FLAG_SMOOTH_MOVEMENT|CAM_FLAG_START_TRANSITION)));
            assert(c.cutscene==CUTSCENE_ENTER_PAINTING);
        }
    }
    // Ordinary castle paintings retain their original smoothing/transition.
    struct Painting native={0,228,-114,5904,-4116};
    gRipplingPainting=&native;
    c.focus[0]=100;
    sModeTransition.framesLeft=20;
    cutscene_enter_painting(&c);
    assert(sModeTransition.framesLeft==20);
    assert(sStatusFlags&CAM_FLAG_SMOOTH_MOVEMENT);
    assert(fabsf(c.focus[0]-90)<0.01f);
    gRipplingPainting=&castle_secret_lab_painting;
    missingFloor=1;
    cutscene_enter_painting(&c);
    assert(c.cutscene==0 && gCutsceneTimer==CUTSCENE_STOP);
}
'''.replace("/* FUNCTIONS */",functions)
        with tempfile.TemporaryDirectory(prefix="castle-camera-") as directory:
            folder = Path(directory)
            (folder / "test.c").write_text(fixture)
            subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", str(folder / "test.c"), "-lm",
                            "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_glass_and_lab_destinations_have_distinct_slots(self):
        script = read_source(ROOT / "levels/castle_grounds/script.c")
        entries = re.findall(r"PAINTING_WARP_NODE\((0x[0-9A-Fa-f]+),\s*(\w+),"
                             r"\s*(0x[0-9A-Fa-f]+|\d+),\s*(0x[0-9A-Fa-f]+|\d+),", script)
        slots = [int(index, 0) for index, *_ in entries]
        self.assertEqual(len(slots), len(set(slots)), "Painting destinations overwrite each other")
        routes = {int(index, 0): (level, int(area, 0), int(node, 0))
                  for index, level, area, node in entries}
        self.assertEqual(routes[3], ("LEVEL_CASTLE", 4, 0x0A))
        for index in range(3):
            self.assertEqual(routes[index], ("LEVEL_PHYSICS_LAB", 1, 0x0A))

    def test_entry_floor_is_behind_the_open_portrait(self):
        data = read_source(ROOT / "levels/castle_grounds/areas/1/secret_room/collision.inc.c")
        data = data.split("const Collision castle_secret_painting_room_collision[] = {", 1)[1]
        vertices = [tuple(map(int, v)) for v in re.findall(
            r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", data)]
        self.assertEqual(len(vertices), int(re.search(r"COL_VERTEX_INIT\((\d+)\)", data)[1]))
        batches = re.findall(r"COL_TRI_INIT\(([^,]+),\s*(\d+)\),(.*?)(?=COL_TRI_INIT|COL_TRI_STOP)", data, re.S)
        trigger_bounds = []
        for surface, count, body in batches:
            triangles = re.findall(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", body)
            self.assertEqual(len(triangles), int(count))
            if "PAINTING" not in surface:
                continue
            points = [vertices[int(i)] for tri in triangles for i in tri]
            self.assertEqual({v[1] for v in points}, {0})
            for tri in triangles:
                a, b, c = [vertices[int(i)] for i in tri]
                self.assertGreater((b[2]-a[2])*(c[0]-a[0]) - (b[0]-a[0])*(c[2]-a[2]), 0)
            bounds = (min(v[0] for v in points), max(v[0] for v in points),
                      min(v[2] for v in points), max(v[2] for v in points))
            trigger_bounds.append((surface.strip(), bounds))
        self.assertEqual(len(trigger_bounds), 6)
        for side, (left, right) in enumerate(((-114, -38), (-38, 38), (38, 114))):
            self.assertEqual(trigger_bounds[side*2][1], (left, right, -705, -630))
            self.assertEqual(trigger_bounds[side*2+1][1], (left, right, -750, -705))

    def test_room_coordinates_native_ripple_and_warp_floor_route(self):
        paintings = Source(ROOT / "src/game/paintings.c")
        definitions = Source(ROOT / "include/surface_terrains.h").declaration("enum", "SurfaceTypes")
        definitions += paintings.macros("RIPPLE_LEFT|RIPPLE_MIDDLE|RIPPLE_RIGHT|ENTER_LEFT|ENTER_MIDDLE|ENTER_RIGHT|NEAREST_4TH|MARIO_Y|RESET_TIMER")
        definitions += Source(ROOT / "src/game/level_update.c").macros("PAINTING_WARP_INDEX_.*")
        functions = "".join(paintings.function(n) for n in (
            "painting_update_floors", "wall_painting_proximity_idle", "wall_painting_proximity_rippling",
            "geo_painting_update", "painting_get_world_position", "geo_castle_secret_painting_draw"))
        functions += Source(ROOT / "src/game/level_update.c").function("get_painting_warp_node")
        fixture = r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
typedef int8_t s8;
typedef float f32;
typedef int32_t s32;
typedef int16_t s16;
typedef uint16_t u16;
typedef uint32_t u32;
typedef float Mat4[4][4];
typedef float Vec3f[3];
typedef int Gfx;
#define UNUSED
#define TRUE 1
#define FALSE 0
#define GEO_CONTEXT_RENDER 1
#define PAINTING_IDLE 0
#define PAINTING_RIPPLE 1
#define PAINTING_ENTERED 2
/* DEFINITIONS */
struct GraphNode { int parameter; };
struct Object { f32 oPosX, oPosY, oPosZ; } room, marioObject, *gMarioObject = &marioObject;
static void *gCurGraphNodeObject = &room;
struct Surface { s16 type; } floorSurface, *queriedFloor = &floorSurface;
struct WarpNode { int id, destLevel, destArea, destNode; } nodes[4];
struct Area { struct WarpNode *paintingWarpNodes; } area, *gCurrentArea = &area;
static int glassWarpReady;
static int castle_secret_glass_warp_ready(void) { return glassWarpReady; }
struct MarioState { struct Surface *floor; f32 pos[3], floorHeight; } mario, *gMarioState = &mario;
struct Painting { int id, state, lastFloor, currFloor, floorEntered;
    int marioWasUnder, marioIsUnder, marioWentUnder; f32 posX, posY, posZ;
} painting, castle_secret_lab_painting;
static Vec3f sCastleSecretPaintingRoomPos;
#define TRUE 1
#define FALSE 0
#define segmented_to_virtual(p) (p)
static void vec3f_set(Vec3f v, f32 x, f32 y, f32 z) { v[0]=x;v[1]=y;v[2]=z; }
static void vec3f_add(Vec3f v, Vec3f delta) { for(int i=0;i<3;i++) v[i]+=delta[i]; }
static s16 gPaintingMarioFloorType, gPaintingUpdateCounter, gLastPaintingUpdateCounter;
static f32 gPaintingMarioXPos, gPaintingMarioYPos, gPaintingMarioZPos;
static u32 gStaticSurfaceGeneration;
static u16 gAreaUpdateCounter;
static int floorQueries, draws;
static Gfx result;
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **surface) {
    (void)x; (void)y; (void)z; floorQueries++; *surface = queriedFloor; return 0;
}
static void painting_state(int state, struct Painting *p, struct Painting **group,
                           int x, int y, int reset) {
    (void)group; assert(x == NEAREST_4TH && y == MARIO_Y && reset == RESET_TIMER); p->state = state;
}
void painting_update_floors(struct Painting *p);
void wall_painting_proximity_idle(struct Painting *p, struct Painting **group);
void wall_painting_proximity_rippling(struct Painting *p, struct Painting **group);
static Gfx *geo_painting_draw(s32 context, struct GraphNode *node, void *unused) {
    (void)unused; draws++; assert(node->parameter == 0x300);
    if (context == GEO_CONTEXT_RENDER) {
        assert(gPaintingMarioXPos == 0 && gPaintingMarioYPos == 170 && gPaintingMarioZPos == -708);
        painting_update_floors(&painting);
        if (painting.state == PAINTING_IDLE) wall_painting_proximity_idle(&painting, NULL);
        else if (painting.state == PAINTING_RIPPLE) wall_painting_proximity_rippling(&painting, NULL);
    }
    return &result;
}
/* FUNCTIONS */
int main(void) {
    struct GraphNode node = {0x300};
    room.oPosY = 5780; room.oPosZ = -3421;
    marioObject.oPosY = 5950; marioObject.oPosZ = -4129;
    area.paintingWarpNodes = nodes; mario.floor = &floorSurface;
    for (int side = 0; side < 3; side++) {
        memset(&painting, 0, sizeof(painting)); painting.posY = 124;
        floorSurface.type = SURFACE_PAINTING_WOBBLE_A6 + side; gAreaUpdateCounter++;
        assert(geo_castle_secret_painting_draw(1, &node, NULL) == &result);
        assert(painting.state == PAINTING_RIPPLE && painting.floorEntered != 0);
        floorSurface.type = SURFACE_PAINTING_WARP_D3 + side; gAreaUpdateCounter++;
        geo_castle_secret_painting_draw(1, &node, NULL);
        assert(painting.state == PAINTING_ENTERED && painting.floorEntered != 0);
        assert(get_painting_warp_node() == &nodes[side]);
        assert(gPaintingMarioXPos == 0 && gPaintingMarioYPos == 5950 && gPaintingMarioZPos == -4129);
    }
    glassWarpReady=1;
    assert(get_painting_warp_node()==&nodes[3]);
    glassWarpReady=0;
    assert(get_painting_warp_node()==&nodes[2]);
    queriedFloor = NULL; gAreaUpdateCounter++;
    geo_castle_secret_painting_draw(1, &node, NULL);
    assert(gPaintingMarioFloorType == SURFACE_DEFAULT);
    int queries = floorQueries;
    gCurGraphNodeObject = NULL;
    assert(geo_castle_secret_painting_draw(0, &node, NULL) == &result);
    assert(floorQueries == queries && draws == 8);
    Vec3f world;
    castle_secret_lab_painting.posX=-114;
    castle_secret_lab_painting.posY=124;
    castle_secret_lab_painting.posZ=-695;
    painting_get_world_position(&castle_secret_lab_painting,world);
    assert(world[0]==-114 && world[1]==5904 && world[2]==-4116);
    painting.posX=10; painting.posY=20; painting.posZ=30;
    painting_get_world_position(&painting,world);
    assert(world[0]==10 && world[1]==20 && world[2]==30);
    return 0;
}
'''.replace("/* DEFINITIONS */", definitions).replace("/* FUNCTIONS */", functions)
        with tempfile.TemporaryDirectory(prefix="castle-painting-") as directory:
            source = Path(directory) / "test.c"
            source.write_text(fixture)
            output = Path(directory) / "test"
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-Wno-unused-parameter", "-fsanitize=undefined", str(source),
                            "-o", str(output)], check=True)
            subprocess.run([str(output)], check=True)


if __name__ == "__main__":
    unittest.main()
