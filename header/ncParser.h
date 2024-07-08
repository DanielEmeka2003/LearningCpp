#ifndef NCPARSER_H
#define NCPARSER_H

#pragma once

#include "ncLexer.h"
// #include "ncAst.h"
#include "ncSemantics.h"
#include "stream/stringoutputstream.h"
#include "variant"

/*
******************************************************************************************************************************************************************************************************************************
<language>               ::= {<language_item>}
<language_item>          ::= <function_decl>|<object_decl>[';']|<named_st_decl>|<named_pt_decl>|<enum_decl>|<alias>[';']|<namespace_decl>
<name_space_decl>        ::= 'name_space' <special_ident_list> ('{' {<language_item>} '}')|[';']
<type_space_decl>        ::= 'type_space' <special_ident_list> ('{' {<language_item>} '}')|[';']
<named_st_decl>          ::= 'named_st' <special_ident_list> ('{' [<optPrefix_type> {',' <optPrefix_type>}] '}')|[';']
<named_pt_decl>          ::= 'named_pt' <special_ident_list> ('{' [<object_decl: <optPrefix_type>> {',' <object_decl: <optPrefix_type>>}] '}')|[';']
<enum_decl>              ::= 'enum' <special_ident_list> [':' <optPrefix_type>] ('{' <identifier>[<initializer] {',' <identifier>[<initializer]} '}')|[';']
<alias_decl>             ::= 'alias' <identifier> '=' ['namespace']<special_ident>|<prefix_type>
<function_decl>          ::= 'fn' <special_ident_list> '('[<func_param_list>]')' [<optPrefix_type>] <block>|[';']
******************************************************************************************************************************************************************************************************************************
<function_signature>     ::= 'fn' '('[<optPrefix_type> {',' <optPrefix_type>}]')' <optPrefix_type>
<typeof>                 ::= 'typeof' '('<expression>')'
<optPrefix_type>         ::= ( (['type'] <special_ident>|<function_signature>)|'guess' [':' <string_literal>] )|<typeof>
<prefix_type>            ::= ( ('type' <special_ident>|<function_signature>)|'guess' [':' <string_literal>] )|<typeof>
<func_param_list>        ::= <object_decl>|<me_func_param> {',' <object_decl>|<me_func_param>}
<me_func_param>          ::= ['imut']['&']'me'
<initializer>            ::= '::='<assignment_exp>|'('[<expArg_list>]')'
<special_ident_list>     ::= <special_ident> {',' <special_ident>}
******************************************************************************************************************************************************************************************************************************
<block>                  ::= '{' {<block_item>} '}'
<block_item>             ::= (<object_decl>|<alias>|'label'<identifier> [';'])|<statement>|<named_pt_decl>|<named_st_decl>|<enum_decl>|<function_decl>
<object_decl>            ::= %input_type% := <prefix_type> <identifier>|'('<identifier_list>')' [<intializier>]
<statement>              ::= <jump_statement>[';']|<match_block>|<if_statement>|<for_statement>|<iter_statement>|<while_statement>|<do_while_statement>|<expression>[';']
<if_statement>           ::= 'if'[<object_decl> ';'] <expression>':' <statement> ['else' <statement>]
<for_statement>          ::= 'for' [<object_decl>|<expression> ';'][<expression>';'][<expression>] ':' <statement> ['else' <statement>]
<iter_statement>         ::= 'iter' <identifier_list> ':' '('[<iter_arg_list>]')' <statement> ['else' <statement>]
<while_statement>        ::= 'while'[<object_decl> ';'] <expression> ':' <statement> ['else' <statement>]
<do_while_statement>     ::= 'do'[':' <object_decl>] <statement> 'while' <expression> ';'
<jump_statement>         ::= '(give'|'defer' [<expression>])|('break'|'continue'|'goto' ['('<identfier>')'])
<match_block>            ::= 'match'[<object_decl> ';'] <expression> [':' <identifier>] '{' {'case' <tal_encapsulation>|'('[<expArg_list>]')' ':' [<statement>]} '}'
<catch_block>            ::= 'catch' '('<func_param_list>')' <statement> $will do next time$
******************************************************************************************************************************************************************************************************************************
<identifier_list>        ::= <identifier> {',' <identifier>}
<iter_arg_list>          ::= 'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>|<param_iterArg_assoc> {',' 'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>|<param_iterArg_assoc>}
<param_iterArg_assoc>    ::= '.'<identifier> '::=' 'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>
******************************************************************************************************************************************************************************************************************************
<expression>             ::= <assignment_exp> {',' <assignment_exp>}
<assignment_exp>         ::= <pipe_exp> [':='|'::='|'+='|'-='|'*='|'/='|'%='|'^='|'=-'|'=/'|'=%'|'=÷'|'<<='|'>>='|'<<<='|'>>>='|'and_eq'|'or_eq'|'xor_eq'|'nand_eq'|'nor_eq'|'nxor_eq' <assignment_exp>]
<pipe_exp>               ::= <conditional_exp> {'|' <conditional_exp>}
<conditional_exp>        ::= <logical_exp> ['?' '('  <expression>|<jump_statement> [':' <expression>|<jump_statement>] ')']
<logical_exp>            ::= <equality_exp> {'and'|'or'|'xor'|'nand'|'nor'|'nxor' <equality_exp>}
<equality_exp>           ::= <relational_exp> {'='|'!='|'≠' <relational_exp>}
<relational_exp>         ::= <shift_exp> {'<'|'>'|'<='|'>='|'≤'|'≥' <shift_exp>}
<shift_exp>              ::= <additive_exp> {'<<'|'>>'|'<<<'|'>>>' <additive_exp>}
<additive_exp>           ::= <multiplicative_exp> {'+'|'-' <multiplicative_exp>}
<multiplicative_exp>     ::= <exponention_exp> {'*'|'/'|'%'|'/%'|'÷' <exponention_exp>}
<exponention_exp>        ::= <member_exp> {'^' <member_exp>}
<member_exp>             ::= <prefix_exp> {'.'|'->' <prefix_exp>}
<prefix_exp>             ::= ('-'|'!'|'odd'|'++'|'--'|'*'|'&'|'cp'|'imut'|'mem' <prefix_exp>)|<postfix_exp>
<postfix_exp>            ::= <primary_exp> {'++'|'--'|'('[<expArg_list>]')'|'['[<expArg_list>]']'}
<primary_exp>            ::= '('<expression>')'|<r_l>|<block>|<arg>|<reserved_exp_ident>|<object_exp>|<function_exp>|<literal>
******************************************************************************************************************************************************************************************************************************
<reserved_exp_ident>     ::= ['::'] 'tcast'|'bcast'|'none'|'me' [<tal_encapsulation>]
<arg>                    ::= '$('[<expArg_list>]')'
<fromType>               ::= 'fromType'|'@' <typeof>|<identifier>[<tal_encapsulation>]|<function_signature>
<special_ident>          ::= ['::'] <identifier>[<tal_encapsulation>]|<fromType> {'::' <identifier>[<tal_encapsulation>]|<fromType>}
<object_exp>             ::= <prefix_type><initializer>
<function_exp>           ::= 'fn' '('[<func_param_list>]')' [<optPrefix_type>] <block>
<expArg_list>            ::= 'default'|<assignment_exp>|'_v'|'v_'|<param_expArg_assoc> {',' 'default'|<assignment_exp>|'_v'|'v_'|<param_expArg_assoc>}
<param_expArg_assoc>     ::= '.'<identifier> '::=' 'default'|<assignment_exp>|'_v'|'v_'
<tal_encapsulation>      ::= '<'[<typeArg_list>]'>'
<typeArg_list>           ::= 'default'|<optPrefix_type>|'_v'|'v_'|<param_typeArg_assoc> {',' 'default'|<optPrefix_type>|'_v'|'v_'|<param_typeArg_assoc>}
<param_typeArg_assoc>    ::= '.'<identifier> '::=' 'default'|<optPrefix_type>|'_v'|'v_'
<r_l>                    ::= 'r_l' '('<r_l_arg_list>')' '('<expArg_list>')'
<r_l_arg_list>           ::= <relational_op>|<logical_op>|<assignment_exp>|<param_r_l_arg_assoc> {',' <relational_op>|<logical_op>|<assignment_exp>|<param_r_l_arg_assoc>}
<param_r_l_arg__asssoc>  ::= '.'<identifier> '::=' <relational_op>|<logical_op>|<assignment_exp>
******************************************************************************************************************************************************************************************************************************
<literal>                ::= <number_literal>|<boolean_literal>|<characterWise_literal>
<number_literal>         ::= <integer_number_literal>|<real_number_literal> '`'<identifier>
<integer_number_literal> ::= <basic_latin_digit>{<basic_latin_digit>|<basic_latin_alphabet>}[<base>]
<real_number_literal>    ::= <basic_latin_digit>{<basic_latin_digit>|<basic_latin_alphabet>}[<base>]['@'('+'|'-')<basic_latin_digit>]
<characterWise_literal>  ::= <string_literal>|<character_literal>
<identifier>             ::= {<valid_nc_unicode_characters>}|('\' <string_literal>|<reserved_nc_ident>|{<valid_nc_unicode_characters>})
<base>                   ::= '₂'..'₃₆' $unicode code-point [2080, 2089)$
******************************************************************************************************************************************************************************************************************************
$ mimics comments $
% means it accepts input %

type fromType int32::int3
[function type]: (int32:(C) (int32)): (r) b{}; ✕
[function type]: !((int32)int32:(C)): (r) b{}; ✓

[Ubiquitous scope stays! No function or gloabl variable declaration]: does not stay, lexical scope is the boss

During the parsing stage, in all blocks(local or global), not expected tokens are not skipped, rather they remain the current tokens till when they
successfully parsed by another unrelated entity or the token list gotten from the lexer reaches it's end.

[NOTES]: for expressions, precedence is achieved by the order in which operators of different predence call each other,
operators with lower precedence call the operators with higher precedence, ensuring that those ones execute first.
While associativity is achieved by either the operator calling itself( or an operator of lower precedence than itself)
when a condition is met .i.e. (R->L) or the operator calling another operator of higher precedence than itself .i.e. (L->R).

Error Reporting:
- For the global space, any non-language-item found should should be reported and parsing terminated
- For the block scope, any non-block-item should be reported and parsing continued
*/

inline std::ostream& operator<<(std::ostream& out, const std::vector<U8string_view>& tokenStr)
{
    std::size_t n = tokenStr.size();
    if (n == 1)
    {
        out << tokenStr.front();
    }
    else
    {
        out << tokenStr.front() << "\033[0m";
        for (size_t i = 1; i < n; i++)
        {
            if (i == std::size_t(n-1))
            {
                out << " or \033[48;2;10;6;26;32;1m" << tokenStr[i];//"\033[0m" is added in function expect
                break;
            }
            else
            out << ',';

            out << ' ' << "\033[48;2;10;6;26;32;1m" << tokenStr[i] << "\033[0m";
        }
    }
    
    return out;
}

namespace Nc
{
    class Parser
    {
    private:
        enum class ProductionRule: std::uint8_t{ expression, statement };

        using Entity = std::variant<ProductionRule, U8string_view>;

        struct FailureCase
        {
            bool shouldReport{};
            std::optional<std::string_view> optErrorMsg{};
            std::string_view extraErrMsg{};
        };

        Lexer& m_lexer;
        std::reference_wrapper<const Lexer::TokenData> m_currentTokenData;
        std::size_t m_currentTokenDataIndex{};
        Log m_log{};
        // Ast m_ast{};
        bool m_isEndofTokenList{};
        bool m_hasType{};

    public:
        Parser(Lexer& lexer);
        ~Parser() = default;

        void parseTokenList();

        void logBoxStyle(Log::BoxLineStyle s);

        // Ast& getAST();
        // const Ast& getAST()const;

    private:
        void language();
        bool language_items(const FailureCase& fc = FailureCase{false, {}, {}});
        bool name_space_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool type_space_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool named_st_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool named_pt_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool enum_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool alias_decl(const FailureCase& fc = FailureCase{false, {}, {}});
        bool function_decl(bool is_parsed_as_blockItem, const FailureCase& fc = FailureCase{false, {}, {}});
      
        bool function_signature(const FailureCase& fc = FailureCase{false, {}, {}});
        bool _typeof(const FailureCase& fc = FailureCase{false, {}, {}});//c/c++ extention in vscode won't allow use 'typeof' which is werid
        bool optPrefix_type(const FailureCase& fc = FailureCase{false, {}, {}});
        bool prefix_type(const FailureCase& fc = FailureCase{false, {}, {}});
        bool func_param_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool me_func_param(const FailureCase& fc = FailureCase{false, {}, {}});
        bool initializer(const FailureCase& fc = FailureCase{false, {}, {}});
        bool special_ident_list(const FailureCase& fc = FailureCase{false, {}, {}});

        bool block(const FailureCase& fc = FailureCase{false, {}, {}});
        bool block_item(const FailureCase& fc = FailureCase{false, {}, {}});
        bool object_decl(bool is_parsed_as_blockItem, bool should_parse_with_prefixType = true, const FailureCase& fc = FailureCase{false, {}, {}});
        bool statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool if_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool for_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool iter_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool while_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool do_while_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool jump_statement(const FailureCase& fc = FailureCase{false, {}, {}});
        bool match_block(const FailureCase& fc = FailureCase{false, {}, {}});

        bool identifier_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool iterArg_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool param_iterArg_assoc(const FailureCase& fc = FailureCase{false, {}, {}});

        bool expression(const FailureCase& fc = FailureCase{false, {}, {}});
        bool assignment_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool pipe_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool conditional_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool logical_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool equality_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool relational_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool shift_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool additive_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool multiplicative_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool exponention_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool member_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool prefix_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool postfix_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool primary_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        
        bool reserved_exp_ident(const FailureCase& fc = FailureCase{false, {}, {}});
        bool arg(const FailureCase& fc = FailureCase{false, {}, {}});
        bool fromType(const FailureCase& fc = FailureCase{false, {}, {}});
        bool special_ident(const FailureCase& fc = FailureCase{false, {}, {}});
        bool object_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool function_exp(const FailureCase& fc = FailureCase{false, {}, {}});
        bool expArg_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool param_expArg_assoc(const FailureCase& fc = FailureCase{false, {}, {}});
        bool tal_encapsulation(const FailureCase& fc = FailureCase{false, {}, {}});
        bool typeArg_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool param_typeArg_assoc(const FailureCase& fc = FailureCase{false, {}, {}});
        bool r_l(const FailureCase& fc = FailureCase{false, {}, {}});
        bool r_l_arg_list(const FailureCase& fc = FailureCase{false, {}, {}});
        bool param_r_l_arg_assoc(const FailureCase& fc = FailureCase{false, {}, {}});

        std::string mapTokenToCategory(const Lexer::TokenData* x = nullptr);

        // bool errorRecoveryStep_altruism(std::list<Entity>){}
        // bool errorRecoveryStep_selfishness(){}
        // bool errorRecoveryStep_timeTravel_altruism(){}

        bool expect(Lexer::TokenType tokenType, std::vector<U8string_view> tokenStr, const FailureCase& fc = FailureCase{false, {}, {}});

        void nextTokenIndex(std::size_t amount = 1);
        void previousTokenIndex(std::size_t amount = 1);

        void start_log();
        void start_multiTokenlog(std::size_t tokenDataIndexStart);
        void afterTokenLog();
        void missingBraceLog(std::size_t savedTokenBraceDataIndex);
        void lineRelated_log(std::size_t tokenDataIndex);
        void spaceLog();
        void additionalLog();

        // LocationFinder makeLocFinder(std::uint32_t lineNumber0, std::uint32_t relativeColumn0, std::uint32_t absoluteColumn1);

        void errorOutput();
    };
    
} // namespace Nc

#endif