// 0x03016C00
static const s16 bowser_key_seg3_animvalue_unlock_door[] = {
#include "assets/rom/generated/271d894b6a91d8c64540.inc.c"









































































































};

// 0x03017294
static const u16 bowser_key_seg3_animindex_unlock_door[] = {
#include "assets/rom/generated/c5cb6ea4a5e0ed264897.inc.c"

};

// 0x030172B8
static const struct Animation bowser_key_seg3_anim_unlock_door = {
    1,
    0,
    0,
    0,
    0x8C,
    ANIMINDEX_NUMPARTS(bowser_key_seg3_animindex_unlock_door),
    bowser_key_seg3_animvalue_unlock_door,
    bowser_key_seg3_animindex_unlock_door,
    0,
};
