# SimpleFlips rare heavy-fall scream

Creator: **SimpleFlips**. Source moment:
[6:57 in the source video](https://www.youtube.com/watch?v=CgIsvX92k0A&t=417s).
Source video: **ChatGPT helps make a Troll Hack**, uploaded by **SimpleFlips** on **2023-10-11**.
The video title, uploader and upload date were verified from public YouTube
metadata on 2026-10-07. The project owner supplied the clip source and timestamp.
See the [asset source register](../../docs/asset-sources.md).

Source: `$HOME/Documents/simpleflipsscream.wav`.
Source SHA-256: `d85c6042da2e9644bee3628afbcee691e31472d42b6827fc2c8dbb0402ac5331`.

The complete supplied 0.812854-second stereo clip is downmixed to mono and
resampled to signed 16-bit big-endian PCM, without trimming or normalization.

- `simpleflips_scream.s16be`: 32,000 Hz, 26,012 samples, 52,024 bytes; SHA-256 `4ebc1ee015d069228690f7af7da149f0997ad16f092a88268d91a6ff181952ac`.
- `simpleflips_scream_30k.s16be`: 30,000 Hz, 24,386 samples, 48,772 bytes; SHA-256 `893f7e3da259dc53d2e984b162bf51a77bfc158fb1f28ca00f625ad29749f080`.

Reproduce from the project root:

```sh
ffmpeg -v error -i "$HOME/Documents/simpleflipsscream.wav" -ac 1 -ar 32000 -c:a pcm_s16be -f s16be sound/pcm/simpleflips_scream.s16be
ffmpeg -v error -i "$HOME/Documents/simpleflipsscream.wav" -ac 1 -ar 30000 -c:a pcm_s16be -f s16be sound/pcm/simpleflips_scream_30k.s16be
```

Only the matching output-rate variant is linked. `sound_data.s` provides the
existing 1,296-byte zero tail for final mixer chunks and complete DMA fetches.
The clip streams from ROM into both dry channels; it does not allocate a full
clip RAM buffer or consume a stealable SFX voice. Its independent audio-thread
position has no game stop API, so ordinary voice stops, pause exits, warps and
other PCM quotes cannot cancel or restart it. Actual game reset clears it.

The existing heavy-fall warning eligibility and once-per-fall flag precede the
1/8192 roll. A successful start spends a game-thread latch until actual loading
of a different save file or game reset. Same-file reloads, deaths, levels and
character changes do not rearm it; the latch is not stored in EEPROM. Demo
playback never rolls. An unavailable/busy stream falls back to the ordinary
voice without spending the latch. The current US/JP/EU PCM path supports this
clip; Shindou's separate audio ABI retains the ordinary scream.
