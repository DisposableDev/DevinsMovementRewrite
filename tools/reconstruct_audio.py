"""Rebuild local trims and cap-song samples from extracted original AIFF inputs."""
import hashlib
import json
from pathlib import Path
import struct


def read_chunks(data):
    if data[:4] != b'FORM' or data[8:12] != b'AIFF':
        raise ValueError('Expected AIFF input')
    if int.from_bytes(data[4:8], 'big') + 8 != len(data):
        raise ValueError('Invalid AIFF length')
    chunks = {}
    position = 12
    while position < len(data):
        if position + 8 > len(data):
            raise ValueError('Truncated AIFF chunk')
        name = data[position:position + 4]
        size = int.from_bytes(data[position + 4:position + 8], 'big')
        end = position + 8 + size
        if end > len(data) or name in chunks:
            raise ValueError('Invalid AIFF chunk')
        chunks[name] = data[position + 8:end]
        position = end + (size & 1)
    return chunks


def chunk(name, data):
    return name + struct.pack('>I', len(data)) + data + (b'\0' if len(data) & 1 else b'')


def half_rate(data, coefficients, rate):
    parts = read_chunks(data)
    comm = bytearray(parts[b'COMM'])
    if len(comm) != 18 or comm[:2] != b'\0\1' or comm[6:8] != b'\0\x10':
        raise ValueError('Expected mono 16-bit AIFF')
    sound = parts[b'SSND']
    if sound[:8] != bytes(8) or len(sound) & 1:
        raise ValueError('Unexpected sound offset or alignment')
    samples = list(struct.unpack('>' + str((len(sound) - 8) // 2) + 'h', sound[8:]))
    if len(samples) < 65 or len(coefficients) != 129:
        raise ValueError('Invalid filter input')
    filtered = []
    for position in range(0, len(samples), 2):
        total = 0.0
        for tap in range(129):
            index = position + tap - 64
            if index < 0:
                index = -index
            if index >= len(samples):
                index = 2 * len(samples) - 2 - index
            total += samples[index] * coefficients[128 - tap]
        filtered.append(max(-32768, min(32767, round(total))))
    comm[2:6] = struct.pack('>I', len(filtered))
    comm[8:18] = bytes.fromhex(rate)
    markers = bytearray(parts[b'MARK'])
    position = 2
    for _ in range(int.from_bytes(markers[:2], 'big')):
        old = int.from_bytes(markers[position + 2:position + 6], 'big')
        markers[position + 2:position + 6] = struct.pack('>I', round(old / 2))
        length = markers[position + 6]
        position += 7 + length + (length % 2 == 0)
    pcm = struct.pack('>' + str(len(filtered)) + 'h', *filtered)
    body = (b'AIFF' + chunk(b'COMM', bytes(comm)) + chunk(b'MARK', bytes(markers))
            + chunk(b'INST', parts[b'INST']) + chunk(b'SSND', bytes(8) + pcm))
    return b'FORM' + struct.pack('>I', len(body)) + body


def safe_path(root, name):
    relative = Path(name)
    if relative.is_absolute() or '..' in relative.parts:
        raise ValueError('Asset path must be inside the project')
    target = root / relative
    if not target.resolve().is_relative_to(root.resolve()):
        raise ValueError('Asset path escapes the project')
    return target


def reconstruct(root):
    manifest = root / 'assets/rom/audio.json'
    if not manifest.exists():
        return
    settings = json.loads(manifest.read_text())
    count = 0
    for recipe in settings['recipes']:
        target = safe_path(root, recipe['path'])
        if target.exists():
            if hashlib.sha256(target.read_bytes()).hexdigest() != recipe['sha256']:
                raise ValueError(f'Modified generated asset: {recipe["path"]}')
            continue
        source = safe_path(root, recipe['source']).read_bytes()
        if hashlib.sha256(source).hexdigest() != recipe['source_sha256']:
            raise ValueError(f'Source sample checksum mismatch: {recipe["source"]}')
        if recipe['kind'] == 'half_rate':
            data = half_rate(source, settings['cap_filter'], recipe['rate'])
        else:
            fragments = []
            for operation in recipe['operations']:
                if operation[0] == 'copy':
                    offset, size = operation[1:]
                    if offset < 0 or size < 0 or offset + size > len(source):
                        raise ValueError('Sample copy range is outside source')
                    fragments.append(source[offset:offset + size])
                elif operation[0] == 'literal':
                    fragments.append(bytes.fromhex(operation[1]))
                else:
                    raise ValueError('Unknown audio reconstruction operation')
            data = b''.join(fragments)
        if hashlib.sha256(data).hexdigest() != recipe['sha256']:
            raise ValueError(f'Generated sample checksum mismatch: {recipe["path"]}')
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        count += 1
    if count:
        print(f'Reconstructed {count} derived audio samples.')
