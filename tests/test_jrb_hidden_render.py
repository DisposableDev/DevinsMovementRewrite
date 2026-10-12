"""JRB terrain bounds and extracted draw paths.

Host doubles observe submissions and update phases, rather than rendered pixels.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
from regression_support import Source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]


class JrbHiddenRender(unittest.TestCase):
    def test_terrain_bounds_cover_native_loads(self):
        import re
        from test_jrb_render_assets import arrays, points
        model_arrays = {}
        for model_file in (ROOT / "levels/jrb/areas/1").glob("*/model.inc.c"):
            model_arrays.update(arrays(read_source(model_file)))
        visibility_source = Source(ROOT / "src/game/rendering_graph_node.c").function(
            "geo_jrb_terrain_in_view")
        terrain_bounds = re.findall(
            r"displayList == (\w+)_culled\).*?minimum = \{([^}]+)\};.*?maximum = \{([^}]+)\};",
            visibility_source, re.S)
        self.assertEqual(len(terrain_bounds), 5)
        for native_display_list, minimum_values, maximum_values in terrain_bounds:
            minimum = [float(v.strip().removesuffix("f")) for v in minimum_values.split(",")]
            maximum = [float(v.strip().removesuffix("f")) for v in maximum_values.split(",")]

            # Follow known nested display lists and check the four-unit vertex padding.
            def check_display_list(display_list_name):
                for operation, arguments in re.findall(
                        r"(gsSPVertex|gsSPDisplayList)\((.*?)\)", model_arrays[display_list_name][1]):
                    fields = [v.strip() for v in arguments.split(",")]
                    if operation == "gsSPDisplayList":
                        if fields[0] in model_arrays:
                            check_display_list(fields[0])
                    else:
                        for vertex in points(model_arrays[fields[0]][1])[:int(fields[1])]:
                            for axis in range(3):
                                self.assertLessEqual(minimum[axis], vertex[axis] - 4)
                                self.assertGreaterEqual(maximum[axis], vertex[axis] + 4)

            check_display_list(native_display_list)

    def test_particle_groups_and_water_phase(self):
        renderer_source = Source(ROOT / 'src/game/rendering_graph_node.c')
        bubble_source = Source(ROOT / 'src/game/envfx_bubbles.c')
        water_source = Source(ROOT / 'src/game/moving_texture.c')
        fixture_source = r'''

#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
typedef int32_t s32;
typedef int16_t s16;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef f32 Mat4[4][4];
typedef uint64_t Gfx;
typedef struct {
    s16 data[8];
} Vtx;
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf fabsf
#define NO_CULLING_EMULATOR_WHITELIST 2
#define LEVEL_JRB 12
#define ENVFX_JETSTREAM_BUBBLES 14
#define TEXTURE_MIST 1
#define ROTATE_CLOCKWISE 0
// Fixed projection inputs for the extracted camera visibility helper.
static struct {
    f32 halfFovHorizontal, halfFovVertical;
} frustum = { 0.7f, 0.5f };
static struct {
    s16 rollScreen;
} camera;
static typeof(camera) *gCurGraphNodeCamera = &camera;
static typeof(frustum) *gCurGraphNodeCamFrustum = &frustum;
static int gEmulator = 1, gCurrLevelNum = LEVEL_JRB;
static f32 sAspectRatio = 4.0f / 3.0f;
static Mat4 gCameraTransform = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
static void linear_mtxf_mul_vec3f_and_translate(Mat4 m, Vec3f dest, const Vec3f src) {
    for (int a = 0; a < 3; a++) {
        dest[a] = src[0] * m[0][a] + src[1] * m[1][a] + src[2] * m[2][a] + m[3][a];
    }
}
// Successful allocations only; reset the arena offset for each simulated frame.
static union {
    uint64_t align;
    unsigned char bytes[65536];
} displayListPool;
static size_t poolBytesUsed;
static int particleUpdates, vertexGroups, triangleCount, waterVertexCount, waterTextureLoads,
    lastGroupFirstParticle, bubbleTextureLookups;
static uint64_t drawnParticleMask;
static void *alloc_display_list(size_t size) {
    size = (size + 7) & ~7;
    assert(poolBytesUsed + size <= sizeof(displayListPool.bytes));
    void *result = displayListPool.bytes + poolBytesUsed;
    poolBytesUsed += size;
    return result;
}
// Command substitutes observe submissions without rendering.
#define gSPDisplayList(p, ...) ((void) (p))
#define gSPEndDisplayList(p, ...) ((void) (p))
#define gDPPipeSync(p, ...) ((void) (p))
#define gDPSetTextureImage(p, ...) ((void) (p))
#define gSP1Triangle(p, a, b, c, flag)                                                                 \
    ((void) (p), assert(a >= 0 && c < 15 && b == a + 1 && c == a + 2), triangleCount++,                \
     drawnParticleMask |= 1ull << (lastGroupFirstParticle + a / 3))
#define gSPVertex(p, ...) ((void) (p))
#define gLoadBlockTexture(p, ...) ((void) (p), waterTextureLoads++)
#define VIRTUAL_TO_PHYSICAL(p) (p)
#define VIRTUAL_TO_PHYSICAL2(p) (p)
static Gfx *sGfxCursor;
static Gfx tiny_bubble_dl_0B006D38, tiny_bubble_dl_0B006D68, tiny_bubble_dl_0B006AB0,
    dl_draw_quad_verts_0123;
static Vtx gBubbleTempVtx[3];
struct EnvFxParticle {
    int xPos, yPos, zPos, unusedBubbleVar;
};
static struct EnvFxParticle particles[10], *gEnvFxBuffer = particles;
static s32 sBubbleActiveCount = 10;
// Update and geometry substitutes: native motion and billboarding are not exercised here.
static void orbit_from_positions(Vec3s a, Vec3s b, s16 *r, s16 *p, s16 *y) {
    *r = *p = *y = 0;
}
static void envfx_bubbles_update_switch(s32 mode, Vec3s c, Vec3s a, Vec3s b, Vec3s d) {
    particleUpdates++;
    for (int i = 0; i < 10; i++) {
        particles[i].yPos++;
    }
}
static void rotate_triangle_vertices(Vec3s a, Vec3s b, Vec3s c, s16 p, s16 y) {
}
static void *envfx_get_bubble_texture(s32 mode, s16 index) {
    bubbleTextureLookups++;
    return (void *) 1;
}
static void append_bubble_vertex_buffer(Gfx *g, Vtx *v, s32 i, Vec3s a, Vec3s b, Vec3s c, Vtx *t) {
    vertexGroups++;
    lastGroupFirstParticle = i;
}
static s16 gMovtexCounter = 1, gMovtexCounterPrev = 0, gMovetexLastTextureId = -1;
static void *gMovtexIdToTexture[2];
// Count water vertex requests without generating native vertex data.
static void movtex_make_quad_vertex(Vtx *v, int i, int x, int y, int z, int rot, int offset, int scale,
                                    int alpha) {
    waterVertexCount++;
}
'''
        # Real production extractions use the host substitutes declared above.
        fixture_source += renderer_source.function('geo_camera_box_in_view')
        fixture_source += bubble_source.function('envfx_jetstream_group_in_view')
        fixture_source += bubble_source.function('envfx_update_bubble_particles')
        fixture_source += water_source.declaration('struct', 'MovtexQuad')
        fixture_source += water_source.function('movtex_jrb_quad_in_view')
        fixture_source += water_source.function('movtex_gen_from_quad')
        fixture_source += r'''

int main(void) {
    Vec3s zero = { 0, 0, 0 };
    for (int i = 0; i < 10; i++) {
        particles[i] = (struct EnvFxParticle){ 5000, 0, -1000, 1 };
    }
    for (int frame = 0; frame < 90; frame++) {
        poolBytesUsed = 0;
        assert(envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero));
    }
    assert(particleUpdates == 90 && vertexGroups == 0 && triangleCount == 0);
    for (int i = 0; i < 10; i++) {
        assert(particles[i].yPos == 90);
    }
    // Projection roll can bring an otherwise hidden jet group into view.
    for (int i = 0; i < 5; i++) {
        particles[i] = (struct EnvFxParticle){ 0, 800, -1000, 1 };
    }
    assert(!envfx_jetstream_group_in_view(0));
    camera.rollScreen = 0x4000;
    assert(envfx_jetstream_group_in_view(0));
    camera.rollScreen = 0;
    assert(!envfx_jetstream_group_in_view(0));
    for (int i = 0; i < 5; i++) {
        particles[i] = (struct EnvFxParticle){ 5000, 90, -1000, 1 };
    }
    for (int i = 5; i < 10; i++) {
        particles[i].xPos = 0;
    }
    poolBytesUsed = 0;
    envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero);
    assert(particleUpdates == 91 && vertexGroups == 1 && triangleCount == 5
           && lastGroupFirstParticle == 5);
    // One in-view particle keeps the group visible; wrapped coordinates bypass culling.
    particles[0].xPos = 0;
    assert(envfx_jetstream_group_in_view(0));
    particles[0].xPos = 32760;
    assert(envfx_jetstream_group_in_view(0));
    // Two visible jet groups resolve the shared texture only once.
    for (int i = 0; i < 10; i++) {
        particles[i].xPos = 0;
    }
    bubbleTextureLookups = vertexGroups = triangleCount = poolBytesUsed = 0;
    envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero);
    assert(vertexGroups == 2 && triangleCount == 10 && bubbleTextureLookups == 1);
    // Non-positive ages omit the waiting particles; their scheduling is tested separately.
    particles[0].unusedBubbleVar = 0;
    particles[9].unusedBubbleVar = -1;
    bubbleTextureLookups = vertexGroups = triangleCount = poolBytesUsed = drawnParticleMask = 0;
    envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero);
    assert(vertexGroups == 2 && triangleCount == 8 && bubbleTextureLookups == 1
           && drawnParticleMask == 0x1fe);
    for (int i = 0; i < 10; i++) {
        particles[i].unusedBubbleVar = 0;
    }
    bubbleTextureLookups = vertexGroups = triangleCount = poolBytesUsed = 0;
    envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero);
    assert(vertexGroups == 0 && triangleCount == 0 && bubbleTextureLookups == 0);
    // Other levels keep drawing jets without consulting JRB timing fields.
    gCurrLevelNum = LEVEL_JRB + 1;
    bubbleTextureLookups = vertexGroups = triangleCount = poolBytesUsed = drawnParticleMask = 0;
    envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, zero, zero, zero);
    assert(vertexGroups == 2 && triangleCount == 10 && drawnParticleMask == 0x3ff);
    gCurrLevelNum = LEVEL_JRB;
    struct MovtexQuad quad = { .rotspeed = 3,
                               .scale = 1,
                               .x1 = 5000,
                               .z1 = -1000,
                               .x2 = 5100,
                               .z2 = -1000,
                               .x3 = 5100,
                               .z3 = -1100,
                               .x4 = 5000,
                               .z4 = -1100,
                               .alpha = 255 };
    // Hidden water advances phase while the update counters differ.
    for (int frame = 0; frame < 90; frame++) {
        poolBytesUsed = 0;
        assert(!movtex_gen_from_quad(0, &quad));
    }
    assert(quad.rot == 270 && waterVertexCount == 0 && waterTextureLoads == 0
           && gMovetexLastTextureId == -1);
    // Matching counters suppress rotation in both hidden and visible calls.
    gMovtexCounterPrev = gMovtexCounter;
    assert(!movtex_gen_from_quad(0, &quad) && quad.rot == 270);
    quad.x1 = quad.x4 = -50;
    quad.x2 = quad.x3 = 50;
    poolBytesUsed = 0;
    assert(movtex_gen_from_quad(0, &quad));
    assert(quad.rot == 270 && waterVertexCount == 4 && waterTextureLoads == 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as fixture_directory:
            fixture_path = Path(fixture_directory) / 'hidden.c'
            executable_path = Path(fixture_directory) / 'hidden'
            fixture_path.write_text(fixture_source)
            subprocess.run(
                [os.environ.get('CC', 'cc'), '-std=gnu11', '-O1',
                 '-fsanitize=undefined,address', str(fixture_path), '-lm', '-o', str(executable_path)],
                check=True)
            subprocess.run([str(executable_path)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == '__main__':
    unittest.main()
