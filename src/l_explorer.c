#include"ldc_internal.h"

#include<dirent.h>

u32 explorer_index = 0;
u32 explorer_entrys_count = 0;
char explorer_entry_path[256] = { 0 };

void explorer_render_func(void)
{
    u16 x = 50;
    u16 y = 20;
    y += GlobalFont.size;
    render_text(x,
                y,
                "Explorer",
                CONFIG_COLOR_FOREGROUND);
    y += 10;
    render_line_horizontal(0,
                           y,
                           GlobalGui.width,
                           CONFIG_COLOR_FOREGROUND);

    render_text(x - 30,
                y + (explorer_index + 1) * GlobalFont.size,
                "->",
                CONFIG_COLOR_EXPLORER_POINTER);

    DIR *decode_entrys_dir = opendir(decode_entrys_save_path);
    if (decode_entrys_dir == NULL)
        return;

    explorer_entrys_count = 0;

    struct dirent *dir_ptr;
    while ((dir_ptr = readdir(decode_entrys_dir)) != NULL)
    {
        if (dir_ptr->d_name[0] == '.')
            continue;

        y += GlobalFont.size;

        if (explorer_entrys_count == explorer_index)
        {
            strcpy(explorer_entry_path, dir_ptr->d_name);
            render_text(x - 30,
                        y,
                        "->",
                        CONFIG_COLOR_EXPLORER_POINTER);
        }

        render_text(x,
                    y,
                    dir_ptr->d_name,
                    CONFIG_COLOR_FOREGROUND);

        explorer_entrys_count++;
    }

    closedir(decode_entrys_dir);
}

void explorer_input_func(u8 key)
{
    switch (key)
    {
        case KEY_ASCII_ENTER:
        {
            GlobalGui.render_func = render_func;
            GlobalGui.input_func = input_func;
            decode_entry_load(explorer_entry_path);
        } break;

        case KEY_ASCII_ESC:
        {
            GlobalGui.render_func = render_func;
            GlobalGui.input_func = input_func;
        } break;

        case 'd':
        {
            char *path = malloc((NAME_MAX_SIZE + strlen(decode_entrys_save_path) + 1) * sizeof(*path));
            strcpy(path, decode_entrys_save_path);
            u16 path_count = strlen(path);
            path[path_count++] = '/';

            u8 i;
            for (i = 0; explorer_entry_path[i] != 0; i++)
                path[i + path_count] = explorer_entry_path[i];

            path[i + path_count] = 0;

            remove(path);

            if (explorer_index != 0)
                explorer_index--;
        } break;

        case 'n': if (explorer_index < explorer_entrys_count - 1) { explorer_index++; } break;
        case 'r': if (explorer_index == 0) { break; } explorer_index--; break;

        default: break;
    }

    gui_update();
}
