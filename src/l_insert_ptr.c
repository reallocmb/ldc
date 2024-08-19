#include"ldc_internal.h"

bool charset_check(Charset *charset, u8 byte)
{
    if (charset == NULL)
        return false;

    u16 i;
    for (i = 0; i < charset->ranges_count; i++)
    {
        if (byte >= charset->ranges[i][0] &&
            byte <= charset->ranges[i][1])
            return true;
    }

    return false;
}
void insert_ptr_attach(InsertPtr *insert_ptr, u8 *ptr, u32 size_max, Charset *charset)
{
    insert_ptr->ptr = ptr;
    insert_ptr->ptr_count = 0;
    insert_ptr->ptr_size_max = size_max;
    insert_ptr->charset = charset;
}

void insert_ptr_append_byte(InsertPtr *insert_ptr, u8 byte)
{
    if (insert_ptr->ptr == NULL)
        return;

    if (byte == KEY_ASCII_BACKSPACE)
    {
        if (insert_ptr->ptr_count == 0)
            return;
        insert_ptr->ptr[--insert_ptr->ptr_count] = 0;
        return;
    }

    if (!charset_check(insert_ptr->charset, byte))
        return;

    if (insert_ptr->ptr_count >= insert_ptr->ptr_size_max)
    {
        insert_ptr->ptr_count = 0;
        insert_ptr->ptr[0] = 0;
    }
    insert_ptr->ptr[insert_ptr->ptr_count++] = byte;
    insert_ptr->ptr[insert_ptr->ptr_count] = 0;
}

