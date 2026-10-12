"""Generate the secret pool, preserving the native inset platform's materials."""
from pathlib import Path
import math
import re

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
AREA = ROOT / 'levels/castle_inside/areas/4'
CENTER_X, CENTER_Z = -1023, 717
SCALE = 4
LEVEL_SCALE = 4
BOUNDARY_TOP = 19900


def scaled_position(x, y, z, scale=SCALE):
    return (round(CENTER_X + (x - CENTER_X) * scale), round(y * scale),
            round(CENTER_Z + (z - CENTER_Z) * scale))

source = read_source(ROOT / 'levels/castle_inside/areas/1/2/model.inc.c')
arrays = {}
for name, body in re.findall(r'static const Vtx (\w+)\[\] = \{(.*?)\n\};', source, re.S):
    arrays[name] = [tuple(int(n, 0) for n in re.findall(r'-?0x[\da-fA-F]+|-?\d+', line))
                    for line in body.splitlines() if '{{{' in line]
    # Strip the Vtx flag; retain position, UVs and the native material colors.
    arrays[name] = [v[:3] + v[4:] for v in arrays[name]]


def inside(v):
    return -1535 <= v[0] <= -511 and -50 <= v[1] <= 0 and 205 <= v[2] <= 1229


def clip_platform(polygon):
    """Clip every height/material to the top platform's octagonal footprint."""
    planes = [(1, 0, 512, 2.04), (-1, 0, 512, 2.04), (0, 1, 512, 2.04), (0, -1, 512, 2.04),
              (1, 1, 717, 2.74), (1, -1, 717, 2.74), (-1, 1, 717, 2.74), (-1, -1, 717, 2.74)]
    for nx, nz, limit, slope in planes:
        clipped = []
        for a, b in zip(polygon, polygon[1:] + polygon[:1]):
            da = limit + slope * a[1] - nx * (a[0] - CENTER_X) - nz * (a[2] - CENTER_Z)
            db = limit + slope * b[1] - nx * (b[0] - CENTER_X) - nz * (b[2] - CENTER_Z)
            if da >= 0:
                clipped.append(a)
            if (da < 0) != (db < 0):
                fraction = da / (da - db)
                clipped.append(tuple(x + fraction * (y - x) for x, y in zip(a, b)))
        polygon = clipped
    return [tuple(round(n) for n in v) for v in polygon]


commands, triangles, model_vertices = [], [], []
for name, body in re.findall(r'static const Gfx (\w+)\[\] = \{(.*?)\n\};', source, re.S):
    temporary, cache = [], {}
    for line in body.splitlines():
        load = re.search(r'gsSPVertex\((\w+),\s*(\d+),\s*(\d+)\)', line)
        if load:
            array, count, base = load.groups()
            cache = {int(base) + i: v for i, v in enumerate(arrays[array][:int(count)])}
        elif 'gsSP1Triangle' in line or 'gsSP2Triangles' in line:
            numbers = [int(n, 0) for n in re.search(r'\((.*?)\)', line)[1].split(',')]
            faces = [numbers[:3]] + ([numbers[4:7]] if len(numbers) > 4 else [])
            for indices in faces:
                native = [cache[i] for i in indices]
                if not all(inside(v) for v in native):
                    continue
                polygon = clip_platform(native)
                for i in range(1, len(polygon) - 1):
                    face = [polygon[0], polygon[i], polygon[i + 1]]
                    # Clipping exactly on an existing corner can duplicate a vertex.
                    a, b, c = [v[:3] for v in face]
                    u, w = [b[j] - a[j] for j in range(3)], [c[j] - a[j] for j in range(3)]
                    cross = (u[1]*w[2]-u[2]*w[1], u[2]*w[0]-u[0]*w[2], u[0]*w[1]-u[1]*w[0])
                    if not any(cross):
                        continue
                    offset = len(model_vertices)
                    model_vertices.extend(face)
                    triangles.append([v[:3] for v in face])
                    temporary += [f'    gsSPVertex(castle_secret_platform_vertices + {offset}, 3, 0),',
                                  '    gsSP1Triangle(0, 1, 2, 0),']
        elif 'gsSPEndDisplayList' not in line:
            temporary.append(line)
    if any('gsSP1Triangle' in line for line in temporary):
        commands.extend(temporary)
assert len(triangles) > 10
model_vertices = [scaled_position(*v[:3]) + v[3:] for v in model_vertices]
vertex_lines = [f'    {{{{{{{x}, {y}, {z}}}, 0, {{{s}, {t}}}, {{{r}, {g}, {b}, {a}}}}}}},'
                for x, y, z, s, t, r, g, b, a in model_vertices]
model = ['// Native inset platform clipped to its octagonal rim.',
         'static const Vtx castle_secret_platform_vertices[] = {', *vertex_lines, '};',
         'const Gfx castle_secret_lobby_platform_dl[] = {',
         '    gsDPPipeSync(),', '    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),',
         '    gsSPClearGeometryMode(G_SHADING_SMOOTH),',
         '    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),',
         '    gsDPTileSync(),',
         '    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP, 5, 0, G_TX_WRAP, 5, 0),',
         '    gsDPSetTileSize(0, 0, 0, 124, 124),',
         '    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),', *commands,
         '    gsSPTexture(0, 0, 0, 0, G_OFF),', '    gsDPPipeSync(),',
         '    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),',
         '    gsSPSetGeometryMode(G_SHADING_SMOOTH),', '    gsSPEndDisplayList(),', '};']
(AREA / 'platform.inc.c').write_text('\n'.join(model) + '\n')


def ring_at(radius):
    return [(round(CENTER_X + radius * math.cos(math.pi/8 + i*math.pi/4)),
             round(CENTER_Z + radius * math.sin(math.pi/8 + i*math.pi/4))) for i in range(8)]


ring, outer_ring = ring_at(2400), ring_at(2850)
vertices, faces = [], []


def add(vertex):
    if vertex not in vertices:
        vertices.append(vertex)
    return vertices.index(vertex)


for triangle in triangles:
    faces.append(tuple(add(scaled_position(*v)) for v in triangle))
for i, (x, z) in enumerate(ring):
    nx, nz = ring[(i + 1) % 8]
    ox, oz = outer_ring[i]
    onx, onz = outer_ring[(i + 1) % 8]
    # Tall containment walls; upward-facing bottom allows re-entry from below.
    low, high = -1400, BOUNDARY_TOP / LEVEL_SCALE
    inner = [add(v) for v in map(lambda p: scaled_position(*p, LEVEL_SCALE), [(x, low, z), (nx, low, nz), (nx, high, nz), (x, high, z)])]
    outer = [add(v) for v in map(lambda p: scaled_position(*p, LEVEL_SCALE), [(ox, low, oz), (onx, low, onz), (onx, high, onz), (ox, high, oz)])]
    a, b, c, d = inner
    e, f, g, h = outer
    faces += [(a, b, c), (a, c, d), (e, g, f), (e, h, g),
              (d, g, h), (d, c, g), (a, f, e), (a, b, f)]
    faces.append((add(scaled_position(CENTER_X, -700, CENTER_Z, LEVEL_SCALE)), add(scaled_position(nx, -700, nz, LEVEL_SCALE)), add(scaled_position(x, -700, z, LEVEL_SCALE))))
# An invisible ceiling closes the top so debug fly cannot go over the walls.
ceiling_faces = []
for i, (x, z) in enumerate(ring):
    nx, nz = ring[(i + 1) % 8]
    ceiling_faces.append((add((CENTER_X, BOUNDARY_TOP, CENTER_Z)),
                          add(scaled_position(x, BOUNDARY_TOP / LEVEL_SCALE, z, LEVEL_SCALE)),
                          add(scaled_position(nx, BOUNDARY_TOP / LEVEL_SCALE, nz, LEVEL_SCALE))))
# Match the visible platform rim, with thick walls extending away from the dry floor.
platform_ring = [(512, 205), (205, 512), (-205, 512), (-512, 205),
                 (-512, -205), (-205, -512), (205, -512), (512, -205)]
platform_boundary_faces = []
for i, (dx, dz) in enumerate(platform_ring):
    ndx, ndz = platform_ring[(i + 1) % 8]
    inner = [(CENTER_X + dx * SCALE, CENTER_Z + dz * SCALE),
             (CENTER_X + ndx * SCALE, CENTER_Z + ndz * SCALE)]
    outer_scale = (512 * SCALE + 1800) / (512 * SCALE)
    outer = [(round(CENTER_X + x * SCALE * outer_scale),
              round(CENTER_Z + z * SCALE * outer_scale))
             for x, z in ((dx, dz), (ndx, ndz))]
    a, b, c, d = [add((x, y, z)) for (x, z), y in
                  ((inner[0], -5600), (inner[1], -5600),
                   (inner[1], BOUNDARY_TOP), (inner[0], BOUNDARY_TOP))]
    e, f, g, h = [add((x, y, z)) for (x, z), y in
                  ((outer[0], -5600), (outer[1], -5600),
                   (outer[1], BOUNDARY_TOP), (outer[0], BOUNDARY_TOP))]
    platform_boundary_faces += [(a, b, c), (a, c, d), (e, g, f), (e, h, g),
                                (d, g, h), (d, c, g), (a, f, e), (a, b, f)]
collision = ['const Collision castle_secret_lobby_collision[] = {', '    COL_INIT(),',
             f'    COL_VERTEX_INIT({len(vertices)}),']
collision += [f'    COL_VERTEX({x}, {y}, {z}),' for x, y, z in vertices]
platform_count = len(triangles)
collision += [f'    COL_TRI_INIT(SURFACE_DEFAULT, {platform_count}),']
collision += [f'    COL_TRI({a}, {b}, {c}),' for a, b, c in faces[:platform_count]]
for sector in range(8):
    start = platform_count + sector * 9
    collision += ['    COL_TRI_INIT(SURFACE_NON_INTERACTIVE_BOUNDARY, 4),']
    collision += [f'    COL_TRI({a}, {b}, {c}),' for a, b, c in faces[start:start + 4]]
    collision += ['    COL_TRI_INIT(SURFACE_DEFAULT, 5),']
    collision += [f'    COL_TRI({a}, {b}, {c}),' for a, b, c in faces[start + 4:start + 9]]
collision += ['    COL_TRI_INIT(SURFACE_NON_INTERACTIVE_BOUNDARY, 8),']
collision += [f'    COL_TRI({a}, {b}, {c}),' for a, b, c in ceiling_faces]
for sector in range(8):
    start = sector * 8
    collision += ['    COL_TRI_INIT(SURFACE_NON_INTERACTIVE_BOUNDARY, 4),']
    collision += [f'    COL_TRI({a}, {b}, {c}),'
                  for a, b, c in platform_boundary_faces[start:start + 4]]
    collision += ['    COL_TRI_INIT(SURFACE_DEFAULT, 4),']
    collision += [f'    COL_TRI({a}, {b}, {c}),'
                  for a, b, c in platform_boundary_faces[start + 4:start + 8]]
collision += ['    COL_TRI_STOP(),', '    COL_WATER_BOX_INIT(1),',
              f'    COL_WATER_BOX(0, {CENTER_X + (min(x for x,z in ring)-CENTER_X)*LEVEL_SCALE}, {CENTER_Z + (min(z for x,z in ring)-CENTER_Z)*LEVEL_SCALE}, {CENTER_X + (max(x for x,z in ring)-CENTER_X)*LEVEL_SCALE}, {CENTER_Z + (max(z for x,z in ring)-CENTER_Z)*LEVEL_SCALE}, {-105*SCALE}),',
              '    COL_END(),', '};']
(AREA / 'collision.inc.c').write_text('\n'.join(collision) + '\n')

# Only the textured floor is visible; the boundary walls are collision-only.
floor_points = [scaled_position(CENTER_X, -700, CENTER_Z, LEVEL_SCALE)] + [scaled_position(x, -700, z, LEVEL_SCALE) for x, z in ring]
floor_vertices = []
for i in range(8):
    triangle = [floor_points[0], floor_points[(i + 1) % 8 + 1], floor_points[i + 1]]
    uvs = [((x - CENTER_X) * 4, (z - CENTER_Z) * 4) for x, y, z in triangle]
    # Shift a whole triangle by complete texture repeats, preserving density and seams.
    shifts = [round((min(uv[axis] for uv in uvs) + max(uv[axis] for uv in uvs)) / 2048) * 1024
              for axis in range(2)]
    for (x, y, z), (s, t) in zip(triangle, uvs):
        s -= shifts[0]
        t -= shifts[1]
        assert -32768 <= s <= 32767 and -32768 <= t <= 32767
        floor_vertices.append('    {{{' + f'{x}, {y}, {z}' + '}, 0, {'
                              + f'{s}, {t}' + '}, {180, 190, 200, 255}}},')
floor_commands = [
    '    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),',
    '    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),',
    '    gsDPTileSync(),',
    '    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP, 5, 0, G_TX_WRAP, 5, 0),',
    '    gsDPSetTileSize(0, 0, 0, 124, 124),',
    '    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),',
    '    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_secret_lobby_ground_texture),',
    '    gsDPLoadSync(),',
    '    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),',
    '    gsSPVertex(castle_secret_lobby_floor_vertices, 24, 0),']
floor_commands += [f'    gsSP1Triangle({i * 3}, {i * 3 + 1}, {i * 3 + 2}, 0),' for i in range(8)]
floor_commands += ['    gsSPTexture(0, 0, 0, 0, G_OFF),', '    gsDPPipeSync(),',
                   '    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),']
(AREA / 'pool.inc.c').write_text(
    'ALIGNED8 static const Texture castle_secret_lobby_ground_texture[] = {\n'
    '#include "textures/water/jrb_textures.02800.rgba16.inc.c"\n};\n'
    'static const Vtx castle_secret_lobby_floor_vertices[] = {\n' + '\n'.join(floor_vertices)
    + '\n};\nconst Gfx castle_secret_lobby_pool_dl[] = {\n    gsDPPipeSync(),\n    gsSPClearGeometryMode(G_LIGHTING),\n    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),\n'
    + '\n'.join(floor_commands)
    + '\n    gsSPSetGeometryMode(G_LIGHTING),\n    gsSPEndDisplayList(),\n};\n')

# Rejection visits retain the pool geometry and collision, but draw it solid black.
black_floor = [
    'const Gfx castle_secret_lobby_black_pool_dl[] = {',
    '    gsDPPipeSync(),',
    '    gsSPTexture(0, 0, 0, 0, G_OFF),',
    '    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),',
    '    gsDPSetPrimColor(0, 0, 0, 0, 0, 255),',
    '    gsSPVertex(castle_secret_lobby_floor_vertices, 24, 0),',
]
black_floor += [f'    gsSP1Triangle({i * 3}, {i * 3 + 1}, {i * 3 + 2}, 0),'
                for i in range(8)]
black_floor += ['    gsDPPipeSync(),',
                '    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),',
                '    gsSPEndDisplayList(),', '};']
with (AREA / 'pool.inc.c').open('a') as output:
    output.write('\n' + '\n'.join(black_floor) + '\n')

# One shared UV mesh replaces independently rotating triangular water patches.
water_points = [scaled_position(CENTER_X, -105, CENTER_Z)] + [scaled_position(x, -105 * SCALE / LEVEL_SCALE, z, LEVEL_SCALE) for x, z in ring]
water = ['Movtex castle_secret_lobby_water_vertices[] = {', '    MOV_TEX_SPD(6),']
water += [f'    MOV_TEX_TRIS({x}, {y}, {z}, {round((x-CENTER_X)/256)}, {round((z-CENTER_Z)/256)}),'
          for x, y, z in water_points]
water += ['    MOV_TEX_END(),', '};', 'Gfx castle_secret_lobby_water_triangles[] = {']
water += [f'    gsSP1Triangle(0, {(i+1)%8+1}, {i+1}, 0),' for i in range(8)]
water += ['    gsSPEndDisplayList(),', '};']
# JRB mist reaches the raised rim's height, with the entire platform cut out.
platform_ring = [(512,205), (205,512), (-205,512), (-512,205),
                 (-512,-205), (-205,-512), (205,-512), (512,-205)]
mist_points = [scaled_position(x, 0, z, LEVEL_SCALE) for x, z in ring]
mist_points += [scaled_position(CENTER_X+x, 0, CENTER_Z+z) for x, z in platform_ring]
base_x, _, base_z = mist_points[0]
water += ['Movtex castle_secret_lobby_mist_vertices[] = {', '    MOV_TEX_SPD(1),']
water += [f'    MOV_TEX_TRIS({x}, {y}, {z}, {round((x-base_x)/512)}, {round((z-base_z)/512)}),'
          for x, y, z in mist_points]
water += ['    MOV_TEX_END(),', '};', 'Gfx castle_secret_lobby_mist_triangles[] = {']
for i in range(8):
    j = (i + 1) % 8
    water.append(f'    gsSP2Triangles({i}, {i+8}, {j}, 0, {j}, {i+8}, {j+8}, 0),')
water += ['    gsSPEndDisplayList(),', '};']
(AREA / 'movtext.inc.c').write_text('\n'.join(water) + '\n')
print(f'Clipped platform: {len(triangles)} triangles; closed boundary: 64 triangles; scale: {SCALE}')

# Reuse the castle's IA light texture for two crossed, softly fading shafts.
# Skybox pixels span 31 texels per 45-degree tile; its fixed 90-degree projection
# places the sun at yaw 0xA630 and elevation 63.87 degrees.
header = read_source(ROOT / 'src/game/level_update.h')
sun_yaw = int(re.search(r'#define CASTLE_SECRET_LOBBY_SUN_YAW (0x[0-9A-F]+)', header)[1], 16)
yaw = sun_yaw * math.tau / 65536
pitch = math.radians(((8 - 58 / 31 + 1) * 120 - 600) / 4)
direction = (math.sin(yaw) * math.cos(pitch), math.sin(pitch), math.cos(yaw) * math.cos(pitch))
sides = [(math.cos(yaw), 0, -math.sin(yaw)),
         (-math.sin(yaw) * math.sin(pitch), math.cos(pitch), -math.cos(yaw) * math.sin(pitch))]
beam_vertices, beam_faces = [], []
for side in sides:
    base = len(beam_vertices)
    for row, (distance, width, alpha) in enumerate([(0, 220, 190), (800, 300, 160),
                                                   (3200, 420, 100), (8000, 600, 0)]):
        for column, (offset, t) in enumerate([(-width, 0), (0, 990), (width, 0)]):
            point = [round(origin + direction[i] * distance * (SCALE / 2)
                           + side[i] * offset * (SCALE / 2))
                     for i, origin in enumerate((CENTER_X, -45 * SCALE, CENTER_Z))]
            x, y, z = point
            beam_vertices.append(f'    {{{{{{{x}, {y}, {z}}}, 0, {{0, {t}}}, {{255, 245, 205, {alpha}}}}}}},')
        if row:
            for column in range(2):
                a = base + (row-1)*3 + column
                beam_faces.append(f'    gsSP2Triangles({a}, {a+1}, {a+4}, 0, {a}, {a+4}, {a+3}, 0),')
(AREA / 'sunbeam.inc.c').write_text(
    'static const Vtx castle_secret_lobby_sunbeam_vertices[] = {\n' + '\n'.join(beam_vertices)
    + '\n};\nconst Gfx castle_secret_lobby_sunbeam_dl[] = {\n'
    '    gsDPPipeSync(),\n'
    '    gsDPSetCycleType(G_CYC_2CYCLE),\n'
    '    gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_XLU_SURF2),\n'
    '    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, SHADE, 0, 0, 0, 0, COMBINED, COMBINED, 0,\n'
    '                       ENVIRONMENT, 0),\n'
    '    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),\n'
    '    gsSPSetGeometryMode(G_SHADING_SMOOTH),\n'
    '    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 0, 0, G_TX_CLAMP, 0, 0),\n'
    '    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),\n'
    '    gsDPTileSync(),\n'
    '    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 5,\n'
    '                0),\n'
    '    gsDPSetTileSize(0, 0, 0, 124, 124),\n'
    '    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, texture_castle_light),\n'
    '    gsDPLoadSync(),\n'
    '    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),\n'
    '    gsSPVertex(castle_secret_lobby_sunbeam_vertices, 24, 0),\n'
    + '\n'.join(beam_faces) + '\n'
    +
    '    gsSPTexture(0, 0, 0, 0, G_OFF),\n'
    '    gsDPPipeSync(),\n'
    '    gsDPSetCycleType(G_CYC_1CYCLE),\n'
    '    gsDPSetEnvColor(255, 255, 255, 255),\n'
    '    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),\n'
    '    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),\n'
    '    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),\n'
    '    gsSPEndDisplayList(),\n};\n')
