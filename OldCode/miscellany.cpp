#include "miscellany.h"

namespace miscellany
{
    
    bool gradeSort_students(Students a, Students b)
    {
        return a.grade > b.grade;
    }
    bool ascending_order(int a, int b)
    {
        return a < b;
    }
    bool descending_order(int a, int b)
    {
        return a > b;
    }
    bool even_order(int a, int b)
    {
        if (a % 2 == 0 and b % 2 != 0)
        return true;
        
        else if (a % 2 != 0 and b % 2 == 0)
        return false;
        
        return ascending_order(a, b);
    }
    int factorial(int N)
    {
        if(N <= 0) return 1;
        else if(N == 1) return 1;
        else return factorial(N - 1) * N;
    }
    int sumOfdigits(int Quotient)
    {
        if(Quotient < 10) return Quotient;
        else
        {
            int Remainder{Quotient % 10};
            Quotient /= 10;
            return sumOfdigits(Quotient) + Remainder;
        }
        
    }
    
    

} // namespace miscellany
