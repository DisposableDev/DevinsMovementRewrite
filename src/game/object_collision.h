#ifndef OBJECT_COLLISION_H
#define OBJECT_COLLISION_H

// Rebuild contacts for participating object lists before remaining object updates.
// Also decrement positive intangibility timers, except for dormant carousel objects.
void detect_object_collisions(void);

#endif // OBJECT_COLLISION_H
