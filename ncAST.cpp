
#include "ncAST.h"

namespace Nc
{
    void NcAST::makeRootNode()
    {
        if (m_isDeadZone)
        return;

        m_root = Root{ std::move(m_ncFile) };
    }

    void NcAST::mkNcFileNode()
    {
        if (m_isDeadZone)
        return;

        m_ncFile.funcList.push_back(new FuncDeclaration{ std::move(m_funcDeclaration) });
    }

    void NcAST::makeFuncDeclarationNode()
    {
        if (m_isDeadZone)
        return;

        m_funcDeclaration.func = std::move(m_func);
    }

    void NcAST::mkFunctionNode_blockItem()
    {
        if (m_isDeadZone)
        return;

        m_func.blockItemList.push_back(new BlockItem{ std::move(m_blockItem) });
    }

    void NcAST::mkFunctionNode_type_ident(std::string_view type, std::string_view ident)
    {
        if (m_isDeadZone)
        return;

        m_func.type = type, m_func.ident = ident;
    }

    void NcAST::makeBlockItemNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_statement.isEmpty())
        {
            m_blockItem = BlockItem{ std::move(m_statement) };
        }
        else if (!m_varDeclaration.isEmpty())
        {
            m_blockItem = BlockItem{ std::move(m_varDeclaration) };
        }
        else
        {
            throw std::runtime_error{ "in function Nc::NcAST::makeBlockItemNode, no condition was met!" };   
        }
    }

    void NcAST::makeVarDeclarationNode()
    {
        if (m_isDeadZone)
        return;

        m_varDeclaration.var_declare = std::move(m_varDeclare);
    }

    void NcAST::mkVarDeclareNode_type(std::string_view type)
    {
        if (m_isDeadZone)
        return;

        m_varDeclare.type = type;
    }

    void NcAST::mkVarDeclareNode_ident_init(std::string_view ident)
    {
        if (m_isDeadZone)
        return;

        m_varDeclare.ident_init_list.emplace_back(ident.data(), new Expression{ std::move(m_exp) });
    }

    void NcAST::makeStatementNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_exp.isEmpty())
        {
            m_statement = Statement{ std::move(m_exp) };
        }
        else if (!m_give.isEmpty())
        {
            m_statement = Statement{ std::move(m_give) };
        }
        else
        {
            throw std::runtime_error{ "in function Nc::NcAST::makeStatementNode, no condition was met!" };   
        }
    }

    void NcAST::mkGiveNode()
    {
        if (m_isDeadZone)
        return;

        m_give = Give{ new Expression{ std::move(m_exp) } };
    }

    void NcAST::makeExpNode()
    {
        if (m_isDeadZone)
        return;

        if (!m_type.isEmpty())
        {
            m_exp = Expression{ std::move(m_type) };
        }
        else if (!m_literal.isEmpty())
        {
            m_exp = Expression{ std::move(m_literal) };
        }
        else if (!m_var.isEmpty())
        {
            m_exp = Expression{ std::move(m_var) };
        }
        else if (!m_binaryOp.isEmpty())
        {
            m_exp = Expression{ std::move(m_binaryOp) };
        }
        else if (!m_uPreOp.isEmpty())
        {
            m_exp = Expression{ std::move(m_uPreOp) };
        }
        else if (!m_uPostOp.isEmpty())
        {
            m_exp = Expression{ std::move(m_uPostOp) };
        }
        else
        {
            throw std::runtime_error{ "in function Nc::NcAST::makeExpNode, no condition was met!" };   
        }
    }

    void NcAST::mkTypeNode(std::string_view type)
    {
        if (m_isDeadZone)
        return;

        m_type = Type{ type.data() };
    }

    void NcAST::mkLiteralNode(std::string_view l, std::string_view lt)
    {
        if (m_isDeadZone)
        return;

        m_literal = Literal{ l.data(), lt.data() };
    }

    void NcAST::mkVarNode(std::string_view ident)
    {
        if (m_isDeadZone)
        return;

        m_var = Variable{ ident.data() };
    }

    void NcAST::mkUprefixNode(std::string_view op, Expression& exp_ref)
    {
        if (m_isDeadZone)
        return;

        m_uPreOp = { op.data(), new Expression{ std::move(exp_ref) } };
    }

    void NcAST::mkUpostfixNode(Expression& exp_ref, std::string_view op)
    {
        if (m_isDeadZone)
        return;

        m_uPostOp = { new Expression{ std::move(exp_ref) }, op.data() };
    }

    void NcAST::mkBinaryNode(Expression& exp_ref1, std::string_view op, Expression& exp_ref2)
    {
        if (m_isDeadZone)
        return;

        m_binaryOp = { new Expression{ std::move(exp_ref1) }, op.data(), new Expression{ std::move(exp_ref2) } };
    }

    void NcAST::mkLocalExpNode(Expression& exp_ref)
    {
        if (m_isDeadZone)
        return;
        
        if (!m_type.isEmpty())
        {
            exp_ref = Expression{ std::move(m_type) };
        }
        else if (!m_literal.isEmpty())
        {
            exp_ref = Expression{ std::move(m_literal) };
        }
        else if (!m_var.isEmpty())
        {
            exp_ref = Expression{ std::move(m_var) };
        }
        else if (!m_binaryOp.isEmpty())
        {
            exp_ref = Expression{ std::move(m_binaryOp) };
        }
        else if (!m_uPreOp.isEmpty())
        {
            exp_ref = Expression{ std::move(m_uPreOp) };
        }
        else if (!m_uPostOp.isEmpty())
        {
            exp_ref = Expression{ std::move(m_uPostOp) };
        }
        else
        {
            throw std::runtime_error{ "in function Nc::NcAST::mkLocalExpNode, no condition was met!" };   
        }
    }

    bool NcAST::isRooTNodeEmpty()
    {
        return m_root.isEmpty(); 
    }

    void NcAST::setDeadZoneFlag()
    { m_isDeadZone = true; }

    NcAST::operator Root&()
    { return m_root; }

} // namespace Nc
