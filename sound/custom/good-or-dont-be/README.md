# Good, or Don't Be — secret castle lobby

Source: the user-supplied `Good_or_Dont_Be_SM64_Fuller_Punchier.mid`, retained as
`arrangement.mid`. This is the definitive fuller/punchier arrangement.

The original MIDI came from [Animezen's Evangelion MIDI collection](https://animezen.net/midis/evangelion),
listed as “Cruel Angel's Thesis (Good or don't be).” It was rearranged by
**GPT-5.6 Sol** into the fuller/punchier version supplied by the project owner.

Run `python3 sound/custom/good-or-dont-be/convert.py` from any directory to regenerate
the native assembly and conversion manifest. The converter preserves all note
on/off pairs, including overlapping equal pitches, track separation, note
velocities, pan, volume and expression controllers. MIDI timing is rounded to
SM64's 48 ticks per quarter note. Five one-MIDI-tick string notes use the
engine's minimum one-native-tick duration instead of rounding to zero. The tempo is 118 BPM. Section 1 starts at
native tick 7296; each repeat lasts 3072 ticks (approximately 32.54 seconds).
The roughly 77.29-second introduction and outro play once. Sequence, channel
controller scripts and all note layers have actual backward jumps to their
Section 1 labels; no stream restarts the introduction. No notes cross either
boundary in the input. Native decay/reverb tails remain engine-dependent.

The existing `0C_extended_bank` contains the intended program mapping:
4 Grand Piano (both piano parts), 7 Strings, 8 Acoustic Guitar, and
13 Celesta (the existing vibraphone sample). No replacement sample bank is
introduced. Seven channels provide 19 monophonic note layers, at most four
per channel. Original MIDI programs are checked explicitly; General MIDI
program assumptions are not used. The track's expression and reverb controller
values remain in the native channel scripts. The SF2 and mastered audio were
not supplied and their EQ, compression, limiting and spatial mix are not
represented by this native sequence.

The active sequence is `sound/sequences/24_custom_good_or_dont_be.s`; the
conversion manifest records source identity, timing, instruments and note
counts. Source/sequence validation and compilation do not establish audible
mix quality or absence of runtime voice stealing.
