#include "gfunctions.h"


namespace numeric
{
    void ignoreInputBuffer()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  
    }

} // namespace input

namespace numeric::numbers
{

    bool NumericInputValidation(std::string_view errorMessage) 
    {
        if (std::cin.fail())
        {
            std::cin.clear();
            ignoreInputBuffer();
            std::cerr << errorMessage << std::endl;
            return true;
        }
        ignoreInputBuffer();
        
        if (std::cin.gcount() > 1)
        {
            std::cerr << errorMessage << std::endl;
            return true;
        }
        return false;
    }
    
    double getInput(std::string_view InputMessage)
    {
        double x{};
        
        while (true)
        {
            std::cout << InputMessage;
            std::cin >> x;

            if (NumericInputValidation("Invalid! Try again"))
            continue;
            else
            {
                std::cout << '\n';
                return x;
            }
        } 
    }    
    
} // namespace numeric::numbers
