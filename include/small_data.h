#ifndef SMALL_DATA_H
#define SMALL_DATA_H

// Selected permanent definitions join the engine's boot-loaded GP region.
#if ENGINE_G16
#define GP_DATA(name) __attribute__((section(".sdata." #name)))
#define GP_BSS(name) __attribute__((section(".sbss." #name)))
#else
#define GP_DATA(name)
#define GP_BSS(name)
#endif

// Segmented assets must keep ordinary addresses even when the engine uses -G16.
// Only engine declarations receive this attribute; asset definitions keep their sections.
#ifdef ENGINE_SMALL_DATA
#define SEGMENTED_DATA __attribute__((section(".rodata")))
#else
#define SEGMENTED_DATA
#endif

#endif // SMALL_DATA_H
