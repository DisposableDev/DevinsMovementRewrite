#ifndef PAUSE_EASTER_EGG_H
#define PAUSE_EASTER_EGG_H

// Game-thread calls. Begin cancels the shared quote/cake request and rolls once per pause.
void pause_easter_egg_begin(void);
// Cancel the opportunity and stop the shared stream on resume/menu exit/warp.
void pause_easter_egg_end(void);
// While paused, wait for opening Start release, then measure idle time in OS cycles.
// Ordinary input cancels a pending opportunity; it does not stop an accepted quote.
void pause_easter_egg_update(void);

#endif
