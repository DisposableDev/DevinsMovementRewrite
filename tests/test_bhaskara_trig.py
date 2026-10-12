#!/usr/bin/env python3
"""Production Bhaskara angle/pair checks; no emulator or timing claims."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

CHECKS = r'''
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; } } while (0)

int main(void) {
    const double tau = 6.283185307179586476925286766559;
    const float cardinal_sines[4] = {0, 1, 0, -1};
    const float cardinal_cosines[4] = {1, 0, -1, 0};
    double max_error = 0;
    double max_phase_error = 0;

    for (unsigned i = 0; i < 4; i++) {
        CHECK(sins(i * 0x4000) == cardinal_sines[i]);
        CHECK(coss(i * 0x4000) == cardinal_cosines[i]);
    }
    CHECK(coss(0x2000) == 12.0f / 17.0f);
    CHECK(fabsf(sins(0x2000) - sqrtf(145.0f) / 17.0f) < 0.0000001f);

    for (unsigned angle = 0; angle < 65536; angle++) {
        float sine;
        float cosine;
        sincos_bhaskara((u16) angle, &sine, &cosine);
        CHECK(isfinite(sine) && isfinite(cosine));
        CHECK(fabsf(sine) <= 1 && fabsf(cosine) <= 1);
        CHECK(sine == sins(angle) && cosine == coss(angle));
        CHECK(sins(-(int) angle) == -sine);
        CHECK(coss(-(int) angle) == cosine);
        CHECK(sins(angle + 65536) == sine && coss(angle + 65536) == cosine);
        CHECK(fabs((double) sine * sine + (double) cosine * cosine - 1) < 0.0000005);

        double radians = angle * tau / 65536;
        max_error = fmax(max_error, fmax(fabs(sine - sin(radians)),
                                        fabs(cosine - cos(radians))));
        max_phase_error = fmax(max_phase_error,
                              fabs(remainder(atan2(sine, cosine) - radians, tau)));
    }
    CHECK(max_error < 0.0021);
    CHECK(max_phase_error * 360 / tau < 0.134);
    printf("65536 angles passed; max error %.12g, max phase error %.12g degrees\n",
           max_error, max_phase_error * 360 / tau);
    return 0;
}
'''


class BhaskaraTrigTests(unittest.TestCase):
    def test_all_angles_and_shared_pairs(self):
        header = (ROOT / "src/engine/math_util.h").read_text()
        macros = "\n".join(line for line in header.splitlines()
                           if line.startswith(("#define sins(", "#define coss(")))
        fixture = """#include <math.h>
#include <stdint.h>
#include <stdio.h>
typedef float f32;
typedef uint16_t u16;
typedef uint32_t u32;
"""
        fixture += '#include "' + str(ROOT / "src/engine/bhaskara_trig.inc.c") + '"\n'
        fixture += macros + "\n" + CHECKS
        with tempfile.TemporaryDirectory(prefix="bhaskara-trig-") as directory:
            source = Path(directory) / "check.c"
            binary = Path(directory) / "check"
            source.write_text(fixture)
            subprocess.run(["cc", "-std=c11", "-Os", "-Wall", "-Wextra", "-Werror",
                            "-ffp-contract=off", "-fsanitize=address,undefined",
                            str(source), "-lm", "-o", str(binary)], check=True)
            environment = dict(os.environ, ASAN_OPTIONS="detect_leaks=0")
            result = subprocess.run([str(binary)], check=True, capture_output=True,
                                    text=True, env=environment)
            print(result.stdout.strip())


if __name__ == "__main__":
    unittest.main()
