#ifndef NCAST_H
#define NCAST_H

#pragma once

#include "ncNonTerminal.h"
namespace Nc
{
    class Ast
    {
    private:
        using DeadZoneFlag = bool;
        using IdentInitPairList = List<std::pair<Identifier, std::optional<Expression>>>;
        
        DeadZoneFlag m_isDeadZone{};


        ExpressionList m_expressionList{};
        ParameterList m_parameterList{};
        IdentInitPairList m_identInitPairList{};

        Type::FunctionType m_funcType{};
        Type m_type{};

        R_And_L_Exp m_r_and_l_exp{};
        AnyTypeListExp m_anyTypeListExp{};
        TempVarDecl m_tempVarDecl{};
        Literal m_literal{};
        Identifier m_ident{};
        UPrefixOperation m_uPreOp{};
        UPostfixOperation m_uPostOp{};
        BinaryOperation m_binaryOp{};
        TenaryOperation m_tenaryOp{};
        Expression m_exp{};

        Give m_giveJS{};
        Break m_breakJS{};
        Continue m_continueJS{};
        JumpStatement m_jumpStatement{};

        IfCondition m_ifCondition{};
        ForLoop m_forLoop{};
        WhileLoop m_whileLoop{};
        DoWhileLoop m_doWhileLoop{};

        Statement m_statement{};

        VarDecl m_varDecl{};
        VarDeclaration m_varDeclaration{};

        BlockItem m_blockItem{};

        Block m_block{};

        FunctionDecl m_funcDecl{};
        FunctionDef m_funcDef{};
        FuncDeclaration m_funcDeclaration{};

        NcFile m_ncFile{};
        Root m_root{};

    public:
        Ast() = default;
        Ast(const Ast&) = default;
        Ast(Ast&&) = default;

        Ast& operator=(const Ast&) = default;
        Ast& operator=(Ast&&) = default;

        ~Ast() = default;

        void makeRootNode();

        void mkNcFileNode();

        FuncDeclaration& getFuncDeclarationNode();
        void makeFuncDeclarationNode();

        void mkFunctionDefNode(Type& giveType, Identifier& functionIdent, ParameterList& parameterList, Block& block);
        void mkFunctionDeclNode(Type& giveType, Identifier& functionIdent, ParameterList& parameterList);

        Block& getBlockNode();
        void makeBlockNode(List<BlockItem>& blockItemList, LocationFinder& locFinder);
        void makeBlockNode(Void& v, LocationFinder& locFinder);

        BlockItem& getBlockItemNode();
        void makeBlockItemNode();

        VarDeclaration& getVarDeclaration();
        void makeVarDeclarationNode();

        void mkVarDeclNode(Type& varType, IdentInitPairList& identInitPairList, LocationFinder& locFinder);

        Statement& getStatementNode();
        void makeStatementNode();

        void mkNullStatementNode(LocationFinder& locFinder){}

        void mkJumpStatementNode();
        void mkGiveJSNode(Expression& exp, LocationFinder& locFinder);
        void mkGiveJSNode(Void& v, LocationFinder& locFinder);
        void mkBreakJSNode(LocationFinder& locFinder);
        void mkContinueJSNode(LocationFinder& locFinder);

        void mkIfConditionNode(Expression& conditionExp, Statement& statementIfTrue, std::optional<Statement>& statementIfFalse);
        void mkForLoopNode(VarDeclaration& localVarDecl, Expression& exp1, Expression& exp2, Statement& statement);
        void mkWhileLoopNode(Expression& conditionExp, Statement& statement);
        void mkDoWhileLoopNode(Statement& statement, Expression& whileConditionExp);

        Expression& getExpNode();
        void makeExpNode();
        
        AnyTypeListExp& getAnyTypeListExpNode();
        
        void mkR_And_L_Exp_Node(Operator& relationalOp, Operator& logicalOp, Expression& exp, ExpressionList& expList);
        void mkAnyTypeListExpNode(ExpressionList& expList, LocationFinder& locFinder);
        void mkAnyTypeListExpNode(Void& v, LocationFinder& locFinder);
        void mkTempVarDeclNode(Type& variableType, AnyTypeListExp& initializer, LocationFinder& locFinder);
        void mkLiteralNode(Literal::LiteralConstant& literalConst, Myfcn::U8string& literalType, LocationFinder& locFinder);
        void mkIdentifierNode(Identifier::IdentifierName& identName, LocationFinder& locFinder);
        void mkUprefixOpNode(Operator& op, Expression& rightExp, LocationFinder& locFinder);
        void mkUpostfixOpNode(Expression& leftExp, ExpressionList& op, LocationFinder& locFinder);
        void mkUpostfixOpNode(Expression& leftExp, Operator& op, LocationFinder& locFinder);
        void mkBinaryOpNode(Expression& leftExp, Operator& op, Expression& rightExp, LocationFinder& locFinder);
        void mkTenaryOpNode(Expression& conditionExp, Operator& op, Expression& leftExp, Expression& rightExp, LocationFinder& locFinder);

        Type& _getType();
        void _makeType(Type::NormalType& Ntype, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder);
        void _makeType(Type::FunctionType& Ftype, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder);

        Type::FunctionType& _getFunctionType();
        void _makeFunctionType(List<Type>& parameterList, Type& giveType);

        IdentInitPairList& _getIdentInitPairList();
        void _pushBack_identInitPair(Identifier& ident, Expression& optionalInit);
        void _pushBack_identInitPair(Identifier& ident);
        ExpressionList& _getExpressionList();
        void _pushBack_expression(Expression& argumentExp);
        ParameterList& _getParameterList();
        void _pushBack_parameter(Type& type, std::optional<Identifier>& optionalIdentifier);

        void mkLocalExpNode(Expression& exp_ref);

        bool isRooTNodeEmpty();

        void setDeadZoneFlag();

        operator Root&();
    };

} // namespace nc

#endif