#ifndef MARIO_CHARACTER_TEXT_H
#define MARIO_CHARACTER_TEXT_H

#include "mario_character.h"

enum MarioTextEncoding { MARIO_TEXT_DIALOG, MARIO_TEXT_ASCII, MARIO_TEXT_HUD, MARIO_TEXT_LITERAL };
#define MARIO_TEXT_SKIP 0xFF

// Call once per original displayed glyph. Colour payloads are handled by the
// existing parser, not passed here. Each separate string starts with previous=0.
// Following is the remaining source text or bounded ASCII lookahead with NUL padding.
// MARIO_TEXT_SKIP omits a glyph without changing source indices or page offsets.
u8 mario_character_text_glyph(enum MarioTextStyle style, enum MarioTextEncoding encoding, u8 *previous,
                              u8 original, const u8 *following);

// Size includes the terminator; unchanged text needs only a one-byte scratch buffer.
u32 mario_character_text_name_size(enum MarioTextStyle style, enum MarioTextEncoding encoding,
                                   const u8 *text);
const u8 *mario_character_text_replace_name(enum MarioTextStyle style, enum MarioTextEncoding encoding,
                                            const u8 *text, u8 *output);

#endif
