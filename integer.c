#include "converter.h"

int digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

char digit_char(unsigned int digit)
{
    return "0123456789ABCDEF"[digit];
}

unsigned int int_magnitude(int value)
{
    if (value < 0)
        return 0U - (unsigned int)value;
    return (unsigned int)value;
}

static void reverse_digits(char *buffer, size_t begin, size_t end)
{
    while (begin < end)
    {
        char temporary = buffer[begin];

        buffer[begin++] = buffer[end];
        buffer[end--] = temporary;
    }
}

int dec_to_base(int value, int base, char *buffer)
{
    unsigned int magnitude;
    size_t length;

    if (base < 2 || base > 16)
        return ERR_BASE;
    if (buffer == NULL)
        return ERR_FORMAT;
    magnitude = int_magnitude(value);
    length = 0;
    if (value < 0)
        buffer[length++] = '-';
    if (magnitude == 0)
        buffer[length++] = '0';
    while (magnitude != 0)
    {
        buffer[length++] = digit_char(magnitude % (unsigned int)base);
        magnitude /= (unsigned int)base;
    }
    buffer[length] = '\0';
    reverse_digits(buffer, value < 0 ? 1 : 0, length - 1);
    return (int)length;
}

int parse_whole(const char **cursor, int base, int negative, int *whole)
{
    unsigned int value;
    unsigned int limit;
    const char *start;

    value = 0;
    limit = (unsigned int)INT_MAX + (unsigned int)negative;
    start = *cursor;
    while (**cursor != '\0' && **cursor != '.')
    {
        int digit = digit_value(**cursor);

        if (digit < 0 || digit >= base)
            return ERR_DIGIT;
        if (value > (limit - (unsigned int)digit) / (unsigned int)base)
            return ERR_OVERFLOW;
        value = value * (unsigned int)base + (unsigned int)digit;
        (*cursor)++;
    }
    if (*cursor == start)
        return ERR_FORMAT;
    if (negative && value == (unsigned int)INT_MAX + 1U)
        *whole = INT_MIN;
    else
        *whole = negative ? -(int)value : (int)value;
    return CONVERT_OK;
}

int base_to_dec(const char *digits, int base, int *result_out)
{
    const char *cursor;
    int status;

    if (base < 2 || base > 16)
        return ERR_BASE;
    if (digits == NULL || result_out == NULL)
        return ERR_FORMAT;
    cursor = digits;
    if (*cursor == '-' || *cursor == '+')
        cursor++;
    status = parse_whole(&cursor, base, *digits == '-', result_out);
    if (status != CONVERT_OK)
        return status;
    if (*cursor != '\0')
        return ERR_FORMAT;
    return CONVERT_OK;
}
