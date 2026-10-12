// 0x0700C7C8 - 0x0700C8F0
static const s16 castle_grounds_seg7_animvalue_flags[] = {
#include "assets/rom/generated/32462bc066fd1aca70f5.inc.c"









};

// 0x0700C8F0 - 0x0700C944
static const u16 castle_grounds_seg7_animindex_flags[] = {
#include "assets/rom/generated/7b52218c6c11385b07ec.inc.c"





};

// 0x0700C944 - 0x0700C95C
static const struct Animation castle_grounds_seg7_anim_flags = {
    0,
    0,
    0,
    0,
    0x1D,
    ANIMINDEX_NUMPARTS(castle_grounds_seg7_animindex_flags),
    castle_grounds_seg7_animvalue_flags,
    castle_grounds_seg7_animindex_flags,
    0,
};

// 0x0700C95C - 0x0700C960
const struct Animation *const castle_grounds_seg7_anims_flags[] = {
    &castle_grounds_seg7_anim_flags, // 0x0700C944
};
