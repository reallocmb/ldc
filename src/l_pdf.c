#include"ldc_internal.h"

#include<hpdf.h>

void pdf_create(void)
{
#if 1
    HPDF_Doc pdf;

    pdf = HPDF_New(NULL, NULL);
    if (!pdf)
        printf("error: HDPF_New() failed\n");

    HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);

    HPDF_Page page;
    page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    float page_width = HPDF_Page_GetWidth(page);
    float page_height = HPDF_Page_GetHeight(page);

#if 0
    printf("width: %f\n", page_width);
    printf("height: %f\n", page_height);
#endif

    float margin_top = 50;
    float margin_right = 50;
    float margin_bottom = 50;
    float margin_left = 50;

    HPDF_Font font = HPDF_GetFont(pdf, "Times-Roman", "WinAnsiEncoding");
    HPDF_Page_SetFontAndSize(page, font, 20);

#if 0
    float font_height = HPDF_Font_GetXHeight(font);
#endif

    float x = margin_left;
    float y = page_height - margin_top - 20;

    HPDF_Page_SetRGBFill(page, 0.7, 0.4, 0.6);

    u16 i;
    for (i = 0; i < GlobalDecodeEntry.decode_words_count; i++)
    {
        float word_width_max = HPDF_Page_TextWidth(page, (char *)GlobalDecodeEntry.decode_words[i].word_foreign);
        float word_width_native = HPDF_Page_TextWidth(page, (char *)GlobalDecodeEntry.decode_words[i].word_native);
        if (word_width_native > word_width_max)
            word_width_max = word_width_native;

        float width = word_width_max + 5 + 5;
        float height = 20 + 5 + 5;
#if 0
        printf("font_height: %f\n", font_height);
#endif

        if (x + width >= page_width - margin_right)
        {
            x = margin_left;
            y -= height * 2 + 5;
            if (y < margin_bottom)
            {
#if 1
                y = page_height - margin_top - 20;

                page = HPDF_AddPage(pdf);
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                HPDF_Page_SetFontAndSize(page, font, 20);
                HPDF_Page_SetRGBFill(page, 0.7, 0.4, 0.6);
                printf("test\n");
#endif
            }
        }

#if 1
        HPDF_Page_SetRGBFill(page, 1, 1, 1);
        HPDF_Page_Rectangle(page, x, y, width, height);
        HPDF_Page_FillStroke(page);

        HPDF_Page_SetRGBFill(page, 0, 0, 0);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x + 5, y + 5, (char *)GlobalDecodeEntry.decode_words[i].word_foreign);
        HPDF_Page_EndText(page);



        HPDF_Page_SetRGBFill(page, 1, 1, 1);
        HPDF_Page_Rectangle(page, x, y - height, width, height);
        HPDF_Page_FillStroke(page);

        HPDF_Page_SetRGBFill(page, 0, 0, 0);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x + 5, y - height + 5, (char *)GlobalDecodeEntry.decode_words[i].word_native);
        HPDF_Page_EndText(page);
#endif


        x += width;
    }

    char pdf_output_name[50] = "decode.pdf";

    char *path = malloc((NAME_MAX_SIZE + strlen(decode_entrys_save_path) + 1) * sizeof(*path));
    strcpy(path, decode_entrys_save_path);
    u16 path_count = strlen(path);
    path[path_count++] = '/';

    for (i = 0; pdf_output_name[i] != 0; i++)
        path[i + path_count] = pdf_output_name[i];

    path[i + path_count] = 0;

    HPDF_SaveToFile(pdf, path);
    HPDF_Free(pdf);
#endif
}
