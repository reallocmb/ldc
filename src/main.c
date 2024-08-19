#include"ldc_internal.h"
#include"l_xlib.c"
#include"l_render.c"
#include"l_font.c"
#include"l_status_bar.c"
#include"l_insert_ptr.c"
#include"l_decode_entry.c"
#include"l_explorer.c"
#include"l_pdf.c"
#include"l_clipboard.c"

#include<sys/stat.h>

void render_func(void)
{
    status_bar_render();

    decode_entry_render();
}

void input_func(u8 key)
{
    if (GlobalDecodeEntry.mode == MODE_NORMAL)
    {
        switch (key)
        {
            case 'q': exit(0); break;
            case 'e':
            {
                GlobalGui.render_func = explorer_render_func;
                GlobalGui.input_func = explorer_input_func;

                printf("%d:saved\n ", GlobalDecodeEntry.is_saved);
            } break;

            case 'c':
            {
                GlobalDecodeEntry.mode = MODE_NAME_EDIT;
                insert_ptr_attach(&GlobalDecodeEntry.insert_ptr,
                                  GlobalDecodeEntry.name,
                                  NAME_MAX_SIZE,
                                  &charset_name);
            } break;

            case 'i':
            {
                GlobalDecodeEntry.mode = MODE_INSERT;
                insert_ptr_attach(&GlobalDecodeEntry.insert_ptr,
                                  GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_foreign + GlobalDecodeEntry.decode_word_index * (WORD_MAX_SIZE + 1),
                                  NAME_MAX_SIZE,
                                  &charset_word);
            } break;

            case 'p':
            {
                pdf_create();
            } break;

            case KEY_ASCII_CONTROL_V:
            {
                u32 buffer_count;
                u8 *buffer = clipboard_func(&buffer_count);
#if 0
                printf("buffer count:%d\n%s\n", buffer_count, buffer);
#endif

                decode_entry_insert_buffer(buffer, buffer_count);

                free(buffer);
            } break;

            case 'd': decode_word_delete(GlobalDecodeEntry.decode_words_index); break;
            case 'w': decode_entry_save(); break;
            case 'a': decode_word_append(); break;
            case 's': decode_words_index_move_left(); break;
            case 't': decode_words_index_move_right(); break;
            case 'r': decode_word_index_move_up(); break;
            case 'n': decode_word_index_move_down(); break;
            case 'R': decode_words_index_move_line_up(); break;
            case 'N': decode_words_index_move_line_down(); break;
            case 'S': decode_words_index_move_line_start(); break;
            case 'T': decode_words_index_move_line_end(); break;
            case KEY_ASCII_CONTROL_U: decode_entry_scroll_up(); break;
            case KEY_ASCII_CONTROL_D: decode_entry_scroll_down(); break;
        }
    }
    else if (GlobalDecodeEntry.mode == MODE_INSERT)
    {
        switch (key)
        {
            case KEY_ASCII_ESC:
            {
                GlobalDecodeEntry.mode = MODE_NORMAL;
            } break;
            
            case ' ':
            {
                if (GlobalDecodeEntry.decode_word_index == 0)
                    decode_word_append();
                else
                    decode_words_index_move_right();
                insert_ptr_attach(&GlobalDecodeEntry.insert_ptr,
                                  GlobalDecodeEntry.decode_words[GlobalDecodeEntry.decode_words_index].word_foreign + GlobalDecodeEntry.decode_word_index * (WORD_MAX_SIZE + 1),
                                  NAME_MAX_SIZE,
                                  &charset_word);
            } break;

            default:
            {
                insert_ptr_append_byte(&GlobalDecodeEntry.insert_ptr, key);
                GlobalDecodeEntry.is_saved = false;
            } break;
        }
    }
    else if (GlobalDecodeEntry.mode == MODE_NAME_EDIT)
    {
        switch (key)
        {
            case KEY_ASCII_ENTER:
            {
                GlobalDecodeEntry.mode = MODE_NORMAL;
                decode_entry_create();
            } break;
        }
        insert_ptr_append_byte(&GlobalDecodeEntry.insert_ptr, key);
    }
    gui_update();
}

int main(void)
{
#ifdef RELEASE
    char *home_path = getenv("HOME"); /* /home/mb/. */
    decode_entrys_save_path = malloc((18 + 1 + strlen(home_path) + 1) * sizeof(*decode_entrys_save_path));
    strcpy(decode_entrys_save_path, home_path);
    strcat(decode_entrys_save_path, "/");
    strcat(decode_entrys_save_path, DECODE_ENTRYS_SAVE_DIRECTORY);
#else
    decode_entrys_save_path = malloc((18 + 1) * sizeof(*decode_entrys_save_path));
    strcpy(decode_entrys_save_path, DECODE_ENTRYS_SAVE_DIRECTORY);
#endif
    printf("path: %s\n", decode_entrys_save_path);
    mkdir(decode_entrys_save_path, 0777);

#if 0
    u32 buffer_count;
    u8 *buffer = clipboard_func(&buffer_count);
    printf("buffer count:%d\n%s\n", buffer_count, buffer);
    free(buffer);
#else
#if 0
    pdf_create();
    exit(0);
#endif
#if 0
    font_init("assets/fonts/SourceCodePro-Medium.otf", 20);
#elif 0
    font_init("assets/fonts/FiraCode-Bold.ttf", 20);
#elif 0
    font_init("assets/fonts/FiraCode-Light.ttf", 20);
#else
    font_init("/usr/share/fonts/TTF/FiraCode-Light.ttf", 20);
#endif

    gui_init(800, 600);

    GlobalGui.render_func = render_func;
    GlobalGui.input_func = input_func;

    gui_loop();
#endif

    return 0;
}
