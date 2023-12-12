#ifndef NCNONTERMINAL_H
#define NCNONTERMINAL_H

#include <variant>

#include "ncTerminal.h"

namespace Nc
{
    struct Expression
    {
        std::variant<UPrefixOperation, UPostfixOperation, BinaryOperation, Literal, Variable, Type> exp;

        Expression(): exp{} {}

        Expression(std::variant<UPrefixOperation, UPostfixOperation, BinaryOperation, Literal, Variable, Type> _exp)
        :exp{ std::move(_exp) } {}

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, exp); }

        bool isLiteral() const
        {
            return std::visit([]<typename T>(T) { 
                if constexpr (std::is_same_v<T, Literal>) return true;
                return false; }
            , exp);
        }
    };

    struct VarDeclaration
    {
        VarDeclare var_declare{};

        bool isEmpty() const
        { return var_declare.isEmpty(); }
    };
    
    struct Statement
    {
        std::variant<Give, Expression> statement{};

        Statement(): statement{} {}

        Statement(std::variant<Give, Expression> _statement)
        :statement{ std::move(_statement) } {}

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, statement); }
    };

    struct BlockItem
    {
        std::variant<Statement, VarDeclaration> blockItem{};

        BlockItem() = default;

        BlockItem(std::variant<Statement, VarDeclaration> _blockItem): blockItem{ _blockItem }
        {}

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, blockItem); }
    };
    
    struct FuncDeclaration
    {
        Function func{};

        bool isEmpty() const
        { return func.isEmpty(); }
    };
    
    struct Root
    {
        NcFile ncF{};

        bool isEmpty() const
        { return ncF.isEmpty(); }
    };
    
    /*-----------------------------------MakeDynamicNode---------------------------------------------------*/

    template <typename NodeType>
    auto mk_dyNode(NodeType&& nT)
    {
        return DynamicNode<NodeType>{ new NodeType{nT} };
    }

    /*----------------------------------OutputStream Overloads-------------------------------------------*/


    std::ostream& operator<<(std::ostream& os, const Type& d);
    std::ostream& operator<<(std::ostream& os, const Literal& d);
    std::ostream& operator<<(std::ostream& os, const Variable& d);
    std::ostream& operator<<(std::ostream& os, const UPrefixOperation& d);
    std::ostream& operator<<(std::ostream& os, const UPostfixOperation& d);
    std::ostream& operator<<(std::ostream& os, const BinaryOperation& d);
    std::ostream& operator<<(std::ostream& os, const Expression& d);


    std::ostream& operator<<(std::ostream& os, const Give& d);
    std::ostream& operator<<(std::ostream& os, const Statement& d);
    std::ostream& operator<<(std::ostream& os, const VarDeclare& d);
    std::ostream& operator<<(std::ostream& os, const VarDeclaration& d);
    std::ostream& operator<<(std::ostream& os, const BlockItem& d);

    std::ostream& operator<<(std::ostream& os, const Function& d);
    std::ostream& operator<<(std::ostream& os, const FuncDeclaration& d);
    std::ostream& operator<<(std::ostream& os, const NcFile& d);
    std::ostream& operator<<(std::ostream& os, const Root& d);


    inline std::ostream& operator<<(std::ostream& os, const Type& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Type&): object d is empty" };

        return os << d.tn;
    }

    inline std::ostream& operator<<(std::ostream& os, const Literal& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Literal&): object d is empty" };

        return os << d.l;
    }

    inline std::ostream& operator<<(std::ostream& os, const Variable& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Variable&): object d is empty" };

        return os << d.ident;
    }

    inline std::ostream& operator<<(std::ostream& os, const UPrefixOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const UPrefixOperation&): object d is empty" };

        return os << "UnaryPrefixOp: " << d.op << ' ' <<  *d.right_exp;
    }

    inline std::ostream& operator<<(std::ostream& os, const UPostfixOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const UPostfixOperation&): object d is empty" };
        
        return os << "UnaryPostfixOp: " << *d.left_exp << ' ' << d.op;
    }

    inline std::ostream& operator<<(std::ostream& os, const BinaryOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const BinaryOperation&): object d is empty" };

        return os << "BinaryOp: " << *d.left_exp << ' ' << d.op << ' ' << *d.right_exp;
    }

    inline std::ostream& operator<<(std::ostream& os, const Expression& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Expression&): object d is empty" };

        return std::visit( [&](const auto& alt) -> std::ostream& { return os << "Exp[ " << alt << " ]"; }, d.exp);
    }

    inline std::ostream& operator<<(std::ostream& os, const Give& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Give&): object d is empty" };

        return os << "Give: " << *d.exp;
    }

    inline std::ostream& operator<<(std::ostream& os, const Statement& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Statement&): object d is empty" };

        return std::visit( [&](const auto& alt) -> std::ostream& { return os << "Statement[ " << alt << " ]"; }, d.statement);
    }
    
    inline std::ostream& operator<<(std::ostream& os, const VarDeclare& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const VarDeclare&): object d is empty" };

        os << "VarDelcare: type=" << d.type << " and idents=(";

        for (auto &&i : d.ident_init_list)
        {
            os << ' ' << i.first << "=" << *i.second;    
        }

        return os << " )";
    }

    inline std::ostream& operator<<(std::ostream& os, const VarDeclaration& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const VarDeclaration&): object d is empty" };

        return os << "VarDelcaration[ " << d.var_declare << " ]";
    }

    inline std::ostream& operator<<(std::ostream& os, const BlockItem& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const BlockItem&): object d is empty" };

        return std::visit( [&](const auto& alt) -> std::ostream& { return os << "BlockItem\n[ " << alt << " ]\n"; }, d.blockItem);
    }

    inline std::ostream& operator<<(std::ostream& os, const Function& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Function&): object d is empty" };

        os << "Function(" << "type=" << d.type << " and ident=" << d.ident << "):\n";

        for (auto &&i : d.blockItemList)
        {
            os << *i << '\n';
        }
        
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const FuncDeclaration& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const FuncDeclaration&): object d is empty" };

        return os << "Function Declaration\n[\n\n" << d.func << "]";
    }

    inline std::ostream& operator<<(std::ostream& os, const NcFile& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const NcFile&): object d is empty" };

        os << "NcFile:\n\n";
    
        for (auto &&i : d.funcList)
        {
            os <<  *i << '\n';
        }

        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Root& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Root&): object d is empty" };

        return os << "Root -> " << d.ncF;
    }

} // namespace nc

#endif