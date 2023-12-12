#include <cctype>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <regex>
#include <utility>

#include "ncLexer.h"
#include "stream/output.h"
#include "stream/stringinputstream.h"

namespace Nc
{
    NcLexer::NcLexer(std::string_view fileBuffer): m_fileBuffer{fileBuffer}
    {}

    NcLexer::NcLexer(std::string&& fileBuffer): m_fileBuffer{ std::move(fileBuffer) }
    {}

    void NcLexer::lexFileBuffer()
    {
        while (lexify() != TokenType::_eot)
        {}

        if (!m_log.isLogEmpty())
        {
            errorOutput();
        }
    }
    
    std::vector<NcLexer::TokenData>& NcLexer::getTokenList()
    { return m_tokenList; }

    const std::vector<NcLexer::TokenData>& NcLexer::getTokenList() const
    { return m_tokenList; }
    
    std::uint32_t NcLexer::getFilePosition() const
    { return m_filePosition; }

    void NcLexer::logBoxStyle(NcLog::BoxLineStyle s)
    {
        m_log.changeBoxLineStyle(s);
    }





    NcLexer::TokenType NcLexer::lexify()
    {
        //clear the token string before a lexing operation begins
        m_tokenString.clear();

        try //main try block for exiting lexing when eof is reached
        {
            while (std::isspace(m_fileBuffer.at(m_filePosition)))
            {
                if (m_fileBuffer.at(m_filePosition) == '\n')
                {
                    ++m_line;
                    m_absoluteColumn = {};
                }
                moveFilePosAndColumn();
            }
            
            if (std::isalpha(m_fileBuffer.at(m_filePosition)) or m_fileBuffer.at(m_filePosition) == *underscore.begin())
            {
                return tokenizeWords();
            }
            else if (m_fileBuffer.at(m_filePosition) == *signleQuotes.begin())
            {
                return tokenizeCharLiterals();
            }
            else if (m_fileBuffer.at(m_filePosition) == *doubleQuotes.begin())
            {
                return tokenizeStrLiterals();
            }
            else if (std::isdigit(m_fileBuffer.at(m_filePosition)))
            {
                return tokenizeNumbers();
            }
            else
            {
                return tokenizeSymbols();   
            }
        }
        catch(const std::exception&){}
        
        return TokenType::_eot;
    }

    void NcLexer::moveFilePosAndColumn(std::uint32_t number)
    { m_filePosition += number, m_absoluteColumn += number; }
    
    void NcLexer::recordRelativeColumn()
    {
        m_relativeColumn = m_absoluteColumn;
    }

    NcLexer::TokenType NcLexer::tokenizeWords()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move from here

        try
        {
            while (std::isalnum(m_fileBuffer.at(m_filePosition)) or m_fileBuffer.at(m_filePosition) == *underscore.begin())
            {
                moveFilePosAndColumn();
            }
        }
        catch(const std::exception&){}

        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));
        
        if (TokenType result{reservedWords()}; result == TokenType::reserved)
        return result;

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{true}
        });

        return TokenType::identifier;
    }
    
    NcLexer::TokenType NcLexer::reservedWords()
    {
        if
        (
            m_tokenString == rConst or m_tokenString == rInt8 or m_tokenString == rInt16 or m_tokenString == rInt32 or
            m_tokenString == rInt64 or
            m_tokenString == rUint8 or m_tokenString == rUint16 or m_tokenString == rUint32 or m_tokenString == rUint64 or
            m_tokenString == rBool or m_tokenString == rFloat32 or m_tokenString == rFloat64 or m_tokenString == rVoid or
            m_tokenString == rMain or m_tokenString == rGive or m_tokenString == rTrue or m_tokenString == rFalse or
            m_tokenString == rIf or m_tokenString == rElse or m_tokenString == rWhile or m_tokenString == rAnd or m_tokenString == rOr
            or m_tokenString == rNot or m_tokenString == rXor or m_tokenString == rBitwiseAnd or m_tokenString == rBitwiseOr or
            m_tokenString == rBitwiseNot or m_tokenString == rBitwiseXor or m_tokenString == rOdd or m_tokenString == rWrite or
            m_tokenString == rRead
        )
        {
            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, 
                .line{m_line}, 
                .absoluteColumn{m_absoluteColumn},
                .relativeColumn{m_relativeColumn},
                .isReserved{true}
            });
            
            return TokenType::reserved; //this does not make sense, but i am too lazy, so...
        }
        return TokenType::_miscellany;
    }


    NcLexer::TokenType NcLexer::validateNumberL(std::string& literalType)
    {
        if (m_tokenString.contains('.') and std::ranges::count(m_tokenString, '.') != 1)
        {
            m_log.writews("[TOKEN ERROR] float literal [", m_tokenString, "] is invalid - multiple radix seperator");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        if (m_fileBuffer.at(m_filePosition) == *hashtag.begin())
        {
            moveFilePosAndColumn(); //move the file position past the hashtag
            
            auto tempLiteralHolder = m_tokenString;
            m_tokenString.clear();
            auto result = tokenizeWords();
            literalType = m_tokenString;
            m_tokenString = std::move(tempLiteralHolder);


            constexpr std::string_view lI8{"i8L"};
            constexpr std::string_view lI16{"i16L"};
            constexpr std::string_view lI32{"i32L"};
            constexpr std::string_view lI64{"i64L"};
            constexpr std::string_view lUi8{"ui8L"};
            constexpr std::string_view lUi16{"ui16L"};
            constexpr std::string_view lUi32{"ui32L"};
            constexpr std::string_view lUi64{"ui64L"};
            constexpr std::string_view lf32{"f32L"};
            constexpr std::string_view lf64{"f64L"};

            auto make_regex = [](std::string_view s1, std::string_view s2)
            {
                using namespace std::string_literals;
                return std::regex{ std::string{s1} + "|"s + std::string{s2}, std::regex_constants::icase };
            };

            if (result == TokenType::reserved)
            {
                m_log.writews("[TOKEN ERROR] reserved word [", literalType, "] is not number literal type");
                finalizeLog();
                return TokenType::_miscellany;
            }
            else if( std::regex_match(literalType, make_regex(lI8, lInt8)) )
            {
                std::int16_t i16{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i16);

                    if (!std::in_range<std::int8_t>(i16))
                    throw std::runtime_error{ Streams::InputStream<char>::failbit_m.data() };
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] int8 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] int8 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lInt8;
            }
            else if( std::regex_match(literalType, make_regex(lI16, lInt16)) )
            {
                std::int16_t i16{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i16);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] int16 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] int16 literal [", m_tokenString, "] is invalid - not within range");
                    
                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lInt16;
            }
            else if( std::regex_match(literalType, make_regex(lI32, lInt32)) )
            {
                std::int32_t i32{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i32);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] int32 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] int32 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lInt32;
            }
            else if( std::regex_match(literalType, make_regex(lI64, lInt64)) )
            {
                std::int64_t i64{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i64);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] int64 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] int64 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lInt64;
            }
            else if( std::regex_match(literalType, make_regex(lUi8, lUint8)) )
            {
                std::int16_t i16{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i16);

                    if (!std::in_range<std::uint8_t>(i16))
                    throw std::runtime_error{ Streams::InputStream<char>::failbit_m.data() };
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] uint8 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] uint8 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lUint8;
            }
            else if( std::regex_match(literalType, make_regex(lUi16, lUint16)) )
            {
                std::uint16_t ui16{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(ui16);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] uint16 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] uint16 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lUint16;
            }
            else if( std::regex_match(literalType, make_regex(lUi32, lUint32)) )
            {
                std::uint32_t ui32{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(ui32);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] uint32 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] uint32 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lUint32;
            }
            else if( std::regex_match(literalType, make_regex(lUi64, lUint64)) )
            {
                std::uint64_t ui64{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(ui64);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] uint64 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] uint64 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lUint64;
            }
            else if( std::regex_match(literalType, make_regex(lf32, lFloat32)) )
            {
                float f32{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(f32);

                    m_tokenString = std::to_string(f32);
                }
                catch(const std::exception&)
                {
                    m_log.write("[TOKEN ERROR] float32 literal [", m_tokenString, "] is invalid - not within range");
                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lFloat32;
            }
            else if( std::regex_match(literalType, make_regex(lf64, lFloat64)) )
            {
                double f64{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(f64);

                    m_tokenString = std::to_string(f64);
                }
                catch(const std::exception&)
                {
                    m_log.write("[TOKEN ERROR] float64 literal [", m_tokenString, "] is invalid - not within range");
                    finalizeLog();
                    return TokenType::_miscellany;
                }
                literalType = lFloat64;
            }
            else
            {
                m_log.writews("[TOKEN ERROR] identifier [", literalType, "] is not a literal type");
                finalizeLog();
                return TokenType::_miscellany;
            }
        }
        else
        {
            if (m_tokenString.contains('.'))
            {
                double f64{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(f64);

                    m_tokenString = std::to_string(f64);
                }
                catch(const std::exception&)
                {
                    m_log.write("[TOKEN ERROR] float64 literal [", m_tokenString, "] is invalid - not within range");
                    finalizeLog();
                    return TokenType::_miscellany;
                }

                literalType = lFloat64;
            }
            else
            {
                std::int32_t i32{};
                try
                {
                    Streams::StringInputStream sis{std::istringstream{}};
                    sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
                    sis.read(i32);
                }
                catch(const std::exception& e)
                {
                    if (e.what() == Streams::InputStream<char>::leftoverInput_m)
                    m_log.writews("[TOKEN ERROR] int32 literal [", m_tokenString, "] is invalid");
                    else
                    m_log.writews("[TOKEN ERROR] int32 literal [", m_tokenString, "] is invalid - not within range");

                    finalizeLog();
                    return TokenType::_miscellany;
                }

                literalType = lInt32;
            }
        }

        return TokenType::literal;
    }

    NcLexer::TokenType NcLexer::tokenizeNumbers()
    {
        // if (m_fileBuffer.at(m_filePosition) == '0')
        // {
        //     try
        //     {
        //         if (auto temp1{m_filePosition+1}; m_fileBuffer.at(temp1) == 'x' or m_fileBuffer.at(temp1) == 'X')
        //         {
        //             moveFilePosAndColumn(2); //move the file position past 0x/X
        //             return hexadecimal();
        //         }
        //         else if (auto temp2{m_filePosition+1}; m_fileBuffer.at(temp2) == 'b' or m_fileBuffer.at(temp2) == 'B')
        //         {
        //             moveFilePosAndColumn(2); //move the file position past 0b/B
        //             return binary();
        //         }
        //         else if (auto temp3{m_filePosition+1}; m_fileBuffer.at(temp3) == 'c' or m_fileBuffer.at(temp3) == 'C')
        //         {
        //             moveFilePosAndColumn(2); //move the file position past 0
        //             return octal();
        //         }
        //     }
        //     catch(const std::exception&){}
        // }

        auto formerFilePos{m_filePosition}; //maintain intial index
        recordRelativeColumn(); //do not move

        try
        {
            while
            (
                std::isdigit(m_fileBuffer.at(m_filePosition)) or //check if it is digit first
                m_fileBuffer.at(m_filePosition) == '\'' or //check if it has a number seperator
                m_fileBuffer.at(m_filePosition) == '.' //check if it has a radix seperator
            )
            {
                moveFilePosAndColumn();
            }
        }
        catch(const std::exception&){}
        
        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));
            
        //check if the number seperator is at the end of the number literal
        //and if true terminate lexing, because that is just wrong
        if (*std::prev(m_tokenString.end()) == '\'')
        {
            m_log.writews("[TOKEN ERROR] token [ ' ] should not end a number literal [", m_tokenString, ']');
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        std::string literalType{};

        if (validateNumberL(literalType) == TokenType::_miscellany)
        return TokenType::_miscellany;

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{},
            .literalType{ std::move(literalType) },
        });

        return TokenType::literal;
    }  
           
    
    
    NcLexer::TokenType NcLexer::hexadecimal()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move

        try
        {
            while
            (
                std::isxdigit(m_fileBuffer.at(m_filePosition)) or //check if it is digit first
                m_fileBuffer.at(m_filePosition) == '\'' or //check if it has a number seperator
                m_fileBuffer.at(m_filePosition) == '.' //check if it has a radix seperator
            )
            { moveFilePosAndColumn(); }
        }
        catch(const std::exception&){}

        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));

        //check if the number seperator is at the end of the number literal
        //and if true terminate lexing, because that is just wrong
        if (*std::prev(m_tokenString.end()) == '\'')
        {
            m_log.write("[TOKEN ERROR] token ['] should not end a number literal");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //validate the number
        //check if it has a radix seperator terminate lexing, because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_log.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        Streams::StringInputStream sis{std::istringstream{}};
        std::uint64_t variable{};
        try
        {
            sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
            sis.hex();
            sis.read(variable);
        }
        catch(const std::exception&)
        {
            m_log.writews("[TOKEN ERROR] hexadecimal integral is invalid: [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        //now we know the hexadecimal is valid, so we just convert variable to a string to store it as a decimal
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{},
            .literalType{lInt32.data()}
        });

        return TokenType::literal;
    }
    
    NcLexer::TokenType NcLexer::binary()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move

        try
        {
            while
            (
                std::isdigit(m_fileBuffer.at(m_filePosition)) or //check if it is digit first
                m_fileBuffer.at(m_filePosition) == '\'' or //check if it has a number seperator
                m_fileBuffer.at(m_filePosition) == '.' //check if it has a radix seperator
            )
            { moveFilePosAndColumn(); }
        }
        catch(const std::exception&){}

        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));

        //check if the number seperator is at the end of the number literal
        //and if true terminate lexing, because that is just wrong
        if (*std::prev(m_tokenString.end()) == '\'')
        {
            m_log.write("[TOKEN ERROR] token ['] should not end a number literal");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //check if it has a radix seperator and terminate lexing because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_log.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        if (!std::ranges::all_of( m_tokenString, [](char x){return x == '0' or x == '1';} ) )
        {
            m_log.write("[TOKEN ERROR] invalid binary literal [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }

        std::uint64_t variable{};
        try
        {
            variable = std::bitset<100>{m_tokenString}.to_ullong();
        }
        catch(const std::exception&)
        {
            m_log.writews("[TOKEN ERROR] binary literal is invalid [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        //now we are sure that the binary is valid, so we get the decimal form of it and convert to a string
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{},
            .literalType{lInt32.data()}
        });

        return TokenType::literal;
    }
    
    NcLexer::TokenType NcLexer::octal()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move

        try
        {
            while
            (
                std::isdigit(m_fileBuffer.at(m_filePosition)) or //check if it is digit first
                m_fileBuffer.at(m_filePosition) == '\'' or //check if it has a number seperator
                m_fileBuffer.at(m_filePosition) == '.' //check if it has a radix seperator
            )
            { moveFilePosAndColumn(); }
        }
        catch(const std::exception&){}

        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));

        //check if the number seperator is at the end of the number literal
        //and if true terminate lexing, because that is just wrong
        if (*std::prev(m_tokenString.end()) == '\'')
        {
            m_log.write("[TOKEN ERROR] token ['] should not end a number literal");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //validate the number
        //check if it has a radix seperator terminate lexing, because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_log.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        //check if the digits 8 or 9 are in the string and if true terminate lexing
        if (m_tokenString.contains('8') or m_tokenString.contains('9'))
        {
            m_log.write("[TOKEN ERROR] invalid octal literal [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        

        Streams::StringInputStream sis{std::istringstream{}};
        std::uint64_t variable{};
        try
        {
            sis.insert_str(sis.prepare_str_for_insertion(m_tokenString));
            sis.oct();
            sis.read(variable);
        }
        catch(const std::exception& e)
        {
            m_log.writews("[TOKEN ERROR] ocatl literal is invalid: [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        //now we know the octal is valid, so we just convert variable to a string to store it as a decimal
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{},
            .literalType{lInt32.data()}
        });

        return TokenType::literal;
    }

    NcLexer::TokenType NcLexer::tokenizeCharLiterals()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move

        try
        {
            moveFilePosAndColumn(); //skip past the single quote that was found

            while (m_fileBuffer.at(m_filePosition) != *signleQuotes.begin())
            {
                if (m_fileBuffer.at(m_filePosition) == '\n')
                {
                    throw std::runtime_error{""};
                }
                
                if (m_fileBuffer.at(m_filePosition) == '\\')//if the current character is a backslash move past it
                {
                    moveFilePosAndColumn();
                }
                
                moveFilePosAndColumn();
            }

            moveFilePosAndColumn(); //for extracting the loop terminating single quote too
            
            //extract the token to m_tokenString
            for (size_t i = formerFilePos; i < m_filePosition; ++i)
            m_tokenString.push_back(m_fileBuffer.at(i));
            
            if (m_tokenString.contains('\\')) //check the size of the character literal
            {
                if (m_tokenString.length() != 4)
                {
                    m_log.writews("[TOKEN ERROR] character literal [", m_tokenString, "] is invalid");
                    finalizeLog();
                    return TokenType::_miscellany;
                }
            }
            else
            {
                if (m_tokenString.length() != 3)
                {
                    m_log.writews("[TOKEN ERROR] character literal [", m_tokenString, "] is invalid");
                    finalizeLog();
                    return TokenType::_miscellany;
                }
            }
            
            //validate escape sequence
            if (auto result{m_tokenString.find('\\')}; result != std::string::npos)
            {
                switch (m_tokenString.at(result + 1))
                {
                case 'b':
                case 'r':
                case 't':
                case 'n':
                case 'v':
                case 'a':
                case '\'':
                case '"':
                case '\\':
                case '0':
                    break;
                default:
                    m_log.write("[TOKEN ERROR] token [ \\", m_tokenString.at(result + 1), " ] is not an escape sequence");
                    finalizeLog();
                    return TokenType::_miscellany;
                    break;
                }
            }
            
            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, 
                .line{m_line}, 
                .absoluteColumn{m_absoluteColumn},
                .relativeColumn{m_relativeColumn},
                .isReserved{}, 
                .isIdentifier{},
                .literalType{lChar.data()}
            });

        }
        catch(const std::exception&)
        {
            m_log.write("[TOKEN ERROR] unterminated character literal");
            finalizeLog();
            return TokenType::_miscellany;
        }

        return TokenType::literal;
    }
    
    NcLexer::TokenType NcLexer::tokenizeStrLiterals()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move

        try
        {
            moveFilePosAndColumn(); //skip past the double quote that was found

            while (m_fileBuffer.at(m_filePosition) != *doubleQuotes.begin())
            {
                if (m_fileBuffer.at(m_filePosition) == '\n')
                {
                    throw std::runtime_error{""};
                }
                
                if (m_fileBuffer.at(m_filePosition) == '\\')//if the current character is a backslash move past it
                {
                    moveFilePosAndColumn();
                }

                moveFilePosAndColumn();
            }

            moveFilePosAndColumn(); //for extracting the loop terminating double quote too

            //extract the token to m_tokenString
            for (size_t i = formerFilePos; i < m_filePosition; ++i)
            m_tokenString.push_back(m_fileBuffer.at(i));

            //validate escape sequence
            if (auto result{m_tokenString.find('\\')}; result != std::string::npos)
            {
                switch (m_tokenString.at(result + 1))
                {
                case 'b':
                case 'r':
                case 't':
                case 'n':
                case 'v':
                case 'a':
                case '\'':
                case '"':
                case '\\':
                    break;
                default:
                    m_log.write("[TOKEN ERROR] token [ \\", m_tokenString.at(result + 1), " ] is not an escape sequence");
                    finalizeLog();
                    return TokenType::_miscellany;
                    break;
                }
            }
            

            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, 
                .line{m_line}, 
                .absoluteColumn{m_absoluteColumn},
                .relativeColumn{m_relativeColumn},
                .isReserved{}, 
                .isIdentifier{},
                .literalType{lStr.data()}
            });

        }
        catch(const std::exception&)
        {
            m_log.write("[TOKEN ERROR] unterminated string literal");
            finalizeLog();
            return TokenType::_miscellany;
        }

        return TokenType::literal;
    }

    NcLexer::TokenType NcLexer::tokenizeSymbols()
    {
        recordRelativeColumn(); //do not move

        switch (m_fileBuffer.at(m_filePosition))
        {
        case *sMinus.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sGreaterthan.begin())
                {
                    m_tokenString.assign(sArrow.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sSubtractionAssign.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sMinus.begin())
                {
                    m_tokenString.assign(sDecrement.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sMinus.data());
                    moveFilePosAndColumn();
                }
            }
            catch(const std::exception&)//when .at(std::size_t) throws an exception, then it must be just that symbol
            {
                m_tokenString.assign(sMinus.data());
                moveFilePosAndColumn();
            }
        
            break;

        case *sLessthan.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sLessthan_equalto.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sLessthan.begin())
                {
                    if (m_fileBuffer.at(temp + 1) == *sEqual.begin())
                    {
                        m_tokenString.assign(sBitwiseShiftLeftAssign.data());
                        moveFilePosAndColumn(3);  
                    }
                    else
                    {
                        m_tokenString.assign(sBitwiseShiftLeft.data());
                        moveFilePosAndColumn(2);
                        
                    }
                }
                else
                {
                    m_tokenString.assign(sLessthan.data());
                    moveFilePosAndColumn();
                } 
            }
            catch(const std::exception&)//when .at(std::size_t) throws an exception, then it must be just that symbol
            {
                m_tokenString.assign(sLessthan.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sGreaterthan.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sGreaterthan_equalto.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sGreaterthan.begin())
                {
                    if (m_fileBuffer.at(temp + 1) == *sEqual.begin())
                    {
                        m_tokenString.assign(sBitwiseShiftRightAssign.data());
                        moveFilePosAndColumn(3);  
                    }
                    else
                    {
                        m_tokenString.assign(sBitwiseShiftRight.data());
                        moveFilePosAndColumn(2);
                        
                    }
                }
                else
                {
                    m_tokenString.assign(sGreaterthan.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sGreaterthan.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sAssign.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sEqual.data());
                    moveFilePosAndColumn(2); //move the file position ahead of the equality symbol
                }
                else
                {
                    m_tokenString.assign(sAssign.data());
                    moveFilePosAndColumn();
                }
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sAssign.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sNot.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sNotEqual.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sNot.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sNot.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sBitwiseAnd.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sAnd.begin())
                {
                    m_tokenString.assign(sAnd.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sBitwiseAndAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sBitwiseAnd.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sBitwiseAnd.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sBitwiseOr.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sOr.begin())
                {
                    m_tokenString.assign(sOr.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sBitwiseOrAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sBitwiseOr.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sBitwiseOr.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sBitwiseXor.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sXor.begin())
                {
                    m_tokenString.assign(sXor.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sBitwiseXorAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sBitwiseXor.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sBitwiseXor.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sPlus.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sAdditionAssign.data());
                    moveFilePosAndColumn(2);
                }
                else if (m_fileBuffer.at(temp) == *sPlus.begin())
                {
                    m_tokenString.assign(sIncrement.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sPlus.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sPlus.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sMultiply.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sMultiplicationAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sMultiply.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sMultiply.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sDivide.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sDivisionAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sDivide.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sDivide.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sModuluo.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sEqual.begin())
                {
                    m_tokenString.assign(sModuluoAssign.data());
                    moveFilePosAndColumn(2);
                }
                else
                {
                    m_tokenString.assign(sModuluo.data());
                    moveFilePosAndColumn();
                }                
            }
            catch(const std::exception&)
            {
                m_tokenString.assign(sModuluo.data());
                moveFilePosAndColumn();
            }
            
            break;

        case *sLcurly.begin():
        case *sRcurly.begin():
        case *sLparen.begin():
        case *sRparen.begin():
        case *sSemicolon.begin():
        case *sBitwiseNot.begin():
        case *sComma.begin():
        case *sColon.begin():
        case *sQuestionMark.begin():

            {
                m_tokenString.push_back(m_fileBuffer.at(m_filePosition));
                moveFilePosAndColumn(); //move the file position past the symbol
            }
            
            break;

        default:
            //keep the unrecongizned token for error purposes
            m_tokenString.push_back(m_fileBuffer.at(m_filePosition));
            moveFilePosAndColumn(); //move the file position past the symbol

            m_log.writews("[TOKEN ERROR] unrecognized token [", m_tokenString, "]");
            finalizeLog();
            return TokenType::_miscellany;
        }
        
        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, 
            .line{m_line}, 
            .absoluteColumn{m_absoluteColumn},
            .relativeColumn{m_relativeColumn},
            .isReserved{}, 
            .isIdentifier{},
            .literalType{},
            .isSymbol{true}
        });

        return TokenType::symbol;
    }

    
    void NcLexer::finalizeLog()
    {
        m_log.log();

        m_log.writews("   line:", m_line, "column: [", m_relativeColumn, '-', m_absoluteColumn, ']');
        m_log.log();
    }

    [[noreturn]] void NcLexer::errorOutput()
    {
        m_log.write("{compilation terminated with an error}");
        m_log.log();
        
        m_log.outputLogEntries(System::coutput);

        m_line = {};
        m_absoluteColumn = {};
        m_relativeColumn = {};
        m_fileBuffer.~basic_string();
        m_filePosition = {};
        m_tokenList.~vector();
        m_tokenString.~basic_string();
        m_log.~NcLog();
        std::exit(EXIT_FAILURE);
    }

    std::ostream& operator<<(std::ostream& out, NcLexer::TokenType temp)
    {
        switch (temp)
        {
        case NcLexer::TokenType::reserved:
            return out << "reserved";
        case NcLexer::TokenType::identifier:
            return out << "identifier";
        case NcLexer::TokenType::literal:
            return out << "literal";
        case NcLexer::TokenType::symbol:
            return out << "symbol";
        case NcLexer::TokenType::_miscellany:
            return out << "NULL";
        case NcLexer::TokenType::_eot:
            return out << "<eot>";
        default:
            throw std::invalid_argument{"parameter temp contains an invalid NcLexer::TokenType enumerator"};
        }
    }

}// namespace Nc
