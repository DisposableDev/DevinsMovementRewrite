# Dunkey pause easter egg

Creator: **videogamedunkey (Dunkey)**. Source moment:
[4:38 in the source video](https://www.youtube.com/watch?v=GmUiVXW4R5E&t=278s).
Source video: **Yooka Laylee (dunkview)**, uploaded by **videogamedunkey** on **2017-04-11**.
The video title, uploader and upload date were verified from public YouTube
metadata on 2026-10-07. The project owner supplied the clip source and timestamp.
See the [asset source register](../../docs/asset-sources.md).

`dunkey_pause.s16be` is the user-supplied Dunkey quote:
“Where's the pause theme at, huh? How are you not going to have pause music?”

32,000 Hz, mono, signed 16-bit big-endian PCM, uncompressed. There are
107,182 samples (3.3494375 seconds), occupying 214,364 bytes. The audio source
is `$HOME/Documents/Dunkey Audio.wav`; the resampling/downmix preserves
its full duration, without additional filtering, normalization or trimming.

Source SHA-256: `d16654c7516471aa39d2949c8c76c101bfedf72583382cdd3562a42d3b622677`

PCM SHA-256: `aa83f5bdf66225f461f4cdbee53f388c195057746dbbbc284bbe8c0be08118a1`

The audio-quality trial selects `dunkey_pause_30k.s16be`, resampled from the
same original WAV at 30,000 Hz. Its 100,483 samples occupy 200,966 bytes and
preserve the quote's nominal duration within one output sample. The original
32 kHz asset remains available for `AUDIO_QUALITY_TRIAL=0`.

30 kHz PCM SHA-256: `b4d83b5b6b520877b8c507251c1c837fb05d9785211082cf777963a86762517a`

Reproduce from the project root:

```sh
ffmpeg -v error -i "$HOME/Documents/Dunkey Audio.wav" -ac 1 -ar 32000 -c:a pcm_s16be -f s16be sound/pcm/dunkey_pause.s16be
ffmpeg -v error -i "$HOME/Documents/Dunkey Audio.wav" -ac 1 -ar 30000 -c:a pcm_s16be -f s16be sound/pcm/dunkey_pause_30k.s16be
```

`sound_data.s` aligns the PCM to 16 bytes and adds a zero tail covering a
complete 1,296-byte sample-cache DMA fetch. The sample stays in ROM; it does
not reserve a full-clip RAM buffer or enter the ADPCM encoder. The current
US/JP/EU synthesizer mixes the matching 30 or 32 kHz streamed PCM into both dry channels before
interleaving. Shindou's different audio ABI is not supported; it cannot trigger
or consume this easter egg.

The consumed marker uses the high bit of the saved cap-area byte. Area access
masks/preserves that bit, keeping the EEPROM size and existing save compatibility.
Copying a consumed save retains it; erasing a save creates a new opportunity.
