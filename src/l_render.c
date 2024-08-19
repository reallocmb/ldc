#include"ldc_internal.h"
#include"itypes.h"

u32 color_alpha_blending(u32 color0, u32 color1, u32 alpha)
{
    u32 alpha1 = 0xff - alpha;

#if 0
    u32 red0 = (color0 >> 16) & 0xff;
    u32 green0 = (color0 >> 8) & 0xff;
    u32 blue0 = color0 & 0xff;

    u32 red1 = (color1 >> 16) & 0xff;
    u32 green1 = (color1 >> 8) & 0xff;
    u32 blue1 = color1 & 0xff;

    u32 result_red = (alpha * red0 + alpha1 * red1) / 255;
    u32 result_green = (alpha * green0 + alpha1 * green1) / 255;
    u32 result_blue = (alpha * blue0 + alpha1 * blue1) / 255;

    return (result_red << 16) | (result_green) << 8 | result_blue;
#else


    return (((alpha * ((color0 >> 16) & 0xff) + alpha1 * ((color1 >> 16) & 0xff)) / 255) << 16) |
        (((alpha * ((color0 >> 8) & 0xff) + alpha1 * ((color1 >> 8) & 0xff)) / 255) << 8) |
        ((alpha * (color0 & 0xff) + alpha1 * (color1 & 0xff)) / 255);
#endif
}

void render(void)
{
    /* clear screen */
    u32 bits_count = GlobalGui.width * GlobalGui.height;
    while (bits_count--)
        GlobalGui.bits[bits_count] = CONFIG_COLOR_BACKGROUND;

    GlobalGui.render_func();
}

void render_line_horizontal_max(u16 x, u16 y, u16 width, u32 color)
{
    if (y >= GlobalGui.height)
        return;

    if (x > GlobalGui.width)
        return;

    if (x + width > GlobalGui.width)
        width = GlobalGui.width - x;

    while (width--)
        GlobalGui.bits[y * GlobalGui.width + x + width] = color_alpha_blending(color, GlobalGui.bits[y * GlobalGui.width + x + width], color >> 24);
}

void render_line_horizontal(u16 x, u16 y, u16 width, u32 color)
{
    if (y >= GlobalGui.height)
        return;

    if (x > GlobalGui.width)
        return;

    if (x + width > GlobalGui.width)
        width = GlobalGui.width - x;

    while (width--)
        GlobalGui.bits[y * GlobalGui.width + x + width] = color_alpha_blending(color, GlobalGui.bits[y * GlobalGui.width + x + width], color >> 24);
}

void render_line_vertical(u16 x, u16 y, u16 height, u32 color)
{
    if (x >= GlobalGui.width)
        return;

    if (y > GlobalGui.height)
        return;

    if (y + height > GlobalGui.height)
        height = GlobalGui.height - y;

    while (height--)
        GlobalGui.bits[(height + y) * GlobalGui.width + x] = color_alpha_blending(color, GlobalGui.bits[(height + y) * GlobalGui.width + x], color >> 24);
}

void render_rectangle(u16 x, u16 y, u16 width, u16 height, u32 color)
{
    render_line_horizontal(x, y, width, color);
    render_line_vertical(x, y, height, color);
    render_line_horizontal(x, y + height - 1, width, color);
    render_line_vertical(x + width - 1, y, height, color);
}

void render_rectangle_fill(u16 x, u16 y, u16 width, u16 height, u32 color)
{
    if (x >= GlobalGui.width)
        return;
    if (y >= GlobalGui.height)
        return;

    if (y + height > GlobalGui.height)
        height = GlobalGui.height - y;
    if (x + width > GlobalGui.width)
        width = GlobalGui.width - x;

    u32 position;

    u16 i;
    while (height--)
        for (i = 0; i < width; i++)
        {
            position = (y + height) * GlobalGui.width + x + i;
            GlobalGui.bits[position] = color_alpha_blending(color & 0xffffff, GlobalGui.bits[position], color >> 24);
        }
}

void render_text_utf8(u16 x, u16 y, const u8 *text, u32 color)
{
    if (x > GlobalGui.width || y > GlobalGui.height)
        return;
    FT_GlyphSlot slot = GlobalFont.face->glyph;

    u32 i;
    u32 glyph_index;
#if 0
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.y_ppem), 200, 0xffaabbff);
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.ascender >> 6), 100, 0xff0000ff);
    render_line_horizontal(x, y, 100, 0xffff0000);
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.descender >> 6), 100, 0xff00ff00);
    render_line_vertical(x, y - 100, 100, 0xffffff00);
#endif

    if (y - (GlobalFont.face->size->metrics.ascender >> 6) < 0)
        return;
    if (y - (GlobalFont.face->size->metrics.descender >> 6) > GlobalGui.height)
        return;

    for (i = 0; text[i] != 0; i++)
    {

        glyph_index = FT_Get_Char_Index(GlobalFont.face,
                                        text[i]);

        FT_Load_Glyph(GlobalFont.face,
                      glyph_index,
                      FT_LOAD_DEFAULT);

        FT_Render_Glyph(GlobalFont.face->glyph,
                        FT_RENDER_MODE_NORMAL);

#if 0
        render_line_vertical(x + (slot->advance.x >> 6), y - 100, 100, 0xffff00);
#endif
        if (x + (slot->advance.x >> 6) > GlobalGui.width)
            return;

        u32 j, k;
        u32 position;
        for (k = 0; k < slot->bitmap.rows; k++)
        {
            for (j = 0; j < slot->bitmap.width; j++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[k * slot->bitmap.pitch + j];
                position = (k + y - slot->bitmap_top) * GlobalGui.width + x + j; 
                GlobalGui.bits[position] = color_alpha_blending(color, GlobalGui.bits[position], bitmap_alpha);
                //GlobalGui.bits[position] = color_alpha_blending(color_alpha_blending(color, GlobalGui.bits[position], color >> 24), GlobalGui.bits[position], bitmap_alpha);
                // GlobalGui.bits[position] = 0x000000;
            }
        }
        
        x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
}

void render_text(u16 x, u16 y, const char *text, u32 color)
{
    if (x > GlobalGui.width || y > GlobalGui.height)
        return;
    FT_GlyphSlot slot = GlobalFont.face->glyph;

    u32 i;
    u32 glyph_index;
#if 0
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.y_ppem), 200, 0xffaabbff);
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.ascender >> 6), 100, 0xff0000ff);
    render_line_horizontal(x, y, 100, 0xffff0000);
    render_line_horizontal(x, y - (GlobalFont.face->size->metrics.descender >> 6), 100, 0xff00ff00);
    render_line_vertical(x, y - 100, 100, 0xffffff00);
#endif

    if (y - (GlobalFont.face->size->metrics.ascender >> 6) < 0)
        return;
    if (y - (GlobalFont.face->size->metrics.descender >> 6) > GlobalGui.height)
        return;

    for (i = 0; i < strlen(text); i++)
    {

        glyph_index = FT_Get_Char_Index(GlobalFont.face,
                                        text[i]);

        FT_Load_Glyph(GlobalFont.face,
                      glyph_index,
                      FT_LOAD_DEFAULT);

        FT_Render_Glyph(GlobalFont.face->glyph,
                        FT_RENDER_MODE_NORMAL);

#if 0
        render_line_vertical(x + (slot->advance.x >> 6), y - 100, 100, 0xffff00);
#endif
        if (x + (slot->advance.x >> 6) > GlobalGui.width)
            return;

        u32 j, k;
        u32 position;
        for (k = 0; k < slot->bitmap.rows; k++)
        {
            for (j = 0; j < slot->bitmap.width; j++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[k * slot->bitmap.pitch + j];
                position = (k + y - slot->bitmap_top) * GlobalGui.width + x + j; 
                GlobalGui.bits[position] = color_alpha_blending(color, GlobalGui.bits[position], bitmap_alpha);
                //GlobalGui.bits[position] = color_alpha_blending(color_alpha_blending(color, GlobalGui.bits[position], color >> 24), GlobalGui.bits[position], bitmap_alpha);
                // GlobalGui.bits[position] = 0x000000;
            }
        }
        
        x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
}

#define COLOR_A(color, alpha) color | (alpha << 24)
#define COLOR_RGB(r, g, b) (r << 16) | (g << 8) | b | (0xff << 24)
#define COLOR_RGB_A(r, g, b, alpha) (r << 16) | (g << 8) | b | (alpha << 24)

void render_default_func(void)
{
#if 1
    render_line_vertical(200, 200, 100, 0xff000000);
    render_line_horizontal(50, 60, 300, 0xff000000);
    render_text(400, 599 - 5, "Geht es dir gut?", 0x0);
    render_text(400, 599 - 25, "Ja mir geht es gut! Und dir?", 0x0);

    render_rectangle_fill(45, 30, 400, 400, COLOR_RGB_A(0xaa, 0xcc, 0x50, 0x22));
    render_text(50, 50, "Matthias Brunner", 0x0);
#endif

}
