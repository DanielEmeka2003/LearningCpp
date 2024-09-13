"""
class D:
    def __init__(self, a: int, b: str):
        self.a = a
        self.b = b

a = 0.0
for i in range(23):
    a += 2 ** ((i+1) * -1)

print(f"result: {a}")

for x in range(0, 3):
    print("number %d" % (x))

class IEEE_FloatingPoint:
    def __init__(self) -> None:
        self.mantissa: float = 0.0
        self.max_range: float
        self.min_range: float
    
    def calculateFRange(self, nOfMantissaBits : int, maxExp: int, expBias: int):
        for i in range(nOfMantissaBits):
            self.mantissa += 2 ** ((i + 1) * -1)
        self.max_range = (1 + self.mantissa) * 2 ** (maxExp - expBias)
        self.min_range = (1 + self.mantissa) * 2 ** (1 - expBias)

    def print(self):
        print(f"mantissa = {self.mantissa} | min_range = {self.min_range} | max_range = {self.max_range}", end='\n\n')

print("\n\n[[Program Starting]]", end='\n')

ieeeFP = IEEE_FloatingPoint()
while True:
    number_of_mantissa = int(input("Number of mantisaa bits: "))
    max_exp = int(input("Maximum number of exponents: "))
    exp_bias = int(input("Exponent bais: "))

    ieeeFP.calculateFRange(number_of_mantissa, max_exp, exp_bias)
    ieeeFP.print()
"""

def count_digits_iteratively(n, base):
    """Counts the number of digits of an integer n in a given base iteratively."""
    if n == 0:
        return 1
    count = 0
    while n > 0:
        n //= base
        count += 1
    return count

import math

def count_digits_in_base(n, base):
    """Counts the number of digits of an integer n in a given base."""
    if n == 0:
        return 1
    return math.floor(math.log(n, base)) + 1

import time

number = 890808
base = 10

# Logarithmic Method
start = time.time()
digits_count_log = count_digits_in_base(number, base)
end = time.time()
print(f"Logarithmic Method: {digits_count_log} digits, Time: {end - start} seconds")

# Iterative Method
start = time.time()
digits_count_iter = count_digits_iteratively(number, base)
end = time.time()
print(f"Iterative Method: {digits_count_iter} digits, Time: {end - start} seconds")

import time
import decimal

# Setup decimal context
decimal.getcontext().prec = 7

# Test values
a = 1.234567890123456789
b = 9.876543210987654321
da = decimal.Decimal('1.234567890123456789')
db = decimal.Decimal('9.876543210987654321')

# Timing float addition
start_time = time.time()
for _ in range(1000000):
    c = a + b
float_addition_time = time.time() - start_time

# Timing decimal addition
start_time = time.time()
for _ in range(1000000):
    dc = da + db
decimal_addition_time = time.time() - start_time

print(f"Float addition time: {float_addition_time} seconds")
print(f"Decimal addition time: {decimal_addition_time} seconds")

# Similar benchmarking can be done for other operations like multiplication and division