# Yoshi coin cake clip

Speaker: **MrSpeedrun** (owner attribution). Uploader: **Flickerform**. Source moment:
[0:08 in the source video](https://www.youtube.com/watch?v=ST42VjR0LYk&t=8s).
Source video: **I lost the run to RNG, dude!**, uploaded by **Flickerform** on **2017-05-13**.
The video title, uploader and upload date were verified from public YouTube
metadata on 2026-10-07. The project owner supplied the speaker attribution and
0:08 source moment. The upload date is for the linked video, not necessarily the
original recording. See the [asset source register](../../docs/asset-sources.md).

Source: `$HOME/Documents/ilosttheruntorngdudewhat.wav` (the supplied cropped WAV).
Source SHA-256: `99749d62036bf8cc578d1adc2ef930c8ba6fef9732939a4626c077b361b917d0`.

The full supplied crop is downmixed to mono and resampled into signed 16-bit
big-endian PCM, without further trimming, normalization or filtering.

- `yoshi_coin_cake.s16be`: 32,000 Hz, 91,030 samples, 182,060 bytes, SHA-256 `70ed7146300ae8d4e32b4fb71fef3a0e35ce82de661d6b106823b7a8d5d26451`.
- `yoshi_coin_cake_30k.s16be`: 30,000 Hz, 85,340 samples, 170,680 bytes, SHA-256 `4e4b68c9bd7e4b0482b1a400a7cac0ed920329c6af7debe6e142dd3901f03aea`.

Reproduce from the project root:

```sh
ffmpeg -v error -i "$HOME/Documents/ilosttheruntorngdudewhat.wav" -ac 1 -ar 32000 -c:a pcm_s16be -f s16be sound/pcm/yoshi_coin_cake.s16be
ffmpeg -v error -i "$HOME/Documents/ilosttheruntorngdudewhat.wav" -ac 1 -ar 30000 -c:a pcm_s16be -f s16be sound/pcm/yoshi_coin_cake_30k.s16be
```

The existing PCM mixer streams the matching-rate asset from ROM into both dry
channels. Only one rate is linked into a build. Each asset has the existing
1,296-byte zero DMA tail supplied by `sound_data.s`; no full-clip RAM buffer is
allocated. Like the pause quote, this path supports US/JP/EU audio and excludes
Shindou's different audio ABI.

Only a fatal Yoshi gift coin requests this clip on the cake screen. Music is
faded out on that entry, and the normal thank-you sound is replaced by the clip.
The special cake cannot pause and will not leave during playback. Its automatic
departure reloads the saved file and enters the normal game-over Mario head.
Ordinary cake visits retain their audio and pause behavior.
