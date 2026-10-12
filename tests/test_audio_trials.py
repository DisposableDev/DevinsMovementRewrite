"""Compare each exact-output audio trial with the shipped ABI1 microcode.

These are instruction/DSP checks, not pipeline timing or listening tests.
"""

from pathlib import Path
import random
import shutil
import struct
import subprocess
import tempfile
import unittest

from test_audio_microcode import ROOT, RspPath


class AudioTrialsTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="sm64-audio-trials-")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.variants = {}
        assembler = shutil.which("armips") or str(ROOT / "tools/armips")
        folder = Path(cls.temp.name)
        for region in ("US", "JP", "EU"):
            for trial in (*range(8), 10):
                stem = folder / f"{region}-{trial}"
                code, data, symbols = (stem.with_suffix(ext) for ext in (".bin", ".data", ".sym"))
                subprocess.run([
                    assembler, "-definelabel", f"VERSION_{region}", "1",
                    "-definelabel", "AUDIO_TRIAL", str(trial), "-sym", str(symbols),
                    "-strequ", "CODE_FILE", str(code), "-strequ", "DATA_FILE", str(data),
                    "rsp/audio.s"], cwd=ROOT, check=True, capture_output=True, text=True)
                labels = {}
                for line in symbols.read_text().splitlines():
                    fields = line.split()
                    if len(fields) == 2 and not fields[1].startswith("."):
                        labels[fields[1]] = int(fields[0], 16)
                cls.variants[region, trial] = (code.read_bytes(), labels, data.read_bytes())
        # Handler addresses come from the actual shipped dispatch table.
        data = (ROOT / "lib/PR/audio/aspMain_data.bin").read_bytes()
        labels = dict(cls.variants["US", 0][1])
        names = ("spnoop", "adpcm", "clearbuff", "envmixer", "loadbuff", "resample",
                 "savebuff", "segment", "setbuff", "setvol", "dmemmove", "loadadpcm",
                 "mixer", "interleave", "polef", "setloop")
        for index, name in enumerate(names):
            labels["cmd_" + name] = int.from_bytes(data[16 + index * 2:18 + index * 2], "big")
        cls.original = ((ROOT / "lib/PR/audio/aspMain.bin").read_bytes(), labels, data)

    def machines(self, region, trials):
        result = [RspPath(*self.original)]
        result.extend(RspPath(*self.variants[region, trial]) for trial in trials)
        for machine in result:
            machine.simulate_dsp = True
            machine.reg[24] = 0x360
            machine.reg[23] = 0xf90
        return result

    def test_fit_and_abi_detection(self):
        for (region, trial), (code, _, data) in self.variants.items():
            with self.subTest(region=region, trial=trial):
                self.assertLessEqual(len(code), 4096 - 0x80)
                self.assertEqual(data[0x28:0x2c], bytes.fromhex("1e24138c"))
                self.assertEqual(data[:4], bytes.fromhex("00000001"))
                self.assertEqual(data[0x40:], self.original[2][0x40:])

    def test_decoder_pcm_history_and_predictor_changes(self):
        rng = random.Random(530)
        book = struct.pack(">256h", *(rng.randrange(-7000, 7001) for _ in range(256)))
        history = rng.randbytes(32)
        loop_history = rng.randbytes(32)
        for region in ("US", "JP", "EU"):
            for count in (0, 1, 16, 32, 33, 64, 96, 128, 320):
                for flags in (0, 1, 2, 3):
                    for repeat in (False, True):
                        with self.subTest(region=region, count=count, flags=flags, repeat=repeat):
                            blocks = (count + 31) // 32
                            stream = b"".join(bytes([(index % 13) << 4 | (3 if repeat else index % 16)])
                                              + rng.randbytes(8) for index in range(blocks + 1))
                            machines = self.machines(region, (0, 1, 3, 4, 5, 10))
                            for machine in machines:
                                machine.store(0x4c0, book)
                                machine.store(0x800, stream)
                                machine.store(0x360, struct.pack(">3H", 0x800, 0xa00, count))
                                machine.store(0x370, struct.pack(">I", 0x3000))
                                machine.rdram[0x2000:0x2020] = history
                                machine.rdram[0x3000:0x3020] = loop_history
                                machine.reg[26] = 0x01000000 | flags << 16
                                machine.reg[25] = 0x2000
                                machine.run("cmd_adpcm", ["cmd_spnoop"])
                            for machine in machines[1:]:
                                self.assertEqual(machine.load(0xa00, 32 + blocks * 32), machines[0].load(0xa00, 32 + blocks * 32))
                                self.assertEqual(machine.rdram, machines[0].rdram)
                            if blocks:
                                # The last-block trial never reads the unused next payload.
                                final_read = 0x800 + blocks * 9 + 1
                                self.assertNotIn((final_read, 8), machines[4].reads)
                                self.assertLess(machines[4].steps, machines[0].steps)
                                self.assertNotIn((final_read, 8), machines[6].reads)
                            if repeat and blocks >= 4:
                                self.assertLess(machines[5].steps, machines[0].steps)
                                original_reads = sum(0x4c0 <= address < 0x6c0 for address, _ in machines[0].reads)
                                combined_reads = sum(0x4c0 <= address < 0x6c0 for address, _ in machines[6].reads)
                                self.assertLess(combined_reads, original_reads)

    def test_resampler_pcm_phase_and_history(self):
        rng = random.Random(640)
        samples = rng.randbytes(0x800)
        for region in ("US", "JP", "EU"):
            for count in (16, 32, 128, 256, 320):
                for pitch in (1, 0x2000, 0x7fff, 0x8000, 0x8001, 0xc000, 0xffff):
                    with self.subTest(region=region, count=count, pitch=pitch):
                        machines = self.machines(region, (0, 1, 7, 10))
                        for phase in (0, 1, 0x7fff, 0xffff):
                            for machine in machines:
                                machine.store(0x5c0, samples)
                                machine.rdram[0x2008:0x200a] = struct.pack(">H", phase)
                                machine.store(0x360, struct.pack(">3H", 0x600, 0xd00, count))
                                machine.reg[26] = 0x05000000 | ((1 if phase == 0 else 0) << 16) | pitch
                                machine.reg[25] = 0x2000
                                machine.run("cmd_resample", ["cmd_spnoop"])
                            for machine in machines[1:]:
                                self.assertEqual(machine.load(0xd00, count), machines[0].load(0xd00, count))
                                self.assertEqual(machine.rdram, machines[0].rdram)
                        if count >= 128:
                            self.assertLess(machines[3].steps, machines[0].steps)

    def test_exponential_mix_pcm_and_continued_state(self):
        rng = random.Random(6401)
        buffers = (0x600, 0x740, 0x880, 0xa00, 0xb40)
        for region in ("US", "JP", "EU"):
            for count in (16, 32, 128, 256, 320):
                for auxiliary in (False, True):
                    for rate in (0, 0x9000, 0xffff, 0x10000, 0x10001, 0x18000):
                        with self.subTest(region=region, count=count, auxiliary=auxiliary, rate=rate):
                            machines = self.machines(region, (0, 1, 2, 6, 10))
                            for update in range(3):
                                block = rng.randbytes(320)
                                for machine in machines:
                                    for address in buffers:
                                        machine.store(address, block)
                                    values = (*buffers[:2], count, 12000, 8000, *buffers[2:],
                                              12000, rate >> 16, rate & 65535, 8000,
                                              rate >> 16, rate & 65535, 0x7fff, 0x6000)
                                    machine.store(0x360, struct.pack(">16H", *values))
                                    machine.reg[26] = 0x03000000 | ((int(auxiliary) * 8 | (update == 0)) << 16)
                                    machine.reg[25] = 0x2000
                                    machine.run("cmd_envmixer", ["cmd_spnoop"])
                                for machine in machines[1:]:
                                    for address in buffers[1:]:
                                        self.assertEqual(machine.load(address, count), machines[0].load(address, count))
                                    self.assertEqual(machine.rdram, machines[0].rdram)
                                if rate == 0x10000 and count >= 128:
                                    fast = machines[4]
                                    self.assertIn(fast.symbols["@@constant_loop"] & 4095, fast.visited)
                                    self.assertLess(fast.steps, machines[0].steps)
                                    combined = machines[5]
                                    self.assertIn(combined.symbols["@@constant_loop"] & 4095, combined.visited)
                                if count >= 128 and not auxiliary:
                                    self.assertLess(machines[3].steps, machines[0].steps)

    def test_fractional_or_unsettled_state_uses_original_mixer(self):
        buffers = (0x600, 0x740, 0x880, 0xa00, 0xb40)
        for mismatch in ("fraction", "integer", "rate", "carry"):
            machines = self.machines("US", (6, 10))
            state = bytearray(struct.pack(">40H", *([12000] * 8 + [0] * 8 + [8000] * 8 + [0] * 8
                                                     + [12000, 1, 0, 8000, 1, 0, 0x7fff, 0x3000])))
            if mismatch == "fraction":
                state[30:32] = b"\x00\x01"
            elif mismatch == "integer":
                state[12:14] = struct.pack(">H", 11999)
            elif mismatch == "rate":
                state[70:72] = b"\x00\x01"
            for machine in machines:
                machine.rdram[0x2000:0x2050] = state
                machine.store(0x360, struct.pack(">8H", *buffers[:2], 320, 12000, 8000, *buffers[2:]))
                machine.reg[26] = 0x03080000
                machine.reg[25] = 0x2000
                if mismatch == "carry":
                    machine.carry[2] = 1
                machine.run("cmd_envmixer", ["cmd_spnoop"])
            for machine in machines[1:]:
                self.assertNotIn(machine.symbols["@@constant_loop"] & 4095, machine.visited)
                self.assertEqual(machine.rdram, machines[0].rdram)
                for address in buffers[1:]:
                    self.assertEqual(machine.load(address, 320), machines[0].load(address, 320))


if __name__ == "__main__":
    unittest.main()
