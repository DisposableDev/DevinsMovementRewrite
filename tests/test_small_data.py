#!/usr/bin/env python3
"""Check GP context width and SDK argument forwarding; no N64 timing claims."""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
HARNESS = r"""
#include <ultra64.h>
#include <assert.h>
#include <string.h>

extern void __wrap_osCreateThread(OSThread *, OSId, void (*)(void *), void *, void *, OSPri);
static OSThread *expectedThread;
static int calls;

static void entry(void *arg) {
    (void) arg;
}

void __real_osCreateThread(OSThread *thread, OSId id, void (*function)(void *), void *arg,
                           void *sp, OSPri pri) {
    assert(thread == expectedThread);
    assert(id == 7 && function == entry);
    assert(arg == (void *) 0x1234 && sp == (void *) 0x10000 && pri == 50);
    thread->state = OS_STATE_STOPPED;
    thread->context.gp = 0xa5a5a5a5a5a5a5a5ULL;
    calls++;
}

int main(void) {
    OSThread thread;
    OSThread expected;

    memset(&thread, 0xa5, sizeof(thread));
    memcpy(&expected, &thread, sizeof(expected));
    expected.state = OS_STATE_STOPPED;
    expected.context.gp = EXPECTED_GP;
    expectedThread = &thread;
    __wrap_osCreateThread(&thread, 7, entry, (void *) 0x1234, (void *) 0x10000, 50);
    assert(calls == 1);
    assert(memcmp(&thread, &expected, sizeof(thread)) == 0);
    return 0;
}
"""


class SmallDataThreadTests(unittest.TestCase):
    def test_kseg0_gp_is_sign_extended_without_other_context_changes(self):
        with tempfile.TemporaryDirectory(prefix="sm64-gp-context-") as temp:
            folder = Path(temp)
            source = folder / "context.c"
            source.write_text(HARNESS)
            for address in (0x80000000, 0x80234560, 0x807FFFF0):
                with self.subTest(gp=hex(address)):
                    program = folder / "context"
                    subprocess.run([
                        "cc", "-D_LANGUAGE_C", "-DENGINE_G16=1",
                        "-Iinclude/n64", "-Iinclude", "-Wall", "-Wextra", "-Werror",
                        "-fno-pie", "-no-pie", "-fsanitize=undefined",
                        f"-DEXPECTED_GP={address | 0xFFFFFFFF00000000:#x}ULL",
                        f"-Wl,--defsym,_gp={address:#x}",
                        "src/boot/small_data.c", str(source), "-o", str(program),
                    ], cwd=ROOT, check=True, capture_output=True, text=True)
                    subprocess.run([str(program)], check=True, capture_output=True, text=True)


if __name__ == "__main__":
    unittest.main()
