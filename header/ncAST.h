#ifndef NCAST_H
#define NCAST_H

#include "ncNonTerminal.h"

namespace Nc
{
    class NcAST
    {
    private:
        using DeadZoneFlag = bool;
        
        DeadZoneFlag m_isDeadZone{};


        Type m_type{};
        Literal m_literal{};
        Variable m_var{};
        UPrefixOperation m_uPreOp{};
        UPostfixOperation m_uPostOp{};
        BinaryOperation m_binaryOp{};
        Expression m_exp{};

        Give m_give{};
        Statement m_statement{};

        VarDeclare m_varDeclare{};
        VarDeclaration m_varDeclaration{};

        BlockItem m_blockItem{};

        Function m_func{};
        FuncDeclaration m_funcDeclaration{};

        NcFile m_ncFile{};
        Root m_root{};

    public:
        NcAST() = default;
        NcAST(const NcAST&) = default;
        NcAST(NcAST&&) = default;

        NcAST& operator=(const NcAST&) = default;
        NcAST& operator=(NcAST&&) = default;

        ~NcAST() = default;

        void makeRootNode();

        void mkNcFileNode();

        void makeFuncDeclarationNode();

        void mkFunctionNode_blockItem();
        void mkFunctionNode_type_ident(std::string_view type, std::string_view ident);

        void makeBlockItemNode();

        void makeVarDeclarationNode();

        void mkVarDeclareNode_type(std::string_view type);
        void mkVarDeclareNode_ident_init(std::string_view ident);

        void makeStatementNode();

        void mkGiveNode();

        void makeExpNode();

        void mkTypeNode(std::string_view type);
        void mkLiteralNode(std::string_view l, std::string_view lt);
        void mkVarNode(std::string_view ident);
        void mkUprefixNode(std::string_view op, Expression& exp_ref);
        void mkUpostfixNode(Expression& exp_ref, std::string_view op);
        void mkBinaryNode(Expression& exp_ref1, std::string_view op, Expression& exp_ref2);

        void mkLocalExpNode(Expression& exp_ref);

        bool isRooTNodeEmpty();

        void setDeadZoneFlag();

        operator Root&();
    };

} // namespace nc

#endif