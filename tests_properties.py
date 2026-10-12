"""Independent checks against Python int and exact fractions; no third-party modules."""
import random
import re
import subprocess
import sys
from fractions import Fraction
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXE = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else ROOT / 'converter.exe'
if not EXE.is_file():
    EXE = ROOT / 'converter'


def dialog(requests):
    data = ''.join(f'{choice}\n{text}\n{base}\n' for choice, text, base in requests) + '3\n'
    run = subprocess.run([str(EXE)], input=data, capture_output=True, text=True, timeout=30)
    assert run.returncode == 0, run.stderr
    assert 'error:' not in run.stdout, run.stdout[-1000:]
    results = re.findall(r'result: ([^\r\n]+)', run.stdout)
    assert len(results) == len(requests), (len(results), len(requests))
    return results, run.stderr


def as_fraction(text, base):
    negative = text.startswith('-')
    whole, _, digits = text.lstrip('+-').partition('.')
    value = Fraction(int(whole, base))
    if digits:
        value += Fraction(int(digits, base), base ** len(digits))
    return -value if negative else value


def check_precision(expected, actual, base):
    value = as_fraction(actual, base)
    assert abs(expected - value) < Fraction(1, base ** 12), (expected, actual, base)
    assert abs(value) <= abs(expected), (expected, actual, base)
    assert len(actual.partition('.')[2]) <= 12, actual
    if expected and value:
        assert (expected < 0) == (value < 0), actual
    return expected != value


rng = random.Random(101)
values = [0, 1, -1, 472, -472, 255, 2147483647, -2147483648]
values += [rng.randint(-2147483648, 2147483647) for _ in range(100)]
requests = [(1, str(value), base) for base in range(2, 17) for value in values]
results, warnings = dialog(requests)
assert warnings == '', warnings
for (_, text, base), actual in zip(requests, results):
    assert int(actual, base) == int(text), (text, base, actual)
reverse = [(2, actual.lower(), base) for (_, _, base), actual in zip(requests, results)]
back, warnings = dialog(reverse)
assert warnings == '', warnings
assert back == [text for _, text, _ in requests]

fractions = ['12.625', '-12.625', '0.1', '-0.1', '0.000000000001',
             '-0.000000000001', '2147483646.999999999999',
             '-2147483647.999999999999', '0.999999999999', '1.000000000000']
forward = [(1, text, base) for base in range(2, 17) for text in fractions]
fraction_results, warnings = dialog(forward)
losses = 0
for (_, text, base), actual in zip(forward, fraction_results):
    losses += check_precision(Fraction(text), actual, base)
assert warnings.count('warning: precision loss') == losses

reverse = []
for base in range(2, 17):
    digits = ''.join('0123456789ABCDEF'[rng.randrange(base)] for _ in range(12))
    reverse.extend([(2, '1.' + digits, base), (2, '-0.' + digits.lower(), base)])
back, warnings = dialog(reverse)
losses = 0
for (_, text, base), actual in zip(reverse, back):
    losses += check_precision(as_fraction(text, base), actual, 10)
assert warnings.count('warning: precision loss') == losses

for text in ('2147483648', '-2147483649', '999999999999999999999999999999'):
    for base in range(2, 17):
        run = subprocess.run([str(EXE), text, str(base)], capture_output=True, text=True, timeout=5)
        assert run.returncode == 1 and 'outside int range' in run.stderr
print('PASS: 3240 integer conversions/round trips, 180 fraction checks, 45 overflow checks.')
