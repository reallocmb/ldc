#include"ldc_internal.h"

#include<stdlib.h>

#define DECODE_ENTRY_ALLOC_SIZE 250

void decode_word_delete(u32 index)
{
    memcpy(GlobalDecodeEntry.decode_words + index,
           GlobalDecodeEntry.decode_words + index + 1,
           (GlobalDecodeEntry.decode_words_count - index) * sizeof(*GlobalDecodeEntry.decode_words));

    GlobalDecodeEntry.decode_words_count--;
}

void decode_entry_insert_buffer(u8 *buffer, u32 buffer_count)
{
    u32 i;
    u32 word_i;
    for (i = 0, word_i = 0; i < buffer_count; i++)
    {
        printf("i %d\n", i);
        if (buffer[i] == ' ' ||
            buffer[i] == '\n')
        {
            GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_foreign[word_i] = 0;
            GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_native[0] = 0;
            word_i = 0;
            decode_word_append();
        }
        else
        {
            if (buffer[i] == 0xc3)
            {
                i++;
                buffer[i] += 0x40;
            }
            else if (buffer[i] == 0xc5)
            {
                i++;
                buffer[i] += 0xc0;
            }
            GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_foreign[word_i++] = buffer[i];
        }

    }
    GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_foreign[word_i] = 0;
    GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_native[0] = 0;
}

void decode_entry_load(char *path_load)
{
    char *path = malloc((NAME_MAX_SIZE + strlen(decode_entrys_save_path) + 1) * sizeof(*path));
    strcpy(path, decode_entrys_save_path);
    u16 path_count = strlen(path);
    path[path_count++] = '/';

    u8 i;
    for (i = 0; path_load[i] != 0; i++)
        path[i + path_count] = path_load[i];

    path[i + path_count] = 0;
    printf("path load: %s\n", path);

    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return;

    GlobalDecodeEntry.insert_ptr.ptr = NULL;
    GlobalDecodeEntry.insert_ptr.ptr_count = 0;
    GlobalDecodeEntry.insert_ptr.ptr_size_max = 0;
    GlobalDecodeEntry.insert_ptr.charset = NULL;


    fread(&GlobalDecodeEntry.decode_words_count,
          1,
          sizeof(GlobalDecodeEntry.decode_words_count) + sizeof(GlobalDecodeEntry.decode_words_index) + sizeof(GlobalDecodeEntry.decode_word_index),
          file);

#if 0
    printf("lost: %ld %ld\n", 
           sizeof(GlobalDecodeEntry.mode) + (NAME_MAX_SIZE + 1) * sizeof(*GlobalDecodeEntry.name) + sizeof(GlobalDecodeEntry.is_saved),
           sizeof(GlobalDecodeEntry.name));
#endif

    fread(&GlobalDecodeEntry.mode,
          sizeof(GlobalDecodeEntry.mode) + (NAME_MAX_SIZE + 1) * sizeof(*GlobalDecodeEntry.name) + sizeof(GlobalDecodeEntry.is_saved),
          1,
          file);
    printf("%d:saved\n ", GlobalDecodeEntry.is_saved);

#if 0
    GlobalDecodeEntry.decode_words = malloc((DECODE_ENTRY_ALLOC_SIZE / GlobalDecodeEntry.decode_words_count) * DECODE_ENTRY_ALLOC_SIZE * sizeof(*GlobalDecodeEntry.decode_words));
#else
    GlobalDecodeEntry.decode_words = realloc(GlobalDecodeEntry.decode_words,
                                             (DECODE_ENTRY_ALLOC_SIZE + GlobalDecodeEntry.decode_words_count) * sizeof(*GlobalDecodeEntry.decode_words));
#endif
    fread(GlobalDecodeEntry.decode_words,
          sizeof(*GlobalDecodeEntry.decode_words),
          GlobalDecodeEntry.decode_words_count,
          file);

    fclose(file);
}

void decode_entry_save(void)
{
    char *path = malloc((NAME_MAX_SIZE + strlen(decode_entrys_save_path) + 1) * sizeof(*path));
    strcpy(path, decode_entrys_save_path);
    u16 path_count = strlen(path);
    path[path_count++] = '/';

    u8 i;
    for (i = 0; GlobalDecodeEntry.name[i] != 0; i++)
        path[i + path_count] = GlobalDecodeEntry.name[i];

    path[i + path_count] = 0;
    printf("path save: %s\n", path);

    FILE *file = fopen(path, "w+b");
    if (file == NULL)
        return;

    GlobalDecodeEntry.is_saved = true;

    fwrite(&GlobalDecodeEntry.decode_words_count,
           sizeof(GlobalDecodeEntry.decode_words_count) + sizeof(GlobalDecodeEntry.decode_words_index) + sizeof(GlobalDecodeEntry.decode_word_index),
           1,
           file);

    printf("%d:saved\n ", GlobalDecodeEntry.is_saved);

    fwrite(&GlobalDecodeEntry.mode,
           sizeof(GlobalDecodeEntry.mode) + (NAME_MAX_SIZE + 1) * sizeof(*GlobalDecodeEntry.name) + sizeof(GlobalDecodeEntry.is_saved),
           1,
           file);

    fwrite(GlobalDecodeEntry.decode_words,
           sizeof(*GlobalDecodeEntry.decode_words),
           GlobalDecodeEntry.decode_words_count,
           file);

    fclose(file);
}

void decode_entry_scroll_down(void)
{
    GlobalDecodeEntry.scroll_offset++;
}

void decode_entry_scroll_up(void)
{
    if (GlobalDecodeEntry.scroll_offset == 0)
        return; 
    GlobalDecodeEntry.scroll_offset--;
}

void decode_words_index_move_line_down(void)
{
    if (GlobalDecodeEntry.decode_words_index >= GlobalDecodeEntry.lines[GlobalDecodeEntry.lines_count - 1])
        return;
    u32 i;
    for (i = 0; GlobalDecodeEntry.lines_count; i++)
    {
        if (GlobalDecodeEntry.decode_words_index < GlobalDecodeEntry.lines[i + 1])
        {
            GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[i + 1];
            return;
        }
    }
}

void decode_words_index_move_line_up(void)
{
#if 1
    if (GlobalDecodeEntry.decode_words_index < GlobalDecodeEntry.lines[1] ||
        GlobalDecodeEntry.lines_count == 1)
        return;
#endif

    u32 i;
    for (i = 1; GlobalDecodeEntry.lines_count - 1; i++)
    {
        if (GlobalDecodeEntry.decode_words_index < GlobalDecodeEntry.lines[i + 1])
        {
            GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[i - 1];
            return;
        }
    }

    GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[i - 2];
}

void decode_words_index_move_line_start(void)
{
    u32 i;
    for (i = 0; i < GlobalDecodeEntry.lines_count; i++)
    {
        if (GlobalDecodeEntry.decode_words_index < GlobalDecodeEntry.lines[i + 1])
        {
            GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[i];
            return;
        }
    }

    GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[GlobalDecodeEntry.lines_count - 1];
}

void decode_words_index_move_line_end(void)
{
    u32 i;
    for (i = 0; i < GlobalDecodeEntry.lines_count; i++)
    {
        if (GlobalDecodeEntry.decode_words_index < GlobalDecodeEntry.lines[i + 1])
        {
            GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.lines[i + 1] - 1;
            return;
        }
    }

    GlobalDecodeEntry.decode_words_index = GlobalDecodeEntry.decode_words_count - 1;
}

void decode_words_index_move_right(void)
{
    if (GlobalDecodeEntry.decode_words_index >= GlobalDecodeEntry.decode_words_count - 1)
        return;
    GlobalDecodeEntry.decode_words_index++;
}

void decode_words_index_move_left(void)
{
    if (GlobalDecodeEntry.decode_words_index == 0)
        return;
    GlobalDecodeEntry.decode_words_index--;
}

void decode_word_index_move_down(void)
{
    if (GlobalDecodeEntry.decode_word_index >= 1)
        return;
    GlobalDecodeEntry.decode_word_index++;
}

void decode_word_index_move_up(void)
{
    if (GlobalDecodeEntry.decode_word_index == 0)
        return;
    GlobalDecodeEntry.decode_word_index--;
}

void decode_word_index_move_line_start(void)
{
}

void decode_word_index_move_line_end(void)
{
}

void decode_word_append(void)
{
    if (GlobalDecodeEntry.decode_words_count % DECODE_ENTRY_ALLOC_SIZE == 0)
    {
        printf("realloc at count: %d\n",
               GlobalDecodeEntry.decode_words_count);
        printf("new_size reallc %ld\n",
               (GlobalDecodeEntry.decode_words_count + DECODE_ENTRY_ALLOC_SIZE * 6) * sizeof(*GlobalDecodeEntry.decode_words));
        GlobalDecodeEntry.decode_words = realloc(GlobalDecodeEntry.decode_words,
                                                 (GlobalDecodeEntry.decode_words_count + DECODE_ENTRY_ALLOC_SIZE) * sizeof(*GlobalDecodeEntry.decode_words));

        if (GlobalDecodeEntry.decode_words == NULL)
        {
            fprintf(stderr, "fail realloc count: %d\n",
                    GlobalDecodeEntry.decode_words_count);
            exit(1);
        }
    }

    if (GlobalDecodeEntry.decode_words_index != GlobalDecodeEntry.decode_words_count - 1)
    {
        memcpy(GlobalDecodeEntry.decode_words + GlobalDecodeEntry.decode_words_index + 2,
               GlobalDecodeEntry.decode_words + GlobalDecodeEntry.decode_words_index + 1,
               (GlobalDecodeEntry.decode_words_count - GlobalDecodeEntry.decode_words_index) * sizeof(*GlobalDecodeEntry.decode_words));
    }

    GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index + 1].word_foreign[0] = 0;
    GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index + 1].word_native[0] = 0;
    GlobalDecodeEntry.decode_words_index++;
    GlobalDecodeEntry.decode_words_count++;
}

void decode_entry_create(void)
{
    GlobalDecodeEntry.decode_words = malloc(500 * sizeof(*GlobalDecodeEntry.decode_words));
    GlobalDecodeEntry.decode_words->word_foreign[0] = 0;
    GlobalDecodeEntry.decode_words->word_native[0] = 0;
    GlobalDecodeEntry.decode_words->word_have_to_practice = false;
    GlobalDecodeEntry.decode_words_count = 1;
    GlobalDecodeEntry.decode_words_index = 0;
    GlobalDecodeEntry.decode_word_index = 0;
    GlobalDecodeEntry.is_saved = false;
}

#define DECODE_ENTRY_MARGIN_LEFT 20
#define DECODE_ENTRY_MARGIN_RIGHT 20

void decode_entry_render(void)
{
    u16 x = DECODE_ENTRY_MARGIN_LEFT;
    u16 y = GlobalFont.size + STATUS_BAR_PADDING * 2 + 20;

    u16 row_width_max = GlobalGui.width - DECODE_ENTRY_MARGIN_RIGHT;

    u16 word_width_max = 0;
    u16 word_width_native = 0;

    u16 margin_top = 6;
    u16 margin_bottom = 6;
    u16 margin_left = 6;
    u16 margin_right = 6;

    u16 width;
    u16 height;

    u32 color = CONFIG_COLOR_FOREGROUND;

    u32 row_counter = 0;

    GlobalDecodeEntry.lines_count = 0;
    GlobalDecodeEntry.lines = malloc(LINES_ALLOC_SIZE * sizeof(GlobalDecodeEntry.lines));
    GlobalDecodeEntry.lines[GlobalDecodeEntry.lines_count++] = 0;

    u16 i;
    for (i = 0; i < GlobalDecodeEntry.decode_words_count; i++)
    {
        /* max width of words */
        word_width_max = font_width_of_str(GlobalDecodeEntry.decode_words[i].word_foreign);
        word_width_native = font_width_of_str(GlobalDecodeEntry.decode_words[i].word_native);
        if (word_width_native > word_width_max)
            word_width_max = word_width_native;

        width = word_width_max + margin_left + margin_right;
        height = GlobalFont.size + margin_top + margin_bottom;

        if (x + width > row_width_max)
        {
            if (row_counter >= GlobalDecodeEntry.scroll_offset)
                y += height * 2 + 5;
            x = DECODE_ENTRY_MARGIN_LEFT;
            row_counter++;
            if (GlobalDecodeEntry.lines_count % LINES_ALLOC_SIZE == 0)
                GlobalDecodeEntry.lines = realloc(GlobalDecodeEntry.lines, (GlobalDecodeEntry.lines_count + LINES_ALLOC_SIZE) * sizeof(GlobalDecodeEntry.lines));
            GlobalDecodeEntry.lines[GlobalDecodeEntry.lines_count++] = i;
        }

        if (row_counter >= GlobalDecodeEntry.scroll_offset)
        {
            if (i == GlobalDecodeEntry.decode_words_index)
            {
                render_rectangle_fill(x,
                                      y + GlobalDecodeEntry.decode_word_index * (GlobalFont.size + margin_top + margin_bottom),
                                      width,
                                      height - 1,
                                      config_colors[COLOR_WORD_INDEX_MODE_NORMAL + GlobalDecodeEntry.mode]);
            }

            render_rectangle(x,
                             y,
                             width,
                             height,
                             color);

            render_text_utf8(x + margin_left,
                             y + GlobalFont.size + margin_top,
                             GlobalDecodeEntry.decode_words[i].word_foreign,
                             color);

            render_rectangle(x,
                             y + GlobalFont.size + margin_top + margin_bottom - 1,
                             width,
                             height,
                             color);

            render_text_utf8(x + margin_left,
                             y + GlobalFont.size + margin_top + GlobalFont.size + margin_top + margin_bottom - 1,
                             GlobalDecodeEntry.decode_words[i].word_native,
                             color);
        }

        x += width - 1;
    }

    /* print lines test */

#if 0
    for (i = 0; i < GlobalDecodeEntry.lines_count; i++)
    {
        printf("lines[%i]: %i\n", i, GlobalDecodeEntry.lines[i]);
    }
#endif
}
