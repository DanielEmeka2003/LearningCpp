#ifndef NCLEXER_H
#define NCLEXER_H

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>

#include "stream/stringoutputstream.h"

namespace Nc
{
    class NcLexer
    {
    public:
    enum class TokenType{reserved, identifier, literal, symbol, _miscellany, _eot};

    /*Literal*/

    static constexpr std::string_view lNumber{"numberL"};

    static constexpr std::string_view lInt32{"int32L"};
    static constexpr std::string_view lUint{"uint32L"};
    static constexpr std::string_view lBool{"boolL"};
    

    static constexpr std::string_view lChar{"charL"};
    static constexpr std::string_view lStr{"strL"};
    static constexpr std::string_view lFloat{"floatL"};

    /*Reserved*/

    static constexpr std::string_view rConst{"const"};          
    static constexpr std::string_view rInt32{"int32"};                                     
    static constexpr std::string_view rUint32{"uint32"};        
    static constexpr std::string_view rBool{"bool"};            
    static constexpr std::string_view rFloat32{"float32"};                        
    static constexpr std::string_view rVoid{"void"};             
    static constexpr std::string_view rMain{"main"};            
    static constexpr std::string_view rGive{"give"};           
    static constexpr std::string_view rReturn{"return"};           
    static constexpr std::string_view rTrue{"true"};            
    static constexpr std::string_view rFalse{"false"};          
    static constexpr std::string_view rIf{"if"};                
    static constexpr std::string_view rElse{"else"};            
    static constexpr std::string_view rElseif{"else_if"};
    static constexpr std::string_view rWhile{"while"};

    /*Symbol*/
    
    static constexpr std::string_view sArrow{"->"};
    static constexpr std::string_view sAssign{"="};
    static constexpr std::string_view sEqual{"=="};
    static constexpr std::string_view sLcurly{"{"};
    static constexpr std::string_view sRcurly{"}"};
    static constexpr std::string_view sLparen{"("};
    static constexpr std::string_view sRparen{")"};
    static constexpr std::string_view sSemicolon{";"};
    static constexpr std::string_view sLessthan{"<"};
    static constexpr std::string_view sGreaterthan{">"};
    static constexpr std::string_view sLessthan_equalto{"<="};
    static constexpr std::string_view sGreaterthan_equalto{">="};
    static constexpr std::string_view sPlus{"+"};
    static constexpr std::string_view sMinus{"-"};
    static constexpr std::string_view sMultiply{"*"};
    static constexpr std::string_view sDivide{"/"};
    static constexpr std::string_view sExponent{"^"};
    static constexpr std::string_view sModulus{"%"};
    static constexpr std::string_view sBitwiseComplement{"~"};
    static constexpr std::string_view sLogicalNegation{"!"};


    struct TokenData
    {
        std::string token{};
        std::uint32_t line{};
        std::uint32_t column{};
        bool isReserved{};
        bool isIdentifier{};
        std::string literalType{};
        bool isSymbol{};
    };
    
    private:
        static constexpr std::string_view signleQuotes{"'"};
        static constexpr std::string_view doubleQuotes{"\""};
        static constexpr std::string_view underscore{"_"};

        std::uint32_t m_line{1};
        std::uint32_t m_column{1};
        std::string m_fileBuffer{};
        std::uint32_t m_filePosition{};
        std::vector<TokenData> m_tokenList{};
        std::string m_tokenString{};
        Streams::StringOutputStream<char> m_error{std::ostringstream{}};

    public:
        NcLexer(const std::string& fileBuffer);
        ~NcLexer() = default;

        void lexFileBuffer();
        const std::vector<TokenData>& getTokenList() const;
        std::uint32_t getFilePosition() const;

    private:
        TokenType lexify();

        void moveFilePosAndColumn(std::size_t number = 1);

        TokenType tokenizeWords();
        TokenType reservedWords();

        TokenType tokenizeNumbers();
        TokenType hexadecimal();
        TokenType binary();
        TokenType octal();

        TokenType tokenizeCharLiterals();
        TokenType tokenizeStrLiterals();

        TokenType tokenizeSymbols();

        [[noreturn]] void errorOutput();
    };

    //overload version of the output stream for type NcLexer::TokenType
    std::ostream& operator<<(std::ostream& out, NcLexer::TokenType temp);

} // namespace Nc

#endif