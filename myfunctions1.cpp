#include <cassert>
#include <limits>

#include "myfunctions1.h"

namespace Myfcn
{
    bool isPrime(int n)
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

    void ignoreBufferInput()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool inputBufferValidation()
    {
        if (std::cin.fail())
        {   
            if (std::cin.eof())
            {
                std::cerr << "Input stream closed due to End of File Character used!....\n";
                std::exit(1);
            }

            std::cin.clear();
            ignoreBufferInput();
            //std::cout << "Inside fail condition\n";
            return false;
        }
        
        if (std::cin.gcount() > 1)
        {
            //std::cout << "Inside gcount condition\n";
            return false;
        }

        return true;
    }

    bool isEven(int x)
    {
        if (x % 2 == 0)
        return true;

        return false;
    }

    std::int64_t powint(int base, int exponent)
    {
        assert(exponent >= 0 and "Exponent must be greater than 0");

        if (exponent == 0)
        return std::int64_t(1); // static_cast preferred here

        // Recursion Section!
        if (exponent == 1)
        return base;
        else
        {
            --exponent;
            return std::int64_t(powint(base, exponent) * base); // static_cast preferred here
        }

    }

    int sumDigits(int dividend)
    {
        /*Recursive termination condition.
        Returns the dividend back if it is not a base10 digit.*/
        if (dividend < 10)
        return dividend;
        //Holds the quotient and remainder of input dividend/divisor where the divisor is equal to 10.
        auto quotient_rem{std::div(dividend, 10)};
        //The quotient now becomes the dividend.
        return sumDigits(quotient_rem.quot) + quotient_rem.rem;
    }

    
} // namespace Myfcn


