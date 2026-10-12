# HE THICC menu portrait sources

Source: Render96 HD Texture Pack's file-select portrait (256 x 128).

- Project: https://github.com/pokeheadroom/RENDER96-HD-TEXTURE-PACK
- Download: https://raw.githubusercontent.com/pokeheadroom/RENDER96-HD-TEXTURE-PACK/master/gfx/levels/menu/main_menu_seg7.01018.rgba16.png
- Retrieved: 2026-09-30
- Original downloaded file: `render96_face_source.png`
- SHA-256: `752a2578482e628bfbf617ca64e1d588226e23a2a9e97d6074f84f8bab0ce9a7`
- Credits: the Render96 HD Texture Pack contributors; original Mario artwork and character by Nintendo. Render96's project description permits using its textures as a base with credit.

This is a higher-resolution reconstruction of the same file-select portrait,
not an authenticated pristine Nintendo render. Render96's source research identifies
the Nintendo June 1996 website render as the original artwork and reports the
full clean original-quality render as lost:
https://github.com/Render96/Render96Wiki/wiki/Nintendo-Asset-Libraries-(SM64)#nintendocojp---nintendo-of-japans-website-circa-june-1996

Run `python3 tools/generate_he_thicc_menu_face.py` from the repository root
(requires Pillow). It reads this source and the current Mario model palette and
pink T artwork, preserves rendered skin shading, colours the hair and cap,
adds the badge and fuller cheeks, and writes:

- `he_thicc_face_master.png`: edited 256 x 128 working portrait.
- `levels/menu/menu_he_thicc_save.rgba16.png`: 64 x 32 game texture, downsampled once with Lanczos.

The source is kept locally so regeneration does not depend on network access.
The generator replaces the earlier method that recoloured the 64 x 32 original.
