#include "converter.h"

#include <string.h>

static int convert_arguments(char **argv)
{
    t_number number;
    int base;
    int status;

    status = parse_base(argv[2], &base);
    if (status != CONVERT_OK)
    {
        report_error(status, 10, stderr);
        return 1;
    }
    status = parse_number(argv[1], 10, &number);
    if (status == ERR_DIGIT || status == ERR_FORMAT)
    {
        status = parse_number(argv[1], base, &number);
        base = status == CONVERT_OK ? 10 : base;
    }
    if (status != CONVERT_OK)
    {
        report_error(status, base, stderr);
        return 1;
    }
    return print_conversion(&number, base, "");
}

static int print_table(const char *text)
{
    t_number number;
    int status;
    int base;
    char prefix[16];

    status = parse_number(text, 10, &number);
    if (status != CONVERT_OK)
    {
        report_error(status, 10, stderr);
        return 1;
    }
    base = 2;
    while (base <= 16)
    {
        snprintf(prefix, sizeof(prefix), "base %2d: ", base);
        if (print_conversion(&number, base, prefix) != 0)
            return 1;
        base++;
    }
    return 0;
}

static void print_steps(int value, int base)
{
    unsigned int magnitude;

    magnitude = int_magnitude(value);
    if (value < 0)
        puts("sign: -");
    if (magnitude == 0)
        printf("0 / %d = 0, remainder 0 (0)\n", base);
    while (magnitude != 0)
    {
        printf("%u / %d = %u, remainder %u (%c)\n", magnitude, base,
            magnitude / (unsigned int)base, magnitude % (unsigned int)base,
            digit_char(magnitude % (unsigned int)base));
        magnitude /= (unsigned int)base;
    }
}

static int run_steps(char **argv)
{
    t_number number;
    int base;
    int status;

    status = parse_base(argv[3], &base);
    if (status != CONVERT_OK)
    {
        report_error(status, 10, stderr);
        return 1;
    }
    status = parse_number(argv[2], 10, &number);
    if (status == CONVERT_OK && strchr(argv[2], '.') != NULL)
        status = ERR_FORMAT;
    if (status != CONVERT_OK)
    {
        report_error(status, 10, stderr);
        return 1;
    }
    print_steps(number.whole, base);
    return print_conversion(&number, base, "result: ");
}

int run_cli(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "-table") == 0)
        return print_table(argv[2]);
    if (argc == 4 && strcmp(argv[1], "-step") == 0)
        return run_steps(argv);
    if (argc == 3)
        return convert_arguments(argv);
    fputs("usage:\n  converter\n  converter NUMBER BASE\n"
        "  converter -table NUMBER\n  converter -step INTEGER BASE\n", stderr);
    return 1;
}
