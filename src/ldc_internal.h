#ifndef INTERNAL_H
#define INTERNAL_H

#include"itypes.h"
#include"l_config.h"

#include<stdbool.h>

#define KEY_ASCII_BACKSPACE 0x8
#define KEY_ASCII_TAB 0x9
#define KEY_ASCII_CONTROL_D 0x4
#define KEY_ASCII_ENTER 0xd
#define KEY_ASCII_CONTROL_U 0x15
#define KEY_ASCII_CONTROL_V 0x16
#define KEY_ASCII_ESC 0x1b

struct {
    u16 width;
    u16 height;
    bool running;
    void (*render_func)(void);
    void (*input_func)(u8 key);
    u32 *bits;
} GlobalGui;

void gui_init(u16 width, u16 height);
void gui_update(void);
void gui_loop(void);

/* Render Functions */
void render_default_func(void);
void render(void);
void render_line_horizontal(u16 x, u16 y, u16 width, u32 color);

/* Font */
#include<ft2build.h>
#include FT_FREETYPE_H

struct {
    FT_Library library;
    FT_Face face;
    u16 size;
} GlobalFont;

void font_init(const char *path, u32 font_size);
void font_size_set(u32 font_size);
u16 font_width_tby_str_size(u32 size);
u16 font_width_of_str(u8 *str);

/* Status Bar */
#define STATUS_BAR_PADDING 5

void status_bar_render(void);

enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_NAME_EDIT,
};

#define CHARSET_RANGE_MAX_SIZE 5

typedef struct Charset {
    u8 ranges[CHARSET_RANGE_MAX_SIZE][2];
    u8 ranges_count;
} Charset;

Charset charset_name = {
    .ranges = {
        { 0x61, 0x7a },
        { 0x41, 0x5a },
        { 0x5f, 0x5f },
        { ' ', ' ' },
    },
    .ranges_count = 4,
};

Charset charset_word = {
    .ranges = {
        { 0x01, 0x7f },
    },
    .ranges_count = 1,
};

typedef struct InsertPtr {
    u8 *ptr;
    u32 ptr_count;
    u32 ptr_size_max;
    Charset *charset;
} InsertPtr;

#define NAME_MAX_SIZE 50

#define WORD_MAX_SIZE 100

typedef struct DecodeWord {
    u8 word_foreign[WORD_MAX_SIZE + 1];
    u8 word_native[WORD_MAX_SIZE + 1];
    bool word_have_to_practice;
} DecodeWord;

struct DecodeEntry {
    DecodeWord *decode_words;
    u32 decode_words_count;
    u32 decode_words_index;
    u8 decode_word_index;
    InsertPtr insert_ptr;
    u8 mode;
    u8 name[NAME_MAX_SIZE + 1];
    bool is_saved;
    u32 scroll_offset;
    u32 *lines;
    u32 lines_count;
} GlobalDecodeEntry = {
    .decode_words = NULL,
    .decode_words_count = 0,
    .decode_words_index = 0,
    .decode_word_index = 0,
    .insert_ptr = {
        .ptr = NULL,
        .ptr_count = 0,
        .ptr_size_max = 0,
        .charset = NULL,
    },
    .mode = MODE_NORMAL,
    .name = { '-' },
    .is_saved = false,
    .scroll_offset = 0,
    .lines = NULL,
    .lines_count = 0,
};

#define LINES_ALLOC_SIZE 200

void insert_ptr_attach(InsertPtr *insert_ptr, u8 *ptr, u32 size_max, Charset *charset);
void insert_ptr_append_byte(InsertPtr *insert_ptr, u8 byte);

void decode_word_append(void);
void decode_word_delete(u32 index);
void decode_entry_insert_buffer(u8 *buffer, u32 buffer_count);
void decode_entry_create(void);
void decode_words_index_move_right(void);
void decode_words_index_move_left(void);
void decode_word_index_move_down(void);
void decode_word_index_move_up(void);
void decode_entry_save(void);
void decode_entry_load(char *path_load);
void decode_entry_scroll_down(void);
void decode_entry_scroll_up(void);
void decode_words_index_move_line_start(void);
void decode_words_index_move_line_end(void);
void decode_words_index_move_line_down(void);
void decode_words_index_move_line_up(void);

/* Explorer */
void explorer_render_func(void);
void explorer_input_func(u8 key);

/* PDF */
void pdf_create(void);

/* Cipboard */
u8 *clipboard_func(u32 *buffer_count);

/* Main */
void render_func(void);
void input_func(u8 key);

#define DECODE_ENTRYS_SAVE_DIRECTORY ".ldc_decode_entrys"
char *decode_entrys_save_path;

#endif
