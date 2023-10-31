#include <cctype>
#include <algorithm>
#include <iterator>
#include <bitset>

#include "ncLexer.h"
#include "drawboxaroundtext.h"
#include "stream/output.h"
#include "stream/stringinputstream.h"

namespace Nc
{
    NcLexer::NcLexer(const std::string& fileBuffer): m_fileBuffer{fileBuffer}
    {}

    void NcLexer::lexFileBuffer()
    {
        while (lexify() != TokenType::_eot)
        {}
    }
    const std::vector<NcLexer::TokenData>& NcLexer::getTokenList() const
    { return m_tokenList; }
    std::uint32_t NcLexer::getFilePosition() const
    { return m_filePosition; }

    NcLexer::TokenType NcLexer::lexify()
    {
        try //main try block for exiting lexing when eof is reached
        {
            while (std::isspace(m_fileBuffer.at(m_filePosition)))
            {
                if (m_fileBuffer.at(m_filePosition) == '\n')
                {
                    ++m_line;
                    m_column = {};
                }
                moveFilePosAndColumn();
            }
            
            if (std::isalpha(m_fileBuffer.at(m_filePosition)) or m_fileBuffer.at(m_filePosition) == *underscore.begin())
            return tokenizeWords();
            if (m_fileBuffer.at(m_filePosition) == *signleQuotes.begin())
            return tokenizeCharLiterals();
            if (m_fileBuffer.at(m_filePosition) == *doubleQuotes.begin())
            return tokenizeStrLiterals();
            if (std::isdigit(m_fileBuffer.at(m_filePosition)))
            return tokenizeNumbers();

            if (auto result{tokenizeSymbols()}; result == TokenType::_miscellany)
            {
                m_error.empty_str();
                m_error.write("[TOKEN ERROR] unrecognized token [", m_tokenString, "]");
                errorOutput();
            }
            else
            return result;
        }
        catch(const std::exception&){}
        
        return TokenType::_eot;
    }

    void NcLexer::moveFilePosAndColumn(std::size_t number)
    { m_filePosition += number, m_column += number; }

    NcLexer::TokenType NcLexer::tokenizeWords()
    {
        //empty the error string incase and error occurs, to avoid overwriting
        m_error.empty_str();
        m_tokenString.clear();

        auto formerFilePos{m_filePosition};

        try
        {
            while (std::isalnum(m_fileBuffer.at(m_filePosition)) or m_fileBuffer.at(m_filePosition) == *underscore.begin())
            { moveFilePosAndColumn(); }
        }
        catch(const std::exception&){}

        //extract the token to m_tokenString
        for (size_t i = formerFilePos; i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer.at(i));
        
        if (TokenType result{reservedWords()}; result == TokenType::reserved)
        return result;

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{true}
        });

        return TokenType::identifier;
    }
    NcLexer::TokenType NcLexer::reservedWords()
    {
        if
        (
            m_tokenString == rConst or m_tokenString == rInt32 or m_tokenString == rUint32 or m_tokenString == rBool or
            m_tokenString == rFloat32 or m_tokenString == rVoid or m_tokenString == rMain or m_tokenString == rGive or
            m_tokenString == rReturn or m_tokenString == rTrue or m_tokenString == rFalse or m_tokenString == rIf or
            m_tokenString == rElse or m_tokenString == rElseif or m_tokenString == rWhile
        )
        {
            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{true}
            });
            
            return TokenType::reserved; //this does not make sense, but i am too lazy, so...
        }
        return TokenType::_miscellany;
    }

    NcLexer::TokenType NcLexer::tokenizeNumbers()
    {
        //empty the error string incase and error occurs, to avoid overwriting
        m_error.empty_str();
        m_tokenString.clear();

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
            m_error.write("[TOKEN ERROR] token ['] should not end a number literal");
            errorOutput();
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //check if it has a radix seperator
        if (m_tokenString.contains('.'))
        {
            //if the radix seperator is not one, then terminate lexing, because that is not a floating point number
            if (std::ranges::count(m_tokenString, '.') != 1)
            {
                m_error.write("[TOKEN ERROR] multiple instances of token [.] found in floating point literal");
                errorOutput();
            }

            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lFloat.data()} 
            });
        }
        else //we are now sure the number token is an integral
        {
            //add token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lNumber.data()} 
            });
        }
        
        return TokenType::literal;
    }
    NcLexer::TokenType NcLexer::hexadecimal()
    {
        auto formerFilePos{m_filePosition};

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
            m_error.write("[TOKEN ERROR] token ['] should not end a number literal");
            errorOutput();
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //validate the number
        //check if it has a radix seperator terminate lexing, because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_error.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            errorOutput();
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
            m_error.write("[TOKEN ERROR] hexadecimal integral is invalid: [", m_tokenString, "]");
            errorOutput();
        }
        //now we know the hexadecimal is valid, so we just convert variable to a string to store it as a decimal
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lNumber.data()} 
        });

        return TokenType::literal;
    }
    NcLexer::TokenType NcLexer::binary()
    {
        auto formerFilePos{m_filePosition};

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
            m_error.write("[TOKEN ERROR] token ['] should not end a number literal");
            errorOutput();
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //check if it has a radix seperator and terminate lexing because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_error.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            errorOutput();
        }
        
        if (!std::ranges::all_of( m_tokenString, [](char x){return x == '0' or x == '1';} ) )
        {
            m_error.write("[TOKEN ERROR] invalid binary literal [", m_tokenString, "]");
            errorOutput();
        }

        std::uint64_t variable{};
        try
        {
            variable = std::bitset<100>{m_tokenString}.to_ullong();
        }
        catch(const std::exception&)
        {
            m_error.write("[TOKEN ERROR] binary literal is invalid [", m_tokenString, "]");
            errorOutput();
        }
        //now we are sure that the binary is valid, so we get the decimal form of it and convert to a string
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lNumber.data()} 
        });

        return TokenType::literal;
    }
    NcLexer::TokenType NcLexer::octal()
    {
        auto formerFilePos{m_filePosition};

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
            m_error.write("[TOKEN ERROR] token ['] should not end a number literal");
            errorOutput();
        }
        
        // remove the number seperator
        std::ranges::replace(m_tokenString, '\'', '\0');

        //validate the number
        //check if it has a radix seperator terminate lexing, because only decimal literals can have floating point
        if (m_tokenString.contains('.'))
        {
            m_error.write("[TOKEN ERROR] only decimals support floating point [", m_tokenString, "]");
            errorOutput();
        }
        //check if the digits 8 or 9 are in the string and if true terminate lexing
        if (m_tokenString.contains('8') or m_tokenString.contains('9'))
        {
            m_error.write("[TOKEN ERROR] invalid octal literal [", m_tokenString, "]");
            errorOutput();
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
            m_error.write("[TOKEN ERROR] ocatl literal is invalid: [", m_tokenString, "]");
            errorOutput();
        }
        //now we know the octal is valid, so we just convert variable to a string to store it as a decimal
        m_tokenString = std::to_string(variable);

        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lNumber.data()} 
        });

        return TokenType::literal;
    }

    NcLexer::TokenType NcLexer::tokenizeCharLiterals()
    {
        //empty the error string incase and error occurs, to avoid overwriting
        m_error.empty_str();
        m_tokenString.clear();

        auto formerFilePos{m_filePosition};

        try
        {
            moveFilePosAndColumn(); //skip past the single quote that was found
            while (m_fileBuffer.at(m_filePosition) != *signleQuotes.begin())
            {
                if (m_fileBuffer.at(m_filePosition) == '\\')//if the current character is a backslash move past it
                { moveFilePosAndColumn(); }
                
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
                    m_error.write("[TOKEN ERROR] character literal [", m_tokenString, "] is invalid");
                    errorOutput();
                }
            }
            else
            {
                if (m_tokenString.length() != 3)
                {
                    m_error.write("[TOKEN ERROR] character literal [", m_tokenString, "] is invalid");
                    errorOutput();
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
                    m_error.write("[TOKEN ERROR] token [", m_tokenString.at(result + 1), "] is not an escape sequence");
                    errorOutput();
                    break;
                }
            }
            
            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lChar.data()} 
            });
        }
        catch(const std::exception&)
        {
            m_error.write("[TOKEN ERROR] unterminated character literal");
            errorOutput();
        }

        return TokenType::literal;
    }
    NcLexer::TokenType NcLexer::tokenizeStrLiterals()
    {
        //empty the error string incase and error occurs, to avoid overwriting
        m_error.empty_str();
        m_tokenString.clear();

        auto formerFilePos{m_filePosition};

        try
        {
            moveFilePosAndColumn(); //skip past the double quote that was found
            while (m_fileBuffer.at(m_filePosition) != *doubleQuotes.begin())
            {
                if (m_fileBuffer.at(m_filePosition) == '\\')//if the current character is a backslash move past it
                { moveFilePosAndColumn(); }

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
                    m_error.write("[TOKEN ERROR] token [", m_tokenString.at(result + 1), "] is not an escape sequence");
                    errorOutput();
                    break;
                }
            }
            

            //add the token to the list
            m_tokenList.push_back(TokenData{
                .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{lStr.data()} 
            });
        }
        catch(const std::exception&)
        {
            m_error.write("[TOKEN ERROR] unterminated string literal");
            errorOutput();
        }

        return TokenType::literal;
    }

    NcLexer::TokenType NcLexer::tokenizeSymbols()
    {
        //empty the error string incase and error occurs, to avoid overwriting
        m_error.empty_str();
        m_tokenString.clear();

        switch (m_fileBuffer.at(m_filePosition))
        {
        case *sMinus.begin():
            try
            {
                if (auto temp{m_filePosition + 1}; m_fileBuffer.at(temp) == *sGreaterthan.begin())
                {
                    m_tokenString.assign(sArrow.data());
                    moveFilePosAndColumn(2); //move the file position ahead of the arrow symbol
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
        case *sEqual.begin():
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
        case *sPlus.begin():
        case *sMultiply.begin():
        case *sDivide.begin():
        case *sModulus.begin():
        case *sLcurly.begin():
        case *sRcurly.begin():
        case *sLparen.begin():
        case *sRparen.begin():
        case *sExponent.begin():
        case *sSemicolon.begin():
        case *sBitwiseComplement.begin():
        case *sLogicalNegation.begin():
            {
                m_tokenString.push_back(m_fileBuffer.at(m_filePosition));
                moveFilePosAndColumn(); //move the file position past the symbol
            }
            break;
        default:
            //keep the unrecongizned token for error purposes
            m_tokenString.push_back(m_fileBuffer.at(m_filePosition));
            return TokenType::_miscellany;
        }
        
        //add the token to the list
        m_tokenList.push_back(TokenData{
            .token{m_tokenString}, .line{m_line}, .column{m_column}, .isReserved{}, .isIdentifier{}, .literalType{}, .isSymbol{true} 
        });

        return TokenType::symbol;
    }


    [[noreturn]] void NcLexer::errorOutput()
    {
        Myfcn::DrawBoxAroundText boxify{System::coutput};
        
        const auto maxBoxSize{ std::max(m_error.get_str().length(), 40U) };

        boxify.style(Myfcn::DrawBoxAroundText::Style::doublelined);

        boxify.topLine(maxBoxSize);
        boxify.spacing(m_error.get_str(), maxBoxSize);
        m_error.empty_str();
        m_error.writews("            line:", m_line, "column:", m_column);
        boxify.spacing(m_error.get_str(), maxBoxSize);
        m_error.insert_str("{compilation terminated with an error}");
        boxify.spacing(m_error.get_str(), maxBoxSize);
        boxify.downLine(maxBoxSize);

        m_line = {};
        m_column = {};
        m_fileBuffer.~basic_string();
        m_filePosition = {};
        m_tokenList.~vector();
        m_tokenString.~basic_string();
        m_error.~StringOutputStream();
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

} // namespace Nc
