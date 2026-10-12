#!/usr/bin/env python3
"""Edit the Render96 HD portrait, then downsample the HE THICC menu texture."""
import colorsys
import math
import re
from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[1]
ART = ROOT / 'tools/assets/he_thicc_menu_face'
SOURCE = ART / 'render96_face_source.png'
MASTER = ART / 'he_thicc_face_master.png'
OUTPUT = ROOT / 'levels/menu/menu_he_thicc_save.rgba16.png'
BADGE = ROOT / 'actors/mario/mario_he_thicc_logo.rgba16.png'


def palette_colour(name, he_thicc=True):
    branches = (ROOT / 'actors/mario/palette.h').read_text().split('#else', 1)
    variant = branches[0 if he_thicc else 1]
    match = re.search(r'#define MARIO_PALETTE_' + name + r' 0x([0-9a-fA-F]{8})', variant)
    if match is None:
        raise ValueError('Missing Mario palette colour: ' + name)
    return tuple(bytes.fromhex(match[1])[:3])


def replace_badge(themed):
    # Rebuild the existing rounded badge at HD resolution, including its
    # perspective, soft contour and light-to-shadow gradient. This removes the
    # M completely without leaving coloured edge fragments under the T.
    mask = Image.new('L', themed.size, 0)
    ImageDraw.Draw(mask).ellipse((103, 14, 152, 42), fill=255)
    ImageDraw.Draw(mask).rectangle((0, 34, themed.width, themed.height), fill=0)
    mask = mask.filter(ImageFilter.GaussianBlur(0.6))
    background = themed.copy()
    for y in range(12, 36):
        for x in range(100, 155):
            brightness = round(235 - 0.5 * (y - 14))
            background.putpixel((x, y), (brightness, brightness, brightness, 255))
    themed.paste(background, (0, 0), mask)
    # Extract the actual model's pink T, leaving its surrounding white artwork
    # transparent so the portrait's original badge lighting stays visible.
    glyph = Image.open(BADGE).convert('RGBA')
    for y in range(glyph.height):
        for x in range(glyph.width):
            r, g, b, a = glyph.getpixel((x, y))
            if r - g <= 1 or b - g <= 1:
                a = 0
            glyph.putpixel((x, y), (r, g, b, a))
    glyph = glyph.crop(glyph.getchannel('A').getbbox()).resize((30, 15), Image.Resampling.LANCZOS)
    # Shade the letter with the badge rather than pasting a bright flat stamp.
    for y in range(glyph.height):
        for x in range(glyph.width):
            r, g, b, a = glyph.getpixel((x, y))
            light = (234 - 0.3 * y) / 255
            glyph.putpixel((x, y), (round(r * light), round(g * light), round(b * light), a))
    themed.alpha_composite(glyph, (113, 18))


def generate():
    original = Image.open(SOURCE).convert('RGBA')
    # The game's save button is opaque; keep the source RGB while normalizing
    # the HD pack's incidental alpha values for the native texture slot.
    original.putalpha(255)
    if original.size != (256, 128):
        raise ValueError('Expected the credited 256x128 Render96 file-select portrait')
    themed = original.copy()
    skin = palette_colour('SKIN_LIGHT')
    standard_skin = palette_colour('SKIN_LIGHT', False)
    hair_light = palette_colour('HAIR_LIGHT')
    for y in range(original.height):
        for x in range(original.width):
            nx, ny = x / 4, y / 4
            r, g, b, a = original.getpixel((x, y))
            source_rgb = (r, g, b)
            h, sat, value = colorsys.rgb_to_hsv(r / 255, g / 255, b / 255)
            cap = ny < 17 or ((nx < 20 or nx > 44) and ny < 23)
            if 10 <= nx <= 53 and cap and sat > 0.10 and (h < 0.055 or h > 0.94):
                rgb = colorsys.hsv_to_rgb(0.523, sat * 0.73, value * 0.78)
                r, g, b = (round(c * 255) for c in rgb)
            elif 10 <= nx <= 53 and ny >= 11 and 0.035 <= h <= 0.20 and sat > 0.10 and value > 0.35:
                # Multiplicative material tint preserves the HD render's
                # gradients and volume, unlike the old low-resolution repaint.
                r, g, b = (round(c * target / base)
                           for c, target, base in zip(source_rgb, skin, standard_skin))
            hair = ((16 <= nx <= 22 or 43 <= nx <= 48) and 11 <= ny <= 27)
            moustache = 20 <= nx <= 43 and 19 <= ny <= 24
            eyebrows = (22 <= nx <= 29 or 34 <= nx <= 41) and 11 <= ny <= 14
            if (hair or moustache or eyebrows) and max(source_rgb) < 130:
                # Preserve contrast in the rendered hair instead of lifting
                # every near-black source pixel to the same flat purple fill.
                shade = 0.20 + 0.80 * min(1.0, max(source_rgb) / 130) ** 0.65
                r, g, b = (round(light * shade) for light in hair_light)
            themed.putpixel((x, y), (r, g, b, a))
    replace_badge(themed)
    output = themed.copy()
    # Apply the accepted fuller cheeks at working resolution as well.
    for y in range(18 * 4, 31 * 4):
        width = 1.0 + 0.10 * math.sin(math.pi * (y / 4 - 17) / 14)
        for x in range(9 * 4, 55 * 4):
            source_x = 127.5 + (x - 127.5) / width
            left = int(source_x)
            weight = source_x - left
            c0, c1 = themed.getpixel((left, y)), themed.getpixel((left + 1, y))
            output.putpixel((x, y), tuple(round(v0 * (1 - weight) + v1 * weight)
                                         for v0, v1 in zip(c0, c1)))
    output.save(MASTER)
    output.resize((64, 32), Image.Resampling.LANCZOS).save(OUTPUT)
    print(MASTER)
    print(OUTPUT)


if __name__ == '__main__':
    generate()
