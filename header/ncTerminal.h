#ifndef NCTERMINAL_H
#define NCTERMINAL_H

#include <string>
#include <memory>
#include <list>
#include <utility>
#include <stdexcept>
#include <type_traits>


namespace Nc
{
    struct Expression;
    struct BlockItem;
    struct FuncDeclaration;
    
    template <typename NodeType>
    class DynamicNode: public std::unique_ptr<NodeType>
    {    
    public:
        using Unique_ptr = std::unique_ptr<NodeType>;

        DynamicNode(): Unique_ptr{}
        {}

        DynamicNode(NodeType*  ptr): Unique_ptr{ptr}
        {}

        DynamicNode(const DynamicNode& src): Unique_ptr{ new NodeType{*src} }
        {}

        DynamicNode(DynamicNode&& src): Unique_ptr{ std::move(src) }
        {}

        ~DynamicNode() = default;

        DynamicNode& operator=(const DynamicNode& src)
        {
            if (this == &src)
            {
                throw std::runtime_error{"in DynamicNode& operator=(const DynamicNode& src): self assignment is not allowed"};
            }
            
            Unique_ptr::reset(new NodeType{*src});
            return *this;
        }

        DynamicNode& operator=(DynamicNode&& src)
        {
            if (this == &src)
            {
                throw std::runtime_error{"in DynamicNode& operator=(DynamicNode&& src): self assignment is not allowed"};
            }

            Unique_ptr::reset(src.release());
            return *this;
        }

        bool isNull() const
        { return *this == nullptr; }
    };


    template <typename Type>
    using List = std::list<Type>;

    struct Type
    {
        using TypeName = std::string;

        TypeName tn{};

        bool isEmpty() const
        { return tn.empty(); }
    };
    
    struct Literal
    {
        using literal = std::string;
        using literalType = std::string;

        literal l{};
        literalType ltype{};

        bool isEmpty() const
        { return l.empty() and ltype.empty(); }
    };

    struct UPrefixOperation
    {
        std::string op{};
        DynamicNode<Expression> right_exp{};

        bool isEmpty() const
        { return op.empty() and right_exp.isNull(); }
    };

    struct UPostfixOperation
    {
        DynamicNode<Expression> left_exp{};
        std::string op{};

        bool isEmpty() const
        { return op.empty() and left_exp.isNull(); }
    };

    struct BinaryOperation
    {
        DynamicNode<Expression> left_exp{};
        std::string op{};
        DynamicNode<Expression> right_exp{};

        bool isEmpty() const
        { return left_exp.isNull() and op.empty() and right_exp.isNull(); }
    };

    struct Variable
    {
        using VarIdentifier = std::string;

        VarIdentifier ident{};

        bool isEmpty() const
        { return ident.empty(); }
    };

    struct Give
    {
        DynamicNode<Expression> exp{};

        bool isEmpty() const
        { return exp.isNull(); }
    };

    struct VarDeclare
    {
        using VarIdentifier = std::string;
        using VarType = std::string;
        using Ident_init = std::pair<VarIdentifier, DynamicNode<Expression>>;

        VarType type{};
        List<Ident_init> ident_init_list{};

        bool isEmpty() const
        { return type.empty() and ident_init_list.empty(); }
    };

    struct Function
    {
        using FcnIdentifier = std::string;
        using GiveType = std::string;

        GiveType type{};
        FcnIdentifier ident{};
        List<DynamicNode<BlockItem>> blockItemList{};

        bool isEmpty() const
        { return type.empty(); ident.empty() and blockItemList.empty(); }
    };
    
    struct NcFile
    {
        List<DynamicNode<FuncDeclaration>> funcList{};

        bool isEmpty() const
        { return funcList.empty(); }
    };

} // namespace nc

#endif