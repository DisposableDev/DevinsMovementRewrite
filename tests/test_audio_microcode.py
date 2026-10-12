"""Execute affected assembled RSP paths, including their DSP and saved state.

The instruction subset models 48-bit accumulation, vector flags and completed
state DMA using the RSP semantics documented in ares's interpreter-vpu.cpp.
It does not model pipeline cycles, DMA timing, AI playback or audible quality.
No ROM assets or ROM build are needed.
"""

from pathlib import Path
import random
import os
import shlex
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def signed(value, bits=32):
    value &= (1 << bits) - 1
    return value - (1 << bits) if value >> (bits - 1) else value


class RspPath:
    def __init__(self, code, symbols, data):
        self.code = code
        self.symbols = symbols
        self.mem = bytearray(4096)
        self.mem[:len(data)] = data
        self.reg = [0] * 32
        self.vec = [bytearray(16) for _ in range(32)]
        self.pc = 0
        self.pending = None
        self.steps = 0
        self.stores = []
        self.arithmetic = []
        self.simulate_dsp = False
        self.acc = [0] * 8
        self.carry = [0] * 8
        self.unequal = [0] * 8
        self.compare_lo = [0] * 8
        self.compare_hi = [0] * 8
        self.clip = [0] * 8
        self.rdram = bytearray(65536)
        self.dma_address = 0
        self.dma_dram = 0
        self.dma_writes = []
        self.reads = []
        self.visited = set()

    def load(self, address, size):
        return bytes(self.mem[(address + i) & 4095] for i in range(size))

    def store(self, address, data):
        for i, value in enumerate(data):
            self.mem[(address + i) & 4095] = value

    def run(self, start, stops):
        self.pc = self.symbols[start] & 4095
        stops = {self.symbols[name] & 4095 for name in stops}
        for _ in range(10000):
            if self.pc in stops and self.pending is None:
                return self.pc
            self.step()
        raise AssertionError("RSP path did not terminate")

    def step(self):
        word = struct.unpack_from(">I", self.code, self.pc - 0x80)[0]
        self.visited.add(self.pc)
        op = word >> 26
        rs, rt, rd = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
        imm = signed(word, 16)
        target = None
        delayed = self.pending
        self.pending = None
        if op == 0:
            fn = word & 63
            if fn == 0:
                self.reg[rd] = self.reg[rt] << ((word >> 6) & 31)
            elif fn == 2:
                self.reg[rd] = (self.reg[rt] & 0xffffffff) >> ((word >> 6) & 31)
            elif fn == 6:
                self.reg[rd] = (self.reg[rt] & 0xffffffff) >> (self.reg[rs] & 31)
            elif fn == 8:
                target = self.reg[rs] & 4095
            elif fn in (32, 33):
                self.reg[rd] = self.reg[rs] + self.reg[rt]
            elif fn in (34, 35):
                self.reg[rd] = self.reg[rs] - self.reg[rt]
            elif fn == 36:
                self.reg[rd] = self.reg[rs] & self.reg[rt]
            elif fn == 37:
                self.reg[rd] = self.reg[rs] | self.reg[rt]
            else:
                raise AssertionError(f"Unsupported scalar instruction {word:08x}")
        elif op in (2, 3):
            target = (word << 2) & 4095
            if op == 3:
                self.reg[31] = self.pc + 8
        elif op == 1 and rt == 0:  # BLTZ
            if self.reg[rs] < 0:
                target = (self.pc + 4 + imm * 4) & 4095
        elif op in (4, 5, 6, 7):
            conditions = {
                4: self.reg[rs] == self.reg[rt],
                5: self.reg[rs] != self.reg[rt],
                6: self.reg[rs] <= 0,
                7: self.reg[rs] > 0,
            }
            if conditions[op]:
                target = (self.pc + 4 + imm * 4) & 4095
        elif op in (8, 9):
            self.reg[rt] = self.reg[rs] + imm
        elif op == 12:
            self.reg[rt] = self.reg[rs] & (word & 65535)
        elif op == 13:
            self.reg[rt] = self.reg[rs] | (word & 65535)
        elif op == 15:
            self.reg[rt] = (word & 65535) << 16
        elif op in (33, 35, 36, 37):
            size = {35: 4, 36: 1}.get(op, 2)
            value = int.from_bytes(self.load(self.reg[rs] + imm, size), "big")
            self.reg[rt] = signed(value, 16) if op == 33 else value
        elif op in (41, 43):
            size = 4 if op == 43 else 2
            self.store(self.reg[rs] + imm, (self.reg[rt] & ((1 << (size * 8)) - 1)).to_bytes(size, "big"))
        elif op == 16:
            if rs == 0:  # DMA/semaphore polling completes immediately in this model.
                self.reg[rt] = 0
            elif rs == 4:
                value = self.reg[rt]
                if rd == 0:
                    self.dma_address = value & 4095 & ~7
                elif rd == 1:
                    self.dma_dram = value & (len(self.rdram) - 1) & ~7
                elif rd in (2, 3):
                    count = ((value & 4095) | 7) + 1
                    assert value >> 12 == 0, "Only single-row state DMAs are modeled"
                    if rd == 2:
                        self.store(self.dma_address, self.rdram[self.dma_dram:self.dma_dram + count])
                    else:
                        self.rdram[self.dma_dram:self.dma_dram + count] = self.load(self.dma_address, count)
                        self.dma_writes.append((self.dma_dram, count))
            else:
                raise AssertionError(f"Unsupported COP0 instruction {word:08x}")
        elif op == 18 and not word & (1 << 25):
            element = (word >> 7) & 15
            if rs == 0:
                self.reg[rt] = signed(int.from_bytes(bytes(self.vec[rd][(element + i) & 15] for i in range(2)), "big"), 16)
            elif rs == 4:
                value = self.reg[rt] & 65535
                self.vec[rd][element] = value >> 8
                if element != 15:
                    self.vec[rd][element + 1] = value & 255
            elif rs in (2, 6):
                flags = ((self.carry, self.unequal), (self.compare_lo, self.compare_hi), (self.clip, [0] * 8))[min(rd & 3, 2)]
                if rs == 2:
                    value = sum(bit << (lane + part * 8) for part, group in enumerate(flags) for lane, bit in enumerate(group))
                    self.reg[rt] = signed(value, 16)
                else:
                    for part, group in enumerate(flags):
                        for lane in range(8):
                            group[lane] = (self.reg[rt] >> (lane + part * 8)) & 1
            else:
                raise AssertionError(f"Unsupported COP2 transfer {word:08x}")
        elif op in (50, 58):
            kind, element = (word >> 11) & 31, (word >> 7) & 15
            scale = {1: 2, 3: 8, 4: 16, 5: 16}[kind]
            address = (self.reg[rs] + signed(word, 7) * scale) & 4095
            size = {1: 2, 3: 8}.get(kind, 16 - (address & 15))
            size = min(size, 16 - element)
            if op == 50:
                if kind == 5:  # LRV fills the right side from an aligned block.
                    size = (address & 15) - element
                    self.vec[rt][16 - size:] = self.load(address & ~15, size)
                else:
                    self.vec[rt][element:element + size] = self.load(address, size)
                self.reads.append((address, size))
            else:
                self.store(address, self.vec[rt][element:element + size])
                self.stores.append((address, size))
        elif op == 18 and word & (1 << 25):
            fn = word & 63
            if fn == 44 and not self.simulate_dsp:  # VXOR, needed by CLEARBUFF
                vs, vt, vd = (word >> 11) & 31, (word >> 16) & 31, (word >> 6) & 31
                self.vec[vd] = bytearray(a ^ b for a, b in zip(self.vec[vs], self.vec[vt]))
            else:
                # Loop-control tests observe the unchanged DSP instructions.
                self.arithmetic.append(word)
                if self.simulate_dsp:
                    self.vector_math(word)
        else:
            raise AssertionError(f"Unsupported RSP instruction {word:08x}")
        self.reg = [signed(value) for value in self.reg]
        self.reg[0] = 0
        self.steps += 1
        self.pc = delayed if delayed is not None else (self.pc + 4) & 4095
        self.pending = target


    def vector_math(self, word):
        fn = word & 63
        vs, vt, vd = (word >> 11) & 31, (word >> 16) & 31, (word >> 6) & 31
        element = (word >> 21) & 15
        left = list(struct.unpack(">8H", self.vec[vs]))
        right = list(struct.unpack(">8H", self.vec[vt]))
        if element >= 8:
            right = [right[element & 7]] * 8
        elif element in (2, 3):
            right = [right[(lane & ~1) + (element & 1)] for lane in range(8)]
        elif element in (4, 5, 6, 7):
            right = [right[(lane & ~3) + (element & 3)] for lane in range(8)]
        result = []
        for lane, (a, b) in enumerate(zip(left, right)):
            sa, sb = signed(a, 16), signed(b, 16)
            if fn in (0, 4, 5, 6, 7, 8, 13, 14, 15):
                product = {
                    0: 2 * sa * sb + 32768,
                    4: (a * b) >> 16,
                    5: sa * b,
                    6: a * sb,
                    7: (sa * sb) << 16,
                    8: 2 * sa * sb,
                    13: sa * b,
                    14: a * sb,
                    15: (sa * sb) << 16,
                }[fn]
                self.acc[lane] = signed(product + (self.acc[lane] if fn >= 8 else 0), 48)
                if fn in (4, 6):
                    value = self.acc[lane] & 65535
                elif fn == 5:
                    value = (self.acc[lane] >> 16) & 65535
                elif fn == 14:
                    high = self.acc[lane] >> 16
                    value = 0 if high < -32768 else 65535 if high > 32767 else self.acc[lane] & 65535
                else:
                    value = max(-32768, min(32767, self.acc[lane] >> 16))
            elif fn == 29:  # VSAR
                value = (self.acc[lane] >> ((10 - element) * 16)) & 65535 if element in (8, 9, 10) else 0
            else:
                if fn in (20, 21):  # VADDC / VSUBC
                    value = a + b if fn == 20 else a - b
                    self.carry[lane] = int(value > 65535) if fn == 20 else int(a < b)
                    self.unequal[lane] = 0 if fn == 20 else int(a != b)
                elif fn in (16, 17):  # VADD / VSUB
                    raw = sa + sb + self.carry[lane] if fn == 16 else sa - sb - self.carry[lane]
                    value = max(-32768, min(32767, raw))
                    self.acc[lane] = (self.acc[lane] & ~65535) | (raw & 65535)
                    self.carry[lane] = self.unequal[lane] = 0
                elif fn in (32, 35):  # VLT / VGE
                    equal_carry = self.carry[lane] and self.unequal[lane]
                    choose = sa < sb or (sa == sb and equal_carry) if fn == 32 else sa > sb or (sa == sb and not equal_carry)
                    self.compare_lo[lane] = int(choose)
                    self.compare_hi[lane] = 0
                    value = a if choose else b
                    self.carry[lane] = self.unequal[lane] = 0
                elif fn == 36:  # VCL, including saved compare and carry flags.
                    if self.carry[lane]:
                        if not self.unequal[lane]:
                            total = a + b
                            carry = total > 65535
                            zero = (total & 65535) == 0
                            self.compare_lo[lane] = int((zero or not carry) if self.clip[lane] else (zero and not carry))
                        value = -b if self.compare_lo[lane] else a
                    else:
                        if not self.unequal[lane]:
                            self.compare_hi[lane] = int(a >= b)
                        value = b if self.compare_hi[lane] else a
                    self.carry[lane] = self.unequal[lane] = self.clip[lane] = 0
                elif fn in (40, 42, 44):  # VAND / VOR / VXOR
                    value = {40: a & b, 42: a | b, 44: a ^ b}[fn]
                else:
                    raise AssertionError(f"Unsupported DSP instruction {word:08x}")
                if fn not in (16, 17):
                    self.acc[lane] = (self.acc[lane] & ~65535) | (value & 65535)
            result.append(value & 65535)
        self.vec[vd][:] = struct.pack(">8H", *result)


class AudioMicrocodeTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="sm64-audio-paths-")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.variants = {}
        cls.trial_variants = {}
        assembler = shutil.which("armips") or str(ROOT / "tools/armips")
        for region in ("US", "JP", "EU", "SH"):
            folder = Path(cls.temp.name)
            code, data, symbols = (folder / f"{region}.{suffix}" for suffix in ("bin", "data", "sym"))
            subprocess.run([
                assembler, "-definelabel", f"VERSION_{region}", "1",
                "-definelabel", "AUDIO_TRIAL", "1",
                "-sym", str(symbols), "-strequ", "CODE_FILE", str(code),
                "-strequ", "DATA_FILE", str(data), "rsp/audio.s",
            ], cwd=ROOT, check=True, capture_output=True, text=True)
            labels = {}
            for line in symbols.read_text().splitlines():
                fields = line.split()
                if len(fields) == 2 and not fields[1].startswith("."):
                    labels[fields[1]] = int(fields[0], 16)
            cls.variants[region] = (code.read_bytes(), labels, data.read_bytes())
            subprocess.run([
                assembler, "-definelabel", f"VERSION_{region}", "1",
                "-definelabel", "AUDIO_LINEAR_ENVELOPES", "1",
                "-sym", str(symbols), "-strequ", "CODE_FILE", str(code),
                "-strequ", "DATA_FILE", str(data), "rsp/audio.s",
            ], cwd=ROOT, check=True, capture_output=True, text=True)
            labels = {}
            for line in symbols.read_text().splitlines():
                fields = line.split()
                if len(fields) == 2 and not fields[1].startswith("."):
                    labels[fields[1]] = int(fields[0], 16)
            cls.trial_variants[region] = (code.read_bytes(), labels, data.read_bytes())

    def machine(self, region):
        machine = RspPath(*self.variants[region])
        machine.reg[24] = machine.symbols["audiostruct"]
        return machine

    def test_imem_budget(self):
        for region, (code, _, _) in list(self.variants.items()) + list(self.trial_variants.items()):
            with self.subTest(region=region):
                self.assertLessEqual(len(code), 4096 - 0x80)

    def test_clear_bytes_and_rounding(self):
        for region in self.variants:
            for offset in range(16):
                for count in (0, 1, 15, 16, 17, 31, 32, 63, 64, 320, 640):
                    with self.subTest(region=region, offset=offset, count=count):
                        machine = self.machine(region)
                        machine.mem[:] = b"\xa5" * 4096
                        base = 0 if region == "SH" else 0x5c0
                        address = 0x480 + offset
                        machine.reg[26] = 0x02000000 | address
                        machine.reg[25] = count
                        expected = bytearray(machine.mem)
                        for i in range((count + 15) // 16 * 16):
                            expected[(base + address + i) & 4095] = 0
                        machine.run("cmd_clearbuff", ["cmd_spnoop"])
                        self.assertEqual(machine.mem, expected)

    def test_move_bytes_overlap_and_rounding(self):
        rng = random.Random(64)
        initial = rng.randbytes(4096)
        for region in self.variants:
            for offset in range(16):
                for distance in (-32, -16, -8, -1, 0, 1, 8, 16, 32, 640):
                    for count in (0, 1, 16, 17, 32, 64, 320, 640):
                        with self.subTest(region=region, offset=offset, distance=distance, count=count):
                            machine = self.machine(region)
                            machine.mem[:] = initial
                            base = 0 if region == "SH" else 0x5c0
                            source = 0x100 + offset
                            destination = source + distance
                            machine.reg[26] = 0x0a000000 | source
                            machine.reg[25] = destination << 16 | count
                            expected = bytearray(initial)
                            for i in range(0, (count + 15) // 16 * 16, 16):
                                block = bytes(expected[(base + source + i + j) & 4095] for j in range(16))
                                for j, value in enumerate(block):
                                    expected[(base + destination + i + j) & 4095] = value
                            machine.run("cmd_dmemmove", ["cmd_spnoop"])
                            self.assertEqual(machine.mem, expected)

    def test_dispatch_consumes_command_before_handler(self):
        for region in self.variants:
            for command in range(32 if region == "SH" else 16):
                for remaining in (8, 16, 128, 320):
                    machine = self.machine(region)
                    machine.reg[29] = 0x800
                    machine.reg[28] = 0x123400
                    machine.reg[27] = remaining + 8
                    machine.reg[30] = remaining
                    first, second = command << 24 | 0x00abcdef, 0xfedcba98
                    machine.store(0x800, struct.pack(">II", first, second))
                    target = int.from_bytes(machine.load(0x10 + command * 2, 2), "big") & 4095
                    handlers = [name for name in machine.symbols if name.startswith("cmd_")]
                    reached = machine.run("audio_040010e4", handlers)
                    self.assertEqual(reached, target)
                    self.assertEqual(machine.reg[26:31], [signed(first), remaining, 0x123408, 0x808, remaining - 8])
                    self.assertEqual(machine.reg[25], signed(second))
                    self.assertEqual(machine.steps, 10)

    def test_envelope_loop_control(self):
        for region in ("US", "JP", "EU"):
            for count in (16, 32, 64, 320):
                for stride in (0, 16):  # no auxiliary output / auxiliary enabled
                    machine = self.machine(region)
                    machine.reg[14] = count
                    machine.reg[15] = stride
                    machine.reg[13] = 0x600
                    machine.reg[19] = 0x700
                    machine.reg[17] = 0x800
                    machine.reg[18] = 0x900
                    machine.reg[16] = 0xa00
                    end = machine.run("@@audio_04001dbc", ["@@audio_04001d04", "@@audio_04001dfc"])
                    target = "@@audio_04001d04" if count > 16 else "@@audio_04001dfc"
                    self.assertEqual(end, machine.symbols[target] & 4095)
                    self.assertEqual(machine.reg[14], count - 16)
                    self.assertEqual(machine.stores, [(0x900, 16), (0xa00, 16)])
                    self.assertEqual(machine.reg[13], 0x610)
                    self.assertEqual(machine.reg[19], 0x710)
                    self.assertEqual(machine.reg[18], 0x910)
                    self.assertEqual(machine.reg[17], 0x800 + stride)
                    self.assertEqual(machine.reg[16], 0xa00 + stride)
                    self.assertEqual(len(machine.arithmetic), 6)


    def test_normal_microcode_and_opt_in_embedding(self):
        settings = [(None, None, None), (0, 0, 0), (1, 0, 0), (0, 1, 0)]
        settings.extend((0, 0, trial) for trial in range(1, 11))
        for region in ("US", "JP", "EU", "SH"):
            for quality, linear, trial in settings:
                with self.subTest(region=region, quality=quality, linear=linear, trial=trial):
                    defines = []
                    if quality is not None:
                        defines = [f"-DAUDIO_QUALITY_TRIAL={quality}",
                                   f"-DAUDIO_LINEAR_ENVELOPES={linear}", f"-DAUDIO_TRIAL={trial}"]
                    command = shlex.split(os.environ.get("CC", "cc")) + [
                        "-E", "-P", "-x", "assembler-with-cpp", f"-DVERSION_{region}=1",
                        *defines, "-I", str(ROOT / "include"), str(ROOT / "lib/aspMain.s")]
                    source = subprocess.check_output(command, text=True)
                    effective_trial = (10 if region in ("US", "JP") else 0) if trial is None else trial
                    assembled = (region == "SH" or quality or linear
                                 or (effective_trial and effective_trial <= 7) or effective_trial == 10)
                    text = "rsp/audio.bin" if assembled else "lib/PR/audio/aspMain.bin"
                    data = "rsp/audio_data.bin" if assembled else "lib/PR/audio/aspMain_data.bin"
                    self.assertIn(f'.incbin "{text}"', source)
                    self.assertIn(f'.incbin "{data}"', source)
                    self.assertEqual(source.count(".incbin"), 2)

    def test_trial_build_paths_and_defines(self):
        settings = [((), "", 10), (("AUDIO_TRIAL=0",), "_audio_trial_0", 0),
                    (("AUDIO_QUALITY_TRIAL=1", "AUDIO_TRIAL=0"), "_audio_trial", 0),
                    (("AUDIO_LINEAR_ENVELOPES=1", "AUDIO_TRIAL=0"), "_audio_linear", 0),
                    (("AUDIO_TRIAL=1", "AUDIO_LOW_LATENCY=0"), "_audio_trial_1_audio_buffered", 1)]
        settings.extend(((f"AUDIO_TRIAL={trial}",), "" if trial == 10 else f"_audio_trial_{trial}", trial)
                        for trial in range(1, 11))
        for options, suffix, trial in settings:
            with self.subTest(options=options):
                output = subprocess.check_output([
                    "make", "-s", "--no-print-directory", "QUALITY_CONFIG_ONLY=1", "VERSION=us",
                    *options, "print-ROM", "print-DEFINES"], cwd=ROOT, text=True)
                self.assertIn(f"set to [build/us_n64{suffix}/sm64.z64]", output)
                self.assertIn(f"AUDIO_TRIAL={trial}", output)
        for options in (("AUDIO_TRIAL=11",), ("AUDIO_TRIAL=10", "VERSION=sh"),
                        ("AUDIO_TRIAL=10", "VERSION=eu"), ("AUDIO_TRIAL=2", "AUDIO_QUALITY_TRIAL=1"),
                        ("AUDIO_TRIAL=3", "AUDIO_LINEAR_ENVELOPES=1"),
                        ("AUDIO_TRIAL=6", "VERSION=sh"), ("AUDIO_TRIAL=8", "VERSION=eu")):
            with self.subTest(options=options):
                invalid = subprocess.run([
                    "make", "-s", "--no-print-directory", "QUALITY_CONFIG_ONLY=1",
                    *options, "print-ROM"], cwd=ROOT, capture_output=True, text=True)
                self.assertNotEqual(invalid.returncode, 0)
                self.assertIn("AUDIO_TRIAL", invalid.stderr)

    def test_compatible_hle_identification(self):
        for region in ("US", "JP", "EU"):
            with self.subTest(region=region):
                _, _, data = self.variants[region]
                self.assertEqual(data[:4], bytes.fromhex("00000001"))
                self.assertEqual(data[0x28:0x2c], bytes.fromhex("1e24138c"))
                self.assertEqual(data[0x30:0x34], bytes.fromhex("f0000f00"))

    def test_linear_envelope_samples(self):
        rng = random.Random(30000)
        for region in ("US", "JP", "EU"):
            for count in (16, 32, 128, 256, 288, 320):
                for source, target in ((1, 32736), (32736, 1), (12345, 12345), (1, 1)):
                    for auxiliary in (False, True):
                        with self.subTest(region=region, count=count, source=source,
                                          target=target, auxiliary=auxiliary):
                            machine = RspPath(*self.trial_variants[region])
                            machine.simulate_dsp = True
                            state = machine.symbols["audiostruct"]
                            machine.reg[24] = state
                            machine.reg[26] = 0x03000000 | ((9 if auxiliary else 1) << 16)
                            buffers = (0x600, 0x740, 0x880, 0xa00, 0xb40)
                            step = int((target - source) * 65536 / (count // 16))
                            opposite = -step
                            values = (buffers[0], buffers[1], count, source, target,
                                      buffers[2], buffers[3], buffers[4], target,
                                      (step >> 16) & 65535, step & 65535, source,
                                      (opposite >> 16) & 65535, opposite & 65535,
                                      0x7fff, 0x3000)
                            machine.store(state, struct.pack(">16H", *values))
                            samples = [rng.randrange(-32768, 32768) for _ in range(count // 2)]
                            machine.store(buffers[0], struct.pack(f">{len(samples)}h", *samples))
                            existing = {}
                            for address in buffers[1:]:
                                existing[address] = [rng.randrange(-32768, 32768) for _ in samples]
                                machine.store(address, struct.pack(f">{len(samples)}h", *existing[address]))
                            machine.run("cmd_envmixer", ["cmd_spnoop"])
                            for channel, address in enumerate(buffers[1:]):
                                actual = struct.unpack(f">{len(samples)}h", machine.load(address, count))
                                if channel >= 2 and not auxiliary:
                                    self.assertEqual(actual, tuple(existing[address]))
                                    continue
                                left_channel = channel in (0, 2)
                                gain = 0x7fff if channel < 2 else 0x3000
                                start = source if left_channel else target
                                end = target if left_channel else source
                                for index, (sample, output) in enumerate(zip(samples, actual)):
                                    volume = start + (end - start) * (index + 1) / len(samples)
                                    scaled = int((2 * int(volume) * gain + 32768) // 65536)
                                    mixed = 2 * existing[address][index] * 32767 + 2 * sample * scaled + 32768
                                    expected = max(-32768, min(32767, mixed // 65536))
                                    self.assertLessEqual(abs(output - expected), 3)
                            if count == 256 and auxiliary:
                                # The old ramping envelope exceeds 900 instructions here,
                                # excluding its state DMA wait time.
                                self.assertLess(machine.steps, 900)

    def test_aligned_buffer_instruction_budgets(self):
        # Original US paths execute 209 / 328 instructions for these requests.
        # These budgets count instructions, not RSP pipeline or DMA cycles.
        for region in self.variants:
            machine = self.machine(region)
            machine.reg[26] = 0x02000480
            machine.reg[25] = 640
            machine.run("cmd_clearbuff", ["cmd_spnoop"])
            self.assertLessEqual(machine.steps, 171)
            machine = self.machine(region)
            machine.reg[26] = 0x0a000100
            machine.reg[25] = (0x380 << 16) | 640
            machine.run("cmd_dmemmove", ["cmd_spnoop"])
            self.assertLessEqual(machine.steps, 252)

    def test_dmem_wrap(self):
        for region in self.variants:
            base = 0 if region == "SH" else 0x5c0
            for offset in range(16):
                source = 0xff0 - base + offset
                for command in ("cmd_clearbuff", "cmd_dmemmove"):
                    machine = self.machine(region)
                    machine.mem[:] = bytes(range(256)) * 16
                    expected = bytearray(machine.mem)
                    machine.reg[26] = source
                    if command == "cmd_clearbuff":
                        machine.reg[25] = 17
                        for i in range(32):
                            expected[(base + source + i) & 4095] = 0
                    else:
                        machine.reg[25] = (0x100 << 16) | 17
                        for i in range(0, 32, 16):
                            block = bytes(expected[(base + source + i + j) & 4095] for j in range(16))
                            for j, value in enumerate(block):
                                expected[(base + 0x100 + i + j) & 4095] = value
                    machine.run(command, ["cmd_spnoop"])
                    self.assertEqual(machine.mem, expected)


if __name__ == "__main__":
    unittest.main()
