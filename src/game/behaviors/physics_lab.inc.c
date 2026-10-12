void bhv_physics_lab_chests_init(void) {
    for (s32 i = 0; i < 2; i++) {
        struct Object *chest = spawn_object_relative(i + 1, i == 0 ? -400 : 400, 0, 0, o,
                                                     MODEL_TREASURE_CHEST_BASE, bhvTreasureChestBottom);
        chest->oFaceAngleYaw += 0x8000;
        chest->oMoveAngleYaw += 0x8000;
    }
    o->oTreasureChestNumOpenedChests = 1;
    o->oTreasureChestAboveWater = o->oBehParams2ndByte;
    o->oTreasureChestDoCloseChests = FALSE;
}

void bhv_physics_lab_chests_loop(void) {
    // Keep the native wrong-order shock and underwater healing. A completed
    // pair closes after Mario leaves, ready for another test; no star or drain.
    if (o->oAction == 0 && o->oTreasureChestNumOpenedChests == 3) {
        play_puzzle_jingle();
        o->oAction = 1;
    } else if (o->oAction == 1 && o->oTimer > 90 && o->oDistanceToMario > 1000.0f) {
        o->oTreasureChestDoCloseChests = TRUE;
        o->oTreasureChestNumOpenedChests = 1;
        o->oAction = 2;
    } else if (o->oAction == 2 && o->oTimer > 30) {
        o->oTreasureChestDoCloseChests = FALSE;
        o->oAction = 0;
    }
}

// Repeatable moving geometry for the Physics Lab. Motion is always relative to
// the placed home position; no gameplay state or Mario action is modified here.
void bhv_physics_lab_mover_init(void) {
    const Collision *collision;

    switch (o->oBehParams2ndByte) {
        case LAB_MOVER_DISK:
            collision = physics_lab_disk_collision;
            break;
        case LAB_MOVER_BRIDGE:
            collision = physics_lab_bridge_collision;
            break;
        case LAB_MOVER_WALL_LEFT:
        case LAB_MOVER_WALL_RIGHT:
            collision = physics_lab_closing_wall_collision;
            break;
        default:
            collision = physics_lab_shuttle_collision;
            break;
    }
    o->collisionData = segmented_to_virtual(collision);
    o->oCollisionDistance = 2400.0f;
    o->oDrawingDistance = 16000.0f;
}

void bhv_physics_lab_mover_loop(void) {
    // A 256-frame cycle wraps without a position discontinuity or timer reset.
    s16 phase = (o->oTimer & 0xFF) * 0x100;
    f32 oldX = o->oPosX;
    f32 oldY = o->oPosY;
    s16 oldRoll = o->oFaceAngleRoll;

    switch (o->oBehParams2ndByte) {
        case LAB_MOVER_SHUTTLE:
            o->oPosX = o->oHomeX + 900.0f * sins(phase);
            break;
        case LAB_MOVER_DISK:
            o->oAngleVelYaw = 0x80;
            o->oFaceAngleYaw = (o->oTimer & 0x1FF) * 0x80;
            break;
        case LAB_MOVER_BRIDGE:
            o->oFaceAngleRoll = 0x700 * sins(phase);
            o->oAngleVelRoll = (s16) (o->oFaceAngleRoll - oldRoll);
            break;
        case LAB_MOVER_LIFT:
            // Placed at its low stop. A smooth reversal tests ceiling contact.
            o->oPosY = o->oHomeY + 340.0f * (1.0f - coss(phase));
            break;
        case LAB_MOVER_WALL_LEFT:
        case LAB_MOVER_WALL_RIGHT: {
            f32 distance = 350.0f + 250.0f * coss(phase);
            // Both objects have the same home center and synchronized timers.
            // The passage varies from 1100 to 100 units and stays open at ends.
            o->oPosX = o->oHomeX + (o->oBehParams2ndByte == LAB_MOVER_WALL_LEFT ? -distance : distance);
            break;
        }
    }
    o->oVelX = o->oPosX - oldX;
    o->oVelY = o->oPosY - oldY;
}
