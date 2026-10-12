#include "converter.h"

#include <ctype.h>
#include <string.h>

static void trim_input(char *buffer)
{
    size_t begin;
    size_t end;

    begin = 0;
    end = strlen(buffer);
    while (begin < end && isspace((unsigned char)buffer[begin]))
        begin++;
    while (end > begin && isspace((unsigned char)buffer[end - 1]))
        end--;
    memmove(buffer, buffer + begin, end - begin);
    buffer[end - begin] = '\0';
}

int read_line(char *buffer)
{
    size_t length;
    int c;
    int invalid;

    length = 0;
    invalid = 0;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (length < INPUT_SIZE - 1 && c != '\0')
            buffer[length++] = (char)c;
        else
            invalid = 1;
    }
    buffer[length] = '\0';
    if (ferror(stdin))
        return ERR_READ;
    if (invalid)
        return ERR_INPUT;
    if (c == EOF && length == 0)
        return END_INPUT;
    trim_input(buffer);
    return CONVERT_OK;
}

int ask_line(const char *prompt, char *buffer)
{
    int status;

    fputs(prompt, stdout);
    fflush(stdout);
    status = read_line(buffer);
    if (status < 0)
        report_error(status, 10, stdout);
    if (status == ERR_READ)
        return END_INPUT;
    return status;
}

int parse_base(const char *text, int *base)
{
    if (base_to_dec(text, 10, base) != CONVERT_OK)
        return ERR_BASE;
    if (*base < 2 || *base > 16)
        return ERR_BASE;
    return CONVERT_OK;
}

int read_menu_choice(void)
{
    char buffer[INPUT_SIZE];
    int choice;
    int status;

    status = ask_line("> ", buffer);
    if (status == END_INPUT)
        return 3;
    if (status != CONVERT_OK)
        return 0;
    if (base_to_dec(buffer, 10, &choice) != CONVERT_OK ||
        choice < 1 || choice > 3)
    {
        puts("error: menu choice must be 1, 2 or 3");
        return 0;
    }
    return choice;
}

static int request_base(void)
{
    char buffer[INPUT_SIZE];
    int base;
    int status;

    status = ask_line("base: ", buffer);
    if (status == END_INPUT)
        return 0;
    if (status != CONVERT_OK)
        return -1;
    if (parse_base(buffer, &base) != CONVERT_OK)
    {
        report_error(ERR_BASE, 10, stdout);
        return -1;
    }
    return base;
}

static int check_number(const char *text, int base, t_number *number)
{
    int status;

    status = parse_number(text, base, number);
    if (status != CONVERT_OK)
        report_error(status, base, stdout);
    return status;
}

int run_dialog(int choice)
{
    char text[INPUT_SIZE];
    t_number number;
    int base;
    int status;

    if (choice != 1 && choice != 2)
        return 0;
    status = ask_line(choice == 1 ? "number: " : "digits: ", text);
    if (status != CONVERT_OK)
        return status == END_INPUT;
    if (choice == 1 && check_number(text, 10, &number) != CONVERT_OK)
        return 0;
    base = request_base();
    if (base <= 0)
        return base == 0;
    if (choice == 2 && check_number(text, base, &number) != CONVERT_OK)
        return 0;
    print_conversion(&number, choice == 1 ? base : 10, "result: ");
    return 0;
}
