"""Preserve four-leaf bundles outside JRB's deep seabed."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class JrbSeaweedTests(unittest.TestCase):
    def test_bundle_density_and_animation(self):
        function = Source(ROOT / "src/game/behaviors/seaweed.inc.c").function("bhv_seaweed_bundle_init")
        code = r'''
#include <assert.h>
#include <string.h>
#define LEVEL_JRB 12
#define MODEL_SEAWEED 1
struct Object {
    float oPosY;
    int oFaceAngleVec[3];
    struct { struct { float scale[3]; struct { float animFrame; } animInfo; } gfx; } header;
};
static struct Object parent, leaves[4], *o = &parent;
static int gCurrLevelNum, count, bhvSeaweed;
static struct Object *spawn_object(struct Object *owner, int model, const void *behavior) {
    assert(owner == o && model == MODEL_SEAWEED && behavior == &bhvSeaweed && count < 4);
    return &leaves[count++];
}
#define bhvSeaweed (&bhvSeaweed)
static void vec3i_set(int (*v)[3], int x, int y, int z) { (*v)[0]=x; (*v)[1]=y; (*v)[2]=z; }
static void vec3f_set(float v[3], float x, float y, float z) { v[0]=x; v[1]=y; v[2]=z; }
static float random_float(void) { return 0.5f; }
/* FUNCTION */
int main(void) {
    const int levels[] = {LEVEL_JRB, 23, 9};
    const float heights[] = {-5119.0f, -4000.0f, -3999.0f, -1023.0f};
    for (int level=0; level<3; level++) for (int height=0; height<4; height++) {
        count=0; memset(leaves,0,sizeof(leaves));
        gCurrLevelNum=levels[level]; o->oPosY=heights[height];
        bhv_seaweed_bundle_init();
        int expected=(gCurrLevelNum==LEVEL_JRB && o->oPosY<=-4000.0f)?3:4;
        assert(count==expected);
        assert(leaves[0].oFaceAngleVec[0]==0x157C && leaves[0].header.gfx.scale[1]==1.0f);
        assert(leaves[1].oFaceAngleVec[0]==0x17D6 && leaves[1].header.gfx.animInfo.animFrame==40.0f);
        assert(leaves[count-1].oFaceAngleVec[0]==0x251C);
        assert(leaves[count-1].header.gfx.scale[1]==1.2f);
        assert(leaves[count-1].header.gfx.animInfo.animFrame==40.0f);
    }
}
'''.replace("/* FUNCTION */", function)
        with tempfile.TemporaryDirectory(prefix="jrb-seaweed-") as directory:
            source = Path(directory) / "test.c"
            binary = Path(directory) / "test"
            source.write_text(code)
            subprocess.run([os.environ.get("CC", "cc"), "-std=gnu17", "-Wall", "-Wextra",
                            "-Werror", "-fsanitize=address,undefined", "-no-pie", str(source),
                            "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
