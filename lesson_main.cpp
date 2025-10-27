/*
	Expressions and Statements
	Types and Variables
	Code Organization in Files and Header Files
	IO handling
	Functions
	Scopes and Namespaces
	Pointer and References
	C arrays and strings
	Function Recursion
	Debugging
*/
#include <iostream>
#include <print>
#include <cstdint>

/*
	Iteration uses explicit looping constructs (like for, while) to repeat a block of code until a condition is met. It often relies
	on changing state variables.

	Recursion solves a problem by calling itself with smaller instances of the same problem until a base case (a simple, solvable
	instance) is reached. It relies on the call stack to manage state.

	The transformation involves identifying:

	Base Case(s): The condition(s) under which the problem can be solved directly without further repetition/recursion. In iterative
	solutions, this is often the loop termination condition.

	Recursive Step / State Change: How the problem is broken down into smaller, similar subproblems, and how the state (parameters)
	changes with each recursive call. In iterative solutions, this corresponds to how variables are updated in each loop iteration.

	To transform an iterative implementation to a recursive implementation it is required to define the abstract algorithmn used to
	solve the problem around the around the function.

	Example:
	[1] fn population(obj year: si4) si4;

	The abstract alogrithm defines the following:
	population(y) = population(y-1) + (population(y-1) / 2)
	
	[2] fn factorial(obj num: ui4) ui4;

	The abstract alogrithm defines the following:
	factorial(n) = factorial(n-1) * n

	[3] fn fibonacci(obj num: ui4) ui4;

	The abstract alogrithm defines the following:
	fibonacci(n) = fibonacci(n-1) + fibonacci(n-2)

	[4] fn count_digits(obj num: ui4) ui4;

	The abstract alogrithm defines the following:
	count_digits(n) = count_digits(n/10) + 1
*/

int population(int year);
int population_recursive(int year);

int factorial(int num);
int factorial_recursive(int num);

int fibonacci(int n);

int count_digits(int num);

bool is_prime(int num);

int power(int base, int exponent);

std::uint32_t toIndex(std::uint32_t n) {return n-1;}

void convertBase10To2(int num, std::uint32_t *base2_store);

void count(std::uint32_t num, std::uint32_t *count_store);

void counting(int a) {
	if (a == 1) {
		std::cout << 1 << '\n';
		return;
	}

	counting(a - 1);
	std::cout << a << '\n';
}

#include <iostream>

int main() {
	// ...
}


void programLogic() {
	int quotient{};
	int dividend{};
	int divisor{};
	int remainder{};

	// quotient = getIntegerInput()
	// getDivisor(quotient, dividend, remainder)
}

int population(int year) {
	if (year <= 0) {
		return 2;
	}

	int current_population = 2;
	for (size_t current_year = 1; current_year <= year; ++current_year) {
		current_population = current_population + (current_population / 2);
	}

	return current_population;
}

int population_recursive(int year) {
	if (year <= 0) {
		return 2;
	}

	int current_population = population_recursive(year - 1);
	return current_population + (current_population / 2);
}

int factorial(int num) {
	if (num <= 1) {
		return 1;
	}

	int current_factorial{1};
	for (size_t i = 1; i <= num; ++i) {
		current_factorial = current_factorial * i;
	}
	return current_factorial;
}

int factorial_recursive(int num) {
	if (num <= 0) {
		return 1;
	}

	return factorial_recursive(num - 1) * num;
}

int fibonacci(int n) {
    if (n <= 1) {
		return n;
	}

    int previous_fibonacci = 0, current_fibonacci = 1, next = 0;

    for (int i = 2; i <= n; ++i) {
        next = previous_fibonacci + current_fibonacci;
        previous_fibonacci = current_fibonacci;
        current_fibonacci = next;
    }
    return current_fibonacci;
}

int count_digits(int num) {
    int count = 0;
    do {
        num /= 10;
        ++count;
    } while (num != 0);
    return count;
}

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return result;
}