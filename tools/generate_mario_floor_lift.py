#!/usr/bin/env python3
"""Bake capped flat-ground shoe penetration for ordinary native Mario poses."""
import argparse
import math
from pathlib import Path
import re

from generate_mario_silhouette import collect_geometry, palette_source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'src/game/mario_floor_lift.inc.c'
ANIMATIONS = (0x48, 0x72, 0x92, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0xC3, 0xC4, 0xC5, 0xCA)


def skeleton():
    geo = palette_source(read_source(ROOT / 'actors/mario/palette_geo.inc.c'), '', '')
    body = re.search(r'const GeoLayout mario_geo_body\[\] = \{(.*?)\n\};', geo, re.S)[1]
    nodes = []
    parents = [-1]
    current = -1
    for command, args in re.findall(r'GEO_(\w+)\(([^)]*)\)', body):
        if command == 'ANIMATED_PART':
            translation = tuple(int(x.strip()) for x in args.split(',')[1:4])
            nodes.append((parents[-1], translation))
            current = len(nodes) - 1
        elif command == 'OPEN_NODE':
            parents.append(current)
        elif command == 'CLOSE_NODE':
            parents.pop()
            current = parents[-1]
        elif command == 'BRANCH' and args.split(',')[1].strip() in (
                'mario_geo_left_hand', 'mario_geo_right_hand'):
            # Every hand choice consumes one identical animated node.
            nodes.append((parents[-1], (60, 0, 0)))
    if len(nodes) != 20 or nodes[15] != (14, (67, 0, 0)) or nodes[19] != (18, (67, 0, 0)):
        raise ValueError('Review changed Mario skeleton before baking foot clearance')
    return nodes


def multiply(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(4)) for j in range(4)] for i in range(4)]


def transform(rotation, translation, sine):
    sx, sy, sz = [sine[(x & 65535) >> 4] for x in rotation]
    cx, cy, cz = [sine[1024 + ((x & 65535) >> 4)] for x in rotation]
    # Transpose of the native row-vector mtxf_rotate_xyz_and_translate layout.
    return [[cy * cz, sx * cz * sy - cx * sz, cx * cz * sy + sx * sz, translation[0]],
            [cy * sz, sx * sz * sy + cx * cz, cx * sz * sy - sx * cz, translation[1]],
            [-sy, sx * cy, cx * cy, translation[2]], [0, 0, 0, 1]]


def shoes():
    blocks, mapping, _ = collect_geometry()
    model = read_source(ROOT / 'actors/mario/model.inc.c')
    vertices = {m[1]: [tuple(map(int, xyz)) for xyz in re.findall(
        r'\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\}', m[2])]
        for m in re.finditer(r'static const Vtx (\w+)\[\] = \{(.*?)\n\};', model, re.S)}
    result = [set(), set()]
    for side, name in enumerate(('left', 'right')):
        for detail in ('', 'medium_poly_', 'low_poly_'):
            cache = {}
            for leaf in mapping[f'mario_{detail}{name}_foot']:
                for command, args in re.findall(r'(gsSP\w+)\(([^)]*)\)', blocks[leaf]):
                    if command == 'gsSPVertex':
                        array, count, slot = [x.strip() for x in args.split(',')]
                        parts = array.split('+')
                        offset = int(parts[1], 0) if len(parts) == 2 else 0
                        points = vertices[parts[0].strip()][offset:offset + int(count)]
                        for i, point in enumerate(points):
                            cache[int(slot) + i] = point
                    elif command in ('gsSP1Triangle', 'gsSP2Triangles'):
                        fields = [int(x.strip(), 0) for x in args.split(',')]
                        indices = (0, 1, 2, 4, 5, 6) if command == 'gsSP2Triangles' else (0, 1, 2)
                        result[side].update(cache[fields[i]] for i in indices)
    return result


def animation(anim_id):
    paths = [p for p in (ROOT / 'assets/anims').glob('anim_*.inc.c')
             if f'{anim_id:02X}' in p.name[5:-6].split('_')]
    if len(paths) != 1:
        raise ValueError(f'Expected one native animation for {anim_id:02X}')
    text = read_source(paths[0])
    header = text.split('};', 1)[0].split('{', 1)[1]
    flags, divisor, _, _, frames = [int(x.strip(), 0) for x in header.split(',')[:5]]
    arrays = {}
    for name in ('indices', 'values'):
        body = re.search(rf'anim_[0-9A-F_]+_{name}\[\] = \{{(.*?)\n\}};', text, re.S)[1]
        arrays[name] = [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|-?\d+', body)]
    if len(arrays['indices']) != 126 or divisor != 189 or flags & 0x20:
        raise ValueError('Review changed native animation layout')
    arrays['values'] = [(x + 32768) % 65536 - 32768 for x in arrays['values']]
    return flags, frames, arrays


def bake():
    nodes, meshes = skeleton(), shoes()
    trig = read_source(ROOT / 'include/trig_tables.inc.c').split('};', 1)[0]
    sine = [float(x) for x in re.findall(r'(-?\d+\.\d+(?:[eE][+-]?\d+)?)f', trig)]
    scale = [[.25, 0, 0, 0], [0, .25, 0, 0], [0, 0, .25, 0], [0, 0, 0, 1]]
    data, records = [], []
    for anim_id in ANIMATIONS:
        flags, frames, arrays = animation(anim_id)
        records.append((len(data), frames, anim_id))
        for frame in range(frames):
            def channel(i):
                count, offset = arrays['indices'][2 * i:2 * i + 2]
                return arrays['values'][offset + min(frame, count - 1)]
            root = [channel(i) for i in range(3)]
            if flags & 8:
                root[0] = root[2] = 0
            elif flags & 16:
                root[1] = 0
            elif flags & 64:
                root = [0, 0, 0]
            matrices = []
            for node, (parent, offset) in enumerate(nodes):
                translation = [offset[i] + (root[i] if node == 0 else 0) for i in range(3)]
                local = transform([channel(3 + node * 3 + i) for i in range(3)], translation, sine)
                matrices.append(multiply(scale if parent < 0 else matrices[parent], local))
            lowest = min(sum(matrices[node][1][j] * point[j] for j in range(3))
                         + matrices[node][1][3] for side, node in enumerate((15, 19))
                         for point in meshes[side])
            data.append(max(0, min(128, math.ceil(-lowest * 16))))
    return data, records


def output():
    data, records = bake()
    lines = ['// Generated by tools/generate_mario_floor_lift.py. Do not edit by hand.',
             '// Maximum native shoe penetration across all details, capped at 8 world units.',
             '// Each byte represents 1/16 world unit; only the listed native poses qualify.',
             'struct MarioFloorLiftAnim {', '    u16 offset;', '    u8 frames;', '    u8 animID;',
             '};', '', '// clang-format off', 'static const u8 sMarioFloorLiftDepth[] = {']
    lines += ['    ' + ', '.join(map(str, data[i:i + 20])) + ',' for i in range(0, len(data), 20)]
    lines += ['};', '', 'static const struct MarioFloorLiftAnim sMarioFloorLiftAnims[] = {']
    lines += [f'    {{ {offset}, {frames}, 0x{anim:02X} }},' for offset, frames, anim in records]
    return '\n'.join(lines + ['};', '// clang-format on', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    text = output()
    if args.check:
        if not OUTPUT.exists() or read_source(OUTPUT) != text:
            raise SystemExit('Mario floor-lift pose data is stale')
    else:
        OUTPUT.write_text(text)
    print('Mario floor-lift pose data is current.')


if __name__ == '__main__':
    main()
