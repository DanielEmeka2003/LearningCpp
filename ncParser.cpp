#include <iterator>

#include "ncParser.h"

namespace Nc
{
    Parser::Parser(Lexer& lexer): m_lexer{ lexer }, m_currentTokenData{ lexer.getTokenDataList()[0] } { }

    void Parser::parseTokenList()
    {
        language();

        if (!m_log.isLogEmpty())
        errorOutput();
    }

    void Parser::logBoxStyle(Log::BoxLineStyle s)
    { m_log.changeBoxLineStyle(s); }

    // Ast& Parser::getAST()
    // { return m_ast; }

    // const Ast& Parser::getAST()const
    // { return m_ast; }
/*
* when expecting two or more token/token-groups, and there is need for reporting an error:
* [using the logical or]: (with short circuit in mind)
* % if (expect() or expect(FailureCase{...}))
* % if (!expect(FailureCase{...}) or !expect(FailureCase{...}))
* 
* [using the logical and]: (with short circuit in mind)
* % if (expect(FailureCase{...}) and expect(FailureCase{...}))
* % if (!expect() and !expect(FailureCase{...}))
*/

    void Parser::language()
    {
        while (true)
        {
            if (m_isEndofTokenList)
            {
                break;
            }
            else if (!language_items())
            {
                expect(Lexer::TokenType::_miscellany, {}, FailureCase{{}, "Expected a \033[48;2;10;6;26;32;1m<language_item>\033[0m", "in \033[48;2;10;6;26;33;1mglobal_space_scope\033[0m"});
                spaceLog();
                m_log.write("   Below is \033[48;2;10;6;26;36;1m<language_item>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   \033[48;2;10;6;26;32;1m<function_decl>|<object_decl>[';']|<named_st_decl>|<named_pt_decl>|<enum_decl>|<alias>[';']|<namespace_decl>\033[0m"), additionalLog();
            }
            // m_ast.mkNcFileNode();
        }

        // m_ast.makeRootNode();
    }

    bool Parser::language_items(const FailureCase& fc)
    {
        if (object_decl(false) or alias_decl())
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            return true;
        }
        else if (function_decl() or named_st_decl() or named_pt_decl() or enum_decl() or namespace_decl())
        {
            return true;
        }
        
        return false;
    }
    
    bool Parser::namespace_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rNamespace}, fc))
        {
            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}))
            {
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                while (true)
                {
                    if (m_isEndofTokenList)
                    {
                        expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "to end namespace declaration"});
                        afterTokenLog(), missingBraceLog(savedLcurlyTokenDataIndex);
                        break;
                    }
                    else if (!language_items())
                    {
                        if (expect(Lexer::TokenType::symbol, {Lexer::sRcurly}))
                        {
                            break;
                        }
                        else
                        {
                            expect(Lexer::TokenType::_miscellany, {}, FailureCase{{}, "Expected a \033[48;2;10;6;26;32;1m<language_item>\033[0m", "in \033[48;2;10;6;26;33;1m<namespace_decl>\033[0m's BNF syntax"});
                            spaceLog();
                            m_log.write("   Below is \033[48;2;10;6;26;36;1m<language_item>\033[0m's BNF syntax:"), additionalLog();
                            m_log.write("   \033[48;2;10;6;26;32;1m<function_decl>|<object_decl>[';']|<named_st_decl>|<named_pt_decl>|<enum_decl>|<alias>[';']|<namespace_decl>\033[0m"), additionalLog();
                        }   
                    }   
                }
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}))
            {}
            else
            m_log.write("expected either [ ; ] or [ { ] in namespace declaration"), start_log();

            return true;
        }
        return false;
    }

    bool Parser::named_st_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rNamed_st}, fc))
        {
            if (!special_ident(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'named_st' \033[48;2;10;6;26;31;1m<special_ident>\033[0m ('{' [<optPrefix_type> {',' <optPrefix_type>}] '}')|[';']"), additionalLog();
            }
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}))
            {
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (optPrefix_type())
                {
                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
                    {
                        if (!optPrefix_type(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m", "in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax"}))
                        {
                            afterTokenLog(), spaceLog();
                            m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax:"), additionalLog();
                            m_log.write("   'named_st' <special_ident> ('{' [<optPrefix_type> {',' \033[48;2;10;6;26;31;1m<optPrefix_type>\033[0m}] '}')|[';']"), additionalLog();
                        }
                    }
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_st_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'named_st' <special_ident> ('{' [<optPrefix_type> {',' <optPrefix_type>}] \033[48;2;10;6;26;31;1m'}'\033[0m)|[';']"), additionalLog();
                }
            }
            else
            {
                expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            }
            return true;
        }
        return false;
    }

    bool Parser::named_pt_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rNamed_pt}, fc))
        {
            if (!special_ident(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'named_pt' \033[48;2;10;6;26;31;1m<special_ident>\033[0m ('{' [<object_decl> {';' <object_decl>}] '}')|[';']"), additionalLog();
            }
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}))
            {
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (object_decl(false, false))
                {
                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
                    {
                        if (!object_decl(false, false, FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<object_decl>\033[0m", "in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax"}))
                        {
                            afterTokenLog(), spaceLog();
                            m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax:"), additionalLog();
                            m_log.write("   'named_pt' <special_ident> ('{' [<object_decl> {';' \033[48;2;10;6;26;31;1m<object_decl>\033[0m}] '}')|[';']"), additionalLog();
                        }
                    }
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<named_pt_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'named_pt' <special_ident> ('{' [<object_decl> {';' <object_decl>}] \033[48;2;10;6;26;31;1m'}'\033[0m)|[';']"), additionalLog();
                }
            }
            else
            {
                expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            }
            return true;
        }
        return false;
    }

    bool Parser::enum_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rEnum}, fc))
        {
            if (!special_ident(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'enum' \033[48;2;10;6;26;31;1m<special_ident>\033[0m ('{' <identifier>[<initializer] {',' <identifier>[<initializer]} '}')|[';']"), additionalLog();
            }
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}))
            {
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (expect(Lexer::TokenType::identifier, {}))
                {
                    if (initializer())
                    {}
                    
                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
                    {
                        if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax"}))
                        {
                            afterTokenLog(), spaceLog();
                            m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax:"), additionalLog();
                            m_log.write("   'enum' <special_ident> ('{' <identifier>[<initializer] {',' \033[48;2;10;6;26;31;1m<identifier>\033[0m[<initializer]} '}')|[';']"), additionalLog();
                        }
                        else
                        {
                            if (initializer()){}
                        }
                    }
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<enum_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'enum' <special_ident> ('{' <identifier>[<initializer] {',' <identifier>[<initializer]} \033[48;2;10;6;26;31;1m'}'\033[0m)|[';']"), additionalLog();
                }
            }
            else
            {
                expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            }
            return true;
        }
        return false;
    }
    
    bool Parser::alias_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rAlias}, fc))
        {
            if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'alias' \033[48;2;10;6;26;31;1m<identifier>\033[0m '=' ['namespace']<special_ident>|<prefix_type>"), additionalLog();
            }
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sEqual}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'alias' <identifier> \033[48;2;10;6;26;31;1m'='\033[0m ['namespace']<special_ident>|<prefix_type>"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rNamespace}))
            {
                if (!special_ident(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'alias' <identifier> '=' ['namespace']\033[48;2;10;6;26;31;1m<special_ident>\033[0m|<prefix_type>"), additionalLog();
                }
            }
            else if (special_ident())
            {}
            else if (prefix_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m, a \033[48;2;10;6;26;32;1m<prefix_type>\033[0m or a \033[48;2;10;6;26;33;1mnamespace\033[0m prefixed \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<alias_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'alias' <identifier> '=' \033[48;2;10;6;26;31;1m['namespace']<special_ident>|<prefix_type>\033[0m"), additionalLog();
            }
            
            return true;
        }
        return false;
    }

    bool Parser::function_decl(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rFn}, fc))
        {
            if (m_currentTokenData.get().token == Lexer::sLparen)//must be a function_exp right??
            return previousTokenIndex(), false;

            if (!special_ident(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m", "in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' \033[48;2;10;6;26;31;1m<special_ident>\033[0m '('[<func_param_list>]')' [<optPrefix_type>] <block>|[';']"), additionalLog();
            }
            
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' <special_ident> \033[48;2;10;6;26;31;1m'('\033[0m[<func_param_list>]')' [<optPrefix_type>] <block>|[';']"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            func_param_list();
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax"}))
            {
                afterTokenLog();
                if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());

                spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' <special_ident> '('[<func_param_list>]\033[48;2;10;6;26;31;1m')'\033[0m [<optPrefix_type>] <block>|[';']"), additionalLog();
            }

            // Type giveType{};
            bool wasTypeFound{};
            if ((wasTypeFound = optPrefix_type())){}

            if (block())
            {
                // m_ast.mkFunctionDefNode(giveType, funcIdent, m_ast._getParameterList(), m_ast.getBlockNode());
            }
            else
            {
                if (!wasTypeFound)
                {
                    //parse again just to log an error
                    optPrefix_type(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m", "because \033[48;2;10;6;26;33;1mfunction-disclosures\033[0m require them since they have no body"});
                }
                expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            }
            // m_ast.makeFuncDeclarationNode();
            
            return true;
        }
        return false;
    }


    bool Parser::function_signature(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rFn}, fc))
        {
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' \033[48;2;10;6;26;31;1m'('\033[0m[<optPrefix_type> {',' <optPrefix_type>}]')' <optPrefix_type>"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            
            // List<Type> typeParameterList{};

            if (optPrefix_type())
            {
                // typeParameterList.push_back(std::move(m_ast._getType()));

                while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
                {
                    if (!optPrefix_type(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m", "in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax"}))
                    {
                        afterTokenLog(), spaceLog();
                        m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   'fn' '('[<optPrefix_type> {',' \033[48;2;10;6;26;31;1m<optPrefix_type>\033[0m}]')' <optPrefix_type>"), additionalLog();
                    }
                
                    // typeParameterList.push_back(std::move(m_ast._getType()));
                }
            }
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax"}))
            {
                afterTokenLog();
                if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());

                spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' '('[<optPrefix_type> {',' <optPrefix_type>}]\033[48;2;10;6;26;31;1m')'\033[0m <optPrefix_type>"), additionalLog();
            }
            
            if (!optPrefix_type(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m", "in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_signature>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' '('[<optPrefix_type> {',' <optPrefix_type>}]')' \033[48;2;10;6;26;31;1m<optPrefix_type>\033[0m"), additionalLog();
            }

            // Type giveType{ std::move(m_ast._getType()) };

            // m_ast._makeFunctionType(typeParameterList, giveType);

            return true;
        }
        return false;
    }

    bool Parser::_typeof(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rTypeof}, fc))
        {
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'typeof' \033[48;2;10;6;26;31;1m'('\033[0m<expression>')'"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'typeof' '('\033[48;2;10;6;26;31;1m<expression>\033[0m')'"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax"}))
            {
                afterTokenLog();
                if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());
                spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<typeof>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'typeof' '('<expression>\033[48;2;10;6;26;31;1m')'\033[0m"), additionalLog();
            }

            return true;
        }
        return false;
    }

    bool Parser::optPrefix_type(const FailureCase& fc)
    {
       bool result{};

       if ((result = expect(Lexer::TokenType::reserved, {Lexer::rType})))
       {
            if (special_ident())
            {
                
            }
            else if (function_signature(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m or a \033[48;2;10;6;26;32;1m<function_signature>\033[0m", "in \033[48;2;10;6;26;36;1m<optPrefix_type>\033[0m's BNF syntax"}))
            {
                
            }
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<optPrefix_type>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   ( (['type'] \033[48;2;10;6;26;31;1m<special_ident>|<function_signature>\033[0m)|'guess' [':' <string_literal>] )|<typeof>"), additionalLog();
            }
       }
       else if ((result = special_ident()))
       {

       }
       else if ((result = function_signature()))
       {

       }
       else if ((result = expect(Lexer::TokenType::reserved, {Lexer::rGuess})))
       {
            
       }
       
       if (result)
       {
            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}))
            {
                if (!expect(Lexer::TokenType::literal, {Lexer::lStr}, FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<string_literal>\033[0m", "in \033[48;2;10;6;26;36;1m<optPrefix_type>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<optPrefix_type>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ( (['type'] <special_ident>|<function_signature>)|'guess' [':' \033[48;2;10;6;26;31;1m<string_literal>\033[0m] )|<typeof>"), additionalLog();
                }
            }
            return true;
       }
       else
       _typeof(fc);
       
        return false;
    }

    bool Parser::prefix_type(const FailureCase& fc)
    {
        if (bool isTypePrefix{}; (isTypePrefix = expect(Lexer::TokenType::reserved, {Lexer::rType})) or expect(Lexer::TokenType::reserved, {Lexer::rGuess}))
        {
            if (isTypePrefix)
            {
                if (special_ident())
                {
                    
                }
                else if (function_signature(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<special_ident>\033[0m or a \033[48;2;10;6;26;32;1m<function_signature>\033[0m", "in \033[48;2;10;6;26;36;1m<prefix_type>\033[0m's BNF syntax"}))
                {
                    
                }
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<prefix_type>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ( ('type' \033[48;2;10;6;26;31;1m<special_ident>|<function_signature>\033[0m)|'guess' [':' <string_literal>] )|<typeof>"), additionalLog();
                }
            }
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}))
            {
                if (!expect(Lexer::TokenType::literal, {Lexer::lStr}, FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<string_literal>\033[0m", "in \033[48;2;10;6;26;36;1m<prefix_type>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<prefix_type>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ( ('type' <special_ident>|<function_signature>)|'guess' [':' \033[48;2;10;6;26;31;1m<string_literal>\033[0m] )|<typeof>"), additionalLog();
                }
            }
            return true;
        }
        else
        return _typeof(fc);
        
        return false;
    }

    bool Parser::func_param_list(const FailureCase& fc)
    {
        if (object_decl(false, false, fc))
        {
            if (expect(Lexer::TokenType::identifier, {}))
            {
                if (initializer())
                {}   
            }
            
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (!object_decl(false, false, FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m", "in \033[48;2;10;6;26;36;1m<func_param_list>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<func_param_list>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <object_decl> {',' \033[48;2;10;6;26;31;1m<object_decl>\033[0m}"), additionalLog();
                }
            }
            return true;
        }
        return false;
    }

    bool Parser::initializer(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sInitAssign}))
        {
            if (!assignment_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<initializer>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<initializer>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '::='\033[48;2;10;6;26;31;1m<assignment_exp>\033[0m|'('[<expArg_list>]')'"), additionalLog();
            }
            
            return true;
        }
        else if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc))
        {
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (expArg_list())
            {}
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<initializer>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<initializer>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '::='<assignment_exp>|'('[<expArg_list>]\033[48;2;10;6;26;31;1m')'\033[0m"), additionalLog();
            }
            return true;
        }
        
        return false;
    }

    
    bool Parser::block(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}, fc))
        {
            auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            // List<BlockItem> blockItemList{};

            while (true)
            {
                if (m_isEndofTokenList)
                {
                    expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<block>\033[0m's BNF syntax"});
                    afterTokenLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<block>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   '{' {<block_item>} \033[48;2;10;6;26;31;1m'}'\033[0m"), additionalLog();
                    break;
                }
                else if (!block_item())
                {
                    if (expect(Lexer::TokenType::symbol, {Lexer::sRcurly}))
                    {
                        // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, m_currentTokenData.get().absoluteColumn)};

                        // if (blockItemList.empty())
                        // {
                        //     Void v{Void::InUse::yes};
                        //     m_ast.makeBlockNode(v, locFinder);
                        // }
                        // else
                        // m_ast.makeBlockNode(blockItemList, locFinder);

                        break;
                    }
                    else
                    {
                        expect(Lexer::TokenType::_miscellany, {}, FailureCase{{}, "Expected a \033[48;2;10;6;26;32;1m<block_item>\033[0m", "in \033[48;2;10;6;26;36;1m<block>\033[0m's BNF syntax"});
                        spaceLog();
                        m_log.write("   Below is \033[48;2;10;6;26;36;1m<block_item>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   \033[48;2;10;6;26;32;1m<object_decl>[';']|<statement>|<named_pt_decl>|<named_st_decl>|<enum_decl>|<alias>[';']\033[0m"), additionalLog();
                    }
                }
            }   
            return true;
        }
        return false;
    }

    bool Parser::block_item(const FailureCase& fc)
    {
        if (object_decl(true) or alias_decl())//<statement> should be parsed after <object_decl> and <function_decl>
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});

            return true;
        }
        else if (named_pt_decl() or named_st_decl() or enum_decl() or function_decl() or statement(fc))//function
        {
            // m_ast.makeStatementNode();

            return true;
        }
        
        return false;
    }

    bool Parser::object_decl(bool is_parsed_as_blockItem, bool should_parse_with_prefixType, const FailureCase& fc)
    {
        std::string which_type{};
        bool result{};
        
        if (should_parse_with_prefixType)
        result = prefix_type(fc), which_type = "<prefix_type>";
        else
        result = optPrefix_type(fc), which_type = "<optPrefix_type>";
        
        if (result)
        {
            if (is_parsed_as_blockItem)
            {
                if (m_currentTokenData.get().token == Lexer::sInitAssign or m_currentTokenData.get().token == Lexer::sLparen)
                return m_hasType = true, false;
            }
            
            if (expect(Lexer::TokenType::identifier, {}))
            {
                
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<identifier>\033[0m or \033[48;2;10;6;26;32;1m(\033[0m to start an \033[48;2;10;6;26;36;1m<identifier-list>\033[0m", "in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax"}))
            {
                auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (!identifier_list(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<identifier_list>\033[0m", "in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ", which_type, " <identifier>|'('\033[48;2;10;6;26;31;1m<identifier_list>\033[0m')' [<intializier>]"), additionalLog();
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "to end the \033[48;2;10;6;26;36;1m<identifier_list>\033[0m in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ", which_type, " <identifier>|'('<identifier_list>\033[48;2;10;6;26;31;1m')'\033[0m [<intializier>]"), additionalLog();
                }
            }
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<object_decl>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   ", which_type, " \033[48;2;10;6;26;31;1m<identifier>|'('<identifier_list>')'\033[0m [<intializier>]"), additionalLog();
            }
            
            if (initializer())
            {}
            return true;
        }
        return false;
    }

    bool Parser::statement(const FailureCase& fc)
    {
        if (jump_statement())
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            // m_ast.mkJumpStatementNode()

            return true;
        }
        if (if_statement() or for_statement() or iter_statement() or while_statement() or do_while_statement() or match_block())
        {
            return true;
        }
        else if (expression(fc))
        {
            // m_ast.makeExpNode()
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            
            return true;
        }
        return false;
    }

    bool Parser::if_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rIf}, fc))
        {
            if (object_decl(false))
            {
                if (!expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'if'[<object_decl> \033[48;2;10;6;26;31;1m';'\033[0m] <expression> ':' <statement> ['else' <statement>]"), additionalLog();
                }
            }

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'if'[<object_decl> ';'] \033[48;2;10;6;26;31;1m<expression>\033[0m ':' <statement> ['else' <statement>]"), additionalLog();
            }

            // m_ast.makeExpNode();
            // Expression conditionalExp{ std::move(m_ast.getExpNode()) };

            if (!expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'if'[<object_decl> ';'] <expression> \033[48;2;10;6;26;31;1m':'\033[0m <statement> ['else' <statement>]"), additionalLog();
            }

            if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'if'[<object_decl> ';'] <expression> ':' \033[48;2;10;6;26;31;1m<statement>\033[0m ['else' <statement>]"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rElse}))
            {
                if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<if_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'if'[<object_decl> ';'] <expression> ':' <statement> ['else' \033[48;2;10;6;26;31;1m<statement>\033[0m]"), additionalLog();
                }
            }
            
            return true;
        }
        return false;
    }

    bool Parser::for_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rFor}, fc))
        {
            bool result{};

            if ((result = object_decl(false)))
            {}
            else if ((result = expression()))
            {}

            if (result)
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'for' [<object_decl>|<expression> \033[48;2;10;6;26;31;1m';'\033[0m][<expression>';'][<expression>] ':' <statement> ['else' <statement>]"), additionalLog();
                }
            }
            
            if (expression())
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'for' [<object_decl>|<expression> ';'][<expression>\033[48;2;10;6;26;31;1m';'\033[0m][<expression>] ':' <statement> ['else' <statement>]"), additionalLog();
                }
            }

            if (expression())
            {}
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'for' [<object_decl>|<expression> ';'][<expression>';'][<expression>] \033[48;2;10;6;26;31;1m':'\033[0m <statement> ['else' <statement>]"), additionalLog();
            }

            if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'for' [<object_decl>|<expression> ';'][<expression>';'][<expression>] ':' \033[48;2;10;6;26;31;1m<statement>\033[0m ['else' <statement>]"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rElse}))
            {
                if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<for_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'for' [<object_decl>|<expression> ';'][<expression>';'][<expression>] ':' <statement> ['else' \033[48;2;10;6;26;31;1m<statement>\033[0m]"), additionalLog();
                }
            }            
            
            return true;
        }
        return false;
    }

    bool Parser::iter_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rIter}, fc))
        {
            if (!identifier_list(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<identifier_list>\033[0m", "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'iter' \033[48;2;10;6;26;31;1m<identifier_list>\033[0m ':' '('[<iter_arg_list>]')' <statement> ['else' <statement>]"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'iter' <identifier_list> \033[48;2;10;6;26;31;1m':'\033[0m '('[<iter_arg_list>]')' <statement> ['else' <statement>]"), additionalLog();
            }
            
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'iter' <identifier_list> ':' \033[48;2;10;6;26;31;1m'('\033[0m[<iter_arg_list>]')' <statement> ['else' <statement>]"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (iterArg_list()){}
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog();
                if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());

                spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'iter' <identifier_list> ':' '('[<iter_arg_list>]\033[48;2;10;6;26;31;1m')'\033[0m <statement> ['else' <statement>]"), additionalLog();
            }

            if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'iter' <identifier_list> ':' '('[<iter_arg_list>]')' \033[48;2;10;6;26;31;1m<statement>\033[0m ['else' <statement>]"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rElse}))
            {
                if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iter_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'iter' <identifier_list> ':' '('[<iter_arg_list>]')' <statement> ['else' \033[48;2;10;6;26;31;1m<statement>\033[0m]"), additionalLog();
                }
            }

            return true;
        }
        return false;
    }

    bool Parser::while_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rWhile}, fc))
        {
            if (object_decl(false))
            {
                if (!expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'while'[<object_decl> \033[48;2;10;6;26;31;1m';'\033[0m] <expression> ':' <statement> ['else' <statement>]"), additionalLog();
                }
            }

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'while'[<object_decl> ';'] \033[48;2;10;6;26;31;1m<expression>\033[0m ':' <statement> ['else' <statement>]"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'while'[<object_decl> ';'] <expression> \033[48;2;10;6;26;31;1m':'\033[0m <statement> ['else' <statement>]"), additionalLog();
            }

            if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'while'[<object_decl> ';'] <expression> ':' \033[48;2;10;6;26;31;1m<statement>\033[0m ['else' <statement>]"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rElse}))
            {
                if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<while_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'while'[<object_decl> ';'] <expression> ':' <statement> ['else' \033[48;2;10;6;26;31;1m<statement>\033[0m]"), additionalLog();
                }
            }
            
            return true;
        }
        return false;
    }

    bool Parser::do_while_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rDo}, fc))
        {
            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}))
            {
                if (!object_decl(false, true, FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<object_decl>\033[0m", "in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'do'[':' \033[48;2;10;6;26;31;1m<object_decl>\033[0m] <statement> 'while' <expression> ';'"), additionalLog();
                }
            }
            
            if (!statement(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<statement>\033[0m", "in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'do'[':' <object_decl>] \033[48;2;10;6;26;31;1m<statement>\033[0m 'while' <expression> ';'"), additionalLog();
            }
            
            // m_ast.makeStatementNode();
            // Statement doStatement{ std::move(m_ast.getStatementNode()) };

            if (!expect(Lexer::TokenType::reserved, {Lexer::rWhile}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'do'[':' <object_decl>] <statement> \033[48;2;10;6;26;31;1m'while'\033[0m <expression> ';'"), additionalLog();
            }

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'do'[':' <object_decl>] <statement> 'while' \033[48;2;10;6;26;31;1m<expression>\033[0m ';'"), additionalLog();
            }

            // m_ast.makeExpNode();
            // Expression whileConditionExp{ std::move(m_ast.getExpNode()) };

            if (!expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<do_while_statement>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'do'[':' <object_decl>] <statement> 'while' <expression> \033[48;2;10;6;26;31;1m';'\033[0m"), additionalLog();
            }

            // m_ast.mkDoWhileLoopNode(doStatement, whileConditionExp);

            return true;
        }
        return false;
    }

    bool Parser::jump_statement(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rGive}))
        {
            if (expression())
            {
                // m_ast.makeExpNode();
                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};
                // m_ast.mkGiveJSNode(m_ast.getExpNode(), locFinder);
            }
            else
            {
                // Void v{Void::InUse::yes};
                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};
                // m_ast.mkGiveJSNode(v, locFinder);
            }
            return true;
        }
        else if (expect(Lexer::TokenType::reserved, {Lexer::rBreak, Lexer::rContinue}, fc))
        {
            // LocationFinder locFinder{makeLocFinder(m_currentTokenData.get().line, m_currentTokenData.get().relativeColumn, m_currentTokenData.get().absoluteColumn)};

            // if (m_currentTokenData.get().token == Lexer::rBreak)
            // m_ast.mkBreakJSNode(locFinder);
            // else
            // m_ast.mkContinueJSNode(locFinder);


            if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}))
            {
                auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;;

                if (!expect(Lexer::TokenType::literal, {Lexer::lInt}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<jump_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<jump_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'give'[<expression>]|('break'|'continue' ['('\033[48;2;10;6;26;31;1m<integer_number_literal>\033[0m')'])"), additionalLog();
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<jump_statement>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<jump_statement>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'give'[<expression>]|('break'|'continue' ['('<integer_number_literal>\033[48;2;10;6;26;31;1m')'\033[0m])"), additionalLog();
                }
            }
            return true;
        }
        return false;
    }

    bool Parser::match_block(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rMatch}, fc))
        {
            if (object_decl(false))
            {
                if (!expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'match'[<object_decl> \033[48;2;10;6;26;31;1m';'\033[0m] <expression> [':' <identifier>] '{' {('case' <tal_encapsulation>|<expArg_list>)|'default ':' [<statement>]}  '}'"), additionalLog();
                }
            }

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'match'[<object_decl> ';'] \033[48;2;10;6;26;31;1m<expression>\033[0m [':' <identifier>] '{' {('case' <tal_encapsulation>|<expArg_list>)|'default ':' [<statement>]}  '}'"), additionalLog();
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}))
            {
                if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'match'[<object_decl> ';'] <expression> [':' \033[48;2;10;6;26;31;1m<identifier>\033[0m] '{' {('case' <tal_encapsulation>|<expArg_list>)|'default ':' [<statement>]}  '}'"), additionalLog();
                }
            }

            std::optional<std::size_t> optional_savedLcurlyTokenDataIndex{};
            if (!expect(Lexer::TokenType::symbol, {Lexer::sLcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'match'[<object_decl> ';'] <expression> [':' <identifier>] \033[48;2;10;6;26;31;1m'{'\033[0m {('case' <tal_encapsulation>|<expArg_list>)|'default ':' [<statement>]}  '}'"), additionalLog();
            }
            else
            optional_savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            
            while (expect(Lexer::TokenType::reserved, {Lexer::rCase, Lexer::rDefault}))
            {
                if (m_currentTokenData.get().token == Lexer::rCase)
                {
                    if (tal_encapsulation())
                    {
                        
                    }
                    else if (expArg_list(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<tal_encapsulation>\033[0m or an \033[48;2;10;6;26;32;1m<expArg_list>\033[0m", "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
                    {
                        
                    }
                    else
                    {
                        afterTokenLog(), spaceLog();
                        m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   'match'[<object_decl> ';'] <expression> [':' <identifier>] '{' {('case' \033[48;2;10;6;26;31;1m<tal_encapsulation>|<expArg_list>\033[0m)|'default ':' [<statement>]}  '}'"), additionalLog();
                    }
                }

                if (!expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'match'[<object_decl> ';'] <expression> [':' <identifier>] '{' {('case' <tal_encapsulation>|<argument_list>)|'default \033[48;2;10;6;26;31;1m':'\033[0m [<statement>]}  '}'"), additionalLog();
                }
                
                statement();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(optional_savedLcurlyTokenDataIndex.value_or(0)), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<match_block>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'match'[<object_decl> ';'] <expression> [':' <identifier>] '{' {('case' <tal_encapsulation>|<argument_list>)|'default ':' [<statement>]}  \033[48;2;10;6;26;31;1m'}'\033[0m"), additionalLog();
            }
            return true;
        }
        return false;
    }


    bool Parser::identifier_list(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::identifier, {}, fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<identifier_list>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<identifier_list>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <identifier> {',' \033[48;2;10;6;26;31;1m<identifier>\033[0m}"), additionalLog();
                } 
            }
            return true;
        }
        return false;
    }

    bool Parser::iterArg_list(const FailureCase& fc)
    {
        bool result{};
        if ((result = expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rIter_no_end})))
        {
            
        }
        else if ((result = expect(Lexer::TokenType::symbol, {Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo})) or (result = expect(Lexer::TokenType::symbol, {Lexer::sAssign, Lexer::sInitAssign, Lexer::sPlusAssign, Lexer::sMinusAssign, Lexer::sMultiplyAssign, Lexer::sDivideAssign, Lexer::sRemainderAssign, Lexer::sExponentionAssign, Lexer::sReverseMinusAssign, Lexer::sReverseDivideAssign, Lexer::sReverseRemainderAssign, Lexer::sReverseAlternateDivideAssign, Lexer::sShiftLeftAssign, Lexer::sShiftRightAssign, Lexer::sRotateLeftAssign, Lexer::sRotateRightAssign})) or (result = expect(Lexer::TokenType::reserved, {Lexer::rAnd_eq, Lexer::rOr_eq, Lexer::rXor_eq, Lexer::rNand_eq, Lexer::rNor_eq, Lexer::rNxor_eq})))
        {
            
        }
        else if ((result = param_iterArg_assoc()))
        {
            
        }
        else if ((result = assignment_exp(fc)))
        {

        }
        
        if (result)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rIter_no_end}))
                {
                    
                }
                else if (expect(Lexer::TokenType::symbol, {Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo}) or expect(Lexer::TokenType::symbol, {Lexer::sAssign, Lexer::sInitAssign, Lexer::sPlusAssign, Lexer::sMinusAssign, Lexer::sMultiplyAssign, Lexer::sDivideAssign, Lexer::sRemainderAssign, Lexer::sExponentionAssign, Lexer::sReverseMinusAssign, Lexer::sReverseDivideAssign, Lexer::sReverseRemainderAssign, Lexer::sReverseAlternateDivideAssign, Lexer::sShiftLeftAssign, Lexer::sShiftRightAssign, Lexer::sRotateLeftAssign, Lexer::sRotateRightAssign}) or expect(Lexer::TokenType::reserved, {Lexer::rAnd_eq, Lexer::rOr_eq, Lexer::rXor_eq, Lexer::rNand_eq, Lexer::rNor_eq, Lexer::rNxor_eq}))
                {
                    
                }
                else if (param_iterArg_assoc())
                {
                    
                }
                else if (assignment_exp(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m, \033[48;2;10;6;26;32;1miter_no_end\033[0m, a \033[48;2;10;6;26;32;1m<relational_op>\033[0m, an \033[48;2;10;6;26;32;1m<assignment_op>\033[0m or a \033[48;2;10;6;26;32;1m<param_iterArg_assoc>\033[0m", "in \033[48;2;10;6;26;36;1m<iterArg_list>\033[0m's BNF syntax"}))
                {

                }
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<iterArg_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>|<param_iterArg_assoc> {',' \033[48;2;10;6;26;31;1m'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>|<param_iterArg_assoc>\033[0m}"), additionalLog();
                }
            }
            return true;
        }
        return false;
    }

    bool Parser::param_iterArg_assoc(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf}, fc))
        {
            if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'\033[48;2;10;6;26;31;1m<identifier>\033[0m '::=' 'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sInitAssign}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> \033[48;2;10;6;26;31;1m'::='\033[0m 'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rIter_no_end}))
            {}
            else if (expect(Lexer::TokenType::symbol, {Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo}) or expect(Lexer::TokenType::symbol, {Lexer::sAssign, Lexer::sInitAssign, Lexer::sPlusAssign, Lexer::sMinusAssign, Lexer::sMultiplyAssign, Lexer::sDivideAssign, Lexer::sRemainderAssign, Lexer::sExponentionAssign, Lexer::sReverseMinusAssign, Lexer::sReverseDivideAssign, Lexer::sReverseRemainderAssign, Lexer::sReverseAlternateDivideAssign, Lexer::sShiftLeftAssign, Lexer::sShiftRightAssign, Lexer::sRotateLeftAssign, Lexer::sRotateRightAssign}) or expect(Lexer::TokenType::reserved, {Lexer::rAnd_eq, Lexer::rOr_eq, Lexer::rXor_eq, Lexer::rNand_eq, Lexer::rNor_eq, Lexer::rNxor_eq}))
            {}
            else if (assignment_exp(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m, \033[48;2;10;6;26;32;1miter_no_end\033[0m, a \033[48;2;10;6;26;32;1m<relational_op>\033[0m or an \033[48;2;10;6;26;32;1m<assignment_op>\033[0m", "in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_iterArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> '::=' \033[48;2;10;6;26;31;1m'default'|<assignment_exp>|'iter_no_end'|<relational_op>|<assignment_op>\033[0m"), additionalLog();
            }
            
            return true;
        }
        return false;
    }


    bool Parser::expression(const FailureCase& fc)
    {
        if (assignment_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!assignment_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<pipe_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<comma_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<comma_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <pipe_exp> {',' \033[48;2;10;6;26;31;1m<pipe_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }

    bool Parser::assignment_exp(const FailureCase& fc)
    {
        if (pipe_exp(fc))
        {
            if (expect(Lexer::TokenType::symbol, {Lexer::sAssign, Lexer::sInitAssign, Lexer::sPlusAssign, Lexer::sMinusAssign, Lexer::sMultiplyAssign, Lexer::sDivideAssign, Lexer::sRemainderAssign, Lexer::sExponentionAssign, Lexer::sReverseMinusAssign, Lexer::sReverseDivideAssign, Lexer::sReverseRemainderAssign, Lexer::sReverseAlternateDivideAssign, Lexer::sShiftLeftAssign, Lexer::sShiftRightAssign, Lexer::sRotateLeftAssign, Lexer::sRotateRightAssign}) or expect(Lexer::TokenType::reserved, {Lexer::rAnd_eq, Lexer::rOr_eq, Lexer::rXor_eq, Lexer::rNand_eq, Lexer::rNor_eq, Lexer::rNxor_eq}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!assignment_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<assignment_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<assignment_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <pipe_exp> [':='|'::='|'+='|'-='|'*='|'/='|'%='|'^='|'=-'|'=/'|'=%'|'=÷'|'<<='|'>>='|'<<<='|'>>>='|'and_eq'|'or_eq'|'xor_eq'|'nand_eq'|'nor_eq'|'nxor_eq' \033[48;2;10;6;26;31;1m<assignment_exp>\033[0m]"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;
        }
        return false;
    }

    bool Parser::pipe_exp(const FailureCase& fc)
    {
        if (conditional_exp(fc))
        {
            while (expect(Lexer::TokenType::reserved, {Lexer::sPipe}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!conditional_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<conditional_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<pipe_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<pipe_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <conditional_exp> {'|' \033[48;2;10;6;26;31;1m<conditional_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;
        }
        return false;
    }

    bool Parser::conditional_exp(const FailureCase& fc)
    {
        if (logical_exp(fc))
        {
            if (expect(Lexer::TokenType::symbol, {Lexer::sQuestionMark}))
            {
                // Expression conditionExp{};
                // m_ast.mkLocalExpNode(conditionExp);

                // U8string t_op{m_currentTokenData.get().token};

                std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

                if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <logical_exp> ['?' \033[48;2;10;6;26;31;1m'('\033[0m  <expression>|<jump_statement> [':' <expression>|<jump_statement>] ')']"), additionalLog();
                }
                else
                optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (jump_statement())
                {
                    
                }
                else if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m or a \033[48;2;10;6;26;32;1m<jump_statement>\033[0m", "to be used as the expression that gets executed if the condition is true in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <logical_exp> ['?' '('  \033[48;2;10;6;26;31;1m<expression>|<jump_statement>\033[0m [':' <expression>|<jump_statement>] ')']"), additionalLog();
                }

                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                if (expect(Lexer::TokenType::symbol, {Lexer::sColon}))
                {
                    if (jump_statement())
                    {
                        
                    }
                    else if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m or a \033[48;2;10;6;26;32;1m<jump_statement>\033[0m", "to be used as the expression that gets executed if the condition is false in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax"}))
                    {
                        afterTokenLog(), spaceLog();
                        m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   <logical_exp> ['?' '('  <expression>|<jump_statement> [':' \033[48;2;10;6;26;31;1m<expression>|<jump_statement>\033[0m] ')']"), additionalLog();
                    }
                }
                
                if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog();
                    if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());
                    
                    spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<conditional_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <logical_exp> ['?' '('  <expression>|<jump_statement> [':' <expression>|<jump_statement>] \033[48;2;10;6;26;31;1m')'\033[0m]"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkTenaryOpNode(conditionExp, t_op, leftExp, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }

    bool Parser::logical_exp(const FailureCase& fc)
    {
        if (equality_exp(fc))
        {
            while (expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!equality_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<equality_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<logical_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<logical_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <equality_exp> {'='|'!='|'≠' \033[48;2;10;6;26;31;1m<equality_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;
        }
        return false;
    }

    bool Parser::equality_exp(const FailureCase& fc)
    {
        if (relational_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!relational_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<relational_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<equality_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<equality_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <relational_exp> {'='|'!='|'≠' \033[48;2;10;6;26;31;1m<relational_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;    
        }
        return false;
    }

    bool Parser::relational_exp(const FailureCase& fc)
    {
        if (shift_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!shift_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<shift_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<relational_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<relational_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <shift_exp> {'<'|'>'|'<='|'>='|'≤'|'≥' \033[48;2;10;6;26;31;1m<shift_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }
    
    bool Parser::shift_exp(const FailureCase& fc)
    {
        if (additive_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sShiftLeft, Lexer::sShiftRight}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);

                // U8string b_op{m_currentTokenData.get().token};

                if (!additive_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<additive_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<shift_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<shift_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <additive_exp> {'<<'|'>>'|'<<<'|'>>>' \033[48;2;10;6;26;31;1m<additive_exp>\033[0m}"), additionalLog();
                }
                
                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;    
        }
        return false;
    }
    
    bool Parser::additive_exp(const FailureCase& fc)
    {
        if (multiplicative_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sPlus, Lexer::sMinus}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);
                
                // U8string b_op{m_currentTokenData.get().token};

                if (!multiplicative_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<multiplicative_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<aditive_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<additive_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <multiplicative_exp> {'+'|'-' \033[48;2;10;6;26;31;1m<multiplicative_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, m_currentTokenData.get().absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }

    bool Parser::multiplicative_exp(const FailureCase& fc)
    {
        if (exponention_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sMultiply, Lexer::sDivide, Lexer::sRemainder}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);
                
                // U8string b_op{m_currentTokenData.get().token};
                
                if (!exponention_exp(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<exponention_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<multiplicative_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<multiplicative_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <exponention_exp> {'*'|'/'|'%'|'/%'|'÷' \033[48;2;10;6;26;31;1m<exponention_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }

    bool Parser::exponention_exp(const FailureCase& fc)
    {
        if (member_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sExponention}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);
                
                // U8string b_op{m_currentTokenData.get().token};
                
                if (!member_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<member_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<exponention_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<exponention_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <member_exp> {'^' \033[48;2;10;6;26;31;1m<member_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }   
            return true;
        }
        return false;
    }

    bool Parser::member_exp(const FailureCase& fc)
    {
        if (prefix_exp(fc))
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf, Lexer::sArrow}))
            {
                // Expression leftExp{};
                // m_ast.mkLocalExpNode(leftExp);
                
                // U8string b_op{m_currentTokenData.get().token};

                if (!prefix_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<prefix_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<member_exp>\033[0m's BNF syntax"}))
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<member_exp>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <prefix_exp> {'.'|'->' \033[48;2;10;6;26;31;1m<prefix_exp>\033[0m}"), additionalLog();
                }

                // Expression rightExp{};
                // m_ast.mkLocalExpNode(rightExp);

                // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                // m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            return true;
        }
        return false;
    }

    bool Parser::prefix_exp(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sMinus, Lexer::sNot, Lexer::sNot, Lexer::sIncrement, Lexer::sDecrement, Lexer::sCommercialAt, Lexer::sAmperSand, Lexer::sDollarSign}) or expect(Lexer::TokenType::reserved, {Lexer::rNot, Lexer::rNot, Lexer::rOdd, Lexer::rMv, Lexer::rCp, Lexer::rImut_lref, Lexer::rMut_lref}))
        {
            // U8string u_op{m_currentTokenData.get().token};

            if (!prefix_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<prefix_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<prefix_exp>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<prefix_exp>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   ('-'|'!'|'+'|'odd'|'++'|'--'|'$'|'&'|'mv'|'cp'|'mut_lref'|'imut_lref' \033[48;2;10;6;26;31;1m<prefix_exp>\033[0m)|<postfix_exp>"), additionalLog();
            }

            // Expression rightExp{};
            // m_ast.mkLocalExpNode(rightExp);

            // LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

            // m_ast.mkUprefixOpNode(u_op, rightExp, locFinder);
            
            return true;
        }
        else if (postfix_exp(fc))
        return true;
        
        return false;
    }

    bool Parser::postfix_exp(const FailureCase& fc)
    {
        if (primary_exp(fc))
        {
            while (true)
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sIncrement, Lexer::sDecrement}))
                {
                    // Expression leftExp{};
                    // m_ast.mkLocalExpNode(leftExp);

                    // auto loc = leftExp.getExpLocation();

                    // LocationFinder locFinder{ makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn) };
                    // U8string u_op{tokenData.token};
                    // m_ast.mkUpostfixOpNode(leftExp, u_op, locFinder);
                }
                else if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}))
                {
                    auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                    // Expression leftExp{};
                    // m_ast.mkLocalExpNode(leftExp);

                    // auto loc = leftExp.getExpLocation();

                    expArg_list();

                    // LocationFinder locFinder{ makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn) };
                    // m_ast.mkUpostfixOpNode(leftExp, m_ast._getExpressionList(), locFinder);

                    if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<postfix_exp>\033[0m's BNF syntax"}))
                    {
                        afterTokenLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                        m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<postfix_exp>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   <primary_exp> {'++'|'--'|'('[<expArg_list>]\033[48;2;10;6;26;31;1m')'\033[0m'['[<expArg_list>]']'}"), additionalLog();
                    }
                }
                else if (expect(Lexer::TokenType::symbol, {Lexer::sLsquare}))
                {
                    auto savedLsquareTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                    // Expression leftExp{};
                    // m_ast.mkLocalExpNode(leftExp);

                    // auto loc = leftExp.getExpLocation();

                    expArg_list();

                    // LocationFinder locFinder{ makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn) };
                    // m_ast.mkUpostfixOpNode(leftExp, m_ast._getExpressionList(), locFinder);

                    if (!expect(Lexer::TokenType::symbol, {Lexer::sRsquare}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<postfix_exp>\033[0m's BNF syntax"}))
                    {
                        afterTokenLog(), missingBraceLog(savedLsquareTokenDataIndex), spaceLog();
                        m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<postfix_exp>\033[0m's BNF syntax:"), additionalLog();
                        m_log.write("   <primary_exp> {'++'|'--'|'('[<expArg_list>]')''['[<expArg_list>]\033[48;2;10;6;26;31;1m']'\033[0m}"), additionalLog();
                    }
                }
                else
                break;
            }
            return true;
        }
        return false;
    }

    bool Parser::primary_exp(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}))
        {
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (!expression(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expression>\033[0m", "inside the parenthesis operator"}))
            afterTokenLog();
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "when using the parenthesis operator"}))
            missingBraceLog(savedLparenTokenDataIndex), afterTokenLog();

            return true;
        }
        else if (r_l() or block() or special_ident() or object_exp() or function_exp())
        {
            return true;
        }
        else if (expect(Lexer::TokenType::literal, {}, fc))
        {
            // LocationFinder locFinder{makeLocFinder(m_currentTokenData.get().line, m_currentTokenData.get().relativeColumn, m_currentTokenData.get().absoluteColumn)};
            // U8string literalConst{m_currentTokenData.get().token};

            // U8string literalType{m_currentTokenData.get().literalType};
            
            // m_ast.mkLiteralNode(literalConst, literalType, locFinder);

            return true;
        }
        return false;
    }


    bool Parser::reserved_exp_ident(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sScopeResolution}))
        {
            if (expect(Lexer::TokenType::reserved, {Lexer::rInput, Lexer::rOutput, Lexer::rTcast, Lexer::rBcast, Lexer::rPanic, Lexer::rNone, Lexer::rNullptr}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<reserved_exp_ident>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<special_ident>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   ['::'] \033[48;2;10;6;26;31;1m'input'|'output'|'tcast'|'bcast'|'panic'|'none'|'nullptr'\033[0m [<tal_encapsulation>]"), additionalLog();
            }
            else
            {
                tal_encapsulation();
            }
            return true;
        }
        else if (expect(Lexer::TokenType::reserved, {Lexer::rInput, Lexer::rOutput, Lexer::rTcast, Lexer::rBcast, Lexer::rPanic, Lexer::rNone, Lexer::rNullptr}, fc))
        {
            tal_encapsulation();

            return true;
        }
        return false;
    }

    bool Parser::arg(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sHashedLparen}, fc))
        {
            auto savedHashedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            expArg_list();

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<arg>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(savedHashedLparenTokenDataIndex), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<arg>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '#('[<expArg_list>]\033[48;2;10;6;26;36;1m')'\033[0m"), additionalLog();
            }
            return true;
        }
        return false;
    }

    bool Parser::fromType(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rFromType}) or expect(Lexer::TokenType::symbol, {Lexer::sCommercialAt}, fc))
        {
            if (expect(Lexer::TokenType::identifier, {}))
            {
                tal_encapsulation();
            }
            else if (_typeof())
            {}
            else if (function_signature(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<typeof>\033[0m, an \033[48;2;10;6;26;32;1m<identifier>\033[0m or a \033[48;2;10;6;26;32;1m<function_signature>\033[0m", "in \033[48;2;10;6;26;36;1m<fromType>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<fromType>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fromType'|'@' \033[48;2;10;6;26;31;1m<typeof>|<identifier>[<tal_encapsulation>]|<function_signature>\033[0m"), additionalLog();
            }
            return true;
        }
        return false;
    }

    bool Parser::special_ident(const FailureCase& fc)
    {
        bool result{};
        if (expect(Lexer::TokenType::symbol, {Lexer::sScopeResolution}))
        {
            if (expect(Lexer::TokenType::identifier, {}))
            {
                tal_encapsulation();
            }
            else if (fromType(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<identifier>\033[0m or a \033[48;2;10;6;26;32;1m<fromType>\033[0m", "in \033[48;2;10;6;26;36;1m<special_ident>\033[0m's BNF syntax"}))
            {
            }
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<special_ident>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   ['::'] \033[48;2;10;6;26;31;1m<identifier>[<tal_encapsulation>]|<fromType>\033[0m {'::' <identifier>[<tal_encapsulation>]|<fromType>}"), additionalLog();
            }

            result = true;
        }
        else if (expect(Lexer::TokenType::identifier, {}))
        {
            tal_encapsulation();
            result = true;
        }
        else if (fromType(fc))
        {
            result = true;
        }
        
        if (result)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sScopeResolution}))
            {
                if (expect(Lexer::TokenType::identifier, {}))
                {
                    tal_encapsulation();
                }
                else if (fromType(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<identifier>\033[0m or a \033[48;2;10;6;26;32;1m<fromType>\033[0m", "in \033[48;2;10;6;26;36;1m<special_ident>\033[0m's BNF syntax"}))
                {
                }
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<special_ident>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   ['::'] <identifier>[<tal_encapsulation>]|<fromType> {'::' \033[48;2;10;6;26;31;1m<identifier>[<tal_encapsulation>]|<fromType>\033[0m}"), additionalLog();
                }
            }
            return true;
        }
        return false;
    }

    bool Parser::object_exp(const FailureCase& fc)
    {
        if (m_hasType or prefix_type(fc))
        {
            m_hasType = false;
            if (!initializer(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<initializer>\033[0m", "in \033[48;2;10;6;26;36;1m<object_exp>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<object_exp>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   <prefix_type>\033[48;2;10;6;26;31;1m<initializer>\033[0m"), additionalLog();
            }
            
            return true;
        }
        return false;
    }

    bool Parser::function_exp(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rFn}, fc))
        {
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' \033[48;2;10;6;26;31;1m'('\033[0m[<func_param_list>]')' [<optPrefix_type>] <block>"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            func_param_list();
            
            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax"}))
            {
                afterTokenLog();
                if (optional_savedLparenTokenDataIndex.has_value()) missingBraceLog(optional_savedLparenTokenDataIndex.value());

                spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' '('[<func_param_list>]\033[48;2;10;6;26;31;1m')'\033[0m [<optPrefix_type>] <block>"), additionalLog();
            }

            // Type giveType{};
            
            if (optPrefix_type()){}

            if (!block(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<block>\033[0m", "in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<function_exp>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'fn' '('[<func_param_list>]')' [<optPrefix_type>] \033[48;2;10;6;26;31;1m<block>\033[0m"), additionalLog();
            }
            // m_ast.mkFunctionDefNode(giveType, funcIdent, m_ast._getParameterList(), m_ast.getBlockNode());
            // m_ast.makeFuncDeclarationNode();
            
            return true;
        }
        return false;
    }

    bool Parser::expArg_list(const FailureCase& fc)
    {
        bool result{};
        if ((result = expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end})))
        {
            
        }
        else if ((result = param_expArg_assoc()))
        {

        }
        else if ((result = assignment_exp(fc)))
        {

        }
        
        if (result)
        {
            // ExpressionList expList{};
            // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));

            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end}))
                {
                }
                else if (param_expArg_assoc())
                {
                }
                else if (assignment_exp(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m, \033[48;2;10;6;26;32;1m_v\033[0m, \033[48;2;10;6;26;32;1mv_\033[0m or a \033[48;2;10;6;26;32;1m<param_expArg_assoc>\033[0m", "in \033[48;2;10;6;26;36;1m<expArg_list>\033[0m's BNF syntax"}))
                {
                }
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<expArg_list>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'default'|<assignment_exp>|'_v'|'v_'|<param_expArg_assoc> {',' \033[48;2;10;6;26;31;1m'default'|<assignment_exp>|'_v'|'v_'|<param_expArg_assoc>\033[0m}"), additionalLog();
                }

                // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));
            }

            // m_ast._getExpressionList() = std::move(expList);
            return true;
        }
            
        return false;
    }

    bool Parser::param_expArg_assoc(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf}, fc))
        {
            if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'\033[48;2;10;6;26;31;1m<identifier>\033[0m '::=' 'default'|<assignment_exp>|'_v'|'v_'"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sInitAssign}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> \033[48;2;10;6;26;31;1m'::='\033[0m 'default'|<assignment_exp>|'_v'|'v_'"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end}))
            {}
            else if (assignment_exp(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m, \033[48;2;10;6;26;32;1m_v\033[0m or \033[48;2;10;6;26;32;1mv_\033[0m", "in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_expArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> '::=' \033[48;2;10;6;26;31;1m'default'|<assignment_exp>|'_v'|'v_'\033[0m"), additionalLog();
            }
            
            return true;
        }
        return false;
    }

    bool Parser::tal_encapsulation(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sLessthan}, fc))
        {
            auto savedLangleTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            typeArg_list();

            if (!expect(Lexer::TokenType::symbol, {Lexer::sGreaterthan}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<tal_encapsulation>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(savedLangleTokenDataIndex), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<tal_encapsulation>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '<'[<typeArg_list>]\033[48;2;10;6;26;31;1m'>'\033[0m"), additionalLog();
            }
            return true;
        }
        return false;
    }

    bool Parser::typeArg_list(const FailureCase& fc)
    {
        bool result{};
        if ((result = expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end})))
        {
            
        }
        else if ((result = param_typeArg_assoc()))
        {

        }
        else if ((result = optPrefix_type(fc)))
        {

        }

        if (result)
        {
            // ExpressionList expList{};
            // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));

            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end}))
                {
                    
                }
                else if (param_typeArg_assoc())
                {
                    
                }
                else if (optPrefix_type(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, a \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m, \033[48;2;10;6;26;32;1m_v\033[0m, \033[48;2;10;6;26;32;1mv_\033[0m or a \033[48;2;10;6;26;32;1m<param_typeArg_assoc>\033[0m", "in \033[48;2;10;6;26;36;1m<typeArg_list>\033[0m's BNF syntax"}))
                {
                }
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<typeArg_list>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   'default'|<optPrefix_type>|'_v'|'v_'|<param_typeArg_assoc> {',' \033[48;2;10;6;26;31;1m'default'|<optPrefix_type>|'_v'|'v_'|<param_typeArg_assoc>\033[0m}"), additionalLog();
                }
                

                // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));
            }

            // m_ast._getExpressionList() = std::move(expList);
            return true;
        }
            
        return false;
    }

    bool Parser::param_typeArg_assoc(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf}, fc))
        {
            if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'\033[48;2;10;6;26;31;1m<identifier>\033[0m '::=' 'default'|<optPrefix_type>|'_v'|'v_'"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sInitAssign}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> \033[48;2;10;6;26;31;1m'::='\033[0m 'default'|<optPrefix_type>|'_v'|'v_'"), additionalLog();
            }

            if (expect(Lexer::TokenType::reserved, {Lexer::rDefault, Lexer::rVarg_start, Lexer::rVarg_end}))
            {}
            else if (optPrefix_type(FailureCase{true, "Expected \033[48;2;10;6;26;32;1mdefault\033[0m, an \033[48;2;10;6;26;32;1m<optPrefix_type>\033[0m, \033[48;2;10;6;26;32;1m_v\033[0m or \033[48;2;10;6;26;32;1mv_\033[0m", "in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_typeArg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> '::=' \033[48;2;10;6;26;31;1m'default'|<optPrefix_type>|'_v'|'v_'\033[0m"), additionalLog();
            }
            
            return true;
        }
        return false;
    }

    bool Parser::r_l(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rR_l}, fc))
        {
            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' \033[48;2;10;6;26;31;1m'('\033[0m<r_l_arg_list>')' '('<expArg_list>')'"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (!r_l_arg_list(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<r_l_arg_list>\033[0m", "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' '('\033[48;2;10;6;26;31;1m<r_l_arg_list>\033[0m')' '('<expArg_list>')'"), additionalLog();
            }

            // m_ast.makeExpNode();
            // auto exp = std::move(m_ast.getExpNode());

            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(optional_savedLparenTokenDataIndex.value_or(0)), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' '('<r_l_arg_list>\033[48;2;10;6;26;31;1m')'\033[0m '('<expArg_list>')'"), additionalLog();

                optional_savedLparenTokenDataIndex.reset();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' '('<r_l_arg_list>')' \033[48;2;10;6;26;31;1m'('\033[0m<expArg_list>')'"), additionalLog();
            }
            else
            optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            
            if (!expArg_list(FailureCase{true, "Expected an \033[48;2;10;6;26;32;1m<expArg_list>\033[0m", "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' '('<r_l_arg_list>')' '('\033[48;2;10;6;26;31;1m<expArg_list>\033[0m')'"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), missingBraceLog(optional_savedLparenTokenDataIndex.value_or(0)), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   'r_l' '('<r_l_arg_list>')' '('<expArg_list>\033[48;2;10;6;26;31;1m')'\033[0m"), additionalLog();
            }

            // m_ast.mkR_And_L_Exp_Node(relationalOp, logicalOp, exp, m_ast._getExpressionList());

            // /*label_name*/ end:
            return true;
        }
        return false;
    }

    bool Parser::r_l_arg_list(const FailureCase& fc)
    {
        bool result{};
        if ((result = expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual, Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo})))
        {

        }
        else if ((result = expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor})))
        {
            
        }
        else if ((result = assignment_exp(fc)))
        {}
        
        if (result)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}))
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual, Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo}))
                {

                }
                else if (expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor}))
                {
                    
                }
                else if (param_r_l_arg_assoc())
                {}
                else if (assignment_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<relational_op>\033[0m, a \033[48;2;10;6;26;32;1m<logical_op>\033[0m, an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m or a \033[48;2;10;6;26;32;1m<param_r_l_arg_assoc>\033[0m", "in \033[48;2;10;6;26;36;1m<r_l_arg_list>\033[0m's BNF syntax"}))
                {}
                else
                {
                    afterTokenLog(), spaceLog();
                    m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<r_l_arg_list>\033[0m's BNF syntax:"), additionalLog();
                    m_log.write("   <relational_op>|<logical_op>|<assignment_exp>|<param_r_l_arg_assoc> {',' \033[48;2;10;6;26;31;1m<relational_op>|<logical_op>|<assignment_exp>|<param_r_l_arg_assoc>\033[0m}"), additionalLog();
                }
            }
            return true;
        }
        return false;
    }

    bool Parser::param_r_l_arg_assoc(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf}, fc))
        {
            if (!expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_r_l_arg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_r_l_arg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'\033[48;2;10;6;26;31;1m<identifier>\033[0m '::=' <relational_op>|<logical_op>|<assignment_exp>"), additionalLog();
            }

            if (!expect(Lexer::TokenType::symbol, {Lexer::sInitAssign}, FailureCase{true, {}, "in \033[48;2;10;6;26;36;1m<param_r_l_arg_assoc>\033[0m's BNF syntax"}))
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_r_l_arg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> \033[48;2;10;6;26;31;1m'::='\033[0m <relational_op>|<logical_op>|<assignment_exp>"), additionalLog();
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual, Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessThan_equalTo, Lexer::sGreaterThan_equalTo, Lexer::sAlternateLessThan_equalTo, Lexer::sAlternateGreaterThan_equalTo}))
            {

            }
            else if (expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor}))
            {
                
            }
            else if (assignment_exp(FailureCase{true, "Expected a \033[48;2;10;6;26;32;1m<relational_op>\033[0m, a \033[48;2;10;6;26;32;1m<logical_op>\033[0m or an \033[48;2;10;6;26;32;1m<assignment_exp>\033[0m", "in \033[48;2;10;6;26;36;1m<r_l_arg_list>\033[0m's BNF syntax"}))
            {}
            else
            {
                afterTokenLog(), spaceLog();
                m_log.write("   The missing entity is highlighted below in \033[48;2;10;6;26;36;1m<param_r_l_arg_assoc>\033[0m's BNF syntax:"), additionalLog();
                m_log.write("   '.'<identifier> '::=' \033[48;2;10;6;26;31;1m<relational_op>|<logical_op>|<assignment_exp>\033[0m"), additionalLog();
            }
            return true;
        }
        return false;
    }


    std::string Parser::mapTokenToCategory(const Lexer::TokenData* x)
    {
        const Lexer::TokenData& tokenData = x == nullptr? m_currentTokenData.get() : *x;
        
        if (tokenData.isReserved)
        {
            return std::string{" reserved-identifier"};
        }
        else if (tokenData.isIdentifier)
        {
            return std::string{" identifier"};
        }
        else if (!tokenData.literalType.empty())
        {
            if (tokenData.token.ends_with('"'_u8))
            return " string-literal";
            else if (tokenData.token.ends_with('\''_u8))
            return " character-literal";
            else if (u8_misc::isBasicLatinDigit(tokenData.token.front()))
            {
                if (tokenData.token.contains('@'_u8) or tokenData.token.contains('.'_u8))
                return " real-number-literal";
                else
                return " integer-number-literal";
            }
            else
            return " boolean-literal";
        }
        return std::string{};
    }


    bool Parser::errorRecoveryStep_altruism(std::list<Entity> next_expected_token)
    {
        auto result = false;

        for (auto &&i : next_expected_token)
        {
            std::visit([&]<typename T>(T& entity){
                if constexpr (std::is_same_v<T, ProductionRule>)
                {
                    switch (entity)
                    {
                        case ProductionRule::expression:
                        {
                            if (!m_currentTokenData.get().literalType.empty() or m_currentTokenData.get().isIdentifier)
                            return true;
                            else if (m_currentTokenData.get().isReserved)
                            {
                                if (m_currentTokenData.get().token == Lexer::rR_l or m_currentTokenData.get().token == Lexer::rFn)
                                return true;
                            }
                            else if (m_currentTokenData.get().isSymbol)
                            {
                                if (m_currentTokenData.get().token == Lexer::sLparen or m_currentTokenData.get().token == Lexer::sHashedLparen)
                                return true;
                            }
                        }
                        case ProductionRule::statement:
                        {

                        }
                    }
                }
                else
                {
                    if (m_currentTokenData.get().token == entity)
                    return true;
                }
            }, i);
        }
        return false;
    }


    bool Parser::expect(Lexer::TokenType tokenType, std::vector<U8string_view> tokenStr, const FailureCase& fc)
    {
        m_currentTokenData = m_lexer.getTokenDataList().at(m_currentTokenDataIndex);
    
        switch (tokenType)
        {
        case Lexer::TokenType::reserved:
        {
            if (!m_isEndofTokenList)
            {
                if (m_currentTokenData.get().isReserved)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_lexer.getTokenDataList().at(m_currentTokenDataIndex).token == i)
                        return nextTokenIndex(), true;
                    }
                }
            }

            if (fc.shouldReport)
            {
                std::string preposition = m_isEndofTokenList? "after" : "before";
                
                if (fc.optErrorMsg.has_value())
                m_log.write(*fc.optErrorMsg, ' ', preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
                else
                m_log.write("Expected reserved-identifier \033[48;2;10;6;26;32;1m", tokenStr, "\033[0m ", preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
    
                start_log();
            }

            break;
        }
        case Lexer::TokenType::identifier:
        {
            if (!m_isEndofTokenList)
            {
                if (m_currentTokenData.get().isIdentifier)
                return nextTokenIndex(), true;
            }

            if (fc.shouldReport)
            {
                std::string preposition = m_isEndofTokenList? "after" : "before";

                if (fc.optErrorMsg.has_value())
                m_log.write(*fc.optErrorMsg, ' ', preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
                else
                m_log.write("Expected an \033[48;2;10;6;26;32;1m<identifier>\033[0m ", preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
    
                start_log();
            }
            
            break;
        }
        case Lexer::TokenType::literal:
        {
            if (!m_isEndofTokenList)
            {
                if (!m_currentTokenData.get().literalType.empty())
                {
                    if (tokenStr.empty())
                    return nextTokenIndex(), true;
                    else
                    {
                        for (auto &&i : tokenStr)
                        {
                            if (i == Lexer::lStr)
                            {
                                if (m_currentTokenData.get().token.ends_with('"'_u8))
                                return nextTokenIndex(), true;
                            }
                            else if (i == Lexer::lChr)
                            {
                                if (m_currentTokenData.get().token.ends_with('\''_u8))
                                return nextTokenIndex(), true;
                            }
                            else if (i == Lexer::lReal)
                            {
                                if (u8_misc::isBasicLatinDigit(m_currentTokenData.get().token.front()) and (m_currentTokenData.get().token.contains('@'_u8) or m_currentTokenData.get().token.contains('.'_u8)))
                                return nextTokenIndex(), true;
                            }
                            else if (i == Lexer::lInt)
                            {
                                if (u8_misc::isBasicLatinDigit(m_currentTokenData.get().token.front()) and !(m_currentTokenData.get().token.contains('@'_u8) or m_currentTokenData.get().token.contains('.'_u8)))
                                return nextTokenIndex(), true;
                            }
                            else if (i == Lexer::lBool)
                            {
                                if (m_currentTokenData.get().literalType.front() == 'b'_u8)
                                return nextTokenIndex(), true;
                            }
                            
                        }
                    }
                }
            }

            if (fc.shouldReport)
            {
                std::string preposition = m_isEndofTokenList? "after" : "before";
                std::string literalType{};
                std::string artical{'a'};

                if (!tokenStr.empty())//there is no case handling for all the values contined in [tokenStr], only the first value
                {
                    if (tokenStr.front() == Lexer::lStr)
                    literalType = "string";
                    else if (tokenStr.front() == Lexer::lReal)
                    literalType = "real_number";
                    else if (tokenStr.front() == Lexer::lInt)
                    literalType = "integer_number", artical = "an";
                    else if (tokenStr.front() == Lexer::lChr)
                    literalType = "character";
                    else if (tokenStr.front() == Lexer::lBool)
                    literalType = "boolean";
                }
                
                if (fc.optErrorMsg.has_value())
                m_log.write(*fc.optErrorMsg, ' ', preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
                else
                m_log.write("Expected ", artical," \033[48;2;10;6;26;32;1m<", literalType, "_literal>\033[0m ", preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
                
                start_log();
            }
            break;
        }
        case Lexer::TokenType::symbol:
        {
            if (!m_isEndofTokenList)
            {
                if (m_currentTokenData.get().isSymbol)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_currentTokenData.get().token == i)
                        return nextTokenIndex(), true;
                    }
                }
            }
            
            if (fc.shouldReport)
            {
                std::string preposition = m_isEndofTokenList? "after" : "before";
                
                if (fc.optErrorMsg.has_value())
                m_log.write(*fc.optErrorMsg, ' ', preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);
                else
                m_log.write("Expected \033[48;2;10;6;26;32;1m", tokenStr, "\033[0m ", preposition, mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg);

                start_log();
            }

            break;
        }
        case Lexer::TokenType::_miscellany:
        {
            if (fc.optErrorMsg.has_value())
            m_log.write(*fc.optErrorMsg, " instead of", mapTokenToCategory(), " \033[48;2;10;6;26;31;1m", m_currentTokenData.get().token, "\033[0m ", fc.extraErrMsg), start_log();
            else
            std::cout << "In fucntion Nc::Parser::expect, parameter function fc.optErrorMsg.has_value() is false under case Lexer::TokenType::_miscellany", panic(1234);
            
            nextTokenIndex();
            break;
        }
        default:
            std::cout << "In fucntion Nc::Parser::expect variable tokenTypeToMatch contains a value that was not expected", std::exit(1);
        }

        return false;
    }
    
    void Parser::nextTokenIndex(std::size_t amount)
    {
        if ((m_currentTokenDataIndex += amount) >= m_lexer.getTokenDataList().size())
        m_currentTokenDataIndex = m_lexer.getTokenDataList().size() - 1, m_isEndofTokenList = true;
    }

    void Parser::previousTokenIndex(std::size_t amount)
    {
        int(m_currentTokenDataIndex -= amount) < 0? m_currentTokenDataIndex = 0 : m_currentTokenDataIndex;
    }

    void Parser::start_log()
    {
        m_log.log(true);
        
        const auto& tokenData{ m_currentTokenData.get() };

        if (tokenData.optLine.has_value())
        {
            auto start_line = tokenData.optLine.value();
            auto end_line = tokenData.line;

            m_log.write("   From [\033[1mline\033[0m: \033[48;2;10;6;26;36;1m", start_line, "\033[0m \033[1mcolumn\033[0m: \033[48;2;10;6;26;36;1m", tokenData.relativeColumn, "\033[0m] - [\033[1mline\033[0m: \033[48;2;10;6;26;36;1m", end_line, "\033[0m \033[1mcolumn\033[0m: \033[48;2;10;6;26;36;1m", tokenData.absoluteColumn, "\033[0m]");
            m_log.log();

            U8string lineContents{};
            
            for (auto current_line = start_line; current_line <= end_line; ++current_line)
            {
                std::uint32_t count{};
                for (auto i = m_lexer.getLineNumToFilePos()[current_line], size = (std::uint32_t)m_lexer.getFileBuffer().size(); i < size; ++i)
                {
                    if (m_lexer.getFileBuffer()[i] == '\n'_u8)
                    break;

                    ++count;
                }
                lineContents.assign(m_lexer.getFileBuffer(), m_lexer.getLineNumToFilePos()[current_line], count);

                m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();
                m_log.write("                │\033[1m");

                if (current_line == start_line)
                {
                    auto temp = lineContents.size() - (std::size_t)(tokenData.relativeColumn - 1);
                    m_log.write(U8string{lineContents.size(), ' '_u8}.replace(tokenData.relativeColumn - 1, temp, temp, "⇧"_u8));
                }
                else if (current_line == end_line)
                {
                    auto n_of_arrows = tokenData.absoluteColumn - 1uz;
                    auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(0uz, n_of_arrows, n_of_arrows, "⇧"_u8);

                    m_log.write(arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1));
                }
                else
                {
                    auto size = lineContents.size();
                    m_log.write(U8string{size, ' '_u8}.replace(0uz, size, size, "⇧"_u8));
                }

                m_log.write("\033[0m"), m_log.log();
            }
        }
        else
        {
            m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", tokenData.line, "\033[0m from \033[1mcolumn\033[0m: [\033[48;2;10;6;26;36;1m", tokenData.relativeColumn, "\033[0m - \033[48;2;10;6;26;36;1m", tokenData.absoluteColumn, "\033[0m]");
            m_log.log();

            U8string lineContents{};
            std::uint32_t count{};

            for (auto i = m_lexer.getLineNumToFilePos()[tokenData.line], size = (std::uint32_t)m_lexer.getFileBuffer().size(); i < size; ++i)
            {
                if (m_lexer.getFileBuffer()[i] == '\n'_u8)
                break;

                ++count;
            }
            lineContents.assign(m_lexer.getFileBuffer(), m_lexer.getLineNumToFilePos()[tokenData.line], count);

            m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();

            auto n_of_arrows = misc::safe_unsigned_sub(tokenData.absoluteColumn, tokenData.relativeColumn);
            auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(tokenData.relativeColumn - 1uz, n_of_arrows, n_of_arrows, "⇧"_u8);

            m_log.write("                │\033[1m", arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1), "\033[0m");

            m_log.log();
        }

        //ensures ast nodes aren't made anymore
        // m_ast.setDeadZoneFlag();
    }

    void Parser::start_multiTokenlog(std::size_t tokenDataIndexStart)//fix later
    {
        m_log.log(true);

        const auto& begin{ m_lexer.getTokenDataList().at(tokenDataIndexStart) };
        const auto& end{ m_lexer.getTokenDataList().at(m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1) };

        if (begin.line == end.line)
        {
            m_log.writews("  line:", begin.line, "from column: [", begin.relativeColumn, '-', end.absoluteColumn, ']');
            m_log.log();

            U8string lineContents{};
            for (auto i = m_lexer.getFileBuffer().begin() + m_lexer.getLineNumToFilePos().at(begin.line); i != m_lexer.getFileBuffer().end(); i.operator++())
            {
                if (*i == '\n'_u8)
                break;

                lineContents.push_back(*i);
            }
            m_log.writews("  line contents:" , lineContents);
            m_log.log();

            if (end.absoluteColumn - begin.relativeColumn == 1)
            m_log.writews("               :", U8string{lineContents.size(), ' '_u8}.insert(begin.relativeColumn - 1, end.absoluteColumn - begin.relativeColumn, "⇑"_u8));
            else
            m_log.writews("               :", U8string{lineContents.size(), ' '_u8}.insert(begin.relativeColumn - 1, end.absoluteColumn - begin.relativeColumn, "↑"_u8));

            m_log.log();
        }
        else
        {
            auto getFirstTokenDataOnLine = [&] (std::uint32_t line_in_question) -> const auto&
            {
                for (auto i = tokenDataIndexStart; i < m_currentTokenDataIndex; ++i)
                {
                    if (m_lexer.getTokenDataList().at(i).line == line_in_question)
                    return m_lexer.getTokenDataList().at(i);
                }
                
                return begin;
            };

            auto getLastTokenDataOnLine = [&] (std::uint32_t line_in_question) -> const auto&
            {
                for (auto i = tokenDataIndexStart; i < m_currentTokenDataIndex; ++i)
                {
                    if (m_lexer.getTokenDataList().at(i).line > line_in_question)
                    return m_lexer.getTokenDataList().at(i - 1);
                }
                
                return end;
            };

            m_log.writews("  from line [", begin.line, '-', end.line, ']');
            m_log.log();

            for (auto i = begin.line; i <= end.line; i++)
            {
                m_log.writews("  line:", i, "from column: [", getFirstTokenDataOnLine(i).relativeColumn, '-', getLastTokenDataOnLine(i).absoluteColumn, ']');
                m_log.log();

                U8string lineContents{};
                bool isLineAllWhiteSpace{true};
                for (auto j = m_lexer.getFileBuffer().begin() + m_lexer.getLineNumToFilePos().at(i); j != m_lexer.getFileBuffer().end(); j.operator++())
                {
                    if (*j == '\n'_u8)
                    break;

                    if (!u8_misc::isBasicLatinWhiteSpace(*j))
                    isLineAllWhiteSpace = false;

                    lineContents.push_back(*j);
                }
                m_log.writews("  line contents:" , lineContents);
                m_log.log();

                if (isLineAllWhiteSpace)
                {
                    //popping back the line and column range log
                    m_log.popBack_logEntry(2).writews("  line:", i);
                    m_log.log();

                    m_log.writews("  line contents:" , lineContents);
                    m_log.log();
                    
                    m_log.writews("               :");
                }
                else if (getLastTokenDataOnLine(i).absoluteColumn - getFirstTokenDataOnLine(i).relativeColumn == 1)
                {
                    m_log.writews("               :",
                    U8string{lineContents.size(), ' '_u8}.
                    insert(getFirstTokenDataOnLine(i).relativeColumn - 1, getLastTokenDataOnLine(i).absoluteColumn - getFirstTokenDataOnLine(i).relativeColumn, "⇑"_u8));
                }
                else
                {
                    m_log.writews("               :",
                    U8string{lineContents.size(), ' '_u8}.
                    insert(getFirstTokenDataOnLine(i).relativeColumn - 1, getLastTokenDataOnLine(i).absoluteColumn - getFirstTokenDataOnLine(i).relativeColumn, "↑"_u8));
                }
                m_log.log();
            }   
        }

        //ensures ast nodes aren't made anymore
        // m_ast.setDeadZoneFlag();
    }

    void Parser::afterTokenLog()
    {
        if (m_isEndofTokenList)
        return;

        auto& previousTokenData{ m_lexer.getTokenDataList().at(m_currentTokenDataIndex - 1) };
        spaceLog();

        m_log.write("   Note: meaning it was expected after", mapTokenToCategory(&previousTokenData), " \033[48;2;10;6;26;33;1m", previousTokenData.token, "\033[0m");
        m_log.log();

        m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", previousTokenData.line, "\033[0m from \033[1mcolumn\033[0m: [\033[48;2;10;6;26;36;1m", previousTokenData.relativeColumn, "\033[0m - \033[48;2;10;6;26;36;1m", previousTokenData.absoluteColumn, "\033[0m]");
        m_log.log();

        U8string lineContents{};
        std::uint32_t count{};

        for (auto i = m_lexer.getLineNumToFilePos()[previousTokenData.line], size = (std::uint32_t)m_lexer.getFileBuffer().size(); i < size; ++i)
        {
            if (m_lexer.getFileBuffer()[i] == '\n'_u8)
            break;

            ++count;
        }
        lineContents.assign(m_lexer.getFileBuffer(), m_lexer.getLineNumToFilePos()[previousTokenData.line], count);

        m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();

        auto n_of_arrows = misc::safe_unsigned_sub(previousTokenData.absoluteColumn, previousTokenData.relativeColumn);
        auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(previousTokenData.relativeColumn - 1uz, n_of_arrows, n_of_arrows, "⇧"_u8);

        m_log.write("                │\033[1m", arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1), "\033[0m");

        m_log.log();
    }

    void Parser::missingBraceLog(std::size_t savedTokenBraceDataIndex)
    {
        if (savedTokenBraceDataIndex == 0) return;

        const auto& tokenData{ m_lexer.getTokenDataList().at(savedTokenBraceDataIndex) };
        spaceLog();

        m_log.write("   To match \033[48;2;10;6;26;33;1m", tokenData.token, "\033[0m");
        m_log.log();

        m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", tokenData.line, "\033[0m from \033[1mcolumn\033[0m: [\033[48;2;10;6;26;36;1m", tokenData.relativeColumn, "\033[0m - \033[48;2;10;6;26;36;1m", tokenData.absoluteColumn, "\033[0m]");
        m_log.log();

        U8string lineContents{};
        std::uint32_t count{};

        for (auto i = m_lexer.getLineNumToFilePos()[tokenData.line], size = (std::uint32_t)m_lexer.getFileBuffer().size(); i < size; ++i)
        {
            if (m_lexer.getFileBuffer()[i] == '\n'_u8)
            break;

            ++count;
        }
        lineContents.assign(m_lexer.getFileBuffer(), m_lexer.getLineNumToFilePos()[tokenData.line], count);

        m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();

        auto n_of_arrows = misc::safe_unsigned_sub(tokenData.absoluteColumn, tokenData.relativeColumn);
        auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(tokenData.relativeColumn - 1uz, n_of_arrows, n_of_arrows, "⇧"_u8);

        m_log.write("                │\033[1m", arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1), "\033[0m");

        m_log.log();
    }

    void Parser::spaceLog()
    { m_log.log(); }

    void Parser::additionalLog()
    { m_log.log(); }


    // LocationFinder Parser::makeLocFinder(std::uint32_t lineNumber0, std::uint32_t relativeColumn0, std::uint32_t absoluteColumn1)
    // {
    //     return LocationFinder{ LineNumber_and_LineCount{lineNumber0, m_currentTokenData.get().line - lineNumber0}, ColumnSpan{relativeColumn0, absoluteColumn1} };
    // }

    void Parser::errorOutput()
    { m_log.outputLogEntries("\002(10)\033[5;1mParser Log\033[0m", io::cerr), panic(); }

} // namespace Nc