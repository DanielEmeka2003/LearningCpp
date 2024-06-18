#ifndef U8CHAR_H
#define U8CHAR_H

#pragma once

#include <string>
#include <cstdint>
#include <bitset>
#include <utility>

#include "misc_utility.h"

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
    }

    auto convertToUtf8CodePoint() const
    {
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
            io::cout.write(nc_misc::convertIntegerBase10ToBaseN(utf8_cp, 2), "₂ ");
            io::cout.write(utf8_cp, "₁₀ ");
            io::cout.write(nc_misc::convertIntegerBase10ToBaseN(utf8_cp, 16), "₁₆");
            io::cout.write("\033[0m");

            io::cout.write_nl(']');
        }

        io::cout.write_nl("");

        io::cout.write("UNICODE_CODE_POINT: [");
        {
            auto u_cp = convertToUnicodeCodePoint();

            io::cout.write("\033[32;1m");
            io::cout.write(nc_misc::convertIntegerBase10ToBaseN(u_cp, 2), "₂ ");
            io::cout.write(u_cp, "₁₀ ");
            io::cout.write(nc_misc::convertIntegerBase10ToBaseN(u_cp, 16), "₁₆");
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

inline std::ostream& operator<<(std::ostream& os, const U8string& d)
{
    for (auto&& i : d)
    os << i;

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const U8string_view& d)
{
    for (auto&& i : d)
    os << i;

    return os;
}

namespace u8_misc
{
    bool isBasicLatinLowerCaseAlphabet(std::uint32_t unicode_code_point);
    bool isBasicLatinUpperCaseAlphabet(std::uint32_t unicode_code_point);

    inline bool isBasicLatinDigit(std::uint32_t unicode_code_point)
    { return unicode_code_point >= 0x30 and unicode_code_point <= 0x39; }

    inline bool isBasicLatinAlphabet(std::uint32_t unicode_code_point)
    { return isBasicLatinLowerCaseAlphabet(unicode_code_point) or isBasicLatinUpperCaseAlphabet(unicode_code_point); }

    inline bool isBasicLatinAlphabetNumeral(std::uint32_t unicode_code_point)
    { return isBasicLatinAlphabet(unicode_code_point) or isBasicLatinDigit(unicode_code_point); }

    inline bool isBasicLatinLowerCaseAlphabet(std::uint32_t unicode_code_point)
    { return unicode_code_point >= 0x61 and unicode_code_point <= 0x7a; }

    inline bool isBasicLatinUpperCaseAlphabet(std::uint32_t unicode_code_point)
    { return unicode_code_point >= 0x41 and unicode_code_point <= 0x5a; }

    inline bool isBasicLatinWhiteSpace(std::uint32_t unicode_code_point)
    { return unicode_code_point == 0x20 or (unicode_code_point >= 0x9 and unicode_code_point <= 0xd); }

    inline U8char basicLatinToLowerCase(const U8char& x)
    {
        auto unicode_code_point = std::uint32_t(x);

        if (isBasicLatinLowerCaseAlphabet(unicode_code_point))
        return x;
        
        unicode_code_point += 32;

        return U8char{std::string{(char)unicode_code_point}};
    }

    inline U8char basicLatinToUpperCase(const U8char& x)
    {
        auto unicode_code_point = std::uint32_t(x);

        if (isBasicLatinUpperCaseAlphabet(unicode_code_point))
        return x;

        unicode_code_point -= 32;
        return U8char{std::string{(char)unicode_code_point}};
    }

    inline bool basicLatinAlphabetCaseInsensitiveCompare(const U8char& lhs, const U8char& rhs)
    { return basicLatinToLowerCase(lhs) == basicLatinToLowerCase(rhs); }


    inline bool isBasicLatin(std::uint32_t unicode_code_point)
    { return unicode_code_point <= 0x7f; }


    inline std::size_t size_of_u8string_as_individual_bytes(U8string_view x)
    {
        std::size_t accumulated_size{};
        for (auto &&i : x)
        accumulated_size += std::size_t(i.getByteEncoding());
        
        return accumulated_size;
    }

    inline U8string convertAsciiStrToU8Str(std::string_view str)
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

    inline std::string convertU8StrToAsciiStr(U8string_view u8str)
    {
        std::string str{};

        for (auto&& i : u8str)
        str += i.to_string();

        return str;
    }
}

inline U8char operator ""_u8(const char* x, std::size_t s)
{ return U8char{ std::string_view{x, s} }; }

inline U8char operator ""_u8(char x)
{ return U8char{std::string{x}}; }

inline U8string operator ""_u8str(const char* x, std::size_t s)
{ return u8_misc::convertAsciiStrToU8Str(std::string{x, s}); }

inline bool operator==(std::string_view lhs, U8string_view rhs)
{
    if (lhs.size() != u8_misc::size_of_u8string_as_individual_bytes(rhs))
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

inline bool operator==(U8string_view lhs, std::string_view rhs)
{ return rhs == lhs; }

inline bool operator==(const U8char& lhs, std::string_view rhs)
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

inline bool operator==(std::string_view lhs, const U8char& rhs)
{ return rhs == lhs; }

#endif