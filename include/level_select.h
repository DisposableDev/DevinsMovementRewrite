#ifndef LEVEL_SELECT_H
#define LEVEL_SELECT_H

#include <PR/ultratypes.h>
#include "level_table.h"

// Display order only: the registry order defines persistent level IDs.
// Keep each debug destination once, with optional stages near their castle district.
static const u8 sLevelSelectOrder[] = {
    LEVEL_CASTLE_GROUNDS,
    LEVEL_CASTLE,
    LEVEL_BOB,
    LEVEL_PSS,
    LEVEL_WF,
    LEVEL_JRB,
    LEVEL_SA,
    LEVEL_CCM,
    LEVEL_TOTWC,
    LEVEL_CASTLE_COURTYARD,
    LEVEL_BBH,
    LEVEL_BITDW,
    LEVEL_BOWSER_1,
    LEVEL_VCUTM,
    LEVEL_HMC,
    LEVEL_COTMC,
    LEVEL_LLL,
    LEVEL_SSL,
    LEVEL_DDD,
    LEVEL_BITFS,
    LEVEL_BOWSER_2,
    LEVEL_SL,
    LEVEL_WDW,
    LEVEL_TTM,
    LEVEL_THI,
    LEVEL_TTC,
    LEVEL_RR,
    LEVEL_WMOTR,
    LEVEL_BITS,
    LEVEL_BOWSER_3,
};

#define LEVEL_SELECT_COUNT ((s32) (sizeof(sLevelSelectOrder) / sizeof(sLevelSelectOrder[0])))

// One destination per area with a usable Mario entrance, checked against the level scripts.
static inline s32 level_select_warp_count(s32 level) {
    switch (level) {
        case LEVEL_CASTLE:
        case LEVEL_THI:
        case LEVEL_TTM:
            return 3;
        case LEVEL_CCM:
        case LEVEL_SSL:
        case LEVEL_SL:
        case LEVEL_JRB:
        case LEVEL_LLL:
            return 2;
        default:
            return 1;
    }
}

// TTM's area 3 has no entrance; its third choice uses the slide exit in area 4.
static inline s32 level_select_warp_area(s32 level, s32 warp) {
    if (level == LEVEL_TTM && warp == 2) {
        return 4;
    }
    return warp + 1;
}

static inline s32 level_select_index(s32 level) {
    for (s32 i = 0; i < LEVEL_SELECT_COUNT; i++) {
        if (sLevelSelectOrder[i] == level) {
            return i;
        }
    }
    return 0;
}

#endif // LEVEL_SELECT_H
