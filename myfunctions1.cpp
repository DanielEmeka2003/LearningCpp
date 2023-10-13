#include <cassert>
#include <limits>
#include <stdexcept>

#include "myfunctions1.h"

namespace Myfcn
{
    bool isPrime(int n) noexcept
    {
        if (n <= 1)
        return false;

        for (size_t i = 2; i < size_t(n); i++)
        {
            if(size_t(n) % i == 0)
            return false;
        }

        return true;
    }

    int sum_ofposbase10digits(int base10number)
    {
        //Throw an exception if base10number is negative.
        if(base10number < 0)
        throw std::invalid_argument{"Error base10number must be positive!"};
        
        /*Recursive termination condition.
        Returns the base10number back if it is not a base10 digit.*/
        if (base10number >= 0 and base10number <= 9)
        return base10number;
        //Holds the quotient and remainder of input base10number/divisor where the divisor is equal to 10.
        auto quot_rem{std::div(base10number, 10)};
        //The quotient now becomes the base10number.
        return sum_ofposbase10digits(quot_rem.quot) + quot_rem.rem;
    }

    
} // namespace Myfcn


