// 0x06005A60
static const s16 moneybag_seg6_animvalue_06005A60[] = {
    0x0000, 0x00B4, 0xFF1A, 0x1FFF, 0x3FFF, 0x1FFF, 0x8001, 0xC001,
    0xC001, 0x8001, 0x8001, 0x8001,
};

// 0x06005A78
static const u16 moneybag_seg6_animindex_06005A78[] = {
#include "assets/rom/generated/ed2984aba744e8cfc6b4.inc.c"






};

// 0x06005AD8
static const struct Animation moneybag_seg6_anim_06005AD8 = {
    0,
    0,
    0,
    0,
    0x1E,
    ANIMINDEX_NUMPARTS(moneybag_seg6_animindex_06005A78),
    moneybag_seg6_animvalue_06005A60,
    moneybag_seg6_animindex_06005A78,
    0,
};
