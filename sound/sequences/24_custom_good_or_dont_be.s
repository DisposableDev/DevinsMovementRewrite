#include "seq_macros.inc"
.section .rodata
.align 0
sequence_start:
seq_setmutebhv 0x60
seq_setmutescale 0
seq_setvol 127
seq_settempo 118
seq_initchannels 0xffff
seq_startchannel 0, channel_0
seq_startchannel 1, channel_1
seq_startchannel 2, channel_2
seq_startchannel 3, channel_3
seq_startchannel 4, channel_4
seq_startchannel 5, channel_5
seq_startchannel 6, channel_6
seq_delay 7296
sequence_repeat:
seq_delay 3072
seq_jump sequence_repeat
channel_0:
chan_largenoteson
chan_setinstr 4
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_0_0
chan_setlayer 1, layer_0_1
chan_setvol 92
chan_setpan 70
chan_setvolscale 127
chan_setreverb 94
chan_delay 7296
channel_0_repeat:
chan_setvol 92
chan_setpan 70
chan_setvolscale 127
chan_setreverb 94
chan_delay 3072
chan_jump channel_0_repeat
layer_0_0:
layer_transpose 0
layer_delay 432
layer_note0 53, 6, 56, 0
layer_delay 21
layer_note0 49, 12, 85, 0
layer_note0 49, 84, 99, 0
layer_note0 49, 24, 85, 0
layer_note0 56, 36, 106, 0
layer_note0 56, 36, 99, 0
layer_note0 49, 96, 85, 0
layer_note0 49, 24, 93, 0
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_delay 24
layer_note0 56, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_delay 24
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_note0 58, 24, 92, 0
layer_note0 51, 72, 99, 0
layer_delay 2
layer_note0 51, 12, 75, 0
layer_delay 10
layer_note0 54, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_note0 53, 24, 92, 0
layer_note0 53, 72, 99, 0
layer_delay 2
layer_note0 54, 12, 75, 0
layer_delay 10
layer_note0 59, 36, 106, 0
layer_note0 58, 36, 99, 0
layer_note0 56, 24, 92, 0
layer_note0 54, 72, 99, 0
layer_delay 1
layer_note0 58, 23, 99, 0
layer_note0 58, 36, 103, 0
layer_note0 56, 36, 99, 0
layer_note0 55, 24, 89, 0
layer_note0 56, 48, 99, 0
layer_note0 51, 48, 89, 0
layer_note0 51, 72, 99, 0
layer_note0 53, 23, 89, 0
layer_note0 53, 94, 99, 0
layer_delay 52
layer_note0 54, 24, 99, 0
layer_delay 11
layer_note0 49, 84, 71, 0
layer_note0 49, 24, 85, 0
layer_note0 56, 36, 106, 0
layer_note0 56, 36, 99, 0
layer_note0 49, 96, 95, 0
layer_note0 49, 24, 93, 0
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_delay 24
layer_note0 56, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_delay 24
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_note0 58, 24, 92, 0
layer_note0 51, 72, 99, 0
layer_delay 2
layer_note0 51, 12, 75, 0
layer_delay 10
layer_note0 54, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_note0 53, 24, 92, 0
layer_note0 53, 72, 99, 0
layer_delay 2
layer_note0 54, 12, 75, 0
layer_delay 10
layer_note0 59, 36, 106, 0
layer_note0 58, 36, 99, 0
layer_note0 56, 24, 92, 0
layer_note0 54, 72, 99, 0
layer_delay 2
layer_note0 58, 22, 99, 0
layer_note0 58, 36, 103, 0
layer_note0 56, 36, 99, 0
layer_note0 55, 24, 94, 0
layer_note0 56, 36, 106, 0
layer_note0 58, 36, 99, 0
layer_note0 59, 24, 93, 0
layer_note0 58, 96, 99, 0
layer_delay 1
layer_note0 53, 48, 80, 0
layer_note0 50, 48, 76, 0
layer_delay 1441
layer_note0 50, 91, 92, 0
layer_delay 4
layer_note0 51, 47, 101, 0
layer_delay 1
layer_note0 54, 46, 101, 0
layer_delay 4
layer_note0 56, 36, 93, 0
layer_delay 33
layer_note0 56, 24, 93, 0
layer_delay 1
layer_note0 56, 23, 101, 0
layer_delay 1
layer_note0 56, 23, 93, 0
layer_delay 1
layer_note0 61, 23, 101, 0
layer_delay 1
layer_note0 59, 23, 93, 0
layer_note0 58, 11, 101, 0
layer_delay 1
layer_note0 56, 23, 93, 0
layer_delay 1
layer_note0 58, 58, 93, 0
layer_delay 3
layer_note0 58, 46, 101, 0
layer_delay 2
layer_note0 61, 46, 101, 0
layer_delay 2
layer_note0 59, 48, 93, 0
layer_delay 24
layer_note0 54, 23, 93, 0
layer_delay 1
layer_note0 53, 23, 101, 0
layer_delay 1
layer_note0 53, 23, 93, 0
layer_delay 1
layer_note0 51, 23, 101, 0
layer_delay 1
layer_note0 53, 23, 93, 0
layer_delay 1
layer_note0 56, 11, 101, 0
layer_delay 1
layer_note0 54, 23, 93, 0
layer_delay 1
layer_note0 54, 11, 93, 0
layer_delay 1
layer_note0 54, 23, 93, 0
layer_delay 26
layer_note0 51, 46, 101, 0
layer_delay 1
layer_note0 54, 46, 101, 0
layer_delay 4
layer_note0 56, 36, 101, 0
layer_delay 33
layer_note0 56, 24, 93, 0
layer_delay 1
layer_note0 56, 23, 101, 0
layer_delay 1
layer_note0 56, 23, 93, 0
layer_delay 1
layer_note0 61, 23, 101, 0
layer_delay 1
layer_note0 59, 23, 93, 0
layer_note0 58, 11, 101, 0
layer_delay 1
layer_note0 56, 23, 93, 0
layer_delay 1
layer_note0 58, 58, 93, 0
layer_delay 3
layer_note0 58, 46, 101, 0
layer_delay 2
layer_note0 61, 46, 101, 0
layer_delay 2
layer_note0 59, 48, 93, 0
layer_delay 24
layer_note0 54, 23, 93, 0
layer_delay 21
layer_note0 61, 22, 93, 0
layer_delay 2
layer_note0 58, 22, 101, 0
layer_delay 2
layer_note0 61, 22, 93, 0
layer_delay 2
layer_note0 61, 33, 101, 0
layer_delay 3
layer_note0 63, 45, 93, 0
layer_delay 20
layer_note0 51, 46, 106, 0
layer_delay 1
layer_note0 54, 46, 106, 0
layer_delay 4
layer_note0 56, 36, 106, 0
layer_delay 33
layer_note0 56, 24, 99, 0
layer_delay 1
layer_note0 56, 23, 106, 0
layer_delay 1
layer_note0 56, 23, 99, 0
layer_delay 1
layer_note0 61, 23, 106, 0
layer_delay 1
layer_note0 59, 23, 99, 0
layer_note0 58, 11, 106, 0
layer_delay 1
layer_note0 56, 23, 99, 0
layer_delay 1
layer_note0 58, 58, 99, 0
layer_delay 3
layer_note0 58, 46, 106, 0
layer_delay 2
layer_note0 61, 46, 106, 0
layer_delay 2
layer_note0 59, 48, 99, 0
layer_delay 24
layer_note0 54, 23, 106, 0
layer_delay 21
layer_note0 61, 22, 106, 0
layer_delay 2
layer_note0 58, 22, 106, 0
layer_delay 2
layer_note0 61, 22, 106, 0
layer_delay 7
layer_note0 61, 33, 106, 0
layer_delay 15
layer_note0 63, 32, 106, 0
layer_delay 11
layer_0_0_repeat:
layer_delay 52
layer_note0 54, 24, 99, 0
layer_delay 11
layer_note0 49, 84, 71, 0
layer_note0 49, 24, 85, 0
layer_note0 56, 36, 106, 0
layer_note0 56, 36, 99, 0
layer_note0 49, 96, 95, 0
layer_note0 49, 24, 93, 0
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_delay 24
layer_note0 56, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_delay 24
layer_note0 58, 36, 106, 0
layer_note0 59, 36, 99, 0
layer_note0 58, 24, 92, 0
layer_note0 51, 72, 99, 0
layer_delay 2
layer_note0 51, 12, 75, 0
layer_delay 10
layer_note0 54, 36, 106, 0
layer_note0 54, 36, 99, 0
layer_note0 53, 24, 92, 0
layer_note0 53, 72, 99, 0
layer_delay 2
layer_note0 54, 12, 75, 0
layer_delay 10
layer_note0 59, 36, 106, 0
layer_note0 58, 36, 99, 0
layer_note0 56, 24, 92, 0
layer_note0 54, 72, 99, 0
layer_delay 2
layer_note0 58, 22, 99, 0
layer_note0 58, 36, 103, 0
layer_note0 56, 36, 99, 0
layer_note0 55, 24, 94, 0
layer_note0 56, 36, 106, 0
layer_note0 58, 36, 99, 0
layer_note0 59, 24, 93, 0
layer_note0 58, 96, 99, 0
layer_delay 1
layer_note0 53, 48, 80, 0
layer_note0 50, 48, 76, 0
layer_delay 1441
layer_note0 50, 91, 92, 0
layer_jump layer_0_0_repeat
layer_0_1:
layer_transpose 0
layer_delay 436
layer_note0 54, 24, 99, 0
layer_delay 381
layer_note0 58, 24, 89, 0
layer_delay 72
layer_note0 56, 24, 85, 0
layer_delay 182
layer_note0 53, 12, 85, 0
layer_delay 180
layer_note0 56, 12, 85, 0
layer_delay 165
layer_note0 56, 6, 56, 0
layer_delay 477
layer_note0 49, 12, 56, 0
layer_delay 370
layer_note0 58, 24, 93, 0
layer_delay 72
layer_note0 56, 24, 93, 0
layer_delay 182
layer_note0 53, 12, 85, 0
layer_delay 180
layer_note0 56, 12, 85, 0
layer_delay 433
layer_note0 53, 24, 85, 0
layer_delay 1787
layer_note0 54, 36, 93, 0
layer_delay 314
layer_note0 63, 34, 93, 0
layer_delay 1
layer_note0 56, 34, 93, 0
layer_delay 349
layer_note0 54, 36, 93, 0
layer_delay 314
layer_note0 63, 34, 101, 0
layer_delay 1
layer_note0 56, 34, 93, 0
layer_delay 22
layer_note0 61, 22, 101, 0
layer_delay 305
layer_note0 54, 36, 99, 0
layer_delay 314
layer_note0 63, 34, 106, 0
layer_delay 1
layer_note0 56, 34, 99, 0
layer_delay 22
layer_note0 61, 22, 106, 0
layer_delay 170
layer_0_1_repeat:
layer_delay 75
layer_note0 49, 12, 56, 0
layer_delay 370
layer_note0 58, 24, 93, 0
layer_delay 72
layer_note0 56, 24, 93, 0
layer_delay 182
layer_note0 53, 12, 85, 0
layer_delay 180
layer_note0 56, 12, 85, 0
layer_delay 433
layer_note0 53, 24, 85, 0
layer_delay 1652
layer_jump layer_0_1_repeat
channel_1:
chan_largenoteson
chan_setinstr 4
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_1_0
chan_setlayer 1, layer_1_1
chan_setlayer 2, layer_1_2
chan_setlayer 3, layer_1_3
chan_setvol 80
chan_setpan 48
chan_setvolscale 127
chan_setreverb 29
chan_delay 7296
channel_1_repeat:
chan_setvol 80
chan_setpan 48
chan_setvolscale 127
chan_setreverb 29
chan_delay 3072
chan_jump channel_1_repeat
layer_1_0:
layer_transpose 0
layer_delay 1972
layer_note0 42, 21, 70, 0
layer_delay 2
layer_note0 37, 9, 48, 0
layer_delay 3
layer_note0 37, 81, 61, 0
layer_delay 3
layer_note0 37, 21, 58, 0
layer_delay 3
layer_note0 44, 33, 82, 0
layer_delay 3
layer_note0 44, 34, 70, 0
layer_delay 2
layer_note0 37, 93, 67, 0
layer_delay 3
layer_note0 37, 21, 65, 0
layer_delay 3
layer_note0 46, 33, 78, 0
layer_delay 3
layer_note0 47, 34, 70, 0
layer_delay 1
layer_note0 46, 21, 65, 0
layer_delay 4
layer_note0 44, 33, 78, 0
layer_delay 3
layer_note0 42, 34, 70, 0
layer_note0 44, 21, 65, 0
layer_delay 5
layer_note0 46, 33, 78, 0
layer_delay 3
layer_note0 47, 34, 70, 0
layer_delay 2
layer_note0 46, 21, 64, 0
layer_delay 3
layer_note0 39, 70, 70, 0
layer_delay 4
layer_note0 39, 9, 49, 0
layer_delay 1
layer_note0 41, 9, 58, 0
layer_delay 3
layer_note0 42, 33, 76, 0
layer_delay 3
layer_note0 42, 34, 70, 0
layer_delay 2
layer_note0 41, 21, 64, 0
layer_delay 3
layer_note0 41, 70, 70, 0
layer_delay 4
layer_note0 42, 9, 49, 0
layer_delay 1
layer_note0 44, 9, 58, 0
layer_delay 3
layer_note0 47, 33, 78, 0
layer_delay 3
layer_note0 46, 34, 70, 0
layer_delay 2
layer_note0 44, 21, 64, 0
layer_delay 3
layer_note0 42, 70, 70, 0
layer_note0 44, 4, 34, 0
layer_delay 1
layer_note0 46, 18, 70, 0
layer_delay 3
layer_note0 46, 33, 73, 0
layer_delay 3
layer_note0 44, 34, 70, 0
layer_delay 2
layer_note0 43, 21, 66, 0
layer_delay 3
layer_note0 44, 33, 78, 0
layer_delay 3
layer_note0 46, 34, 70, 0
layer_delay 2
layer_note0 47, 21, 65, 0
layer_delay 3
layer_note0 46, 45, 73, 0
layer_delay 3
layer_note0 46, 45, 73, 0
layer_delay 2
layer_note0 34, 45, 36, 0
layer_delay 4
layer_note0 41, 45, 65, 0
layer_delay 1
layer_note0 23, 46, 69, 0
layer_delay 4
layer_note0 11, 94, 69, 0
layer_delay 2
layer_note0 23, 45, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 3
layer_note0 11, 94, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 1
layer_note0 34, 35, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 14
layer_note0 34, 22, 65, 0
layer_delay 1
layer_note0 29, 46, 69, 0
layer_delay 3
layer_note0 22, 46, 69, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 15, 94, 69, 0
layer_delay 2
layer_note0 27, 46, 69, 0
layer_delay 1
layer_note0 35, 35, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 21, 65, 0
layer_delay 1
layer_note0 32, 46, 69, 0
layer_delay 4
layer_note0 20, 46, 69, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 10, 94, 69, 0
layer_delay 2
layer_note0 34, 10, 65, 0
layer_delay 14
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 39, 35, 65, 0
layer_delay 2
layer_note0 39, 22, 65, 0
layer_delay 14
layer_note0 39, 21, 65, 0
layer_delay 1
layer_note0 35, 46, 65, 0
layer_delay 4
layer_note0 23, 46, 69, 0
layer_delay 1
layer_note0 34, 45, 67, 0
layer_delay 3
layer_note0 34, 45, 73, 0
layer_delay 3
layer_note0 34, 90, 79, 0
layer_delay 7
layer_note0 39, 45, 80, 0
layer_delay 3
layer_note0 39, 43, 80, 0
layer_delay 4
layer_note0 44, 43, 80, 0
layer_delay 6
layer_note0 44, 44, 80, 0
layer_delay 2
layer_note0 37, 43, 68, 0
layer_delay 7
layer_note0 37, 40, 68, 0
layer_delay 3
layer_note0 37, 4, 68, 0
layer_note0 49, 8, 80, 0
layer_delay 41
layer_note0 42, 44, 68, 0
layer_delay 4
layer_note0 39, 43, 80, 0
layer_delay 5
layer_note0 39, 39, 80, 0
layer_delay 3
layer_note0 39, 4, 80, 0
layer_note0 41, 4, 80, 0
layer_delay 48
layer_note0 39, 21, 80, 0
layer_delay 4
layer_note0 35, 20, 80, 0
layer_note0 41, 88, 80, 0
layer_delay 8
layer_note0 37, 42, 68, 0
layer_delay 5
layer_note0 39, 42, 68, 0
layer_delay 8
layer_note0 39, 44, 80, 0
layer_delay 3
layer_note0 39, 43, 80, 0
layer_delay 4
layer_note0 44, 43, 80, 0
layer_delay 7
layer_note0 44, 44, 80, 0
layer_delay 2
layer_note0 37, 43, 68, 0
layer_delay 7
layer_note0 37, 40, 68, 0
layer_delay 3
layer_note0 37, 4, 68, 0
layer_note0 49, 8, 80, 0
layer_delay 41
layer_note0 42, 44, 68, 0
layer_delay 4
layer_note0 39, 43, 80, 0
layer_delay 5
layer_note0 39, 39, 80, 0
layer_delay 3
layer_note0 39, 4, 80, 0
layer_note0 41, 4, 80, 0
layer_delay 96
layer_note0 37, 89, 80, 0
layer_delay 3
layer_note0 53, 43, 80, 0
layer_delay 9
layer_note0 39, 43, 80, 0
layer_delay 3
layer_note0 39, 44, 85, 0
layer_delay 3
layer_note0 39, 43, 85, 0
layer_delay 4
layer_note0 44, 43, 85, 0
layer_delay 7
layer_note0 44, 44, 85, 0
layer_delay 2
layer_note0 37, 43, 73, 0
layer_delay 7
layer_note0 37, 40, 73, 0
layer_delay 3
layer_note0 37, 4, 73, 0
layer_note0 49, 8, 85, 0
layer_delay 41
layer_note0 42, 44, 73, 0
layer_delay 4
layer_note0 39, 43, 85, 0
layer_delay 5
layer_note0 39, 39, 85, 0
layer_delay 3
layer_note0 39, 4, 85, 0
layer_note0 41, 4, 85, 0
layer_delay 48
layer_note0 39, 21, 85, 0
layer_delay 4
layer_note0 35, 20, 85, 0
layer_delay 3
layer_note0 37, 89, 85, 0
layer_delay 3
layer_note0 37, 43, 85, 0
layer_delay 5
layer_note0 39, 29, 85, 0
layer_delay 18
layer_1_0_repeat:
layer_delay 52
layer_note0 42, 21, 70, 0
layer_delay 2
layer_note0 37, 9, 48, 0
layer_delay 3
layer_note0 37, 81, 61, 0
layer_delay 3
layer_note0 37, 21, 58, 0
layer_delay 3
layer_note0 44, 33, 82, 0
layer_delay 3
layer_note0 44, 34, 70, 0
layer_delay 2
layer_note0 37, 93, 67, 0
layer_delay 3
layer_note0 37, 21, 65, 0
layer_delay 3
layer_note0 46, 33, 78, 0
layer_delay 3
layer_note0 47, 34, 70, 0
layer_delay 1
layer_note0 46, 21, 65, 0
layer_delay 4
layer_note0 44, 33, 78, 0
layer_delay 3
layer_note0 42, 34, 70, 0
layer_note0 44, 21, 65, 0
layer_delay 5
layer_note0 46, 33, 78, 0
layer_delay 3
layer_note0 47, 34, 70, 0
layer_delay 2
layer_note0 46, 21, 64, 0
layer_delay 3
layer_note0 39, 70, 70, 0
layer_delay 4
layer_note0 39, 9, 49, 0
layer_delay 1
layer_note0 41, 9, 58, 0
layer_delay 3
layer_note0 42, 33, 76, 0
layer_delay 3
layer_note0 42, 34, 70, 0
layer_delay 2
layer_note0 41, 21, 64, 0
layer_delay 3
layer_note0 41, 70, 70, 0
layer_delay 4
layer_note0 42, 9, 49, 0
layer_delay 1
layer_note0 44, 9, 58, 0
layer_delay 3
layer_note0 47, 33, 78, 0
layer_delay 3
layer_note0 46, 34, 70, 0
layer_delay 2
layer_note0 44, 21, 64, 0
layer_delay 3
layer_note0 42, 70, 70, 0
layer_note0 44, 4, 34, 0
layer_delay 1
layer_note0 46, 18, 70, 0
layer_delay 3
layer_note0 46, 33, 73, 0
layer_delay 3
layer_note0 44, 34, 70, 0
layer_delay 2
layer_note0 43, 21, 66, 0
layer_delay 3
layer_note0 44, 33, 78, 0
layer_delay 3
layer_note0 46, 34, 70, 0
layer_delay 2
layer_note0 47, 21, 65, 0
layer_delay 3
layer_note0 46, 45, 73, 0
layer_delay 3
layer_note0 46, 45, 73, 0
layer_delay 2
layer_note0 34, 45, 36, 0
layer_delay 4
layer_note0 41, 45, 65, 0
layer_delay 1
layer_note0 23, 46, 69, 0
layer_delay 4
layer_note0 11, 94, 69, 0
layer_delay 2
layer_note0 23, 45, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 3
layer_note0 11, 94, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 1
layer_note0 34, 35, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 14
layer_note0 34, 22, 65, 0
layer_delay 1
layer_note0 29, 46, 69, 0
layer_delay 3
layer_note0 22, 46, 69, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 15, 94, 69, 0
layer_delay 2
layer_note0 27, 46, 69, 0
layer_delay 1
layer_note0 35, 35, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 21, 65, 0
layer_delay 1
layer_note0 32, 46, 69, 0
layer_delay 4
layer_note0 20, 46, 69, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 10, 94, 69, 0
layer_delay 2
layer_note0 34, 10, 65, 0
layer_delay 14
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 39, 35, 65, 0
layer_delay 2
layer_note0 39, 22, 65, 0
layer_delay 14
layer_note0 39, 21, 65, 0
layer_delay 1
layer_note0 35, 46, 65, 0
layer_delay 4
layer_note0 23, 46, 69, 0
layer_delay 1
layer_note0 34, 45, 67, 0
layer_delay 3
layer_note0 34, 45, 73, 0
layer_delay 3
layer_note0 34, 90, 79, 0
layer_delay 4
layer_jump layer_1_0_repeat
layer_1_1:
layer_transpose 0
layer_delay 3050
layer_note0 56, 4, 48, 0
layer_delay 262
layer_note0 50, 45, 73, 0
layer_delay 2
layer_note0 46, 45, 69, 0
layer_delay 4
layer_note0 46, 45, 65, 0
layer_delay 1
layer_note0 35, 38, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 20, 65, 0
layer_delay 2
layer_note0 35, 36, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 22, 65, 0
layer_delay 1
layer_note0 41, 45, 65, 0
layer_delay 28
layer_note0 37, 19, 65, 0
layer_delay 1
layer_note0 32, 45, 69, 0
layer_delay 31
layer_note0 37, 19, 65, 0
layer_delay 2
layer_note0 22, 46, 69, 0
layer_delay 2
layer_note0 10, 94, 69, 0
layer_delay 26
layer_note0 34, 21, 65, 0
layer_delay 3
layer_note0 27, 46, 69, 0
layer_delay 26
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 27, 46, 69, 0
layer_delay 27
layer_note0 34, 22, 65, 0
layer_delay 2
layer_note0 20, 46, 69, 0
layer_delay 2
layer_note0 8, 94, 69, 0
layer_delay 26
layer_note0 35, 22, 65, 0
layer_delay 2
layer_note0 37, 34, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 14
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 22, 46, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 2
layer_note0 11, 94, 69, 0
layer_delay 26
layer_note0 39, 22, 65, 0
layer_delay 2
layer_note0 38, 45, 67, 0
layer_delay 3
layer_note0 10, 44, 77, 0
layer_delay 4
layer_note0 38, 90, 79, 0
layer_delay 7
layer_note0 42, 45, 80, 0
layer_delay 3
layer_note0 46, 43, 80, 0
layer_delay 4
layer_note0 47, 45, 80, 0
layer_delay 51
layer_note0 49, 20, 80, 0
layer_delay 3
layer_note0 49, 20, 80, 0
layer_delay 51
layer_note0 39, 4, 68, 0
layer_delay 94
layer_note0 51, 43, 80, 0
layer_delay 5
layer_note0 51, 43, 80, 0
layer_delay 3
layer_note0 51, 45, 80, 0
layer_delay 51
layer_note0 37, 88, 80, 0
layer_delay 10
layer_note0 41, 45, 56, 0
layer_delay 2
layer_note0 42, 45, 56, 0
layer_delay 5
layer_note0 42, 45, 80, 0
layer_delay 2
layer_note0 46, 43, 80, 0
layer_delay 5
layer_note0 47, 45, 80, 0
layer_delay 51
layer_note0 49, 20, 80, 0
layer_delay 3
layer_note0 49, 20, 80, 0
layer_delay 51
layer_note0 39, 4, 68, 0
layer_delay 94
layer_note0 51, 43, 80, 0
layer_delay 5
layer_note0 51, 43, 80, 0
layer_delay 3
layer_note0 51, 45, 80, 0
layer_delay 149
layer_note0 56, 43, 80, 0
layer_delay 54
layer_note0 42, 45, 85, 0
layer_delay 2
layer_note0 46, 43, 85, 0
layer_delay 5
layer_note0 47, 45, 85, 0
layer_delay 51
layer_note0 49, 20, 85, 0
layer_delay 3
layer_note0 49, 20, 85, 0
layer_delay 51
layer_note0 39, 4, 73, 0
layer_delay 94
layer_note0 51, 43, 85, 0
layer_delay 5
layer_note0 51, 43, 85, 0
layer_delay 3
layer_note0 51, 45, 85, 0
layer_delay 148
layer_note0 49, 41, 85, 0
layer_delay 7
layer_note0 51, 29, 85, 0
layer_delay 17
layer_1_1_repeat:
layer_delay 1130
layer_note0 56, 4, 48, 0
layer_delay 262
layer_note0 50, 45, 73, 0
layer_delay 2
layer_note0 46, 45, 69, 0
layer_delay 4
layer_note0 46, 45, 65, 0
layer_delay 1
layer_note0 35, 38, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 20, 65, 0
layer_delay 2
layer_note0 35, 36, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 14
layer_note0 35, 22, 65, 0
layer_delay 1
layer_note0 41, 45, 65, 0
layer_delay 28
layer_note0 37, 19, 65, 0
layer_delay 1
layer_note0 32, 45, 69, 0
layer_delay 31
layer_note0 37, 19, 65, 0
layer_delay 2
layer_note0 22, 46, 69, 0
layer_delay 2
layer_note0 10, 94, 69, 0
layer_delay 26
layer_note0 34, 21, 65, 0
layer_delay 3
layer_note0 27, 46, 69, 0
layer_delay 26
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 27, 46, 69, 0
layer_delay 27
layer_note0 34, 22, 65, 0
layer_delay 2
layer_note0 20, 46, 69, 0
layer_delay 2
layer_note0 8, 94, 69, 0
layer_delay 26
layer_note0 35, 22, 65, 0
layer_delay 2
layer_note0 37, 34, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 14
layer_note0 37, 22, 65, 0
layer_delay 1
layer_note0 34, 46, 65, 0
layer_delay 3
layer_note0 22, 46, 69, 0
layer_delay 2
layer_note0 23, 46, 69, 0
layer_delay 2
layer_note0 11, 94, 69, 0
layer_delay 26
layer_note0 39, 22, 65, 0
layer_delay 2
layer_note0 38, 45, 67, 0
layer_delay 3
layer_note0 10, 44, 77, 0
layer_delay 4
layer_note0 38, 90, 79, 0
layer_delay 3
layer_jump layer_1_1_repeat
layer_1_2:
layer_transpose 0
layer_delay 3364
layer_note0 50, 45, 69, 0
layer_delay 26
layer_note0 34, 22, 65, 0
layer_delay 2
layer_note0 30, 45, 69, 0
layer_delay 50
layer_note0 30, 45, 69, 0
layer_delay 52
layer_note0 37, 37, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 35
layer_note0 37, 37, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 35
layer_note0 37, 46, 65, 0
layer_delay 50
layer_note0 34, 34, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 39
layer_note0 37, 33, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 38
layer_note0 31, 46, 69, 0
layer_delay 50
layer_note0 39, 46, 65, 0
layer_delay 49
layer_note0 35, 35, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 38
layer_note0 22, 46, 69, 0
layer_delay 50
layer_note0 37, 34, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 36
layer_note0 35, 46, 65, 0
layer_delay 51
layer_note0 39, 35, 65, 0
layer_delay 2
layer_note0 39, 22, 65, 0
layer_delay 36
layer_note0 22, 44, 71, 0
layer_delay 6
layer_note0 38, 45, 73, 0
layer_delay 4
layer_note0 22, 43, 83, 0
layer_delay 54
layer_note0 46, 44, 80, 0
layer_delay 3
layer_note0 51, 43, 80, 0
layer_delay 6
layer_note0 51, 45, 80, 0
layer_delay 50
layer_note0 53, 20, 80, 0
layer_delay 3
layer_note0 53, 20, 80, 0
layer_delay 52
layer_note0 54, 8, 80, 0
layer_delay 89
layer_note0 54, 43, 80, 0
layer_delay 5
layer_note0 58, 43, 80, 0
layer_delay 4
layer_note0 42, 42, 80, 0
layer_delay 54
layer_note0 44, 88, 80, 0
layer_delay 9
layer_note0 44, 45, 56, 0
layer_delay 2
layer_note0 46, 45, 56, 0
layer_delay 5
layer_note0 46, 43, 80, 0
layer_delay 4
layer_note0 51, 43, 80, 0
layer_delay 6
layer_note0 51, 45, 80, 0
layer_delay 50
layer_note0 53, 20, 80, 0
layer_delay 3
layer_note0 53, 20, 80, 0
layer_delay 52
layer_note0 54, 8, 80, 0
layer_delay 89
layer_note0 54, 43, 80, 0
layer_delay 5
layer_note0 58, 43, 80, 0
layer_delay 4
layer_note0 42, 42, 80, 0
layer_delay 153
layer_note0 37, 43, 80, 0
layer_delay 52
layer_note0 46, 43, 85, 0
layer_delay 4
layer_note0 51, 43, 85, 0
layer_delay 6
layer_note0 51, 45, 85, 0
layer_delay 50
layer_note0 53, 20, 85, 0
layer_delay 3
layer_note0 53, 20, 85, 0
layer_delay 52
layer_note0 54, 8, 85, 0
layer_delay 89
layer_note0 54, 43, 85, 0
layer_delay 5
layer_note0 58, 43, 85, 0
layer_delay 4
layer_note0 42, 42, 85, 0
layer_delay 150
layer_note0 41, 41, 85, 0
layer_delay 7
layer_note0 42, 29, 85, 0
layer_delay 16
layer_1_2_repeat:
layer_delay 1444
layer_note0 50, 45, 69, 0
layer_delay 26
layer_note0 34, 22, 65, 0
layer_delay 2
layer_note0 30, 45, 69, 0
layer_delay 50
layer_note0 30, 45, 69, 0
layer_delay 52
layer_note0 37, 37, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 35
layer_note0 37, 37, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 35
layer_note0 37, 46, 65, 0
layer_delay 50
layer_note0 34, 34, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 39
layer_note0 37, 33, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 38
layer_note0 31, 46, 69, 0
layer_delay 50
layer_note0 39, 46, 65, 0
layer_delay 49
layer_note0 35, 35, 65, 0
layer_delay 2
layer_note0 35, 22, 65, 0
layer_delay 38
layer_note0 22, 46, 69, 0
layer_delay 50
layer_note0 37, 34, 65, 0
layer_delay 2
layer_note0 37, 22, 65, 0
layer_delay 36
layer_note0 35, 46, 65, 0
layer_delay 51
layer_note0 39, 35, 65, 0
layer_delay 2
layer_note0 39, 22, 65, 0
layer_delay 36
layer_note0 22, 44, 71, 0
layer_delay 6
layer_note0 38, 45, 73, 0
layer_delay 4
layer_note0 22, 43, 83, 0
layer_delay 49
layer_jump layer_1_2_repeat
layer_1_3:
layer_transpose 0
layer_delay 3386
layer_note0 29, 47, 74, 0
layer_delay 27
layer_note0 39, 45, 65, 0
layer_delay 51
layer_note0 39, 45, 65, 0
layer_delay 51
layer_note0 32, 45, 69, 0
layer_delay 51
layer_note0 41, 45, 65, 0
layer_delay 51
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 37, 46, 65, 0
layer_delay 50
layer_note0 31, 46, 69, 0
layer_delay 50
layer_note0 34, 33, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 39
layer_note0 32, 46, 69, 0
layer_delay 49
layer_note0 39, 46, 65, 0
layer_delay 51
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 41, 46, 65, 0
layer_delay 49
layer_note0 42, 46, 65, 0
layer_delay 50
layer_note0 42, 46, 65, 0
layer_delay 51
layer_note0 41, 45, 67, 0
layer_delay 3
layer_note0 41, 45, 73, 0
layer_delay 3
layer_note0 41, 90, 79, 0
layer_delay 199
layer_note0 44, 20, 80, 0
layer_delay 75
layer_note0 42, 43, 80, 0
layer_delay 54
layer_note0 46, 43, 80, 0
layer_delay 5
layer_note0 46, 43, 80, 0
layer_delay 3
layer_note0 54, 45, 80, 0
layer_delay 51
layer_note0 49, 88, 80, 0
layer_delay 10
layer_note0 49, 45, 56, 0
layer_delay 2
layer_note0 51, 45, 56, 0
layer_delay 196
layer_note0 44, 20, 80, 0
layer_delay 75
layer_note0 42, 43, 80, 0
layer_delay 54
layer_note0 46, 43, 80, 0
layer_delay 5
layer_note0 46, 43, 80, 0
layer_delay 3
layer_note0 54, 45, 80, 0
layer_delay 437
layer_note0 44, 20, 85, 0
layer_delay 75
layer_note0 42, 43, 85, 0
layer_delay 54
layer_note0 46, 43, 85, 0
layer_delay 5
layer_note0 46, 43, 85, 0
layer_delay 3
layer_note0 54, 45, 85, 0
layer_delay 148
layer_note0 56, 43, 85, 0
layer_delay 5
layer_note0 58, 29, 85, 0
layer_delay 15
layer_1_3_repeat:
layer_delay 1466
layer_note0 29, 47, 74, 0
layer_delay 27
layer_note0 39, 45, 65, 0
layer_delay 51
layer_note0 39, 45, 65, 0
layer_delay 51
layer_note0 32, 45, 69, 0
layer_delay 51
layer_note0 41, 45, 65, 0
layer_delay 51
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 37, 46, 65, 0
layer_delay 50
layer_note0 31, 46, 69, 0
layer_delay 50
layer_note0 34, 33, 65, 0
layer_delay 2
layer_note0 34, 22, 65, 0
layer_delay 39
layer_note0 32, 46, 69, 0
layer_delay 49
layer_note0 39, 46, 65, 0
layer_delay 51
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 41, 46, 65, 0
layer_delay 49
layer_note0 42, 46, 65, 0
layer_delay 50
layer_note0 42, 46, 65, 0
layer_delay 51
layer_note0 41, 45, 67, 0
layer_delay 3
layer_note0 41, 45, 73, 0
layer_delay 3
layer_note0 41, 90, 79, 0
layer_delay 2
layer_jump layer_1_3_repeat
channel_2:
chan_largenoteson
chan_setinstr 4
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_2_0
chan_setlayer 1, layer_2_1
chan_setvol 80
chan_setpan 48
chan_setvolscale 127
chan_setreverb 29
chan_delay 7296
channel_2_repeat:
chan_setvol 80
chan_setpan 48
chan_setvolscale 127
chan_setreverb 29
chan_delay 3072
chan_jump channel_2_repeat
layer_2_0:
layer_transpose 0
layer_delay 3461
layer_note0 42, 45, 65, 0
layer_delay 148
layer_note0 44, 45, 65, 0
layer_delay 146
layer_note0 46, 46, 65, 0
layer_delay 50
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 44, 46, 65, 0
layer_delay 50
layer_note0 43, 46, 65, 0
layer_delay 49
layer_note0 42, 46, 65, 0
layer_delay 50
layer_note0 44, 46, 65, 0
layer_delay 51
layer_note0 46, 46, 65, 0
layer_delay 50
layer_note0 46, 46, 65, 0
layer_delay 49
layer_note0 47, 46, 65, 0
layer_delay 49
layer_note0 47, 46, 65, 0
layer_delay 51
layer_note0 46, 45, 67, 0
layer_delay 4
layer_note0 48, 45, 73, 0
layer_delay 3
layer_note0 46, 90, 79, 0
layer_delay 2214
layer_note0 44, 41, 85, 0
layer_delay 7
layer_note0 46, 29, 85, 0
layer_delay 14
layer_2_0_repeat:
layer_delay 1541
layer_note0 42, 45, 65, 0
layer_delay 148
layer_note0 44, 45, 65, 0
layer_delay 146
layer_note0 46, 46, 65, 0
layer_delay 50
layer_note0 41, 46, 65, 0
layer_delay 50
layer_note0 44, 46, 65, 0
layer_delay 50
layer_note0 43, 46, 65, 0
layer_delay 49
layer_note0 42, 46, 65, 0
layer_delay 50
layer_note0 44, 46, 65, 0
layer_delay 51
layer_note0 46, 46, 65, 0
layer_delay 50
layer_note0 46, 46, 65, 0
layer_delay 49
layer_note0 47, 46, 65, 0
layer_delay 49
layer_note0 47, 46, 65, 0
layer_delay 51
layer_note0 46, 45, 67, 0
layer_delay 4
layer_note0 48, 45, 73, 0
layer_delay 3
layer_note0 46, 90, 79, 0
layer_delay 1
layer_jump layer_2_0_repeat
layer_2_1:
layer_transpose 0
layer_delay 3462
layer_note0 47, 45, 65, 0
layer_delay 722
layer_note0 47, 46, 65, 0
layer_delay 50
layer_note0 47, 46, 65, 0
layer_delay 241
layer_note0 51, 46, 65, 0
layer_delay 50
layer_note0 51, 46, 65, 0
layer_delay 2542
layer_2_1_repeat:
layer_delay 1542
layer_note0 47, 45, 65, 0
layer_delay 722
layer_note0 47, 46, 65, 0
layer_delay 50
layer_note0 47, 46, 65, 0
layer_delay 241
layer_note0 51, 46, 65, 0
layer_delay 50
layer_note0 51, 46, 65, 0
layer_delay 238
layer_jump layer_2_1_repeat
channel_3:
chan_largenoteson
chan_setinstr 7
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_3_0
chan_setlayer 1, layer_3_1
chan_setlayer 2, layer_3_2
chan_setlayer 3, layer_3_3
chan_setvol 55
chan_setpan 57
chan_setvolscale 119
chan_setreverb 63
chan_delay 3350
chan_setvolscale 119
chan_delay 50
chan_setvolscale 116
chan_delay 35
chan_setvolscale 112
chan_delay 35
chan_setvolscale 107
chan_delay 50
chan_setvolscale 106
chan_delay 1320
chan_setvolscale 107
chan_delay 30
chan_setvolscale 111
chan_delay 40
chan_setvolscale 115
chan_delay 50
chan_setvolscale 119
chan_delay 32
chan_setvolscale 119
chan_delay 2304
channel_3_repeat:
chan_setvol 55
chan_setpan 57
chan_setvolscale 119
chan_setreverb 63
chan_delay 1430
chan_setvolscale 119
chan_delay 50
chan_setvolscale 116
chan_delay 35
chan_setvolscale 112
chan_delay 35
chan_setvolscale 107
chan_delay 50
chan_setvolscale 106
chan_delay 1320
chan_setvolscale 107
chan_delay 30
chan_setvolscale 111
chan_delay 40
chan_setvolscale 115
chan_delay 50
chan_setvolscale 119
chan_delay 32
chan_jump channel_3_repeat
layer_3_0:
layer_transpose 0
layer_note0 30, 48, 93, 0
layer_delay 2
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 37, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 24, 93, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 24, 93, 0
layer_delay 21
layer_note0 30, 48, 93, 0
layer_delay 2
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 37, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 24, 93, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 70, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 70, 0
layer_note0 37, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 39, 48, 98, 0
layer_note0 42, 24, 70, 0
layer_note0 39, 48, 90, 0
layer_note0 42, 24, 70, 0
layer_note0 39, 24, 94, 0
layer_note0 42, 24, 70, 0
layer_note0 35, 48, 98, 0
layer_note0 42, 24, 70, 0
layer_note0 35, 46, 98, 0
layer_delay 1
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 98, 0
layer_note0 42, 24, 70, 0
layer_note0 35, 24, 90, 0
layer_delay 21
layer_note0 23, 24, 84, 0
layer_delay 1
layer_note0 42, 24, 69, 0
layer_note0 29, 48, 98, 0
layer_note0 29, 24, 84, 0
layer_delay 1
layer_note0 44, 12, 69, 0
layer_delay 10
layer_note0 30, 24, 90, 0
layer_delay 24
layer_note0 30, 24, 84, 0
layer_delay 2
layer_note0 46, 24, 69, 0
layer_delay 1
layer_note0 34, 24, 98, 0
layer_delay 21
layer_note0 27, 24, 84, 0
layer_delay 2
layer_note0 42, 24, 69, 0
layer_delay 1
layer_note0 41, 24, 98, 0
layer_delay 23
layer_note0 48, 24, 64, 0
layer_delay 1
layer_note0 41, 24, 84, 0
layer_delay 23
layer_note0 48, 24, 63, 0
layer_delay 1
layer_note0 41, 24, 92, 0
layer_note0 44, 24, 62, 0
layer_note0 34, 24, 88, 0
layer_delay 21
layer_note0 22, 24, 89, 0
layer_delay 1
layer_note0 48, 24, 65, 0
layer_delay 1
layer_note0 41, 24, 70, 0
layer_delay 22
layer_note0 22, 24, 88, 0
layer_note0 46, 24, 69, 0
layer_note0 30, 48, 93, 0
layer_delay 2
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 37, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 24, 93, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 62, 0
layer_note0 37, 24, 94, 0
layer_note0 41, 24, 62, 0
layer_note0 39, 48, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 39, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 39, 24, 94, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 48, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 46, 98, 0
layer_delay 1
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 35, 24, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 24, 90, 0
layer_delay 21
layer_note0 23, 24, 84, 0
layer_delay 1
layer_note0 42, 24, 69, 0
layer_note0 29, 48, 98, 0
layer_note0 29, 24, 84, 0
layer_delay 1
layer_note0 41, 24, 69, 0
layer_delay 1
layer_note0 37, 24, 93, 0
layer_delay 21
layer_note0 30, 24, 84, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 84, 0
layer_note0 39, 24, 69, 0
layer_note0 34, 24, 98, 0
layer_delay 21
layer_note0 29, 24, 98, 0
layer_delay 24
layer_note0 48, 24, 70, 0
layer_note0 29, 24, 88, 0
layer_delay 26
layer_note0 44, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 79, 0
layer_note0 41, 24, 70, 0
layer_note0 34, 24, 98, 0
layer_delay 23
layer_note0 41, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 88, 0
layer_delay 23
layer_note0 41, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 92, 0
layer_note0 38, 24, 70, 0
layer_delay 39
layer_note0 38, 5, 62, 0
layer_delay 7
layer_note0 39, 24, 84, 0
layer_note0 42, 72, 89, 0
layer_delay 12
layer_note0 44, 36, 89, 0
layer_note0 44, 144, 89, 0
layer_delay 12
layer_note0 47, 36, 98, 0
layer_note0 49, 36, 98, 0
layer_note0 49, 48, 70, 0
layer_delay 36
layer_note0 46, 24, 98, 0
layer_note0 47, 24, 94, 0
layer_note0 49, 24, 89, 0
layer_delay 21
layer_note0 46, 12, 62, 0
layer_note0 46, 48, 70, 0
layer_delay 84
layer_note0 46, 24, 70, 0
layer_note0 47, 24, 88, 0
layer_note0 54, 24, 98, 0
layer_delay 12
layer_note0 47, 12, 80, 0
layer_delay 1
layer_note0 46, 12, 74, 0
layer_delay 1
layer_note0 44, 12, 68, 0
layer_delay 1
layer_note0 49, 36, 91, 0
layer_delay 8
layer_note0 46, 12, 80, 0
layer_delay 1
layer_note0 44, 12, 70, 0
layer_delay 1
layer_note0 42, 12, 64, 0
layer_delay 1
layer_note0 44, 12, 59, 0
layer_delay 1
layer_note0 46, 24, 98, 0
layer_delay 9
layer_note0 41, 24, 98, 0
layer_delay 12
layer_note0 41, 12, 98, 0
layer_note0 37, 48, 84, 0
layer_delay 36
layer_note0 37, 24, 98, 0
layer_note0 42, 24, 98, 0
layer_note0 39, 24, 84, 0
layer_note0 46, 24, 98, 0
layer_note0 44, 24, 84, 0
layer_note0 44, 24, 98, 0
layer_delay 48
layer_note0 44, 24, 98, 0
layer_note0 44, 48, 98, 0
layer_delay 93
layer_note0 34, 24, 62, 0
layer_delay 1
layer_note0 41, 23, 62, 0
layer_note0 27, 24, 99, 0
layer_delay 24
layer_note0 27, 24, 105, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 99, 0
layer_delay 21
layer_note0 23, 24, 94, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 99, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 70, 0
layer_note0 30, 24, 99, 0
layer_delay 24
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 39, 24, 99, 0
layer_delay 21
layer_note0 27, 24, 94, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 105, 0
layer_delay 21
layer_note0 23, 24, 94, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 105, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 70, 0
layer_note0 25, 24, 102, 0
layer_delay 2
layer_note0 41, 24, 70, 0
layer_delay 1
layer_note0 39, 25, 103, 0
layer_note0 39, 23, 70, 0
layer_note0 39, 1, 99, 0
layer_delay 1
layer_note0 42, 1, 99, 0
layer_note0 46, 1, 99, 0
layer_delay 41
layer_note0 27, 24, 105, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 99, 0
layer_delay 21
layer_note0 23, 24, 94, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 99, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 70, 0
layer_note0 30, 24, 99, 0
layer_delay 24
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 39, 24, 99, 0
layer_delay 21
layer_note0 27, 24, 105, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 99, 0
layer_delay 21
layer_note0 23, 24, 94, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 99, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 70, 0
layer_note0 25, 24, 99, 0
layer_note0 25, 18, 94, 0
layer_note0 25, 12, 70, 0
layer_note0 46, 24, 105, 0
layer_delay 18
layer_note0 27, 24, 99, 0
layer_delay 24
layer_note0 27, 24, 105, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 99, 0
layer_delay 21
layer_note0 23, 24, 94, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 99, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 70, 0
layer_note0 30, 24, 99, 0
layer_delay 24
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 39, 24, 99, 0
layer_delay 21
layer_note0 27, 24, 84, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 99, 0
layer_delay 21
layer_note0 23, 24, 98, 0
layer_delay 2
layer_note0 47, 24, 70, 0
layer_delay 1
layer_note0 37, 24, 103, 0
layer_delay 21
layer_note0 25, 24, 94, 0
layer_note0 49, 24, 75, 0
layer_note0 25, 24, 103, 0
layer_note0 25, 18, 94, 0
layer_note0 25, 12, 70, 0
layer_note0 46, 16, 105, 0
layer_delay 26
layer_3_0_repeat:
layer_note0 30, 48, 93, 0
layer_delay 2
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 37, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 24, 93, 0
layer_note0 42, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 62, 0
layer_note0 37, 48, 98, 0
layer_note0 41, 24, 62, 0
layer_note0 37, 24, 94, 0
layer_note0 41, 24, 62, 0
layer_note0 39, 48, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 39, 48, 90, 0
layer_note0 42, 24, 62, 0
layer_note0 39, 24, 94, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 48, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 46, 98, 0
layer_delay 1
layer_note0 46, 24, 62, 0
layer_delay 1
layer_note0 35, 24, 98, 0
layer_note0 42, 24, 62, 0
layer_note0 35, 24, 90, 0
layer_delay 21
layer_note0 23, 24, 84, 0
layer_delay 1
layer_note0 42, 24, 69, 0
layer_note0 29, 48, 98, 0
layer_note0 29, 24, 84, 0
layer_delay 1
layer_note0 41, 24, 69, 0
layer_delay 1
layer_note0 37, 24, 93, 0
layer_delay 21
layer_note0 30, 24, 84, 0
layer_delay 2
layer_note0 46, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 84, 0
layer_note0 39, 24, 69, 0
layer_note0 34, 24, 98, 0
layer_delay 21
layer_note0 29, 24, 98, 0
layer_delay 24
layer_note0 48, 24, 70, 0
layer_note0 29, 24, 88, 0
layer_delay 26
layer_note0 44, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 79, 0
layer_note0 41, 24, 70, 0
layer_note0 34, 24, 98, 0
layer_delay 23
layer_note0 41, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 88, 0
layer_delay 23
layer_note0 41, 24, 70, 0
layer_delay 1
layer_note0 34, 24, 92, 0
layer_note0 38, 24, 70, 0
layer_delay 39
layer_note0 38, 5, 62, 0
layer_delay 7
layer_note0 39, 24, 84, 0
layer_note0 42, 72, 89, 0
layer_delay 12
layer_note0 44, 36, 89, 0
layer_note0 44, 144, 89, 0
layer_delay 12
layer_note0 47, 36, 98, 0
layer_note0 49, 36, 98, 0
layer_note0 49, 48, 70, 0
layer_delay 36
layer_note0 46, 24, 98, 0
layer_note0 47, 24, 94, 0
layer_note0 49, 24, 89, 0
layer_delay 21
layer_note0 46, 12, 62, 0
layer_note0 46, 48, 70, 0
layer_delay 84
layer_note0 46, 24, 70, 0
layer_note0 47, 24, 88, 0
layer_note0 54, 24, 98, 0
layer_delay 12
layer_note0 47, 12, 80, 0
layer_delay 1
layer_note0 46, 12, 74, 0
layer_delay 1
layer_note0 44, 12, 68, 0
layer_delay 1
layer_note0 49, 36, 91, 0
layer_delay 8
layer_note0 46, 12, 80, 0
layer_delay 1
layer_note0 44, 12, 70, 0
layer_delay 1
layer_note0 42, 12, 64, 0
layer_delay 1
layer_note0 44, 12, 59, 0
layer_delay 1
layer_note0 46, 24, 98, 0
layer_delay 9
layer_note0 41, 24, 98, 0
layer_delay 12
layer_note0 41, 12, 98, 0
layer_note0 37, 48, 84, 0
layer_delay 36
layer_note0 37, 24, 98, 0
layer_note0 42, 24, 98, 0
layer_note0 39, 24, 84, 0
layer_note0 46, 24, 98, 0
layer_note0 44, 24, 84, 0
layer_note0 44, 24, 98, 0
layer_delay 48
layer_note0 44, 24, 98, 0
layer_note0 44, 48, 98, 0
layer_delay 93
layer_note0 34, 24, 62, 0
layer_delay 1
layer_note0 41, 23, 62, 0
layer_jump layer_3_0_repeat
layer_3_1:
layer_transpose 0
layer_delay 1
layer_note0 34, 48, 98, 0
layer_delay 2
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 93, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 24, 93, 0
layer_delay 20
layer_note0 34, 48, 98, 0
layer_delay 2
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 93, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 70, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 70, 0
layer_delay 2
layer_note0 41, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 39, 23, 70, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 39, 23, 70, 0
layer_delay 2
layer_note0 42, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 35, 23, 70, 0
layer_delay 2
layer_note0 42, 46, 98, 0
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 22, 98, 0
layer_delay 1
layer_note0 35, 23, 70, 0
layer_delay 2
layer_note0 39, 48, 90, 0
layer_note0 39, 22, 84, 0
layer_note0 39, 24, 69, 0
layer_note0 32, 48, 98, 0
layer_note0 32, 24, 84, 0
layer_note0 37, 24, 69, 0
layer_note0 37, 24, 93, 0
layer_delay 22
layer_note0 34, 24, 84, 0
layer_delay 2
layer_note0 42, 24, 69, 0
layer_delay 1
layer_note0 39, 24, 98, 0
layer_delay 21
layer_note0 30, 24, 84, 0
layer_delay 2
layer_note0 39, 24, 69, 0
layer_delay 2
layer_note0 44, 24, 98, 0
layer_delay 22
layer_note0 44, 24, 64, 0
layer_delay 2
layer_note0 44, 24, 84, 0
layer_delay 22
layer_note0 44, 24, 70, 0
layer_delay 2
layer_note0 44, 22, 92, 0
layer_delay 1
layer_note0 41, 24, 62, 0
layer_delay 1
layer_note0 38, 24, 88, 0
layer_delay 20
layer_note0 29, 24, 89, 0
layer_delay 1
layer_note0 38, 24, 70, 0
layer_delay 1
layer_note0 38, 24, 70, 0
layer_delay 21
layer_note0 46, 24, 92, 0
layer_delay 2
layer_note0 41, 22, 69, 0
layer_delay 1
layer_note0 34, 48, 98, 0
layer_delay 2
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 93, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 62, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 39, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 39, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 62, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 35, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 98, 0
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 22, 98, 0
layer_delay 1
layer_note0 35, 23, 62, 0
layer_delay 2
layer_note0 39, 48, 90, 0
layer_note0 39, 22, 84, 0
layer_note0 39, 24, 69, 0
layer_note0 32, 48, 98, 0
layer_note0 32, 24, 84, 0
layer_note0 37, 24, 69, 0
layer_delay 2
layer_note0 42, 24, 93, 0
layer_delay 20
layer_note0 34, 24, 84, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 1
layer_note0 39, 23, 84, 0
layer_delay 1
layer_note0 34, 23, 69, 0
layer_delay 1
layer_note0 39, 24, 98, 0
layer_delay 20
layer_note0 48, 24, 70, 0
layer_delay 26
layer_note0 44, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 88, 0
layer_delay 24
layer_note0 41, 24, 70, 0
layer_delay 2
layer_note0 41, 22, 79, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 1
layer_note0 38, 24, 98, 0
layer_delay 22
layer_note0 38, 24, 70, 0
layer_delay 2
layer_note0 38, 24, 88, 0
layer_delay 22
layer_note0 38, 24, 70, 0
layer_delay 2
layer_note0 38, 22, 92, 0
layer_delay 1
layer_note0 34, 24, 70, 0
layer_delay 122
layer_note0 46, 24, 89, 0
layer_delay 168
layer_note0 46, 36, 98, 0
layer_delay 225
layer_note0 46, 20, 98, 0
layer_delay 219
layer_note0 51, 12, 88, 0
layer_delay 73
layer_note0 47, 12, 88, 0
layer_delay 72
layer_note0 44, 12, 84, 0
layer_delay 490
layer_note0 38, 24, 62, 0
layer_delay 2
layer_note0 38, 21, 62, 0
layer_delay 1
layer_note0 34, 24, 99, 0
layer_delay 24
layer_note0 34, 24, 105, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 99, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 24
layer_note0 34, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 34, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 105, 0
layer_delay 20
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 105, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 32, 24, 102, 0
layer_delay 1
layer_note0 44, 24, 70, 0
layer_delay 3
layer_note0 42, 22, 103, 0
layer_note0 42, 24, 70, 0
layer_note0 39, 24, 99, 0
layer_delay 22
layer_note0 34, 24, 105, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 99, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 24
layer_note0 34, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 34, 24, 105, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 99, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 32, 24, 99, 0
layer_note0 32, 16, 94, 0
layer_note0 32, 12, 70, 0
layer_note0 42, 24, 105, 0
layer_delay 20
layer_note0 34, 24, 99, 0
layer_delay 24
layer_note0 34, 24, 105, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 30, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 99, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 24
layer_note0 34, 24, 94, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 34, 24, 84, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 42, 24, 99, 0
layer_delay 20
layer_note0 30, 24, 98, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 103, 0
layer_delay 20
layer_note0 32, 24, 94, 0
layer_note0 41, 24, 75, 0
layer_note0 32, 24, 103, 0
layer_note0 32, 16, 94, 0
layer_note0 32, 12, 70, 0
layer_note0 42, 16, 105, 0
layer_delay 27
layer_3_1_repeat:
layer_delay 1
layer_note0 34, 48, 98, 0
layer_delay 2
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 93, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 46, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 2
layer_note0 41, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 62, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 39, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 90, 0
layer_delay 1
layer_note0 39, 23, 62, 0
layer_delay 2
layer_note0 42, 22, 94, 0
layer_delay 1
layer_note0 37, 24, 62, 0
layer_delay 1
layer_note0 42, 46, 98, 0
layer_delay 1
layer_note0 35, 23, 62, 0
layer_delay 2
layer_note0 42, 46, 98, 0
layer_note0 42, 24, 62, 0
layer_delay 2
layer_note0 42, 22, 98, 0
layer_delay 1
layer_note0 35, 23, 62, 0
layer_delay 2
layer_note0 39, 48, 90, 0
layer_note0 39, 22, 84, 0
layer_note0 39, 24, 69, 0
layer_note0 32, 48, 98, 0
layer_note0 32, 24, 84, 0
layer_note0 37, 24, 69, 0
layer_delay 2
layer_note0 42, 24, 93, 0
layer_delay 20
layer_note0 34, 24, 84, 0
layer_delay 2
layer_note0 42, 24, 70, 0
layer_delay 1
layer_note0 39, 23, 84, 0
layer_delay 1
layer_note0 34, 23, 69, 0
layer_delay 1
layer_note0 39, 24, 98, 0
layer_delay 20
layer_note0 48, 24, 70, 0
layer_delay 26
layer_note0 44, 24, 70, 0
layer_delay 1
layer_note0 35, 24, 88, 0
layer_delay 24
layer_note0 41, 24, 70, 0
layer_delay 2
layer_note0 41, 22, 79, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 1
layer_note0 38, 24, 98, 0
layer_delay 22
layer_note0 38, 24, 70, 0
layer_delay 2
layer_note0 38, 24, 88, 0
layer_delay 22
layer_note0 38, 24, 70, 0
layer_delay 2
layer_note0 38, 22, 92, 0
layer_delay 1
layer_note0 34, 24, 70, 0
layer_delay 122
layer_note0 46, 24, 89, 0
layer_delay 168
layer_note0 46, 36, 98, 0
layer_delay 225
layer_note0 46, 20, 98, 0
layer_delay 219
layer_note0 51, 12, 88, 0
layer_delay 73
layer_note0 47, 12, 88, 0
layer_delay 72
layer_note0 44, 12, 84, 0
layer_delay 490
layer_note0 38, 24, 62, 0
layer_delay 2
layer_note0 38, 21, 62, 0
layer_jump layer_3_1_repeat
layer_3_2:
layer_transpose 0
layer_delay 3
layer_note0 37, 48, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 3
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 93, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 24, 93, 0
layer_delay 21
layer_note0 37, 48, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 3
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 93, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 70, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 70, 0
layer_delay 4
layer_note0 44, 20, 94, 0
layer_note0 44, 24, 70, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 70, 0
layer_delay 4
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 70, 0
layer_delay 4
layer_note0 46, 20, 94, 0
layer_note0 46, 24, 70, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 70, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_delay 2
layer_note0 35, 23, 70, 0
layer_delay 3
layer_note0 46, 20, 98, 0
layer_note0 46, 24, 70, 0
layer_delay 4
layer_note0 47, 24, 90, 0
layer_delay 19
layer_note0 30, 28, 84, 0
layer_note0 30, 24, 69, 0
layer_note0 37, 48, 98, 0
layer_note0 37, 22, 84, 0
layer_delay 1
layer_note0 32, 24, 69, 0
layer_delay 1
layer_note0 42, 24, 93, 0
layer_delay 22
layer_note0 37, 24, 84, 0
layer_delay 1
layer_note0 37, 24, 69, 0
layer_delay 2
layer_note0 42, 24, 98, 0
layer_delay 21
layer_note0 34, 24, 84, 0
layer_delay 1
layer_note0 34, 24, 69, 0
layer_delay 2
layer_note0 48, 24, 92, 0
layer_delay 22
layer_note0 41, 23, 64, 0
layer_delay 3
layer_note0 48, 24, 84, 0
layer_delay 22
layer_note0 41, 23, 70, 0
layer_delay 3
layer_note0 48, 20, 88, 0
layer_note0 48, 24, 62, 0
layer_delay 4
layer_note0 41, 24, 88, 0
layer_delay 20
layer_note0 34, 24, 89, 0
layer_note0 41, 24, 70, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 21
layer_note0 29, 24, 88, 0
layer_delay 2
layer_note0 38, 21, 69, 0
layer_delay 3
layer_note0 37, 48, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 3
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 93, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 44, 20, 94, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 94, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_delay 2
layer_note0 35, 23, 62, 0
layer_delay 3
layer_note0 46, 20, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 47, 24, 90, 0
layer_delay 19
layer_note0 30, 28, 84, 0
layer_note0 30, 24, 69, 0
layer_note0 37, 48, 98, 0
layer_note0 37, 22, 84, 0
layer_delay 1
layer_note0 32, 24, 69, 0
layer_delay 2
layer_note0 46, 24, 93, 0
layer_delay 21
layer_note0 37, 24, 84, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 2
layer_note0 42, 20, 84, 0
layer_note0 42, 24, 69, 0
layer_delay 4
layer_note0 42, 24, 98, 0
layer_delay 21
layer_note0 35, 24, 98, 0
layer_delay 24
layer_note0 41, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 88, 0
layer_delay 23
layer_note0 35, 23, 70, 0
layer_delay 3
layer_note0 44, 20, 79, 0
layer_note0 44, 24, 70, 0
layer_delay 4
layer_note0 41, 24, 98, 0
layer_delay 22
layer_note0 34, 23, 70, 0
layer_delay 3
layer_note0 41, 24, 88, 0
layer_delay 22
layer_note0 34, 23, 70, 0
layer_delay 3
layer_note0 41, 20, 92, 0
layer_note0 41, 24, 70, 0
layer_delay 1489
layer_note0 41, 22, 62, 0
layer_delay 4
layer_note0 34, 19, 62, 0
layer_delay 3
layer_note0 39, 24, 99, 0
layer_delay 24
layer_note0 39, 24, 105, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 99, 0
layer_delay 21
layer_note0 35, 24, 94, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 99, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 70, 0
layer_note0 37, 24, 99, 0
layer_delay 24
layer_note0 37, 24, 94, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 2
layer_note0 46, 24, 99, 0
layer_delay 21
layer_note0 39, 24, 94, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 105, 0
layer_delay 21
layer_note0 35, 24, 94, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 105, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 70, 0
layer_note0 37, 24, 102, 0
layer_note0 37, 24, 70, 0
layer_delay 3
layer_note0 46, 20, 103, 0
layer_note0 46, 24, 70, 0
layer_delay 3
layer_note0 42, 24, 99, 0
layer_delay 22
layer_note0 39, 24, 105, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 99, 0
layer_delay 21
layer_note0 35, 24, 94, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 99, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 70, 0
layer_note0 37, 24, 99, 0
layer_delay 24
layer_note0 37, 24, 94, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 2
layer_note0 46, 24, 99, 0
layer_delay 21
layer_note0 39, 24, 105, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 99, 0
layer_delay 21
layer_note0 35, 24, 94, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 99, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 70, 0
layer_note0 37, 24, 99, 0
layer_note0 37, 12, 94, 0
layer_note0 37, 12, 70, 0
layer_delay 48
layer_note0 39, 24, 99, 0
layer_delay 24
layer_note0 39, 24, 105, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 99, 0
layer_delay 21
layer_note0 35, 24, 94, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 99, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 70, 0
layer_note0 37, 24, 99, 0
layer_delay 24
layer_note0 37, 24, 94, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 2
layer_note0 46, 24, 99, 0
layer_delay 21
layer_note0 39, 24, 84, 0
layer_delay 1
layer_note0 39, 24, 70, 0
layer_delay 2
layer_note0 47, 24, 99, 0
layer_delay 21
layer_note0 35, 24, 98, 0
layer_delay 1
layer_note0 35, 24, 70, 0
layer_delay 2
layer_note0 49, 24, 103, 0
layer_delay 21
layer_note0 37, 24, 94, 0
layer_note0 37, 24, 75, 0
layer_note0 37, 24, 103, 0
layer_note0 37, 12, 94, 0
layer_note0 37, 12, 70, 0
layer_delay 45
layer_3_2_repeat:
layer_delay 3
layer_note0 37, 48, 98, 0
layer_delay 1
layer_note0 37, 23, 62, 0
layer_delay 3
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 93, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 44, 44, 98, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 44, 20, 94, 0
layer_note0 44, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 90, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 20, 94, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 46, 44, 98, 0
layer_delay 2
layer_note0 35, 23, 62, 0
layer_delay 3
layer_note0 46, 20, 98, 0
layer_note0 46, 24, 62, 0
layer_delay 4
layer_note0 47, 24, 90, 0
layer_delay 19
layer_note0 30, 28, 84, 0
layer_note0 30, 24, 69, 0
layer_note0 37, 48, 98, 0
layer_note0 37, 22, 84, 0
layer_delay 1
layer_note0 32, 24, 69, 0
layer_delay 2
layer_note0 46, 24, 93, 0
layer_delay 21
layer_note0 37, 24, 84, 0
layer_delay 1
layer_note0 37, 24, 70, 0
layer_delay 2
layer_note0 42, 20, 84, 0
layer_note0 42, 24, 69, 0
layer_delay 4
layer_note0 42, 24, 98, 0
layer_delay 21
layer_note0 35, 24, 98, 0
layer_delay 24
layer_note0 41, 24, 70, 0
layer_delay 2
layer_note0 41, 24, 88, 0
layer_delay 23
layer_note0 35, 23, 70, 0
layer_delay 3
layer_note0 44, 20, 79, 0
layer_note0 44, 24, 70, 0
layer_delay 4
layer_note0 41, 24, 98, 0
layer_delay 22
layer_note0 34, 23, 70, 0
layer_delay 3
layer_note0 41, 24, 88, 0
layer_delay 22
layer_note0 34, 23, 70, 0
layer_delay 3
layer_note0 41, 20, 92, 0
layer_note0 41, 24, 70, 0
layer_delay 1489
layer_note0 41, 22, 62, 0
layer_delay 4
layer_note0 34, 19, 62, 0
layer_jump layer_3_2_repeat
layer_3_3:
layer_transpose 0
layer_delay 5
layer_note0 42, 46, 98, 0
layer_delay 2
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 93, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 98, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 93, 0
layer_delay 28
layer_note0 42, 46, 98, 0
layer_delay 2
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 93, 0
layer_delay 4
layer_note0 34, 24, 62, 0
layer_delay 48
layer_note0 32, 20, 70, 0
layer_note0 32, 48, 98, 0
layer_delay 4
layer_note0 32, 20, 70, 0
layer_note0 32, 24, 94, 0
layer_delay 4
layer_note0 32, 24, 70, 0
layer_delay 48
layer_note0 34, 20, 70, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 70, 0
layer_note0 34, 24, 94, 0
layer_delay 4
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 30, 20, 70, 0
layer_note0 30, 52, 98, 0
layer_note0 30, 20, 70, 0
layer_note0 30, 24, 98, 0
layer_delay 4
layer_note0 30, 20, 70, 0
layer_note0 30, 48, 90, 0
layer_delay 2
layer_note0 35, 24, 84, 0
layer_delay 1
layer_note0 35, 24, 69, 0
layer_delay 2
layer_note0 41, 48, 98, 0
layer_note0 44, 12, 84, 0
layer_delay 8
layer_note0 41, 24, 69, 0
layer_delay 4
layer_note0 46, 24, 93, 0
layer_delay 23
layer_note0 42, 22, 84, 0
layer_delay 2
layer_note0 34, 22, 69, 0
layer_delay 49
layer_note0 39, 23, 84, 0
layer_delay 2
layer_note0 30, 24, 69, 0
layer_delay 48
layer_note0 36, 20, 64, 0
layer_note0 36, 24, 84, 0
layer_delay 28
layer_note0 36, 20, 70, 0
layer_note0 36, 24, 92, 0
layer_delay 4
layer_note0 36, 24, 62, 0
layer_delay 1
layer_note0 48, 24, 92, 0
layer_delay 22
layer_note0 38, 22, 89, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 1
layer_note0 29, 24, 70, 0
layer_delay 22
layer_note0 34, 24, 88, 0
layer_delay 1
layer_note0 34, 20, 69, 0
layer_delay 5
layer_note0 42, 46, 98, 0
layer_delay 2
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 93, 0
layer_delay 4
layer_note0 34, 24, 62, 0
layer_delay 48
layer_note0 32, 20, 62, 0
layer_note0 32, 48, 98, 0
layer_delay 4
layer_note0 32, 20, 62, 0
layer_note0 32, 24, 94, 0
layer_delay 4
layer_note0 32, 24, 62, 0
layer_delay 48
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 94, 0
layer_delay 4
layer_note0 34, 24, 62, 0
layer_delay 48
layer_note0 30, 20, 62, 0
layer_note0 30, 52, 98, 0
layer_note0 30, 20, 62, 0
layer_note0 30, 24, 98, 0
layer_delay 4
layer_note0 30, 20, 62, 0
layer_note0 30, 48, 90, 0
layer_delay 2
layer_note0 35, 24, 84, 0
layer_delay 1
layer_note0 35, 24, 69, 0
layer_delay 2
layer_note0 41, 48, 98, 0
layer_note0 41, 20, 84, 0
layer_delay 3
layer_note0 29, 24, 69, 0
layer_delay 48
layer_note0 42, 22, 84, 0
layer_delay 2
layer_note0 34, 22, 70, 0
layer_delay 26
layer_note0 30, 20, 69, 0
layer_note0 30, 24, 98, 0
layer_delay 28
layer_note0 41, 24, 98, 0
layer_delay 23
layer_note0 35, 23, 70, 0
layer_delay 3
layer_note0 44, 24, 88, 0
layer_delay 24
layer_note0 23, 18, 70, 0
layer_note0 23, 24, 79, 0
layer_delay 6
layer_note0 29, 19, 70, 0
layer_note0 29, 24, 98, 0
layer_delay 28
layer_note0 29, 20, 70, 0
layer_note0 29, 24, 88, 0
layer_delay 28
layer_note0 29, 20, 70, 0
layer_note0 29, 24, 92, 0
layer_delay 4
layer_note0 29, 24, 70, 0
layer_delay 1488
layer_note0 46, 24, 62, 0
layer_delay 24
layer_note0 42, 24, 99, 0
layer_delay 24
layer_note0 42, 22, 105, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 24, 70, 0
layer_note0 42, 24, 99, 0
layer_delay 24
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 34, 20, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 28
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 20, 70, 0
layer_delay 4
layer_note0 41, 21, 102, 0
layer_delay 3
layer_note0 32, 24, 70, 0
layer_delay 24
layer_note0 34, 20, 70, 0
layer_note0 34, 1, 99, 0
layer_delay 4
layer_note0 46, 24, 99, 0
layer_delay 23
layer_note0 42, 22, 105, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 24, 70, 0
layer_note0 42, 24, 99, 0
layer_delay 24
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 34, 20, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 28
layer_note0 42, 22, 105, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 20, 70, 0
layer_delay 3
layer_note0 41, 24, 99, 0
layer_delay 1
layer_note0 41, 9, 94, 0
layer_note0 41, 12, 70, 0
layer_delay 51
layer_note0 42, 24, 99, 0
layer_delay 24
layer_note0 42, 22, 105, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 24, 70, 0
layer_note0 42, 24, 99, 0
layer_delay 24
layer_note0 42, 22, 94, 0
layer_delay 2
layer_note0 34, 20, 70, 0
layer_note0 34, 24, 99, 0
layer_delay 28
layer_note0 42, 22, 84, 0
layer_delay 2
layer_note0 34, 24, 70, 0
layer_delay 48
layer_note0 42, 22, 98, 0
layer_delay 2
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 41, 20, 94, 0
layer_delay 4
layer_note0 32, 20, 75, 0
layer_delay 3
layer_note0 41, 24, 103, 0
layer_delay 1
layer_note0 41, 9, 94, 0
layer_note0 41, 12, 70, 0
layer_delay 46
layer_3_3_repeat:
layer_delay 5
layer_note0 42, 46, 98, 0
layer_delay 2
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 93, 0
layer_delay 4
layer_note0 34, 24, 62, 0
layer_delay 48
layer_note0 32, 20, 62, 0
layer_note0 32, 48, 98, 0
layer_delay 4
layer_note0 32, 20, 62, 0
layer_note0 32, 24, 94, 0
layer_delay 4
layer_note0 32, 24, 62, 0
layer_delay 48
layer_note0 34, 20, 62, 0
layer_note0 34, 48, 90, 0
layer_delay 4
layer_note0 34, 20, 62, 0
layer_note0 34, 24, 94, 0
layer_delay 4
layer_note0 34, 24, 62, 0
layer_delay 48
layer_note0 30, 20, 62, 0
layer_note0 30, 52, 98, 0
layer_note0 30, 20, 62, 0
layer_note0 30, 24, 98, 0
layer_delay 4
layer_note0 30, 20, 62, 0
layer_note0 30, 48, 90, 0
layer_delay 2
layer_note0 35, 24, 84, 0
layer_delay 1
layer_note0 35, 24, 69, 0
layer_delay 2
layer_note0 41, 48, 98, 0
layer_note0 41, 20, 84, 0
layer_delay 3
layer_note0 29, 24, 69, 0
layer_delay 48
layer_note0 42, 22, 84, 0
layer_delay 2
layer_note0 34, 22, 70, 0
layer_delay 26
layer_note0 30, 20, 69, 0
layer_note0 30, 24, 98, 0
layer_delay 28
layer_note0 41, 24, 98, 0
layer_delay 23
layer_note0 35, 23, 70, 0
layer_delay 3
layer_note0 44, 24, 88, 0
layer_delay 24
layer_note0 23, 18, 70, 0
layer_note0 23, 24, 79, 0
layer_delay 6
layer_note0 29, 19, 70, 0
layer_note0 29, 24, 98, 0
layer_delay 28
layer_note0 29, 20, 70, 0
layer_note0 29, 24, 88, 0
layer_delay 28
layer_note0 29, 20, 70, 0
layer_note0 29, 24, 92, 0
layer_delay 4
layer_note0 29, 24, 70, 0
layer_delay 1488
layer_note0 46, 24, 62, 0
layer_delay 19
layer_jump layer_3_3_repeat
channel_4:
chan_largenoteson
chan_setinstr 7
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_4_0
chan_setlayer 1, layer_4_1
chan_setlayer 2, layer_4_2
chan_setvol 55
chan_setpan 57
chan_setvolscale 119
chan_setreverb 63
chan_delay 3350
chan_setvolscale 119
chan_delay 50
chan_setvolscale 116
chan_delay 35
chan_setvolscale 112
chan_delay 35
chan_setvolscale 107
chan_delay 50
chan_setvolscale 106
chan_delay 1320
chan_setvolscale 107
chan_delay 30
chan_setvolscale 111
chan_delay 40
chan_setvolscale 115
chan_delay 50
chan_setvolscale 119
chan_delay 32
chan_setvolscale 119
chan_delay 2304
channel_4_repeat:
chan_setvol 55
chan_setpan 57
chan_setvolscale 119
chan_setreverb 63
chan_delay 1430
chan_setvolscale 119
chan_delay 50
chan_setvolscale 116
chan_delay 35
chan_setvolscale 112
chan_delay 35
chan_setvolscale 107
chan_delay 50
chan_setvolscale 106
chan_delay 1320
chan_setvolscale 107
chan_delay 30
chan_setvolscale 111
chan_delay 40
chan_setvolscale 115
chan_delay 50
chan_setvolscale 119
chan_delay 32
chan_jump channel_4_repeat
layer_4_0:
layer_transpose 0
layer_delay 6
layer_note0 46, 44, 98, 0
layer_delay 4
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 86, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 24, 90, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 93, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 85, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 24, 90, 0
layer_delay 30
layer_note0 46, 44, 98, 0
layer_delay 4
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 86, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 24, 90, 0
layer_delay 6
layer_note0 30, 24, 62, 0
layer_delay 48
layer_note0 25, 18, 70, 0
layer_note0 25, 48, 86, 0
layer_delay 6
layer_note0 25, 18, 70, 0
layer_note0 25, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 70, 0
layer_delay 48
layer_note0 27, 18, 70, 0
layer_note0 27, 48, 86, 0
layer_delay 6
layer_note0 27, 18, 70, 0
layer_note0 27, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 70, 0
layer_delay 48
layer_note0 23, 18, 70, 0
layer_note0 23, 54, 98, 0
layer_note0 23, 18, 70, 0
layer_note0 23, 24, 98, 0
layer_delay 6
layer_note0 23, 18, 70, 0
layer_note0 23, 48, 90, 0
layer_delay 6
layer_note0 42, 19, 84, 0
layer_delay 5
layer_note0 23, 24, 69, 0
layer_note0 44, 48, 98, 0
layer_note0 41, 20, 84, 0
layer_delay 3
layer_note0 29, 24, 69, 0
layer_delay 49
layer_note0 46, 20, 84, 0
layer_delay 4
layer_note0 30, 19, 69, 0
layer_note0 30, 24, 98, 0
layer_delay 29
layer_note0 42, 20, 84, 0
layer_delay 4
layer_note0 27, 24, 69, 0
layer_delay 48
layer_note0 29, 18, 64, 0
layer_note0 29, 24, 84, 0
layer_delay 30
layer_note0 29, 18, 70, 0
layer_note0 29, 24, 92, 0
layer_delay 6
layer_note0 29, 19, 62, 0
layer_note0 29, 24, 88, 0
layer_delay 28
layer_note0 41, 21, 89, 0
layer_delay 3
layer_note0 29, 24, 70, 0
layer_delay 1
layer_note0 22, 24, 89, 0
layer_delay 23
layer_note0 38, 22, 88, 0
layer_delay 2
layer_note0 29, 19, 69, 0
layer_delay 6
layer_note0 46, 44, 98, 0
layer_delay 4
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 86, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 24, 90, 0
layer_delay 6
layer_note0 30, 24, 62, 0
layer_delay 48
layer_note0 25, 18, 62, 0
layer_note0 25, 48, 86, 0
layer_delay 6
layer_note0 25, 18, 62, 0
layer_note0 25, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 62, 0
layer_delay 48
layer_note0 27, 18, 62, 0
layer_note0 27, 48, 86, 0
layer_delay 6
layer_note0 27, 18, 62, 0
layer_note0 27, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 62, 0
layer_delay 48
layer_note0 23, 18, 62, 0
layer_note0 23, 54, 98, 0
layer_note0 23, 18, 62, 0
layer_note0 23, 24, 98, 0
layer_delay 6
layer_note0 23, 18, 62, 0
layer_note0 23, 48, 90, 0
layer_delay 6
layer_note0 44, 24, 84, 0
layer_note0 23, 24, 69, 0
layer_delay 42
layer_note0 25, 24, 84, 0
layer_delay 6
layer_note0 25, 24, 69, 0
layer_delay 48
layer_note0 46, 20, 84, 0
layer_delay 4
layer_note0 30, 19, 70, 0
layer_note0 30, 24, 84, 0
layer_delay 5
layer_note0 27, 18, 69, 0
layer_note0 27, 24, 98, 0
layer_delay 30
layer_note0 44, 24, 98, 0
layer_delay 24
layer_note0 29, 18, 70, 0
layer_note0 48, 24, 70, 0
layer_delay 30
layer_note0 29, 18, 70, 0
layer_note0 29, 24, 79, 0
layer_delay 6
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 22, 18, 70, 0
layer_note0 22, 24, 88, 0
layer_delay 30
layer_note0 22, 18, 70, 0
layer_note0 22, 24, 92, 0
layer_delay 6
layer_note0 22, 24, 70, 0
layer_delay 1536
layer_note0 46, 24, 99, 0
layer_delay 24
layer_note0 46, 20, 105, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 94, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 24, 70, 0
layer_note0 46, 24, 99, 0
layer_delay 24
layer_note0 46, 20, 94, 0
layer_delay 4
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 46, 20, 94, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 94, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 18, 70, 0
layer_delay 6
layer_note0 44, 20, 102, 0
layer_delay 4
layer_note0 25, 24, 70, 0
layer_delay 24
layer_note0 27, 18, 70, 0
layer_note0 27, 1, 99, 0
layer_note0 34, 24, 99, 0
layer_delay 29
layer_note0 46, 20, 105, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 94, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 24, 70, 0
layer_note0 46, 24, 99, 0
layer_delay 24
layer_note0 46, 20, 94, 0
layer_delay 4
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 46, 20, 105, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 94, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 18, 70, 0
layer_delay 6
layer_note0 49, 24, 99, 0
layer_note0 49, 7, 94, 0
layer_note0 49, 12, 70, 0
layer_note0 34, 24, 105, 0
layer_delay 29
layer_note0 46, 24, 99, 0
layer_delay 24
layer_note0 46, 20, 105, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 94, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 24, 70, 0
layer_note0 46, 24, 99, 0
layer_delay 24
layer_note0 46, 20, 94, 0
layer_delay 4
layer_note0 30, 24, 70, 0
layer_delay 48
layer_note0 46, 20, 84, 0
layer_delay 4
layer_note0 27, 24, 70, 0
layer_delay 48
layer_note0 47, 20, 98, 0
layer_delay 4
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 49, 18, 94, 0
layer_delay 6
layer_note0 25, 18, 75, 0
layer_delay 6
layer_note0 49, 24, 103, 0
layer_note0 49, 7, 94, 0
layer_note0 49, 12, 70, 0
layer_note0 34, 16, 105, 0
layer_delay 31
layer_4_0_repeat:
layer_delay 6
layer_note0 46, 44, 98, 0
layer_delay 4
layer_note0 30, 18, 62, 0
layer_note0 30, 48, 86, 0
layer_delay 6
layer_note0 30, 18, 62, 0
layer_note0 30, 24, 90, 0
layer_delay 6
layer_note0 30, 24, 62, 0
layer_delay 48
layer_note0 25, 18, 62, 0
layer_note0 25, 48, 86, 0
layer_delay 6
layer_note0 25, 18, 62, 0
layer_note0 25, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 62, 0
layer_delay 48
layer_note0 27, 18, 62, 0
layer_note0 27, 48, 86, 0
layer_delay 6
layer_note0 27, 18, 62, 0
layer_note0 27, 24, 90, 0
layer_delay 6
layer_note0 25, 24, 62, 0
layer_delay 48
layer_note0 23, 18, 62, 0
layer_note0 23, 54, 98, 0
layer_note0 23, 18, 62, 0
layer_note0 23, 24, 98, 0
layer_delay 6
layer_note0 23, 18, 62, 0
layer_note0 23, 48, 90, 0
layer_delay 6
layer_note0 44, 24, 84, 0
layer_note0 23, 24, 69, 0
layer_delay 42
layer_note0 25, 24, 84, 0
layer_delay 6
layer_note0 25, 24, 69, 0
layer_delay 48
layer_note0 46, 20, 84, 0
layer_delay 4
layer_note0 30, 19, 70, 0
layer_note0 30, 24, 84, 0
layer_delay 5
layer_note0 27, 18, 69, 0
layer_note0 27, 24, 98, 0
layer_delay 30
layer_note0 44, 24, 98, 0
layer_delay 24
layer_note0 29, 18, 70, 0
layer_note0 48, 24, 70, 0
layer_delay 30
layer_note0 29, 18, 70, 0
layer_note0 29, 24, 79, 0
layer_delay 6
layer_note0 23, 24, 70, 0
layer_delay 48
layer_note0 22, 18, 70, 0
layer_note0 22, 24, 88, 0
layer_delay 30
layer_note0 22, 18, 70, 0
layer_note0 22, 24, 92, 0
layer_delay 6
layer_note0 22, 24, 70, 0
layer_delay 1530
layer_jump layer_4_0_repeat
layer_4_1:
layer_transpose 0
layer_delay 576
layer_note0 25, 48, 93, 0
layer_delay 144
layer_note0 27, 48, 94, 0
layer_delay 144
layer_note0 23, 48, 98, 0
layer_delay 150
layer_note0 42, 48, 90, 0
layer_note0 47, 24, 84, 0
layer_note0 46, 24, 69, 0
layer_delay 42
layer_note0 25, 24, 84, 0
layer_delay 6
layer_note0 25, 24, 69, 0
layer_delay 90
layer_note0 27, 24, 98, 0
layer_delay 72
layer_note0 29, 24, 98, 0
layer_delay 168
layer_note0 22, 24, 88, 0
layer_delay 30
layer_note0 48, 19, 83, 0
layer_delay 5
layer_note0 22, 24, 70, 0
layer_note0 46, 24, 93, 0
layer_delay 24
layer_note0 41, 20, 88, 0
layer_delay 4
layer_note0 22, 18, 69, 0
layer_delay 192
layer_note0 25, 48, 93, 0
layer_delay 144
layer_note0 27, 48, 94, 0
layer_delay 144
layer_note0 23, 48, 98, 0
layer_delay 150
layer_note0 44, 48, 90, 0
layer_note0 47, 24, 84, 0
layer_note0 47, 24, 69, 0
layer_delay 90
layer_note0 30, 24, 90, 0
layer_delay 72
layer_note0 27, 24, 84, 0
layer_delay 264
layer_note0 22, 24, 98, 0
layer_delay 1800
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 99, 0
layer_delay 168
layer_note0 27, 24, 99, 0
layer_delay 72
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 100, 0
layer_delay 120
layer_note0 27, 30, 103, 0
layer_delay 18
layer_note0 27, 24, 99, 0
layer_delay 72
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 99, 0
layer_delay 168
layer_note0 27, 24, 99, 0
layer_delay 72
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 99, 0
layer_delay 120
layer_note0 27, 24, 105, 0
layer_delay 120
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 99, 0
layer_delay 168
layer_note0 27, 24, 99, 0
layer_delay 72
layer_note0 23, 24, 99, 0
layer_delay 72
layer_note0 25, 24, 103, 0
layer_delay 120
layer_note0 27, 16, 105, 0
layer_delay 32
layer_4_1_repeat:
layer_delay 192
layer_note0 25, 48, 93, 0
layer_delay 144
layer_note0 27, 48, 94, 0
layer_delay 144
layer_note0 23, 48, 98, 0
layer_delay 150
layer_note0 44, 48, 90, 0
layer_note0 47, 24, 84, 0
layer_note0 47, 24, 69, 0
layer_delay 90
layer_note0 30, 24, 90, 0
layer_delay 72
layer_note0 27, 24, 84, 0
layer_delay 264
layer_note0 22, 24, 98, 0
layer_delay 1704
layer_jump layer_4_1_repeat
layer_4_2:
layer_transpose 0
layer_delay 577
layer_note0 32, 48, 98, 0
layer_delay 144
layer_note0 34, 48, 98, 0
layer_delay 144
layer_note0 30, 48, 98, 0
layer_delay 239
layer_note0 25, 48, 92, 0
layer_delay 49
layer_note0 34, 24, 93, 0
layer_delay 168
layer_note0 36, 24, 98, 0
layer_delay 552
layer_note0 32, 48, 98, 0
layer_delay 144
layer_note0 34, 48, 98, 0
layer_delay 144
layer_note0 30, 48, 98, 0
layer_delay 239
layer_note0 25, 48, 92, 0
layer_delay 49
layer_note0 34, 24, 93, 0
layer_delay 2184
layer_note0 30, 24, 99, 0
layer_delay 72
layer_note0 32, 24, 99, 0
layer_delay 264
layer_note0 30, 24, 105, 0
layer_delay 72
layer_note0 32, 24, 105, 0
layer_delay 120
layer_note0 34, 28, 103, 0
layer_delay 116
layer_note0 30, 24, 99, 0
layer_delay 72
layer_note0 32, 24, 99, 0
layer_delay 264
layer_note0 30, 24, 99, 0
layer_delay 72
layer_note0 32, 24, 99, 0
layer_delay 119
layer_note0 39, 24, 105, 0
layer_delay 121
layer_note0 30, 24, 99, 0
layer_delay 72
layer_note0 32, 24, 99, 0
layer_delay 264
layer_note0 30, 24, 99, 0
layer_delay 72
layer_note0 32, 24, 103, 0
layer_delay 119
layer_note0 39, 16, 105, 0
layer_delay 32
layer_4_2_repeat:
layer_delay 193
layer_note0 32, 48, 98, 0
layer_delay 144
layer_note0 34, 48, 98, 0
layer_delay 144
layer_note0 30, 48, 98, 0
layer_delay 239
layer_note0 25, 48, 92, 0
layer_delay 49
layer_note0 34, 24, 93, 0
layer_delay 2087
layer_jump layer_4_2_repeat
channel_5:
chan_largenoteson
chan_setinstr 8
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_5_0
chan_setlayer 1, layer_5_1
chan_setvol 81
chan_setpan 81
chan_setvolscale 127
chan_setreverb 74
chan_delay 7296
channel_5_repeat:
chan_setvol 81
chan_setpan 81
chan_setvolscale 127
chan_setreverb 74
chan_delay 3072
chan_jump channel_5_repeat
layer_5_0:
layer_transpose 0
layer_delay 3495
layer_note0 38, 5, 65, 0
layer_delay 7
layer_note0 39, 24, 87, 0
layer_note0 42, 72, 92, 0
layer_delay 12
layer_note0 44, 35, 92, 0
layer_delay 1
layer_note0 44, 144, 92, 0
layer_delay 12
layer_note0 47, 30, 101, 0
layer_delay 6
layer_note0 49, 35, 101, 0
layer_delay 1
layer_note0 49, 48, 73, 0
layer_delay 36
layer_note0 46, 24, 101, 0
layer_note0 47, 24, 97, 0
layer_note0 49, 24, 92, 0
layer_delay 21
layer_note0 46, 11, 65, 0
layer_delay 1
layer_note0 46, 48, 73, 0
layer_delay 84
layer_note0 46, 24, 73, 0
layer_note0 47, 24, 91, 0
layer_note0 54, 24, 101, 0
layer_note0 51, 12, 91, 0
layer_note0 47, 12, 83, 0
layer_note0 46, 12, 77, 0
layer_note0 44, 12, 71, 0
layer_note0 49, 36, 94, 0
layer_note0 47, 12, 91, 0
layer_note0 46, 12, 83, 0
layer_note0 44, 12, 73, 0
layer_note0 42, 12, 67, 0
layer_note0 44, 12, 62, 0
layer_note0 46, 24, 101, 0
layer_note0 44, 12, 87, 0
layer_note0 41, 24, 101, 0
layer_delay 12
layer_note0 41, 12, 101, 0
layer_note0 37, 48, 87, 0
layer_delay 36
layer_note0 37, 24, 101, 0
layer_note0 42, 24, 101, 0
layer_note0 39, 24, 87, 0
layer_note0 46, 24, 101, 0
layer_note0 44, 23, 87, 0
layer_delay 1
layer_note0 44, 24, 101, 0
layer_delay 48
layer_note0 44, 23, 101, 0
layer_delay 1
layer_note0 44, 48, 101, 0
layer_delay 2448
layer_5_0_repeat:
layer_delay 1575
layer_note0 38, 5, 65, 0
layer_delay 7
layer_note0 39, 24, 87, 0
layer_note0 42, 72, 92, 0
layer_delay 12
layer_note0 44, 35, 92, 0
layer_delay 1
layer_note0 44, 144, 92, 0
layer_delay 12
layer_note0 47, 30, 101, 0
layer_delay 6
layer_note0 49, 35, 101, 0
layer_delay 1
layer_note0 49, 48, 73, 0
layer_delay 36
layer_note0 46, 24, 101, 0
layer_note0 47, 24, 97, 0
layer_note0 49, 24, 92, 0
layer_delay 21
layer_note0 46, 11, 65, 0
layer_delay 1
layer_note0 46, 48, 73, 0
layer_delay 84
layer_note0 46, 24, 73, 0
layer_note0 47, 24, 91, 0
layer_note0 54, 24, 101, 0
layer_note0 51, 12, 91, 0
layer_note0 47, 12, 83, 0
layer_note0 46, 12, 77, 0
layer_note0 44, 12, 71, 0
layer_note0 49, 36, 94, 0
layer_note0 47, 12, 91, 0
layer_note0 46, 12, 83, 0
layer_note0 44, 12, 73, 0
layer_note0 42, 12, 67, 0
layer_note0 44, 12, 62, 0
layer_note0 46, 24, 101, 0
layer_note0 44, 12, 87, 0
layer_note0 41, 24, 101, 0
layer_delay 12
layer_note0 41, 12, 101, 0
layer_note0 37, 48, 87, 0
layer_delay 36
layer_note0 37, 24, 101, 0
layer_note0 42, 24, 101, 0
layer_note0 39, 24, 87, 0
layer_note0 46, 24, 101, 0
layer_note0 44, 23, 87, 0
layer_delay 1
layer_note0 44, 24, 101, 0
layer_delay 48
layer_note0 44, 23, 101, 0
layer_delay 1
layer_note0 44, 48, 101, 0
layer_delay 144
layer_jump layer_5_0_repeat
layer_5_1:
layer_transpose 0
layer_delay 3579
layer_note0 46, 24, 92, 0
layer_delay 168
layer_note0 46, 36, 101, 0
layer_delay 225
layer_note0 46, 20, 101, 0
layer_delay 3244
layer_5_1_repeat:
layer_delay 1659
layer_note0 46, 24, 92, 0
layer_delay 168
layer_note0 46, 36, 101, 0
layer_delay 225
layer_note0 46, 20, 101, 0
layer_delay 940
layer_jump layer_5_1_repeat
channel_6:
chan_largenoteson
chan_setinstr 13
chan_setvol 127
chan_setvolscale 127
chan_setlayer 0, layer_6_0
chan_setlayer 1, layer_6_1
chan_setvol 62
chan_setpan 71
chan_setvolscale 127
chan_setreverb 100
chan_delay 7296
channel_6_repeat:
chan_setvol 62
chan_setpan 71
chan_setvolscale 127
chan_setreverb 100
chan_delay 3072
chan_jump channel_6_repeat
layer_6_0:
layer_transpose 0
layer_delay 3495
layer_note0 38, 7, 45, 0
layer_delay 5
layer_note0 39, 26, 61, 0
layer_delay 46
layer_note0 46, 26, 65, 0
layer_delay 10
layer_note0 44, 36, 65, 0
layer_note0 44, 146, 65, 0
layer_delay 10
layer_note0 47, 32, 72, 0
layer_delay 4
layer_note0 49, 36, 72, 0
layer_note0 49, 50, 51, 0
layer_delay 34
layer_note0 46, 26, 72, 0
layer_delay 22
layer_note0 49, 26, 65, 0
layer_delay 19
layer_note0 46, 12, 45, 0
layer_note0 46, 50, 51, 0
layer_delay 82
layer_note0 46, 26, 51, 0
layer_delay 22
layer_note0 54, 26, 72, 0
layer_delay 10
layer_note0 47, 14, 58, 0
layer_delay 10
layer_note0 44, 14, 50, 0
layer_delay 34
layer_note0 47, 14, 64, 0
layer_delay 10
layer_note0 44, 14, 51, 0
layer_delay 10
layer_note0 44, 14, 43, 0
layer_delay 22
layer_note0 44, 14, 61, 0
layer_delay 34
layer_note0 41, 14, 72, 0
layer_delay 82
layer_note0 37, 26, 72, 0
layer_delay 22
layer_note0 39, 26, 61, 0
layer_delay 22
layer_note0 44, 24, 61, 0
layer_note0 44, 26, 72, 0
layer_delay 46
layer_note0 44, 24, 72, 0
layer_note0 44, 50, 72, 0
layer_delay 2446
layer_6_0_repeat:
layer_delay 1575
layer_note0 38, 7, 45, 0
layer_delay 5
layer_note0 39, 26, 61, 0
layer_delay 46
layer_note0 46, 26, 65, 0
layer_delay 10
layer_note0 44, 36, 65, 0
layer_note0 44, 146, 65, 0
layer_delay 10
layer_note0 47, 32, 72, 0
layer_delay 4
layer_note0 49, 36, 72, 0
layer_note0 49, 50, 51, 0
layer_delay 34
layer_note0 46, 26, 72, 0
layer_delay 22
layer_note0 49, 26, 65, 0
layer_delay 19
layer_note0 46, 12, 45, 0
layer_note0 46, 50, 51, 0
layer_delay 82
layer_note0 46, 26, 51, 0
layer_delay 22
layer_note0 54, 26, 72, 0
layer_delay 10
layer_note0 47, 14, 58, 0
layer_delay 10
layer_note0 44, 14, 50, 0
layer_delay 34
layer_note0 47, 14, 64, 0
layer_delay 10
layer_note0 44, 14, 51, 0
layer_delay 10
layer_note0 44, 14, 43, 0
layer_delay 22
layer_note0 44, 14, 61, 0
layer_delay 34
layer_note0 41, 14, 72, 0
layer_delay 82
layer_note0 37, 26, 72, 0
layer_delay 22
layer_note0 39, 26, 61, 0
layer_delay 22
layer_note0 44, 24, 61, 0
layer_note0 44, 26, 72, 0
layer_delay 46
layer_note0 44, 24, 72, 0
layer_note0 44, 50, 72, 0
layer_delay 142
layer_jump layer_6_0_repeat
layer_6_1:
layer_transpose 0
layer_delay 3531
layer_note0 42, 74, 65, 0
layer_delay 166
layer_note0 46, 38, 72, 0
layer_delay 178
layer_note0 47, 26, 69, 0
layer_delay 19
layer_note0 46, 22, 72, 0
layer_delay 170
layer_note0 47, 26, 64, 0
layer_delay 22
layer_note0 51, 14, 64, 0
layer_delay 10
layer_note0 46, 14, 54, 0
layer_delay 10
layer_note0 49, 38, 66, 0
layer_delay 10
layer_note0 46, 14, 58, 0
layer_delay 10
layer_note0 42, 14, 47, 0
layer_delay 10
layer_note0 46, 26, 72, 0
layer_delay 10
layer_note0 41, 26, 72, 0
layer_delay 22
layer_note0 37, 50, 61, 0
layer_delay 58
layer_note0 42, 26, 72, 0
layer_delay 22
layer_note0 46, 26, 72, 0
layer_delay 2614
layer_6_1_repeat:
layer_delay 1611
layer_note0 42, 74, 65, 0
layer_delay 166
layer_note0 46, 38, 72, 0
layer_delay 178
layer_note0 47, 26, 69, 0
layer_delay 19
layer_note0 46, 22, 72, 0
layer_delay 170
layer_note0 47, 26, 64, 0
layer_delay 22
layer_note0 51, 14, 64, 0
layer_delay 10
layer_note0 46, 14, 54, 0
layer_delay 10
layer_note0 49, 38, 66, 0
layer_delay 10
layer_note0 46, 14, 58, 0
layer_delay 10
layer_note0 42, 14, 47, 0
layer_delay 10
layer_note0 46, 26, 72, 0
layer_delay 10
layer_note0 41, 26, 72, 0
layer_delay 22
layer_note0 37, 50, 61, 0
layer_delay 58
layer_note0 42, 26, 72, 0
layer_delay 22
layer_note0 46, 26, 72, 0
layer_delay 310
layer_jump layer_6_1_repeat
