#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p validation
make re > validation/linux-build.txt 2>&1
sh tests.sh > validation/tests-linux.txt 2>&1
cppcheck --enable=all --error-exitcode=1 --std=c11 --check-level=exhaustive \
    --suppress=missingIncludeSystem main.c integer.c fraction.c input.c output.c cli.c \
    > validation/cppcheck.txt 2>&1
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-dialog.txt ./converter < tests/dialog_examples.in > /dev/null
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-invalid.txt ./converter < tests/dialog_invalid.in > /dev/null
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-boundaries.txt ./converter < tests/dialog_boundaries.in > /dev/null
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-fraction.txt ./converter < tests/dialog_fraction.in > /dev/null
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-table.txt ./converter -table 255 > /dev/null
valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
    --log-file=validation/valgrind-step.txt ./converter -step -472 2 > /dev/null
printf 'Build, 45 tests, Cppcheck and 6 Valgrind runs passed.\n'
