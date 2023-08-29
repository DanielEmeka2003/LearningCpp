#include "gfunctions.h"

namespace text
{
    std::string getStringInput(std::string_view InputMessage)
    {
        std::cout << InputMessage;
        std::string input{};
        std::getline(std::cin >> std::ws, input);
        return input; 
    }
    std::string getStringInput()
    {
        std::string input{};
        std::getline(std::cin >> std::ws, input);
        return input;  
    }

    char getChar(std::string_view InputMessage)
    {
        std::cout << InputMessage;
        char ch{};
        std::cin >> ch;
        numeric::ignoreInputBuffer();
        return ch;
    }

    bool isAlphanumeric(char c)
    {
        return std::isalnum(c) != 0;
    }
    bool isAlpha(char c)
    {
        return std::isalpha(c) != 0;
    }
    bool isLowercase(char c)
    {
        return std::islower(c) != 0;
    }
    bool isUppercase(char c)
    {
        return std::isupper(c) != 0;
    }
    bool isDigit(char c)
    {
        return std::isdigit(c) != 0;
    }
    bool isHexadecimalCharacter(char c)
    {
        return std::isxdigit(c) != 0;
    }
    bool isControlCharacter(char c)
    {
        return std::iscntrl(c) != 0;
    }
    bool isGraphicalCharacter(char c)
    {
        return std::isgraph(c) != 0;
    }
    bool isSpaceCharacter(char c)
    {
        return std::isspace(c) != 0;
    }
    bool isBlankCharacter(char c)
    {
        return std::isblank(c) != 0;
    }
    bool isPrintableCharacter(char c)
    {
        return std::isprint(c) != 0;
    }
    bool isPunctuationCharacter(char c)
    {
        return std::ispunct(c) != 0;
    }
    char convertToLowercase(char c)
    {
        return std::tolower(c);
    }
    char convertToUppercase(char c)
    {
        return std::toupper(c);
    }

    bool isNameValid(std::string_view str_v)
    {
        if ( std::ranges::all_of(str_v, isAlpha) or std::ranges::any_of(str_v, isSpaceCharacter) )
        {
            if ( std::ranges::count_if(str_v, isSpaceCharacter) > 3 )
            {
                return false;
            }
            
            return true;
        }
        return false;
    }

} // namespace text
