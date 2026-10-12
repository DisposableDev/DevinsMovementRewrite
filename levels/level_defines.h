// Define lists for list of level for macros. Each of the following fields are described:
// Argument 1: Readable name displayed by the debug level selectors.
// Argument 2: Level enumerator for enum used to identify the level ID.
// Argument 3: Course enumerator for enum used to identify the course ID.
// Argument 4: Shorthand name of the level which should be the name of the levels/ folder of the level.
// Argument 5: The shared texture bin used.
// Argument 6: Acoustic reaches for each levels.
// Argument 7, 8, 9: Echo levels for individual areas.
// Clueless hackers should change these for their levels btw.
//
// Argument 10: Specify dynamic music tables for levels, if specified. _ for none.
//
// Argument 11: Specify level camera table, if specified. _ for none.

// NOTE: Be sure to edit sZoomOutAreaMasks in camera.c, as there isnt a good way to macro those right now.
// HACKERSM64_DO: Figure something out for sZoomOutAreaMasks?

STUB_LEVEL(  "",               LEVEL_UNKNOWN_1,        COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, _,         _)
STUB_LEVEL(  "",               LEVEL_UNKNOWN_2,        COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, _,         _)
STUB_LEVEL(  "",               LEVEL_UNKNOWN_3,        COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, _,         _)
DEFINE_LEVEL("Big Boo's Haunt", LEVEL_BBH, COURSE_BBH, bbh, spooky, 28000, 0x28, 0x28, 0x28, sDynBbh,
             sCamBBH)
DEFINE_LEVEL("Cool Cool Mountain", LEVEL_CCM, COURSE_CCM, ccm, snow, 17000, 0x10, 0x38, 0x38, _,
             sCamCCM)
DEFINE_LEVEL("Castle Interior", LEVEL_CASTLE, COURSE_NONE, castle_inside, inside, 20000, 0x20, 0x20,
             0x30, _, sCamCastle)
DEFINE_LEVEL("Hazy Maze Cave", LEVEL_HMC, COURSE_HMC, hmc, cave, 16000, 0x28, 0x28, 0x28, sDynHmc,
             sCamHMC)
DEFINE_LEVEL("Shifting Sand Land", LEVEL_SSL, COURSE_SSL, ssl, generic, 15000, 0x08, 0x30, 0x30, _,
             sCamSSL)
DEFINE_LEVEL("Bob-omb Battlefield", LEVEL_BOB, COURSE_BOB, bob, generic, 15000, 0x08, 0x08, 0x08, _, _)
DEFINE_LEVEL("Snowman's Land", LEVEL_SL, COURSE_SL, sl, snow, 14000, 0x10, 0x28, 0x28, _, sCamSL)
DEFINE_LEVEL("Wet-Dry World", LEVEL_WDW, COURSE_WDW, wdw, grass, 17000, 0x10, 0x18, 0x18, sDynWdw, _)
DEFINE_LEVEL("Jolly Roger Bay", LEVEL_JRB, COURSE_JRB, jrb, water, 20000, 0x10, 0x18, 0x18, sDynJrb, _)
DEFINE_LEVEL("Tiny-Huge Island", LEVEL_THI, COURSE_THI, thi, grass, 20000, 0x0c, 0x0c, 0x20, _, sCamTHI)
DEFINE_LEVEL("Tick Tock Clock", LEVEL_TTC, COURSE_TTC, ttc, machine, 18000, 0x18, 0x18, 0x18, _, _)
DEFINE_LEVEL("Rainbow Ride", LEVEL_RR, COURSE_RR, rr, sky, 20000, 0x20, 0x20, 0x20, _, sCamRR)
DEFINE_LEVEL("Castle Grounds", LEVEL_CASTLE_GROUNDS, COURSE_NONE, castle_grounds, outside, 25000, 0x08,
             0x08, 0x08, _, _)
DEFINE_LEVEL("Bowser in the Dark World", LEVEL_BITDW, COURSE_BITDW, bitdw, sky, 16000, 0x28, 0x28, 0x28,
             _, _)
DEFINE_LEVEL("Vanish Cap Under the Moat", LEVEL_VCUTM, COURSE_VCUTM, vcutm, outside, 30000, 0x28, 0x28,
             0x28, _, _)
DEFINE_LEVEL("Bowser in the Fire Sea", LEVEL_BITFS, COURSE_BITFS, bitfs, sky, 16000, 0x28, 0x28, 0x28,
             _, _)
DEFINE_LEVEL("The Secret Aquarium", LEVEL_SA, COURSE_SA, sa, inside, 20000, 0x10, 0x10, 0x10, _, _)
DEFINE_LEVEL("Bowser in the Sky", LEVEL_BITS, COURSE_BITS, bits, sky, 16000, 0x28, 0x28, 0x28, _, _)
DEFINE_LEVEL("Lethal Lava Land", LEVEL_LLL, COURSE_LLL, lll, fire, 22000, 0x08, 0x30, 0x30, _, _)
DEFINE_LEVEL("Dire Dire Docks", LEVEL_DDD, COURSE_DDD, ddd, water, 17000, 0x10, 0x20, 0x20, sDynDdd, _)
DEFINE_LEVEL("Whomp's Fortress", LEVEL_WF, COURSE_WF, wf, grass, 13000, 0x08, 0x08, 0x08, _, _)
DEFINE_LEVEL("Ending Cake", LEVEL_ENDING, COURSE_CAKE_END, ending, generic, 20000, 0x00, 0x00, 0x00, _,
             _)
DEFINE_LEVEL("Castle Courtyard", LEVEL_CASTLE_COURTYARD, COURSE_NONE, castle_courtyard, outside, 20000,
             0x08, 0x08, 0x08, _, _)
DEFINE_LEVEL("The Princess's Secret Slide", LEVEL_PSS, COURSE_PSS, pss, mountain, 20000, 0x28, 0x28,
             0x28, _, _)
DEFINE_LEVEL("Cavern of the Metal Cap", LEVEL_COTMC, COURSE_COTMC, cotmc, cave, 18000, 0x28, 0x28, 0x28,
             _, sCamCotMC)
DEFINE_LEVEL("Tower of the Wing Cap", LEVEL_TOTWC, COURSE_TOTWC, totwc, sky, 20000, 0x20, 0x20, 0x20, _,
             _)
DEFINE_LEVEL("Bowser Fight 1", LEVEL_BOWSER_1, COURSE_BITDW, bowser_1, generic, 60000, 0x40, 0x40, 0x40,
             _, _)
DEFINE_LEVEL("Wing Mario Over the Rainbow", LEVEL_WMOTR, COURSE_WMOTR, wmotr, generic, 20000, 0x28,
             0x28, 0x28, _, _)
DEFINE_LEVEL("Physics Lab", LEVEL_PHYSICS_LAB, COURSE_NONE, physics_lab, generic, 20000, 0x08, 0x08,
             0x08, _, _)
DEFINE_LEVEL("Bowser Fight 2", LEVEL_BOWSER_2, COURSE_BITFS, bowser_2, fire, 60000, 0x40, 0x40, 0x40, _,
             _)
DEFINE_LEVEL("Bowser Fight 3", LEVEL_BOWSER_3, COURSE_BITS, bowser_3, generic, 60000, 0x40, 0x40, 0x40,
             _, _)
STUB_LEVEL(  "",               LEVEL_UNKNOWN_35,       COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, _,         _)
DEFINE_LEVEL("Tall Tall Mountain", LEVEL_TTM, COURSE_TTM, ttm, mountain, 15000, 0x08, 0x08, 0x08, _, _)
STUB_LEVEL(  "",               LEVEL_UNKNOWN_37,       COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, _,         _)
STUB_LEVEL(  "",               LEVEL_UNKNOWN_38,       COURSE_NONE,                                 20000, 0x00, 0x00, 0x00, sDynUnk38, _)
