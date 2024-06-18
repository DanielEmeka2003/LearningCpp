
#include "ncAst.h"

namespace Nc
{
    void Ast::makeRootNode()
    {
        if (m_isDeadZone)
        return;

        m_root = Root{ std::move(m_ncFile) };
    }

    void Ast::mkNcFileNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_varDeclaration.isEmpty())
        m_ncFile.setGlobalContents(mk_dyNode(m_varDeclaration));
        else if (!m_funcDeclaration.isEmpty())
        m_ncFile.setGlobalContents(mk_dyNode(m_funcDeclaration));
    }

    FuncDeclaration& Ast::getFuncDeclarationNode()
    { return m_funcDeclaration; }

    void Ast::makeFuncDeclarationNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_funcDecl.isEmpty()) m_funcDeclaration.setFuncDeclaration(m_funcDecl);
        else if (!m_funcDef.isEmpty()) m_funcDeclaration.setFuncDeclaration(m_funcDef);
    }

    void Ast::mkFunctionDefNode(Type& giveType, Identifier& functionIdent, ParameterList& parameterList, Block& block)
    {
        if (m_isDeadZone)
        return;

        m_funcDef.setGiveType(giveType);
        m_funcDef.setFunctionIdent(functionIdent);
        m_funcDef.setParameterList(parameterList);
        m_funcDef.setBlock(block);
    }

    void Ast::mkFunctionDeclNode(Type& giveType, Identifier& functionIdent, ParameterList& parameterList)
    {
        if (m_isDeadZone)
        return;

        m_funcDecl.setGiveType(giveType);
        m_funcDecl.setFunctionIdent(functionIdent);
        m_funcDecl.setParameterList(parameterList);
    }
    
    Block& Ast::getBlockNode()
    { return m_block; }

    void Ast::makeBlockNode(List<BlockItem>& blockItemList, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        List<DynamicNode<BlockItem>> bIList{};

        for (auto &&i : blockItemList)
        bIList.push_back(mk_dyNode(i));

        m_block.setBlockContents(bIList);
        m_block.setLocationFinder(locFinder);

        blockItemList.clear();
    }

    void Ast::makeBlockNode(Void& v, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_block.setBlockContents(v);
        m_block.setLocationFinder(locFinder);
    }
    
    BlockItem& Ast::getBlockItemNode()
    { return m_blockItem; }

    void Ast::makeBlockItemNode()
    {
        if (m_isDeadZone)
        return;
        
        if (!m_statement.isEmpty()) m_blockItem.setBlockItem(m_statement);
        else if (!m_varDeclaration.isEmpty()) m_blockItem.setBlockItem(m_varDeclaration);
    }

    VarDeclaration& Ast::getVarDeclaration()
    { return m_varDeclaration; }

    void Ast::makeVarDeclarationNode()
    {
        if (m_isDeadZone)
        return;

        m_varDeclaration.setVarDecl(m_varDecl);
    }

    void Ast::mkVarDeclNode(Type& varType, IdentInitPairList& identInitPairList, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_varDecl.setVarType(varType);
        for (auto&& i : identInitPairList)
        {
            if (i.second.has_value())
            m_varDecl.pushBack_identInitPair(VarDecl::IdentInitPair{ std::move(i.first), std::optional{mk_dyNode(i.second.value())} });
            else
            m_varDecl.pushBack_identInitPair(VarDecl::IdentInitPair{ std::move(i.first), std::optional<DynamicNode<Expression>>{} });
        }
        identInitPairList.clear();
        m_varDecl.setLocationFinder(locFinder);
    }
    
    Statement& Ast::getStatementNode()
    { return m_statement; }

    void Ast::makeStatementNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_jumpStatement.isEmpty()) m_statement.setStatement(m_jumpStatement);
        else if (!m_ifCondition.isEmpty()) m_statement.setStatement(m_ifCondition);
        else if (!m_forLoop.isEmpty()) m_statement.setStatement(m_forLoop);
        else if (!m_whileLoop.isEmpty()) m_statement.setStatement(m_whileLoop);
        else if (!m_doWhileLoop.isEmpty()) m_statement.setStatement(m_doWhileLoop);
        else if (!m_exp.isEmpty()) m_statement.setStatement(m_exp);
    }
    
    void Ast::mkJumpStatementNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_giveJS.isEmpty())
        {
            m_jumpStatement.setJStatement(m_giveJS);
        }
        else if (!m_breakJS.isEmpty())
        {
            m_jumpStatement.setJStatement(m_breakJS);
        }
        else if (!m_continueJS.isEmpty())
        {
            m_jumpStatement.setJStatement(m_continueJS);
        }
    }

    void Ast::mkGiveJSNode(Expression& exp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_giveJS.set_exp_void(mk_dyNode(exp));
        m_giveJS.setLocationFinder(locFinder);
    }

    void Ast::mkGiveJSNode(Void& v, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_giveJS.set_exp_void(v);
        m_giveJS.setLocationFinder(locFinder);
    }

    void Ast::mkBreakJSNode(LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_breakJS.set();
        m_breakJS.setLocationFinder(locFinder);
    }

    void Ast::mkContinueJSNode(LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_continueJS.set();
        m_continueJS.setLocationFinder(locFinder);
    }

    void Ast::mkIfConditionNode(Expression& conditionExp, Statement& statementIfTrue, std::optional<Statement>& statementIfFalse)
    {
        if (m_isDeadZone)
        return;

        m_ifCondition.setConditionExp(mk_dyNode(conditionExp));
        m_ifCondition.setStatementIfTrue(mk_dyNode(statementIfTrue));
        statementIfFalse.has_value() ? m_ifCondition.setStatementIfFalse(mk_dyNode(*statementIfFalse)) : m_ifCondition.setStatementIfFalse({});
    }

    void Ast::mkForLoopNode(VarDeclaration& localVarDecl, Expression& exp1, Expression& exp2, Statement& statement)
    {
        if (m_isDeadZone)
        return;

        m_forLoop.setLocalVarDecl(mk_dyNode(localVarDecl));
        m_forLoop.setExp1(mk_dyNode(exp1));
        m_forLoop.setExp2(mk_dyNode(exp2));
        m_forLoop.setStatement(mk_dyNode(statement));
    }

    void Ast::mkWhileLoopNode(Expression& conditionExp, Statement& statement)
    {
        if (m_isDeadZone)
        return;

        m_whileLoop.setConditionExp(mk_dyNode(conditionExp));
        m_whileLoop.setStatement(mk_dyNode(statement));
    }

    void Ast::mkDoWhileLoopNode(Statement& statement, Expression& whileConditionExp)
    {
        if (m_isDeadZone)
        return;

        m_doWhileLoop.setStatement(mk_dyNode(statement));
        m_doWhileLoop.setWhileConditionExp(mk_dyNode(whileConditionExp));
    }


    Expression& Ast::getExpNode()
    { return m_exp; }

    void Ast::makeExpNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_literal.isEmpty()) m_exp.setExp(m_literal);
        else if (!m_ident.isEmpty()) m_exp.setExp(m_ident);
        else if (!m_binaryOp.isEmpty()) m_exp.setExp(m_binaryOp);
        else if (!m_uPreOp.isEmpty()) m_exp.setExp(m_uPreOp);
        else if (!m_uPostOp.isEmpty()) m_exp.setExp(m_uPostOp);
        else if (!m_tenaryOp.isEmpty()) m_exp.setExp(m_tenaryOp);
        else if (!m_tempVarDecl.isEmpty()) m_exp.setExp(m_tempVarDecl);
        else if (!m_anyTypeListExp.isEmpty()) m_exp.setExp(m_anyTypeListExp);
        else if (!m_block.isEmpty()) m_exp.setExp(m_block);
        else if (!m_r_and_l_exp.isEmpty()) m_exp.setExp(m_r_and_l_exp);
    }


    AnyTypeListExp& Ast::getAnyTypeListExpNode()
    { return m_anyTypeListExp; }


    void Ast::mkR_And_L_Exp_Node(Operator& relationalOp, Operator& logicalOp, Expression& exp, ExpressionList& expList)
    {
        if (m_isDeadZone)
        return;

        m_r_and_l_exp.setRelationalOp(relationalOp);
        m_r_and_l_exp.setLogicalOp(logicalOp);
        m_r_and_l_exp.setExpression(mk_dyNode(exp));

        for (auto&& i : expList)
        m_r_and_l_exp.pushBack_exp(mk_dyNode(i));

        expList.clear();
    }

    void Ast::mkAnyTypeListExpNode(ExpressionList& expList, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        AnyTypeListExp::DynamicExpressionList dyExpList{};
        for (auto&& i : expList)
        dyExpList.push_back(mk_dyNode(i));

        m_anyTypeListExp.setListContents(dyExpList);
        m_anyTypeListExp.setLocationFinder(locFinder);

        expList.clear();
    }

    void Ast::mkAnyTypeListExpNode(Void& v, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_anyTypeListExp.setListContents(v);
        m_anyTypeListExp.setLocationFinder(locFinder);
    }

    void Ast::mkTempVarDeclNode(Type& variableType, AnyTypeListExp& initializer, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_tempVarDecl.setVariableType(variableType);
        m_tempVarDecl.setIntializerList(initializer);
        m_tempVarDecl.setLocationFinder(locFinder);
    }

    void Ast::mkLiteralNode(Literal::LiteralConstant& literalConst, Myfcn::U8string& literalType, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;
        
        m_literal.setLiteralConst(literalConst);
        m_literal.setLocationFinder(locFinder);
        m_literal.setLiteralType(literalType);
    }

    void Ast::mkIdentifierNode(Identifier::IdentifierName& identName, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_ident.setIdentName(identName);
        m_ident.setLocationFinder(locFinder);        
    }

    void Ast::mkUprefixOpNode(Operator& op, Expression& rightExp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_uPreOp.setOperator(op);
        m_uPreOp.setRightExp(mk_dyNode(rightExp));
        m_uPreOp.setLocationFinder(locFinder);
    }

    void Ast::mkUpostfixOpNode(Expression& leftExp, ExpressionList& op, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_uPostOp.setLeftExp(mk_dyNode(leftExp));

        UPostfixOperation::DynamicExpressionList d_expList{};
        for (auto&& i : op)
        d_expList.push_back(mk_dyNode(i));
        
        m_uPostOp.setOperator(d_expList);
        m_uPostOp.setLocationFinder(locFinder);

        op.clear();
    }

    void Ast::mkUpostfixOpNode(Expression& leftExp, Operator& op, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_uPostOp.setLeftExp(mk_dyNode(leftExp));
        m_uPostOp.setOperator(op);
        m_uPostOp.setLocationFinder(locFinder);
    }

    void Ast::mkBinaryOpNode(Expression& leftExp, Operator& op, Expression& rightExp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_binaryOp.setLeftExp(mk_dyNode(leftExp));
        m_binaryOp.setOperator(op);
        m_binaryOp.setRightExp(mk_dyNode(rightExp));
        m_binaryOp.setLocationFinder(locFinder);
    }

    void Ast::mkTenaryOpNode(Expression& conditionExp, Operator& op, Expression& leftExp, Expression& rightExp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_tenaryOp.setConditionExp(mk_dyNode(conditionExp));
        m_tenaryOp.setOperator(op);
        m_tenaryOp.setLeftExp(mk_dyNode(leftExp));
        m_tenaryOp.setRightExp(mk_dyNode(rightExp));
        m_tenaryOp.setLocationFinder(locFinder);
    }


    Type& Ast::_getType()
    { return m_type; }

    void Ast::_makeType(Type::NormalType& Ntype, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_type.setType(Ntype);
        m_type.setTypeAttributesExp(typeAttributesExp);
        m_type.setLocationFinder(locFinder);
    }

    void Ast::_makeType(Type::FunctionType& Ftype, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder)
    {
        if (m_isDeadZone)
        return;

        m_type.setType(Ftype);
        m_type.setTypeAttributesExp(typeAttributesExp);
        m_type.setLocationFinder(locFinder);
    }    
    
    Type::FunctionType& Ast::_getFunctionType()
    { return m_funcType; }

    void Ast::_makeFunctionType(List<Type>& parameterList, Type& giveType)
    {
        if (m_isDeadZone)
        return;

        for (auto&& i : parameterList)
        m_funcType.pushBack_parameterType(mk_dyNode(i));
        
        parameterList.clear();
        m_funcType.setGiveType(mk_dyNode(giveType));
    }

    Ast::IdentInitPairList& Ast::_getIdentInitPairList()
    { return m_identInitPairList; }

    void Ast::_pushBack_identInitPair(Identifier& ident, Expression& optionalInit)
    {
        if (m_isDeadZone)
        return;

        m_identInitPairList.push_back(std::pair{ std::move(ident), std::move(optionalInit) });
    }

    void Ast::_pushBack_identInitPair(Identifier& ident)
    {
        if (m_isDeadZone)
        return;

        m_identInitPairList.push_back(std::pair{ std::move(ident), std::optional<Expression>{} });
    }

    ExpressionList& Ast::_getExpressionList()
    { return m_expressionList; }

    void Ast::_pushBack_expression(Expression& argumentExp)
    {
        if (m_isDeadZone)
        return;

        m_expressionList.push_back(std::move(argumentExp));
    }

    ParameterList& Ast::_getParameterList()
    { return m_parameterList; }

    void Ast::_pushBack_parameter(Type& type, std::optional<Identifier>& optionalIdentifier)
    {
        if (m_isDeadZone)
        return;
        
        m_parameterList.push_back(std::pair{std::move(type), std::move(optionalIdentifier)});
    }



    void Ast::mkLocalExpNode(Expression& exp_ref)
    {
        if (m_isDeadZone)
        return;

        if (!m_literal.isEmpty()) exp_ref.setExp(m_literal);
        else if (!m_ident.isEmpty()) exp_ref.setExp(m_ident);
        else if (!m_binaryOp.isEmpty()) exp_ref.setExp(m_binaryOp);
        else if (!m_uPreOp.isEmpty()) exp_ref.setExp(m_uPreOp);
        else if (!m_uPostOp.isEmpty()) exp_ref.setExp(m_uPostOp);
        else if (!m_tenaryOp.isEmpty()) exp_ref.setExp(m_tenaryOp);
        else if (!m_tempVarDecl.isEmpty()) exp_ref.setExp(m_tempVarDecl);
        else if (!m_anyTypeListExp.isEmpty()) exp_ref.setExp(m_anyTypeListExp);
        else if (!m_block.isEmpty()) exp_ref.setExp(m_block);
        else if (!m_r_and_l_exp.isEmpty()) exp_ref.setExp(m_r_and_l_exp);
    }

    bool Ast::isRooTNodeEmpty()
    { return m_root.isEmpty(); }

    void Ast::setDeadZoneFlag()
    { m_isDeadZone = true; }

    Ast::operator Root&()
    { return m_root; }

} // namespace Nc
