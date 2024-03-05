#ifndef NCTERMINAL_H
#define NCTERMINAL_H

#include <variant>
#include <optional>
#include <string>
#include <memory>
#include <list>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <cstdint>

#include "stream/output.h"
#include "myfunctions1.h"

namespace Nc
{
    class Expression;
    class Statement;
    class BlockItem;
    class FuncDeclaration;
    class VarDeclaration;
    
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
        void clear()
        { Unique_ptr::reset(nullptr); }
    };

    class Void
    {
    public:
        enum class InUse: bool {yes, no};

    private:
        InUse m_inUse{ InUse::no };

    public:
        Void() = default;
        Void(InUse inUse): m_inUse{inUse} {}
        Void(const Void&) = default;
        Void(Void&& src): m_inUse{ src.m_inUse }
        { src.m_inUse = InUse::no; }

        Void& operator=(const Void&) = default;
        Void& operator=(Void&&) = default;

        Void& reset()
        { return m_inUse = InUse::no, *this; }
        Void& set()
        { return m_inUse = InUse::yes, *this; }

        bool isEmpty() const
        { return m_inUse == InUse::no; }
    };

    template <typename T>
    using List = std::list<T>;
    using Operator = Myfcn::U8string;

    class Type;
    class Identifier;

    using ParameterList = List< std::pair<Type, std::optional<Identifier>> >;
    using ExpressionList = List<Expression>;

    using ColumnSpan = std::pair<std::uint32_t, std::uint32_t>;
    using LineNumber_and_LineCount = std::pair<std::uint32_t, std::uint32_t>;
    using LocationFinder = std::pair<LineNumber_and_LineCount, ColumnSpan>;

    class Type
    {
    public:
        using NormalType = Myfcn::U8string;

        class FunctionType
        {
        private:
            List<DynamicNode<Type>> m_parameterTypeList{};
            DynamicNode<Type> m_giveType{};

        public:
            FunctionType() = default;

            FunctionType(List<DynamicNode<Type>>& parameterTypeList, DynamicNode<Type>& giveType):
            m_parameterTypeList{ std::move(parameterTypeList) }, m_giveType{ std::move(giveType) }{}

            FunctionType(List<DynamicNode<Type>>&& parameterTypeList, DynamicNode<Type>&& giveType):
            m_parameterTypeList{ std::move(parameterTypeList) }, m_giveType{ std::move(giveType) }{}
            

            void setParameterTypeList(List<DynamicNode<Type>>& parameterTypeList)
            { m_parameterTypeList = std::move(parameterTypeList); }
            void setParameterTypeList(List<DynamicNode<Type>>&& parameterTypeList)
            { m_parameterTypeList = std::move(parameterTypeList); }

            void setGiveType(DynamicNode<Type>& giveType)
            { m_giveType = std::move(giveType); }
            void setGiveType(DynamicNode<Type>&& giveType)
            { m_giveType = std::move(giveType); }

            auto& getParameterTypeList()
            { return m_parameterTypeList; }
            const auto& getParameterTypeList() const
            { return m_parameterTypeList; }

            auto& getGiveType()
            { return m_giveType; }
            const auto& getGiveType() const
            { return m_giveType; }

            void pushBack_parameterType(DynamicNode<Type>& parameterType)
            { m_parameterTypeList.push_back(std::move(parameterType)); }
            void pushBack_parameterType(DynamicNode<Type>&& parameterType)
            { m_parameterTypeList.push_back(std::move(parameterType)); }

            void clear()
            { m_parameterTypeList.clear(), m_giveType.clear(); }

            bool isEmpty() const
            { return m_giveType.isNull(); }
        };

    private:
        std::variant<NormalType, FunctionType> m_type{};
        Myfcn::U8string m_typeAttributesExp{};
        bool m_isConst{};
        bool m_isPointer{};
        bool m_isLvalueRef{};
        bool m_isRvalueRef{};
        LocationFinder m_locFinder{};

    public:
        Type() = default;

        Type(NormalType& type, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder):
        m_type{ std::move(type) }, m_typeAttributesExp{ std::move(typeAttributesExp) }, m_locFinder{ locFinder }{}

        Type(NormalType&& type, Myfcn::U8string&& typeAttributesExp, LocationFinder&& locFinder):
        m_type{ std::move(type) }, m_typeAttributesExp{ std::move(typeAttributesExp) }, m_locFinder{ locFinder }{}

        Type(FunctionType& type, Myfcn::U8string& typeAttributesExp, LocationFinder& locFinder):
        m_type{ std::move(type) }, m_typeAttributesExp{ std::move(typeAttributesExp) }, m_locFinder{ locFinder }{}

        Type(FunctionType&& type, Myfcn::U8string&& typeAttributesExp, LocationFinder&& locFinder):
        m_type{ std::move(type) }, m_typeAttributesExp{ std::move(typeAttributesExp) }, m_locFinder{ locFinder }{}
        

        void setType(NormalType& type)
        { m_type = NormalType{std::move(type)}; }
        void setType(NormalType&& type)
        { m_type = NormalType{std::move(type)}; }
        void setType(FunctionType& type)
        { m_type = FunctionType{std::move(type)}; }
        void setType(FunctionType&& type)
        { m_type = FunctionType{std::move(type)}; }
        
        void setTypeAttributesExp(Myfcn::U8string& typeAttributesExp)
        { m_typeAttributesExp = std::move(typeAttributesExp); }
        void setTypeAttributesExp(Myfcn::U8string&& typeAttributesExp)
        { m_typeAttributesExp = std::move(typeAttributesExp); }

        void setConstBoolean(bool flag)
        { m_isConst = flag; }
        void setPointerBoolean(bool flag)
        { m_isPointer = flag; }
        void setLvalueRefBoolean(bool flag)
        { m_isLvalueRef = flag; }
        void setRvalueRefBoolean(bool flag)
        { m_isRvalueRef = flag; }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getType()
        { return m_type; }
        const auto& getType() const
        { return m_type; }

        auto& getTypeAttributesExp()
        { return m_typeAttributesExp; }
        const auto& getTypeAttributesExp() const
        { return m_typeAttributesExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { std::visit([](auto& item){ item.clear(); }, m_type), m_typeAttributesExp.clear(); }

        bool isEmpty() const
        {
            auto lambda = [&]<typename T>(T&)
            {
                if constexpr (std::is_same_v<std::decay_t<T>, NormalType>)
                return std::get<NormalType>(m_type).empty();
                else
                return std::get<FunctionType>(m_type).isEmpty();
            };

            return std::visit(lambda, m_type);
        }
    };
    
    
    class AnyTypeListExp
    {
    public:
        using DynamicExpressionList = List<DynamicNode<Expression>>;

    private:
        std::variant<Void, DynamicExpressionList> m_listContents{};
        LocationFinder m_locFinder{};

    public:
        AnyTypeListExp() = default;
        AnyTypeListExp(DynamicExpressionList& listContents, LocationFinder& locFinder): m_listContents{ std::move(listContents) }, m_locFinder{ std::move(locFinder) } {}
        AnyTypeListExp(DynamicExpressionList&& listContents, LocationFinder&& locFinder): m_listContents{ std::move(listContents) }, m_locFinder{ std::move(locFinder) } {}
        AnyTypeListExp(Void& listContents, LocationFinder&& locFinder): m_listContents{ std::move(listContents) }, m_locFinder{ std::move(locFinder) } {}
        AnyTypeListExp(Void&& listContents, LocationFinder&& locFinder): m_listContents{ std::move(listContents) }, m_locFinder{ std::move(locFinder) } {}

        void setListContents(DynamicExpressionList& listContents)
        { m_listContents = std::move(listContents); }
        void setListContents(DynamicExpressionList&& listContents)
        { m_listContents = std::move(listContents); }

        void setListContents(Void& listContents)
        { m_listContents = std::move(listContents); }
        void setListContents(Void&& listContents)
        { m_listContents = std::move(listContents); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getListContents()
        { return m_listContents; }
        auto& getListContents() const
        { return m_listContents; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        {
            std::visit([](auto& alt){
                if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
                alt.reset();
                else
                alt.clear();

            }, m_listContents);
        }

        bool isEmpty() const
        {
            return std::visit([](const auto& alt){
                if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
                return alt.isEmpty();
                else
                return alt.empty();

            }, m_listContents);
        }
    };

    class TempVarDecl
    {
    private:
        Type m_variableType{};
        AnyTypeListExp m_anyTypeListExp{};
        LocationFinder m_locFinder{};

    public:
        TempVarDecl() = default;

        TempVarDecl(Type& variableType, AnyTypeListExp& anyTypeListExp, LocationFinder& locFinder):
        m_variableType{ std::move(variableType) }, m_anyTypeListExp{ std::move(anyTypeListExp) }, m_locFinder{ std::move(locFinder) } {}

        TempVarDecl(Type&& variableType, AnyTypeListExp&& anyTypeListExp, LocationFinder&& locFinder):
        m_variableType{ std::move(variableType) }, m_anyTypeListExp{ std::move(anyTypeListExp) }, m_locFinder{ std::move(locFinder) } {}

        void setVariableType(Type& variableType)
        { m_variableType = std::move(variableType); }
        void setVariableType(Type&& variableType)
        { m_variableType = std::move(variableType); }

        void setIntializerList(AnyTypeListExp& anyTypeListExp)
        { m_anyTypeListExp = std::move(anyTypeListExp); }
        void setIntializerList(AnyTypeListExp&& anyTypeListExp)
        { m_anyTypeListExp = std::move(anyTypeListExp); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getVariableType()
        { return m_variableType; }
        auto& getVariableType() const
        { return m_variableType; }

        auto& getAnyTypeListExp()
        { return m_anyTypeListExp; }
        auto& getAnyTypeListExp() const
        { return m_anyTypeListExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_variableType.clear(), m_anyTypeListExp.clear(); }

        bool isEmpty() const
        { return m_variableType.isEmpty() or m_anyTypeListExp.isEmpty(); }
    };

    class Literal
    {
    public:
        using LiteralConstant = Myfcn::U8string;

    private:
        LiteralConstant m_literalConst{};
        Myfcn::U8string m_literalType{};
        LocationFinder m_locFinder{};

    public:
        Literal() = default;

        Literal(LiteralConstant& literalConst, LocationFinder& locFinder):
        m_literalConst{ std::move(literalConst) }, m_locFinder{ locFinder }{}

        Literal(LiteralConstant&& literalConst, LocationFinder&& locFinder):
        m_literalConst{ std::move(literalConst) }, m_locFinder{ locFinder }{}

        void setLiteralConst(LiteralConstant& literalConst)
        { m_literalConst = std::move(literalConst); }
        void setLiteralConst(LiteralConstant&& literalConst)
        { m_literalConst = std::move(literalConst); }

        void setLiteralType(Myfcn::U8string& literalType)
        { m_literalType = std::move(literalType); }
        void setLiteralType(Myfcn::U8string&& literalType)
        { m_literalType = std::move(literalType); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getLiteralConstant()
        { return m_literalConst; }
        const auto& getLiteralConstant() const
        { return m_literalConst; }

        auto& getLiteralType()
        { return m_literalType; }
        const auto& getLiteralType() const
        { return m_literalType; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_literalConst.clear(), m_literalType.clear(); }

        bool isEmpty() const
        { return m_literalConst.empty(); }
    };

    class UPrefixOperation
    {
    private:
        Operator m_op{};
        DynamicNode<Expression> m_rightExp{};
        LocationFinder m_locFinder{};

    public:
        UPrefixOperation() = default;
        UPrefixOperation(Operator& op, DynamicNode<Expression>& rightExp, LocationFinder& locFinder):
        m_op{ std::move(op) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }
        {}
        UPrefixOperation(Operator&& op, DynamicNode<Expression>&& rightExp, LocationFinder&& locFinder):
        m_op{ std::move(op) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }
        {}

        void setOperator(Operator& op)
        { m_op = std::move(op); }
        void setOperator(Operator&& op)
        { m_op = std::move(op); }

        void setRightExp(DynamicNode<Expression>& rightExp)
        { m_rightExp = std::move(rightExp); }
        void setRightExp(DynamicNode<Expression>&& rightExp)
        { m_rightExp = std::move(rightExp); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getOperator()
        { return m_op; }
        const auto& getOperator() const
        { return m_op; }

        auto& getRightExp()
        { return m_rightExp; }
        const auto& getRightExp() const
        { return m_rightExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_op.clear(), m_rightExp.clear(); }

        bool isEmpty() const
        { return m_op.empty() or m_rightExp.isNull(); }
    };
    
    class UPostfixOperation
    {
    public:
        using DynamicExpressionList = List<DynamicNode<Expression>>;

    private:

        DynamicNode<Expression> m_leftExp{};
        std::variant<Operator, DynamicExpressionList> m_op{};
        LocationFinder m_locFinder{};

    public:
        UPostfixOperation() = default;

        UPostfixOperation(DynamicNode<Expression>& leftExp, Operator& op, LocationFinder& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_locFinder{ locFinder }{}

        UPostfixOperation(DynamicNode<Expression>&& leftExp, Operator&& op, LocationFinder&& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_locFinder{ locFinder }{}

        UPostfixOperation(DynamicNode<Expression>& leftExp, DynamicExpressionList& op, LocationFinder& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_locFinder{ locFinder }{}

        UPostfixOperation(DynamicNode<Expression>&& leftExp, DynamicExpressionList&& op, LocationFinder&& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_locFinder{ locFinder }{}

        void setOperator(Operator& op)
        { m_op = std::move(op); }
        void setOperator(Operator&& op)
        { m_op = std::move(op); }

        void setOperator(DynamicExpressionList& op)
        { m_op = std::move(op); }
        void setOperator(DynamicExpressionList&& op)
        { m_op = std::move(op); }

        void setLeftExp(DynamicNode<Expression>& rightExp)
        { m_leftExp = std::move(rightExp); }
        void setLeftExp(DynamicNode<Expression>&& rightExp)
        { m_leftExp = std::move(rightExp); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getOperator()
        { return m_op; }
        const auto& getOperator() const
        { return m_op; }

        auto& getLeftExp()
        { return m_leftExp; }
        const auto& getLeftExp() const
        { return m_leftExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_leftExp.clear(), std::visit([](auto& alt){ alt.clear(); }, m_op); }

        bool isEmpty() const
        {
            auto lambda = [&]<typename T>(T&)
            {
                if constexpr (std::is_same_v<std::decay_t<T>, Operator>)
                return std::get<Operator>(m_op).empty();
                
                return false;
            };

            return m_leftExp.isNull() or std::visit(lambda, m_op);
        }
    };

    class BinaryOperation
    {
    private:
        DynamicNode<Expression> m_leftExp{};
        Operator m_op{};
        DynamicNode<Expression> m_rightExp{};
        LocationFinder m_locFinder{};

    public:
        BinaryOperation() = default;

        BinaryOperation(DynamicNode<Expression>& leftExp, Operator& op, DynamicNode<Expression>& rightExp, LocationFinder& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }{}

        BinaryOperation(DynamicNode<Expression>&& leftExp, Operator&& op, DynamicNode<Expression>&& rightExp, LocationFinder&& locFinder):
        m_leftExp{ std::move(leftExp) }, m_op{ std::move(op) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }{}
        
        void setLeftExp(DynamicNode<Expression>& leftExp)
        { m_leftExp = std::move(leftExp); }
        void setLeftExp(DynamicNode<Expression>&& leftExp)
        { m_leftExp = std::move(leftExp); }

        void setOperator(Operator& op)
        { m_op = std::move(op); }
        void setOperator(Operator&& op)
        { m_op = std::move(op); }

        void setRightExp(DynamicNode<Expression>& rightExp)
        { m_rightExp = std::move(rightExp); }
        void setRightExp(DynamicNode<Expression>&& rightExp)
        { m_rightExp = std::move(rightExp); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }
        
        auto& getLeftExp()
        { return m_leftExp; }
        const auto& getLeftExp() const
        { return m_leftExp; }

        auto& getOperator()
        { return m_op; }
        const auto& getOperator() const
        { return m_op; }

        auto& getRightExp()
        { return m_rightExp; }
        const auto& getRightExp() const
        { return m_rightExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_leftExp.clear(), m_op.clear(), m_rightExp.clear(); }

        bool isEmpty() const
        { return m_leftExp.isNull() or m_op.empty() or m_rightExp.isNull(); }
    };

    class TenaryOperation
    {
    private:
        DynamicNode<Expression> m_conditionExp{};
        Operator m_op{};
        DynamicNode<Expression> m_leftExp{};
        DynamicNode<Expression> m_rightExp{};
        LocationFinder m_locFinder{};

    public:
        TenaryOperation() = default;

        TenaryOperation(DynamicNode<Expression>& conditionExp, Operator& op, DynamicNode<Expression>& leftExp, DynamicNode<Expression>& rightExp, LocationFinder& locFinder):
        m_conditionExp{ std::move(conditionExp) }, m_op{ std::move(op) }, m_leftExp{ std::move(leftExp) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }{}

        TenaryOperation(DynamicNode<Expression>&& conditionExp, Operator&& op, DynamicNode<Expression>&& leftExp, DynamicNode<Expression>&& rightExp, LocationFinder&& locFinder):
        m_conditionExp{ std::move(conditionExp) }, m_op{ std::move(op) }, m_leftExp{ std::move(leftExp) }, m_rightExp{ std::move(rightExp) }, m_locFinder{ locFinder }{}
        
        void setConditionExp(DynamicNode<Expression>& conditionExp)
        { m_conditionExp = std::move(conditionExp); }
        void setConditionExp(DynamicNode<Expression>&& conditionExp)
        { m_conditionExp = std::move(conditionExp); }

        void setOperator(Operator& op)
        { m_op = std::move(op); }
        void setOperator(Operator&& op)
        { m_op = std::move(op); }

        void setLeftExp(DynamicNode<Expression>& leftExp)
        { m_leftExp = std::move(leftExp); }
        void setLeftExp(DynamicNode<Expression>&& leftExp)
        { m_leftExp = std::move(leftExp); }

        void setRightExp(DynamicNode<Expression>& rightExp)
        { m_rightExp = std::move(rightExp); }
        void setRightExp(DynamicNode<Expression>&& rightExp)
        { m_rightExp = std::move(rightExp); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getConditionExp()
        { return m_conditionExp; }
        const auto& getConditionExp() const
        { return m_conditionExp; }

        auto& getLeftExp()
        { return m_leftExp; }
        const auto& getLeftExp() const
        { return m_leftExp; }

        auto& getOperator()
        { return m_op; }
        const auto& getOperator() const
        { return m_op; }

        auto& getRightExp()
        { return m_rightExp; }
        const auto& getRightExp() const
        { return m_rightExp; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_conditionExp.clear(), m_op.clear(), m_leftExp.clear(), m_rightExp.clear(); }

        bool isEmpty() const
        { return m_conditionExp.isNull() or m_op.empty() or m_leftExp.isNull() or m_rightExp.isNull(); }
    };

    class Identifier
    {
    public:
        using IdentifierName = Myfcn::U8string;

    private:
        IdentifierName m_identName{};
        Type m_identType{};
        LocationFinder m_locFinder{};

    public:
        Identifier() = default;

        Identifier(IdentifierName& identName, LocationFinder& locFinder):
        m_identName{ std::move(identName) }, m_locFinder{ locFinder }{}

        Identifier(IdentifierName&& identName, LocationFinder&& locFinder):
        m_identName{ std::move(identName) }, m_locFinder{ locFinder }{}
        
        void setIdentName(IdentifierName& identName)
        { m_identName = std::move(identName); }
        void setIdentName(IdentifierName&& identName)
        { m_identName = std::move(identName); }

        void setIdentType(Type& identType)
        { m_identType = std::move(identType); }
        void setIdentType(Type&& identType)
        { m_identType = std::move(identType); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getIdentName()
        { return m_identName; }
        const auto& getIdentName() const
        { return m_identName; }

        auto& getIdentType()
        { return m_identType; }
        const auto& getIdentType() const
        { return m_identType; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_identName.clear(), m_identType.clear(); }

        bool isEmpty() const
        { return m_identName.empty(); }
    };

    class R_And_L_Exp
    {
    public:
        using DynamicExpression = DynamicNode<Expression>;
        using DynamicExpressionList = List<DynamicNode<Expression>>;

    private:
        Operator m_relationalOp{};
        Operator m_logicalOp{};
        DynamicExpression m_exp{};
        DynamicExpressionList m_expList{};
        LocationFinder m_locFinder{};

    public:
        R_And_L_Exp() = default;

        R_And_L_Exp(Operator& relationalOp, Operator& logicalOp, DynamicExpression& exp, DynamicExpressionList& expList, LocationFinder& locFinder):
        m_relationalOp{ std::move(relationalOp) }, m_logicalOp{ std::move(logicalOp) }, m_exp{ std::move(exp) }, m_expList{ std::move(expList) }, m_locFinder{locFinder} {}

        R_And_L_Exp(Operator&& relationalOp, Operator&& logicalOp, DynamicExpression&& exp, DynamicExpressionList&& expList, LocationFinder&& locFinder):
        m_relationalOp{ std::move(relationalOp) }, m_logicalOp{ std::move(logicalOp) }, m_exp{ std::move(exp) }, m_expList{ std::move(expList) }, m_locFinder{locFinder} {}
    
        void setRelationalOp(Operator& relationalOp)
        { m_relationalOp = std::move(relationalOp); }
        void setRelationalOp(Operator&& relationalOp)
        { m_relationalOp = std::move(relationalOp); }

        void setLogicalOp(Operator& logicalOp)
        { m_logicalOp = std::move(logicalOp); }
        void setLogicalOp(Operator&& logicalOp)
        { m_logicalOp = std::move(logicalOp); }

        void setExpression(DynamicExpression& exp)
        { m_exp = std::move(exp); }
        void setExpression(DynamicExpression&& exp)
        { m_exp = std::move(exp); }

        void setExpressionList(DynamicExpressionList& expList)
        { m_expList = std::move(expList); }
        void setExpressionList(DynamicExpressionList&& expList)
        { m_expList = std::move(expList); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getRelationalOp()
        { return m_relationalOp; }
        const auto& getRelationalOp() const
        { return m_relationalOp; }

        auto& getLogicalOp()
        { return m_logicalOp; }
        const auto& getLogicalOp() const
        { return m_logicalOp; }

        auto& getExp()
        { return m_exp; }
        const auto& getExp() const
        { return m_exp; }

        auto& getExpList()
        { return m_expList; }
        const auto& getExpList() const
        { return m_expList; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void pushBack_exp(DynamicExpression& exp)
        { m_expList.push_back(std::move(exp)); }
        void pushBack_exp(DynamicExpression&& exp)
        { m_expList.push_back(std::move(exp)); }

        void clear()
        { m_relationalOp.clear(), m_logicalOp.clear(), m_exp.clear(), m_expList.clear(); }

        bool isEmpty() const
        { return m_relationalOp.empty() or m_logicalOp.empty() or m_exp.isNull(); }
    };

    class Give
    {
    private:
        std::variant<DynamicNode<Expression>, Void> m_exp_void{ Void{} };
        LocationFinder m_locFinder{};
        
    public:
        Give() = default;

        Give(Void& exp_void, LocationFinder& locFinder):
        m_exp_void{ std::move(exp_void) }, m_locFinder{ locFinder }{}

        Give(Void&& exp_void, LocationFinder&& locFinder):
        m_exp_void{ std::move(exp_void) }, m_locFinder{ locFinder }{}

        Give(DynamicNode<Expression>& exp_void, LocationFinder& locFinder):
        m_exp_void{ std::move(exp_void) }, m_locFinder{ locFinder }{}

        Give(DynamicNode<Expression>&& exp_void, LocationFinder&& locFinder):
        m_exp_void{ std::move(exp_void) }, m_locFinder{ locFinder }{}


        void set_exp_void(Void& exp_void)
        { m_exp_void = std::move(exp_void); }
        void set_exp_void(Void&& exp_void)
        { m_exp_void = std::move(exp_void); }
        void set_exp_void(DynamicNode<Expression>& exp_void)
        { m_exp_void = std::move(exp_void); }
        void set_exp_void(DynamicNode<Expression>&& exp_void)
        { m_exp_void = std::move(exp_void); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& get_exp_void()
        { return m_exp_void; }
        const auto& get_exp_void() const
        { return m_exp_void; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        {
            auto lambda = [&]<typename T>(T&)
            {
                if constexpr (std::is_same_v<std::decay_t<T>, Void>)
                std::move(std::get<Void>(m_exp_void)).reset();
                else
                std::get<DynamicNode<Expression>>(m_exp_void).clear();
            };

            return std::visit(lambda, m_exp_void);
        }

        bool isEmpty() const
        {
            auto lambda = [&]<typename T>(T&)
            {
                if constexpr (std::is_same_v<std::decay_t<T>, DynamicNode<Expression>>)
                return std::get<DynamicNode<Expression>>(m_exp_void).isNull();
                else
                return std::get<Void>(m_exp_void).isEmpty();
            };

            return std::visit(lambda, m_exp_void);
        }
    };

    class Break
    {
    public:
        enum class InUse: bool { yes, no };

    private:
        InUse m_inUse{ InUse::no };
        LocationFinder m_locFinder{};

    public:
        Break() = default;
        Break(InUse inUse, LocationFinder locFinder): m_inUse{inUse}, m_locFinder{locFinder}{}

        Break(const Break&) = default;
        Break(Break&& src): m_inUse{src.m_inUse}, m_locFinder{src.m_locFinder}
        { src.m_inUse = InUse::no; }

        Break& operator=(const Break&) = default;
        Break& operator=(Break&&) = default;

        Break& reset()
        { return m_inUse = InUse::no, *this; }
        Break& set()
        { return m_inUse = InUse::yes, *this; }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        bool isEmpty() const
        { return m_inUse == InUse::no; }
    };

    class Continue
    {
    public:
        enum class InUse: bool { yes, no };

    private:
        InUse m_inUse{ InUse::no };
        LocationFinder m_locFinder{};

    public:
        Continue() = default;

        Continue(InUse inUse, LocationFinder locFinder): m_inUse{inUse}, m_locFinder{locFinder}{}

        Continue(const Continue&) = default;
        Continue(Continue&& src): m_inUse{src.m_inUse}, m_locFinder{src.m_locFinder}
        { src.m_inUse = InUse::no; }

        Continue& operator=(const Continue&) = default;
        Continue& operator=(Continue&&) = default;

        Continue& reset()
        { return m_inUse = InUse::no, *this; }
        Continue& set()
        { return m_inUse = InUse::yes, *this; }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        bool isEmpty() const
        { return m_inUse == InUse::no; }
    };

    class JumpStatement
    {    
    private:
        std::variant<Give, Break, Continue> m_jStatement{};

    public:
        JumpStatement() = default;

        JumpStatement(Give& jStatement): m_jStatement{ std::move(jStatement) } {}
        JumpStatement(Give&& jStatement): m_jStatement{ std::move(jStatement) } {}
        JumpStatement(Break& jStatement): m_jStatement{ std::move(jStatement) } {}
        JumpStatement(Break&& jStatement): m_jStatement{ std::move(jStatement) } {}
        JumpStatement(Continue& jStatement): m_jStatement{ std::move(jStatement) } {}
        JumpStatement(Continue&& jStatement): m_jStatement{ std::move(jStatement) } {}

        void setJStatement(Give& jStatement)
        { m_jStatement = std::move(jStatement); }
        void setJStatement(Give&& jStatement)
        { m_jStatement = std::move(jStatement); }
        void setJStatement(Break& jStatement)
        { m_jStatement = std::move(jStatement); }
        void setJStatement(Break&& jStatement)
        { m_jStatement = std::move(jStatement); }
        void setJStatement(Continue& jStatement)
        { m_jStatement = std::move(jStatement); }
        void setJStatement(Continue&& jStatement)
        { m_jStatement = std::move(jStatement); }

        auto& getJStatement()
        { return m_jStatement; }
        const auto& getJStatement() const
        { return m_jStatement; }

        void clear()
        { std::visit([](auto& item){ std::remove_reference_t<decltype(item)>{ std::move(item) }; }, m_jStatement); }

        bool isEmpty() const
        { return std::visit([](const auto& item){ return item.isEmpty(); }, m_jStatement); }
    };

    class IfCondition
    {
    private:
        DynamicNode<Expression> m_conditionExp{};
        DynamicNode<Statement> m_statementIfTrue{};
        std::optional<DynamicNode<Statement>> m_statementIfFalse{};

    public:
        IfCondition() = default;
        
        IfCondition(DynamicNode<Expression>& conditionExp, DynamicNode<Statement>& statementIfTrue, std::optional<DynamicNode<Statement>>& statementIfFalse):
        m_conditionExp{ std::move(conditionExp) }, m_statementIfTrue{ std::move(statementIfTrue) },  m_statementIfFalse{ std::move(statementIfFalse) }{}

        IfCondition(DynamicNode<Expression>&& conditionExp, DynamicNode<Statement>&& statementIfTrue, std::optional<DynamicNode<Statement>>&& statementIfFalse):
        m_conditionExp{ std::move(conditionExp) }, m_statementIfTrue{ std::move(statementIfTrue) },  m_statementIfFalse{ std::move(statementIfFalse) }{}

        void setConditionExp(DynamicNode<Expression>& conditionExp)
        { m_conditionExp = std::move(conditionExp); }
        void setConditionExp(DynamicNode<Expression>&& conditionExp)
        { m_conditionExp = std::move(conditionExp); }

        void setStatementIfTrue(DynamicNode<Statement>& statementIfTrue)
        { m_statementIfTrue = std::move(statementIfTrue); }
        void setStatementIfTrue(DynamicNode<Statement>&& statementIfTrue)
        { m_statementIfTrue = std::move(statementIfTrue); }

        void setStatementIfFalse(std::optional<DynamicNode<Statement>>& statementIfFalse)
        { m_statementIfFalse = std::move(statementIfFalse); }
        void setStatementIfFalse(std::optional<DynamicNode<Statement>>&& statementIfFalse)
        { m_statementIfFalse = std::move(statementIfFalse); }

        auto& getConditionExp()
        { return m_conditionExp; }
        const auto& getConditionExp() const
        { return m_conditionExp; }

        auto& getStatementIfTrue()
        { return m_statementIfTrue; }
        const auto& getStatementIfTrue() const
        { return m_statementIfTrue; }

        auto& getStatementIfFalse()
        { return m_statementIfFalse; }
        const auto& getStatementIfFalse() const
        { return m_statementIfFalse; }

        void clear()
        {
            m_conditionExp.clear(), m_statementIfTrue.clear();
            if (m_statementIfFalse.has_value())
            m_statementIfFalse.operator*().clear();
        }

        bool isEmpty() const
        { return m_conditionExp.isNull() or m_statementIfTrue.isNull(); }
    };
    
    class ForLoop 
    {
    private:
        DynamicNode<VarDeclaration> m_localVarDecl{};
        DynamicNode<Expression> m_exp1{};
        DynamicNode<Expression> m_exp2{};
        DynamicNode<Statement> m_statement{};

    public:
        ForLoop() = default;

        ForLoop(DynamicNode<VarDeclaration>& localVarDecl, DynamicNode<Expression>& exp1, DynamicNode<Expression>& exp2, DynamicNode<Statement>& statement):
        m_localVarDecl{ std::move(localVarDecl) }, m_exp1{ std::move(exp1) }, m_exp2{ std::move(exp2) }, m_statement{ statement }{}

        ForLoop(DynamicNode<VarDeclaration>&& localVarDecl, DynamicNode<Expression>&& exp1, DynamicNode<Expression>&& exp2, DynamicNode<Statement>& statement):
        m_localVarDecl{ std::move(localVarDecl) }, m_exp1{ std::move(exp1) }, m_exp2{ std::move(exp2) }, m_statement{ statement }{}

        void setLocalVarDecl(DynamicNode<VarDeclaration>& localVarDecl)
        { m_localVarDecl = std::move(localVarDecl); }
        void setLocalVarDecl(DynamicNode<VarDeclaration>&& localVarDecl)
        { m_localVarDecl = std::move(localVarDecl); }

        void setExp1(DynamicNode<Expression>& exp1)
        { m_exp1 = std::move(exp1); }
        void setExp1(DynamicNode<Expression>&& exp1)
        { m_exp1 = std::move(exp1); }

        void setExp2(DynamicNode<Expression>& exp2)
        { m_exp2 = std::move(exp2); }
        void setExp2(DynamicNode<Expression>&& exp2)
        { m_exp2 = std::move(exp2); }

        void setStatement(DynamicNode<Statement>& statement)
        { m_statement = std::move(statement); }
        void setStatement(DynamicNode<Statement>&& statement)
        { m_statement = std::move(statement); }

        auto& getLocalVarDecl()
        { return m_localVarDecl; }
        const auto& getLocalVarDecl() const
        { return m_localVarDecl; }

        auto& getExp1()
        { return m_exp1; }
        const auto& getExp1() const
        { return m_exp1; }

        auto& getExp2()
        { return m_exp2; }
        const auto& getExp2() const
        { return m_exp2; }

        auto& getStatement()
        { return m_statement; }
        const auto& getStatement() const
        { return m_statement; }

        void clear()
        { m_localVarDecl.clear(), m_exp1.clear(), m_exp2.clear(), m_statement.clear(); }

        bool isEmpty() const
        { return m_localVarDecl.isNull() or m_exp1.isNull() or m_exp2.isNull() or m_statement.isNull(); }
    };

    class WhileLoop
    {
    private:
        DynamicNode<Expression> m_conditionExp{};
        DynamicNode<Statement> m_statement{};

    public:
        WhileLoop() = default;

        WhileLoop(DynamicNode<Expression>& conditionExp, DynamicNode<Statement>& statement):
        m_conditionExp{ std::move(conditionExp) }, m_statement{ std::move(statement) }{}

        WhileLoop(DynamicNode<Expression>&& conditionExp, DynamicNode<Statement>&& statement):
        m_conditionExp{ std::move(conditionExp) }, m_statement{ std::move(statement) }{}

        void setConditionExp(DynamicNode<Expression>& conditionExp)
        { m_conditionExp = std::move(conditionExp); }
        void setConditionExp(DynamicNode<Expression>&& conditionExp)
        { m_conditionExp = std::move(conditionExp); }

        void setStatement(DynamicNode<Statement>& statement)
        { m_statement = std::move(statement); }
        void setStatement(DynamicNode<Statement>&& statement)
        { m_statement = std::move(statement); }

        auto& getCondtionExp()
        { return m_conditionExp; }
        const auto& getCondtionExp() const
        { return m_conditionExp; }

        auto& getStatement()
        { return m_statement; }
        const auto& getStatement() const
        { return m_statement; }

        void clear()
        { m_conditionExp.clear(), m_statement.clear(); }

        bool isEmpty() const
        { return m_conditionExp.isNull() or m_statement.isNull(); }
    };

    class DoWhileLoop
    {
    private:
        DynamicNode<Statement> m_statement{};
        DynamicNode<Expression> m_whileConditionExp{};

    public:
        DoWhileLoop() = default;

        DoWhileLoop(DynamicNode<Statement>& statement, DynamicNode<Expression>& whileConditionExp):
        m_statement{ std::move(statement) }, m_whileConditionExp{ std::move(whileConditionExp) }{}

        DoWhileLoop(DynamicNode<Statement>&& statement, DynamicNode<Expression>&& whileConditionExp):
        m_statement{ std::move(statement) }, m_whileConditionExp{ std::move(whileConditionExp) }{}

        void setStatement(DynamicNode<Statement>& statement)
        { m_statement = std::move(statement); }
        void setStatement(DynamicNode<Statement>&& statement)
        { m_statement = std::move(statement); }

        void setWhileConditionExp(DynamicNode<Expression>& whileConditionExp)
        { m_whileConditionExp = std::move(whileConditionExp); }
        void setWhileConditionExp(DynamicNode<Expression>&& whileConditionExp)
        { m_whileConditionExp = std::move(whileConditionExp); }

        auto& getStatement()
        { return m_statement; }
        const auto& getStatement() const
        { return m_statement; }

        auto& getWhileConditionExp()
        { return m_whileConditionExp; }
        const auto& getWhileConditionExp() const
        { return m_whileConditionExp; }

        void clear()
        { m_statement.clear(), m_whileConditionExp.clear(); }

        bool isEmpty() const
        { return m_statement.isNull() or m_whileConditionExp.isNull(); }
    };

    class VarDecl
    {
    public:
        using IdentInitPair = std::pair<Identifier, std::optional<DynamicNode<Expression>>>;
        using IdentInitPairList = List<IdentInitPair>;

    private:
        Type m_varType{};
        IdentInitPairList m_identInitPairlist{};
        LocationFinder m_locFinder{};

    public:
        VarDecl() = default;

        VarDecl(Type& varType, IdentInitPairList& identInitPair, LocationFinder& locFinder):
        m_varType{ std::move(varType) }, m_identInitPairlist{ std::move(identInitPair) }, m_locFinder{ locFinder }{}

        VarDecl(Type&& varType, IdentInitPairList&& identInitPair, LocationFinder&& locFinder):
        m_varType{ std::move(varType) }, m_identInitPairlist{ std::move(identInitPair) }, m_locFinder{ locFinder }{}

        void setVarType(Type& varType)
        { m_varType = std::move(varType); }
        void setVarType(Type&& varType)
        { m_varType = std::move(varType); }

        void setIdentInitPairList(IdentInitPairList& identIniitPair)
        { m_identInitPairlist = std::move(identIniitPair); }
        void setIdentInitPairList(IdentInitPairList&& identIniitPair)
        { m_identInitPairlist = std::move(identIniitPair); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        void pushBack_identInitPair(IdentInitPair& i)
        { m_identInitPairlist.push_back(std::move(i)); }
        void pushBack_identInitPair(IdentInitPair&& i)
        { m_identInitPairlist.push_back(std::move(i)); }

        auto& getVarType()
        { return m_varType; }
        const auto& getVarType() const
        { return m_varType; }

        auto& getIdentInitPairList()
        { return m_identInitPairlist; }
        const auto& getIdentInitPairList() const
        { return m_identInitPairlist; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        { m_varType.clear(), m_identInitPairlist.clear(); }

        bool isEmpty() const
        { return m_varType.isEmpty() or m_identInitPairlist.empty(); }
    };

    class Block
    {
    private:
        std::variant<Void, List<DynamicNode<BlockItem>> > m_blockContents{};
        LocationFinder m_locFinder{};

    public:
        Block() = default;

        Block(Void& blockContents, LocationFinder& locFinder): m_blockContents{ std::move(blockContents) }, m_locFinder{locFinder} {}
        Block(Void&& blockContents, LocationFinder&& locFinder): m_blockContents{ std::move(blockContents) }, m_locFinder{locFinder} {}
        Block(List<DynamicNode<BlockItem>>& blockContents, LocationFinder& locFinder): m_blockContents{ std::move(blockContents) }, m_locFinder{locFinder} {}
        Block(List<DynamicNode<BlockItem>>&& blockContents, LocationFinder&& locFinder): m_blockContents{ std::move(blockContents) }, m_locFinder{locFinder} {}

        void setBlockContents(Void& blockContents)
        { m_blockContents = std::move(blockContents); }
        void setBlockContents(Void&& blockContents)
        { m_blockContents = std::move(blockContents); }

        void setBlockContents(List<DynamicNode<BlockItem>>& blockContents)
        { m_blockContents = std::move(blockContents); }
        void setBlockContents(List<DynamicNode<BlockItem>>&& blockContents)
        { m_blockContents = std::move(blockContents); }

        void setLocationFinder(LocationFinder& locFinder)
        { m_locFinder = locFinder; }
        void setLocationFinder(LocationFinder&& locFinder)
        { m_locFinder = locFinder; }

        auto& getBlockContents()
        { return m_blockContents; }
        const auto& getBlockContents() const
        { return m_blockContents; }

        auto& getLocationFinder()
        { return m_locFinder; }
        const auto& getLocationFinder() const
        { return m_locFinder; }

        void clear()
        {
            auto lambda = [](auto& alt)
            {
                if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
                alt.reset();
                else
                alt.clear();
            };
            std::visit(lambda, m_blockContents);
        }

        bool isEmpty() const
        {
            auto lambda = [](const auto& alt)
            {
                if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
                return alt.isEmpty();
                else
                return alt.empty();
            };
            return std::visit(lambda, m_blockContents);
        }
    };

    class FunctionDef
    {
    private:
        Type m_giveType{};
        Identifier m_functionIdent{};
        ParameterList m_parameterList{};
        Block m_block{};

    public:
        FunctionDef() = default;

        FunctionDef(Type& giveType, Identifier& functionIdent, ParameterList& parameterList, Block& block):
        m_giveType{std::move(giveType)}, m_functionIdent{std::move(functionIdent)}, m_parameterList{std::move(parameterList)}, m_block{std::move(block)}{}

        FunctionDef(Type& giveType, Identifier&& functionIdent, ParameterList&& parameterList, Block&& block):
        m_giveType{std::move(giveType)}, m_functionIdent{std::move(functionIdent)}, m_parameterList{std::move(parameterList)}, m_block{std::move(block)}{}
        
        void setGiveType(Type& giveType)
        { m_giveType = std::move(giveType); }
        void setGiveType(Type&& giveType)
        { m_giveType = std::move(giveType); }

        void setFunctionIdent(Identifier& functionIdent)
        { m_functionIdent = std::move(functionIdent); }
        void setFunctionIdent(Identifier&& functionIdent)
        { m_functionIdent = std::move(functionIdent); }

        void setParameterList(ParameterList& parameterList)
        { m_parameterList = std::move(parameterList); }
        void setParameterList(ParameterList&& parameterList)
        { m_parameterList = std::move(parameterList); }

        void setBlock(Block& block)
        { m_block = std::move(block); }
        void setBlock(Block&& block)
        { m_block = std::move(block); }

        void pushBack_parameter(std::pair<Type, std::optional<Identifier>>& parameter)
        { m_parameterList.push_back(std::move(parameter)); }
        void pushBack_parameter(std::pair<Type, std::optional<Identifier>>&& parameter)
        { m_parameterList.push_back(std::move(parameter)); }

        auto& getGiveType()
        { return m_giveType; }
        const auto& getGiveType() const
        { return m_giveType; }

        auto& getFunctionIdent()
        { return m_functionIdent; }
        const auto& getFunctionIdent() const
        { return m_functionIdent; }

        auto& getParameterList()
        { return m_parameterList; }
        const auto& getParameterList() const
        { return m_parameterList; }

        auto& getBlock()
        { return m_block; }
        const auto& getBlock() const
        { return m_block; }

        void clear()
        { m_giveType.clear(), m_functionIdent.clear(), m_parameterList.clear(), m_block.clear(); }

        bool isEmpty() const
        { return m_giveType.isEmpty() or m_functionIdent.isEmpty(); }
    };

    class FunctionDecl
    {
    private:
        Type m_giveType{};
        Identifier m_functionIdent{};
        ParameterList m_parameterList{};

    public:
        FunctionDecl() = default;

        FunctionDecl(Type& giveType, Identifier& functionIdent, ParameterList& parameterList):
        m_giveType{std::move(giveType)}, m_functionIdent{std::move(functionIdent)}, m_parameterList{std::move(parameterList)}{}

        FunctionDecl(Type& giveType, Identifier&& functionIdent, ParameterList&& parameterList):
        m_giveType{std::move(giveType)}, m_functionIdent{std::move(functionIdent)}, m_parameterList{std::move(parameterList)}{}
        
        void setGiveType(Type& giveType)
        { m_giveType = std::move(giveType); }
        void setGiveType(Type&& giveType)
        { m_giveType = std::move(giveType); }

        void setFunctionIdent(Identifier& functionIdent)
        { m_functionIdent = std::move(functionIdent); }
        void setFunctionIdent(Identifier&& functionIdent)
        { m_functionIdent = std::move(functionIdent); }

        void setParameterList(ParameterList& parameterList)
        { m_parameterList = std::move(parameterList); }
        void setParameterList(ParameterList&& parameterList)
        { m_parameterList = std::move(parameterList); }

        void pushBack_parameter(std::pair<Type, std::optional<Identifier>>& parameter)
        { m_parameterList.push_back(std::move(parameter)); }
        void pushBack_parameter(std::pair<Type, std::optional<Identifier>>&& parameter)
        { m_parameterList.push_back(std::move(parameter)); }

        auto& getGiveType()
        { return m_giveType; }
        const auto& getGiveType() const
        { return m_giveType; }

        auto& getFunctionIdent()
        { return m_functionIdent; }
        const auto& getFunctionIdent() const
        { return m_functionIdent; }

        auto& getParameterList()
        { return m_parameterList; }
        const auto& getParameterList() const
        { return m_parameterList; }

        void clear()
        { m_giveType.clear(), m_functionIdent.clear(), m_parameterList.clear(); }

        bool isEmpty() const
        { return m_giveType.isEmpty() or m_functionIdent.isEmpty(); }
    };
    
    class NcFile
    {
    public:
        using DynamicFuncDeclaration = DynamicNode<FuncDeclaration>;
        using DynamicVarDeclaration = DynamicNode<VarDeclaration>;

    private:
        List<std::variant<DynamicVarDeclaration, DynamicFuncDeclaration>> m_globalContents{};

    public:
        NcFile() = default;

        NcFile(DynamicVarDeclaration& globalContents): m_globalContents{ std::move(globalContents) } {}
        NcFile(DynamicVarDeclaration&& globalContents): m_globalContents{ std::move(globalContents) } {}
        NcFile(DynamicFuncDeclaration& globalContents): m_globalContents{ std::move(globalContents) } {}
        NcFile(DynamicFuncDeclaration&& globalContents): m_globalContents{ std::move(globalContents) } {}

        void setGlobalContents(DynamicVarDeclaration& globalContents)
        { m_globalContents.push_back(std::move(globalContents)); }
        void setGlobalContents(DynamicVarDeclaration&& globalContents)
        { m_globalContents.push_back(std::move(globalContents)); }
        void setGlobalContents(DynamicFuncDeclaration& globalContents)
        { m_globalContents.push_back(std::move(globalContents)); }
        void setGlobalContents(DynamicFuncDeclaration&& globalContents)
        { m_globalContents.push_back(std::move(globalContents)); }

        auto& getGlobalContents()
        { return m_globalContents; }
        const auto& getGlobalContents() const
        { return m_globalContents; }

        void clear()
        { m_globalContents.clear(); }

        bool isEmpty() const
        { return m_globalContents.empty(); }
    };

} // namespace nc

#endif