#ifndef MYFUNCTIONS1_H
#define MYFUNCTIONS1_H

#pragma once

#include <string_view>
#include <cstdint> 
#include <type_traits>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <cstdlib>

#include "stream/i_o.h"

namespace Myfcn
{
    /*A class with completely static members whoose sole purpose is to manipulate strings as number bases*/
    class ManipulateStrAsBaseN
    {
    private:
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
    
        static void equalizeOperands(std::string& lhs, std::string& rhs);
    public:
        
        static bool equal(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool notEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool lessThan(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool greaterThan(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool lessThanOrEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool greaterThanOrEqual(const std::string& lhs, const std::string& rhs, std::uint8_t base);
        static bool isValidBaseDigits(const std::string& str, std::uint8_t base);
        static std::string convertBase10ToBaseN(const std::string& str, std::uint8_t base);
        static std::string convertBaseNToBase10(const std::string& str, std::uint8_t base);
        static std::string convertFractionalBaseNToBase10(const std::string& str, std::uint8_t base);
        static std::string convertFractionalBase10ToBaseN(const std::string& str, std::uint8_t base);

        static std::string normalizeToScientificN(const std::string& str);
        static std::string denormalizeToFraction(const std::string& str);
        static std::string approximation(const std::string& str, std::uint32_t desired_digits);
        static bool isFPNumberEqualTo(const std::string& str1, const std::string& str2);
        static bool isFPNumberNotEqualTo(const std::string& str1, const std::string& str2);
        static bool isFPNumberLessThan(const std::string& str1, const std::string& str2);
        static bool isFPNumberGreaterThan(const std::string& str1, const std::string& str2);
        static bool isFPNumberLessThanEqualTo(const std::string& str1, const std::string& str2);
        static bool isFPNumberGreaterThanEqualTo(const std::string& str1, const std::string& str2);
    };

    class U8char
    {
    public:
        enum ByteEncoding: std::uint8_t { empty, one, two, three, four };

    private:
        std::uint8_t m_u8[4];
        ByteEncoding m_byteEncoding;

    public:
        U8char() = default;
        U8char(const std::string& u8);

    private:
        bool isU8charValid(const std::string u8) const;
        void calculate_byteEncoding(const std::string& u8);

    public:
        std::string to_string() const;
        std::string convertToUnicodeCodePoint() const;
        std::string convertToUtf8CodePoint() const;
        ByteEncoding getByteEncoding() const;        
        void info() const;
        std::size_t size() const;
        bool isEmpty() const;
        operator std::uint32_t() const;

        static U8char intToU8char(std::uint32_t x);

        bool isBasicLatinDigit() const;
        bool isBasicLatinAlphabet() const;
        bool isBasicLatinAlphabetNumeral() const;
        bool isBasicLatinLowerCaseAlphabet() const;
        bool isBasicLatinUpperCaseAlphabet() const;
        bool isBasicLatinWhiteSpace() const;
        U8char& basicLatinToLowerCase();
        U8char& basicLatinToUpperCase();
        bool basicLatinAlphabetCaseInsensitiveCompare(const U8char& x) const;

        bool isBasicLatin() const;
        bool isLatin1Supplement() const;
        bool isLatinExtendedA() const;
        bool isLatinExtendedB() const;
        bool isIPAExtensions() const;
        bool isSpacingModifierLetters() const;
        bool isCombiningDiacriticalMarks() const;
        bool isGreekAndCoptic() const;
        bool isCyrillic() const;
        bool isArmenian() const;
        bool isHebrew() const;
        bool isArabic() const;
        bool isSyriac() const;
        bool isArabicSupplement() const;
        bool isThaana() const;
        bool isNKo()const;
        bool isSamaritan()const;
        bool isMandaic()const;
        bool isSyriacSupplement()const;
        bool isArabicExtendedB()const;
        bool isArabicExtendedA()const;
        bool isDevanagari() const;
        bool isBengali() const;
        bool isGurmukhi() const;
        bool isGujarati() const;
        bool isOriya() const;
        bool isTamil() const;
        bool isTelugu() const;
        bool isKannada() const;
        bool isMalayalam() const;
        bool isSinhala() const;
        bool isThai() const;
        bool isLao() const;
        bool isTibetan() const;
        bool isMyanmar() const;
        bool isGeorgian() const;
        bool isHangulJamo() const;
        bool isEthiopic() const;
        bool isEthiopicSupplement() const;
        bool isCherokee() const;
        bool isUnifiedCanadianAboriginalSyllabics() const;
        bool isOgham() const;
        bool isRunic() const;
        bool isTagalog() const;
        bool isHanunoo() const;
        bool isBuhid() const;
        bool isTagbanwa() const;
        bool isKhmer() const;
        bool isMongolian() const;
        bool isCombiningDiacriticalMarksExtended() const;
        bool isCombiningDiacriticalMarksSupplement() const;
        bool isLatinExtendedAdditional() const;
        bool isGreekExtended() const;
        bool isGeneralPunctuation() const;
        bool isSuperscriptsAndSubscripts() const;
        bool isCurrencySymbols() const;
        bool isCombiningDiacriticalMarksForSymbols() const;
        bool isLetterLikeSymbols() const;
        bool isNumberForms() const;
        bool isArrows() const;
        bool isMathematicalOperators() const;
        bool isMiscellaneousTechnical() const;
        bool isControlPictures() const;
        bool isOpticalCharacterRecognition() const;
        bool isEncolsedAlphanumerics() const;
        bool isBoxDrawing() const;
        bool isBlockElements() const;
        bool isGeometricShapes() const;
        bool isMiscellaneousMathematicalSymbolsA() const;
        bool isSupplementalArrowsA() const;
        bool isBraillePatterns() const;
        bool isSupplementalArrowsB() const;
        bool isMiscellaneousMathematicalSymbolsB() const;
        bool isSupplementalMathematicalOperators() const;
        bool isMiscellaneousSymbolsAndArrows() const;
        bool isMiscellaneousSymbols() const;
        bool isDingbats() const;
        bool isCjkRadicalsSupplement() const;
        bool isKangxiRadicals() const;
        bool isIdeographicsDescription() const;
        bool isCjkSymbols_Punctuation() const;
        bool isHiragana() const;
        bool isKatakana() const;
        bool isBopomofo() const;
        bool isHangulCompatibilityJamo() const;
        bool isKanbun() const;
        bool isBopomofoExtended() const;
        bool isEnclosedCjkLetters_Months() const;
        bool isCjkCompatibility() const;
        bool isCjkUnifiedIdeographsExtensionA() const;
        bool isCjkUnifiedIdeographs() const;
        bool isYiSyllables() const;
        bool isYiRadicals() const;
        bool isHangulSyllables() const;
        bool isHighSurrogates() const;
        bool isHighPrivateUseSurrogates() const;
        bool isLowSurrogates() const;
        bool isPrivateUse() const;
        bool isCjkCompatibilityIdeographs() const;
        bool isAlphabeticPresentationForms() const;
        bool isArabicPresentationFormsA() const;
        bool isCombiningHalfMarks() const;
        bool isCjkCompatibilityForms() const;
        bool isSmallFormVariants() const;
        bool isArabicPresentationFormsB() const;
        bool isSpecials() const;
        bool isHalfwidth_Fullwidth_Forms() const;
        bool isIdeographicSymbolsAndPunctuation() const;
        bool isSupplementalArrowsC() const;

        friend bool operator==(const U8char& x, const U8char& y);
        friend bool operator!=(const U8char& x, const U8char& y);
        friend bool operator<(const U8char& x, const U8char& y);
        friend bool operator>(const U8char& x, const U8char& y);
        friend bool operator<=(const U8char& x, const U8char& y);
        friend bool operator>=(const U8char& x, const U8char& y);
    };

    std::ostream& operator<<(std::ostream& os, U8char::ByteEncoding d);
    std::ostream& operator<<(std::ostream& os, const U8char& d);

    using u8streampos = std::fpos<std::char_traits<U8char>::state_type>;

    struct U8charTraits: public std::char_traits<U8char>
    {
        using int_type = std::uint32_t;
        using pos_type = u8streampos;


        static char_type to_char_type(const int_type& x)
        { return U8char::intToU8char(x); }
    };

    using U8string = std::basic_string<U8char, U8charTraits>;
    using U8string_view = std::basic_string_view<U8char, U8charTraits>;

    std::ostream& operator<<(std::ostream& os, const U8string& d);
    std::ostream& operator<<(std::ostream& os, const U8string_view& d);

    U8string convertAsciiStrToU8Str(const std::string& str);
    std::string convertU8StrToAsciiStr(const U8string& u8str);

    namespace u8literals
    {
        U8char operator ""_u8(const char* x, std::size_t s);
        U8char operator ""_u8(char x);
        U8string operator ""_u8str(const char* x, std::size_t s);
    }
    
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
        System::cinput.read_str(input);
        else
        {
            if (wleftOverInputs)
            System::cinput.readwEC(input);
            else
            System::cinput.read(input);
        }   
    }*/
    template <typename T>
    T getInput(std::string_view prompt, [[maybe_unused]] bool wleftOverInputs = false)
    {
        T input{};
        System::coutput.writews(prompt);

        if constexpr (std::is_same_v<T, std::string>)
        {
            try
            {
                System::cinput.read_str(input);
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
                    System::cinput.readwLO(input);
                    else
                    System::cinput.read(input);

                    break; //do not move, needed here
                }
                catch(const std::exception& e)
                {
                    System::cinput.ignoreLeftOverInputs();
                    System::cerror.write_endl("<Try again!>");
                    System::coutput.writews(prompt);
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