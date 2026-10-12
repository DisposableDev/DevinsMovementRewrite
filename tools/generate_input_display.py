#!/usr/bin/env python3
"""Bake the approved native-resolution controller and direction sprites."""
import argparse
import io
import math
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'textures/segment2'


def polygon_mask(vertices):
    result = set()
    for y in range(min(v[1] for v in vertices), max(v[1] for v in vertices) + 1):
        for x in range(min(v[0] for v in vertices), max(v[0] for v in vertices) + 1):
            inside = False
            for i, (ax, ay) in enumerate(vertices):
                bx, by = vertices[(i + 1) % len(vertices)]
                if (ay > y + .5) != (by > y + .5) and x + .5 < (bx - ax) * (y + .5 - ay) / (by - ay) + ax:
                    inside = not inside
            if inside:
                result.add((x, y))
    return result


def pixels(image, mask, color):
    draw = ImageDraw.Draw(image)
    for point in mask:
        draw.point(point, fill=color)


def circle(image, x, y, radius, color):
    draw = ImageDraw.Draw(image)
    for dy in range(-radius, radius + 1):
        half = int(math.sqrt((radius + .25) ** 2 - dy * dy))
        draw.rectangle((x - half, y + dy, x + half, y + dy), fill=color)


def button(radius, color, active, letter=None):
    image = Image.new('RGBA', (2 * radius + 1, 2 * radius + 1))
    circle(image, radius, radius, radius, '#ffffff' if active else '#333333')
    circle(image, radius, radius, radius - 1, color)
    if letter and (letter != 'S' or active):
        rows = {'S': ['111', '100', '111', '001', '111'],
                'B': ['110', '101', '110', '101', '110'],
                'A': ['010', '101', '111', '101', '101']}[letter]
        # The lettering positions reproduce the approved sprite, including B/A's slight inset.
        x = radius - (1 if letter == 'S' else 2)
        pixels(image, {(x + u, radius - 2 + v) for v, row in enumerate(rows)
                       for u, value in enumerate(row) if value == '1'},
               '#ffffff' if active else '#424242')
    return image


def direction_mask(turn, large=False):
    up = {(0, -3), (-1, -2), (0, -2), (1, -2),
          (-2, -1), (-1, -1), (0, -1), (1, -1), (2, -1),
          (0, 0), (0, 1), (0, 2), (0, 3)} if large else {
        (0, -2), (-1, -1), (0, -1), (1, -1), (0, 0), (0, 1), (0, 2)}
    for _ in range(turn):
        up = {(-v, u) for u, v in up}
    return up


def outlined(mask, color, size, origin, border_color='#000000'):
    image = Image.new('RGBA', size)
    mask = {(x + origin[0], y + origin[1]) for x, y in mask}
    border = {(x + dx, y + dy) for x, y in mask for dx, dy in [(0, 0), (1, 0), (-1, 0), (0, 1), (0, -1)]}
    pixels(image, border, border_color)
    pixels(image, mask, color)
    return image



def mini_glyph(im, cx, cy, rows, color):
    d = ImageDraw.Draw(im)
    for y, row in enumerate(rows):
        for x, bit in enumerate(row):
            if bit == '1':
                d.point((cx-len(row)//2+x, cy-len(rows)//2+y), fill=color)


def mini_rounded_five(im, cx, cy, color):
    # A symmetric five-pixel disc gives a full three-pixel span at either pole.
    mini_glyph(im, cx, cy, ['01110', '11111', '11111', '11111', '01110'], color)

MINI_A = ['010', '101', '111', '101', '101']
MINI_B = ['110', '101', '110', '101', '110']
MINI_Z = ['11111', '00010', '00100', '01000', '11111']


def mini_button(im, cx, cy, fill, label, pressed):
    circle(im, cx, cy, 3, '#ffffff' if pressed else '#333333')
    mini_rounded_five(im, cx, cy, fill)
    # A three-by-five letter needs the full inner span; retain the original
    # dark idle lettering and white held lettering.
    mini_glyph(im, cx, cy, label, '#ffffff' if pressed else '#424242')


def mini_shoulder(side, pressed):
    patch = Image.new('RGBA', (12, 5))
    colors = {'O': '#eeeeee' if pressed else '#424242',
              'F': '#898989' if pressed else '#858585'}
    # Shared native steps keep both shoulders mirrored, without isolated shades.
    for y, row in enumerate(['....OOOOOOOO', '.OOOFFFFFFFF', 'OFF.........']):
        for x, value in enumerate(row):
            if value != '.':
                pixels(patch, {(11 - x if side else x, y)}, colors[value])
    return patch


def generate_mini_base(images, housing, pressed=False):
    base = Image.new('RGBA', (50, 35))
    # Half-size sampling skips parts of the original one-pixel contour. Rebuild
    # that contour on the native mini grid, using the large controller's palette.
    mini_housing = {(x, y) for y in range(35) for x in range(50)
                    if (6 + 2 * x + 1, 160 + int((y + .5) * 69 / 35)) in housing}
    # The first two housing rows lose a pixel on the right when sampled. Match
    # the left curve so both shoulder strips meet the bevel at the same step.
    mini_housing |= {(48 - x, y) for x, y in mini_housing if y < 4}
    row_edges = {y: (min(x for x, hy in mini_housing if hy == y),
                     max(x for x, hy in mini_housing if hy == y))
                 for y in {p[1] for p in mini_housing}}
    column_edges = {x: (min(y for hx, y in mini_housing if hx == x),
                        max(y for hx, y in mini_housing if hx == x))
                    for x in {p[0] for p in mini_housing}}
    for x, y in mini_housing:
        left, right = row_edges[y]
        top, bottom = column_edges[x]
        light, shade = min(x - left, y - top), min(right - x, bottom - y)
        if any((x + dx, y + dy) not in mini_housing
               for dx, dy in [(1, 0), (-1, 0), (0, 1), (0, -1)]):
            pixels(base, {(x, y)}, '#424242')
        elif light == 1:
            pixels(base, {(x, y)}, '#cdcdcd')
        elif shade == 1:
            pixels(base, {(x, y)}, '#7f7f7f')
        else:
            face = 2 * x + 1 + 2 * (int((y + .5) * 69 / 35) - 4)
            color = '#b3b3b3' if face < 65 else '#ababab' if face < 135 else '#a4a4a4'
            pixels(base, {(x, y)}, color)
    for side, pos in enumerate([(5, 1), (32, 1)]):
        base.alpha_composite(mini_shoulder(side, False), pos)
    # Continue the housing bevel below each shoulder bend, without a dark tip.
    pixels(base, {(8, 3), (40, 3)}, '#cdcdcd')
    im = base.copy()
    d = ImageDraw.Draw(im)
    # Native mini controls: B and A retain seven-pixel legible labels.
    mini_button(im, 30, 17, '#38ff58' if pressed else '#36984e', MINI_B, pressed)
    mini_button(im, 37, 22, '#6098ff' if pressed else '#3d65b0', MINI_A, pressed)
    mini_rounded_five(im, 24, 10, '#ffffff' if pressed else '#333333')
    d.rectangle((23, 9, 25, 11), fill='#ff5050' if pressed else '#c94b4b')
    # Four distinct C directions use compact chevrons at mini scale.
    for (cx, cy), tip in zip([(38, 7), (42, 11), (38, 15), (34, 11)],
                              [(0,-1), (1,0), (0,1), (-1,0)]):
        held = pressed and tip == (0,-1)
        mini_rounded_five(im, cx, cy, '#ffffff' if held else '#333333')
        d.rectangle((cx-1, cy-1, cx+1, cy+1), fill='#ffff00' if held else '#c5b041')
        # Compact chevrons retain a visible direction at native resolution.
        arrow = [(0,-1), (-1,0), (1,0)]
        turn = [(0,-1), (1,0), (0,1), (-1,0)].index(tip)
        for _ in range(turn):
            arrow = [(-y,x) for x,y in arrow]
        for x,y in arrow:
            d.point((cx+x, cy+y), fill='#ffffff' if held else '#655924')
    # Z stays detached below the housing, as in the approved full-size layout.
    # Keep the frame distinct from the lettering so the Z cannot merge into it.
    d.rectangle((21, 27, 27, 33), fill='#898989' if pressed else '#424242')
    d.rectangle((22, 28, 26, 32), fill='#424242')
    mini_glyph(im, 24, 30, MINI_Z, '#ffffff' if pressed else '#898989')
    # Keep the gate at its original location; the neutral cap is seven pixels.
    d.polygon([(10,7),(12,7),(15,10),(15,12),(12,15),(10,15),(7,12),(7,10)],
              fill='#555555')
    d.polygon([(10,8),(12,8),(14,10),(14,12),(12,14),(10,14),(8,12),(8,10)],
              fill='#929292')
    d.polygon([(10,9),(12,9),(13,10),(13,12),(12,13),(10,13),(9,12),(9,10)],
              fill='#494949')
    # All four D-pad arms retain a symmetric one-pixel border.
    d.rectangle((15, 15, 17, 21), fill='#252525')
    d.rectangle((13, 17, 19, 19), fill='#252525')
    d.line((16, 16, 16, 20), fill='#686868')
    d.line((14, 18, 18, 18), fill='#686868')
    if pressed:
        # Match the other held controls: a continuous one-pixel white outline
        # around the arm, with its lighter gray interior kept separate.
        d.rectangle((15, 15, 17, 17), fill='#ffffff')
        d.line((16, 16, 16, 17), fill='#969696')
    if pressed:
        for side, pos in enumerate([(5, 1), (32, 1)]):
            im.alpha_composite(mini_shoulder(side, True), pos)
    return im


def generate_mini(images, housing):
    """Bake the approved mini artwork; every texture draws at one screen pixel per texel."""
    neutral = generate_mini_base(images, housing)
    held = generate_mini_base(images, housing, True)
    result = {'mini_base': neutral}
    for name, box in [('start', (22, 8, 27, 13)), ('b', (27, 14, 34, 21)),
                      ('a', (34, 19, 41, 26)), ('z', (21, 27, 28, 34)),
                      ('c_0', (36, 5, 41, 10))]:
        patch = held.crop(box)
        if name != 'z':
            mask = Image.new('RGBA', patch.size)
            if patch.width == 7:
                circle(mask, 3, 3, 3, '#ffffff')
            else:
                mini_rounded_five(mask, 2, 2, '#ffffff')
            patch.putalpha(mask.getchannel('A'))
        result['mini_' + name] = patch
    for turn in range(1, 4):
        result['mini_c_' + str(turn)] = result['mini_c_0'].rotate(-90 * turn)
    for side, name in enumerate(['shoulder_l', 'shoulder_r']):
        result['mini_' + name] = mini_shoulder(side, True)
    dpad = Image.new('RGBA', (7, 7))
    draw = ImageDraw.Draw(dpad)
    draw.rectangle((2, 0, 4, 2), fill='#ffffff')
    draw.line((3, 1, 3, 2), fill='#969696')
    for turn in range(4):
        result['mini_d_' + str(turn)] = dpad.rotate(-90 * turn)
    stick = Image.new('RGBA', (7, 7))
    circle(stick, 3, 3, 3, '#606060')
    mini_rounded_five(stick, 3, 3, '#d8d8d8')
    for pos, color in [((2, 2), '#eeeeee'), ((4, 4), '#f0f0f0'), ((3, 3), '#989898')]:
        stick.putpixel(pos, tuple(bytes.fromhex(color[1:])) + (255,))
    result['mini_stick'] = stick
    return result


def generate():
    images = {}
    base = Image.new('RGBA', (100, 69))
    outline = [(26, 164), (84, 164), (93, 167), (100, 173), (104, 182), (105, 195), (102, 205),
               (97, 212), (88, 216), (80, 216), (69, 212), (42, 212), (31, 216), (23, 216),
               (14, 212), (9, 205), (6, 195), (7, 182), (11, 173), (18, 167)]
    housing = polygon_mask(outline)
    contour = {p for p in housing if any((p[0] + dx, p[1] + dy) not in housing
                                       for dx, dy in [(1, 0), (-1, 0), (0, 1), (0, -1)])}
    row_edges = {y: (min(x for x, hy in housing if hy == y), max(x for x, hy in housing if hy == y))
                 for y in {p[1] for p in housing}}
    column_edges = {x: (min(y for hx, y in housing if hx == x), max(y for hx, y in housing if hx == x))
                    for x in {p[0] for p in housing}}
    for x, y in housing:
        left, right = row_edges[y]
        top, bottom = column_edges[x]
        light, shade = min(x - left, y - top), min(right - x, bottom - y)
        face = (x - 6) + 2 * (y - 164)
        color = '#b3b3b3' if face < 65 else '#ababab' if face < 135 else '#a4a4a4'
        if (x, y) in contour:
            color = '#424242'
        elif light == 1:
            color = '#cdcdcd'
        elif shade == 1:
            color = '#7f7f7f'
        elif light == 2:
            color = '#bfbfbf'
        elif shade == 2:
            color = '#969696'
        pixels(base, {(x - 6, y - 160)}, color)
    for side in range(2):
        for active in range(2):
            patch = Image.new('RGBA', (25, 10))
            for x in range(16, 41):
                edge = column_edges[x][0]
                for y in range(edge - (1 if x in (16, 40) else 2), edge + 2):
                    colors = ('#eeeeee', '#b6b6b6', '#898989') if active else ('#424242', '#858585', '#606060')
                    color = colors[0 if y < edge else 1 if y == edge else 2]
                    px = x if side == 0 else 110 - x
                    pixels(patch, {(px - (16 if side == 0 else 70), y - 162)}, color)
            if active:
                images['shoulder_' + ('l' if side == 0 else 'r')] = patch
            else:
                base.alpha_composite(patch, (10 if side == 0 else 64, 2))
    # Point-oriented octagonal gate, with the cap baked separately so it can move continuously.
    for radius, color in [(9, '#555555'), (8, '#929292'), (7, '#494949')]:
        vertices = [(radius * math.cos(i * math.pi / 4), radius * math.sin(i * math.pi / 4)) for i in range(8)]
        mask = set()
        for y in range(-radius, radius + 1):
            for x in range(-radius, radius + 1):
                if all((vertices[(i + 1) % 8][0] - ax) * (y - ay)
                       - (vertices[(i + 1) % 8][1] - ay) * (x - ax) >= -1e-8
                       for i, (ax, ay) in enumerate(vertices)):
                    mask.add((21 + x, 22 + y))
        pixels(base, mask, color)
    stick = button(6, '#d8d8d8', False)
    circle(stick, 6, 6, 6, '#606060')
    circle(stick, 6, 6, 5, '#d8d8d8')
    for y in range(-3, 4):
        for x in range(-3, 4):
            distance = x * x + y * y
            if distance <= 10:
                color = ('#989898' if x + y < 0 else '#f0f0f0') if distance >= 5 else '#c4c4c4'
                pixels(stick, {(6 + x, 6 + y)}, color)
    pixels(stick, {(3, 2), (4, 1), (5, 1)}, '#eeeeee')
    pixels(stick, {(6, 6)}, '#989898')
    images['stick'] = stick
    for name, x, y, radius, neutral, active in [('start', 54, 181, 5, '#c94b4b', '#ff5050'),
                                               ('b', 67, 195, 6, '#36984e', '#38ff58'),
                                               ('a', 80, 204, 6, '#3d65b0', '#6098ff')]:
        letter = 'S' if name == 'start' else name.upper()
        base.alpha_composite(button(radius, neutral, False, letter), (x - radius - 6, y - radius - 160))
        images[name] = button(radius, active, True, letter)
    for turn, (x, y) in enumerate([(82, 175), (90, 183), (82, 191), (74, 183)]):
        for active in range(2):
            patch = Image.new('RGBA', (9, 9))
            draw = ImageDraw.Draw(patch)
            for dy, width in zip(range(-4, 5), [3, 5, 7, 9, 9, 9, 7, 5, 3]):
                draw.rectangle((4 - width // 2, 4 + dy, 4 + width // 2, 4 + dy), fill='#ffffff' if active else '#333333')
            for dy, width in zip(range(-3, 4), [3, 5, 7, 7, 7, 5, 3]):
                draw.rectangle((4 - width // 2, 4 + dy, 4 + width // 2, 4 + dy), fill='#ffff00' if active else '#c5b041')
            pixels(patch, {(4 + u, 4 + v) for u, v in direction_mask(turn)}, '#655924')
            if active:
                images['c_' + str(turn)] = patch
            else:
                base.alpha_composite(patch, (x - 10, y - 164))
        images['direction_' + str(turn)] = outlined(
            direction_mask(turn, True), '#d0d0d0', (9, 9), (4, 4), '#484848')
        images['c_direction_' + str(turn)] = outlined(
            direction_mask(turn, True), '#ffff00', (9, 9), (4, 4), '#484848')
    dpad = polygon_mask([(37, 190), (42, 190), (42, 195), (47, 195), (47, 200), (42, 200),
                         (42, 205), (37, 205), (37, 200), (32, 200), (32, 195), (37, 195)])
    inner = polygon_mask([(38, 191), (41, 191), (41, 196), (46, 196), (46, 199), (41, 199),
                          (41, 204), (38, 204), (38, 199), (33, 199), (33, 196), (38, 196)])
    for x, y in dpad:
        pixels(base, {(x - 6, y - 160)}, '#686868' if (x, y) in inner else '#252525')
    for turn in range(4):
        patch = Image.new('RGBA', (16, 16))
        for x, y in dpad:
            active = (37 <= x <= 41 and y <= 199, y >= 195 and y <= 199 and x >= 38,
                      37 <= x <= 41 and y >= 195, y >= 195 and y <= 199 and x <= 41)[turn]
            if active:
                pixels(patch, {(x - 32, y - 190)}, '#969696' if (x, y) in inner else '#ffffff')
        images['d_' + str(turn)] = patch
    for active in range(2):
        z = Image.new('RGBA', (13, 13), '#ffffff' if active else '#424242')
        ImageDraw.Draw(z).rectangle((1, 1, 11, 11), fill='#898989')
        pixels(z, {(4 + u, 4 + v) for v, row in enumerate(['11111', '00010', '00100', '01000', '11111'])
                   for u, value in enumerate(row) if value == '1'}, '#ffffff' if active else '#424242')
        if active:
            images['z'] = z
        else:
            base.alpha_composite(z, (43, 54))
    # Exact pixel steps keep sharp arrowheads and three-pixel shafts consistent in all directions.
    diagonal_rows = [
        '................',
        '................',
        '................',
        '......######....',
        '.......#####....',
        '........####....',
        '.......#####....',
        '......###.##....',
        '.....###...#....',
        '....###.........',
        '...###..........',
        '....#...........',
        '................',
        '................',
        '................',
        '................',
    ]
    diagonal = {(x, y) for y, row in enumerate(diagonal_rows)
                for x, value in enumerate(row) if value == '#'}
    cardinal = {(x, y) for x in range(3, 8) for y in range(7, 10)}
    cardinal |= {(x, y) for x in range(8, 13) for y in range(4, 13)
                 if abs(y - 8) <= 12 - x}
    for sector in range(8):
        mask = (diagonal if sector % 2 else cardinal).copy()
        for _ in range(sector // 2):
            mask = {(y, 15 - x) for x, y in mask}
        images['arrow_' + str(sector)] = outlined(mask, '#ffffff', (16, 16), (0, 0))
    # Keep this cleanup local to the overlay; other PuppyPrint text keeps its original font.
    with Image.open(OUTPUT / 'custom_text2.ia4.preswap.png') as font:
        label_a = font.convert('RGBA').crop((224, 0, 232, 12))
    # Frame counters alone use black outlines; leave the shared readout font unchanged.
    with Image.open(OUTPUT / 'custom_text2.ia4.preswap.png') as font:
        for digit in range(10):
            glyph = font.convert('RGBA').crop((120 + digit * 8, 0, 128 + digit * 8, 12))
            glyph.putdata([(0, 0, 0, a) if r == 85 else (r, g, b, a)
                           for r, g, b, a in glyph.getdata()])
            images['count_' + str(digit)] = glyph
    label_a.putpixel((7, 2), (0, 0, 0, 0))
    images['label_a'] = label_a
    images['neutral'] = outlined({(6, 6), (7, 6), (6, 7), (7, 7)}, '#ffffff', (16, 16), (0, 0))
    # Outline-font gray is intensity 2 in IA4: 2 * 36 = 72 before RGBA16 rounding.
    images['degree'] = outlined(
        {(2, 0), (3, 1), (2, 2), (1, 1)}, '#ffffff', (5, 5), (0, 1), '#484848')
    images['base'] = base
    vertical = Image.new('RGBA', (3, 3), '#000000')
    ImageDraw.Draw(vertical).line((1, 0, 1, 2), fill='#ffffff')
    images['bracket_vertical'] = vertical
    cap = Image.new('RGBA', (6, 3), '#000000')
    ImageDraw.Draw(cap).line((1, 1, 4, 1), fill='#ffffff')
    images['bracket_cap'] = cap
    images.update(generate_mini(images, housing))
    return images


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    errors = []
    declarations = ['/* Generated by tools/generate_input_display.py. */']
    for name, image in generate().items():
        symbol = ''.join(part.capitalize() for part in name.split('_'))
        declarations.extend([
            'ALIGNED8 static const Texture sInputDisplayTexture' + symbol + '[] = {',
            '#include \"textures/segment2/input_display_' + name + '.rgba16.inc.c\"',
            '};',
            'static const struct InputDisplaySprite sInputDisplay' + symbol + ' = {',
            '    sInputDisplayTexture' + symbol + ',',
            '    ' + str(image.width) + ',',
            '    ' + str(image.height) + ',',
            '};',
        ])
        path = OUTPUT / ('input_display_' + name + '.rgba16.png')
        data = io.BytesIO()
        image.save(data, format='PNG')
        if args.check:
            if not path.exists() or path.read_bytes() != data.getvalue():
                errors.append(str(path.relative_to(ROOT)))
        else:
            path.write_bytes(data.getvalue())
    metadata = ROOT / 'src/game/input_display_assets.inc.c'
    source = '\n'.join(declarations) + '\n'
    if args.check:
        if not metadata.exists() or metadata.read_text() != source:
            errors.append(str(metadata.relative_to(ROOT)))
    else:
        metadata.write_text(source)
    if errors:
        raise SystemExit('Outdated input display assets: ' + ', '.join(errors))
    print('Input display assets match.' if args.check else 'Input display assets generated.')


if __name__ == '__main__':
    main()
