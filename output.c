#include "converter.h"

static const char *error_message(int status)
{
    if (status == ERR_BASE)
        return "base must be 2..16";
    if (status == ERR_OVERFLOW)
        return "number is outside int range";
    if (status == ERR_PRECISION)
        return "at most 12 input fractional digits are supported";
    if (status == ERR_INPUT)
        return "input line is too long or contains a NUL byte";
    if (status == ERR_READ)
        return "cannot read input";
    return "invalid number format";
}

void report_error(int status, int base, FILE *stream)
{
    if (status == ERR_DIGIT)
        fprintf(stream, "error: invalid digit for base %d\n", base);
    else
        fprintf(stream, "error: %s\n", error_message(status));
}

int print_conversion(const t_number *number, int base, const char *prefix)
{
    char buffer[OUTPUT_SIZE];
    int status;

    status = number_to_base(number, base, buffer);
    if (status < 0)
    {
        report_error(status, base, stderr);
        return 1;
    }
    printf("%s%s\n", prefix, buffer);
    if (status > 0)
        fprintf(stderr, "warning: precision loss in base %d "
            "(truncated after 12 fractional digits)\n", base);
    return 0;
}
