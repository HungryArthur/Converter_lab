#include "converter.h"

int main(int argc, char **argv)
{
    int choice;

    if (argc != 1)
        return run_cli(argc, argv);
    puts("1) dec -> base (2..16)\n2) base (2..16) -> dec\n3) exit");
    choice = 0;
    while (choice != 3)
    {
        choice = read_menu_choice();
        if ((choice == 1 || choice == 2) && run_dialog(choice))
            return 0;
    }
    return 0;
}
