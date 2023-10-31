
#include "ncParser.h"
#include "drawboxaroundtext.h"
#include "stream/output.h"

namespace Nc
{
    NcParser::NcParser(std::vector<NcLexer::TokenData> tokenList): m_tokenList{tokenList}
    {}

    void NcParser::parseTokenList()
    {
        program();
    }

    void NcParser::program()
    {
        function(NcLexer::rMain);
    }

    void NcParser::function(std::string_view name)
    {
        type();

        if (name == "")
        expect(NcLexer::TokenType::identifier, {});
        else
        expect(NcLexer::TokenType::reserved, {name});

        m_codeGen.fcn(m_tokenList.at(m_currentToken).token);
        next();

        expect(NcLexer::TokenType::symbol, {NcLexer::sLparen});
        next();
        expect(NcLexer::TokenType::symbol, {NcLexer::sRparen});
        next();

        block();
    }

    void NcParser::type()
    {
        expect(NcLexer::TokenType::reserved, {NcLexer::rInt32});
        next();
    }

    void NcParser::block()
    {
        expect(NcLexer::TokenType::symbol, {NcLexer::sLcurly});
        next();
        statement();
        expect(NcLexer::TokenType::symbol, {NcLexer::sRcurly});
        next();
    }

    void NcParser::statement()
    {
        give();
    }

    void NcParser::give()
    {
        expect(NcLexer::TokenType::reserved, {NcLexer::rGive, NcLexer::rReturn});
        next();
        expression();
        expect(NcLexer::TokenType::symbol, {NcLexer::sSemicolon});
        next();
        m_codeGen.ret();
    }

    void NcParser::expression()
    {
        if (auto result{literals()}; result == Result::failure)
        {
            unaryOperation();
        }
    }

    NcParser::Result NcParser::literals()
    {
        auto result = expect(NcLexer::TokenType::literal, {NcLexer::lInt32, null_str});
        
        if (result == Result::sucess)
        {
            if (m_safe_token != null_str)
            m_codeGen.mov({"$", m_safe_token, ", %", "eax"});
            
    
            next();
        }

        return result;
    }

    NcParser::Result NcParser::unaryOperation()
    {
        auto result = expect
        (
            NcLexer::TokenType::symbol,
            {NcLexer::sMinus, NcLexer::sBitwiseComplement, NcLexer::sLogicalNegation}, //for now no null string
            "expected an expression"
        );

        if (result == Result::sucess)
        {
            std::string uOperator{m_safe_token.data()};
            next();
            expression();

            if (uOperator == NcLexer::sMinus)
            {
                m_codeGen.neg("%eax");
            }
            else if (uOperator == NcLexer::sBitwiseComplement)
            {
                m_codeGen.neg("%eax");
            }
            else if (uOperator == NcLexer::sLogicalNegation)
            {
                m_codeGen.comp({"$0", ", ", "%eax"});
                m_codeGen.mov({"$0", ", ", "%eax"});
                m_codeGen.sete("%al");
            }
        }
        
        return result;
    }

    NcParser::Result NcParser::expect(NcLexer::TokenType tokenTypeToMatch, std::vector<std::string_view> tokenStringsToMatch, std::string_view optErrorMsg)
    {
        m_error.empty_str();

        /*The way i structured the following code is odd, but efficient, let's go through it shall we
        1. It first checks if the generic token type given is a match.
        2. If true it then declares a try block to catch any expected or unexpected expections, while the operation is happening
        3. in the try block, it first writes the pontential error message, so as to not be caught off guard by .at(size_t)
        throwing an exception
        4. Since the token list(m_tokenList) contains metadata about the tokens it has, we gracious use one of them to first check
        if it matches the generic token type expected, if true we then we compare the token string given to see if it actually matches
        the current token string in the list.
        5. You might be thinking why are you throwing exceptions anyhow in your code = because it is safe to assume that any wrong thing
        that might happen during this operation constitues a failure which this function is meant to handle by shutting down
        the process and printing an error message to the console, so any wrong thing might be:
        a. Variable m_currentToken might be greater or equal to the size of the m_tokenList vector, so we use member function .at(size_t)
        that throws an exception whenever such is the case.
        b. Everything is alright up to this point, but to avoid unneccasry string comparison, we first check if the current token type is the
        token type expected, by using meta data stored in the m_tokenList, if it is not we throw an expection
        c. Once the generic token type is match, we then compare the token strings, that token string(1) is the current token while (2) is
        the expected token given as an argument to the function. If it fails, we still throw an exception(because, remember every wrong that
        happens constitutes a failure)
        */
       
       std::string allTokens{};
       
       if (m_currentToken >= m_tokenList.size())
       {
            m_safe_token = null_str;
       }
       else
       {
            m_safe_token = m_tokenList.at(m_currentToken).token;
       }
       
       

       for (auto &&i : tokenStringsToMatch)
       {
            allTokens += i;

            if (i != tokenStringsToMatch.back())
            allTokens += " or ";      
       }
    
        switch (tokenTypeToMatch)
        {
        case NcLexer::TokenType::reserved:

            try
            {
                if (m_tokenList.at(m_currentToken).isReserved)
                {
                    for (auto &&i : tokenStringsToMatch)
                    {
                        if (m_tokenList.at(m_currentToken).token == i)
                        return Result::sucess;
                    }
                    //throw when nothing it did not match
                    throw std::runtime_error{""}; 
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (tokenStringsToMatch.back() != null_str)
                {
                    if (optErrorMsg == null_str)
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR] expected", allTokens);
                        else
                        m_error.writews("[SYNTAX ERROR] expected", allTokens, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    else
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg);
                        else
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    errorOutput();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::identifier:
            try
            {
                if (m_tokenList.at(m_currentToken).isIdentifier)
                return Result::sucess;
            
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (tokenStringsToMatch.back() != null_str)
                {
                    if (optErrorMsg == null_str)
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.write("[SYNTAX ERROR] expected an identifier");
                        else
                        m_error.writews("[SYNTAX ERROR] expected an identifier", "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    else
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg);
                        else
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    errorOutput();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::literal:
            try
            {
                if (m_tokenList.at(m_currentToken).literalType != "")
                {
                    for (auto &&i : tokenStringsToMatch)
                    {
                        if (NcLexer::lInt32 == i)
                        {
                            try
                            {
                                size_t pos{}; //is irrelevant to the code at hand, but need in function std::stol
                                [[maybe_unused]] auto variable = std::stoi(m_tokenList.at(m_currentToken).token, &pos);
                            }
                            catch(const std::exception&)
                            { throw; }
                            
                            return Result::sucess;
                        }
                    }
                    //throw when nothing it did not match
                    throw std::runtime_error{""}; 
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            { 
                if (tokenStringsToMatch.back() != null_str)
                {
                    if (optErrorMsg == null_str)
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR] expected", allTokens);   
                        else
                        m_error.writews("[SYNTAX ERROR] expected", allTokens, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    else
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg);
                        else
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    errorOutput();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::symbol:
            try
            {
                if (m_tokenList.at(m_currentToken).isSymbol)
                {
                    for (auto &&i : tokenStringsToMatch)
                    {
                        if (m_tokenList.at(m_currentToken).token == i)
                        return Result::sucess;
                    }
                    //throw when nothing it did not match
                    throw std::runtime_error{""};
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (tokenStringsToMatch.back() != null_str)
                {
                    if (optErrorMsg == null_str)
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR] expected", allTokens);   
                        else
                        m_error.writews("[SYNTAX ERROR] expected", allTokens, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    else
                    {
                        if (m_currentToken >= m_tokenList.size())
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg);
                        else
                        m_error.writews("[SYNTAX ERROR]", optErrorMsg, "before token [", m_tokenList.at(m_currentToken).token, "]");
                    }
                    errorOutput();
                }

                return Result::failure;
            }

        default:
            throw std::invalid_argument{"In fucntion Nc::NcParser::expect variable tokenTypeToMatch contains a value that was not expected"};
            break;
        }

        //just needed to keep compiler warnings at bay
        return Result::failure;
    }
    
    void NcParser::next()
    { ++m_currentToken; }

    [[noreturn]] void NcParser::errorOutput()
    {
        Myfcn::DrawBoxAroundText boxify{System::coutput};
        
        const auto maxBoxSize{ std::max(m_error.get_str().length(), 40U) };

        boxify.style(Myfcn::DrawBoxAroundText::Style::doublelined);

        boxify.topLine(maxBoxSize);
        boxify.spacing(m_error.get_str(), maxBoxSize);
        m_error.empty_str();

        if (m_currentToken >= m_tokenList.size())
        m_error.writews("            line:", m_tokenList.at(m_tokenList.size()-1).line, "column:", m_tokenList.at(m_tokenList.size()-1).column);
        else
        m_error.writews("            line:", m_tokenList.at(m_currentToken).line, "column:", m_tokenList.at(m_currentToken).column);
        
        boxify.spacing(m_error.get_str(), maxBoxSize);
        m_error.insert_str("{compilation terminated with an error}");
        boxify.spacing(m_error.get_str(), maxBoxSize);
        boxify.downLine(maxBoxSize);

        m_codeGen.~NcCodeGen();
        m_tokenList.~vector();
        m_currentToken = {};
        m_error.~StringOutputStream();
        std::exit(EXIT_FAILURE);
    }

} // namespace Nc
