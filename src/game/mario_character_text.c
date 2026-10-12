#include "sm64.h"
#include "mario_character_text.h"

// Look past well-formed zero-width colours without treating their hex payload
// as letters. Invalid controls remain the existing parser's responsibility.
static const u8 *character_next_glyph(const u8 *str, enum MarioTextEncoding encoding) {
    if (str == NULL) {
        return NULL;
    }
    while (encoding == MARIO_TEXT_DIALOG && *str == 0xDF) {
        const u8 *payload = ++str;
        s32 i;
        for (i = 0; i < 8; i++) {
            if (!((payload[i] < 0x10) || (payload[i] >= 0x24 && payload[i] <= 0x29))) {
                break;
            }
        }
        if (i != 8) {
            for (i = 0; i < 8 && payload[i] == 0x9F; i++) {
                // A default colour is eight 0x9F bytes.
            }
            if (i != 8) {
                return payload;
            }
        }
        str += 8;
    }
    return str;
}

static s32 character_word_continues(const u8 *following, enum MarioTextEncoding encoding) {
    const u8 *next = character_next_glyph(following, encoding);
    u8 c;
    if (next == NULL) {
        return FALSE;
    }
    c = *next;
    // An apostrophe continues a contraction/possessive only before a letter.
    if (c == (encoding == MARIO_TEXT_ASCII ? '\'' : 0x3E)) {
        next = character_next_glyph(next + 1, encoding);
        c = *next;
    }
    if (encoding == MARIO_TEXT_ASCII) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    }
    return c <= 0x3D; // Encoded digits and Latin letters.
}

u8 mario_character_text_glyph(enum MarioTextStyle style, enum MarioTextEncoding encoding, u8 *previous,
                              u8 original, const u8 *following) {
    u8 result = original;
    u8 upperC = encoding == MARIO_TEXT_ASCII ? 'C' : 0x0C;
    u8 upperK = encoding == MARIO_TEXT_ASCII ? 'K' : 0x14;
    u8 lowerC = encoding == MARIO_TEXT_ASCII ? 'c' : 0x26;
    u8 lowerK = encoding == MARIO_TEXT_ASCII ? 'k' : 0x2E;
    u8 upperS = encoding == MARIO_TEXT_ASCII ? 'S' : 0x1C;
    u8 upperE = encoding == MARIO_TEXT_ASCII ? 'E' : 0x0E;
    u8 lowerS = encoding == MARIO_TEXT_ASCII ? 's' : 0x36;
    u8 lowerE = encoding == MARIO_TEXT_ASCII ? 'e' : 0x28;
    s32 lowercase = encoding == MARIO_TEXT_ASCII || encoding == MARIO_TEXT_DIALOG;

    // Inline colour changes have no visible glyph and do not break a word.
    if (encoding == MARIO_TEXT_DIALOG && original == 0xDF) {
        return original;
    }

    if (style == MARIO_TEXT_HE_THICC && encoding != MARIO_TEXT_LITERAL) {
        if (*previous == upperC || (lowercase && *previous == lowerC)) {
            if (original == upperK) {
                result = upperC;
            }
            if (lowercase && original == lowerK) {
                result = lowerC;
            }
        }
        if (*previous == upperS || (lowercase && *previous == lowerS)) {
            if (original == upperE) {
                result = upperS;
            }
            if (lowercase && original == lowerE) {
                result = lowerS;
            }
        }
        if ((original == (encoding == MARIO_TEXT_ASCII ? 'D' : 0x0D)
             || (lowercase && original == (encoding == MARIO_TEXT_ASCII ? 'd' : 0x27)))
            && !character_word_continues(following, encoding)) {
            result = MARIO_TEXT_SKIP;
        }
    }
    // Remember the source: ckk becomes cck, and see becomes sse.
    *previous = original;
    return result;
}

static s32 character_name_letter(u8 glyph, enum MarioTextEncoding encoding) {
    if (encoding == MARIO_TEXT_ASCII) {
        if (glyph >= 'a' && glyph <= 'z') {
            glyph -= 'a' - 'A';
        }
        return glyph >= 'A' && glyph <= 'Z' ? glyph - 'A' + 10 : -1;
    }
    if (encoding == MARIO_TEXT_DIALOG && glyph >= 0x24 && glyph <= 0x3D) {
        glyph -= 0x1A;
    }
    return glyph >= 10 && glyph <= 35 ? glyph : -1;
}

static s32 character_name_matches(const u8 *text, enum MarioTextEncoding encoding, s32 previous) {
    const u8 name[] = { 22, 10, 27, 18, 24 };
    if (previous >= 0) {
        return FALSE;
    }
    for (s32 i = 0; i < 5; i++) {
        if (character_name_letter(text[i], encoding) != name[i]) {
            return FALSE;
        }
    }
    return character_name_letter(text[5], encoding) < 0;
}

// Copy controls intact and replace complete names without modifying source text.
static u32 character_named_text(enum MarioTextEncoding encoding, const u8 *text, u8 *output) {
    const char name[] = "HE THICC";
    u8 end = encoding == MARIO_TEXT_ASCII ? 0 : 0xFF;
    u32 length = 0;
    s32 previous = -1;
    while (*text != end) {
        if (encoding == MARIO_TEXT_DIALOG && *text == 0xDF) {
            const u8 *next = character_next_glyph(text, encoding);
            if (next > text + 1) {
                while (text < next) {
                    if (output != NULL) {
                        output[length] = *text;
                    }
                    length++;
                    text++;
                }
                continue;
            }
        }
        if (character_name_matches(text, encoding, previous)) {
            for (s32 i = 0; i < 8; i++) {
                u8 glyph = name[i];
                if (encoding != MARIO_TEXT_ASCII) {
                    glyph = glyph == ' ' ? 0x9E : glyph - 'A' + 10;
                }
                if (output != NULL) {
                    output[length] = glyph;
                }
                length++;
            }
            text += 5;
            previous = 24;
        } else {
            previous = character_name_letter(*text, encoding);
            if (output != NULL) {
                output[length] = *text;
            }
            length++;
            text++;
        }
    }
    if (output != NULL) {
        output[length] = end;
    }
    return length + 1;
}

u32 mario_character_text_name_size(enum MarioTextStyle style, enum MarioTextEncoding encoding,
                                   const u8 *text) {
    if (style != MARIO_TEXT_HE_THICC || encoding == MARIO_TEXT_LITERAL) {
        return 1;
    }
    return character_named_text(encoding, text, NULL);
}

const u8 *mario_character_text_replace_name(enum MarioTextStyle style, enum MarioTextEncoding encoding,
                                            const u8 *text, u8 *output) {
    if (style != MARIO_TEXT_HE_THICC || encoding == MARIO_TEXT_LITERAL) {
        return text;
    }
    character_named_text(encoding, text, output);
    return output;
}
