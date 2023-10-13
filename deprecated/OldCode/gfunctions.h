#if !defined(FUNCTIONS_H)
#define FUNCTIONS_H

#pragma once

#include "includes.h"
#include "constants.h"

using namespace std::literals; 
using namespace std::numbers; 

inline const int dimensions{45};

namespace numeric 
{
    void ignoreInputBuffer(); 
} // namespace input 

namespace numeric::numbers
{
    bool NumericInputValidation(std::string_view errorMessage = "Try again!"); 
    double getInput(std::string_view InputMessage = "Enter a number: ");  
} // namespace numeric::numbers
namespace practice
{
    char getArithmetic();
    void arithmeticCalculations(double x, char ch, double y);
    void calculateHeightOfTheBall(double height); 
    bool isEven(int integer); 
    bool isPrime(int number);
} // namespace practice

namespace text
{
    std::string getStringInput(std::string_view InputMessage = "Enter anything you want: ");
    std::string getStringInput();
    char getChar(std::string_view InputMessage = "Enter anything character you want: ");

    bool isAlphanumeric(char c);
    bool isAlpha(char c);
    bool isLowercase(char c);
    bool isUppercase(char c);
    bool isDigit(char c);
    bool isHexadecimalCharacter(char c);
    bool isControlCharacter(char c); 
    bool isGraphicalCharacter(char c);
    bool isSpaceCharacter(char c);
    bool isBlankCharacter(char c);
    bool isPrintableCharacter(char c);
    bool isPunctuationCharacter(char c);
    char convertToLowercase(char c);
    char convertToUppercase(char c);

    template <typename T>
    std::string convertPrimitivesToString(T value)
    {
        std::ostringstream os;
        os << value;
        return std::string{ os.str() };
    }  
    
    template <typename T>
    void convertStringToPrimitives(const std::string& str, T& value)
    {
        if ( std::ranges::any_of(str, isAlpha) or std::ranges::any_of(str, isPunctuationCharacter)  )
        return;

        std::istringstream is{ str };
        //is.str(str);
        is >> value;
    }

    bool isNameValid(std::string_view str_v);

} // namespace text


#endif // FUNCTIONS_H
