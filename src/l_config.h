#ifndef L_CONFIG_H
#define L_CONFIG_H

#include"itypes.h"

enum {
    COLOR_FOREGROUND,
    COLOR_BACKGROUND,
    COLOR_WORD_INDEX_MODE_NORMAL,
    COLOR_WORD_INDEX_MODE_INSERT,
    COLOR_WORD_INDEX_MODE_NAME_EDIT,
    COLOR_EXPLORER_POINTER,
};

#define CONFIG_COLOR_FOREGROUND config_colors[COLOR_FOREGROUND]
#define CONFIG_COLOR_BACKGROUND config_colors[COLOR_BACKGROUND]
#define CONFIG_COLOR_WORD_INDEX_MODE_NORMAL config_colors[COLOR_WORD_INDEX_MODE_NORMAL]
#define CONFIG_COLOR_WORD_INDEX_MODE_INSERT config_colors[COLOR_WORD_INDEX_MODE_INSERT]
#define CONFIG_COLOR_WORD_INDEX_MODE_NAME_EDIT config_colors[COLOR_WORD_INDEX_MODE_NAME_EDIT]
#define CONFIG_COLOR_EXPLORER_POINTER config_colors[COLOR_EXPLORER_POINTER]

#if 1
u32 config_colors[] = {
    0xff181818,
    0xffd8d8d8,
    0xffa8a8a8,
    0xffc8c8c8,
    0xffa8a8a8,
    0xffcc3388,
};
#else
u32 config_colors[] = {
    0xff000000,
    0xff746220,
    0xffa8a8a8,
    0xffc8c8c8,
    0xffa8a8a8,
    0xffcc3388,
};
#endif

char config_modes[][10] = {
    "NORMAL",
    "INSERT",
    "NAME EDIT",
};

#endif
