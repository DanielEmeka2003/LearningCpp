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

#include <bitset>
#include <utility>
#include <stdexcept>
#include <regex>
#include <numeric>
#include <cmath>
#include <csignal>
#include <limits>

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

bool equal(const std::string& lhs, const std::string& rhs)
{
    if (lhs.empty() or rhs.empty())
    throw std::runtime_error{ "in function Myfcn::ManipulateStringAsNBases::equal either lhs or rhs is empty" };

    if (lhs.size() != rhs.size())
    return false;

    for (auto i = 0uz; i < lhs.size(); i++)
    {
        if (digitsMap[ std::toupper(lhs[i]) ] != digitsMap[ std::toupper(rhs[i]) ])
        return false;
    }
    
    return true;
}

bool notEqual(const std::string& lhs, const std::string& rhs)
{ return !equal(lhs, rhs); }

bool lessThan(const std::string& lhs, const std::string& rhs)
{
    if (lhs.empty() or rhs.empty())
    throw std::runtime_error{ "in function Myfcn::ManipulateStringAsNBases::lessThan either lhs or rhs is empty" };

    if (lhs.size() < rhs.size())
    return true;

    if (lhs.size() > rhs.size())
    return false;
    
    if (equal(lhs, rhs))
    return false;

    for (auto i = 0uz; i < lhs.size(); i++)
    {
        if (digitsMap[ std::toupper(lhs[i]) ] <= digitsMap[ std::toupper(rhs[i]) ])
        {}
        else
        return false;
    }
    
    return true;
}

bool greaterThan(const std::string& lhs, const std::string& rhs)
{
    if (lhs.empty() or rhs.empty())
    throw std::runtime_error{ "in function Myfcn::ManipulateStringAsNBases::greaterThan either lhs or rhs is empty" };

    if (equal(lhs, rhs))
    return false;

    return !lessThan(lhs, rhs);
}

bool lessThanOrEqual(const std::string& lhs, const std::string& rhs)
{
    if (equal(lhs, rhs) or lessThan(lhs, rhs))
    return true;

    return false;
}

bool greaterThanOrEqual(const std::string& lhs, const std::string& rhs)
{
    if (equal(lhs, rhs) or greaterThan(lhs, rhs))
    return true;

    return false;
}

bool isValidBaseDigits(const std::string& str, std::uint8_t base)
{
    if (str.empty())
    throw std::runtime_error{ "in function Myfcn::ManipulateStringAsNBases::isValidBaseDigits str is empty" };

    for (auto &&i : str)
    {
        if (digitsMap[ std::toupper(i) ] >= int(base))
        return false;
    }
    
    return true;
}


auto convertBase10ToBaseN(const std::string& str, long long base)
{
    if (!isValidBaseDigits(str, 10))
    throw std::runtime_error{"Invalid base"};

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
    
    DivLarge divLarge{};
    std::string result{};

    divLarge.perform(boost::multiprecision::cpp_int{str.data()}, base);
    result.insert(result.begin(), mapDigits[std::stoi(divLarge.rem.str())]);

    while (true)
    {
        if (divLarge.quot == 0)
        break;

        divLarge.perform(divLarge.quot, base);
        result.insert(result.begin(), mapDigits[std::stoi(divLarge.rem.str())]);
    }
    
    return result;
}

auto convertBaseNToBase10(const std::string& str, std::uint8_t base)
{
    if (!isValidBaseDigits(str, base))
    throw std::runtime_error{"Base is not valid"};
    
    intN_vector vec{ smartConstruct(std::uint8_t()) };
    boost::multiprecision::cpp_int cppint{ std::to_string(digitsMap[ std::toupper(str[0]) ]).data() };

    for (auto i = 1uz; i < str.size(); ++i)
    {
        cppint = (cppint * base) + digitsMap[ std::toupper(str[i]) ];
    }
    
    return cppint.str();
}


class U8char
{
public:
    enum ByteEncoding: std::uint8_t { empty, one, two, three, four };

private:
    std::uint8_t m_u8[4];
    ByteEncoding m_byteEncoding;

public:
    U8char() = default;
    U8char(const std::string& u8)
    {
        if (u8.empty())
        return;
        
        calculate_byteEncoding(u8);

        if (!isU8charValid(u8))
        throw std::invalid_argument{"in function U8char::U8char parameter u8charholder contains an invalid utf8 character"};

        for (std::size_t index{}; auto&& i : u8)
        {
            m_u8[index] = std::uint8_t(i);
            ++index;
        }
    }

private:
    bool isU8charValid(const std::string u8) const
    {
        if (m_byteEncoding == ByteEncoding::one and u8.size() > 1)
        return false;
        else if (m_byteEncoding == ByteEncoding::two and u8.size() > 2)
        return false;
        else if (m_byteEncoding == ByteEncoding::three and u8.size() > 3)
        return false;
        else if (m_byteEncoding == ByteEncoding::four and u8.size() > 4)
        return false;

        return true;
    }

    void calculate_byteEncoding(const std::string& u8)
    {
        if (u8.empty())
        m_byteEncoding = ByteEncoding::empty;

        std::bitset<8> byte{ std::uint8_t(u8[0]) };
        
        if (!byte.test(7))
        m_byteEncoding = ByteEncoding::one;
        else
        {
            std::size_t i{7};
            std::size_t count{};

            while (byte.test(i))
            { --i, ++count; }

            switch(count)
            {
                case 2: m_byteEncoding = ByteEncoding::two; break;
                case 3: m_byteEncoding = ByteEncoding::three; break;
                case 4: m_byteEncoding = ByteEncoding::four; break;
            }
        }
    }

public:
    std::string to_string() const
    {
        std::string temp{};

        for (std::uint16_t i = 0; i < std::uint16_t(m_byteEncoding); ++i)
        temp.push_back(char(m_u8[i]));

        return temp;
    }

    auto convertToUnicodeCodePoint() const
    {
        std::string unicodeCodePoint{};
        std::bitset<8> byte{};
        std::string u8{to_string()};

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
        unicodeCodePoint = convertBase10ToBaseN(convertBaseNToBase10(unicodeCodePoint, 2), 16);

        return unicodeCodePoint;
    }

    auto convertToUtf8CodePoint() const
    {
        std::string u8CodePoint{};

        for (auto&& i : to_string())
        {
            u8CodePoint.append(convertBase10ToBaseN(convertBaseNToBase10(std::bitset<8>{std::uint8_t(i)}.to_string(), 2), 16));
        }

        return u8CodePoint;
    }

    auto getByteEncoding() const
    { return m_byteEncoding; }

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
        System::coutput.setFillc('=').setWidth(42).write_endl("");
        System::coutput.writews_endl("ByteEncoding:", m_byteEncoding);
        System::coutput.writews_endl("UTF8 character:", to_string());
        System::coutput.write_endl("");

        System::coutput.write_endl("UTF8 CODE POINT:");
        
        System::coutput.writews("Base2  :");
        for(auto&& i : to_string())
        {
            System::coutput.writews(std::bitset<8>{std::uint8_t(i)});
        }
        System::coutput.write_endl("");

        System::coutput.hex().writews("Base16 :");
        for(auto&& i : to_string())
        {
            System::coutput.write(std::bitset<8>{std::uint8_t(i)}.to_ulong());
        }
        System::coutput.dec();
        System::coutput.write_endl("");

        System::coutput.write_endl("");

        System::coutput.write_endl("UNICODE CODE POINT:");
        std::string unicodeCodePoints = convertToUnicodeCodePoint();
        System::coutput.writews_endl("Base2  :", convertBase10ToBaseN(convertBaseNToBase10(unicodeCodePoints, 16), 2));        
        System::coutput.writews_endl("Base16 :", unicodeCodePoints);

        System::coutput.setFillc('=').setWidth(42).write_endl("");
    }

    friend std::ostream& operator<<(std::ostream& os, const U8char& d)
    { return os << d.to_string(); }

    auto size() const
    { return 1uz; }

    auto isEmpty() const
    {
        if (to_string().empty())
        return true;

        return false;
    }

    operator std::uint32_t() const
    { return std::stoul(convertToUtf8CodePoint(), nullptr, 16); }

    static U8char intToU8char(std::uint32_t x)
    {
        std::string str{ convertBase10ToBaseN(std::to_string(x), 16) };
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
        
        return u8str;
    }

    friend bool operator==(const U8char& x, const U8char& y)
    {
        if (x.isEmpty() and y.isEmpty())
        return true;

        if ((x.isEmpty() and !y.isEmpty()) or (!x.isEmpty() and y.isEmpty()))
        return false;

        if (x.m_byteEncoding != y.m_byteEncoding)
        return false;
        
        if (notEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint()))
        return false;

        return true;
    }

    friend bool operator!=(const U8char& x, const U8char& y)
    { return !(x == y); }

    friend bool operator<(const U8char& x, const U8char& y)
    {
        if (x == y)
        return false;

        if (x.isEmpty() and !y.isEmpty())
        return true;
        else if (!x.isEmpty() and y.isEmpty())
        return false;

        if (x.m_byteEncoding > y.m_byteEncoding)
        return false;

        if (greaterThanOrEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint()))
        return false;

        return true;
    }

    friend bool operator>(const U8char& x, const U8char& y)
    {
        if (x == y)
        return false;

        if (x.isEmpty() and !y.isEmpty())
        return false;
        else if (!x.isEmpty() and y.isEmpty())
        return true;

        if (x.m_byteEncoding > y.m_byteEncoding)
        return true;

        if (lessThanOrEqual(x.convertToUnicodeCodePoint(), y.convertToUnicodeCodePoint()))
        return false;

        return true;
    }

    friend bool operator<=(const U8char& x, const U8char& y)
    {
        if ((x == y) or (x < y))
        return true;

        return false;
    }

    friend bool operator>=(const U8char& x, const U8char& y)
    {
        if ((x == y) or (x > y))
        return true;

        return false;
    }
};

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

std::ostream& operator<<(std::ostream& os, const U8string& d)
{
    for (auto&& i : d)
    os << i;

    return os;
}

U8char operator ""_u8(const char* x, std::size_t s)
{
    if (s > 4)
    throw std::invalid_argument{R"(in function operator ""_u8, s of type std::size_t is greater than 4)"};
    return U8char{std::string{x, s}};
}
U8char operator ""_u8(char x)
{ return U8char{std::string{x}}; }

U8string convertCharstrToU8str(const std::string& str)
{
    U8string u8str{};
    struct makeU8Char
    {
        U8char inc(std::string::const_iterator& it)
        {
            U8char::ByteEncoding byteEncoding{U8char{std::string{*it}}.getByteEncoding()};
            std::string str{};
            
            for (auto i = 0; i < int(byteEncoding); ++i)
            {
                str.push_back(*(it + i));
            }
            it += std::size_t(byteEncoding);

            return str;
        }
    };

    makeU8Char mku8{};
    for (auto i = str.begin(); i != str.end(); u8str.push_back(mku8.inc(i)))
    ;

    return u8str;
}

std::string convertU8strToAsciiStr(const U8string& u8str)
{
    std::string str{};
        
    for (auto&& i : u8str)
    str += i.to_string();
        
    return str;
}


std::string denormalizeToFraction(const std::string& str);
std::string approximation(const std::string& str, std::uint32_t desired_digits);

std::string convertFractionalBaseNToBase10(const std::string& str, std::uint8_t base)
{
    auto convertFractionPart = [&](const std::string& fraction)
    {
        if (!isValidBaseDigits(fraction, base))
        std::cout << "in function convertFractionalBaseNToBase10, the fraction part of str(.[xxxx]) contains invalid base digits\n", std::exit(0);

        boost::multiprecision::cpp_dec_float_50 fractional_part{};
        boost::multiprecision::cpp_dec_float_50 base_alt{int(base)};
        for (std::int64_t exp{-1}; auto&& i : fraction)
        {
            if (digitsMap[std::toupper(i)] != 0)
            fractional_part += boost::multiprecision::cpp_dec_float_50{digitsMap[std::toupper(i)]} * boost::multiprecision::pow(base_alt, {exp});
            
            exp -= 1;
        }

        Streams::StringOutputStream sos{std::ostringstream{}};
        sos.nouppercase().setPrecision(40).scientific().write(fractional_part);

        auto string_fractional_part = sos.get_str();
        
        if (string_fractional_part.contains('e'))
        std::ranges::replace(string_fractional_part, 'e', '@');

        //denoramlization of the calculated fractional number
        string_fractional_part = denormalizeToFraction(string_fractional_part);
        //removal of the leading zero i.e [0].xxxxxxxx
        string_fractional_part.erase(string_fractional_part.begin());
        
        return string_fractional_part;
    };

    if (!str.contains('.'))
    std::cout << "in function convertFractionalBaseNToBase10, str does not contain a radix point", std::exit(0);
    else if (std::ranges::count(str, '.') != 1)
    std::cout << "in function convertFractionalBaseNToBase10, str contains multiple radix point", std::exit(0);

    auto found = std::ranges::find(str, '.');
    return convertBaseNToBase10({str.begin(), found}, base) + convertFractionPart({found + 1, str.end()});
}

/*
    auto convertFractionPart = [&](const std::string& fraction)
    {
        if (!isValidBaseDigits(fraction, 10))
        throw std::runtime_error{ "in function convertFractionalBase10ToBaseN, the fraction part of str(.[xxxx]) contains invalid base digits" };

        double fractional_part{};
        std::uint32_t max_iterations{70};
        std::string result_str{'.'};

        try { Streams::StringInputStream{ std::istringstream{fraction} }.read(fractional_part); }
        catch(std::exception&){}

        for (decltype(max_iterations) i = {}; i < max_iterations; ++i)
        {
            if (fractional_part == 0.0)
            break;

            fractional_part *= double(base);
            result_str.push_back(mapDigits[int(fractional_part)]);
            
            System::coutput.writews_endl("fractional_part(1):", fractional_part);
            if (int(fractional_part) >= 1)
            fractional_part -= double(int(fractional_part));
            System::coutput.writews_endl("fractional_part(2):", fractional_part);
        }
        
        if (result_str.empty())
        result_str.push_back('0');

        return result_str;
    };

    if (!str.contains('.'))
    throw std::runtime_error{ "in function convertFractionalBase10ToBaseN, str does not contain a radix point" };
    else if (std::ranges::count(str, '.') != 1)
    throw std::runtime_error{ "in function convertFractionalBase10ToBaseN, str contains multiple radix point" };

    auto found = std::ranges::find(str, '.');
    return convertBase10ToBaseN({str.begin(), found}, base) + convertFractionPart({found, str.end()});
*/

std::string convertFractionalBase10ToBaseN(const std::string& str, std::uint8_t base)
{
    auto convertFractionPart = [&](const std::string& fraction)
    {
        if (!isValidBaseDigits(fraction, 10))
        std::cout << "in function convertFractionalBase10ToBaseN, the fraction part of str(.[xxxx]) contains invalid base digits", std::exit(0);

        boost::multiprecision::cpp_dec_float_50 fractional_part{};
        boost::multiprecision::cpp_dec_float_50 base_alt{int(base)};
        std::uint32_t max_iterations{50};
        std::string result_str{'.'};

        try { Streams::StringInputStream{ std::istringstream{fraction} }.read(fractional_part); }
        catch(std::exception&){}

        for (decltype(max_iterations) i = {}; i < max_iterations; ++i)
        {
            if (fractional_part == boost::multiprecision::cpp_dec_float_50{"0.0"})
            break;

            fractional_part *= base_alt;
            result_str.push_back(mapDigits[fractional_part.convert_to<int>()]);
            
            if (fractional_part.convert_to<int>() >= 1)
            fractional_part -= boost::multiprecision::cpp_dec_float_50{fractional_part.convert_to<int>()};
        }
        
        if (result_str.empty())
        result_str.push_back('0');

        return result_str;
    };

    if (!str.contains('.'))
    std::cout << "in function convertFractionalBase10ToBaseN, str does not contain a radix point", std::exit(0);
    else if (std::ranges::count(str, '.') != 1)
    std::cout << "in function convertFractionalBase10ToBaseN, str contains multiple radix point", std::exit(0);

    auto found = std::ranges::find(str, '.');
    return convertBase10ToBaseN({str.begin(), found}, base) + convertFractionPart({found, str.end()});
}

std::string normalizeToScientificN(const std::string& str)
{
    std::regex reg{R"(^[1-9]{1}(\.[[:digit:]]*)?[@][+-]?[[:digit:]]+$)"};
    std::string newStr{};

    /*
    [PreCondition]
    1. string input digits must be base(10)
    2. the exponent identifier is not [E|e] but [@]

    [Invariant]
    1. the input string is remains unchanged, regardless of the operation that takes place

    [PostCondition]
    1. the string input must return with a fractional part of atleast [0] and an exponent of atleast/atmost [0]
    2. the returned rvalue string would not habour any leading or trailing implicit zeros, even if it is the result of an operation
    */

    //removal of impilcit leading zeros meaning someting like this: ([000000000]x.xxxxx) is discarded
    if (str.starts_with('0'))
    {
        for (auto i = 0uz, count = 0uz; i != str.size(); ++i)
        {
            if (str[i] != '0')
            {
                if (count != 0)
                newStr.assign(str.begin() + count, str.end());

                break;
            }

            if (str[i] == '0' and !(str[i+1] == '.' or str[i+1] == '@'))
            ++count;
        }
    }
    
    if (newStr.empty())
    newStr.assign(str);

    //removal of implicit trailing zeros
    auto removal_of_implicit_trailing_zeros = [&]
    {
        if (auto radix_pos = newStr.find('.'); radix_pos != std::string::npos)
        {
            auto begin = newStr.find('@');
            if (begin == std::string::npos)
            begin = newStr.size() - 1; //example: so it would point to the character before the end
            else
            --begin;
            //all to ensure that only the fractional part is iterated over x.(xxxxx)@xx

            auto i = begin, leadingZerosCount = 0uz;
            for (; i > radix_pos; i--)
            {
                if (newStr[i] != '0')
                break;
                else if (newStr[i] == '0' and i - 1 == radix_pos) //to save for something like this: (1.0) instead of (1.)
                break;
                else
                ++leadingZerosCount;
            }
            if (leadingZerosCount != 0)
            newStr.erase(i + 1, leadingZerosCount);   
        }
    };
    
    removal_of_implicit_trailing_zeros();

    //enforcing PostCondition(0)
    {
        auto begin = newStr.find('@');
        if (begin == std::string::npos)
        begin = newStr.size();
        
        if (newStr.contains('.'))
        {
            //PostCondition(0) when a radix separator is found
            if (newStr.find('.') == begin - 1)
            newStr.insert(begin, 1, '0');
        }
        else //PostCondition(0) when a radix separator is not found
        newStr.insert(begin, ".0");
    }


    if (std::regex_match(newStr, reg))
    {
        return newStr;
    }
    else if (std::regex_match(newStr, reg = R"(^[0-9]{1}(\.[[:digit:]]*)?$)"))//first case for matching something like this: 0, 0., 0.1234, 1, 1., 1.2345
    {
        if (newStr.starts_with('0'))
        {
            if (newStr.contains('.')) //for normalizing something like this: 0.12345 => 1.2345@-1
            {
                if (auto non_zero_pos = std::string{newStr.begin() + 2, newStr.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                {
                    newStr.insert((non_zero_pos + 2) + 1, 1, '.');
                    return std::string{newStr.begin()+(non_zero_pos + 2), newStr.end()}.append("@-") + std::to_string(non_zero_pos + 1);
                }
            }
        }
        return newStr.append("@0");
    }
    else if (std::regex_match(newStr, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//second case for matching something like this: 12, 12., 12.34
    {
        if (newStr.contains('.'))
        {
            auto radix_loc = newStr.find('.');
            newStr.erase(newStr.begin() + radix_loc);
            newStr.insert(newStr.begin() + 1, '.');
            return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(radix_loc - 1);
        }
        else
        {
            newStr.insert(newStr.begin() + 1, '.');
            return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(newStr.size() - 2);
        }
    }
    else if (std::regex_search(newStr, reg = R"([@][+-]?[[:digit:]]+)"))//third and final case for any fallout that has an exponent, 12.34@1, 123.4@-23
    {
        std::string exponent{};
        //extracting the exponent
        for (auto i = newStr.size() - 1; i != 0; --i)
        {
            if (newStr[i] == '@')
            {
                exponent.append(newStr.begin() + (i + 1), newStr.end());
                newStr.erase(newStr.begin() + i, newStr.end());
                break;
            }
        }
        
        if (std::regex_match(newStr, reg = R"(^[0]{1}(\.[[:digit:]]*)?$)"))//a reiteration of the first case, but one which has an exponent
        {
            if (newStr.contains('.'))
            {
                if (auto non_zero_pos = std::string{newStr.begin() + 2, newStr.end()}.find_first_not_of('0'); non_zero_pos != std::string::npos)
                {
                    newStr.insert((non_zero_pos+2) + 1, 1, '.');
                    return std::string{newStr.begin()+(non_zero_pos+2), newStr.end()}.append({'E'}) + std::to_string(std::stoi(exponent) - int(non_zero_pos+1));
                }
            }
            
            return newStr.append({'@'}).append(exponent.begin(), exponent.end());
        }
        else if (std::regex_match(newStr, reg = R"(^[0-9]{2,}(\.[[:digit:]]*)?$)"))//a reiteration of the second case, but one which has an exponent
        {
            if (newStr.contains('.'))
            {
                auto radix_loc = newStr.find('.');
                newStr.erase(newStr.begin() + radix_loc);
                newStr.insert(newStr.begin()+1, '.');
                return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(std::stoi(exponent) + int(radix_loc - 1));
            }
            else
            {
                newStr.insert(newStr.begin()+1, '.');
                return removal_of_implicit_trailing_zeros(), newStr.append({'@'}) + std::to_string(std::stoi(exponent) + int(newStr.size() - 2));
            }
        }
    }

    //if failure occurs
    std::cout << "in function normalizeToScientificN, the Exponent character is [ @ ](also exponent digits must be specified when the [ @ ] is found) and base(10) digits are expected", std::exit(0);
    
    return "";
}

bool isFPNumberEqualTo(const std::string& str1, const std::string& str2)
{
    //                 |   1     |      2            |      3
    std::regex reg{R"(^([0-9]){1}(\.[[:digit:]]*)?[@]([+-]?[[:digit:]]+)$)"};
    std::smatch smatch1{};
    std::smatch smatch2{};

    if (std::regex_match(str1, smatch1, reg) and std::regex_match(str2, smatch2, reg)) 
    {
        //check if the exponent-part is not equal, because somethin like this: 3.98@-2 is always not equal to 3.98@6
        if (std::stoi(smatch1[3]) != std::stoi(smatch2[3]))
        return false;

        //check if the whole-number-part is not equal first
        if (!equal(smatch1[1], smatch2[1]))
        return false;
        else //check if the fractional-part is not equal next
        {
            std::string fractional_part_1 = smatch1[2].str();
            std::string fractional_part_2 = smatch2[2].str();
            
            if (fractional_part_1.contains('.'))
            fractional_part_1.erase(fractional_part_1.begin());

            if (fractional_part_2.contains('.'))
            fractional_part_2.erase(fractional_part_2.begin());
            
            if (fractional_part_1.empty())
            fractional_part_1.push_back('0');
            
            if (fractional_part_2.empty())
            fractional_part_2.push_back('0');

            if (fractional_part_1.size() != fractional_part_2.size())
            {
                bool fp1_isMax{};

                if (fractional_part_1.size() > fractional_part_2.size())
                fp1_isMax = true;
                else
                fp1_isMax = false;
                
                auto maxSize = (fp1_isMax? fractional_part_1.size() : fractional_part_2.size()) - (fp1_isMax? fractional_part_2.size() : fractional_part_1.size());
                
                //making the size of each fractional_part equal, by adding implicit zeros
                for (auto i = 0uz; i < maxSize; i++)
                {
                    if (!fp1_isMax)
                    fractional_part_1.push_back('0');
                    else
                    fractional_part_2.push_back('0');
                }   
            }
            
            if (equal(fractional_part_1, fractional_part_2))
            return true;

            return false;
        }
    }
    else
    std::cout << "in function isFPNumberEqualTo Don't do that!\n\n", std::exit(0);
    
    return false;
}

bool isFPNumberNotEqualTo(const std::string& str1, const std::string& str2)
{ return !isFPNumberEqualTo(str1, str2); }

bool isFPNumberLessThan(const std::string& str1, const std::string& str2)
{
    std::regex reg{R"(^([0-9]){1}(\.[[:digit:]]*)?[@]([+-]?[[:digit:]]+)$)"};
    std::smatch smatch1{};
    std::smatch smatch2{};

    if (std::regex_match(str1, smatch1, reg) and std::regex_match(str2, smatch2, reg)) 
    {
        //check the exponent first, to see which is lower or greater; something like this: 3.45@32 is always greater than 9.45@10
        if (std::stoi(smatch1[3]) < std::stoi(smatch2[3]))
        return true;
        else if (std::stoi(smatch1[3]) > std::stoi(smatch2[3]))
        return false;

        if (lessThan(smatch1[1], smatch2[1]))
        return true;
        else if (greaterThan(smatch1[1], smatch2[1]))
        return false;
        else
        {
            std::string fractional_part_1 = smatch1[2].str();
            std::string fractional_part_2 = smatch2[2].str();
            
            if (fractional_part_1.contains('.'))
            fractional_part_1.erase(fractional_part_1.begin());

            if (fractional_part_2.contains('.'))
            fractional_part_2.erase(fractional_part_2.begin());
            
            if (fractional_part_1.empty())
            fractional_part_1.push_back('0');
            
            if (fractional_part_2.empty())
            fractional_part_2.push_back('0');

            if (fractional_part_1.size() != fractional_part_2.size())
            {
                bool fp1_isMax{};

                if (fractional_part_1.size() > fractional_part_2.size())
                fp1_isMax = true;
                else
                fp1_isMax = false;
                
                auto maxSize = (fp1_isMax? fractional_part_1.size() : fractional_part_2.size()) - (fp1_isMax? fractional_part_2.size() : fractional_part_1.size());

                //making the size of each fractional_part equal, by adding implicit zeros
                for (auto i = 0uz; i < maxSize; i++)
                {
                    if (!fp1_isMax)
                    fractional_part_1.push_back('0');
                    else
                    fractional_part_2.push_back('0');
                }   
            }

            if (lessThan(fractional_part_1, fractional_part_2))
            return true;
            else if (greaterThan(fractional_part_1, fractional_part_2))
            return false;
            else
            return false;
        }
    }
    else
    std::cout << "in function isFPNumberLessThan Don't do that!\n\n", std::exit(0);
    
    return false;
}

bool isFPNumberGreaterThan(const std::string& str1, const std::string& str2)
{ return !isFPNumberEqualTo(str1, str2) and !isFPNumberLessThan(str1, str2); }

bool isFPNumberLessThanEqualTo(const std::string& str1, const std::string& str2)
{ return isFPNumberEqualTo(str1, str2) or isFPNumberLessThan(str1, str2); }

bool isFPNumberGreaterThanEqualTo(const std::string& str1, const std::string& str2)
{ return isFPNumberEqualTo(str1, str2) or isFPNumberGreaterThan(str1, str2); }

std::string denormalizeToFraction(const std::string& str)
{
    std::regex reg{R"(^[[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)$)"};
    std::smatch smatch{};

    if (std::regex_match(str, smatch, reg))
    {
        std::string newStr{};

        //removal of implicit leading zeros
        if (str.starts_with('0'))
        {
            for (auto i = 0uz, count = 0uz; i != str.size(); ++i)
            {
                if (str[i] != '0')
                {
                    if (count != 0)
                    newStr.assign(str.begin() + count, str.end());

                    break;
                }

                if (str[i] == '0' and !(str[i+1] == '.' or str[i+1] == '@'))
                ++count;
            }
        }
        
        if (newStr.empty())
        newStr.assign(str);

        //removal of the exponent
        newStr.erase(newStr.begin() + newStr.find('@'), newStr.end());

        auto exponent = std::stoi(smatch[2]);

        if (smatch[1].str().empty()) //smatch[i] contains the optional fractional part, inculding the radix point
        {
            if (exponent >= 0)
            {
                for (auto&& i = 0; i < exponent; ++i)
                newStr.push_back('0');

                newStr.append(".0");
            }
            else
            {
                exponent *= -1;

                if (std::size_t(exponent) == newStr.size())
                {
                    newStr.insert(newStr.begin(), {'0', '.'});
                }
                else if (std::size_t(exponent) < newStr.size())
                {   
                    newStr.insert(newStr.begin() + (newStr.size() - std::size_t(exponent)), '.');
                }
                else //if (exponent > newStr.size())
                {
                    auto number_of_fractionalzeros = std::max(exponent, int(newStr.size())) - std::min(exponent, int(newStr.size()));

                    for (auto i = 0; i < number_of_fractionalzeros; ++i)
                    newStr.insert(newStr.begin(), '0');

                    newStr.insert(newStr.begin(), {'0', '.'});
                }
            }
        }
        else
        {
            auto radix_pos = newStr.find('.');
            
            //removal of implicit trailing zeros
            {
                std::size_t i = newStr.size() - 1, leadingZerosCount = {};
                for (; i > radix_pos; i--)
                {
                    if (newStr[i] != '0')
                    break;
                    else ++leadingZerosCount;
                }
                if (leadingZerosCount != 0)
                newStr.erase(i + 1, leadingZerosCount);
            }
            
            auto whole_number_size = radix_pos;
            newStr.erase(newStr.begin() + radix_pos); //also removing the radix separator
            auto fractional_part_size = newStr.size() - whole_number_size; //fractional_part_size depends on newStr devoid of the radix seperator

            if (exponent >= 0)
            {
                if (std::size_t(exponent) > fractional_part_size)
                {
                    auto newExponent = std::max(exponent, int(fractional_part_size)) - std::min(exponent, int(fractional_part_size));
                    
                    for (auto&& i = 0; i < newExponent; ++i)
                    newStr.push_back('0');

                    newStr.append(".0");
                }
                else if (std::size_t(exponent) < fractional_part_size)
                {
                    auto newRadixPos = exponent + radix_pos;
                    newStr.insert(newStr.begin() + newRadixPos, '.');
                }
                else
                {
                    newStr.append(".0");
                }
            }
            else
            {
                exponent *= -1;

                if (std::size_t(exponent) == whole_number_size)
                {
                    newStr.insert(newStr.begin(), {'0', '.'});
                }
                else if (std::size_t(exponent) < whole_number_size)
                {   
                    newStr.insert(newStr.begin() + (whole_number_size - std::size_t(exponent)), '.');
                }
                else //if (exponent > newStr.size())
                {
                    auto number_of_fractionalzeros = std::max(exponent, int(whole_number_size)) - std::min(exponent, int(whole_number_size));

                    for (auto i = 0; i < number_of_fractionalzeros; ++i)
                    newStr.insert(newStr.begin(), '0');

                    newStr.insert(newStr.begin(), {'0', '.'});
                }
            }
        }

        return newStr;
    }
    else
    std::cout << R"(in function denormalizeToFraction format for string was not met, i.e [[:alnum:]]{1,}(\.[[:alnum:]]*)?[@]([+-]?[[:digit:]]+)", std::exit(0);

    return "";
}

std::string approximation(const std::string& str, std::uint32_t desired_digits)
{
    /*
        [PreCondition]
        1. input string must the return value of normalizeScientificN
    */

    std::regex reg{R"(^([0-9]{1})(\.[[:digit:]]*)([@][+-]?[[:digit:]]+)$)"};
    std::smatch smatch{};

    if (std::regex_match(str, smatch, reg))
    {
        if (str.front() == '0')
        return str;
        
        std::string fractional_part = smatch[2];
        fractional_part.erase(fractional_part.begin()); //removing the radix separator

        if (fractional_part.size() > desired_digits)
        {
            std::string wholeNumber_part = smatch[1];
            std::string exponent_part = smatch[3];

            auto index_of_the_first_digit_to_remove = desired_digits;

            if (digitsMap[fractional_part[index_of_the_first_digit_to_remove]] >= 5)
            {
                auto carry_digit = 0;
                auto temp_insert_digit = 0;
                for (int i = index_of_the_first_digit_to_remove - 1 /*to get the last safe digit index*/; i >= 0; --i)
                {
                    temp_insert_digit = 1 + digitsMap[fractional_part[i]];
                    carry_digit = temp_insert_digit / 10, temp_insert_digit %= 10;

                    fractional_part.replace((std::size_t)i, 1uz, std::string{mapDigits[temp_insert_digit]});
                    
                    if (carry_digit == 0)
                    break;
                }
                
                if (carry_digit != 0)
                wholeNumber_part.replace(0uz, 1uz, "10");
            }
            //removing the undesired digits
            fractional_part.erase((std::size_t)index_of_the_first_digit_to_remove);
            //insertion of the radix separator again
            fractional_part.insert(fractional_part.begin(), '.');

            return normalizeToScientificN(wholeNumber_part + fractional_part + exponent_part);
        }
        return str;
    }
    else
    std::cout << R"(in function approximation, the PreCondition ditates that the str must be in this format: ^[1-9]{1}\.[[:digit:]]*?[@][+-]?[[:digit:]]+$)", std::exit(0);

    return "";
}

void max_normal(int n_of_mantissa, int exponent_bais)
{
    std::cout << "max_normal: ";
    using boost50 = boost::multiprecision::cpp_dec_float_50;
    std::cout << (boost50{2} - boost::multiprecision::pow(boost50{2}, {-n_of_mantissa})) * boost::multiprecision::pow(boost50{2}, {exponent_bais}) << '\n';
}

void min_denormal(int n_of_mantissa, int default_denormal_exp)
{
    std::cout << "min_denormal: ";
    using boost50 = boost::multiprecision::cpp_dec_float_50;
    std::cout << boost::multiprecision::pow(boost50{2}, {-n_of_mantissa}) * boost::multiprecision::pow(boost50{2}, {default_denormal_exp}) << '\n';
}

void 👩🏿👀(int a, int)
{
    std::cout << "a: " << a << '\n';
}

namespace R
{
    #include <tuple>
    
};

main()
{
    try
    {
        //1.0000101E⋶23 1.000101⋲32 1.000101⋿23 0e.233E⋿23 0e.sdkfje⋿34 2.34⋿23
        /*g++ -Iheader -I"C:\Program Files\Boost" -L"C:/Program Files/Boost/stage/lib" -std=c++23 pseudomain.cpp -o pseudomain -lboost_locale-mgw13-mt-d-x64-1_84*/

        // for (auto i : convertCharstrToU8str("+§№34™ad℉asf®ad_©℗ad⁆"))
        // i.info();

        // std::string{}.std::string::begin(); //did not know i could do this

        // std::basic_string<char>::const_iterator

        // System::coutput.setPrecision(40);
        // max_normal(10, 15), max_normal(23, 127), max_normal(52, 1'023), max_normal(112, 16'383);
        // std::cout << '\n';
        // min_denormal(10, -14), min_denormal(23, -126), min_denormal(52, -1'022), min_denormal(112, -16'382);
        
        //add optimization to the number checks std::all_of(str, [](char& x){ return x == '\0' })

    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception{ " << e.what() << " }\n";
    }
}

std::ostream& operator<<(std::ostream& os, const intN_vector& d)
{
    for (auto &&i : d)
    {
        os << int(i);
    }
    return os;
}
