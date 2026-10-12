# Custom asset sources

This register identifies the creators, original sources and adaptations of the
hack's assets. Nintendo's Super Mario 64 and inherited HackerSM64 assets form
the base; imported assets and project additions are identified below. Contributor
credits are also retained in [credits.md](../credits.md).

Eligible original project assets and authorized artistic contributions are
licensed **CC BY-SA 4.0** under the [project licensing declaration](../licence.md).
Imported and Nintendo-derived material retains its underlying rights and terms;
the custom-asset label alone does not place an entire mixed asset under this
licence. The provenance entries below identify those exceptions and credits.

## Audio clips

The game uses the supplied WAV crops from the following videos. Source links,
timestamps and speaker attributions were recorded on 2026-10-07; video metadata
was checked on that date.

| Source video | Uploader | Upload date |
| --- | --- | --- |
| [Yooka Laylee (dunkview)](https://www.youtube.com/watch?v=GmUiVXW4R5E) | videogamedunkey | 2017-04-11 |
| [ChatGPT helps make a Troll Hack](https://www.youtube.com/watch?v=CgIsvX92k0A) | SimpleFlips | 2023-10-11 |
| [I lost the run to RNG, dude!](https://www.youtube.com/watch?v=ST42VjR0LYk) | Flickerform | 2017-05-13 |

MrSpeedrun is the owner-identified speaker of the RNG quote; **Flickerform** is
its YouTube uploader. The upload date describes this linked video, not the date
of the original stream or recording.

| Clip and credited speaker | Source moment | Use in the hack |
| --- | --- | --- |
| Dunkey pause quote — videogamedunkey | [4:38 in the source video](https://www.youtube.com/watch?v=GmUiVXW4R5E&t=278s) | Pause easter egg: “Where's the pause theme at, huh? How are you not going to have pause music?” |
| SimpleFlips scream — SimpleFlips | [6:57 in the source video](https://www.youtube.com/watch?v=CgIsvX92k0A&t=417s) | Rare heavy-fall scream |
| RNG quote — MrSpeedrun | [0:08 in the source video](https://www.youtube.com/watch?v=ST42VjR0LYk&t=8s) | “I lost the run to RNG” clip on the fatal Yoshi gift-coin cake screen |

Each supplied clip is downmixed to mono and converted to signed 16-bit
big-endian PCM at 32,000 and 30,000 Hz. Conversion preserves the supplied crop
without further trimming or normalization. Only the output-rate variant selected
by the build is linked. Attribution identifies the source; no separate reuse
permission statement is recorded for these three clips.

The individual notes retain source hashes, output hashes, durations and conversion
instructions:

- [Dunkey pause quote](../sound/pcm/README.md)
- [SimpleFlips scream](../sound/pcm/simpleflips_scream.md)
- [MrSpeedrun RNG quote](../sound/pcm/yoshi_coin_cake.md)

## Mii Channel music

- Original music: Nintendo's Wii Mii Channel theme.
- MIDI arrangement: **iteachvader**, credited as a YouTube creator, obtained
  from haminet.com according to the supplied README.
- Super Mario 64 conversion: **TheNail**, using sound bank 14, Inside Castle.
- [Creator showcase, Mii Channel but it's Mario 64](https://www.youtube.com/watch?v=0JTq_ImPezg)
- [TheNail's channel](https://www.youtube.com/@TheNailDev)
- [Download folder linked by the creator](https://drive.google.com/drive/folders/1M3P5R9R0F50lMpFn4d7uJ2jiLSuYMr5a?usp=sharing)
- The sequence used by Physics Lab is byte-identical to the supplied source.

The creator's supplied README permits use of their songs in ROMhacks and makes
credit optional. This project retains the attribution. The exact haminet MIDI
page is not identified. See the [expanded source notes](../sound/custom/mii-channel/README.txt)
and [unchanged creator README](../sound/custom/mii-channel/README.original.txt).

## Evangelion MIDI — Good, or Don't Be

- Original MIDI source: [Animezen's Evangelion MIDI collection](https://animezen.net/midis/evangelion),
  listed as “Cruel Angel's Thesis (Good or don't be).”
- Rearranged by **GPT-5.6 Sol** into the fuller/punchier version supplied by the
  project owner as `Good_or_Dont_Be_SM64_Fuller_Punchier.mid`.
- The supplied rearrangement is retained as `sound/custom/good-or-dont-be/arrangement.mid`
  and converted to a native Super Mario 64 sequence for the secret castle lobby.

See the [arrangement and conversion notes](../sound/custom/good-or-dont-be/README.md).

## Motos model and animations

The JRB encounter's Motos assets came from the owner-supplied patch,
associated with [this showcase](https://www.youtube.com/watch?v=Aud0uFGDOIs).
The patch creator and individual asset artist are not named in the local import
notes; no separate permission statement is recorded there.

Mesh vertices, triangles, textures, animation samples and joint indices are
preserved. Local integration changes geometry macros, lighting commands,
opacity and material cleanup. It omits the enemy held-Mario callback and shadow.
The patch's enemy behavior, level replacements and unrelated configuration
changes are not imported.

The [import README](../actors/motos/README.md) records the patch SHA-256 and
integration details.

## Mario portrait

The portrait derives from the **Render96 HD Texture Pack** file-select portrait,
with original Mario artwork and character credited to **Nintendo** and the
reconstruction credited to the **Render96 contributors**.

- [Render96 texture pack](https://github.com/pokeheadroom/RENDER96-HD-TEXTURE-PACK)
- [Exact source image](https://raw.githubusercontent.com/pokeheadroom/RENDER96-HD-TEXTURE-PACK/master/gfx/levels/menu/main_menu_seg7.01018.rgba16.png)
- [Render96 artwork source research](https://github.com/Render96/Render96Wiki/wiki/Nintendo-Asset-Libraries-(SM64)#nintendocojp---nintendo-of-japans-website-circa-june-1996)
- Retrieved: 2026-09-30.
- Source dimensions: 256 × 128 pixels.
- Game portrait dimensions: 64 × 32 pixels.

Local edits adapt the appearance, and downsample once for the game. This is a
Render96 reconstruction, not an authenticated pristine Nintendo render.
Render96's project description permits using the textures as a base with
credit; its repository also carries a GPL-3.0 license.

## Castle secret-room paintings

- Lady Bow portrait: artwork from **Paper Mario (2000)**, supplied by the project
  owner as `smug bow.png` (35×33 pixels). Preserved at its original pixel size,
  centered on an opaque 64×64 canvas and split into two 64×32 RGBA16 tiles for
  N64 texture memory. No generated artwork.
- Alternate Lady Bow portrait: artwork from **Paper Mario (2000)**, supplied by
  the project owner as `bow.png` (41×34 pixels). Original opaque sprite pixels
  are preserved, centered on the same opaque 64×64 background as the smug
  portrait and split into two 64×32 RGBA16 tiles.
- Physics Lab completion painting: **GPT-6.1 Sol** captured a screenshot inside
  the Physics Lab for this painting. The existing 64×64 asset,
  `tools/assets/physics_lab_painting/physics-lab-painting.rgba16.png`, is copied
  without resizing into two 64×32 tiles for the secret room.

## Project additions and Nintendo derivatives

| Asset group | Origin and local work |
| --- | --- |
| HE THICC body and palette | Modified Nintendo Mario geometry and textures. Body proportions are baked locally while preserving topology, UVs, bones and animation. The recolors are project edits. |
| Dab animation | Project-authored poses on Nintendo's standard Mario skeleton, with local breathing, recovery and analog-control extensions. No external animation import is recorded. |
| Yoshi tank top and sunglasses | Local additions to Nintendo's Yoshi model based on the owner-supplied [reference by @of_zeina on X](https://x.com/of_zeina/status/1418302107044794370). No external model import is recorded. |
| Poison mushroom | Custom texture using the existing Nintendo mushroom sprite/model setup. |
| Physics Lab geometry and collision | Project-generated test environment. The initial committed version is described as Astra-generated; later revisions use the same local generator. Enemy assets are repackaged Nintendo actors. |
| SimpleFlips encounter room and sign | Project-created room/sign geometry with locally repackaged Nintendo Toad and MIPS model/animation data. |
| Interactive SimpleFlips head | Adaptation of Nintendo Toad head data for the intro/game-over segment. |
| Title credit | Edited HackerSM64 title-credit textures. |
| English Rumble Pak label | Local pixel-text edit of Nintendo's Shindou title-screen Rumble Pak graphic, preserving the controller and border. The game uses `textures/custom/rumble_pak_supported.rgba16.png`; ROM extraction keeps the original texture separate. |
| Smaller shadow textures | Resized derivatives of existing game shadow textures. |
| Trimmed and cap-specific audio samples | Derived copies of existing game instruments and sound effects for local audio processing. No new outside recording is associated with these copies. |

See the [Physics Lab notes](../levels/physics_lab/README.md) for its development
and source details.

Runtime recolors, display-list variants, LOD copies and HUD drawing built from
existing assets do not introduce a separate external art source. Nintendo
remains the origin of the underlying game models, textures, animations and
audio samples used by those variants.

## Inherited HackerSM64 assets

The checkout inherits HackerSM64's Floomba face textures, modified Fly Guy face,
warp-pipe side texture, breath-meter graphics, custom text textures and title
branding. The [upstream credits](../credits.md) credit **ArcticJaguar725** for
Floombas and **Pyro Jay** for texture improvements and repository banner art.
They do not assign every inherited texture to an individual artist.

The crash-screen bitmap font is adapted from
[ProFont](https://tobiasjung.name/profont/), credited to **Carl Osterwald,
Stephen C. Gilardi and Andrew Welch**. Its retained
[MIT license](../textures/crash_custom/LICENSE) includes the copyright and
permission notice.

## Attribution still to identify

- Motos patch creator, asset artist and any creator-supplied reuse terms.
- Exact haminet page for iteachvader's Mii Channel MIDI.
- Individual artists for inherited HackerSM64 textures where upstream credits
  give only general contributions.
