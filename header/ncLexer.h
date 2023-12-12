#ifndef NCLEXER_H
#define NCLEXER_H

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>

#include "stream/stringoutputstream.h"
#include "ncLog.h"

namespace Nc
{
    class NcLexer
    {
    public:
    enum class TokenType{reserved, identifier, literal, symbol, _miscellany, _eot};

    /*Literal*/

    static constexpr std::string_view lInt8{"int8L"};
    static constexpr std::string_view lInt16{"int16L"};
    static constexpr std::string_view lInt32{"int32L"};
    static constexpr std::string_view lInt64{"int64L"};
    static constexpr std::string_view lUint8{"uint8L"};
    static constexpr std::string_view lUint16{"uint16L"};
    static constexpr std::string_view lUint32{"uint32L"};
    static constexpr std::string_view lUint64{"uint64L"};
    static constexpr std::string_view lBool{"boolL"};
    

    static constexpr std::string_view lChar{"charL"};
    static constexpr std::string_view lStr{"strL"};
    static constexpr std::string_view lFloat32{"float32L"};
    static constexpr std::string_view lFloat64{"float64L"};

    /*Reserved*/

    static constexpr std::string_view rConst{"const"};
    static constexpr std::string_view rInt8{"int8"};          
    static constexpr std::string_view rInt16{"int16"};          
    static constexpr std::string_view rInt32{"int32"};          
    static constexpr std::string_view rInt64{"int64"};          
    static constexpr std::string_view rUint8{"uint8"};
    static constexpr std::string_view rUint16{"uint16"};
    static constexpr std::string_view rUint32{"uint32"};
    static constexpr std::string_view rUint64{"uint64"};
    static constexpr std::string_view rBool{"bool"};
    static constexpr std::string_view rFloat32{"float32"};
    static constexpr std::string_view rFloat64{"float64"};
    static constexpr std::string_view rVoid{"void"};
    static constexpr std::string_view rMain{"main"};
    static constexpr std::string_view rGive{"give"};
    static constexpr std::string_view rTrue{"true"};
    static constexpr std::string_view rFalse{"false"};
    static constexpr std::string_view rIf{"if"};    
    static constexpr std::string_view rElse{"else"};
    static constexpr std::string_view rWhile{"while"};
    static constexpr std::string_view rAnd{"and"};
    static constexpr std::string_view rOr{"or"};
    static constexpr std::string_view rNot{"not"};
    static constexpr std::string_view rXor{"xor"};
    static constexpr std::string_view rBitwiseAnd{"bitand"};
    static constexpr std::string_view rBitwiseOr{"bitor"};
    static constexpr std::string_view rBitwiseNot{"bitnot"};
    static constexpr std::string_view rBitwiseXor{"bitxor"};
    static constexpr std::string_view rOdd{"odd"};
    static constexpr std::string_view rWrite{"write"};
    static constexpr std::string_view rRead{"read"};

    /*Symbol*/
    
    static constexpr std::string_view sArrow{"->"};
    static constexpr std::string_view sAssign{"="};
    static constexpr std::string_view sEqual{"=="};
    static constexpr std::string_view sNotEqual{"!="};
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
    static constexpr std::string_view sModuluo{"%"};
    static constexpr std::string_view sAnd{"&&"};
    static constexpr std::string_view sOr{"||"};
    static constexpr std::string_view sNot{"!"};
    static constexpr std::string_view sXor{"^^"};
    static constexpr std::string_view sBitwiseAnd{"&"};
    static constexpr std::string_view sBitwiseOr{"|"};
    static constexpr std::string_view sBitwiseNot{"~"};
    static constexpr std::string_view sBitwiseXor{"^"};
    static constexpr std::string_view sBitwiseShiftLeft{"<<"};
    static constexpr std::string_view sBitwiseShiftRight{">>"};
    static constexpr std::string_view sComma{","};
    static constexpr std::string_view sAdditionAssign{"+="};
    static constexpr std::string_view sSubtractionAssign{"-="};
    static constexpr std::string_view sMultiplicationAssign{"*="};
    static constexpr std::string_view sDivisionAssign{"/="};
    static constexpr std::string_view sModuluoAssign{"%="};
    static constexpr std::string_view sBitwiseShiftLeftAssign{"<<="};
    static constexpr std::string_view sBitwiseShiftRightAssign{">>="};
    static constexpr std::string_view sBitwiseAndAssign{"&="};
    static constexpr std::string_view sBitwiseOrAssign{"|="};
    static constexpr std::string_view sBitwiseXorAssign{"^="};
    static constexpr std::string_view sIncrement{"++"};
    static constexpr std::string_view sDecrement{"--"};
    static constexpr std::string_view sColon{":"};
    static constexpr std::string_view sQuestionMark{"?"};
    

    struct TokenData
    {
        std::string token{};
        std::uint32_t line{};
        std::uint32_t absoluteColumn{};
        std::uint32_t relativeColumn{};
        bool isReserved{};
        bool isIdentifier{};
        std::string literalType{};
        bool isSymbol{};
    };
    
    private:
        static constexpr std::string_view signleQuotes{"'"};
        static constexpr std::string_view doubleQuotes{"\""};
        static constexpr std::string_view underscore{"_"};
        static constexpr std::string_view hashtag{"#"};

        std::uint32_t m_line{1};
        std::uint32_t m_absoluteColumn{1};
        std::uint32_t m_relativeColumn{};
        std::string m_fileBuffer{};
        std::uint32_t m_filePosition{};
        std::vector<TokenData> m_tokenList{};
        std::string m_tokenString{};
        NcLog m_log{};

    public:
        NcLexer(std::string_view fileBuffer);
        NcLexer(std::string&& fileBuffer);
        ~NcLexer() = default;

        void lexFileBuffer();
        const std::vector<TokenData>& getTokenList() const;
        std::vector<TokenData>& getTokenList();
        std::uint32_t getFilePosition() const;

        void logBoxStyle(NcLog::BoxLineStyle s);

    private:
        TokenType lexify();

        void moveFilePosAndColumn(std::uint32_t number = 1);
        void recordRelativeColumn();

        TokenType tokenizeWords();
        TokenType reservedWords();
        
        TokenType validateNumberL(std::string& literalType);
        TokenType tokenizeNumbers();
        TokenType hexadecimal();
        TokenType binary();
        TokenType octal();

        TokenType tokenizeCharLiterals();
        TokenType tokenizeStrLiterals();

        TokenType tokenizeSymbols();

        void finalizeLog();

        [[noreturn]] void errorOutput();
    };

    //overload version of the output stream for type NcLexer::TokenType
    std::ostream& operator<<(std::ostream& out, NcLexer::TokenType temp);

} // namespace Nc

#endif