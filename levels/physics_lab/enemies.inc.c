// Keep the start, coasting lanes and controlled water terraces clear. These are
// ordinary non-boss behaviors with their normal interaction and attack logic.

// Small-to-large comparison cells share the same approach and floor material.
OBJECT(MODEL_GOOMBA, 2100, 0, -12000, 0, 0, 0, 2 << 16, bhvGoomba),
OBJECT(MODEL_GOOMBA, 3300, 0, -12000, 0, 0, 0, 0, bhvGoomba),
OBJECT(MODEL_GOOMBA, 4500, 0, -12000, 0, 0, 0, 1 << 16, bhvGoomba),
#ifdef FLOOMBAS
OBJECT(MODEL_GOOMBA, 5700, 0, -12000, 0, 0, 0, 0, bhvFloomba),
#endif
OBJECT(LAB_MODEL_KOOPA_WITH_SHELL, 3300, 0, -11100, 0, 0, 0, KOOPA_BP_NORMAL << 16, bhvKoopa),
OBJECT(LAB_MODEL_KOOPA_WITHOUT_SHELL, 4500, 0, -11100, 0, 0, 0, KOOPA_BP_UNSHELLED << 16, bhvKoopa),
OBJECT(LAB_MODEL_PIRANHA_PLANT, 2100, 0, -11100, 0, 0, 0, 0, bhvPiranhaPlant),
OBJECT(LAB_MODEL_SCUTTLEBUG, 5700, 0, -11100, 0, 0, 0, 0, bhvScuttlebug),

// Grabbing targets share an open recovery strip north of mover boarding.
// Approach Heave-Ho from the west to observe its long eastward throw.
OBJECT(MODEL_CHUCKYA, 8100, 0, -8150, 0, 0, 0, 0, bhvChuckya),
OBJECT(LAB_MODEL_HEAVE_HO, 10500, 0, -8150, 0, 270, 0, 0, bhvHeaveHo),

// Heavy targets form a north row; native rolling courses retain their poses.
OBJECT(LAB_MODEL_WHOMP, 11200, 0, -11800, 0, 0, 0, 0, bhvSmallWhomp),
OBJECT(LAB_MODEL_THWOMP, 12300, 0, -11800, 0, 0, 0, 0, bhvThwomp),
OBJECT(LAB_MODEL_SSL_GRINDEL, 13400, 0, -11800, 0, 0, 0, 0, bhvGrindel),
OBJECT(LAB_MODEL_SSL_SPINDEL, -13800, 188, 4600, 0, 0, 0, 0, bhvSpindel),
OBJECT(LAB_MODEL_SSL_TOX_BOX, 7600, 0, -11800, 0, 90, 0, 2 << 16, bhvToxBox),
// Second Tox Box: its complete rolling footprint stays in the open north court.
OBJECT(LAB_MODEL_SSL_TOX_BOX, -500, 0, -11100, 0, 0, 0, 2 << 16, bhvToxBox),
// Projectile comparison lanes, separate from the matched impact beds.
OBJECT(LAB_MODEL_BULLET_BILL, -4500, 200, 11650, 0, 0, 0, 0, bhvBulletBill),
OBJECT(MODEL_NONE, -3500, 0, 13200, 0, 0, 0, 4 << 16, bhvWaterBombSpawner),
// Neighboring explosion, aimed-shot and fire bays have tall side backstops.
OBJECT(MODEL_BLACK_BOBOMB, -5550, 0, 11600, 0, 0, 0, 0, bhvBobomb),
OBJECT(LAB_MODEL_SNUFIT, -5550, 450, 12400, 0, 270, 0, 0, bhvSnufit),
OBJECT(LAB_MODEL_PIRANHA_PLANT, -5550, 0, 13200, 0, 270, 0, 0, bhvFirePiranhaPlant),
OBJECT(MODEL_NONE, -5550, 200, 13900, 0, 270, 0, 0, bhvFlamethrower),
OBJECT(MODEL_NONE, -5550, 2, 14300, 0, 270, 0, 0, bhvBouncingFireball),
// Contained north/south rolling lane beyond the BLJ exit; clear of the pools.
OBJECT(LAB_MODEL_HMC_ROLLING_ROCK, 14300, 0, 12600, 0, 0, 0, 0, bhvBigBoulder),

// West air targets progress from a Spiny spawning floor to flying/cap targets.
OBJECT(LAB_MODEL_ENEMY_LAKITU, -12200, 700, -11800, 0, 0, 0, 0, bhvEnemyLakitu),
OBJECT(LAB_MODEL_SPINY, -12200, 0, -11150, 0, 0, 0, 0, bhvSpiny),
OBJECT(MODEL_FLYGUY, -12200, 450, -9800, 0, 0, 0, 0, bhvFlyGuy),
OBJECT(LAB_MODEL_KLEPTO, -12100, 1100, -8200, 0, 0, 0, KLEPTO_BP_NO_STAR << 16, bhvKlepto),
OBJECT(MODEL_NONE, -10500, 0, -6500, 0, 0, 0, 0, bhvPokey),
// Wind faces north, away from the southern contact trials and main runway.
OBJECT(LAB_MODEL_TWEESTER, -10800, 0, -2600, 0, 0, 0, 0, bhvTweester),
OBJECT(MODEL_MIST, -10800, 450, -300, 0, 180, 0, CLOUD_BP_FWOOSH << 16, bhvCloud),

// Matched dry contact courts avoid confounding Bully comparison with ice.
OBJECT(LAB_MODEL_BULLY, -10800, 0, 1500, 0, 0, 0, 0, bhvSmallBully),
OBJECT(LAB_MODEL_CHILL_BULLY, -10800, 0, 4200, 0, 0, 0, 0, bhvSmallChillBully),
// Environmental contacts remain beside the cold-water and shore tests.
OBJECT(LAB_MODEL_SPINDRIFT, 2700, 0, 13000, 0, 0, 0, 0, bhvSpindrift),
OBJECT(LAB_MODEL_MR_BLIZZARD, 2700, 0, 13900, 0, 180, 0, 0, bhvMrBlizzard),
OBJECT(MODEL_YELLOW_COIN, 11600, 0, 7200, 0, 0, 0, 0, bhvMoneybagHidden),
OBJECT(LAB_MODEL_UKIKI, 2700, 0, 7100, 0, 0, 0, UKIKI_BP_CAP << 16, bhvUkiki),

// Cave and haunted-house targets. Flying books and projectiles use their
// native spawners; Monty Mole has three usable holes, not a stranded mole.
// Flat circling space away from the homing Amp and mole-hole cluster.
OBJECT(LAB_MODEL_MR_I_BODY, -10600, 0, -5000, 0, 90, 0, 0, bhvMrI),
OBJECT(LAB_MODEL_SWOOP, -13800, 1000, 9600, 0, 0, 0, 0, bhvSwoop),
OBJECT(LAB_MODEL_BOO, -13900, 250, 11300, 0, 0, 0, 0, bhvBoo),
OBJECT(LAB_MODEL_HAUNTED_CHAIR, -13100, 0, 11000, 0, 270, 0, 0, bhvHauntedChair),
OBJECT(LAB_MODEL_MAD_PIANO, -13900, 0, 11000, 0, 270, 0, 0, bhvMadPiano),
OBJECT(MODEL_NONE, -14600, 200, 11400, 0, 90, 0, 0, bhvBookendSpawn),
// Burrowing targets have a flat three-hole court east of the flight landing.
OBJECT(LAB_MODEL_DL_MONTY_MOLE_HOLE, 12700, 0, -10200, 0, 0, 0, 0, bhvMontyMoleHole),
OBJECT(LAB_MODEL_DL_MONTY_MOLE_HOLE, 13400, 0, -9500, 0, 0, 0, 0, bhvMontyMoleHole),
OBJECT(LAB_MODEL_DL_MONTY_MOLE_HOLE, 12700, 0, -8900, 0, 0, 0, 0, bhvMontyMoleHole),
OBJECT(LAB_MODEL_MONTY_MOLE, 12700, 0, -10200, 0, 0, 0, 0, bhvMontyMole),
OBJECT(LAB_MODEL_CHAIN_CHOMP, -11800, 0, -6500, 0, 0, 0, 0, bhvChainChomp),

// Open southeast aquatic habitat. The shark's complete native
// orbit fits inside this tank; the eel uses a local loop, never JRB's path.
OBJECT(LAB_MODEL_SUSHI, 9600, -900, 12300, 0, 0, 0, 0, bhvSushiShark),
OBJECT(LAB_MODEL_UNAGI, 8700, -1300, 11300, 0, 0, 0, UNAGI_BP_START_SWIMMING << 16, bhvUnagi),
OBJECT(LAB_MODEL_BUBBA, 8500, -500, 13500, 0, 0, 0, 0, bhvBubba),
OBJECT(MODEL_NONE, 9000, -600, 13100, 0, 0, 0, 0, bhvBubSpawner),
OBJECT(LAB_MODEL_CLAM_SHELL, 8200, -2200, 11400, 0, 0, 0, 0, bhvClamShell),
OBJECT(LAB_MODEL_SKEETER, 8800, -40, 14000, 0, 0, 0, 0, bhvSkeeter),

// Shock contacts at the main shoreline, shallow terraces and cold-water pool.
OBJECT(MODEL_AMP, 6200, 140, 5600, 0, 0, 0, AMP_BP_ROT_RADIUS_200 << 16, bhvCirclingAmp),
OBJECT(MODEL_AMP, 5000, 240, 8800, 0, 0, 0, AMP_BP_ROT_RADIUS_200 << 16, bhvCirclingAmp),
OBJECT(MODEL_AMP, 5200, 320, 13600, 0, 0, 0, AMP_BP_ROT_RADIUS_200 << 16, bhvCirclingAmp),
// A separate shock court keeps homing Amps away from impact bay C's exit.
OBJECT(MODEL_AMP, -14400, 300, 12900, 0, 0, 0, 0, bhvHomingAmp),
OBJECT(MODEL_AMP, -12700, 300, 13250, 0, 0, 0, 0, bhvHomingAmp),
OBJECT(MODEL_AMP, -14200, 200, 14200, 0, 0, 0, 0, bhvCirclingAmp),

// Left chest, then right: one pair on shore, one on the cold pool's flat floor.
OBJECT(MODEL_NONE, 7300, 0, 7100, 0, 0, 0, 1 << 16, bhvPhysicsLabChests),
OBJECT(MODEL_NONE, 4500, 120, 13100, 0, 0, 0, 0, bhvPhysicsLabChests),
