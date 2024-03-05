#ifndef NCPARSER_H
#define NCPARSER_H

#pragma once

#include "ncLexer.h"
#include "ncAst.h"
#include "ncSemantics.h"
#include "stream/stringoutputstream.h"

/*
*<ncfile>              ::= {<ncfile_contents>}
*<ncfile_contents>     ::= <function_decl> | <globalVar_decl> [";"] | <sumType_decl> | <productType_decl> | <enum_decl> | <type_alias> [";"] | <namespace_decl>
*<namespace_decl>      ::= "namespace" <namespace_ident> "{" {<ncfile_contents>} "};"
*<namespace_ident>     ::= <identifier> {"::" <identifier>}
*<sumType_decl>        ::= "sumType" <identifier> ("{" [<type> {"," <type>}] "};") | ";"
*<productType_decl>    ::= "productType" <identifier> ("{" [<type> <identifier> {"," <type> <identifier>}] "};") | ";"
*<enum_decl>           ::= "enum" <identifier> [":" <type>] ("{" <identifier> [<initializer] {"," <identifier> [<initializer]}) | ";"
*<type_alias>          ::= "type_alias" <identifier> ":=" <type>
*<namespace_alias>     ::= "namespace_alias" <identifier> ":=" <namespace_ident>
*<fundamental_types>   ::= "int8"|"int16"|"int32"|"int64"|"int128"|"int256"|"int512"|"int1024"|"uint8"|"uint16"|"uint32"|"uint64"|"uint128"|"uint256"|"uint512"|"uint1024"|"bool"|"achar"|"pchar"|"u8char"|"astring"|"pstring"|"u8string"|"bin16fp"|"bin32fp"|"binfp64"|"bin128fp"|"dec32fp"|"dec64fp"|"dec128fp"|"dec256fp"|"dec512fp"|"voidType"|"nullptrType"|"noneType"|"initListType"|"ncType"|"typeinfo"
*<userDefined_types>   ::= <special_ident:"sumType"|"productType">
*<type_name>           ::= <userDefined_types>|<fundamental_types>
*<function_type>       ::= "!(" "(" <type> {"," <type>} ")" <type> ")"
*<type>                ::=  (<type_name>|<function_type>|"guess") [":"] [ "(" [<type_arrtibutes>] ")" ]
*<type_arrtibutes>     ::= c for const, p for pointer, l for lvalue-reference, r for rvalue-reference
*<function_decl>       ::= <identifier> "(" [<parameter_list>] ")" <type> <block>|";"
*<parameter_list>      ::= <type> [<identifier> [<initializer>]] { "," <type> [<identifier> [<initializer>]] }
*<globalVar_decl>      ::= <type> <identifier> [<initializier>] { "," <identifier> [<initializer>] }
*<initializier>        ::= (":=" <assignment_exp>) | <anyTypeList_exp>
*<identifier>          ::= $ any valid name $
*<block>               ::= "{" {<block_item>} "}"
*<block_item>          ::= <localVar_decl> [";"] | <statement>
*<statement>           ::= <jump_statement> [";"] | <if_condition> | <for_loop> | <for_loop_variant> | <while_loop> | <do_while_loop> | <expression> [";"]
*<if_condition>        ::= "if" "(" <expression> ")" <statement> ["else" <statement>]
*<for_loop>            ::= "for" "(" [<localVar_decl> ";"] <expression> ";" <expression> ";" ")" <statement>
*for_loop_variant>     ::= "for" <identifier_list>|"none" ":" "(" "default"|<assignment_exp> ["," "default"|<relational_op>|<assignment_exp>] ")" <statement>
*<identifier_list>     ::= <identifier> {"," <identifier>}
*<while_loop>          ::= "while" "(" <expression> ")" <statement>
*<do_while_loop>       ::= "do" <statement> "while" "(" <expression> ")"
*<jump_statement>      ::= "give" [<expression>] | "break" | "continue"
*<localVar_decl>       ::= <type> <identifier> [<intiializier>] { "," <identifier> [<initializer>] }
******************************************************************************************************************************************************************************
*<expression>          ::= <assignment_exp> { (",") <assignment_exp> }
*<assignment_exp>      ::= <conditional_exp> [ (":="|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=") <assignment_exp> ]
*<conditional_exp>     ::= <logicalOr_exp> [ "?" <expression> ":" <expression> ] 
*<logical_exp>         ::= <bitwise_exp> { ("&&"|"and"|"||"|"or"|"xor"|"^^") <bitwise_exp> }
*<bitwise_exp>         ::= <equality_exp> { ("&"|"bitand"|"|"|"bitor"|"^"|"bitxor") <equality_exp> }
*<equality_exp>        ::= <relational_exp> { ("="|"!="|"≠") <relational_exp> }
*<relational_exp>      ::= <shift_exp> { ("<"|">"|"<="|">="|"≤"|"≥") <bitwiseShift_exp> }
*<shift_exp>           ::= <additive_exp> { ("<<"|">>") <additive_exp> }
*<additive_exp>        ::= <multiplicative_exp> { ("+"|"-") <multiplicative_exp> }
*<multiplicative_exp>  ::= <exponention_exp> { ("*"|"/"|"%") <exponention_exp> }
*<exponention_exp>     ::= <member_exp> { "∘" <cast_exp> }
*<member_exp>          ::= <prefix_exp> { ("."|"->") <prefix_exp> }
*<prefix_exp>          ::= ("-"|"~"|"!"|"+"|"odd"|"++"|"--"|"@"|"mem") <prefix_exp> | <postfix_exp>
*<postfix_exp>         ::= <primary_exp> {"++"|"--"|"(" [<expression_list>] ")"}
*<primary_exp>         ::= "(" <expression> ")"| <l_and_r_exp> | <block> | <special_ident:"input"|"output"|"typecast"|"bitcast"> | <anyTypeList_exp> | <tempVarDecl> | <literal>
*<special_ident>       ::= (<identifier>|%<reserved>%)["<" [<type_and_exp_list>] ">"] {"::" <identifier>["<" [<type_and_exp_list>] ">"]}
*<tempVarDecl>         ::= <type> <anyTypeList_exp>
*<anyTypeList_exp>     ::= "!{" [<expression_list>] "}"
*<expression_list>     ::= "default"|<assignment_exp> {"," "default"|<assignment_exp>}
*<type_and_exp_list>   ::= "default"|<type>|<primary_exp> {"," "default"|<type>|<primary_exp>}
*<r_and_l_exp>         ::= "#(" <relational_op> "," <logical_op> "," <assignment_exp> ")" "(" <expression_list> ")"
*<relational_op>       ::= "="|"!="|"≠"|"<"|">"|"<="|">="|"≤"|"≥" $defined of convience$
*<logical_op>          ::= "&&"|"||"|"^^"|"and"|"or"|"xor" $defined of convience$
******************************************************************************************************************************************************************************
$ mimics comments $
% means it accepts input %

[function type]: (int32:(C) (int32)): (r) b{}; ✕
[function type]: !((int32)int32:(C)): (r) b{}; ✓
#(=, and, b)
[Ubiquitous scope stays! No function or gloabl variable declaration]: does not stay, lexical scope is the boss

During the parsing stage, in all blocks(local or global), not expected tokens are not skipped, rather they remain the current tokens till when they
successfully parsed by another unrelated entity or the token list gotten from the lexer reaches it's end.

[NOTES]: for expressions, precedence is achieved by the order in which operators of different predence call each other,
operators with lower precedence call the operators with higher precedence, ensuring that those ones execute first.
While associativity is achieved by either the operator calling itself( or an operator of lower precedence than itself)
when a condition is met .i.e. (R->L) or the operator calling another operator of higher precedence than itself .i.e. (L->R).
*/

namespace Nc
{
    class Parser
    {
    private:
        enum class Result: short {success, failure};

        struct FailureCase
        {
            bool shouldReport{};
            std::optional<std::string_view> optErrorMsg{};
            std::string_view extraErrMsg{};
        };

        std::vector<Lexer::TokenData> m_tokenDataList{};
        Lexer::LineNumerToFilePositionMap m_lineNumToFilePos{};
        Myfcn::U8string m_fileBuffer{};
        std::size_t m_currentTokenDataIndex{};
        Lexer::TokenData m_currentTokenData{};
        Log m_log{};
        Ast m_ast{};
        std::vector<Myfcn::U8string> m_customTypesList{};
        Myfcn::U8string m_currentNameSpace{"global::"_u8str};
        bool m_isEndofTokenList{};
        bool m_hasType{};

    public:
        Parser(Lexer& lex);
        ~Parser() = default;

        void parseTokenList();

        void logBoxStyle(Log::BoxLineStyle s);

        Ast& getAST();
        const Ast& getAST()const;

    private:
        void ncfile();
        Result ncfile_contents(const FailureCase& fc = FailureCase{false, {}, {}});
        Result namespace_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result namespace_ident(const FailureCase& fc = FailureCase{false, {}, {}});
        Result sumType_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result productType_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result enum_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result type_alias(const FailureCase& fc = FailureCase{false, {}, {}});
        Result type_name(const FailureCase& fc = FailureCase{false, {}, {}});
        Result function_type(const FailureCase& fc = FailureCase{false, {}, {}});
        Result type(const FailureCase& fc = FailureCase{false, {}, {}});
        Result function_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result parameter_list(const FailureCase& fc = FailureCase{false, {}, {}});
        Result globalVar_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result initializer(const FailureCase& fc = FailureCase{false, {}, {}});
      

        Result block(const FailureCase& fc = FailureCase{false, {}, {}});
        Result block_item(const FailureCase& fc = FailureCase{false, {}, {}});
        Result localVar_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result statement(const FailureCase& fc = FailureCase{false, {}, {}});
        Result if_condition(const FailureCase& fc = FailureCase{false, {}, {}});
        Result for_loop(const FailureCase& fc = FailureCase{false, {}, {}});
        Result for_loop_variant(const FailureCase& fc = FailureCase{false, {}, {}});
        Result identifier_list(const FailureCase& fc = FailureCase{false, {}, {}});
        Result while_loop(const FailureCase& fc = FailureCase{false, {}, {}});
        Result do_while_loop(const FailureCase& fc = FailureCase{false, {}, {}});
        Result jump_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        

        Result expression(const FailureCase& fc = FailureCase{false, {}, {}});
        Result conditional_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result assignment_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result logical_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result bitwise_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result equality_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result relational_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result shift_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result additive_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result multiplicative_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result exponention_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result member_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result prefix_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result postfix_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result primary_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result special_ident(std::vector<Myfcn::U8string_view> reservedInputs){}
        Result tempVarDecl(const FailureCase& fc = FailureCase{false, {}, {}});
        Result anyTypeList_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        Result expression_list(const FailureCase& fc = FailureCase{false, {}, {}});
        Result type_and_exp_list(const FailureCase& fc = FailureCase{false, {}, {}});
        Result r_and_l_exp(const FailureCase& fc = FailureCase{false, {}, {}});

        std::string_view mapTokenToCategory(std::optional<Lexer::TokenData*> optional_td = {});

        Result expect(Lexer::TokenType tokenType, std::vector<Myfcn::U8string_view> tokenStr, const FailureCase& fc = FailureCase{false, {}, {}});

        void next(std::size_t amount = 1);
        void previous(std::size_t amount = 1);

        void start_Log(bool isAdditionalLog = false);
        void start_MultiLineLog(std::size_t tokenDataIndexStart);
        void sensibleLog();
        void missingBraceLog(std::size_t savedTokenBraceDataIndex);
        void spaceLog();
        void additionalLog();

        LocationFinder makeLocFinder(std::uint32_t lineNumber0, std::uint32_t relativeColumn0, std::uint32_t absoluteColumn1);

        [[noreturn]] void errorOutput();
    };
    
} // namespace Nc

#endif