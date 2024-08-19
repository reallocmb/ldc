#include"ldc_internal.h"

void status_bar_render(void)
{
    u16 text_y = GlobalFont.size + STATUS_BAR_PADDING;
    u16 status_bar_height = text_y + STATUS_BAR_PADDING;

    render_line_horizontal(0,
                           status_bar_height,
                           GlobalGui.width,
                           CONFIG_COLOR_FOREGROUND);
    render_text(10,
                text_y,
                config_modes[GlobalDecodeEntry.mode],
                CONFIG_COLOR_FOREGROUND);


    u32 color_name = 0xffff0000; /* red */
    if (GlobalDecodeEntry.is_saved)
        color_name = 0xff00ff00; /* green */

    render_text_utf8(GlobalGui.width / 2 - font_width_of_str(GlobalDecodeEntry.name) / 2,
                     text_y,
                     GlobalDecodeEntry.name,
                     color_name);
}
