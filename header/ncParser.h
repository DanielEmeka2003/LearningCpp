#ifndef NCPARSER_H
#define NCPARSER_H

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "ncLexer.h"
#include "ncCodeGen.h"
#include "ncAST.h"
#include "stream/stringoutputstream.h"

/*
*<ncfile>             ::= <main>
*<main>               ::= "int32" "main" "("  ")" <block> | "main" "("  ")" ":" "int32" <block>
*<identifier>         ::= $any valid name$
*<type>               ::= (int32|uint32|bool|char|string) [ ":" "(" <typeInstruction> ")" ]
*<block>              ::= "{" { <block_item> } "}"
*<block_item>         ::= <declaration> | <statement>
*<statement>          ::= <give> [";"] | <expression> [";"] | <if_condition>
*<if_condition>       ::= "if" "(" <expression> ")" <statement> ["else" <statement> ]
*<give>               ::= ("return"|"give") <expression>
*<declaration>        ::= "int32" <identifier> [ "=" <assignment_exp> ] { "," <identifier> [ "=" <assignment_exp> ] } [";"]
*****************************************************************************************************************************
*<expression>         ::= <assignment_exp> { (",") <assignment_exp> }
*<assignment_exp>     ::= <conditional_exp> [ ("="|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=") <assignment_exp> ]
*<conditional_exp>    ::= <logicalOr_exp> [ "?" <expression> ":" <expression> ]
*<logicalOr_exp>      ::= <logicalXor_exp> { ("||"|"or") <logicalXor_exp> }
*<logicalXor_exp>     ::= <logicalAnd_exp> { ("^^"|"xor") <logicalAnd_exp> } 
*<logicalAnd_exp>     ::= <bitwiseOr_exp> { ("&&"|"and") <bitwiseOr_exp> }
*<bitwiseOr_exp>      ::= <bitwiseXor_exp> { ("|"|"bitor") <bitwiseXor_exp> }
*<bitwiseXor_exp>     ::= <bitwiseAnd_exp> { ("^"|"bitxor") <bitwiseAnd_exp> }
*<bitwiseAnd_exp>     ::= <equality_exp> { ("&"|"bitand") <equality_exp> }
*<equality_exp>       ::= <relational_exp> { ("=="|"!=") <relational_exp> }
*<relational_exp>     ::= <bitwiseShift_exp> { ("<"|">"|"<="|">=") <bitwiseShift_exp> }
*<bitwiseShift_exp>   ::= <additive_exp> { ("<<"|">>") <additive_exp> }
*<additive_exp>       ::= <multiplicative_exp> { ("+"|"-") <multiplicative_exp> }
*<multiplicative_exp> ::= <cast_exp> { ("*"|"/"|"%") <cast_exp> }
*<cast_exp>           ::= <prefix_exp> { "->" <type> }
*<prefix_exp>         ::= ("-"|"~"|"!"|"+"|"odd"|"++"|"--") <prefix_exp> | <postfix_exp>
*<postfix_exp>        ::= <primary_exp> { ("++"|"--") }
*<primary_exp>        ::= "(" <expression> ")" | <identifier> | int32L
*****************************************************************************************************************************

[NOTES]: for expressions, precedence is achieved by the order in which operators of different predence call each other,
operators with lower precedence call the operators with higher precedence, ensuring that those ones execute first.
While associativity is achieved by either the operator calling itself(or an operator of lower precedence than itself) 
when a condition is met .i.e. (R->L) or the operator calling another operator of higher precedence than itself - .i.e. (L->R).

Also, pushing the values of the first operand to the stack is needed to save intermediate value results
between switching from a lower precedence operation to a higher precedence operation.
*/


namespace Nc
{
    class NcParser
    {
    private:
        class ValueCategory
        {
        public:
            enum Value: short {lvalue, rvalue};

        private:
            Value m_value{};

        public:
            void setAsLvalue();
            void setAsRvalue();

            bool isLvalue();
            bool isRvalue();

            friend std::ostream& operator<<(std::ostream& out, ValueCategory v)
            { 
                switch (v.m_value)
                {
                case v.lvalue:
                    out << "lvalue";
                    break;
                case v.rvalue:
                    out << "rvalue";
                    break;
                default:
                    throw std::invalid_argument
                    {"in friend function operator<<(std::ostream&, ValueCategory), v is invalid!"};
                    break;
                }
                return out;
            }
        };
        
        enum class Result: bool {success, failure};

        using StackIndex = int;
        using Location = int;
        using VariableIdentifier = std::string;

        static constexpr std::string_view null_str{""};


        std::vector<NcLexer::TokenData> m_tokenList{};
        std::size_t m_currentToken{};
        std::string_view m_safe_token{};

        NcLog m_log{};
        
        std::unordered_map<VariableIdentifier, Location> m_variableMap{};
        StackIndex m_stackIndex{};
        ValueCategory m_valueCategory{};
        NcAST m_ast{};

    public:
        NcParser(const std::vector<NcLexer::TokenData>& tokenList);
        ~NcParser() = default;

        void parseTokenList();

        void logBoxStyle(NcLog::BoxLineStyle s);

        NcAST& getAST();
        const NcAST& getAST()const;

        

    private:
        void ncfile();
        void main();

        void block();
        Result block_item(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result declaration(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result statement(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result if_condition(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result give(bool shouldReport = false, std::string_view optErrorMsg = null_str);

        Result expression(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result conditional_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result assignment_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result logicalOr_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result logicalXor_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result logicalAnd_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result bitwiseOr_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result bitwiseXor_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result bitwiseAnd_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result equality_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result relational_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result bitwiseShift_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result additive_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result multiplicative_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result cast_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result prefix_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result postfix_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);
        Result primary_exp(bool shouldReport = false, std::string_view optErrorMsg = null_str);

        Result expect
        (NcLexer::TokenType tokenType, std::vector<std::string_view> tokenStr,
        bool shouldReport = false, std::string_view optErrorMsg = null_str);

        void next();

        void finalizeLog();

        [[noreturn]] void errorOutput();
    };
    
} // namespace Nc


#endif