"""Host checks for shared painting distances and bounded match indices."""
from pathlib import Path
import os
import re
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class PaintingCacheTests(unittest.TestCase):
    def test_geometry_reuse_and_invalidation(self):
        source = Source(ROOT / "src/game/paintings.c")
        cache = re.search(r"static struct \{[^}]+\} sPaintingRippleCache;", source.text)[0]
        definitions = source.macros("PAINTING_RIPPLE_CACHE_VERTICES")
        definitions += Source(ROOT / "src/game/paintings.h").macros("PAINTING_SIZE")
        fixture = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef float f32;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
#define TRUE 1
#define FALSE 0
struct Painting { f32 size, dispersionFactor, rippleX, rippleY, timer; };
static int roots;
static f32 counted_sqrt(f32 value) { roots++; return sqrtf(value); }
/* DEFINITIONS */
/* CACHE */
#define sqrtf counted_sqrt
/* FUNCTION */
#undef sqrtf
static void verify(struct Painting *painting, s16 *mesh, int count) {
    for (int i = 0; i < count; i++) {
        if (!mesh[i * 3 + 3]) { continue; }
        f32 dx = mesh[i * 3 + 1] * (painting->size / PAINTING_SIZE) - painting->rippleX;
        f32 dy = mesh[i * 3 + 2] * (painting->size / PAINTING_SIZE) - painting->rippleY;
        f32 expected = sqrtf(dx * dx + dy * dy) / painting->dispersionFactor;
        assert(expected == sPaintingRippleCache.distance[i]);
        int match = sPaintingRippleCache.matchingVertex[i];
        assert(match <= i && mesh[match * 3 + 3]);
        assert(sPaintingRippleCache.distance[match] == expected);
    }
}
int main(void) {
    s16 mesh[1 + 3 * PAINTING_RIPPLE_CACHE_VERTICES] = {PAINTING_RIPPLE_CACHE_VERTICES};
    for (int i = 0; i < PAINTING_RIPPLE_CACHE_VERTICES; i++) {
        mesh[i * 3 + 1] = i;
        mesh[i * 3 + 2] = 0;
        mesh[i * 3 + 3] = i != 4;
    }
    mesh[3 * 2 + 1] = -1; // Equal distance to vertex 1.
    struct Painting a = {PAINTING_SIZE, 10, 0, 0, 0}, b = a;
    assert(painting_cache_ripple_distances(&a, mesh, mesh[0]));
    verify(&a, mesh, mesh[0]);
    assert(sPaintingRippleCache.matchingVertex[2] == 1);
    assert(sPaintingRippleCache.matchingVertex[156] == 156);
    int initialRoots = roots;
    for (int frame = 0; frame < 10; frame++) {
        a.timer = b.timer = frame;
        assert(painting_cache_ripple_distances(frame & 1 ? &a : &b, mesh, mesh[0]));
    }
    assert(roots == initialRoots); // Identity/timer changes require no distance work.
    f32 *parameters[] = {&b.size, &b.dispersionFactor, &b.rippleX, &b.rippleY};
    for (int i = 0; i < 4; i++) {
        *parameters[i] += 3;
        int before = roots;
        assert(painting_cache_ripple_distances(&b, mesh, mesh[0]));
        assert(roots > before);
        verify(&b, mesh, mesh[0]);
    }
    s16 other[sizeof(mesh) / sizeof(*mesh)];
    memcpy(other, mesh, sizeof(mesh));
    other[1] = 40;
    int before = roots;
    assert(painting_cache_ripple_distances(&b, other, mesh[0]));
    assert(roots > before);
    verify(&b, other, mesh[0]);
    before = roots;
    assert(painting_cache_ripple_distances(&b, other, mesh[0] - 1));
    assert(roots > before);
    assert(!painting_cache_ripple_distances(&b, other, mesh[0] + 1));
    return 0;
}
'''
        fixture = fixture.replace("/* DEFINITIONS */", definitions).replace("/* CACHE */", cache)
        fixture = fixture.replace("/* FUNCTION */", source.function("painting_cache_ripple_distances"))
        with tempfile.TemporaryDirectory(prefix="painting-cache-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(fixture)
            command = shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=address,undefined",
                str(path / "test.c"), "-lm", "-o", str(path / "test")]
            result = subprocess.run(command, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(path / "test")], capture_output=True, text=True, timeout=10,
                                    env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
