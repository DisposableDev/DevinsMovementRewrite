from pathlib import Path
import struct, collections, math, json, hashlib
root = Path(__file__).resolve().parents[3]
b = (root / 'sound/custom/good-or-dont-be/arrangement.mid').read_bytes()
_, fmt, nt, ppq = struct.unpack('>IHHH', b[4:14])
pos = 14
tracks = []
marker = None
tempo = None
total = 0
for t in range(nt):
    n = int.from_bytes(b[pos + 4:pos + 8], 'big')
    d = b[pos + 8:pos + 8 + n]
    pos += 8 + n
    i = 0
    tick = 0
    running = 0
    events = []
    notes = []
    active = {}
    name = ''
    program = None

    def vlq():
        global i
        n = 0
        while True:
            v = d[i]
            i += 1
            n = n << 7 | v & 127
            if not v & 128:
                return n
    while i < len(d):
        tick += vlq()
        st = d[i]
        if st >= 128:
            i += 1
            running = st if st < 240 else running
        else:
            st = running
        if st == 255:
            typ = d[i]
            i += 1
            n = vlq()
            v = d[i:i + n]
            i += n
            if typ == 3:
                name = v.decode()
            if typ == 6 and v == b'Section 1':
                marker = tick
            if typ == 81:
                assert tempo in (None, int.from_bytes(v, 'big'))
                tempo = int.from_bytes(v, 'big')
        elif st in (240, 247):
            n = vlq()
            i += n
        else:
            typ = st >> 4
            ch = st & 15
            n = 1 if typ in (12, 13) else 2
            v = d[i:i + n]
            i += n
            if typ == 12:
                assert program in (None, v[0])
                program = v[0]
            if typ == 11:
                events.append((tick, v[0], v[1]))
            if typ == 9 and v[1]:
                active.setdefault(v[0], []).append((tick, v[1]))
            if typ == 8 or (typ == 9 and (not v[1])):
                start, vel = active[v[0]].pop(0)
                notes.append((start, tick, v[0], vel))
                if not active[v[0]]:
                    del active[v[0]]
    assert not active
    total = max(total, tick)
    if notes:
        tracks.append((name, program, events, sorted(notes)))
assert marker == 72960 and total == 103680 and (ppq == 480)
assert all((not a < marker < z for _, _, _, notes in tracks for a, z, _, _ in notes))

def qt(x):
    return round(x * 48 / ppq)
M, T = (qt(marker), qt(total))
asm = [
    '#include "seq_macros.inc"',
    '.section .rodata',
    '.align 0',
    'sequence_start:',
    'seq_setmutebhv 0x60',
    'seq_setmutescale 0',
    'seq_setvol 127',
    f'seq_settempo {round(60000000 / tempo)}',
    'seq_initchannels 0xffff',
]
parts = []
channel = 0
report = []
for ti, (name, program, events, notes) in enumerate(tracks):
    assert program in (4, 7, 8, 13)
    voices = []
    for note in notes:
        stream = next((v for v in voices if v[-1][1] <= note[0]), None)
        if stream is None:
            stream = []
            voices.append(stream)
        stream.append(note)
    report.append({'track': name, 'instrument': program, 'notes': len(notes), 'voices': len(voices)})
    for vi in range(0, len(voices), 4):
        ch = channel
        channel += 1
        assert channel <= 16
        asm.append(f'seq_startchannel {ch}, channel_{ch}')
        p = [
            f'channel_{ch}:',
            'chan_largenoteson',
            f'chan_setinstr {program}',
            'chan_setvol 127',
            'chan_setvolscale 127',
        ]
        for li, v in enumerate(voices[vi:vi + 4]):
            p.append(f'chan_setlayer {li}, layer_{ch}_{li}')
        ce = collections.defaultdict(list)
        for tick, cc, val in events:
            cmd = {
                7: 'chan_setvol',
                10: 'chan_setpan',
                11: 'chan_setvolscale',
                91: 'chan_setreverb',
            }.get(cc)
            if cmd:
                ce[qt(tick)].append(f'{cmd} {val}')
        ce[M]
        ce[T]
        now = 0
        for tick, commands in sorted(ce.items()):
            if tick > now:
                p.append(f'chan_delay {tick - now}')
                now = tick
            if tick == M:
                p.append(f'channel_{ch}_repeat:')
            p.extend(commands)
        p.append(f'chan_jump channel_{ch}_repeat')
        parts += p
        for li, v in enumerate(voices[vi:vi + 4]):
            transpose = max(0, max((n[2] for n in v)) - 84)
            p = [f'layer_{ch}_{li}:', f'layer_transpose {transpose}']
            now = 0
            for start, end, key, vel in v:
                start, end = (qt(start), qt(end))
                end = max(start + 1, end)
                if now <= M <= start:
                    if M > now:
                        p.append(f'layer_delay {M - now}')
                    p.append(f'layer_{ch}_{li}_repeat:')
                    now = M
                    M_done = True
                if start > now:
                    p.append(f'layer_delay {start - now}')
                pitch = key - 21 - transpose
                assert 0 <= pitch < 64
                p.append(f'layer_note0 {pitch}, {end - start}, {vel}, 0')
                now = end
            # Each voice has notes after the repeat marker in this arrangement.
            assert any(('_repeat:' in x for x in p))
            if now < T:
                p.append(f'layer_delay {T - now}')
            p.append(f'layer_jump layer_{ch}_{li}_repeat')
            parts += p
asm += [f'seq_delay {M}', 'sequence_repeat:', f'seq_delay {T - M}', 'seq_jump sequence_repeat'] + parts
# Exactly one repeat label per stream; no crossing or held notes at either boundary.
assert len([x for x in asm if x.endswith('_repeat:')]) == (
    1 + channel + sum((x['voices'] for x in report))
)
(root / 'sound/sequences/24_custom_good_or_dont_be.s').write_text('\n'.join(asm) + '\n')
(root / 'sound/custom/good-or-dont-be/conversion.json').write_text(
    json.dumps({
        'midi_sha256': hashlib.sha256(b).hexdigest(),
        'tempo_bpm': round(60000000 / tempo),
        'native_repeat_tick': M,
        'native_end_tick': T,
        'intro_seconds': marker / ppq * tempo / 1e6,
        'repeat_seconds': (total - marker) / ppq * tempo / 1e6,
        'channels': channel,
        'tracks': report,
        'boundary_crossing_notes': 0,
    }, indent=2) + '\n'
)
print(report, 'channels', channel)
