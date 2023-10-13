#include <filesystem>

#include <cstdint>
#include <typeinfo>
#include <type_traits>
#include <chrono>
#include <limits>
#include <cmath>
#include <tuple>

#include <exception>
#include <stdexcept>

#include <string>
#include <string_view>
#include <format>

#include <array>
#include <vector>
#include <initializer_list>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <functional>
#include <thread>

#include <regex>


#include "stream/i_o.h"
#include "stream/stringoutputstream.h"
#include "stream/stringinputstream.h"
#include "stream/fileoutputstream.h"
#include "stream/fileinputstream.h"

#include "myfunctions1.h"
#include "timer.h"


using namespace std::string_literals;


#if 0
/*
    "┌──────────┬────────────┬──────────────────────────┐\n"
    "│  number  │   iomanip  │      representation      │\n"
    "├──────────┼────────────┼──────────────────────────┤\n";
    "├──────────┼────────────┼──────────────────────────┤\n" :
    "└──────────┴────────────┴──────────────────────────┘\n";
*/
#endif

#if 0
    // #ifdef _WIN32
    //     std::cout << "Running on Windows" << std::endl;
    // #elif __linux__
    //     std::cout << "Running on Linux" << std::endl;
    // #elif __APPLE__
    //     std::cout << "Running on macOS" << std::endl;
    // #else
    //     std::cout << "Running on an unknown operating system" << std::endl;
    // #endif

#endif

#if 0
        //std::regex reg0{"abc.", std::regex_constants::icase}; //. Any character except newline
        //std::regex reg1{"abc?"};                         //? Zero or one preceding character
        //std::regex reg2{"abc*"};                         //* Zero or more preceding character
        //std::regex reg3{"abc+"};                         //+ One or more preceding character
        //std::regex reg4{"ab[cd]*"};                      //[...] Any character inside the square brackets is a match
        //std::regex reg5{"ab[^cd]"};                      //[^...] Any character that is not inside the square brackets is a match
        //std::regex reg6{"ab[cd]{3}"};                    //{n} Exactly n number of characters is a match
        //std::regex reg7{"ab[cd]{3,}"};                   //{n,} Exactly n number of characters and more
        //std::regex reg8{"ab[cd]{3,5}"};                  //{n1,n2} Exactky n1 - n2 number of characters are matches
        //std::regex reg9{"abc[12]|de[fg]"};               // n1|n2 either n1 or n2 is a match
        /*std::regex reg10{"(abc)(de+)\\1\\2"};               (n1)\\nofgroup Paranthesis defines a group and the backslash+nofgroup
        means the submatch appears exactly in that position*/
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    using namespace Streams;
    
    try
    {
        //integrate noexcept in all code 
        
        /*Find out why function identifers can be bound to both lvalue and rvalue references*/
        [[maybe_unused]] const auto&& t{std::boolalpha};
        


        
        

        /*Brace initialization is not to be used with function template type:
        main.cpp:125:74: error: invalid initialization of non-const reference of type 'std::basic_ostream<char>& (&)(std::basic_ostream<char>&)' from an rvalue of type '<brace-enclosed initializer list>'
        125 |         [[maybe_unused]] auto&& g{std::endl<char, std::char_traits<char>>};
            |                                                                               
                
        [[maybe_unused]] auto&& g{std::endl<char, std::char_traits<char>>};
        */
    }
    catch(const std::exception& e)
    {
        System::cerror.printws_endl("[Exception]", e.what());
    }
    catch(...)
    {
        System::cerror.printwl("Inside catch all");
    }
    
    return EXIT_SUCCESS;
}  