#include "gfunctions.h"

     
namespace practice
{ 
    char getArithmetic()
    {
        char x; 
        bool y{true}; 
        while (y)
        {
            std::cout << "Enter any of these operators +, -, *, /: ";
            std::cin >> x;
            numeric::ignoreInputBuffer(); 
            switch (x)
            {
                case '+':
                case '-':
                case '*':
                case '/':
                y = false; 
                break;

                default:
                std::cerr << "Opps, Wrong operator. Try again!\n";
                y = true;    
                break;
            }
        }
        std::cout << '\n';
        return x;
    }

    void arithmeticCalculations(double x, char ch, double y)
    {
        switch (ch)
        {
            case '+':
                std::cout << x << " + " << y << " = " << x + y << '\n';
                break;
            case '-':
                std::cout << x << " - " << y << " = " << x - y << '\n';
                break;
            case '*':
                std::cout << x << " * " << y << " = " << x * y << '\n';
                break;
            case '/':
                std::cout << x << " / " << y << " = " << x / y << '\n';
                break;
        }
    }
    
    void calculateHeightOfTheBall(double height)
    {
        
        for (int s = 0; s <= 100; ++s)
        {
            double distanceFalllen{constants::gravity*(s * s) / 2}; 
            double currentHeight{height - distanceFalllen};

            if(height > distanceFalllen)
            std::cout << "At " << s << " seconds, the ball is at height: " << currentHeight << " meters\n";
            else
            {
                std::cout << "At " << s << " seconds, the ball is on the ground\n";
                break;
            }
        }

    }

    bool isEven(int integer)
    {
        bool flag{};

        if (integer % 2 == 0) flag = true;
        else flag = false;
        
        return flag;
    }

    bool isPrime(int number)
    {
        bool flag{true};
        
        if(number == 0 || number == 1) flag = true;

        else
        {
            for (int i = 2; i < number; ++i)
            {
                if(number % i == 0)
                {
                    flag = false;
                    break;
                }
                else flag = true;
            }
            
        }
        
        return flag;
    }
} // namespace practice

