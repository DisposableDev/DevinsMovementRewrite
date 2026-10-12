#!/usr/bin/env python3
"""Reconstruct generated C numeric initializers from a privately supplied US ROM."""
import hashlib
import gzip
import math
import json
from pathlib import Path
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
TOKEN = re.compile(r'@([-+]?)(?:(0[xX])(\d+)([xX])|(d))@')

def render(recipe, raw):
    data = bytearray(raw)
    for offset, value in recipe['edits']:
        if not 0 <= offset < len(data) or not 0 <= value <= 255:
            raise ValueError('Invalid initializer edit')
        data[offset] = value
    if recipe['type'] == 'Vtx':
        if len(data) % 16:
            raise ValueError('Invalid vertex size')
        values = [(value, 16 if i < 6 else 8)
                  for pos in range(0, len(data), 16)
                  for i, value in enumerate(struct.unpack_from('>6H4B', data, pos))]
    else:
        if len(data) % 2:
            raise ValueError('Invalid initializer size')
        values = [(v, 16) for (v,) in struct.iter_unpack('>H', data)]
    cursor = 0
    def token(match):
        nonlocal cursor
        if cursor >= len(values):
            raise ValueError('Too many template values')
        value, bits = values[cursor]
        cursor += 1
        sign, prefix, width, letter, decimal = match.groups()
        if sign == '-':
            value = (1 << bits) - value if value else 0
        if decimal:
            return sign + str(value)
        return sign + prefix + format(value, '0' + width + letter)
    body = TOKEN.sub(token, recipe['template'])
    if cursor != len(values) or hashlib.sha256(body.encode()).hexdigest() != recipe['sha256']:
        raise ValueError('Reconstructed initializer does not match its pinned hash')
    return body

def reconstruct(root=ROOT):
    manifest = root / 'assets/rom/arrays.json.gz'
    if not manifest.exists():
        return
    recipes = json.loads(gzip.decompress(manifest.read_bytes()))
    output = root / 'assets/rom/generated'
    output.mkdir(parents=True, exist_ok=True)
    pending = []
    for key, recipe in recipes.items():
        if not re.fullmatch('[0-9a-f]{20}', key):
            raise ValueError('Invalid initializer name')
        target = output / (key + '.inc.c')
        if target.exists() and hashlib.sha256(target.read_bytes()).hexdigest() == recipe['sha256']:
            continue
        pending.append((target, recipe))
    if not pending:
        return
    try:
        from .detect_baseroms import get_rom_candidates
        from .reconstruct_animations import reconstruct_animation_array
        from .reconstruct_actors import reconstruct_actor_array
    except ImportError:
        from detect_baseroms import get_rom_candidates
        from reconstruct_animations import reconstruct_animation_array
        from reconstruct_actors import reconstruct_actor_array
    roms = get_rom_candidates()
    if 'us' not in roms:
        raise ValueError('US baserom is required to reconstruct model and animation data')
    rompath = Path(roms['us']).resolve()
    rom = rompath.read_bytes()
    if hashlib.sha1(rom).hexdigest() != '9bef1128717f958171a4afac3ed78ee2bb4e86ce':
        raise ValueError('US baserom checksum mismatch')
    blobs = {None: rom}
    with tempfile.TemporaryDirectory(prefix='sm64-rom-arrays-') as temporary:
        cache = {}
        visiting = set()
        def raw_initializer(key):
            if key in cache:
                return cache[key]
            if key in visiting:
                raise ValueError('Cyclic initializer dependency')
            visiting.add(key)
            recipe = recipes[key]
            kind = recipe.get('transform_kind')
            if kind == 'body':
                parent = raw_initializer(recipe['source_ref'])
                scale = recipe['scale']
                vertices = bytearray()
                for fields in struct.iter_unpack('>6h4B', parent):
                    values = list(fields)
                    normal = [(n if n < 128 else n - 256) / factor
                              for n, factor in zip(values[6:9], scale)]
                    length = math.sqrt(sum(n * n for n in normal))
                    if length:
                        normal = [round(n * 127 / length) for n in normal]
                    values[:3] = [round(n * factor) for n, factor in zip(values[:3], scale)]
                    values[6:9] = [int(n) & 255 for n in normal]
                    vertices.extend(struct.pack('>6h4B', *values))
                raw = bytes(vertices)
            elif kind == 'animation':
                raw = reconstruct_animation_array(rom, recipe['transform'])
            else:
                source = recipe['source']
                if source['region'] != 'us':
                    raise ValueError('Unsupported reconstruction region')
                segment = source['segment']
                if segment not in blobs:
                    decoded = Path(temporary) / f'{segment:x}.bin'
                    subprocess.run([str(root / 'tools/mio0'), '-d', '-o', str(segment),
                                    str(rompath), str(decoded)], check=True)
                    blobs[segment] = decoded.read_bytes()
                blob = blobs[segment]
                if kind == 'actor':
                    raw = reconstruct_actor_array(blob, recipe)
                else:
                    start, size = source['offset'], recipe['bytes']
                    if start < 0 or size < 0 or start + size > len(blob):
                        raise ValueError('Initializer source range is outside ROM data')
                    raw = blob[start:start + size]
            data = bytearray(raw)
            for offset, value in recipe['edits']:
                if not 0 <= offset < len(data) or not 0 <= value <= 255:
                    raise ValueError('Invalid initializer edit')
                data[offset] = value
            if len(data) != recipe['bytes']:
                raise ValueError('Initializer size mismatch')
            visiting.remove(key)
            cache[key] = bytes(data)
            return cache[key]
        for target, recipe in pending:
            body = render(recipe, raw_initializer(target.stem.removesuffix('.inc')))
            target.write_text(body)
    print(f'Reconstructed {len(pending)} ROM-derived initializers.')

if __name__ == '__main__':
    reconstruct()
