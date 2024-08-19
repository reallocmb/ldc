#include"ldc_internal.h"

void font_size_set(u32 font_size)
{
    GlobalFont.size = font_size;

    u32 error;
    error = FT_Set_Pixel_Sizes(GlobalFont.face, 0, font_size);
    if (error)
        printf("%s\n", FT_Error_String(error));
}

void font_init(const char *path, u32 font_size)
{
    u32 error;
    error = FT_Init_FreeType(&GlobalFont.library);
    if (error)
        fprintf(stderr,
                "FILE: %s, LINE: %u -> FT_Init_FreeType() failed\n",
                __FILE__,
                __LINE__);


    error = FT_New_Face(GlobalFont.library,
                        path,
                        0,
                        &GlobalFont.face);

    font_size_set(font_size);

    if (error)
        printf("%s\n", FT_Error_String(error));
}

u16 font_width_by_str_size(u32 size)
{
    return (GlobalFont.face->size->metrics.max_advance >> 6) * size;
}

u16 font_width_of_str(u8 *str)
{
    u16 i;
    for (i = 0; str[i] != 0; i++) { }
    return (GlobalFont.face->size->metrics.max_advance >> 6) * i;
}
