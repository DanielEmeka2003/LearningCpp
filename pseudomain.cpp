#include <list>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cstdlib>
#include <regex>
#include "boost/multiprecision/cpp_int.hpp"
#include "boost/multiprecision/cpp_dec_float.hpp"

#include "stream/output.h"
#include "stream/input.h"
#include "stream/stringoutputstream.h"
#include "stream/stringinputstream.h"
// #include "myfunctions1.h"
#include <bitset>
#include <utility>
#include <stdexcept>
#include <regex>
#include <numeric>
#include <cmath>
#include <csignal>
#include <limits>
#include <format>
#include <source_location>

using namespace boost::multiprecision;

using intN_vector = std::list<std::uint8_t>;

std::ostream& operator<<(std::ostream& os, const intN_vector& d);

std::string iv_to_string(const intN_vector& ivec)
{
    std::string result{};
    for (auto &&i : ivec)
    {
        result.push_back(i);
    }
    return result;
}

intN_vector mul(const intN_vector& operand, std::uint64_t factor)
{
    decltype(factor) carry{};
    intN_vector result{};

    auto lambda = [&](std::uint8_t i)
    {
        auto product = i * factor + carry;
        result.push_front(product % 10);
        carry = product / 10;
    };
    
    std::for_each(operand.rbegin(), operand.rend(), lambda);

    if (carry > 0)
    result.push_front(carry);

    return result;
}

intN_vector smartConstruct(std::uint8_t i)
{
    if (i >= 10)
    {
        std::div_t divpp{};
        divpp = std::div(int(i), 10);

        return intN_vector{ std::uint8_t(divpp.quot), std::uint8_t(divpp.rem) };
    }
    
    return intN_vector{i};
}

intN_vector add(const intN_vector& operand1, const intN_vector& operand2);
intN_vector operator+(const intN_vector& operand1, const intN_vector& operand2);

intN_vector mul(const intN_vector& operand1, const intN_vector& operand2)
{
    std::list<intN_vector> results{};

    auto lambda2 = [&, count = 0uz](std::uint8_t i) mutable
    {
        std::uint32_t carry{};
        results.push_back(intN_vector{});
        
        for (size_t i = 0; i < count; i++)
        results.back().push_back(0);

        for (auto&& j = operand1.rbegin(); j != operand1.rend(); ++j)
        {
            auto product = i * j.operator*() + carry;

            if (std::prev(operand1.rend()) == j)
            results.back().push_front(product);
            else
            results.back().push_front(product % 10);

            carry = product / 10;
        }

        ++count;
    };
    std::for_each(operand2.rbegin(), operand2.rend(), lambda2);

    intN_vector sum{};
    for (auto &&i : results)
    {
        sum = sum + i;
    }

    return sum;
}

intN_vector operator*(const intN_vector& operand1, const intN_vector& operand2)
{
    std::list<intN_vector> results{};

    auto lambda2 = [&, count = 0uz](std::uint8_t i) mutable
    {
        std::uint32_t carry{};
        results.push_back(intN_vector{});
        
        for (size_t i = 0; i < count; i++)
        results.back().push_back(0);

        for (auto&& j = operand1.rbegin(); j != operand1.rend(); ++j)
        {
            auto product = i * j.operator*() + carry;

            if (std::prev(operand1.rend()) == j)
            results.back().push_front(product);
            else
            results.back().push_front(product % 10);

            carry = product / 10;
        }

        ++count;
    };
    std::for_each(operand2.rbegin(), operand2.rend(), lambda2);

    intN_vector sum{};
    for (auto &&i : results)
    {
        sum = sum + i;
    }

    return sum;
}

intN_vector add(const intN_vector& operand1, const intN_vector& operand2)
{
    intN_vector result{};

    auto&& j = operand1.rbegin();
    auto&& k = operand2.rbegin();
    std::uint32_t carry{};

    auto lambda = [&](std::uint8_t i) mutable
    {
        if (j != operand1.rend())
        {
            auto product = i + *j + carry;

            if (operand1.size() >= operand2.size() and std::prev(operand1.rend()) == j)
            result.push_front(product);
            else
            result.push_front(product % 10);

            carry = product / 10;
        
            std::advance(j,1);
            std::advance(k,1);
        }
    };
    std::for_each(operand2.rbegin(), operand2.rend(), lambda);
    
    for(; j != operand1.rend(); std::advance(k,1))
    {
        auto product = *j + carry;

        if (std::prev(operand1.rend()) == j)
        result.push_front(product);
        else
        result.push_front(product % 10);

        carry = product / 10;
    }

    for(; k != operand2.rend(); std::advance(k,1))
    {
        auto product = *k + carry;

        if (std::prev(operand2.rend()) == k)
        result.push_front(product);
        else
        result.push_front(product % 10);

        carry = product / 10;
    }
    
    return result;
}

intN_vector operator+(const intN_vector& operand1, const intN_vector& operand2)
{
    intN_vector result{};

    auto&& j = operand1.rbegin();
    auto&& k = operand2.rbegin();
    std::uint32_t carry{};

    auto lambda = [&](std::uint8_t i) mutable
    {
        if (j != operand1.rend())
        {
            auto product = i + *j + carry;

            if (operand1.size() >= operand2.size() and std::prev(operand1.rend()) == j)
            result.push_front(product);
            else
            result.push_front(product % 10);

            carry = product / 10;
        
            std::advance(j,1);
            std::advance(k,1);
        }
    };
    std::for_each(operand2.rbegin(), operand2.rend(), lambda);
    
    for(; j != operand1.rend(); std::advance(k,1))
    {
        auto product = *j + carry;

        if (std::prev(operand1.rend()) == j)
        result.push_front(product);
        else
        result.push_front(product % 10);

        carry = product / 10;
    }

    for(; k != operand2.rend(); std::advance(k,1))
    {
        auto product = *k + carry;

        if (std::prev(operand2.rend()) == k)
        result.push_front(product);
        else
        result.push_front(product % 10);

        carry = product / 10;
    }
    
    return result;
}



/*...............................................................................................................................................*/

std::unordered_map<char, int> digitsMap
{
    {
        {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
        {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20},
        {'L', 21}, {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27}, {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31},
        {'W', 32}, {'X', 33}, {'Y', 34}, {'Z', 35}
    }
};

std::unordered_map<int, char> mapDigits
{
    {
        {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'}, {5, '5'}, {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'},
        {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'}, {16, 'G'}, {17, 'H'}, {18, 'I'}, {19, 'J'}, {20, 'K'},
        {21, 'L'}, {22, 'M'}, {23, 'N'}, {24, 'O'}, {25, 'P'}, {26, 'Q'}, {27, 'R'}, {28, 'S'}, {29, 'T'}, {30, 'U'}, {31, 'V'},
        {32, 'W'}, {33, 'X'}, {34, 'Y'}, {35, 'Z'}
    }
};

struct ImmutableLref{};
thread_local ImmutableLref immutable_lref{};

struct Uncatchable{};
thread_local Uncatchable uncatchable{};

template<typename... T>
void panic(const T&... args)
{
    io::cerr.write("Paniced with error: \033[31m"), io::cerr.writews(args...), io::cerr.write_nl("\033[0m");
    throw uncatchable;
}

void panic()
{ throw uncatchable; }

void trim_integer_leading_zeros(std::string& x)
{
    if (x.size() == 1) return;

    //does not enforce pre-condition

    if (x.starts_with('0'))
    {
        bool is_zero_ch{}, is_last_element{};

        for (auto i = 0uz, count = 0uz, size = x.size(); /*no check needed because of [1]*/; ++i)
        {
            is_zero_ch = x[i] == '0', is_last_element = i == size - 1;

            if (is_zero_ch and !is_last_element)
            ++count;

            if (!is_zero_ch or is_last_element)//[1]: checks if the character is non-zero or the last element index is reached
            {
                if (count != 0)
                x.erase(x.begin(), x.begin() + count);

                return;
            }
        }
    }
}

std::string trim_integer_leading_zeros(const std::string& x, ImmutableLref)
{
    if (x.size() == 1) return x;

    //does not enforce pre-condition

    if (x.starts_with('0'))
    {
        std::string result{};
        bool is_zero_ch{}, is_last_element{};

        for (auto i = 0uz, count = 0uz, size = x.size(); /*no check needed because of [1]*/; ++i)
        {
            is_zero_ch = x[i] == '0', is_last_element = i == size - 1;

            if (is_zero_ch and !is_last_element)
            ++count;

            if (!is_zero_ch or is_last_element)//[1]: checks if the character is non-zero or the last element index is reached
            {
                if (count != 0)
                return result.assign(x.begin() + count, x.end());
            }
        }
    }
    return x;
}

void trim_real_leading_zeros(std::string& x)
{
    if (x.size() == 1) return;

    //does not enforce pre-condition

    if (x.starts_with('0'))
    {
        bool is_zero_ch{};

        for (auto i = 0uz, count = 0uz, size = x.size(); ; ++i)
        {
            is_zero_ch = x[i] == '0';

            if (i == size-1 or !is_zero_ch or (is_zero_ch and (x[i + 1] == '@' or x[i + 1] == '.')))//[1]
            {
                if (count != 0)
                x.erase(0, count);
            
                return;
            }
            ++count;
        }
    }
}

std::string trim_real_leading_zeros(const std::string& x, ImmutableLref)
{
    if (x.size() == 1) return x;

    //does not enforce pre-condition

    if (x.starts_with('0'))
    {
        bool is_zero_ch{};

        for (auto i = 0uz, count = 0uz, size = x.size(); ; ++i)
        {
            is_zero_ch = x[i] == '0';

            if (i == size-1 or !is_zero_ch or (is_zero_ch and (x[i + 1] == '@' or x[i + 1] == '.')))//[1]
            {
                if (count != 0)
                return std::string{}.assign(x.begin() + count, x.end());
            }
            ++count;
        }
    }
    return x;
}

void trim_integer_trailing_zeros(std::string& x)
{
    if (x.size() == 1) return;

    //does not enforce pre-condition

    if (x.ends_with('0'))
    {
        for (auto i = x.size() - 1, count = 0uz; /*no check needed because of [1]*/; --i)
        {
            if (x[i] != '0' or i == 0)//[1]
            {
                if (count != 0)
                x.erase(i + 1, count);

                return;
            }
            ++count;
        }
    }
}

std::string trim_integer_trailing_zeros(const std::string& x, ImmutableLref)
{
    if (x.size() == 1) return x;

    //does not enforce pre-condition

    if (x.ends_with('0'))
    {
        for (auto i = x.size() - 1, count = 0uz; /*no check needed because of [1]*/; --i)
        {
            if (x[i] != '0' or i == 0)//checks if the element is non-zero or if the first element index is reached
            {
                if (count != 0)
                return std::string{}.assign(x.begin(), x.begin() + (i + 1));
            }
            ++count;
        }
    }
    return x;
}

void trim_real_trailing_zeros(std::string& x)
{
    if (x.size() == 1) return;

    //does not enforce pre-condition

    auto exponent_indicator_index = x.find('@');//0@+0 meant to have a radix separator

    for (auto i = exponent_indicator_index == std::string::npos? x.size() - 1 : exponent_indicator_index - 1, count = 0uz; /*no check needed because of [1]*/; --i)
    {
        if (i == 0 or x[i] == '.' or x[i] != '0' or (x[i] == '0' and x[i - 1] == '.'))//[1]
        {
            if (count != 0)
            x.erase(i + 1, count);

            return;
        }
        ++count;
    }
}

std::string trim_real_trailing_zeros(const std::string& x, ImmutableLref)
{
    if (x.size() == 1) return x;

    auto exponent_indicator_index = x.find('@');

    for (auto i = exponent_indicator_index == std::string::npos? x.size() - 1 : exponent_indicator_index - 1, count = 0uz; /*no check needed because of [1]*/; --i)
    {
        if (i == 0 or x[i] == '.' or x[i] != '0' or (x[i] == '0' and x[i - 1] == '.'))//[1]
        {
            if (count != 0)
            return std::string{x}.erase(i + 1, count);
        }
        ++count;
    }

    return x;
}

bool integer_eq(const std::string& lhs, const std::string& rhs)
{
    if (lhs.empty())
    panic("in function", std::source_location::current().function_name(), "\b, arg lhs is empty");
    else if (rhs.empty())
    panic("in function", std::source_location::current().function_name(), "\b, arg rhs is empty");

    if (lhs.starts_with('0') or rhs.starts_with('0'))
    {
        auto lhs_copy = trim_integer_leading_zeros(lhs, immutable_lref), rhs_copy = trim_integer_leading_zeros(rhs, immutable_lref);

        if (lhs_copy.size() != rhs_copy.size())
        return false;

        for (auto i = 0uz; i < lhs_copy.size(); i++)
        {
            if (digitsMap[ std::toupper(lhs_copy[i]) ] != digitsMap[ std::toupper(rhs_copy[i]) ])
            return false;
        }

        return true;
    }
    else
    {
        if (lhs.size() != rhs.size())
        return false;

        for (auto i = 0uz; i < lhs.size(); i++)
        {
            if (digitsMap[ std::toupper(lhs[i]) ] != digitsMap[ std::toupper(rhs[i]) ])
            return false;
        }
        
        return true;
    }
    
    std::unreachable();
}

bool integer_lt(const std::string& lhs, const std::string& rhs)
{
    if (lhs.empty())
    panic("in function", std::source_location::current().function_name(), "\b, arg lhs is empty");
    else if (rhs.empty())
    panic("in function", std::source_location::current().function_name(), "\b, arg rhs is empty");

    if (lhs.starts_with('0') or rhs.starts_with('0'))
    {
        auto lhs_copy = trim_integer_leading_zeros(lhs, immutable_lref), rhs_copy = trim_integer_leading_zeros(rhs, immutable_lref);

        if (lhs_copy.size() < rhs_copy.size())
        return true;
        else if (lhs_copy.size() > rhs_copy.size())
        return false;
        else if (integer_eq(lhs_copy, rhs_copy))
        return false;

        int lhs_digit{}, rhs_digit{};
        for (auto i = 0uz; i < lhs_copy.size(); i++)
        {
            lhs_digit = digitsMap[ std::toupper(lhs_copy[i]) ], rhs_digit = digitsMap[ std::toupper(rhs_copy[i]) ];

            if (lhs_digit < rhs_digit)
            return true;
            else if (lhs_digit > rhs_digit)
            return false;
            /* don't bother if they are equal */
        }
    }
    else
    {
        if (lhs.size() < rhs.size())
        return true;
        else if (lhs.size() > rhs.size())
        return false;
        else if (integer_eq(lhs, rhs))
        return false;

        int lhs_digit{}, rhs_digit{};
        for (auto i = 0uz; i < lhs.size(); i++)
        {
            lhs_digit = digitsMap[ std::toupper(lhs[i]) ], rhs_digit = digitsMap[ std::toupper(rhs[i]) ];

            if (lhs_digit < rhs_digit)
            return true;
            else if (lhs_digit > rhs_digit)
            return false;
            /* don't bother if they are equal */
        }
    }
    
    std::unreachable();/*it wouldn't reach here because of an assertion proving that they aren't equal have already been tested*/
}

bool integer_gt(const std::string& lhs, const std::string& rhs)
{ return !integer_eq(lhs, rhs) and !integer_lt(lhs, rhs); }

bool integer_lt_eq(const std::string& lhs, const std::string& rhs)
{ return integer_eq(lhs, rhs) or integer_lt(lhs, rhs); }

bool integer_gt_eq(const std::string& lhs, const std::string& rhs)
{ return integer_eq(lhs, rhs) or integer_gt(lhs, rhs); }

bool areBaseDigitsValid(const std::string& x, std::uint8_t base)
{
    if (x.empty())
    panic("in function", std::source_location::current().function_name(), "\b, arg x is empty");

    for (auto &&i : x)
    {
        if (digitsMap[ std::toupper(i) ] >= int(base))
        return false;
    }
    
    return true;
}


std::string convertIntegerBase10ToBaseN(const std::string& x, std::uint8_t base)
{
    if (!areBaseDigitsValid(x, 10))
    panic("in function", std::source_location::current().function_name(), "\b, arg x(", x, ") contains invalid base(10) digits");

    struct DivLarge
    {
        boost::multiprecision::cpp_int quot{};
        boost::multiprecision::cpp_int rem{};

        void perform(boost::multiprecision::cpp_int dividend, boost::multiprecision::cpp_int divisor)
        {
            quot = dividend / divisor;
            rem = dividend % divisor;
        }
    };
    
    DivLarge divLarge{ .quot{ boost::multiprecision::cpp_int{x} } };
    std::string result{};

    while (true)
    {
        if (divLarge.quot == 0)
        break;

        divLarge.perform(divLarge.quot, (std::uint32_t)base);
        result.insert(result.begin(), mapDigits[std::stoi(divLarge.rem.str())]);
    }
    
    return result.empty()? result.assign({'0'}) : result;
}

std::string convertIntegerBase10ToBaseN(std::uint32_t x, std::uint8_t base)
{
    struct DivLarge
    {
        std::uint32_t quot{};
        std::uint32_t rem{};

        void perform(std::uint32_t dividend, std::uint32_t divisor)
        {
            quot = dividend / divisor;
            rem = dividend % divisor;
        }
    };
    
    DivLarge divLarge{ .quot{x} };
    std::string result{};

    while (true)
    {
        if (divLarge.quot == 0)
        break;

        divLarge.perform(divLarge.quot, (std::uint32_t)base);
        result.insert(result.begin(), mapDigits[int(divLarge.rem)]);
    }
    
    return result.empty()? result.assign({'0'}) : result;
}

std::string convertIntegerBaseNToBase10(const std::string& x, std::uint8_t base)
{
    if (!areBaseDigitsValid(x, base))
    panic("in function", std::source_location::current().function_name(), "\b, arg x(", x, ") contains invalid base(", '\b', base, "\b) digits");
    
    boost::multiprecision::cpp_int cppint{ std::to_string(digitsMap[ std::toupper(x[0]) ]).data() };

    for (auto i = 1uz; i < x.size(); ++i)
    cppint = (cppint * base) + digitsMap[ std::toupper(x[i]) ];
    
    return cppint.str();
}


class U8char
{
public:
    enum class ByteEncoding: std::uint8_t { none, one, two, three, four };

    static constexpr std::uint8_t two_byte_marker = 0b110'00000;
    static constexpr std::uint8_t three_byte_marker = 0b1110'0000;
    static constexpr std::uint8_t four_byte_marker = 0b11110'000;
    static constexpr std::uint8_t intermediate_marker = 0b10'000000;

private:
    std::uint8_t m_u8[4];

public:
    U8char() = default;

    U8char(std::string_view u8)
    {
        if (u8.empty())
        {
            m_u8[0] = '\0';
            return;
        }
        
        ByteEncoding byteEncoding = getByteEncoding(u8);
        
        if (!isU8charValid(u8, byteEncoding))
        panic("in function", std::source_location::current().function_name(), "\b, arg u8(", u8, ") contains an invalid utf8 character");

        
        for (auto i = (std::uint8_t)0; i < std::to_underlying(byteEncoding); i++)
        m_u8[(std::uint16_t)i] = u8[(std::uint16_t)i];
    }

private:
    bool isU8charValid(std::string_view u8, ByteEncoding byteEncoding) const
    {
        return
        (
            (byteEncoding == ByteEncoding::one and u8.size() <= 1)
            or
            (byteEncoding == ByteEncoding::two and u8.size() <= 2)
            or
            (byteEncoding == ByteEncoding::three and u8.size() <= 3)
            or
            (byteEncoding == ByteEncoding::four and u8.size() <= 4)
            or
            false /*for returning false when the byteEncoding holds nothing valid*/
        );
    }

    ByteEncoding getByteEncoding(std::string_view u8) const
    {
        ByteEncoding byteEncoding{};

        if (u8.empty())
        return ByteEncoding::none;

        std::bitset<8> byte{ std::uint8_t(u8[0]) };
        
        if (!byte.test(7))
        byteEncoding = ByteEncoding::one;
        else
        {
            std::size_t i{7}, count{};

            while (byte.test(i))
            --i, ++count;

            switch(count)
            {
                case 2: byteEncoding = ByteEncoding::two; break;
                case 3: byteEncoding = ByteEncoding::three; break;
                case 4: byteEncoding = ByteEncoding::four; break;
                default: byteEncoding = ByteEncoding::none; break;
            }
        }
        return byteEncoding;
    }

public:
    ByteEncoding getByteEncoding() const
    {
        ByteEncoding byteEncoding{};

        std::bitset<8> byte{ std::uint8_t(m_u8[0]) };
        
        if (!byte.test(7))
        byteEncoding = ByteEncoding::one;
        else
        {
            std::size_t i{7}, count{};

            while (byte.test(i))
            --i, ++count;

            switch(count)
            {
                case 2: byteEncoding = ByteEncoding::two; break;
                case 3: byteEncoding = ByteEncoding::three; break;
                case 4: byteEncoding = ByteEncoding::four; break;
                default: byteEncoding = ByteEncoding::none; break;
            }
        }
        return byteEncoding;
    }

    std::string to_string() const
    {
        std::string temp{};

        for (std::uint16_t i = 0, n = std::uint16_t(getByteEncoding()); i < n; ++i)
        temp.push_back(char(m_u8[i]));

        return temp;
    }

    auto convertToUnicodeCodePoint() const
    {
        std::uint32_t unicode_code_point{};
        
        std::uint8_t i_byte{};

        switch (getByteEncoding())
        {
            case ByteEncoding::one:
            {
                unicode_code_point = m_u8[0];
                break;
            }
            case ByteEncoding::two:
            {
                for (size_t i = 0; i < 2uz; i++)
                {
                    if (i == 0)
                    {
                        i_byte = two_byte_marker xor m_u8[i]; //collect the bits without the marker [110]XXXXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 5-bits to the main unicode-code-point store
                    }
                    else
                    {
                        unicode_code_point <<= 6; //make room for the upcoming 6-bits, i.e 10[XXXXXX]
                        i_byte = intermediate_marker xor m_u8[i]; //collect the bits without the marker [10]XXXXXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 6-bits to the main unicode-code-point store
                    }
                }
                break;
            }
            case ByteEncoding::three:
            {
                for (size_t i = 0; i < 3uz; i++)
                {
                    if (i == 0)
                    {
                        i_byte = three_byte_marker xor m_u8[i]; //collect the bits without the marker [1110]XXXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 4-bits to the main unicode-code-point store
                    }
                    else
                    {
                        unicode_code_point <<= 6; //make room for the upcoming 6-bits, i.e 10[XXXXXX]
                        i_byte = intermediate_marker xor m_u8[i]; //collect the bits without the marker [10]XXXXXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 6-bits to the main unicode-code-point store
                    }
                }
                break;
            }
            case ByteEncoding::four:
            {
                for (size_t i = 0; i < 4uz; i++)
                {
                    if (i == 0)
                    {
                        i_byte = four_byte_marker xor m_u8[i]; //collect the bits without the marker [11110]XXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 3-bits to the main unicode-code-point store
                    }
                    else
                    {
                        unicode_code_point <<= 6; //make room for the upcoming 6-bits, i.e 10[XXXXXX]
                        i_byte = intermediate_marker xor m_u8[i]; //collect the bits without the marker [10]XXXXXX
                        unicode_code_point |= std::uint32_t(i_byte); //add the 6-bits to the main unicode-code-point store
                    }
                }
                break;
            }
            default:
            panic("in function", std::source_location::current().function_name(), "\b, the encoding is errorneos");
        }
        
        return unicode_code_point;

        /*std::string unicodeCodePoint{};
        std::bitset<8> byte{};
        std::string u8{ to_string() };

        switch (m_byteEncoding)
        {
        case ByteEncoding::one:
            byte = std::uint8_t(u8[0]);

            unicodeCodePoint = byte.to_string().erase(0, 1);
            break;
        case ByteEncoding::two:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 3));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        case ByteEncoding::three:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 4));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[2]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        case ByteEncoding::four:
            byte = std::uint8_t(u8[0]);
            unicodeCodePoint.append(byte.to_string().erase(0, 5));

            byte = std::uint8_t(u8[1]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[2]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));

            byte = std::uint8_t(u8[3]);
            unicodeCodePoint.append(byte.to_string().erase(0, 2));
            break;
        default:
            unicodeCodePoint  = "[NO UTF8 TO STRIP TO UNICODE CODE POINTS]";
            break;
        }

        if (m_byteEncoding != ByteEncoding::empty)
        unicodeCodePoint = convertIntegerBase10ToBaseN(convertIntegerBaseNToBase10(unicodeCodePoint, 2), 16);

        return unicodeCodePoint;*/
    
    }

    auto convertToUtf8CodePoint() const
    {
        // std::string u8CodePoint{};

        // for (std::int16_t i = 0, n = (std::uint16_t)getByteEncoding(); i < n; i++)
        // u8CodePoint.append(convertIntegerBase10ToBaseN(std::uint32_t(m_u8[(std::uint16_t)i]), 16));

        // return u8CodePoint;

        std::uint32_t utf8_code_point{};

        for (std::int16_t i = 0, n = (std::uint16_t)getByteEncoding(); i < n; i++)
        {
            if (i != 0)
            utf8_code_point <<= 8;/*create space for the incoming 8-bits*/

            utf8_code_point |= std::uint32_t(m_u8[i]);/*store the 8-bits into the utf8_code_point*/
        }
        return utf8_code_point;
    }

    friend std::ostream& operator<<(std::ostream& os, ByteEncoding d)
    {
        switch(d)
        {
            case ByteEncoding::one: return os << "[1 byte encoding]";
            case ByteEncoding::two: return os << "[2 byte encoding]";
            case ByteEncoding::three: return os << "[3 byte encoding]";
            case ByteEncoding::four: return os << "[4 byte encoding]";
            default: return  os << "[No Encoding]";
        }

        return os;
    }
    
    void info() const
    {
        io::cout.write("\033[34m"), io::cout.setFillc('=').setWidth(50).write_nl("\033[0m");
        io::cout.writews_nl("ByteEncoding:", getByteEncoding());
        io::cout.writews_nl("Unicode character:", *this);
        io::cout.write_nl("");

        io::cout.write("UTF8_CODE_POINT: [");
        {
            auto utf8_cp = convertToUtf8CodePoint();

            io::cout.write("\033[32;1m");
            io::cout.write(convertIntegerBase10ToBaseN(utf8_cp, 2), "₂ ");
            io::cout.write(utf8_cp, "₁₀ ");
            io::cout.write(convertIntegerBase10ToBaseN(utf8_cp, 16), "₁₆");
            io::cout.write("\033[0m");

            io::cout.write_nl(']');
        }

        io::cout.write_nl("");

        io::cout.write("UNICODE_CODE_POINT: [");
        {
            auto u_cp = convertToUnicodeCodePoint();

            io::cout.write("\033[32;1m");
            io::cout.write(convertIntegerBase10ToBaseN(u_cp, 2), "₂ ");
            io::cout.write(u_cp, "₁₀ ");
            io::cout.write(convertIntegerBase10ToBaseN(u_cp, 16), "₁₆");
            io::cout.write("\033[0m");

            io::cout.write_nl(']');
        }

        io::cout.write("\033[35;1m"), io::cout.setFillc('=').setWidth(50).write_nl("\033[0m");
    }

    friend std::ostream& operator<<(std::ostream& os, const U8char& d)
    {
        for (std::uint16_t i = 0, n = std::uint16_t(d.getByteEncoding()); i < n; i++)
        os << d.m_u8[i];
        
        return os;
    }

    operator std::uint32_t() const
    { return convertToUnicodeCodePoint(); }

    /*static U8char intToU8char(std::uint32_t x) //remove because it is not working, find a better fix
    {
        std::string str{ convertIntegerBase10ToBaseN(x, 16) };
        std::string u8str{};

        struct CharByte
        {
            std::string byte{};
            void inc(std::string::iterator& it)
            {
                byte.push_back(*it);
                byte.push_back(*(it + 1));
                it += 2;
            }
        };
        
        CharByte charByte{};
        for (auto i = str.begin(); i != str.end(); )
        {
            charByte.inc(i);
            u8str.push_back(char(std::stoul(charByte.byte, nullptr, 16)));
        }
        
        return U8char{u8str};
    }*/

    std::uint8_t operator[](std::uint8_t pos) const
    { return m_u8[pos - std::uint8_t(1)]; }

    std::uint8_t at(std::uint8_t pos) const
    {
        if (pos == 0 or pos > std::to_underlying(getByteEncoding()))
        panic("in function", std::source_location::current().function_name(), "arg pos(", pos, ") is = 0 or >", std::to_underlying(getByteEncoding()));
        
        return m_u8[pos - std::uint8_t(1)];
    }

    friend bool operator==(const U8char& x, const U8char& y)
    { return x.convertToUnicodeCodePoint() == y.convertToUnicodeCodePoint(); }

    friend bool operator!=(const U8char& x, const U8char& y)
    { return !(x == y); }

    friend bool operator<(const U8char& x, const U8char& y)
    { return x != y and x.convertToUnicodeCodePoint() < y.convertToUnicodeCodePoint(); }

    friend bool operator>(const U8char& x, const U8char& y)
    { return x != y and !(x < y); }

    friend bool operator<=(const U8char& x, const U8char& y)
    { return x == y or x < y; }

    friend bool operator>=(const U8char& x, const U8char& y)
    { return x == y or x > y; }
    
};

using u8streampos = std::fpos<std::char_traits<U8char>::state_type>;

struct U8charTraits: public std::char_traits<U8char>
{
    using int_type = std::uint32_t;
    using pos_type = u8streampos;


    // static char_type to_char_type(const int_type& x)
    // { return U8char::intToU8char(x); }
};

using U8string = std::basic_string<U8char, U8charTraits>;
using U8string_view = std::basic_string_view<U8char, U8charTraits>;

std::ostream& operator<<(std::ostream& os, const U8string& d)
{
    for (auto&& i : d)
    os << i;

    return os;
}

std::ostream& operator<<(std::ostream& os, U8string_view d)
{
    for (auto&& i : d)
    os << i;

    return os;
}

U8char operator ""_u8(const char* x, std::size_t s)
{ return U8char{ std::string_view{x, s} }; }

U8char operator ""_u8(char x)
{ return U8char{std::string{x}}; }

U8string convertAsciiStrToU8Str(std::string_view str)
{
    U8string u8str{};
    struct makeU8Char
    {
        U8char inc(std::string_view::const_iterator& it)
        {
            U8char::ByteEncoding byteEncoding{U8char{ std::string_view{it, it+1} }.getByteEncoding()};

            std::string_view u8{ it, it + std::size_t(byteEncoding) };
            it += std::size_t(byteEncoding);
            
            return U8char{ u8 };
        }
    };

    makeU8Char mku8{};
    for (auto i = str.begin(); i != str.end(); u8str.push_back(mku8.inc(i)))
    ;

    return u8str;
}

std::string convertU8strToAsciiStr(const U8string_view& u8str)
{
    std::string str{};
        
    for (auto&& i : u8str)
    str += i.to_string();
        
    return str;
}

U8string operator ""_u8str(const char* x, std::size_t s)
{ return convertAsciiStrToU8Str(std::string_view{x, s}); }

std::size_t size_of_u8string_as_individual_bytes(U8string_view x)
{
    std::size_t accumulated_size{};
    for (auto &&i : x)
    accumulated_size += std::size_t(i.getByteEncoding());
    
    return accumulated_size;
}

void denormalize_realNum(std::string& realNum)
{
    std::regex reg{R"(^[[:alnum:]]+(\.[[:alnum:]]*)?@([+-]?[[:digit:]]+)$)"};
    std::smatch smatch{};

    if (!std::regex_match(realNum, smatch, reg))
    panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, R"() is not in the expected format i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)");
    
    //removal of the exponent
    realNum.erase(realNum.begin() + realNum.find('@'), realNum.end());

    auto exponent = std::stoi(smatch[2]); //smatch[2] contains the exponent

    //smatch[1] contains the optional real-part, inculding the radix point
    if (smatch[1].str().empty())
    {
        if (exponent >= 0)
        {
            realNum.append((std::size_t)exponent, '0');
            realNum.append(".0");
        }
        else
        {
            exponent *= -1; //making the exponent positive

            if (std::size_t(exponent) == realNum.size())
            {
                realNum.insert(realNum.begin(), {'0', '.'});
            }
            else if (std::size_t(exponent) < realNum.size())
            {   
                realNum.insert(realNum.begin() + (realNum.size() - std::size_t(exponent)), '.');
            }
            else //if (exponent > realNum.size())
            {
                auto number_of_realZeros_to_add = std::max(exponent, int(realNum.size())) - std::min(exponent, int(realNum.size()));
                realNum.insert(0, number_of_realZeros_to_add, '0');
                realNum.insert(realNum.begin(), {'0', '.'});
            }
        }
    }
    else
    {
        auto radix_index = realNum.find('.');
        auto whole_number_size = radix_index;

        realNum.erase(realNum.begin() + radix_index); //removing the radix separator
        auto real_part_size = realNum.size() - whole_number_size; //real_part_size depends on realNum devoid of the radix seperator

        if (exponent >= 0)
        {
            if (std::size_t(exponent) > real_part_size)
            {
                auto number_of_realZeros_to_add = (std::size_t)exponent - real_part_size;
                realNum.append(number_of_realZeros_to_add, '0');
                realNum.append(".0");
            }
            else if (std::size_t(exponent) < real_part_size)
            {
                auto newRadix_index = exponent + radix_index;
                realNum.insert(realNum.begin() + newRadix_index, '.');
            }
            else //if (std::size_t(exponent) == real_part_size)
            realNum.append({'.', '0'});
        }
        else
        {
            exponent *= -1; //making the exponent positive

            if (std::size_t(exponent) == whole_number_size)//0.008@-2
            {
                realNum.insert(realNum.begin(), {'0', '.'});
            }
            else if (std::size_t(exponent) < whole_number_size)
            {   
                realNum.insert(realNum.begin() + (whole_number_size - std::size_t(exponent)), '.');
            }
            else //if (std::size_t(exponent) > whole_number_size)
            {
                auto number_of_realZeros_to_add = std::max((std::size_t)exponent, whole_number_size) - std::min((std::size_t)exponent, whole_number_size);
                realNum.insert(0, number_of_realZeros_to_add, '0');
                realNum.insert(realNum.begin(), {'0', '.'});
            }
        }
    }

    trim_real_leading_zeros(realNum), trim_real_trailing_zeros(realNum);
}

std::string denormalize_realNum(const std::string& realNum, ImmutableLref)
{
    std::regex reg{R"(^[[:alnum:]]+(\.[[:alnum:]]*)?@([+-]?[[:digit:]]+)$)"};
    std::smatch smatch{};
    auto realNum_copy = realNum;

    if (!std::regex_match(realNum, smatch, reg))
    panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, R"() is not in the expected format i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)");
    
    //removal of the exponent
    realNum_copy.erase(realNum_copy.begin() + realNum_copy.find('@'), realNum_copy.end());

    auto exponent = std::stoi(smatch[2]); //smatch[2] contains the exponent

    //smatch[1] contains the optional real-part, inculding the radix point
    if (smatch[1].str().empty())
    {
        if (exponent >= 0)
        {
            realNum_copy.append((std::size_t)exponent, '0');
            realNum_copy.append(".0");
        }
        else
        {
            exponent *= -1; //making the exponent positive

            if (std::size_t(exponent) == realNum_copy.size())
            {
                realNum_copy.insert(realNum_copy.begin(), {'0', '.'});
            }
            else if (std::size_t(exponent) < realNum_copy.size())
            {   
                realNum_copy.insert(realNum_copy.begin() + (realNum_copy.size() - std::size_t(exponent)), '.');
            }
            else //if (exponent > realNum_copy.size())
            {
                auto number_of_realZeros_to_add = std::max(exponent, int(realNum_copy.size())) - std::min(exponent, int(realNum_copy.size()));
                realNum_copy.insert(0, number_of_realZeros_to_add, '0');
                realNum_copy.insert(realNum_copy.begin(), {'0', '.'});
            }
        }
    }
    else
    {
        auto radix_index = realNum_copy.find('.');
        auto whole_number_size = radix_index;

        realNum_copy.erase(realNum_copy.begin() + radix_index); //removing the radix separator
        auto real_part_size = realNum_copy.size() - whole_number_size; //real_part_size depends on realNum_copy devoid of the radix seperator

        if (exponent >= 0)
        {
            if (std::size_t(exponent) > real_part_size)
            {
                auto number_of_realZeros_to_add = (std::size_t)exponent - real_part_size;
                realNum_copy.append(number_of_realZeros_to_add, '0');
                realNum_copy.append(".0");
            }
            else if (std::size_t(exponent) < real_part_size)
            {
                auto newRadix_index = exponent + radix_index;
                realNum_copy.insert(realNum_copy.begin() + newRadix_index, '.');
            }
            else //if (std::size_t(exponent) == real_part_size)
            realNum_copy.append({'.', '0'});
        }
        else
        {
            exponent *= -1; //making the exponent positive

            if (std::size_t(exponent) == whole_number_size)//0.008@-2
            {
                realNum_copy.insert(realNum_copy.begin(), {'0', '.'});
            }
            else if (std::size_t(exponent) < whole_number_size)
            {
                realNum_copy.insert(realNum_copy.begin() + (whole_number_size - std::size_t(exponent)), '.');
            }
            else //if (std::size_t(exponent) > whole_number_size)
            {
                auto number_of_realZeros_to_add = std::max((std::size_t)exponent, whole_number_size) - std::min((std::size_t)exponent, whole_number_size);
                realNum_copy.insert(0, number_of_realZeros_to_add, '0');
                realNum_copy.insert(realNum_copy.begin(), {'0', '.'});
            }
        }
    }

    trim_real_leading_zeros(realNum_copy), trim_real_trailing_zeros(realNum_copy);
    return realNum_copy;
}

void normalize_realNum(std::string& realNum)
{
    std::regex reg{R"(^[1-9[[:alpha:]]]{1}(\.[[:alnum:]]*)?@[+-]?[[:digit:]]+$)"};

    if (std::regex_match(realNum, reg)){}
    else if (std::regex_match(realNum, reg = R"(^(0{1,}|[[:alnum:]]{1})(\.[[:alnum:]]*)?$)"))//<first-case> for matching something like this: [00000.23], [0.1223] or [1.2345]
    {
        if (auto radix_index = realNum.find('.'); realNum.starts_with('0') and radix_index != std::string::npos)
        {
            if (auto non_zero_pos = std::string_view{realNum.begin() + radix_index + 1, realNum.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
            {
                //example with 0.00234
                realNum.insert((non_zero_pos + radix_index + 1) + 1, 1, '.');//insertion occurs here: 0.002[.]34 => 0.002.34
                realNum.append("@-" + std::to_string(non_zero_pos  + 1));//appending the exponent: 0.002.34@-3
                realNum.erase(radix_index, 1);//erasure of the [0.]: 0.002.34@-3 => 002.34@-3; [plus one because of the insertion of a new radix point]
            }
            else realNum += "@+0";
        }
        else realNum += "@+0";
    }
    else if (std::regex_match(realNum, reg = R"(^[[:alnum:]]{2,}(\.[[:alnum:]]*)?$)"))//<second-case> for matching something like this: [12], [00012.2] or [123.34]
    {
        if (auto radix_index = realNum.find('.'); radix_index != std::string::npos)
        {
            /*
            * the insertion followed by the erasure is so unnecessary, an alorigthm like [move] would be more efficient, example:
            * instead of inserting a new radix_seperator[.] and then erasing the old one, moving the radix_seperator from it's former index_position
            * to a new desitination would be better and more efficient; realNum.move(radix_index, (radix_index - 1) - (radix_index - 2));
            * to calculate how much the radix point was moved, with index starting from 1 is = (radix_index - 1) - (radix_index - 2)
            * while from 0 is = radix_index - radix_index - 1
            */

            //example with 12.34 1234.5

            realNum.erase(radix_index, 1);//erase the radix seperator
            realNum.insert(1, 1, '.');//insertion occurs here: 1[.]2.34 => 1.2.34
            realNum.append("@+" + std::to_string(radix_index - 1));//appending the exponent: 1.2.34@+1
        }
        else
        {
            realNum.insert(1, 1, '.');
            realNum.append("@+" + std::to_string(realNum.size() - 2));
        }
    }
    else if (std::regex_match(realNum, reg = R"(^[[:alnum:]]+(\.[[:alnum:]]*)?@[+-]?[[:digit:]]+$)"))//<third-and-final case> for any fallout that has an exponent: [12.34@1], [123.4@-23], [0023.34@34] or [0.4534@-10]
    {
        /* dangerous: regex_search does not work like i thought it would, it doesn't search for the characters in a row but instead individually.
        * std::string_view would be better to reference only the non-exponent
        */
        auto exponent_indicator_index = realNum.find('@');
	    auto real_only = std::string{realNum.begin(), realNum.begin() + exponent_indicator_index};

        //0.23456@-33 | 0356@-12 | 0.1@-3 | 0000.1@-3 | 0.0@-2 | 0000.0@-3 | 0000@-23
        //replace with my implementation of regex_search: nc_reg::regex_search(immutable_lref realNum, reg = R"('0'+('.'<digit>+)?)")
        if (std::regex_match(real_only, reg = R"(0+(\.[[:digit:]]*)?)"))//a similar reiteration of the <first-case> but captures exponents: [0000000000.1@-3], [000.000@-23] or [0.234@+4]
        {
            if (auto radix_index = realNum.find('.'); radix_index != std::string::npos)
            {
                if (auto non_zero_pos = std::string_view{realNum.begin() + radix_index + 1, realNum.begin() + (exponent_indicator_index != std::string::npos? exponent_indicator_index : realNum.size())}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                {
                    int exponent{};
                    //extracting the exponent
                    for (auto i = realNum.size() - 1; i != 0; --i)
                    {
                        if (realNum[i] == '@')
                        {
                            exponent =  std::stoi(std::string/*_view*/(realNum.begin() + (i + 1), realNum.end()));
                            realNum.erase(realNum.begin() + (i + 1), realNum.end());
                            break;
                        }
                    }

                    realNum.insert((non_zero_pos + radix_index + 1) + 1, 1, '.');
                    realNum.append(exponent >= 0? '+' + std::to_string(exponent - int(non_zero_pos  + 1)) : std::to_string(exponent - int(non_zero_pos  + 1)));
                    realNum.erase(radix_index, 1);
                }
                else
                realNum = "0.0@+0";
            }
            else
            realNum = "0.0@+0";
        }
        else if (std::regex_match(real_only, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//a similar reiteration of the <second-case>, but one which captures exponents: [000001@-23], [02343@+45] or [233445.3434@+2]
        {
            int exponent{};
            //extracting the exponent
            for (auto i = realNum.size() - 1; i != 0; --i)
            {
                if (realNum[i] == '@')
                {
                    exponent =  std::stoi(std::string/*_view*/(realNum.begin() + (i + 1), realNum.end()));
                    realNum.erase(realNum.begin() + (i + 1), realNum.end());
                    break;
                }
            }

            if (auto radix_index = realNum.find('.'); radix_index != std::string::npos)
            {
                realNum.erase(radix_index, 1);
                realNum.insert(realNum.find_first_not_of('0') + 1, 1, '.');
                realNum.append(exponent >= 0? '+' + std::to_string(exponent + int(radix_index - 1)) : std::to_string(exponent + int(radix_index - 1)));
            }
            else
            {
                realNum.insert(realNum.find_first_not_of('0') + 1, 1, '.');
                realNum.append(exponent >= 0? '+' +  std::to_string(exponent + int(realNum.size() - 2)) : std::to_string(exponent + int(realNum.size() - 2)));
            }
        }
    }
    else
    panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, ") is not in the expected format .i.e a real-like format");
    
    trim_real_leading_zeros(realNum), trim_real_trailing_zeros(realNum);
}

std::string normalize_realNum(const std::string& realNum, ImmutableLref)
{
    std::regex reg{R"(^[1-9[[:alpha:]]]{1}(\.[[:alnum:]]*)?@[+-]?[[:digit:]]+$)"};
    std::string realNum_copy = realNum;

    if (std::regex_match(realNum, reg)){}
    else if (std::regex_match(realNum_copy, reg = R"(^(0{1,}|[[:alnum:]]{1})(\.[[:alnum:]]*)?$)"))//<first-case> for matching something like this: [00000.23], [0.1223] or [1.2345]
    {
        if (auto radix_index = realNum_copy.find('.'); realNum_copy.starts_with('0') and radix_index != std::string::npos)
        {
            if (auto non_zero_pos = std::string_view{realNum_copy.begin() + radix_index + 1, realNum_copy.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
            {
                //example with 0.00234
                realNum_copy.insert((non_zero_pos + radix_index + 1) + 1, 1, '.');//insertion occurs here: 0.002[.]34 => 0.002.34
                realNum_copy.append("@-" + std::to_string(non_zero_pos  + 1));//appending the exponent: 0.002.34@-3
                realNum_copy.erase(radix_index, 1);//erasure of the [0.]: 0.002.34@-3 => 002.34@-3; [plus one because of the insertion of a new radix point]
            }
            else realNum_copy += "@+0";
        }
        else realNum_copy += "@+0";
    }
    else if (std::regex_match(realNum_copy, reg = R"(^[[:alnum:]]{2,}(\.[[:alnum:]]*)?$)"))//<second-case> for matching something like this: [12], [00012.2] or [123.34]
    {
        if (auto radix_index = realNum_copy.find('.'); radix_index != std::string::npos)
        {
            /*
            * the insertion followed by the erasure is so unnecessary, an alorigthm like [move] would be more efficient, example:
            * instead of inserting a new radix_seperator[.] and then erasing the old one, moving the radix_seperator from it's former index_position
            * to a new desitination would be better and more efficient; realNum_copy.move(radix_index, (radix_index - 1) - (radix_index - 2));
            * to calculate how much the radix point was moved, with index starting from 1 is = (radix_index - 1) - (radix_index - 2)
            * while from 0 is = radix_index - radix_index - 1
            */

            //example with 12.34 1234.5

            realNum_copy.erase(radix_index, 1);//erase the radix seperator
            realNum_copy.insert(1, 1, '.');//insertion occurs here: 1[.]2.34 => 1.2.34
            realNum_copy.append("@+" + std::to_string(radix_index - 1));//appending the exponent: 1.2.34@+1
        }
        else
        {
            realNum_copy.insert(1, 1, '.');
            realNum_copy.append("@+" + std::to_string(realNum_copy.size() - 2));
        }
    }
    else if (std::regex_match(realNum_copy, reg = R"(^[[:alnum:]]+(\.[[:alnum:]]*)?@[+-]?[[:digit:]]+$)"))//<third-and-final case> for any fallout that has an exponent: [12.34@1], [123.4@-23], [0023.34@34] or [0.4534@-10]
    {
        /* dangerous: regex_search does not work like i thought it would, it doesn't search for the characters in a row but instead individually.
        * std::string_view would be better to reference only the non-exponent
        */
        auto exponent_indicator_index = realNum_copy.find('@');
	    auto real_only = std::string{realNum_copy.begin(), realNum_copy.begin() + exponent_indicator_index};

        //0.23456@-33 | 0356@-12 | 0.1@-3 | 0000.1@-3 | 0.0@-2 | 0000.0@-3 | 0000@-23
        //replace with my implementation of regex_search: nc_reg::regex_search(immutable_lref realNum_copy, reg = R"('0'+('.'<digit>+)?)")
        if (std::regex_match(real_only, reg = R"(0+(\.[[:digit:]]*)?)"))//a similar reiteration of the <first-case> but captures exponents: [0000000000.1@-3], [000.000@-23] or [0.234@+4]
        {
            if (auto radix_index = realNum_copy.find('.'); radix_index != std::string::npos)
            {
                if (auto non_zero_pos = std::string_view{realNum_copy.begin() + radix_index + 1, realNum_copy.begin() + (exponent_indicator_index != std::string::npos? exponent_indicator_index : realNum_copy.size())}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                {
                    int exponent{};
                    //extracting the exponent
                    for (auto i = realNum_copy.size() - 1; i != 0; --i)
                    {
                        if (realNum_copy[i] == '@')
                        {
                            exponent =  std::stoi(std::string/*_view*/(realNum_copy.begin() + (i + 1), realNum_copy.end()));
                            realNum_copy.erase(realNum_copy.begin() + (i + 1), realNum_copy.end());
                            break;
                        }
                    }

                    realNum_copy.insert((non_zero_pos + radix_index + 1) + 1, 1, '.');
                    realNum_copy.append(exponent >= 0? '+' + std::to_string(exponent - int(non_zero_pos  + 1)) : std::to_string(exponent - int(non_zero_pos  + 1)));
                    realNum_copy.erase(radix_index, 1);
                }
                else
                realNum_copy = "0.0@+0";
            }
            else
            realNum_copy = "0.0@+0";
        }
        else if (std::regex_match(real_only, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//a similar reiteration of the <second-case>, but one which captures exponents: [000001@-23], [02343@+45] or [233445.3434@+2]
        {
            int exponent{};
            //extracting the exponent
            for (auto i = realNum_copy.size() - 1; i != 0; --i)
            {
                if (realNum_copy[i] == '@')
                {
                    exponent =  std::stoi(std::string/*_view*/(realNum_copy.begin() + (i + 1), realNum_copy.end()));
                    realNum_copy.erase(realNum_copy.begin() + (i + 1), realNum_copy.end());
                    break;
                }
            }

            if (auto radix_index = realNum_copy.find('.'); radix_index != std::string::npos)
            {
                realNum_copy.erase(radix_index, 1);
                realNum_copy.insert(realNum_copy.find_first_not_of('0') + 1, 1, '.');
                realNum_copy.append(exponent >= 0? '+' + std::to_string(exponent + int(radix_index - 1)) : std::to_string(exponent + int(radix_index - 1)));
            }
            else
            {
                realNum_copy.insert(realNum_copy.find_first_not_of('0') + 1, 1, '.');
                realNum_copy.append(exponent >= 0? '+' +  std::to_string(exponent + int(realNum_copy.size() - 2)) : std::to_string(exponent + int(realNum_copy.size() - 2)));
            }
        }
    }
    else
    panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, ") is not in the expected format .i.e a real-like format");
    
    trim_real_leading_zeros(realNum_copy), trim_real_trailing_zeros(realNum_copy);
    return realNum_copy;
}

std::string approximate_base10_real(const std::string& realNum, std::uint32_t desired_digits)
{
    std::regex reg{R"(^([0-9]+)(\.[0-9]+)?$)"};
    std::smatch smatch{};

    if (std::regex_match(realNum, smatch, reg))
    {
        /* slices would be soo fucking good for this; when i switch to my slice's rich language, re-implement this using mutable slices instead*/
        
        std::string real_part = smatch[2];/*contains the real_part plus the radix*/

        if (real_part.size() - 1/*minus the radix*/ > desired_digits)
        {
            std::string integer_part = smatch[1];

            if (digitsMap[real_part.back()] >= 5)
            {
                auto carry_digit_flag = true;
                auto temp_insert_digit = 0;
                
                for (int i = desired_digits /*to get the last safe digit index*/; i > 0/*because the radix is at index[0]*/; --i)
                {
                    if (carry_digit_flag)
                    {
                        temp_insert_digit = 1 + digitsMap[real_part[i]];
                        carry_digit_flag = temp_insert_digit / 10 == 1, temp_insert_digit %= 10;

                        real_part.replace((std::size_t)i, 1uz, 1uz, mapDigits[temp_insert_digit]);
                    }
                    else
                    break;
                }

                if (carry_digit_flag)//for dealing with the integer_part of approximation
                {
                    for (int i = integer_part.size() - 1; /*no need for this check*/; i--)
                    {
                        if (i == 0)
                        {
                            if (carry_digit_flag)
                            {
                                temp_insert_digit = 1 + digitsMap[integer_part[i]];

                                if (temp_insert_digit == 10)
                                integer_part.replace((std::size_t)i, 1uz, "10");
                                else
                                integer_part.replace((std::size_t)i, 1uz, 1uz, mapDigits[temp_insert_digit]);
                            }

                            break;
                        }
                        else
                        {
                            if (carry_digit_flag)
                            {
                                temp_insert_digit = 1 + digitsMap[integer_part[i]];
                                carry_digit_flag = temp_insert_digit / 10 == 1, temp_insert_digit %= 10;

                                integer_part.replace((std::size_t)i, 1uz, 1uz, mapDigits[temp_insert_digit]);
                            }
                            else
                            break;
                        }
                    }
                    
                }
            }
            //removing the undesired digits
            real_part.erase((std::size_t)desired_digits + 1 /*, default value is set to end*/);

            return integer_part + real_part;
        }
        return realNum;
    }
    else
    panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, R"() is not in the expected format .i.e [0-9]{1}\.[0-9]*?@[+-]?[0-9]+)");

    return "";
}

std::string convertRealBaseNToBase10(const std::string& realNum, std::uint8_t base)
{
    auto convertRealPart = [&](std::string& real_part)
    {
        if (!areBaseDigitsValid(real_part, base))
        panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, ") contains invalid base(10) digits");

        using boost_dec50 = boost::multiprecision::cpp_dec_float_50;
        // using boost_dec40 = boost::multiprecision::number<boost::multiprecision::backends::cpp_dec_float<40U>>;

        boost_dec50 result{}, base_alt{ (std::uint32_t)base };
        
        for (std::int64_t exp{-1}; auto&& i : real_part)
        {
            if (digitsMap[std::toupper(i)] != 0)
            result += digitsMap[std::toupper(i)] * boost::multiprecision::pow(base_alt, exp);
            
            exp -= 1;
        }

        auto result_string = result.str();

        if (result_string.contains('e'))
        std::ranges::replace(result_string, 'e', '@'), denormalize_realNum(result_string);
        
        //removal of the leading zero i.e [0].xxxxxxxx
        result_string.erase(result_string.begin());

        return result_string;
    };

    std::tuple<std::string, std::string, std::string> i_r_e{};
    //spliting up the integer_real_exponent
    auto split_up =  [&](const std::string& real_num)
    {
        auto radix_index = real_num.find('.');
        auto exponent_index = real_num.find('@');

        if (radix_index != std::string::npos)
        {
            std::get<0>(i_r_e).assign(real_num.begin(), real_num.begin() + radix_index);

            if (exponent_index != std::string::npos)
            {
                std::get<1>(i_r_e).assign(real_num.begin() + radix_index + 1, real_num.begin() + exponent_index);
                std::get<2>(i_r_e).assign(real_num, exponent_index + 1);
            }
            else
            std::get<1>(i_r_e).assign(real_num.begin() + radix_index + 1, real_num.end());
        }
        else
        {
            if (exponent_index != std::string::npos)
            {
                std::get<0>(i_r_e).assign(real_num.begin(), real_num.begin() + exponent_index);
                std::get<2>(i_r_e).assign(real_num, exponent_index + 1);
            }
            else
            std::get<0>(i_r_e).assign(real_num);
        }
    };

    split_up(realNum);

    if (realNum.contains('@'))
    {
        auto denormalized_real = denormalize_realNum(realNum, immutable_lref);
        split_up(denormalized_real);
    }
    else
    split_up(realNum);

    //calculation starts here..........................................................................
    std::get<0>(i_r_e) = convertIntegerBaseNToBase10(std::get<0>(i_r_e), base);

    if (!std::get<1>(i_r_e).empty())
    std::get<1>(i_r_e) = convertRealPart(std::get<1>(i_r_e));
    
    /*if (realNum.contains('@'))
    {
        using boost_dec50 = boost::multiprecision::cpp_dec_float_50;
        std::cout << "Don't mess with me: " << std::get<1>(i_r_e) << '\n';
        auto converted_real = std::get<0>(i_r_e) + std::get<1>(i_r_e);
        std::string temp = (boost_dec50{converted_real} * boost::multiprecision::pow(boost_dec50{(std::uint32_t)base}, boost_dec50{std::get<2>(i_r_e)})).str();
    
        if (temp.contains('e')) 
        std::ranges::replace(temp, 'e', '@');

        return normalize_realNum(temp), temp;
    }
    else
    return normalize_realNum(std::get<0>(i_r_e) + std::get<1>(i_r_e), immutable_lref);

    std::unreachable();*/
    
    return normalize_realNum(std::get<0>(i_r_e) + std::get<1>(i_r_e), immutable_lref);
}

std::string convertRealBase10ToBaseN(const std::string& realNum, std::uint8_t base)//convert
{
    /*
    * STEPS SIMPLE CONVERSION OF A BASE(10) REALNUMBER TO BASE(N)
    *
    * 1. first denormalize the real number i.e something like this: [23.3@-1 or 2.45@+2] to [2.33 or 245.0]
    * 2. slpit the integer and real part .i.e [2.33] becomes {2, 0.33}
    * 3. perform respective conversions on both seperately .i.e {2, 0.33}₁₀ gets converted to {2, 0.547AE147AE147AE}₁₆
    * 4. join both the integer and real part back and normalize .i.e {2, 0.547AE147AE147AE}₁₆ => 2.547AE147AE147AE₁₆@+0
    * 
    * STEPS SENSIBLE CONVERSION OF A BASE(10) REALNUMBER TO BASE(N)
    * 
    * Reason: the above simple steps dosen't account for the preformance cost and overhead of denormalizing a very large or small real-number,
    *         example: something like this[ 8.23456@+2000 or 3.4554554@-3400 ] is very costly.
    *         But the steps below account for such a scenario and is the preferred one to be used in this function.
    *         The idea behind it is that, since {23.3@-1 | 233@-2 | 0.233@+1} = 2.33 and the initial-base is 10,
    *         instead of denormalizing we simply just, frist convert the real-number to be desired-base(N) and second multiply that
    *         result by the exponent, which is the initial-base(10) raised to the power of the exponent which would be for example:
    *         [@-1 or @+2] = [10⁻¹ or 10²]
    *         example:
    *         (a). 23.3₁₀ => 17.4CCCCCC₁₆; then multiplied by the exponent (17.4CCCCCC₁₆ * A⁻¹) = 2.547AE147AE147AE₁₆@+0
    *         (b). 233.0₁₀ => E9; then muliplied by the exponent (E9 * A⁻²) = 2.547AE147AE147AE₁₆@+0
    *         (c). 0.233 => 0.3BA5E353F7CED95 then muliplied by the exponent (0.3BA5E353F7CED95 * A¹) = 2.547AE147AE147AE₁₆@+0
    * 
    *         [NOTE]: a variation of the simple step(without the denormalization of the real-number) would be used to convert the real-number
    *
    * 1. first, use a regex to both assert a pre-condition that states that the string must be in a real-number format and split the real-number
    *    and exponent, if any.
    * 2. define a condition that checks if the exponent is non-exsitent or the is zero
    * 3. if the conditon is true, a simple conversion would be all that is needed
    * 4. else the sensible conversion would be preformed
    * 5. normalize the result
    */
    
    auto convertRealPart = [&](std::string& real_part)
    {
        if (!areBaseDigitsValid(real_part, 10))
        panic("in function", std::source_location::current().function_name(), "\b, arg realNum(", realNum, ") contains invalid base(10) digits");

        using boost_dec50 = boost::multiprecision::cpp_dec_float_50;
        // using boost_dec20 = boost::multiprecision::number<boost::multiprecision::backends::cpp_dec_float<20U>>;

        //insert [0.] in the begining of real_part
        real_part.insert(0, "0.");
        
        boost_dec50 real_part_copy{real_part}, base_alt{ (std::uint32_t)base };
        /*
        * a base(10) number like this: 3.0@-345 is not convertible directly to base(30) because every real implementation available dosen't
        * compute it's multiplication or division in scientific-notation
        */
        constexpr std::uint32_t max_iterations = 70;
        std::string result{'.'};
        int integer_part{};

        for (auto i = 0u; i < max_iterations; ++i)
        {
            if (real_part_copy == boost_dec50{"0.0"})
            break;

            real_part_copy *= base_alt, integer_part = real_part_copy.convert_to<int>();

            result.push_back(mapDigits[integer_part]);
            
            if (integer_part >= 1)
            real_part_copy -= integer_part;
        }

        if (result.size() == 1)
        result.push_back('0');
        
        return result;
    };

    std::tuple<std::string, std::string, std::string> i_r_e{};
    //spliting up the integer_real_exponent
    auto split_up =  [&](const std::string& real_num)
    {
        auto radix_index = real_num.find('.');
        auto exponent_index = real_num.find('@');

        if (radix_index != std::string::npos)
        {
            std::get<0>(i_r_e).assign(real_num.begin(), real_num.begin() + radix_index);

            if (exponent_index != std::string::npos)
            {
                std::get<1>(i_r_e).assign(real_num.begin() + radix_index + 1, real_num.begin() + exponent_index);
                std::get<2>(i_r_e).assign(real_num, exponent_index + 1);
            }
            else
            std::get<1>(i_r_e).assign(real_num.begin() + radix_index + 1, real_num.end());
        }
        else
        {
            if (exponent_index != std::string::npos)
            {
                std::get<0>(i_r_e).assign(real_num.begin(), real_num.begin() + exponent_index);
                std::get<2>(i_r_e).assign(real_num, exponent_index + 1);
            }
            else
            std::get<0>(i_r_e).assign(real_num);
        }
    };

    if (realNum.contains('@'))
    {
        auto denormalized_real = denormalize_realNum(realNum, immutable_lref);
        split_up(denormalized_real);
    }
    else
    split_up(realNum);
    
    //calculation starts here..........................................................................

    std::get<0>(i_r_e) = convertIntegerBase10ToBaseN(std::get<0>(i_r_e), base);

    if (!std::get<1>(i_r_e).empty())
    std::get<1>(i_r_e) = convertRealPart(std::get<1>(i_r_e));
    
    return normalize_realNum(std::get<0>(i_r_e) + std::get<1>(i_r_e), immutable_lref);
}

bool real_eq(const std::string& lhs, const std::string& rhs)
{
    /*
    * Pre-condition
    * 1.assumes that the input string is a return value of function normalize_real
    */
    //                 |   1     |      2            |      3
    std::regex reg{R"(^([[:alnum:]]){1}(\.[[:alnum:]]*)?@([+-]?[[:digit:]]+)$)"};
    std::smatch smatch1{};
    std::smatch smatch2{};

    if (std::regex_match(lhs, smatch1, reg) and std::regex_match(rhs, smatch2, reg)) 
    {
        if (integer_eq(smatch1[1], "0") and integer_eq(smatch2[1], "0"))
        return true;
        else
        {
            //check if the exponent-part is not equal, because somethin like this: 3.98@-2 is always not equal to 3.98@6
            if (std::stoi(smatch1[3]) != std::stoi(smatch2[3]))
            return false;
        }

        //check if the whole-number-part is not equal first
        if (!integer_eq(smatch1[1], smatch2[1]))
        return false;
        else //check if the real-part is not equal next
        {
            std::string lhs_real_part = smatch1[2].str();
            std::string rhs_real_part = smatch2[2].str();

            if (lhs_real_part.size() == 1 and rhs_real_part.size() == 1)//optimization for something like this: 3. or 4.
            return true;
            else
            {
                if (!lhs_real_part.empty()) lhs_real_part.erase(0, 1);
                if (!rhs_real_part.empty()) rhs_real_part.erase(0, 1);
            }

            if (lhs_real_part.empty()) lhs_real_part.push_back('0');
            if (rhs_real_part.empty()) rhs_real_part.push_back('0');

            //comparison starts here
            if (lhs_real_part.ends_with('0') or rhs_real_part.ends_with('0'))
            {
                auto lhs_real_part_copy = trim_integer_trailing_zeros(lhs_real_part, immutable_lref);
                auto rhs_real_part_copy = trim_integer_trailing_zeros(rhs_real_part, immutable_lref);

                if (lhs_real_part_copy.size() != rhs_real_part_copy.size())
                return false;

                for (auto i = 0uz; i < lhs_real_part_copy.size(); i++)
                {
                    if (digitsMap[ std::toupper(lhs_real_part_copy[i]) ] != digitsMap[ std::toupper(rhs_real_part_copy[i]) ])
                    return false;
                }

                return true;
            }
            else
            {
                if (lhs_real_part.size() != rhs_real_part.size())
                return false;

                for (auto i = 0uz; i < lhs_real_part.size(); i++)
                {
                    if (digitsMap[ std::toupper(lhs_real_part[i]) ] != digitsMap[ std::toupper(rhs_real_part[i]) ])
                    return false;
                }
                
                return true;
            }
        }
    }
    else
    panic("in function", std::source_location::current().function_name(), "\b, arg lhs(", lhs, ") or arg rhs(", rhs, R"() is not in the expected format i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)");
    
    std::unreachable();
}

bool real_lt(const std::string& lhs, const std::string& rhs)
{
    /*
    * pre-condition:
    * assumes the input values have been normalized
    */

    std::regex reg{R"(^([[:alnum:]]){1}(\.[[:alnum:]]*)?@([+-]?[[:digit:]]+)$)"};
    std::smatch smatch1{};
    std::smatch smatch2{};

    if (std::regex_match(lhs, smatch1, reg) and std::regex_match(rhs, smatch2, reg)) 
    {
        /*
        * How this works soley depends on the operands being normalized in scientific notation.
        * [Example of how they are compared]:
        * (1.0@-23 < 1.0@-34) returns false because:
        * the exponent is compared first:
        * (-23 < -34 = false)
        * (-23 > -34 = true) returns false
        * [Another one]:
        * (2.3@34 < 8.2@34) returns true because:
        * the exponent is compared first:
        * (34 < 34 = false)
        * (34 > 34 = false)
        * the integer_part is compared second:
        * (2 < 8 = true) returns true
        * [Another example]:
        * (3.04@-45 < 3.003@-45) returns false because:
        * the exponent is compared first:
        * (-45 < -45 = false)
        * (-45 > -45 = false)
        * the integer_part is compared second:
        * (3 < 3 = false)
        * (3 > 3 = false)
        * the real_part is compared last:
        * (0.04 < 0.003 = false) returns false
        * {
        *   0.04 < 0.003 is compared like so:
        *   what is actually compared = [04] < [0003]
        *   now because leading zeros are considered valid values from the pov of digits after the radix point[real_part], a padding to the
        *   left is expected for the values to be compared as equals
        *   [04] => [0400]
        *   [0003] => [remains as it is]
        * 
        *   now comparinf them:
        *   (0400 < 0003 = false)
        *   (0400 > 0003 = true) returns false 
        *   NOTE: it is the opposite in terms of comparing digits before the radix point i.e integers
        * }
        * 
        * [..................................................................................................................]
        * While the above works for every case, it is not designed to work for the only denormalized real_number which is [0]
        * example:
        * [0.0@0 < 9.645@-23] is expected to return true, but it returns true instead
        * the exponent is compared first:
        * (0 < -23 = false)
        * (0 > -23 = true) returns false
        * As you can see, the anamoly lays in the first comparison that gets executed, although it is correct of numbers other than zero, i.e
        * [2.0@3 < 9.645@-23] returns false because the exponent of lhs is greater than that of rhs, it not correct for zero;
        * So a compromise is added:
        * First check if either of the integer_part is zero, if true, execute according, i.e
        * [lhs is 0 and rhs is not 0]: return true
        * [lhs is not 0 and rhs is 0]: return false
        * [lhs and rhs are both zero]: return false
        * 
        * [Note]: all this is only possible because the inputs are expected to be in normalized scientific form
        */

        if (integer_eq(smatch1[1], "0") and !integer_eq(smatch2[1], "0"))
        return true;
        else if (!integer_eq(smatch1[1], "0") and integer_eq(smatch2[1], "0"))
        return false;
        else if (integer_eq(smatch1[1], "0") and integer_eq(smatch2[1], "0"))
        return false;
        else
        {
            //check the exponent first, to see which is lower or greater; something like this: 3.45@32 is always greater than 9.45@10
            if (auto lhs_exponent = std::stoi(smatch1[3]), rhs_exponent = std::stoi(smatch2[3]); lhs_exponent < rhs_exponent)
            return true;
            else if (lhs_exponent > rhs_exponent)
            return false;
        }

        if (integer_lt(smatch1[1], smatch2[1]))
        return true;
        else if (integer_gt(smatch1[1], smatch2[1]))
        return false;
        else
        {
            std::string lhs_real_part = smatch1[2].str();
            std::string rhs_real_part = smatch2[2].str();

            if (lhs_real_part.size() == 1 and rhs_real_part.size() == 1)//optimization for something like this: 3. or 4.
            return true;
            else
            {
                if (!lhs_real_part.empty()) lhs_real_part.erase(0, 1);
                if (!rhs_real_part.empty()) rhs_real_part.erase(0, 1);
            }

            if (lhs_real_part.empty()) lhs_real_part.push_back('0');
            if (rhs_real_part.empty()) rhs_real_part.push_back('0');

            auto lhs_real_part_size = lhs_real_part.size();
            auto rhs_real_part_size = rhs_real_part.size();
            auto max_size = std::max(lhs_real_part_size, rhs_real_part_size);

            //fill with leading zeros, if they aren't equal
            if (lhs_real_part_size != rhs_real_part_size)//i.e. these become (00345, 09) => (00345, 09000)
            lhs_real_part.append(max_size - lhs_real_part_size, '0'), rhs_real_part.append(max_size - rhs_real_part_size, '0');
            
            //comparison starts here
            {
                int lhs_digit{}, rhs_digit{};
                for (auto i = 0uz; i < max_size; i++)
                {
                    lhs_digit = digitsMap[ std::toupper(lhs_real_part[i]) ], rhs_digit = digitsMap[ std::toupper(rhs_real_part[i]) ];

                    if (lhs_digit < rhs_digit)
                    return true;
                    else if (lhs_digit > rhs_digit)
                    return false;
                    /* don't bother if they are equal */
                }
                //if they are equal
                return false;
            }
        }
    }
    else
    panic("in function", std::source_location::current().function_name(), "\b, arg lhs(", lhs, ") or arg rhs(", rhs, R"() is not in the expected format i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)");
    
    std::unreachable();
}

bool real_gt(const std::string& lhs, const std::string& rhs)
{ return !real_eq(lhs, rhs) and !real_lt(lhs, rhs); }

bool real_lt_eq(const std::string& lhs, const std::string& rhs)
{ return real_eq(lhs, rhs) or real_lt(lhs, rhs); }

bool real_gt_eq(const std::string& lhs, const std::string& rhs)
{ return real_eq(lhs, rhs) or real_gt(lhs, rhs); }


void max_normal(int n_of_mantissa, int exponent_bais)
{
    io::cout.writews("\033[31;1mmax_normal:\033[0m");
    using boost50 = boost::multiprecision::cpp_dec_float_50;
    io::cout.get() << (boost50{2} - boost::multiprecision::pow(boost50{2}, {-n_of_mantissa})) * boost::multiprecision::pow(boost50{2}, {exponent_bais}) << '\n';
}

void min_denormal(int n_of_mantissa, int default_denormal_exp)
{
    io::cout.writews("min_denormal:");
    using boost50 = boost::multiprecision::cpp_dec_float_50;
    io::cout.get() << boost::multiprecision::pow(boost50{2}, {-n_of_mantissa}) * boost::multiprecision::pow(boost50{2}, {default_denormal_exp}) << '\n';
}

namespace R
{
    #include <tuple>
    
};

bool operator==(std::string_view lhs, U8string_view rhs)
{
    if (lhs.size() != size_of_u8string_as_individual_bytes(rhs))
    return false;

    std::uint8_t u8char_pos{}, byteEncoding_as_ui8{};

    for (std::size_t u8string_index = 0, astring_index = 0, n = lhs.size(); astring_index < n;)
    {
        byteEncoding_as_ui8 = std::to_underlying(rhs[u8string_index].getByteEncoding());
        u8char_pos = 1;
        
        while (u8char_pos <= byteEncoding_as_ui8)
        {
            if (std::uint8_t(lhs[astring_index++]) != rhs[u8string_index][u8char_pos++])
            return false;
        }
        ++u8string_index;
    }
    return true;
}

bool operator==(U8string_view lhs, std::string_view rhs)
{ return rhs == lhs; }

bool operator==(const U8char& lhs, std::string_view rhs)
{
    std::uint8_t byteEncoding_as_ui8 = std::to_underlying(lhs.getByteEncoding());

    if (byteEncoding_as_ui8 != std::uint8_t(rhs.size()))
    return false;

    for (std::uint8_t i = 1; i <= byteEncoding_as_ui8; i++)
    {
        if (lhs[i] != std::uint8_t(rhs[i-1]))
        return false;
    }
    return true;
}

bool operator==(std::string_view lhs, const U8char& rhs)
{ return rhs == lhs; }

using fn_ptr = void(*)();
void g(){ std::cout << "Hello "; }
void h(){ std::cout << "world\n"; }

int main()
{
    try
    {
        /*g++ -Iheader -I"C:\Program Files\Boost" -L"C:/Program Files/Boost/stage/lib" -std=c++23 pseudomain.cpp -o pseudomain -lboost_locale-mgw13-mt-d-x64-1_84*/

        // for (auto i : convertAsciiStrToU8Str("+§№™🌹℉asf®_©℗⁆"))
        
        // io::cout.setPrecision(40);
        // max_normal(10, 15), max_normal(23, 127), max_normal(52, 1'023), max_normal(112, 16'383);
        // std::cout << '\n';
        // min_denormal(10, -14), min_denormal(23, -126), min_denormal(52, -1'022), min_denormal(112, -16'382);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception{ " << e.what() << " }\n";
    }
    catch(...){}
}

std::ostream& operator<<(std::ostream& os, const intN_vector& d)
{
    for (auto &&i : d)
    {
        os << int(i);
    }
    return os;
}