#include "converter.h"

#include <string.h>

static int parse_fraction(const char *digits, int base, t_number *number)
{
    int count;

    count = 0;
    if (*digits == '\0')
        return ERR_FORMAT;
    while (*digits != '\0')
    {
        int digit = digit_value(*digits++);

        if (digit < 0 || digit >= base)
            return ERR_DIGIT;
        if (count++ == FRACTION_DIGITS)
            return ERR_PRECISION;
        number->fraction = number->fraction * (unsigned int)base + digit;
        number->scale *= (unsigned int)base;
    }
    if (number->fraction != 0 &&
        (number->whole == INT_MIN || number->whole == INT_MAX))
        return ERR_OVERFLOW;
    return CONVERT_OK;
}

int parse_number(const char *digits, int base, t_number *number)
{
    const char *cursor;
    int status;

    if (base < 2 || base > 16)
        return ERR_BASE;
    if (digits == NULL || number == NULL)
        return ERR_FORMAT;
    *number = (t_number){0, 0, 1, *digits == '-'};
    if (strchr(digits, '.') == NULL)
        return base_to_dec(digits, base, &number->whole);
    cursor = digits;
    if (*cursor == '-' || *cursor == '+')
        cursor++;
    status = parse_whole(&cursor, base, number->negative, &number->whole);
    if (status != CONVERT_OK)
        return status;
    return parse_fraction(cursor + 1, base, number);
}

static int append_fraction(const t_number *number, int base,
    char *buffer, size_t length)
{
    unsigned long long remainder;
    int count;

    remainder = number->fraction;
    count = 0;
    if (remainder != 0)
        buffer[length++] = '.';
    while (remainder != 0 && count++ < FRACTION_DIGITS)
    {
        remainder *= (unsigned int)base;
        buffer[length++] = digit_char((unsigned int)(remainder / number->scale));
        remainder %= number->scale;
    }
    buffer[length] = '\0';
    return remainder != 0;
}

int number_to_base(const t_number *number, int base, char *buffer)
{
    int length;
    int sign;

    if (number == NULL || buffer == NULL)
        return ERR_FORMAT;
    sign = number->negative && number->whole == 0 && number->fraction != 0;
    if (sign)
        buffer[0] = '-';
    length = dec_to_base(number->whole, base, buffer + sign);
    if (length < 0)
        return length;
    return append_fraction(number, base, buffer, (size_t)(length + sign));
}
