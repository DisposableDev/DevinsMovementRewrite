#!/usr/bin/env python3
"""Validate playground model layouts offline and export simple drawing limits.

Unknown graph operations, transform callbacks and matrix-changing display lists
retain ordinary rendering. Pure branch selectors include every alternative.
The parser retains all-pose bounds for offline
checks; no animation or shadow envelope is evaluated by the runtime policy.
"""
import argparse
from dataclasses import dataclass, field
import math
from pathlib import Path
import re

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'levels/physics_lab/object_bounds.inc.c'


def clean(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)


class Unsupported(ValueError):
    pass


@dataclass
class Node:
    op: str = 'GEO_NODE_START'
    args: list = field(default_factory=list)
    children: list = field(default_factory=list)


class Bounds:
    def __init__(self):
        self.arrays = {}
        paths = list((ROOT / 'actors').glob('*/model.inc.c')) + list((ROOT / 'actors').glob('*/geo.inc.c'))
        paths += [ROOT / f'levels/{folder}/{name}.inc.c'
                  for folder in ('ssl/grindel', 'ssl/spindel', 'ssl/tox_box', 'hmc/rolling_rock')
                  for name in ('model', 'geo')]
        paths += [ROOT / 'levels/physics_lab/model.inc.c', ROOT / 'levels/physics_lab/geo.c']
        for path in paths:
            text = clean(read_source(path))
            if path == ROOT / 'actors/mario_cap/model.inc.c':
                # Expand the cap's material names for each included palette.
                # Both variants share meshes; their RSP commands still pass display().
                materials = clean(read_source(ROOT / 'actors/mario_cap/palette_materials.inc.c'))
                names = re.findall(r'^#define MARIO_PALETTE_NAME\(name\) (name|\w+##name)$',
                                   text, re.M)
                if len(names) != text.count('#include "actors/mario_cap/palette_materials.inc.c"'):
                    raise Unsupported('cap palette material names')
                for naming in names:
                    prefix = '' if naming == 'name' else naming.removesuffix('##name')
                    text += '\n' + re.sub(r'\bMARIO_PALETTE_NAME\((\w+)\)',
                                          lambda match: prefix + match[1], materials)
            if path.name == 'geo.c':
                # Expand the lab's authored macro, so edits to its body are
                # validated by the same fail-closed parser as other layouts.
                macro = re.search(r'#define LAB_MOVER_GEO\(name, radius\) ([\s\S]*?)\n\n', text)[1]
                macro = macro.replace('\\\n', '\n')

                def mover(match):
                    return macro.replace('##name##', match[1]).replace('(radius)', f'({match[2]})')

                text = re.sub(r'^LAB_MOVER_GEO\((\w+), (\d+)\)$', mover, text, flags=re.M)
            for kind, name, body in re.findall(r'\b(Vtx|Gfx|GeoLayout)\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};', text, re.S):
                self.arrays[name] = (kind, body)
        self.cache = {}

    def display(self, name, seen=()):
        if name == 'NULL':
            return 0.0
        if name in self.cache:
            return self.cache[name]
        if name not in self.arrays or name in seen:
            raise Unsupported(name)
        kind, body = self.arrays[name]
        if kind != 'Gfx':
            raise Unsupported(name)
        radius = 0.0
        commands = re.findall(r'\b(gs\w+)\(([^()]*)\)', body)
        if len(re.findall(r'\bgsSP\w+\(', body)) != sum(op.startswith('gsSP') for op, _ in commands):
            raise Unsupported('nested RSP command arguments')
        for op, args in commands:
            args = [a.strip() for a in args.split(',')]
            if op == 'gsSPVertex':
                array = args[0].split()[0]
                if array not in self.arrays:
                    raise Unsupported(array)
                entries = re.findall(r'\{\s*\{\s*\{?([^{}]+)\}', self.arrays[array][1])
                try:
                    points = [tuple(int(c.strip(), 0) for c in entry.split(',')) for entry in entries]
                except ValueError as exc:
                    raise Unsupported(array) from exc
                if any(len(p) != 3 for p in points):
                    raise Unsupported(array)
                if not points:
                    raise Unsupported(array)
                radius = max(radius, max(math.sqrt(sum(int(c)**2 for c in p)) for p in points))
            elif op in ('gsSPDisplayList', 'gsSPBranchList'):
                radius = max(radius, self.display(args[0], (*seen, name)))
            elif not (op.startswith('gsDP') or op in (
                    'gsSP1Triangle', 'gsSP2Triangles', 'gsSP1Quadrangle', 'gsSPEndDisplayList',
                    'gsSPClearGeometryMode', 'gsSPSetGeometryMode', 'gsSPTexture',
                    'gsSPLight', 'gsSPLightColor', 'gsSPNumLights', 'gsSPSetLights1', 'gsSPSetLights2',
                    'gsSPFogFactor', 'gsSPFogPosition')):
                raise Unsupported(op)
        self.cache[name] = radius
        return radius

    def triangles(self, root):
        """Offline render-cost estimate; switch branches are alternatives."""
        cache = {}

        def display(name):
            if name == 'NULL':
                return 0
            if name in cache:
                return cache[name]
            count = 0
            for op, args in re.findall(r'\b(gs\w+)\(([^()]*)\)', self.arrays[name][1]):
                if op == 'gsSP1Triangle':
                    count += 1
                elif op in ('gsSP2Triangles', 'gsSP1Quadrangle'):
                    count += 2
                elif op in ('gsSPDisplayList', 'gsSPBranchList'):
                    count += display(args.strip())
            cache[name] = count
            return count

        def visit(node):
            children = [visit(child) for child in node.children]
            count = max(children, default=0) if node.op == 'GEO_SWITCH_CASE' else sum(children)
            if node.op in ('GEO_ANIMATED_PART', 'GEO_DISPLAY_LIST') or node.op.endswith('_WITH_DL'):
                count += display(node.args[-1])
            return count

        return visit(root)

    def layout(self, name, seen=()):
        if name not in self.arrays or name in seen:
            raise Unsupported(name)
        kind, body = self.arrays[name]
        if kind != 'GeoLayout':
            raise Unsupported(name)
        root = Node()
        stack = [root]
        last = None
        commands = re.findall(r'\b(GEO_\w+)\(([^()]*)\)', body)
        if len(re.findall(r'\bGEO_\w+\(', body)) != len(commands):
            raise Unsupported('nested geometry command arguments')
        for op, args in commands:
            args = [a.strip() for a in args.split(',')] if args else []
            if op == 'GEO_OPEN_NODE':
                if last is None:
                    raise Unsupported('unpaired open')
                stack.append(last)
            elif op == 'GEO_CLOSE_NODE':
                if len(stack) < 2:
                    raise Unsupported('unpaired close')
                stack.pop()
            elif op in ('GEO_END', 'GEO_RETURN'):
                pass
            elif op in ('GEO_BRANCH', 'GEO_BRANCH_AND_LINK'):
                last = self.layout(args[-1], (*seen, name))
                stack[-1].children.append(last)
            else:
                last = Node(op, args)
                stack[-1].children.append(last)
        if len(stack) != 1:
            raise Unsupported('unclosed layout')
        return root

    def measure(self, root):
        radius = shadow = 0.0
        animation_scale = 1.0

        def visit(node, offset, scale):
            nonlocal radius, shadow, animation_scale
            op, args = node.op, node.args
            display = None
            if op == 'GEO_SCALE':
                scale *= abs(int(args[1], 0) / 65536)
            elif op in ('GEO_ANIMATED_PART', 'GEO_TRANSLATE_NODE', 'GEO_TRANSLATE_NODE_WITH_DL',
                        'GEO_TRANSLATE_ROTATE', 'GEO_TRANSLATE_ROTATE_WITH_DL'):
                offset += scale * math.sqrt(sum(int(c, 0)**2 for c in args[1:4]))
                if op == 'GEO_ANIMATED_PART' or op.endswith('_WITH_DL'):
                    display = args[-1]
            elif op in ('GEO_BILLBOARD', 'GEO_BILLBOARD_WITH_PARAMS'):
                if args:
                    offset += scale * math.sqrt(sum(int(c, 0)**2 for c in args[1:4]))
                # Renderer billboards reset to the object's scale, bypassing
                # ancestor GEO_SCALE nodes. Keep the translated pivot.
                scale = 1.0
            elif op == 'GEO_DISPLAY_LIST':
                display = args[-1]
            elif op == 'GEO_SHADOW':
                kind = args[0]
                factor = {'SHADOW_RECTANGLE_SPINDEL': 7.2, 'SHADOW_RECTANGLE_WHOMP': 4.0}.get(kind, 1.0)
                if kind not in ('SHADOW_CIRCLE_4_VERTS', 'SHADOW_CIRCLE_9_VERTS', 'SHADOW_SQUARE_PERMANENT',
                                'SHADOW_SQUARE_SCALABLE', 'SHADOW_SQUARE_TOGGLABLE', 'SHADOW_CIRCLE',
                                'SHADOW_RECTANGLE_SPINDEL', 'SHADOW_RECTANGLE_WHOMP'):
                    raise Unsupported(kind)
                shadow = max(shadow, abs(int(args[2], 0)) * factor)
            elif op == 'GEO_ASM':
                # Layer/alpha changes do not move geometry. Held/projectile
                # transforms and other geometry callbacks retain ordinary rendering.
                if args[-1] != 'geo_update_layer_transparency':
                    raise Unsupported(args[-1])
            elif op == 'GEO_SWITCH_CASE':
                # These selectors only choose immutable branches; measure every branch.
                if args[-1] not in ('geo_switch_anim_state', 'geo_switch_mario_palette',
                                    'geo_switch_seaweed_lod'):
                    raise Unsupported(args[-1])
            elif op not in ('GEO_NODE_START', 'GEO_CULLING_RADIUS', 'GEO_RENDER_RANGE',
                            'GEO_ROTATION_NODE'):
                raise Unsupported(op)
            animation_scale = max(animation_scale, scale)
            if display is not None:
                radius = max(radius, offset + scale * self.display(display))
            for child in node.children:
                visit(child, offset, scale)

        visit(root, 0.0, 1.0)
        return math.ceil(radius + 8), math.ceil(shadow + 8) if shadow else 0, animation_scale


def records():
    bounds = Bounds()
    scripts = read_source(ROOT / 'levels/scripts.c')
    common = re.search(r'const LevelScript script_func_global_1\[\] = \{(.*?)\n\};', scripts, re.S)[1]
    loads = list(re.findall(r'LOAD_MODEL_FROM_GEO\((\w+),\s*(\w+)\)', common))
    private = read_source(ROOT / 'levels/physics_lab/enemy_models.inc.c')
    loads += [(model, geo.removeprefix('physics_lab_enemy_')) for model, geo in
              re.findall(r'LOAD_MODEL_FROM_GEO\((\w+),\s*(\w+)\)', private)]
    loads.append(('MODEL_BOB_BUBBLY_TREE', 'bubbly_tree_geo'))
    loads += re.findall(r'LOAD_MODEL_FROM_GEO\((MODEL_LEVEL_GEOMETRY_0[5-9]),\s*(\w+)\)',
                        read_source(ROOT / 'levels/physics_lab/script.c'))
    result = []
    skipped = []
    for model, geo in loads:
        try:
            radius, shadow, scale = bounds.measure(bounds.layout(geo))
            result.append((model, geo, radius, shadow, scale))
        except (Unsupported, ValueError) as exc:
            skipped.append((model, str(exc)))
    return result, skipped


def output(rows, skipped):
    lines = ['// Generated by tools/generate_physics_lab_bounds.py; edit the generator.',
             '// Unsupported layouts retain ordinary object rendering.',
             'static const struct PhysicsLabObjectBound sLabObjectBounds[] = {']
    for model, geo, radius, shadow, scale in rows:
        # Shorten only approved layouts with safe selectors and lab fixtures.
        # This table feeds the existing model cache; no extra per-frame search.
        if model in {
            'MODEL_GOOMBA', 'MODEL_FLYGUY', 'LAB_MODEL_UKIKI',
            'LAB_MODEL_SPINY', 'LAB_MODEL_SPINY_BALL', 'LAB_MODEL_PIRANHA_PLANT',
            'LAB_MODEL_KOOPA_WITH_SHELL', 'LAB_MODEL_KOOPA_WITHOUT_SHELL',
            'LAB_MODEL_CHAIN_CHOMP',
        }:
            draw_distance = 3500
        elif model.startswith('MODEL_LEVEL_GEOMETRY_'):
            draw_distance = 6000
        else:
            continue
        lines.append(f'    {{ {model}, {draw_distance} }}, // {geo}')
    lines.append('};')
    lines += [f'// Fallback: {model} ({reason})' for model, reason in skipped]
    return '\n'.join(lines) + '\n'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    rows, skipped = records()
    text = output(rows, skipped)
    if args.check:
        if not OUT.exists() or read_source(OUT) != text:
            raise SystemExit('Stale Physics Lab object bounds')
    else:
        OUT.write_text(text)
    print(f'Physics Lab object bounds: {len(rows)} supported layouts; {len(skipped)} conservative fallbacks.')
