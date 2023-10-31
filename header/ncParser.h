#ifndef NCPARSER_H
#define NCPARSER_H

#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "ncLexer.h"
#include "ncCodeGen.h"
#include "stream/stringoutputstream.h"

/*
<program>            ::= {<function( <identifier> )>} <function("main")>
<function(name)>     ::= <type> <identifier> "("  ")" <block>
<identifier>         ::= $any valid name$
<type>               ::= (int32|uint32|bool|char|string)
<block>              ::= "{" <statement> "}"
<statement>          ::= <give>
<give>               ::= ("return"|"give") <expression> ";"
<expression>         ::= <literals> | <unaryOperation>
<literals>           ::= int32L
<unaryOperation>      ::= (-|~|!) <expression>
*/

namespace Nc
{
    class NcParser
    {
    private:
        enum class Result: bool {sucess, failure};

        static constexpr std::string_view null_str{""};

        std::vector<NcLexer::TokenData> m_tokenList{};
        std::string_view m_safe_token{};
        std::size_t m_currentToken{};
        Streams::StringOutputStream<char> m_error{std::ostringstream{}};
        NcCodeGen m_codeGen{};

    public:
        NcParser(std::vector<NcLexer::TokenData> tokenList);
        ~NcParser() = default;

        void parseTokenList();

    private:
        void program();
        void function(std::string_view name = "");
        void type();
        void block();
        void statement();
        void give();
        void expression();
        Result literals();
        Result unaryOperation();

        Result expect(NcLexer::TokenType tokenTypeToMatch, std::vector<std::string_view> tokenStringsToMatch, std::string_view optErrorMsg = "");
        void next();

        [[noreturn]] void errorOutput();
    };
    
} // namespace Nc



#endif