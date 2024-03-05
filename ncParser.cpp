#include <iterator>

#include "ncParser.h"
#include "drawboxaroundtext.h"

namespace Nc
{
    Parser::Parser(Lexer& lex):
    m_tokenDataList{ std::move(lex.getTokenList()) },
    m_lineNumToFilePos{ std::move(lex.getLineNumToFilePos()) },
    m_fileBuffer{ std::move(lex.getFileBuffer()) }
    {}

    void Parser::parseTokenList()
    {
        ncfile();

        if (!m_log.isLogEmpty())
        errorOutput();
        
        for (auto &&i : m_customTypesList)
        System::coutput.write_endl("customType: ", i);
    }

    void Parser::logBoxStyle(Log::BoxLineStyle s)
    { m_log.changeBoxLineStyle(s); }

    Ast& Parser::getAST()
    { return m_ast; }

    const Ast& Parser::getAST()const
    { return m_ast; }
/*
* when expecting two or more token/token-groups, and there is need for reporting an error:
* [using the logical or]: (with short circuit in mind)
* % if (expect() == Result::success or expect(FailureCase{...}) == Result::success)
* % if (expect(FailureCase{...}) == Result::failure or expect(FailureCase{...}) == Result::failure)
* 
* [using the logical and]: (with short circuit in mind)
* % if (expect(FailureCase{...}) == Result::success and expect(FailureCase{...}) == Result::success)
* % if (expect() == Result::failure and expect(FailureCase{...}) == Result::failure)
*/

    void Parser::ncfile()
    {
        while (true)
        {
            if (m_isEndofTokenList)
            {
                break;
            }
            else if (ncfile_contents() == Result::failure)
            {
                m_log.writews("token [", m_tokenDataList.at(m_currentTokenDataIndex).token, "] was not expected to be found at global namespace scope");
                start_Log();
                // spaceLog();
                // m_log.write("  either a global variable or a function declaration would fine instead"), additionalLog();
                // m_log.write(R"(  | global variable BNF syntax |: <type> <identifier> [<intializier>] { "," <identifier> [<intializer>] } [";"])"), additionalLog();
                // m_log.write("  | function declaration BNF syntax |: <identifier> \"(\" [<parameter_list>] \")\" <type> <block>|\";\""), additionalLog();
                next();
            }
            m_ast.mkNcFileNode();
        }

        m_ast.makeRootNode();
    }

    Parser::Result Parser::ncfile_contents(const FailureCase& fc)
    {
        if (globalVar_decl() == Result::success)
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            return Result::success;
        }
        else if (function_decl() == Result::success)
        {
            return Result::success;
        }
        else if (sumType_decl() == Result::success)
        {
            return Result::success;
        }
        else if (productType_decl() == Result::success)
        {
            return Result::success;
        }
        else if (enum_decl() == Result::success)
        {
            return Result::success;
        }
        else if (type_alias() == Result::success)
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            return Result::success;
        }
        else if (namespace_decl() == Result::success)
        {
            return Result::success;
        }
        
        return Result::failure;
    }
    
    Parser::Result Parser::namespace_decl(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rNamespace}, fc) == Result::success and namespace_ident(fc) == Result::success)
        {

            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success)
            {
                /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                while (true)
                {
                    if (m_isEndofTokenList)
                    {
                        expect(Lexer::TokenType::symbol, {Lexer::sRsemicolonCurly}, FailureCase{true, {}, "to end namespace declaration"});
                        sensibleLog(), missingBraceLog(savedLcurlyTokenDataIndex);
                        break;
                    }
                    else if (ncfile_contents() == Result::failure)
                    {
                        
                        if (expect(Lexer::TokenType::symbol, {Lexer::sRsemicolonCurly}) == Result::success)
                        {
                            break;
                        }
                        else
                        {
                            m_log.writews("token [", m_tokenDataList.at(m_currentTokenDataIndex).token, "] was not expected to be found at [waiting_for_ast_ident] namespace scope");
                            start_Log();
                            // spaceLog();
                            // m_log.write("  either a global variable or a function declaration would fine instead"), additionalLog();
                            // m_log.write(R"(  | global variable BNF syntax |: <type> <identifier> [<intializier>] { "," <identifier> [<intializer>] } [";"])"), additionalLog();
                            // m_log.write("  | function declaration BNF syntax |: <identifier> \"(\" [<parameter_list>] \")\" <type> <block>|\";\""), additionalLog();
                            next();   
                        }   
                    }   
                }
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}) == Result::success)
            {}
            else
            m_log.write("expected either [ ; ] or [ { ] in namespace declaration"), start_Log();

            m_currentNameSpace.assign("global::"_u8str);
            return Result::success;
        }
        else
        previous(m_currentTokenDataIndex - savedTokenDataIndex);

        return Result::failure;
    }

    Parser::Result Parser::namespace_ident(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::identifier, {}, fc) == Result::success)
        {
            m_currentNameSpace += (m_currentTokenData.token + "::"_u8str);

            while (expect(Lexer::TokenType::symbol, {Lexer::sScopeResolution}) == Result::success)
            {
                if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "to be used as a namespace identifier"}) == Result::failure)
                sensibleLog();
                else
                m_currentNameSpace += (m_currentTokenData.token + "::"_u8str);
            }

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::sumType_decl(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rSumType}, fc) == Result::success and expect(Lexer::TokenType::identifier, {}, fc) == Result::success)
        {
            m_customTypesList.push_back(m_currentNameSpace + m_currentTokenData.token);

            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success)
            {
                /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (type() == Result::success)
                {
                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) == Result::success)
                    {
                        if (type(FailureCase{true, "expected a type", "in sumType definition"}) == Result::failure)
                        {
                            sensibleLog(), spaceLog();
                            m_log.write("  | @[missing] denotes what is missing |: \"sumType\" <identifier> \"{\" [<type> {\",\" @<type>}] \"};\""), additionalLog();
                        }
                    }
                }

                if (expect(Lexer::TokenType::symbol, {Lexer::sRsemicolonCurly}, FailureCase{true, {}, "to end sumType definition"}) == Result::failure)
                {
                    sensibleLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"sumType\" <identifier> \"{\" [<type> {\",\" <type>}] @\"};\""), additionalLog();
                }
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}) == Result::success)
            {}
            else
            m_log.write("expected either [ ; ] denoting a sumType declaration or [ { ] denoting the start of a sumType definition"), start_Log();

            return Result::success;
        }
        else
        previous(m_currentTokenDataIndex - savedTokenDataIndex);

        return Result::failure;
    }

    Parser::Result Parser::productType_decl(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rProductType}, fc) == Result::success and expect(Lexer::TokenType::identifier, {}, fc) == Result::success)
        {
            m_customTypesList.push_back(m_currentNameSpace + m_currentTokenData.token);

            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success)
            {
                /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (type() == Result::success)
                {
                    if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in productType definition"}) == Result::failure)
                    {
                        sensibleLog(), spaceLog();
                        m_log.write("  | @[missing] denotes what is missing |: \"productType\" <identifier> \"{\" [<type> @<identifier> {\",\" <type> <identifier>}] \"};\"");
                        additionalLog();
                    }

                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) == Result::success)
                    {
                        if (type(FailureCase{true, "expected a type", "in productType declaration"}) == Result::failure)
                        {
                            sensibleLog(), spaceLog();
                            m_log.write("  | @[missing] denotes what is missing |: \"productType\" <identifier> \"{\" [<type> <identifier> {\",\" @<type> <identifier>}] \"};\"");
                            additionalLog();
                        }

                        if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in productType declaration"}) == Result::failure)
                        {
                            sensibleLog(), spaceLog();
                            m_log.write("  | @[missing] denotes what is missing |: \"productType\" <identifier> \"{\" [<type> <identifier> {\",\" <type> @<identifier>}] \"};\"");
                            additionalLog();
                        }
                    }
                }

                if (expect(Lexer::TokenType::symbol, {Lexer::sRsemicolonCurly}, FailureCase{true, {}, "to end productType definition"}) == Result::failure)
                {
                    sensibleLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"productType\" <identifier> \"{\" [<type> <identifier> {\",\" <type> <identifier>}] @\"};\"");
                    additionalLog();
                }
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success)
            {}
            else
            m_log.write("expected either [ ; ] denoting a productType declaration or [ { ] denoting the start of a productType definition"), start_Log();
            
            return Result::success;
        }
        else
        previous(m_currentTokenDataIndex - savedTokenDataIndex);

        return Result::failure;
    }

    Parser::Result Parser::enum_decl(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rEnum}, fc) == Result::success and expect(Lexer::TokenType::identifier, {}, fc) == Result::success)
        {
            m_customTypesList.push_back(m_currentNameSpace + m_currentTokenData.token);

            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}) == Result::success)
            {
                if (type(FailureCase{true, "expected a type", "to be used as the base type for enum"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"enum\" <identifier> [\":\" @<type>] (\"{\" <identifier> [<initializer] {\",\" <identifier> [<initializer] \"};\") | \";\"");
                    additionalLog();
                }
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success)
            {
                /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                if (expect(Lexer::TokenType::identifier, {}) == Result::success)
                {
                    if (initializer() == Result::success){}
                    else{}

                    while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
                    {
                        if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in enum definition"}) == Result::success)
                        {
                            if (initializer() == Result::success){}
                            else{}
                        }
                        else
                        {
                            sensibleLog(), spaceLog();
                            m_log.write("  | @[missing] denotes what is missing |: \"enum\" <identifier> [\":\" <type>] (\"{\" <identifier> [<initializer] {\",\" @<identifier> [<initializer] \"};\") | \";\"");
                            additionalLog();
                        }
                        
                    }
                }

                if (expect(Lexer::TokenType::symbol, {Lexer::sRsemicolonCurly}, FailureCase{true, {}, "to end enum definition"}) == Result::failure)
                {
                    sensibleLog(), missingBraceLog(savedLcurlyTokenDataIndex), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"enum\" <identifier> [\":\" <type>] (\"{\" <identifier> [<initializer] {\",\" <identifier> [<initializer] @\"};\") | \";\"");
                    additionalLog();
                }
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}) == Result::success){}
            else
            m_log.write("expected either [ ; ] denoting a enum declaration or [ { ] denoting the start of a enum definition"), start_Log();

            return Result::success;
        }
        else
        previous(m_currentTokenDataIndex - savedTokenDataIndex);

        return Result::failure;
    }
    
    Parser::Result Parser::type_alias(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rType_alias}, fc) == Result::success and expect(Lexer::TokenType::identifier, {}, fc) == Result::success)
        {
            m_customTypesList.push_back(m_currentNameSpace + m_currentTokenData.token);

            if (expect(Lexer::TokenType::symbol, {Lexer::sAssign}, FailureCase{true, {}, "in type aliasing"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"type_alias\" <identifier> @\":=\" <type>"), additionalLog();
            }

            if (type(FailureCase{true, {}, "in type aliasing"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"type_alias\" <identifier> \":=\" @<type>"), additionalLog();
            }

            return Result::success;
        }
        else
        previous(m_currentTokenDataIndex - savedTokenDataIndex);

        return Result::failure;
    }

    Parser::Result Parser::type_name(const FailureCase& fc)
    {
        auto isFundamentalType = [&]
        {
            if (expect(Lexer::TokenType::reserved,
            {Lexer::rInt8, Lexer::rInt16, Lexer::rInt32, Lexer::rInt64, Lexer::rInt128, Lexer::rInt256, Lexer::rInt512, Lexer::rInt1024,
            Lexer::rUint8, Lexer::rUint16, Lexer::rUint32, Lexer::rUint64, Lexer::rUint128, Lexer::rUint256, Lexer::rUint512, Lexer::rUint1024,
            Lexer::rBool, Lexer::rAchar, Lexer::rPchar, Lexer::rU8char, Lexer::rAstring, Lexer::rPstring, Lexer::rU8string, Lexer::rBin16fp,
            Lexer::rBin32fp, Lexer::rBin64fp, Lexer::rBin128fp, Lexer::rDec32fp, Lexer::rDec64fp, Lexer::rDec128fp, Lexer::rDec256fp, Lexer::rDec512fp,
            Lexer::rVoid, Lexer::rNullptrType, Lexer::rNoneType, Lexer::rAnyTypeList, Lexer::rAnyType, Lexer::rNcType}, fc/*remove fc when you define somthing concrete for parsing for custom types*/) == Result::success)
            {
                return true;
            }
            else
            return false;
        };

        if (isFundamentalType())
        return Result::success;
        
        return Result::failure;//jackel::sumType<int32, int1024, pstring>; lg::tv::Arial_D<int32>
    }

    Parser::Result Parser::function_type(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::symbol, {Lexer::sLinverseParen}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc) == Result::success)
        {
            /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            auto savedLinverseParenTokenDataIndex = savedLparenTokenDataIndex - 1;

            List<Type> typeParameterList{};
            if (type() == Result::success)
            {
                typeParameterList.push_back(std::move(m_ast._getType()));

                while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) == Result::success)
                {
                    if (type(FailureCase{true, "expected a type after [ , ]", "in type-parameter-list during function-type declaration"}) == Result::failure)
                    {
                        sensibleLog(), spaceLog();
                        m_log.write("  | @[missing] denotes what is missing |: \"!(\" \"(\" <type> {\",\" @<type>} \")\" <type> \")\""), additionalLog();
                    }
                
                    typeParameterList.push_back(std::move(m_ast._getType()));
                }
            }
            else
            {
                Myfcn::U8string illegalTokens{};
                savedTokenDataIndex = m_currentTokenDataIndex;
                while (!m_isEndofTokenList)
                {
                    if (m_tokenDataList.at(m_currentTokenDataIndex).token == Lexer::sRparen)
                    break;

                    illegalTokens += m_tokenDataList.at(m_currentTokenDataIndex).token + ' '_u8;
                    next();
                }

                if (!illegalTokens.empty())
                {
                    if (m_currentTokenDataIndex - savedTokenDataIndex <= 1)
                    {
                        m_log.write("token [ ", illegalTokens, "] was found in an illegal position when parsing function type"), start_MultiLineLog(savedTokenDataIndex);
                        spaceLog();
                        m_log.write("  | [X] denotes where it was found |: \"!(\" \"(\" [X] <type> {\",\" <type>} \")\" <type> \")\""), additionalLog();
                    }
                    else
                    {
                        m_log.write("tokens [ ", illegalTokens, "] were found in illegal positions when parsing function type"), start_MultiLineLog(savedTokenDataIndex);
                        spaceLog();
                        m_log.write("  | [X] denotes where they were found |: \"!(\" \"(\" [X] <type> {\",\" <type>} \")\" <type> \")\""), additionalLog();
                    }
                }
            }
              
            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "to denote end of type-parameter-list during function-type declaration"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"!(\" \"(\" <type> {\",\" <type>} @\")\" <type> \")\""), additionalLog();
            }
            
            if (type(FailureCase{true, "expected a type denoting the give-type", "in function-type declaration"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"!(\" \"(\" <type> {\",\" <type>} \")\" @<type> \")\""), additionalLog();
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "to denote end of function-type declaration"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLinverseParenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"!(\" \"(\" <type> {\",\" <type>} \")\" <type> @\")\""), additionalLog();
            }

            Type giveType{ std::move(m_ast._getType()) };

            m_ast._makeFunctionType(typeParameterList, giveType);

            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }
        
        return Result::failure;
    }

    Parser::Result Parser::type(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (type_name() == Result::success or function_type() == Result::success or expect(Lexer::TokenType::reserved, {Lexer::rGuess}, fc) == Result::success)
        {
            auto savedTokenData = tokenData;
            Type::NormalType nType{tokenData.token};
            Myfcn::U8string typeAttributes{};

            if (expect(Lexer::TokenType::symbol, {Lexer::sColon}) == Result::success)
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}) == Result::success)
                {
                    /*when attempting to get the index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                    subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                    when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                    the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                    auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                    while (expect(Lexer::TokenType::identifier, {}) == Result::success)
                    { typeAttributes.append(tokenData.token); }

                    Myfcn::U8string falseTypeAttributes{};
                    auto savedTokenDataListIndex = m_currentTokenDataIndex;

                    while (!m_isEndofTokenList)
                    {
                        if (m_tokenDataList.at(m_currentTokenDataIndex).token == Lexer::sRparen)
                        break;

                        falseTypeAttributes += m_tokenDataList.at(m_currentTokenDataIndex).token + ' '_u8;
                        next();
                    }

                    if (!falseTypeAttributes.empty())
                    {
                        if (m_currentTokenDataIndex - savedTokenDataListIndex <= 1)
                        {
                            m_log.write("type attributes were expected instead of this [ ", falseTypeAttributes, ']'), start_MultiLineLog(savedTokenDataListIndex);
                        }
                        else
                        {
                            m_log.write("type attributes were expected instead of these [ ", falseTypeAttributes, ']'), start_MultiLineLog(savedTokenDataListIndex);
                        }
                        spaceLog();

                        m_log.write("  try any valid combination of the below instead:"), additionalLog();
                        m_log.write("  ● [ c ] stands for const; meaning immutablity, similar to the const keyword in the C/C++ language"), additionalLog();
                        m_log.write("  ● [ p ] stands for pointer; similar to C/C++'s (int*)"), additionalLog();
                        m_log.write("  ● [ l ] stands for lvalue-reference; similar to C++'s (int&)"), additionalLog();
                        m_log.write("  ● [ r ] stands for rvalue-reference; similar to C++'s (int&&)"), additionalLog();
                        m_log.write("  they are all case insentisive, so this [ int32:(c) ] is the same as [ int32:(C) ]"), additionalLog();
                        m_log.write("  [NOTE] the parser does not enforce the above, it only expects a bunch of identifiers; so something like this [ int32:(xvG) ] is parsed without error");
                        additionalLog();
                    }
                    
                    if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "for type declaration"}) == Result::failure)
                    {
                        sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                        m_log.write("  | @[missing] denotes what is missing |: (<type_name>|<function_type>|\"guess\") [\":\"] [ \"(\" [<type_arrtibutes>] @\")\" ]");
                        additionalLog();
                    }
                }
            }
            
            LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};
            
            if (!m_ast._getFunctionType().isEmpty())
            m_ast._makeType(m_ast._getFunctionType(), typeAttributes, locFinder);
            else
            m_ast._makeType(nType, typeAttributes, locFinder);

            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::function_decl(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::identifier, {}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc) == Result::success)
        {
            auto& identTokenData = m_tokenDataList.at(savedTokenDataIndex);
            Identifier funcIdent{ Myfcn::U8string{identTokenData.token}, makeLocFinder(identTokenData.line, identTokenData.relativeColumn, identTokenData.absoluteColumn) };
            
            /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            parameter_list();
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in function declaration"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: <identifier> \"(\" [<parameter_list>] @\")\" <type> <block>|\";\""), additionalLog();
            }

            Type giveType{};
            
            if (type() == Result::success)
            giveType = std::move(m_ast._getType());
            else
            giveType = Type{Myfcn::U8string{Lexer::rGuess}, ""_u8str, LocationFinder{}};
            
            Myfcn::U8string falseTypes{};
            savedTokenDataIndex = m_currentTokenDataIndex;
            while (!m_isEndofTokenList)
            {
                if (m_tokenDataList.at(m_currentTokenDataIndex).token == Lexer::sLcurly or m_tokenDataList.at(m_currentTokenDataIndex).token == Lexer::sSemicolon)
                break;

                falseTypes += m_tokenDataList.at(m_currentTokenDataIndex).token + ' '_u8;
                next();
            }

            if (!falseTypes.empty())
            {
                if (m_currentTokenDataIndex - savedTokenDataIndex <= 1)
                {
                    m_log.write("token [ ", falseTypes, "] was found in an illegal position when parsing function declaration"), start_MultiLineLog(savedTokenDataIndex);
                    spaceLog();
                    m_log.write(R"(  | [X] denotes where it was found |: <identifier> <parameter_list> [<type>] [X] <block>|";")"), additionalLog();
                }
                else
                {
                    m_log.write("tokens [ ", falseTypes, "] were found in illegal positions when parsing function declaration"), start_MultiLineLog(savedTokenDataIndex);
                    spaceLog();
                    m_log.write(R"(  | [X] denotes where they were found |: <identifier> <parameter_list> [<type>] [X] <block>|";")"), additionalLog();
                }
            }

            if (block() == Result::success)
            {
                m_ast.mkFunctionDefNode(giveType, funcIdent, m_ast._getParameterList(), m_ast.getBlockNode());
            }
            else if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}) == Result::success)
            {
                m_ast.mkFunctionDeclNode(giveType, funcIdent, m_ast._getParameterList());
            }
            else
            {
                m_log.write("expected either [ ; ] denoting the end of a function declaration or [ { ] denoting the begining of a function definition");
                start_Log();
            }
            m_ast.makeFuncDeclarationNode();
            
            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }
    
    Parser::Result Parser::parameter_list(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (type(fc) == Result::success)
        {
            std::optional<Identifier> ident{};

            if (expect(Lexer::TokenType::identifier, {}) == Result::success)
            {
                ident = Identifier{ Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn) };
                if (initializer() == Result::success){}
                else{}
            }
            
            m_ast._pushBack_parameter(m_ast._getType(), ident);
            ident.reset();

            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                if (type(FailureCase{true, "expected a type after comma", "in parameter list"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: <type> [<identifier>] { \",\" @<type> [<identifier>] }"), additionalLog();
                }

                if (expect(Lexer::TokenType::identifier, {}) == Result::success)
                {
                    ident = Identifier{ Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn) };
                    if (initializer() == Result::success){}
                    else{}
                }
                
                m_ast._pushBack_parameter(m_ast._getType(), ident);
                ident.reset();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::globalVar_decl(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (type(fc) == Result::success)
        {
            Type type{ std::move(m_ast._getType()) };

            if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "for global variable declaration"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: <type> @<identifier> [<intialization>] { \",\" <identifier> [<intialization>] }"), additionalLog();
            }

            Identifier ident{Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn)};

            if (initializer() == Result::success)
            m_ast.makeExpNode(), m_ast._pushBack_identInitPair(ident, m_ast.getExpNode());
            else
            m_ast._pushBack_identInitPair(ident);
            
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "for global variable declaration"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: <type> <identifier> [<intialization>] { \",\" @<identifier> [<intialization>] }"), additionalLog();
                }

                ident = {Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn)};

                if (initializer() == Result::success)
                m_ast.makeExpNode(), m_ast._pushBack_identInitPair(ident, m_ast.getExpNode());
                else
                m_ast._pushBack_identInitPair(ident);
            }

            auto& loc = type.getLocationFinder();
            LocationFinder locFinder{makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn)};
            m_ast.mkVarDeclNode(type, m_ast._getIdentInitPairList(), locFinder);
            m_ast.makeVarDeclarationNode();
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::initializer(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sAssign}) == Result::success)
        {
            if (assignment_exp(FailureCase{true, "expected an expected", "to be used as the intializer"}) == Result::failure)
            sensibleLog();
            
            return Result::success;
        }
        else if (anyTypeList_exp(fc) == Result::success)
        {
            return Result::success;
        }
        
        return Result::failure;
    }


    Parser::Result Parser::block(const FailureCase& fc)
    {
        auto savedTokenData = m_currentTokenData;

        if (expect(Lexer::TokenType::symbol, {Lexer::sLcurly}, fc) == Result::success)
        {
            /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLcurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            List<BlockItem> blockItemList{};

            while (true)
            {
                if (m_isEndofTokenList)
                {
                    expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "to end block declaration"});
                    sensibleLog(), missingBraceLog(savedLcurlyTokenDataIndex);
                    break;
                }
                else if (block_item() == Result::failure)
                {
                    if (expect(Lexer::TokenType::symbol, {Lexer::sRcurly}) == Result::success)
                    {
                        LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, m_currentTokenData.absoluteColumn)};

                        if (blockItemList.empty())
                        {
                            Void v{Void::InUse::yes};
                            m_ast.makeBlockNode(v, locFinder);
                        }
                        else
                        m_ast.makeBlockNode(blockItemList, locFinder);

                        break;
                    }
                    else
                    expect(Lexer::TokenType::_miscellany, {}, FailureCase{{}, "expected a block-item", "consider a <statement> or <localVar_decl>"});
                }
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::block_item(const FailureCase& fc)
    {
        if (localVar_decl() == Result::success)
        {
            expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});

            return Result::success;
        }
        else if (statement(fc) == Result::success)
        {
            m_ast.makeStatementNode();

            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::localVar_decl(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (type(fc) == Result::success)
        {
            //if the current token is [ !{ ], then it must be a temporary variable
            if (m_tokenDataList[m_currentTokenDataIndex].token == Nc::Lexer::sLinverseCurly)
            {
                m_hasType = true;
                return Result::failure;
            }

            Type type{ std::move(m_ast._getType()) };

            if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "for local variable declaration"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: <type> @<identifier> [<intialization>] { \",\" <identifier> [<intialization>] }"), additionalLog();
            }
                
            Identifier ident{Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn)};

            if (initializer() == Result::success)
            m_ast.makeExpNode(), m_ast._pushBack_identInitPair(ident, m_ast.getExpNode());
            else
            m_ast._pushBack_identInitPair(ident);
                
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "for local variable declaration"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: <type> <identifier> [<intialization>] { \",\" @<identifier> [<intialization>] }"), additionalLog();
                }

                ident = {Myfcn::U8string{tokenData.token}, makeLocFinder(tokenData.line, tokenData.relativeColumn, tokenData.absoluteColumn)};

                if (initializer() == Result::success)
                m_ast.makeExpNode(), m_ast._pushBack_identInitPair(ident, m_ast.getExpNode());
                else
                m_ast._pushBack_identInitPair(ident);
                
            }

            auto& loc = type.getLocationFinder();
            LocationFinder locFinder{makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn)};
            m_ast.mkVarDeclNode(type, m_ast._getIdentInitPairList(), locFinder);
            m_ast.makeVarDeclarationNode();
                
            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::statement(const FailureCase& fc)
    {
        if (jump_statement() == Result::success)
        {
            m_ast.mkJumpStatementNode(), expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});

            return Result::success;
        }
        if (if_condition() == Result::success)
        {
            return Result::success;
        }
        else if (for_loop() == Result::success)
        {
            return Result::success;
        }
        else if (for_loop_variant() == Result::success)
        {
            return Result::success;
        }
        else if (while_loop() == Result::success)
        {
            return Result::success;
        }
        else if (do_while_loop() == Result::success)
        {
            return Result::success;
        }
        else if (expression(fc) == Result::success)
        {
            m_ast.makeExpNode(), expect(Lexer::TokenType::symbol, {Lexer::sSemicolon});
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::if_condition(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rIf}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc) == Result::success)
        {
            /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            
            if (expression(FailureCase{true, "expected a condition-expression", "in if statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"if\" \"(\" @<expression> \")\" <statement> [\"else\" <statement>]"), additionalLog();
            }

            m_ast.makeExpNode();
            Expression conditionalExp{ std::move(m_ast.getExpNode()) };

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "after condition-expression in if statement"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"if\" \"(\" <expression> @\")\" <statement> [\"else\" <statement>]"), additionalLog();
            }

            if (statement(FailureCase{true, "expected a statement", "in if statement body"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"if\" \"(\" <expression> \")\" @<statement> [\"else\" <statement>]"), additionalLog();
            }

            m_ast.makeStatementNode();
            Statement statementIfTrue{ std::move(m_ast.getStatementNode()) };

            std::optional<Statement> statementIfFalse{};

            if (expect(Lexer::TokenType::reserved, {Lexer::rElse}) == Result::success)
            {   
                if (statement(FailureCase{true, "expected a statement", "for if-else statement body"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"if\" \"(\" <expression> \")\" <statement> [\"else\" @<statement>]"), additionalLog();
                }

                m_ast.makeStatementNode();
                statementIfFalse = std::move(m_ast.getStatementNode());
            }

            m_ast.mkIfConditionNode(conditionalExp, statementIfTrue, statementIfFalse);

            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }

    Parser::Result Parser::for_loop(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rFor}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc) == Result::success)
        {
            /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            
            if (localVar_decl() == Result::success)
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "to end local variable declaration"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> @\";\"] <expression> \";\" <expression> \")\" <statement>");
                    additionalLog();
                }   
            }

            VarDeclaration varDecl = std::move(m_ast.getVarDeclaration());

            if (expression(FailureCase{true, "expected an expression", "in for statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> \";\"] @<expression> \";\" <expression> \")\" <statement>");
                additionalLog();
            }

            m_ast.makeExpNode();
            Expression exp1 = std::move(m_ast.getExpNode());

            if (expect(Lexer::TokenType::symbol, {Lexer::sSemicolon}, FailureCase{true, {}, "to end expression in for statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> \";\"] <expression> @\";\" <expression> \")\" <statement>");
                additionalLog();
            }

            if (expression(FailureCase{true, "expected another expression", "in for-loop statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> \";\"] <expression> \";\" @<expression> \")\" <statement>");
                additionalLog();
            }

            m_ast.makeExpNode();
            Expression exp2 = std::move(m_ast.getExpNode());

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in for-loop statement"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> \";\"] <expression> \";\" <expression> @\")\" <statement>");
                additionalLog();
            }

            if (statement(FailureCase{true, "expected a statement", "for for-loop statement body"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" \"(\" [<localVar_decl> \";\"] <expression> \";\" <expression> \")\" @<statement>");
                additionalLog();
            }
                
            m_ast.makeStatementNode();
            Statement statement{ std::move(m_ast.getStatementNode()) };
                
            m_ast.mkForLoopNode(varDecl, exp1, exp2, statement);
            
            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }

    Parser::Result Parser::for_loop_variant(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rFor}, fc) == Result::success and (identifier_list() == Result::success or expect(Lexer::TokenType::literal, {Lexer::lnone}, fc) == Result::success))
        {
            if (expect(Nc::Lexer::TokenType::symbol, {Nc::Lexer::sColon}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" <identifier_list>|\"none\" @\":\" \"(\" \"default\"|<assignment_exp> [\",\" \"default\"|<relational_op>|<assignment_exp>] \")\" <statement>");
                additionalLog();
            }

            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in for-loop variant statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" <identifier_list>|\"none\" \":\" @\"(\" \"default\"|<assignment_exp> [\",\" \"default\"|<relational_op>|<assignment_exp>] \")\" <statement>");
                additionalLog();
            }
            else
            {
                /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            }

            if (expect(Nc::Lexer::TokenType::reserved, {Lexer::rDefault}) == Result::success or assignment_exp() == Result::success)
            {
                while (expect(Nc::Lexer::TokenType::symbol, {Lexer::sComma}) == Result::success)
                {
                    if (expect(Nc::Lexer::TokenType::reserved, {Lexer::rDefault}) == Result::failure and expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual, Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessthan_equalto, Lexer::sGreaterthan_equalto, Lexer::sAlternateLessthan_equalto, Lexer::sAlternateGreaterthan_equalto}) == Result::failure and assignment_exp(FailureCase{true, "expected either reserved token [ default ], relational operator or an expression", "in for-loop variant statement"}) == Result::failure)
                    {
                        sensibleLog(), spaceLog();
                        m_log.write("  | @[missing] denotes what is missing |: \"for\" <identifier_list>|\"none\" \":\" \"(\" \"default\"|<assignment_exp> [\",\" @(\"default\"|<relational_op>|<assignment_exp>)] \")\" <statement>");
                        additionalLog();
                        m_log.write("  by the way, the following are relational operators in NC: \"=\"|\"!=\"|\"≠\"|\"<\"|\">\"|\"<=\"|\">=\"|\"≤\"|\"≥\"");
                        additionalLog();
                    }
                }
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in for-loop variant statement"}) == Result::failure)
            {
                sensibleLog();
                if (optional_savedLparenTokenDataIndex.has_value())
                missingBraceLog(optional_savedLparenTokenDataIndex.value());
                spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" <identifier_list>|\"none\" \":\" \"(\" \"default\"|<assignment_exp> [\",\" \"default\"|<relational_op>|<assignment_exp>] @\")\" <statement>");
                additionalLog();
            }
            
            if (statement(FailureCase{true, "expected a statement", "for for-loop variant statement body"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"for\" <identifier_list>|\"none\" \":\" \"(\" \"default\"|<assignment_exp> [\",\" \"default\"|<relational_op>|<assignment_exp>] \")\" @<statement>");
                additionalLog();
            }

            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }

    Parser::Result Parser::identifier_list(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::identifier, {}) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) == Result::success)
            {
                if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "in identifier list"}) == Result::failure)
                {
                    sensibleLog(), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: <identifier> {\",\" @<identifier>}");
                    additionalLog();
                }
            }

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::while_loop(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::reserved, {Lexer::rWhile}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sLparen}, fc) == Result::success)
        {
            /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (expression(FailureCase{true, "expected a condition-expression", "in while-loop statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"while\" \"(\" @<expression> \")\" <statement>"), additionalLog();
            }

            m_ast.makeExpNode();
            Expression conditionalExp{ std::move(m_ast.getExpNode()) };

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in while-loop statement"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"while\" \"(\" <expression> @\")\" <statement>"), additionalLog();
            }

            if (statement(FailureCase{true, "expected a statement", "for while-loop statement body"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"while\" \"(\" <expression> \")\" @<statement>"), additionalLog();
            }

            m_ast.makeStatementNode();
            m_ast.mkWhileLoopNode(conditionalExp, m_ast.getStatementNode());

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::do_while_loop(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };

        if (expect(Lexer::TokenType::reserved, {Lexer::rDo}, fc) == Result::success and statement(fc) == Result::success)
        {
            m_ast.makeStatementNode();
            Statement doStatement{ std::move(m_ast.getStatementNode()) };

            if (expect(Lexer::TokenType::reserved, {Lexer::rWhile}, FailureCase{true, {}, "after token [ do ] in do-while-loop"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"do\" <statement> @\"while\" \"(\" <expression> \")\""), additionalLog();
            }

            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "in do-while-loop statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"do\" <statement> \"while\" @\"(\" <expression> \")\""), additionalLog();
            }
            else
            {
                /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            }

            if (expression(FailureCase{true, "expected a condition-expression", "after [ ( ] in do-while-loop statement"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"do\" <statement> \"while\" \"(\" @<expression> \")\""), additionalLog();
            }

            m_ast.makeExpNode();
            Expression whileConditionExp{ std::move(m_ast.getExpNode()) };

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "after expression in do-while-loop statement"}) == Result::failure)
            {
                sensibleLog();
                if (optional_savedLparenTokenDataIndex.has_value())
                missingBraceLog(optional_savedLparenTokenDataIndex.value());
                spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"do\" <statement> \"while\" \"(\" <expression> @\")\""), additionalLog();
            }

            m_ast.mkDoWhileLoopNode(doStatement, whileConditionExp);

            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }

    Parser::Result Parser::jump_statement(const FailureCase& fc)
    {
        auto& tokenData  = m_currentTokenData;

        if (expect(Lexer::TokenType::reserved, {Lexer::rGive}) == Result::success)
        {
            auto savedTokenData = tokenData;
            if (expression() == Result::success)
            {
                m_ast.makeExpNode();
                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};
                m_ast.mkGiveJSNode(m_ast.getExpNode(), locFinder);
            }
            else
            {
                Void v{Void::InUse::yes};
                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};
                m_ast.mkGiveJSNode(v, locFinder);
            }
            
            return Result::success;
        }
        else if (expect(Lexer::TokenType::reserved, {Lexer::rBreak, Lexer::rContinue}, fc) == Result::success)
        {
            LocationFinder locFinder{makeLocFinder(m_currentTokenData.line, m_currentTokenData.relativeColumn, m_currentTokenData.absoluteColumn)};

            if (m_currentTokenData.token == Lexer::rBreak)
            m_ast.mkBreakJSNode(locFinder);
            else
            m_ast.mkContinueJSNode(locFinder);
            
            return Result::success;
        }
        
        return Result::failure;
    }


    Parser::Result Parser::expression(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (assignment_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                assignment_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in comma expression"});

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::assignment_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (conditional_exp(fc) == Result::success)
        {
            if (expect(Lexer::TokenType::symbol,
            {Lexer::sAssign, Lexer::sAdditionAssign, Lexer::sSubtractionAssign, Lexer::sMultiplicationAssign, Lexer::sDivisionAssign, Lexer::sModuluoAssign,
            Lexer::sBitwiseAndAssign, Lexer::sBitwiseOrAssign, Lexer::sBitwiseXorAssign, Lexer::sBitwiseShiftLeftAssign, Lexer::sBitwiseShiftRightAssign})
            != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (assignment_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in assignment expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::conditional_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;
        
        if (logical_exp(fc) == Result::success)
        {
            if (expect(Lexer::TokenType::symbol, {Lexer::sQuestionMark}) == Result::success)
            {
                auto savedTokenData = tokenData;

                Expression conditionExp{};
                m_ast.mkLocalExpNode(conditionExp);

                Myfcn::U8string t_op{m_currentTokenData.token};

                if (assignment_exp(FailureCase{true, "expected an expression to be used as the expression-if-true", "in conditional_expression"}) == Result::failure)
                { sensibleLog(); }

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                if (expect(Lexer::TokenType::symbol, {Lexer::sColon}, FailureCase{true, {}, "to seperate the expressions in conditional_expression"}) == Result::failure)
                { sensibleLog(); }

                if (assignment_exp(FailureCase{true, "expected an expression to be used as the expression-if-false", "in conditional_expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkTenaryOpNode(conditionExp, t_op, leftExp, rightExp, locFinder);
            }
            
            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::logical_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (bitwise_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sAnd, Lexer::sOr, Lexer::sXor}) != Result::failure or expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (bitwise_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in logical expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::bitwise_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (equality_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sBitwiseAnd, Lexer::sBitwiseOr, Lexer::sBitwiseXor}) != Result::failure or expect(Lexer::TokenType::reserved, {Lexer::rBitwiseAnd, Lexer::rBitwiseOr, Lexer::rBitwiseXor}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (equality_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in bitwise expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::equality_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (relational_exp(fc) == Result::success)
        {

            while (expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (relational_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in equality expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;    
        }

        return Result::failure;
    }

    Parser::Result Parser::relational_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (shift_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessthan_equalto, Lexer::sGreaterthan_equalto, Lexer::sAlternateLessthan_equalto, Lexer::sAlternateGreaterthan_equalto}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (shift_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in relational expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }
    
    Parser::Result Parser::shift_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (additive_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sBitwiseShiftLeft, Lexer::sBitwiseShiftRight}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);

                Myfcn::U8string b_op{m_currentTokenData.token};

                if (additive_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in shift expression"}) == Result::failure)
                { sensibleLog(); }
                
                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }

            return Result::success;    
        }

        return Result::failure;
    }
    /*I fucked up in getting the LocationFinder by savedTokenData not being where it is supposed to be, additive_exp has the solution[fix later]*/
    Parser::Result Parser::additive_exp(const FailureCase& fc)
    {
        auto savedTokenData = m_currentTokenData;

        if (multiplicative_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sPlus, Lexer::sMinus}) != Result::failure)
            {
                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);
                
                Myfcn::U8string b_op{m_currentTokenData.token};

                if (multiplicative_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in additive expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, m_currentTokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::multiplicative_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (exponention_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sMultiply, Lexer::sDivide, Lexer::sModuluo}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);
                
                Myfcn::U8string b_op{m_currentTokenData.token};
                
                if (exponention_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in multiplicative expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::exponention_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (member_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sExponention}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);
                
                Myfcn::U8string b_op{m_currentTokenData.token};
                
                if (member_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in exponention expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::member_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (prefix_exp(fc) == Result::success)
        {
            while (expect(Lexer::TokenType::symbol, {Lexer::sMemberOf, Lexer::sArrow}) != Result::failure)
            {
                auto savedTokenData = tokenData;

                Expression leftExp{};
                m_ast.mkLocalExpNode(leftExp);
                
                Myfcn::U8string b_op{m_currentTokenData.token};

                if (prefix_exp(FailureCase{true, "expected an expression to be used as the rhs(right-hand-side)", "in member expression"}) == Result::failure)
                { sensibleLog(); }

                Expression rightExp{};
                m_ast.mkLocalExpNode(rightExp);

                LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

                m_ast.mkBinaryOpNode(leftExp, b_op, rightExp, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::prefix_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (expect(Lexer::TokenType::symbol, {Lexer::sMinus, Lexer::sBitwiseNot, Lexer::sNot, Lexer::sPlus, Lexer::sIncrement, Lexer::sDecrement, Lexer::sCommercialAt}) == Result::success or expect(Lexer::TokenType::reserved, {Lexer::rBitwiseNot, Lexer::rNot, Lexer::rOdd, Lexer::rMem}) == Result::success)
        {
            auto savedTokenData = tokenData;

            Myfcn::U8string u_op{m_currentTokenData.token};

            if (prefix_exp(FailureCase{true, "expected an expression", "in prefix expression"}) == Result::failure)
            { sensibleLog(); }

            Expression rightExp{};
            m_ast.mkLocalExpNode(rightExp);

            LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, tokenData.absoluteColumn)};

            m_ast.mkUprefixOpNode(u_op, rightExp, locFinder);
            
            return Result::success;
        }
        else if (postfix_exp(fc) == Result::success)
        {
            return Result::success;
        }
            
        return Result::failure;
    }

    Parser::Result Parser::postfix_exp(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (primary_exp(fc) == Result::success)
        {
            while (true)
            {
                if (expect(Lexer::TokenType::symbol, {Lexer::sIncrement, Lexer::sDecrement}) == Result::success)
                {
                    Expression leftExp{};
                    m_ast.mkLocalExpNode(leftExp);

                    auto loc = leftExp.getExpLocation();

                    LocationFinder locFinder{ makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn) };
                    Myfcn::U8string u_op{tokenData.token};
                    m_ast.mkUpostfixOpNode(leftExp, u_op, locFinder);
                }
                else if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}) == Result::success)
                {
                    /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                    subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                    when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                    the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                    auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                    Expression leftExp{};
                    m_ast.mkLocalExpNode(leftExp);

                    auto loc = leftExp.getExpLocation();

                    expression_list();

                    LocationFinder locFinder{ makeLocFinder(loc.first.first, loc.second.first, tokenData.absoluteColumn) };
                    m_ast.mkUpostfixOpNode(leftExp, m_ast._getExpressionList(), locFinder);

                    if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "to end call expression"}) == Result::failure)
                    { missingBraceLog(savedLparenTokenDataIndex), sensibleLog(); }
                }
                else
                break;
            }

            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::primary_exp(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}) == Result::success)
        {
            /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            if (expression(FailureCase{true, "expected an expression", "inside the parantheses operator"}) == Result::failure)
            sensibleLog();
            
            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "when using the parantheses operator"}) == Result::failure)
            missingBraceLog(savedLparenTokenDataIndex), sensibleLog();

            return Result::success;
        }
        else if (r_and_l_exp() == Result::success)
        {
            return Result::success;
        }
        else if (block() == Result::success)
        {
            return Result::success;
        }
        else if (special_ident({Lexer::rInput, Lexer::rOutput, Lexer::rTypecast, Lexer::rBitcast, Lexer::rGlobal}) == Result::success)
        {
            return Result::success;
        }
        else if (tempVarDecl() == Result::success)
        {
            return Result::success;
        }
        else if (anyTypeList_exp() == Result::success)
        {
            return Result::success;
        }
        else if (expect(Lexer::TokenType::literal, {}, fc) == Result::success)
        {
            LocationFinder locFinder{makeLocFinder(m_currentTokenData.line, m_currentTokenData.relativeColumn, m_currentTokenData.absoluteColumn)};
            Myfcn::U8string literalConst{m_currentTokenData.token};

            Myfcn::U8string literalType{m_currentTokenData.literalType};
            
            m_ast.mkLiteralNode(literalConst, literalType, locFinder);

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::special_ident(std::vector<Myfcn::U8string_view> reservedInputs)
    {
        if (expect(Lexer::TokenType::identifier, {}) == Result::success or expect(Lexer::TokenType::reserved, {reservedInputs}) == Result::success)
        {
            auto savedTokenData = m_currentTokenData;
    
            if (expect(Lexer::TokenType::symbol, {Lexer::sLessthan}) == Result::success)
            {
                /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                auto savedLangleTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                type_and_exp_list();

                if (expect(Lexer::TokenType::symbol, {Lexer::sGreaterthan}, FailureCase{true, {}, "to end template agrument"}) == Result::failure)
                {
                    sensibleLog(), missingBraceLog(savedLangleTokenDataIndex), spaceLog();
                    m_log.write("  | @[missing] denotes what is missing |: \"<\" [<expression_list>] @\">\"");
                    additionalLog();
                }
            }

            while (expect(Lexer::TokenType::symbol, {Lexer::sScopeResolution}) == Result::success)
            {
                if (expect(Lexer::TokenType::identifier, {}, FailureCase{true, {}, "after scope associator [ :: ]"}) == Result::success)
                {
                    if (expect(Lexer::TokenType::symbol, {Lexer::sLessthan}) == Result::success)
                    {
                        /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                        subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                        when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                        the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                        auto savedLangleTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

                        type_and_exp_list();

                        if (expect(Lexer::TokenType::symbol, {Lexer::sGreaterthan}, FailureCase{true, {}, "to end template agrument"}) == Result::failure)
                        {
                            sensibleLog(), missingBraceLog(savedLangleTokenDataIndex), spaceLog();
                            m_log.write("  | @[missing] denotes what is missing |: \"<\" [<expression_list>] @\">\"");
                            additionalLog();
                        }
                    }
                }
                else
                sensibleLog();
            }
            
            LocationFinder locFinder{makeLocFinder(savedTokenData.line, savedTokenData.relativeColumn, m_currentTokenData.absoluteColumn)};
            Myfcn::U8string ident{savedTokenData.token};
            m_ast.mkIdentifierNode(ident, locFinder);

            return Result::success;
        }

        return Result::failure;
    }

    Parser::Result Parser::tempVarDecl(const FailureCase& fc)
    {
        auto& tokenData = m_currentTokenData;

        if (m_hasType or type(fc) == Result::success)
        {
            m_hasType = false;
            Type type{ std::move(m_ast._getType()) };

            if (anyTypeList_exp(FailureCase{true, "expected an anyTypeList expression to be used as the temporary variable initializer"}) == Result::failure)
            sensibleLog();
            
            auto& typeLocFinder{ type.getLocationFinder() };
            LocationFinder locFinder{makeLocFinder(typeLocFinder.first.first, typeLocFinder.second.first, tokenData.absoluteColumn)};
            m_ast.mkTempVarDeclNode(type, m_ast.getAnyTypeListExpNode(), locFinder);
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::anyTypeList_exp(const FailureCase& fc)
    {
        if (expect(Lexer::TokenType::symbol, {Lexer::sLinverseCurly}, fc) == Result::success)
        {
            /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
            auto savedLinverseCurlyTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;

            expression_list();

            if (expect(Lexer::TokenType::symbol, {Lexer::sRcurly}, FailureCase{true, {}, "in anyTypeList expression"}) == Result::failure)
            sensibleLog(), missingBraceLog(savedLinverseCurlyTokenDataIndex);

            LocationFinder locFinder{};

            if (!m_ast._getExpressionList().empty())
            m_ast.mkAnyTypeListExpNode(m_ast._getExpressionList(), locFinder);
            else
            {
                Void v{Void::InUse::yes};
                m_ast.mkAnyTypeListExpNode(v, locFinder);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    Parser::Result Parser::expression_list(const FailureCase& fc)
    {
        if (expect(Nc::Lexer::TokenType::reserved, {Nc::Lexer::rDefault}) == Result::success or assignment_exp(fc) == Result::success)
        {
            ExpressionList expList{};
            m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));

            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                if (expect(Nc::Lexer::TokenType::reserved, {Nc::Lexer::rDefault}) == Result::failure and assignment_exp(FailureCase{true, "expected either reserved token [ default ] or an expression", "in expression list"}) == Result::failure)
                sensibleLog();

                m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));
            }

            m_ast._getExpressionList() = std::move(expList);
            return Result::success;
        }
            
        return Result::failure;
    }

    Parser::Result Parser::type_and_exp_list(const FailureCase& fc)
    {
        if (expect(Nc::Lexer::TokenType::reserved, {Nc::Lexer::rDefault}) == Result::success or type() == Result::success or primary_exp(fc) == Result::success) //i expect a type before primary expression because primary expression also looks for a type through tempVarDecl
        {
            // ExpressionList expList{};
            // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));

            while (expect(Lexer::TokenType::symbol, {Lexer::sComma}) != Result::failure)
            {
                if (expect(Nc::Lexer::TokenType::reserved, {Nc::Lexer::rDefault}) == Result::failure and type() == Result::failure and primary_exp(FailureCase{true, "expected either reserved token [ default ], type or an expression"}) == Result::failure)
                sensibleLog();

                // m_ast.makeExpNode(), expList.push_back(std::move(m_ast.getExpNode()));
            }

            // m_ast._getExpressionList() = std::move(expList);
            return Result::success;
        }
            
        return Result::failure;
    }

    Parser::Result Parser::r_and_l_exp(const FailureCase& fc)
    {
        auto savedTokenDataIndex{ m_currentTokenDataIndex };
        auto savedTokenData = m_currentTokenData;

        if (expect(Lexer::TokenType::symbol, {Lexer::sLhashedParen}, fc) == Result::success and expect(Lexer::TokenType::symbol, {Lexer::sEqual, Lexer::sNotEqual, Lexer::sAlternateNotEqual, Lexer::sLessthan, Lexer::sGreaterthan, Lexer::sLessthan_equalto, Lexer::sGreaterthan_equalto, Lexer::sAlternateLessthan_equalto, Lexer::sAlternateGreaterthan_equalto}, fc) == Result::success)
        {
            auto relationalOp = m_currentTokenData.token;

            /*when attempting to get the index of a [Brace], because next() would be called if the expected [Brace] was found, we need to
            subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
            when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
            the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments
            [Special Case because the brace is first encountered]*/
            auto savedLhashedparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex - 1 : m_currentTokenDataIndex - 2;

            if (expect(Lexer::TokenType::symbol, {Lexer::sComma}, FailureCase{true, {}, "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> @\",\" <logical_op> \",\" <assignment_exp> \")\" \"(\" <expression_list> \")\"");
                additionalLog();
            }
            
            if (expect(Lexer::TokenType::reserved, {Lexer::rAnd, Lexer::rOr, Lexer::rXor}) == Result::failure and expect(Lexer::TokenType::symbol, {Lexer::sAnd, Lexer::sOr, Lexer::sXor}, FailureCase{true, {}, "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" @<logical_op> \",\" <assignment_exp> \")\" \"(\" <expression_list> \")\"");
                additionalLog();
            }

            auto logicalOp = m_currentTokenData.token;

            if (expect(Lexer::TokenType::symbol, {Lexer::sComma}, FailureCase{true, {}, "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> @\",\" <assignment_exp> \")\" \"(\" <expression_list> \")\"");
                additionalLog();
            }

            if (assignment_exp(FailureCase{true, "expected an expression", "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> \",\" @<assignment_exp> \")\" \"(\" <expression_list> \")\"");
                additionalLog();
            }

            m_ast.makeExpNode();
            auto exp = std::move(m_ast.getExpNode());

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), missingBraceLog(savedLhashedparenTokenDataIndex), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> \",\" <assignment_exp> @\")\" \"(\" <expression_list> \")\"");
                additionalLog();
            }

            std::optional<std::size_t> optional_savedLparenTokenDataIndex{};

            if (expect(Lexer::TokenType::symbol, {Lexer::sLparen}, FailureCase{true, {}, "denoting start of expression list in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> \",\" <assignment_exp> \")\" @\"(\" <expression_list> \")\"");
                additionalLog();
            }
            else
            {
                /*when attempting to get a index of the [Brace], because next() would be called if the expected [Brace] was found, we need to
                subtract one from the currentTokenDataIndex to ensure that we get the [Brace] index, but precaution should also be taken, because
                when the end of tokenDataList is reached, next() dosen't increment, it just leaves the index, so there should be a check to see if
                the end of tokenDataList is reached, so that we need not bother ourselves with a subtraction by one, since next() never increments*/
                optional_savedLparenTokenDataIndex = m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1;
            }

            if (expression_list(FailureCase{true, "expected a list of expressions", "in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog(), spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> \",\" <assignment_exp> \")\" \"(\" @<expression_list> \")\"");
                additionalLog();
            }

            if (expect(Lexer::TokenType::symbol, {Lexer::sRparen}, FailureCase{true, {}, "denoting end of expression list in relational_and_logical expression"}) == Result::failure)
            {
                sensibleLog();
                if (optional_savedLparenTokenDataIndex.has_value())
                missingBraceLog(optional_savedLparenTokenDataIndex.value());
                spaceLog();
                m_log.write("  | @[missing] denotes what is missing |: \"#(\" <relational_op> \",\" <logical_op> \",\" <assignment_exp> \")\" \"(\" <expression_list> @\")\"");
                additionalLog();
            }

            m_ast.mkR_And_L_Exp_Node(relationalOp, logicalOp, exp, m_ast._getExpressionList());

            return Result::success;
        }
        else
        { previous(m_currentTokenDataIndex - savedTokenDataIndex); }

        return Result::failure;
    }


    std::string_view Parser::mapTokenToCategory(std::optional<Lexer::TokenData*> optional_td)
    {
        if (!optional_td.has_value())
        optional_td = &m_tokenDataList[m_currentTokenDataIndex];
        
        if (optional_td.value()->isIdentifier) return "identifier-token";
        else if (not (optional_td.value()->literalType.empty())) return "literal-token";
        else if (optional_td.value()->isReserved) return "reserved-token";
        else if (optional_td.value()->isSymbol) return "symbol-token";
    
        return "";
    }
    

    Parser::Result Parser::expect(Lexer::TokenType tokenType, std::vector<Myfcn::U8string_view> tokenStr, const FailureCase& fc)
    {
       Myfcn::U8string allTokens{};
       m_currentTokenData = m_tokenDataList.at(m_currentTokenDataIndex);
       
       for (auto &&i : tokenStr)
       {
            allTokens += i;

            if (i != tokenStr.back())
            allTokens += " or "_u8str;      
       }
    
        switch (tokenType) //refactor your switch statement to disallow exceptions
        {
        case Lexer::TokenType::reserved:
            if (!m_isEndofTokenList)
            {
                if (m_tokenDataList.at(m_currentTokenDataIndex).isReserved)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_tokenDataList.at(m_currentTokenDataIndex).token == i)
                        {
                            next();
                            return Result::success;
                        }
                    }
                }
            }

            if (fc.shouldReport)
            {
                if (m_isEndofTokenList)
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected [", allTokens, "] after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }
                else
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected [", allTokens, "] before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    
                }
    
                start_Log();
            }

            break;
        case Lexer::TokenType::identifier:
            if (!m_isEndofTokenList)
            {
                if (m_tokenDataList.at(m_currentTokenDataIndex).isIdentifier)
                {
                    next();
                    return Result::success;
                }
            }

            if (fc.shouldReport)
            {
                if (m_isEndofTokenList)
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected an identifier after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }
                else
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected an identifier before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }
                    
                start_Log();
            }
            
            break;
        case Lexer::TokenType::literal:
            if (!m_isEndofTokenList)
            {
                if (!m_tokenDataList.at(m_currentTokenDataIndex).literalType.empty())
                {
                    if (tokenStr.empty())
                    {
                        next();
                        return Result::success;
                    }
                    else
                    {
                        for (auto &&i : tokenStr)
                        {
                            if (m_tokenDataList.at(m_currentTokenDataIndex).literalType == i)
                            {
                                next();
                                return Result::success;
                            }
                        }
                    }
                }
            }

            if (fc.shouldReport)
            {
                if (m_isEndofTokenList)
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected a literal after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }
                else
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected a literal before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }

                start_Log();
            }

            break;
        case Lexer::TokenType::symbol:
            if (!m_isEndofTokenList)
            {
                if (m_tokenDataList.at(m_currentTokenDataIndex).isSymbol)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_tokenDataList.at(m_currentTokenDataIndex).token == i)
                        {
                            next();
                            return Result::success;
                        }
                    }
                }
            }
            
            if (fc.shouldReport)
            {
                if (m_isEndofTokenList)
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected [", allTokens, "] after", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }
                else
                {
                    if (fc.optErrorMsg.has_value())
                    m_log.writews(*fc.optErrorMsg, "before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                    else
                    m_log.writews("expected [", allTokens, "] before", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg);
                }

                start_Log();
            }

                break;
        case Lexer::TokenType::_miscellany:
            if (fc.optErrorMsg.has_value())
            m_log.writews(*fc.optErrorMsg, "instead of", mapTokenToCategory(), '[', m_currentTokenData.token, ']', fc.extraErrMsg), start_Log();
            else
            std::cout << "In fucntion Nc::Parser::expect, parameter function fc.optErrorMsg.has_value() is false under case Lexer::TokenType::_miscellany", std::exit(1);
            
            next();
            break;
        default:
            std::cout << "In fucntion Nc::Parser::expect variable tokenTypeToMatch contains a value that was not expected", std::exit(1);
        }

        return Result::failure;
    }
    
    void Parser::next(std::size_t amount)
    {
        if ((m_currentTokenDataIndex += amount) >= m_tokenDataList.size())
        m_currentTokenDataIndex = m_tokenDataList.size() - 1, m_isEndofTokenList = true;
    }

    void Parser::previous(std::size_t amount)
    {
        int(m_currentTokenDataIndex -= amount) < 0? m_currentTokenDataIndex = 0 : m_currentTokenDataIndex;
    }

    void Parser::start_Log(bool isAdditionalLog)
    {
        if (isAdditionalLog)
        {
            m_log.log();
        }
        else
        {
            m_log.log(true);
            
            const auto& tokenData{ m_tokenDataList.at(m_currentTokenDataIndex) };

            m_log.writews("  line:", tokenData.line, "from column: [", tokenData.relativeColumn, '-', tokenData.absoluteColumn, ']');
            m_log.log();

            Myfcn::U8string lineContents{};
            for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[tokenData.line]; i != m_fileBuffer.end(); i.operator++())
            {
                if (*i == '\n'_u8)
                break;

                lineContents.push_back(*i);
            }
            m_log.writews("  line contents:" , lineContents);
            m_log.log();
            
            if (tokenData.absoluteColumn - tokenData.relativeColumn == 1)
            m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(tokenData.relativeColumn - 1, tokenData.absoluteColumn - tokenData.relativeColumn, "⇑"_u8));
            else
            m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(tokenData.relativeColumn - 1, tokenData.absoluteColumn - tokenData.relativeColumn, "↑"_u8));

            m_log.log();
        }

        //ensures ast nodes aren't made anymore
        m_ast.setDeadZoneFlag();
    }

    void Parser::start_MultiLineLog(std::size_t tokenDataIndexStart)
    {
        m_log.log(true);

        const auto& begin{ m_tokenDataList.at(tokenDataIndexStart) };
        const auto& end{ m_tokenDataList.at(m_isEndofTokenList? m_currentTokenDataIndex : m_currentTokenDataIndex - 1) };

        if (begin.line == end.line)
        {
            m_log.writews("  line:", begin.line, "from column: [", begin.relativeColumn, '-', end.absoluteColumn, ']');
            m_log.log();

            Myfcn::U8string lineContents{};
            for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[begin.line]; i != m_fileBuffer.end(); i.operator++())
            {
                if (*i == '\n'_u8)
                break;

                lineContents.push_back(*i);
            }
            m_log.writews("  line contents:" , lineContents);
            m_log.log();

            if (end.absoluteColumn - begin.relativeColumn == 1)
            m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(begin.relativeColumn - 1, end.absoluteColumn - begin.relativeColumn, "⇑"_u8));
            else
            m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(begin.relativeColumn - 1, end.absoluteColumn - begin.relativeColumn, "↑"_u8));

            m_log.log();
        }
        else
        {
            auto getFirstTokenDataOnLine = [&] (std::uint32_t line_in_question) -> const auto&
            {
                for (auto i = tokenDataIndexStart; i < m_currentTokenDataIndex; ++i)
                {
                    if (m_tokenDataList.at(i).line == line_in_question)
                    return m_tokenDataList.at(i);
                }
                
                return begin;
            };

            auto getLastTokenDataOnLine = [&] (std::uint32_t line_in_question) -> const auto&
            {
                for (auto i = tokenDataIndexStart; i < m_currentTokenDataIndex; ++i)
                {
                    if (m_tokenDataList.at(i).line > line_in_question)
                    return m_tokenDataList.at(i - 1);
                }
                
                return end;
            };

            m_log.writews("  from line [", begin.line, '-', end.line, ']');
            m_log.log();

            for (auto i = begin.line; i <= end.line; i++)
            {
                m_log.writews("  line:", i, "from column: [", getFirstTokenDataOnLine(i).relativeColumn, '-', getLastTokenDataOnLine(i).absoluteColumn, ']');
                m_log.log();

                Myfcn::U8string lineContents{};
                bool isLineAllWhiteSpace{true};
                for (auto j = m_fileBuffer.begin() + m_lineNumToFilePos[i]; j != m_fileBuffer.end(); j.operator++())
                {
                    if (*j == '\n'_u8)
                    break;

                    if (!j->isBasicLatinWhiteSpace())
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
                    Myfcn::U8string{lineContents.size(), ' '_u8}.
                    insert(getFirstTokenDataOnLine(i).relativeColumn - 1, getLastTokenDataOnLine(i).absoluteColumn - getFirstTokenDataOnLine(i).relativeColumn, "⇑"_u8));
                }
                else
                {
                    m_log.writews("               :",
                    Myfcn::U8string{lineContents.size(), ' '_u8}.
                    insert(getFirstTokenDataOnLine(i).relativeColumn - 1, getLastTokenDataOnLine(i).absoluteColumn - getFirstTokenDataOnLine(i).relativeColumn, "↑"_u8));
                }
                m_log.log();
            }   
        }

        //ensures ast nodes aren't made anymore
        m_ast.setDeadZoneFlag();
    }

    void Parser::sensibleLog()
    {
        if (m_isEndofTokenList)
        return;

        auto& previousTokenData{ m_tokenDataList.at(m_currentTokenDataIndex - 1) };
        spaceLog();

        m_log.writews("  [NOTE] meaning it was expected after", mapTokenToCategory(&previousTokenData), '[', previousTokenData.token, ']');
        m_log.log();

        m_log.writews("  line:", previousTokenData.line, "from column: [", previousTokenData.relativeColumn, '-', previousTokenData.absoluteColumn, ']');
        m_log.log();

        Myfcn::U8string lineContents{};
        for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[previousTokenData.line]; i != m_fileBuffer.end(); i.operator++())
        {
            if (*i == '\n'_u8)
            break;

            lineContents.push_back(*i);
        }
        m_log.writews("  line contents:" , lineContents);
        m_log.log();
            
        if (previousTokenData.absoluteColumn - previousTokenData.relativeColumn == 1)
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(previousTokenData.relativeColumn - 1, previousTokenData.absoluteColumn - previousTokenData.relativeColumn, "⇑"_u8));
        else
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(previousTokenData.relativeColumn - 1, previousTokenData.absoluteColumn - previousTokenData.relativeColumn, "↑"_u8));

        m_log.log();
    }

    void Parser::missingBraceLog(std::size_t savedTokenBraceDataIndex)
    {
        const auto& tokenData{ m_tokenDataList.at(savedTokenBraceDataIndex) };
        spaceLog();

        m_log.writews("  to match symbol-token [", tokenData.token, ']');
        m_log.log();

        m_log.writews("  line:", tokenData.line, "from column: [", tokenData.relativeColumn, '-', tokenData.absoluteColumn, ']');
        m_log.log();

        Myfcn::U8string lineContents{};
        for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[tokenData.line]; i != m_fileBuffer.end(); i.operator++())
        {
            if (*i == '\n'_u8)
            break;

            lineContents.push_back(*i);
        }
        m_log.writews("  line contents:" , lineContents);
        m_log.log();
            
        if (tokenData.absoluteColumn - tokenData.relativeColumn == 1)
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(tokenData.relativeColumn - 1, tokenData.absoluteColumn - tokenData.relativeColumn, "⇑"_u8));
        else
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(tokenData.relativeColumn - 1, tokenData.absoluteColumn - tokenData.relativeColumn, "↑"_u8));

        m_log.log();
    }

    void Parser::spaceLog()
    { m_log.log(); }

    void Parser::additionalLog()
    { m_log.log(); }


    LocationFinder Parser::makeLocFinder(std::uint32_t lineNumber0, std::uint32_t relativeColumn0, std::uint32_t absoluteColumn1)
    {
        return LocationFinder{ LineNumber_and_LineCount{lineNumber0, m_currentTokenData.line - lineNumber0}, ColumnSpan{relativeColumn0, absoluteColumn1} };
    }

    [[noreturn]] void Parser::errorOutput()
    {
        m_log.write("{compilation terminated} (⌐■_■) ψ(._. )>");
        m_log.log();
        
        m_log.outputLogEntries("PARSER ERROR", System::coutput);

        m_tokenDataList.~vector();
        m_lineNumToFilePos.~unordered_map();
        m_fileBuffer.~basic_string();
        m_currentTokenDataIndex = {};
        m_currentTokenData.~TokenData();
        m_log.~Log();
        m_isEndofTokenList = {};
        m_ast.~Ast();
        m_customTypesList.~vector();

        std::exit(EXIT_FAILURE);
    }

} // namespace Nc