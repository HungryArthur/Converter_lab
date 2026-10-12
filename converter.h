#ifndef CONVERTER_H
#define CONVERTER_H

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#define INPUT_SIZE 256
#define FRACTION_DIGITS 12
#define OUTPUT_SIZE (sizeof(int) * CHAR_BIT + FRACTION_DIGITS + 4)

enum e_status
{
    CONVERT_OK = 0,
    ERR_BASE = -1,
    ERR_DIGIT = -2,
    ERR_OVERFLOW = -3,
    ERR_FORMAT = -4,
    ERR_PRECISION = -5,
    ERR_INPUT = -6,
    ERR_READ = -7,
    END_INPUT = 1
};

typedef struct s_number
{
    int whole;
    unsigned long long fraction;
    unsigned long long scale;
    int negative;
} t_number;

/* Output buffers must hold at least OUTPUT_SIZE bytes. */
int digit_value(char c);
char digit_char(unsigned int digit);
unsigned int int_magnitude(int value);
int dec_to_base(int value, int base, char *buffer);
int base_to_dec(const char *digits, int base, int *result_out);
int parse_whole(const char **cursor, int base, int negative, int *whole);
int parse_number(const char *digits, int base, t_number *number);
int number_to_base(const t_number *number, int base, char *buffer);
int parse_base(const char *text, int *base);
void report_error(int status, int base, FILE *stream);
int print_conversion(const t_number *number, int base, const char *prefix);
int read_line(char *buffer);
int ask_line(const char *prompt, char *buffer);
int read_menu_choice(void);
int run_dialog(int choice);
int run_cli(int argc, char **argv);

#endif
