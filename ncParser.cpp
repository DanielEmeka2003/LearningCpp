#include <iterator>
#include <format>

#include "ncParser.h"
#include "drawboxaroundtext.h"
#include "stream/output.h"

namespace Nc
{
    void NcParser::ValueCategory::setAsLvalue()
    { m_value = lvalue; }
    void NcParser::ValueCategory::setAsRvalue()
    { m_value = rvalue; }
    
    bool NcParser::ValueCategory::isLvalue()
    { return m_value == lvalue; }
    bool NcParser::ValueCategory::isRvalue()
    { return m_value == rvalue; }
            


    NcParser::NcParser(const std::vector<NcLexer::TokenData>& tokenList): m_tokenList{tokenList}
    {}

    void NcParser::parseTokenList()
    {
        ncfile();

        if (!m_log.isLogEmpty())
        {
            errorOutput();
        }
        
        m_ast.makeRootNode();
    }

    void NcParser::logBoxStyle(NcLog::BoxLineStyle s)
    { m_log.changeBoxLineStyle(s); }

    NcAST& NcParser::getAST()
    { return m_ast; }

    const NcAST& NcParser::getAST()const
    { return m_ast; }



    void NcParser::ncfile()
    {
        main();

        m_ast.makeFuncDeclarationNode();
        m_ast.mkNcFileNode();
    }

    void NcParser::main()
    {
        if (expect(NcLexer::TokenType::reserved, {NcLexer::rMain}, true) == Result::success)
        {
            //m_codeGen.mainFcnSetup();
            next();

            expect(NcLexer::TokenType::symbol, {NcLexer::sLparen}, true);
            next();

            expect(NcLexer::TokenType::symbol, {NcLexer::sRparen}, true);
            next();

            expect(NcLexer::TokenType::symbol, {NcLexer::sColon}, true);
            next();

            expect(NcLexer::TokenType::reserved, {NcLexer::rInt32}, true);
            next();

            m_ast.mkFunctionNode_type_ident(NcLexer::rInt32, NcLexer::rMain);

            block();

            return;
        }
    
    }


    void NcParser::block()
    {
        expect(NcLexer::TokenType::symbol, {NcLexer::sLcurly}, true);
        next();

        while (block_item() == Result::success)
        {
            m_ast.makeBlockItemNode();
            m_ast.mkFunctionNode_blockItem();
        }

        expect(NcLexer::TokenType::symbol, {NcLexer::sRcurly}, true);
        next();
    }

    NcParser::Result NcParser::block_item(bool shouldReport, std::string_view optErrorMsg)
    {
        if (declaration() == Result::success)
        {
            return Result::success;
        }
        else if (statement(shouldReport, optErrorMsg) == Result::success)
        {
            m_ast.makeStatementNode();

            return Result::success;
        }

        return Result::failure;
    }

    NcParser::Result NcParser::declaration(bool shouldReport, std::string_view optErrorMsg)
    {
        if (expect(NcLexer::TokenType::reserved, {NcLexer::rInt32}, shouldReport, optErrorMsg) == Result::success)
        {
            next();

            m_ast.mkVarDeclareNode_type(NcLexer::rInt32);

            expect(NcLexer::TokenType::identifier, {}, true);
                        
            std::string variableName{ m_safe_token.data() };

            if (m_variableMap.contains(variableName))
            {
                m_log.writews("[VIOLATION] variable", variableName, "has already been declared");
                finalizeLog();
            }
            else
            {
                //calculate the next avialable memory location
                m_stackIndex -= 4;
                //insert the variable to the map
                m_variableMap.insert_or_assign(variableName, m_stackIndex);
            }
            next();
            
            if (expect(NcLexer::TokenType::symbol, {NcLexer::sAssign}) == Result::success)
            {
                next();

                assignment_exp(true, "expected an expression");

                m_ast.makeExpNode();
            }
            else
            {
                m_ast.mkLiteralNode("0", NcLexer::lInt32);
                m_ast.makeExpNode();
            }
            
            m_ast.mkVarDeclareNode_ident_init(variableName);
            
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sComma}) != Result::failure)
            {
                next();

                expect(NcLexer::TokenType::identifier, {}, true);
                
                variableName = m_safe_token.data();

                if (m_variableMap.contains(variableName))
                {
                    m_log.writews("[VIOLATION] variable", variableName, "has already been declared");
                    finalizeLog();
                }
                else
                {
                    //calculate the next avialable memory location
                    m_stackIndex -= 4;
                    //insert the variable to the map
                    m_variableMap.insert_or_assign(variableName, m_stackIndex);
                }
                next();

                if (expect(NcLexer::TokenType::symbol, {NcLexer::sAssign}) == Result::success)
                {
                    next();

                    assignment_exp(true, "expected an expression");

                    m_ast.makeExpNode();
                }
                else
                {
                    m_ast.mkLiteralNode("0", NcLexer::lInt32);
                    m_ast.makeExpNode();
                }

                m_ast.mkVarDeclareNode_ident_init(variableName);
            }
            
            m_ast.makeVarDeclarationNode();

            if (expect(NcLexer::TokenType::symbol, {NcLexer::sSemicolon}) == Result::success)
            next();
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::statement(bool shouldReport, std::string_view optErrorMsg)
    {
        if (give() == Result::success)
        {
            m_ast.mkGiveNode();

            if (expect(NcLexer::TokenType::symbol, {NcLexer::sSemicolon}) == Result::success)
            next();

            return Result::success;
        }
        else if (if_condition() == Result::success)
        {
            return Result::success;
        }
        else if (expression(shouldReport, optErrorMsg) == Result::success) //here for a reason
        {
            m_ast.makeExpNode();

            if (expect(NcLexer::TokenType::symbol, {NcLexer::sSemicolon}) == Result::success)
            next();
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::if_condition(bool shouldReport, std::string_view optErrorMsg)
    {
        if (expect(NcLexer::TokenType::reserved, {NcLexer::rIf}, shouldReport, optErrorMsg) == Result::success)
        {
            next();

            expect(NcLexer::TokenType::symbol, {NcLexer::sLparen}, true);
            next();

            expression(true, "expected an expression");

            expect(NcLexer::TokenType::symbol, {NcLexer::sRparen}, true);
            next();

            statement(true, "expected a statement");

            if (expect(NcLexer::TokenType::reserved, {NcLexer::rElse}) == Result::success)
            {
                next();
                
                statement(true, "expected a statement");
            }

            return Result::success;
        }

        return Result::failure;
    }

    NcParser::Result NcParser::give(bool shouldReport, std::string_view optErrorMsg)
    {
        if (expect(NcLexer::TokenType::reserved, {NcLexer::rGive}, shouldReport, optErrorMsg) == Result::success)
        {
            next();

            expression(true, "expected an expression");

            m_ast.makeExpNode();
            
            return Result::success;
        }
        
        return Result::failure;
    }



    NcParser::Result NcParser::expression(bool shouldReport, std::string_view optErrorMsg)
    {
        if (assignment_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sComma}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                assignment_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sComma, right_exp);
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::assignment_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (conditional_exp(shouldReport, optErrorMsg) == Result::success)
        {
            if (expect(NcLexer::TokenType::symbol,
            {NcLexer::sAssign, NcLexer::sAdditionAssign, NcLexer::sSubtractionAssign, NcLexer::sMultiplicationAssign,
            NcLexer::sDivisionAssign, NcLexer::sModuluoAssign, NcLexer::sBitwiseAndAssign, NcLexer::sBitwiseOrAssign,
            NcLexer::sBitwiseXorAssign, NcLexer::sBitwiseShiftLeftAssign, NcLexer::sBitwiseShiftRightAssign}) != Result::failure)
            {
                if (m_valueCategory.isRvalue())
                {
                    m_log.write("[SYNTAX ERROR] left operand of an assignment operator must be lvalue");
                    finalizeLog();
                }
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);
                
                std::string b_op{m_safe_token};

                assignment_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);

                m_valueCategory.setAsLvalue();
            }

            return Result::success;
        }

        return Result::failure;
    }

    NcParser::Result NcParser::conditional_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (logicalOr_exp(shouldReport, optErrorMsg) == Result::success)
        {
            if (expect(NcLexer::TokenType::symbol, {NcLexer::sQuestionMark}) == Result::success)
            {
                next();

                assignment_exp(true, "expected an expression");

                expect(NcLexer::TokenType::symbol, {NcLexer::sColon}, true);
                next();

                assignment_exp(true, "expected an expression");

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }

        return Result::failure;
    }

    NcParser::Result NcParser::logicalOr_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (logicalXor_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sOr}) != Result::failure 
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rOr}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                logicalXor_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sOr, right_exp);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::logicalXor_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (logicalAnd_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sXor}) != Result::failure 
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rXor}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                logicalAnd_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sXor, right_exp);

                m_valueCategory.setAsRvalue();
            }

            return Result::success;
        }
        
        return Result::failure;   
    }

    NcParser::Result NcParser::logicalAnd_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (bitwiseOr_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sAnd}) != Result::failure
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rAnd}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                bitwiseOr_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sAnd, right_exp);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::bitwiseOr_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (bitwiseXor_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sBitwiseOr}) != Result::failure 
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rBitwiseOr}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                bitwiseXor_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sBitwiseOr, right_exp);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::bitwiseXor_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (bitwiseAnd_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sBitwiseXor}) != Result::failure 
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rBitwiseXor}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                bitwiseAnd_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sBitwiseXor, right_exp);

                m_valueCategory.setAsRvalue();
            }
        
           return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::bitwiseAnd_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (equality_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sBitwiseAnd}) != Result::failure 
            or
            expect(NcLexer::TokenType::reserved, {NcLexer::rBitwiseAnd}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                equality_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, NcLexer::sBitwiseAnd, right_exp);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::equality_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (relational_exp(shouldReport, optErrorMsg) == Result::success)
        {

            while (expect(NcLexer::TokenType::symbol, {NcLexer::sEqual, NcLexer::sNotEqual}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                std::string b_op{m_safe_token};

                relational_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);
                
                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;    
        }

        return Result::failure;
    }

    NcParser::Result NcParser::relational_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (bitwiseShift_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol,
            {NcLexer::sLessthan, NcLexer::sGreaterthan, NcLexer::sLessthan_equalto, NcLexer::sGreaterthan_equalto}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                std::string b_op{m_safe_token};

                bitwiseShift_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);
            
                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }
    
    NcParser::Result NcParser::bitwiseShift_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (additive_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sBitwiseShiftLeft, NcLexer::sBitwiseShiftRight}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                std::string b_op{m_safe_token};

                additive_exp(true, "expected an expression");
                
                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);

                m_valueCategory.setAsRvalue();                
            }

            return Result::success;    
        }

        return Result::failure;
    }

    NcParser::Result NcParser::additive_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (multiplicative_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sPlus, NcLexer::sMinus}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);
                
                std::string b_op{m_safe_token};

                multiplicative_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::multiplicative_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (cast_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sMultiply, NcLexer::sDivide, NcLexer::sModuluo}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);
                
                std::string b_op{m_safe_token};
                
                cast_exp(true, "expected an expression");

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);
   
                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::cast_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (prefix_exp(shouldReport, optErrorMsg) == Result::success)
        {
            while (expect(NcLexer::TokenType::symbol, {NcLexer::sArrow}) != Result::failure)
            {
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);
                
                std::string b_op{m_safe_token};

                expect(NcLexer::TokenType::reserved, {NcLexer::rInt32}, true);
                next();

                m_ast.mkTypeNode(m_safe_token);

                Expression right_exp{};
                m_ast.mkLocalExpNode(right_exp);

                m_ast.mkBinaryNode(left_exp, b_op, right_exp);
   
                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::prefix_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if
        (expect(NcLexer::TokenType::symbol, 
        {NcLexer::sMinus, NcLexer::sBitwiseNot, NcLexer::sNot, NcLexer::sPlus, NcLexer::sIncrement, NcLexer::sDecrement}) == Result::success
            or
        expect(NcLexer::TokenType::reserved, {NcLexer::rBitwiseNot, NcLexer::rNot, NcLexer::rOdd}) == Result::success
        )
        {
            next();

            std::string u_op{m_safe_token};

            prefix_exp(true, "expected an expression");

            if (m_valueCategory.isRvalue() and (u_op == NcLexer::sIncrement or u_op == NcLexer::sDecrement))
            {
                m_log.write("[SYNTAX ERROR] left operand of an assignment operator must be lvalue");
                finalizeLog();
            }

            Expression right_exp{};
            m_ast.mkLocalExpNode(right_exp);

            m_ast.mkUprefixNode(u_op, right_exp); 
            
            if (u_op == NcLexer::sIncrement or u_op == NcLexer::sDecrement)
            m_valueCategory.setAsLvalue();
            else
            m_valueCategory.setAsRvalue();
            
            return Result::success;
        }
        else if (postfix_exp(shouldReport, optErrorMsg) == Result::success)
        {
            return Result::success;
        }
            
        return Result::failure;
    }

    NcParser::Result NcParser::postfix_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (primary_exp(shouldReport, optErrorMsg) == Result::success)
        {

            while (expect(NcLexer::TokenType::symbol, {NcLexer::sIncrement, NcLexer::sDecrement}) == Result::success)
            {
                if (m_valueCategory.isRvalue())
                {
                    m_log.write("[SYNTAX ERROR] left operand of an assignment operator must be lvalue");
                    finalizeLog();
                }
                next();

                Expression left_exp{};
                m_ast.mkLocalExpNode(left_exp);

                std::string u_op{m_safe_token.data()};

                m_ast.mkUpostfixNode(left_exp, u_op);

                m_valueCategory.setAsRvalue();
            }
            
            return Result::success;
        }
        
        return Result::failure;
    }

    NcParser::Result NcParser::primary_exp(bool shouldReport, std::string_view optErrorMsg)
    {
        if (expect(NcLexer::TokenType::symbol, {NcLexer::sLparen}) == Result::success)
        {
            next();

            assignment_exp(true, "expected an expression");
            
            expect(NcLexer::TokenType::symbol, {NcLexer::sRparen}, true);
            next();

            return Result::success;
        }
        else if (expect(NcLexer::TokenType::identifier, {}) == Result::success)
        {

            if (!m_variableMap.contains(m_safe_token.data()))
            {
                m_log.writews("[VIOLATION ERROR] variable", m_safe_token, "has not been declared in this scope");
                finalizeLog();
            }
            next();

            m_ast.mkVarNode(m_safe_token);
            
            m_valueCategory.setAsLvalue();
            
            return Result::success;
        }
        else if (expect(NcLexer::TokenType::literal, {NcLexer::lInt32}, shouldReport, optErrorMsg) == Result::success)
        {
            next();

            m_ast.mkLiteralNode(m_safe_token, NcLexer::lInt32);

            m_valueCategory.setAsRvalue();

            return Result::success;
        }

        if (shouldReport)
        {
            next();
        }
        
        return Result::failure;
    }
   


    NcParser::Result NcParser::expect
    (NcLexer::TokenType tokenType, std::vector<std::string_view> tokenStr, bool shouldReport, std::string_view optErrorMsg)
    {  
       std::string allTokens{};
       
       try
       {
            m_safe_token = m_tokenList.at(m_currentToken).token;
       }
       catch(const std::exception&)
       {
            m_safe_token = null_str;
       }
       
       for (auto &&i : tokenStr)
       {
            allTokens += i;

            if (i != tokenStr.back())
            allTokens += " or ";      
       }
    
        switch (tokenType)
        {
        case NcLexer::TokenType::reserved:
            try
            {
                if (m_tokenList.at(m_currentToken).isReserved)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_tokenList.at(m_currentToken).token == i)
                        return Result::success;
                    }
                    //throw when nothing matches
                    throw std::runtime_error{""}; 
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (shouldReport)
                {
                    if (optErrorMsg == null_str)
                    {   
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, "] instead of token [", tokenData.token, "]"); 
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, ']');
                        }
                    }
                    else
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR]", optErrorMsg, "instead of token [", tokenData.token, "]");
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR]", optErrorMsg);
                        }
                    }
                    finalizeLog();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::identifier:
            try
            {
                if (m_tokenList.at(m_currentToken).isIdentifier)
                return Result::success;
            
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (shouldReport)
                {
                    if (optErrorMsg == null_str)
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR] expected an identifier instead of token [", tokenData.token, "]");   
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR] expected an identifier");
                        }
                    }
                    else
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR]", optErrorMsg, "instead of token [", tokenData.token, "]");
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR]", optErrorMsg);
                        }
                    }
                    finalizeLog();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::literal:
            try
            {
                if (m_tokenList.at(m_currentToken).literalType != "")
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_tokenList.at(m_currentToken).literalType == i)
                        {   
                            return Result::success;
                        }
                    }
                    //throw when nothing matches
                    throw std::runtime_error{""}; 
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            { 
                if (shouldReport)
                {
                    if (optErrorMsg == null_str)
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, "] instead of token [", tokenData.token, "]");
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, ']');
                        }
                    }
                    else
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR]", optErrorMsg, "instead of token [", tokenData.token, "]");
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR]", optErrorMsg);
                        }
                    }
                    finalizeLog();
                }

                return Result::failure;
            }
            
        case NcLexer::TokenType::symbol:
            try
            {
                if (m_tokenList.at(m_currentToken).isSymbol)
                {
                    for (auto &&i : tokenStr)
                    {
                        if (m_tokenList.at(m_currentToken).token == i)
                        return Result::success;
                    }
                    //throw when nothing it did not match
                    throw std::runtime_error{""};
                }
                else
                throw std::runtime_error{""};
            }
            catch(const std::exception&)
            {
                if (shouldReport)
                {
                    if (optErrorMsg == null_str)
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, "] instead of token [", tokenData.token, "]");   
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR] expected [", allTokens, ']');
                        }
                    }
                    else
                    {
                        try
                        {
                            const auto& tokenData{m_tokenList.at(m_currentToken)};

                            m_log.writews("[SYNTAX ERROR]", optErrorMsg, "instead of token [", tokenData.token, "]");
                        }
                        catch(const std::exception&)
                        {
                            m_log.writews("[SYNTAX ERROR]", optErrorMsg);
                        }
                    }
                    finalizeLog();
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

    void NcParser::finalizeLog()
    {
        m_log.log();

        try
        {
            const auto& tokenData{ m_tokenList.at(m_currentToken) };

            m_log.writews("   line:", tokenData.line, "from column: [", tokenData.relativeColumn, '-', tokenData.absoluteColumn, ']');
        }
        catch(const std::exception&)
        {
            const auto& tokenData{ m_tokenList.back() };

            m_log.writews("   line:", tokenData.line, "column:", tokenData.absoluteColumn);
        }
        m_log.log();

        //ensures ast nodes aren't made anymore
        m_ast.setDeadZoneFlag();
    }

    [[noreturn]] void NcParser::errorOutput()
    {
        m_log.write("{compilation terminated with an error}");
        m_log.log();
        
        m_log.outputLogEntries(System::coutput);

        m_tokenList.~vector();
        m_currentToken = {};
        m_safe_token.~basic_string_view();
        m_log.~NcLog();
        m_variableMap.~unordered_map();
        m_stackIndex = {};
        m_valueCategory.~ValueCategory();
        m_ast.~NcAST();

        std::exit(EXIT_FAILURE);
    }

} // namespace Nc
