#ifndef MISC_UTILITY_H
#define MISC_UTILITY_H

#pragma once

#include <string_view>
#include <cstdint>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <cstdlib>
#include <source_location>
#include <filesystem>

#include "stream/i_o.h"
// #include "u8char.h"

struct ImmutableLref{};
inline thread_local ImmutableLref immutable_lref{};

struct Uncatchable{};
inline thread_local Uncatchable uncatchable{};

namespace nc_misc
{
    static inline std::unordered_map<char, int> digitsMap
    {
        {
            {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
            {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20},
            {'L', 21}, {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27}, {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31},
            {'W', 32}, {'X', 33}, {'Y', 34}, {'Z', 35}
        }
    };

    static inline std::unordered_map<int, char> mapDigits
    {
        {
            {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'}, {5, '5'}, {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'},
            {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'}, {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'},
            {21, 'L'}, {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'}, {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'},
            {32, 'W'}, {33, 'X'}, {34, 'Y'}, {35, 'Z'}
        }
    };

    void trim_integer_leading_zeros(std::string& x);
    std::string trim_integer_leading_zeros(const std::string& x, ImmutableLref);

    void trim_real_leading_zeros(std::string& x);
    std::string trim_real_leading_zeros(const std::string& x, ImmutableLref);

    void trim_integer_trailing_zeros(std::string& x);
    std::string trim_integer_trailing_zeros(const std::string& x, ImmutableLref);

    void trim_real_trailing_zeros(std::string& x);
    std::string trim_real_trailing_zeros(const std::string& x, ImmutableLref);

    bool integer_eq(const std::string& lhs, const std::string& rhs);
    bool integer_lt(const std::string& lhs, const std::string& rhs);
    bool integer_gt(const std::string& lhs, const std::string& rhs);
    bool integer_lt_eq(const std::string& lhs, const std::string& rhs);
    bool integer_gt_eq(const std::string& lhs, const std::string& rhs);

    bool areBaseDigitsValid(const std::string& x, std::uint8_t base);

    std::string convertIntegerBase10ToBaseN(const std::string& x, std::uint8_t base);
    std::string convertIntegerBase10ToBaseN(std::uint32_t x, std::uint8_t base);
    std::string convertIntegerBaseNToBase10(const std::string& x, std::uint8_t base);

    void denormalize_realNum(std::string& realNum);
    std::string denormalize_realNum(std::string realNum, ImmutableLref);
    
    void normalize_realNum(std::string& realNum);
    std::string normalize_realNum(std::string realNum, ImmutableLref);

    void approximate_base10_real(std::string& realNum, std::uint32_t desired_digits);
    std::string approximate_base10_real(const std::string& realNum, std::uint32_t desired_digits, ImmutableLref);

    std::string convertRealBaseNToBase10(const std::string& realNum, std::uint8_t base);
    std::string convertRealBase10ToBaseN(const std::string& realNum, std::uint8_t base);

    bool real_eq(const std::string& lhs, const std::string& rhs);
    bool real_lt(const std::string& lhs, const std::string& rhs);
    bool real_gt(const std::string& lhs, const std::string& rhs);
    bool real_lt_eq(const std::string& lhs, const std::string& rhs);
    bool real_gt_eq(const std::string& lhs, const std::string& rhs);
};

template<typename... T>
void panic(const T&... args)
{
    io::cerr.write("Paniced with error: \033[31m"), io::cerr.writews(args...), io::cerr.write_nl("\033[0m");
    throw uncatchable;
}

void panic();

class U8char;
using u8streampos = std::fpos<std::char_traits<U8char>::state_type>;

struct U8charTraits;

using U8string = std::basic_string<U8char, U8charTraits>;
using U8string_view = std::basic_string_view<U8char, U8charTraits>;

namespace misc
{
    class DrawBoxAroundText
    {
    public:
        enum class Style{none, plain, round, bold, doublelined};

    private:
        Streams::OutputStream<char>& m_out;
        Style m_s{};

    public:
        ~DrawBoxAroundText() = default;
        DrawBoxAroundText(Streams::OutputStream<char>& out): m_out{out} {}
        
        void style(Style s);
        void topLine(size_t lineAmount);
        void downLine(size_t lineAmount);
        void spacing(std::string_view text, size_t spaceAmount, size_t optional_TextSize = {});
        void spacing(U8string_view text, size_t spaceAmount, size_t optional_TextSize = {});
        void division(size_t lineAmount);
    };

    U8string read_file(const std::filesystem::path& filePath);

    std::uint64_t safe_unsigned_sub(std::uint64_t x, std::uint64_t y);

    /* Function isPrime(int) runs through an algorithm designed to check for prime numbers 
    with the given number [n] -> of type int*/
    bool isPrime(int n) noexcept;
    
    /*Function getInput<T>(std::string_view) asks the user through a prompt message[string_view object prompt] 
    for an input of type T, determined by the caller.

    prompt - prompt message, uses Streams::System.writews(arg) to output the prompt message.

    wleftOverInputs(with error handling) - if true the function getInput performs error handling by calling
    function InputStream<CharT>::readwEC to read the user input, else it calls InputStream<Char>::read to read the user input.
    [Note] This parameter is useless in the case of the type to extract being a std::string, because at compile time it is replaced
    with InputStream::read_str, for reading user input to std::strings
    Behaves exactly like this:
    {
        if constexpr (std::is_same_v<T, std::string>)
        io::cin.read_str(input);
        else
        {
            if (wleftOverInputs)
            io::cin.readwEC(input);
            else
            io::cin.read(input);
        }   
    }*/
    template <typename T>
    T getInput(std::string_view prompt, [[maybe_unused]] bool wleftOverInputs = false)
    {
        T input{};
        io::cout.writews(prompt);

        if constexpr (std::is_same_v<T, std::string>)
        {
            try
            {
                io::cin.read_str(input);
            }
            catch(const std::exception&)
            {} 
        }
        else
        {
            while (true)
            {
                try
                {
                    if (wleftOverInputs)
                    io::cin.readwLO(input);
                    else
                    io::cin.read(input);

                    break; //do not move, needed here
                }
                catch(const std::exception& e)
                {
                    io::cin.ignoreLeftOverInputs();
                    io::cerr.write_nl("<Try again!>");
                    io::cout.writews(prompt);
                }
                
            }
            
        }

        return input;
    }

    consteval int compileTime(auto value)
    { return value; }

    int sum_ofposbase10digits(int base10number);

    #if 0 //Sentiment Code

    // Performs selection sort[ascending order] on a fixed array of any type
    template <size_t size, typename Type>
    void selection_sort(Type (&array)[size])
    {
        constexpr size_t maximum_n_of_search{size - 1};

        for (size_t currentIndex{}; currentIndex < maximum_n_of_search; ++currentIndex)
        {
            size_t smallestIndex{currentIndex};

            for (size_t i{currentIndex + 1}; i < std::size(array); ++i)
            {
                if (array[i] < array[smallestIndex])
                smallestIndex = i;

            }

            std::swap(array[currentIndex], array[smallestIndex]);
        }
    }

    // Performs bubble sort[ascending order] on a fixed array of any type 
    template <size_t size, typename Type>
    void bubble_sort(Type (&array)[size])
    {
        const size_t maximumBubbleSort{std::size(array) - 1};
        bool swap_flag{};

        for (size_t n_of_times{}; n_of_times < (std::size(array) - 1); ++n_of_times)
        {
            for (size_t leftIndex{}, rightIndex{1}; leftIndex < (maximumBubbleSort - n_of_times); ++leftIndex, ++rightIndex)
            {
                if (array[leftIndex] > array[rightIndex])
                {
                    std::swap(array[leftIndex], array[rightIndex]);
                    swap_flag = true;
                }
            }
            if (!swap_flag)
            {
                std::cout << "Early Termination at the " << (n_of_times + 1) << " Iteration\n";
                break;
            }
            swap_flag = false;
        }
    }
    #endif


} // namespace Myfcn

#endif