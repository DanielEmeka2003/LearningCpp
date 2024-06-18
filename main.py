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

import turtle as resource

