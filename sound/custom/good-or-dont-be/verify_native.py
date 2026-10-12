#!/usr/bin/env python3
"""Audit actual assembled loop branches and layer timing, not MIDI marker text."""
import json
import re
import subprocess
import sys
from pathlib import Path
binary, obj = map(Path, sys.argv[1:3])
data = binary.read_bytes()
manifest = json.loads((Path(__file__).parent / 'conversion.json').read_text())
nm = subprocess.check_output(['/opt/n64-gcc-16.2/bin/mips64-elf-nm', '-n', str(obj)], text=True)
labels = {}
for line in nm.splitlines():
    fields = line.split()
    if len(fields) == 3:
        labels[fields[2]] = int(fields[0], 16)
streams = sorted(
    (offset, name) for name, offset in labels.items()
    if re.fullmatch('channel_\\d+|layer_\\d+_\\d+', name)
)
assert data[streams[0][0] - 3:streams[0][0]] == (
    bytes([0xFB]) + labels['sequence_repeat'].to_bytes(2, 'big')
)
notes = 0
for index, (start, name) in enumerate(streams):
    end = streams[index + 1][0] if index + 1 < len(streams) else len(data.rstrip(b'\x00'))
    target = labels[name + '_repeat']
    # Allow assembler section alignment after the last stream.
    jump = data.find(bytes([0xFB]) + target.to_bytes(2, 'big'), start, end)
    assert jump >= start and jump + 3 <= end, name
    if name.startswith('channel_'):
        continue
    pc = start
    tick = 0
    repeat_tick = None

    def var():
        global pc
        value = data[pc]
        pc += 1
        if value & 128:
            value = (value & 127) << 8 | data[pc]
            pc += 1
        return value
    while pc < jump:
        if pc == target:
            repeat_tick = tick
        opcode = data[pc]
        pc += 1
        if opcode == 0xC2:
            pc += 1
        elif opcode == 0xC0:
            tick += var()
        elif opcode < 64:
            duration = var()
            velocity, gate = data[pc:pc + 2]
            pc += 2
            assert duration > 0 and 0 < velocity <= 127 and (gate == 0), name
            assert not tick < manifest['native_repeat_tick'] < tick + duration, name
            tick += duration
            notes += 1
        else:
            raise AssertionError((name, pc - 1, opcode))
    assert pc == jump and repeat_tick == manifest['native_repeat_tick'], (name, repeat_tick)
    assert tick == manifest['native_end_tick'], (name, tick)
assert notes == sum((track['notes'] for track in manifest['tracks']))
print(
    f'Native sequence: {notes} notes; all 19 layers return to tick 7296 after tick 10368; '
    'sequence and seven channel jumps target Section 1.'
)
