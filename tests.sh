#!/bin/sh
set -u
cd "$(dirname "$0")" || exit 1
if [ -z "${PROG:-}" ]; then
    if [ -f ./converter ]; then PROG=./converter; else PROG=./converter.exe; fi
fi
test_tmp=$(mktemp -d './.test-results.XXXXXX') || exit 1
cleanup() {
    rm -f "$test_tmp/out" "$test_tmp/err" "$test_tmp/out.clean" "$test_tmp/err.clean"
    rmdir "$test_tmp"
}
trap cleanup EXIT HUP INT TERM
passed=0
failed=0

check_case() {
    case_name=$1
    expected_status=$2
    shift 2
    input_file=/dev/null
    if [ -f "tests/$case_name.in" ]; then input_file="tests/$case_name.in"; fi
    if cat "$input_file" | "$PROG" "$@" > "$test_tmp/out" 2> "$test_tmp/err"; then
        actual_status=0
    else
        actual_status=$?
    fi
    tr -d '\r' < "$test_tmp/out" > "$test_tmp/out.clean"
    tr -d '\r' < "$test_tmp/err" > "$test_tmp/err.clean"
    if diff -u "tests/$case_name.out" "$test_tmp/out.clean" &&
       diff -u "tests/$case_name.err" "$test_tmp/err.clean" &&
       [ "$actual_status" -eq "$expected_status" ]; then
        printf 'PASS %s\n' "$case_name"
        passed=$((passed + 1))
    else
        printf 'FAIL %s (exit %s, expected %s)\n' "$case_name" "$actual_status" "$expected_status"
        failed=$((failed + 1))
    fi
}

check_case cli_binary 0 472 2
check_case cli_hex 0 472 16
check_case cli_negative 0 -472 8
check_case cli_zero 0 0 2
check_case cli_reverse 0 1D8 16
check_case cli_lowercase 0 1d8 16
check_case cli_reverse_negative 0 -1D8 16
check_case cli_fraction 0 12.625 2
check_case cli_negative_fraction 0 -0.625 2
check_case cli_reverse_fraction 0 1D8.F 16
check_case cli_max 0 2147483647 16
check_case cli_min 0 -2147483648 16
check_case cli_overflow 1 2147483648 16
check_case cli_underflow 1 -2147483649 16
check_case cli_hex_overflow 1 7FFFFFFFFF 16
check_case cli_fraction_overflow 1 2147483647.1 2
check_case cli_base_low 1 25 1
check_case cli_base_high 1 25 17
check_case cli_base_text 1 25 2x
check_case cli_invalid_digit 1 GG 16
check_case cli_empty 1 '' 2
check_case cli_sign 1 - 2
check_case cli_empty_fraction 1 1. 2
check_case cli_precision 0 0.1 2
check_case cli_tiny_fraction 0 0.000000000001 2
check_case cli_long_fraction 1 0.1234567890123 2
check_case cli_usage 1 -unknown
check_case table_255 0 -table 255
check_case table_zero 0 -table 0
check_case table_min 0 -table -2147483648
check_case step_binary 0 -step 472 2
check_case step_hex 0 -step 472 16
check_case step_zero 0 -step 0 2
check_case step_negative 0 -step -472 16
check_case step_fraction 1 -step 12.625 2
check_case dialog_examples 0
check_case dialog_invalid 0
check_case dialog_boundaries 0
check_case dialog_fraction 0
check_case dialog_whitespace 0
check_case dialog_long_line 0
check_case dialog_nul 0
check_case eof_menu 0
check_case eof_number 0
check_case eof_base 0

printf 'Passed: %s; failed: %s\n' "$passed" "$failed"
[ "$failed" -eq 0 ]
