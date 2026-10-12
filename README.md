# Contributor notice
This is a source code archive only. No contributions are allowed.

# Introduction
This rewrite increases the responsiveness and pace-of-play of Mario's moves, with niche additions that allow for high skill expression. In addition, Mario's power-ups have new properties to allow for further design space. Bowser's fights are also significantly harder, to encourage use of the new controls. In sum, the game will feel vastly better to play, and there are new options for players to explore at their leisure.

An extensive suite of options is available in-game, allowing you to configure the game as you please. This also includes new accessibility and control options. A statistics page also keeps detailed information on most in-game actions, and there is a records page for in-game time.

The design was authored wholly by Devin. New code implementations, and a small amount of assets, were authored by GPT-6.1 Sol, GPT-6 Astra, GPT-6 Sol, and GPT-5.6 Sol. You may view the [documentation](docs/README.md), [credits](credits.md), and [asset sources](docs/asset-sources.md) for detailed authorship information, including credits for external assets.

# Features
- Mario runs faster and jumps higher.
- Input buffers on most moves, up to 6 frames. (A frame is 33.33ms at 30FPS.)
- Animation cancelling on most moves.
- Swimming is much faster and more responsive.
- Transitions between moves are more intentional.
- Attacks are rebalanced so they all have a use.
- Mario can twirl on the ground, in the air, or while swimming.
- A built-in input display, with a controller overlay.
- Camera controls are dramatically improved.
- Dozens of options and tracked statistics.
- Optimizing performance for console compatibility, tested through ares. (Original hardware untested.)
- Lots of new easter eggs in the base game.
- Mario can dab.

# 0.1.0 Known Issues
- In ares, certain scenes dip below 30 FPS.

# Building
See [docs/building.md](docs/building.md). General steps:

1. Install requirements. For Ubuntu: `sudo apt install build-essential python3 git bsdextrautils curl ca-certificates bzip2 patch texinfo libmpfr-dev libmpc-dev zlib1g-dev`.
2. Install N64 GCC 16.2, targeting `mips64-elf`. Building this will take a long time. It uses libdragon's toolchain. Use this script:
   ```
   mkdir -p "$HOME/n64-toolchain-build/build"
   cd "$HOME/n64-toolchain-build"

   curl -fL https://raw.githubusercontent.com/DragonMinded/libdragon/e356bf3f56f7afbf7e5246329562f145965cfdfc/tools/build-toolchain.sh -o build-toolchain.sh

   echo "5e1a604d44acd5bf6d92fd0bb5eba0bfaf1b2a8446378227200cf1a9bf2f55de  build-toolchain.sh" | sha256sum -c - &&
   N64_INST="$HOME/.local/n64-gcc-16.2" BUILD_PATH="$PWD/build" JOBS="$(nproc)" bash build-toolchain.sh
   ```
3. In the project root, place all three original big-endian US, EU, and JP Shindou (SH) .z64 ROMs: `baserom.us.z64`, `baserom.eu.z64`, `baserom.sh.z64`. `sha1sum` checksums:
   1. US: `9bef1128717f958171a4afac3ed78ee2bb4e86ce`
   2. EU: `4ac5721683d0e0b6bbb561b58a71740845dceea9`
   3. SH: `3f319ae697533a255a1003d09202379d78d5a2e0`
4. From the project root, build the game: `make`.

# Version
Devin's Movement Rewrite version 0.1.0. Forked from HackerSM64 version 2.4.0.

# Licence
Where applicable, code under GPLv3 or later. Where applicable, custom assets under CC BY-SA 4.0. See [licence.md](licence.md)
