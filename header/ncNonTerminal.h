#ifndef NCNONTERMINAL_H
#define NCNONTERMINAL_H

#include <regex>

#include "ncTerminal.h"

namespace Nc
{
    class Expression
    {
    private:
        std::variant<UPrefixOperation, UPostfixOperation, BinaryOperation, TenaryOperation, Literal, Identifier, TempVarDecl, AnyTypeListExp, R_And_L_Exp, Block> m_exp{};

    public:
        Expression() = default;

        Expression(UPrefixOperation& exp): m_exp{ std::move(exp) } {}
        Expression(UPrefixOperation&& exp): m_exp{ std::move(exp) } {}
        Expression(UPostfixOperation& exp): m_exp{ std::move(exp) } {}
        Expression(UPostfixOperation&& exp): m_exp{ std::move(exp) } {}
        Expression(BinaryOperation& exp): m_exp{ std::move(exp) } {}
        Expression(BinaryOperation&& exp): m_exp{ std::move(exp) } {}
        Expression(TenaryOperation& exp): m_exp{ std::move(exp) } {}
        Expression(TenaryOperation&& exp): m_exp{ std::move(exp) } {}
        Expression(Literal& exp): m_exp{ std::move(exp) } {}
        Expression(Literal&& exp): m_exp{ std::move(exp) } {}
        Expression(Identifier& exp): m_exp{ std::move(exp) } {}
        Expression(Identifier&& exp): m_exp{ std::move(exp) } {}
        Expression(TempVarDecl& exp): m_exp{ std::move(exp) } {}
        Expression(TempVarDecl&& exp): m_exp{ std::move(exp) } {}
        Expression(AnyTypeListExp& exp): m_exp{ std::move(exp) } {}
        Expression(AnyTypeListExp&& exp): m_exp{ std::move(exp) } {}
        Expression(R_And_L_Exp& exp): m_exp{ std::move(exp) } {}
        Expression(R_And_L_Exp&& exp): m_exp{ std::move(exp) } {}
        Expression(Block& exp): m_exp{ std::move(exp) } {}
        Expression(Block&& exp): m_exp{ std::move(exp) } {}
        
        void setExp(UPrefixOperation& exp)
        { m_exp = std::move(exp); }
        void setExp(UPrefixOperation&& exp)
        { m_exp = std::move(exp); }
        void setExp(UPostfixOperation& exp)
        { m_exp = std::move(exp); }
        void setExp(UPostfixOperation&& exp)
        { m_exp = std::move(exp); }
        void setExp(BinaryOperation& exp)
        { m_exp = std::move(exp); }
        void setExp(BinaryOperation&& exp)
        { m_exp = std::move(exp); }
        void setExp(TenaryOperation& exp)
        { m_exp = std::move(exp); }
        void setExp(TenaryOperation&& exp)
        { m_exp = std::move(exp); }
        void setExp(Literal& exp)
        { m_exp = std::move(exp); }
        void setExp(Literal&& exp)
        { m_exp = std::move(exp); }
        void setExp(Identifier& exp)
        { m_exp = std::move(exp); }
        void setExp(Identifier&& exp)
        { m_exp = std::move(exp); }
        void setExp(TempVarDecl& exp)
        { m_exp = std::move(exp); }
        void setExp(TempVarDecl&& exp)
        { m_exp = std::move(exp); }
        void setExp(AnyTypeListExp& exp)
        { m_exp = std::move(exp); }
        void setExp(AnyTypeListExp&& exp)
        { m_exp = std::move(exp); }
        void setExp(R_And_L_Exp& exp)
        { m_exp = std::move(exp); }
        void setExp(R_And_L_Exp&& exp)
        { m_exp = std::move(exp); }
        void setExp(Block& exp)
        { m_exp = std::move(exp); }
        void setExp(Block&& exp)
        { m_exp = std::move(exp); }

        auto& getExp()
        { return m_exp; }
        const auto& getExp() const
        { return m_exp; }

        auto& getExpLocation()
        { return std::visit([](const auto& item) -> auto& { return item.getLocationFinder(); }, m_exp); }
        const auto& getExpLocation() const
        { return std::visit([](const auto& item) -> auto& { return item.getLocationFinder(); }, m_exp); }

        void clear()
        { std::visit([](auto& item){ item.clear(); }, m_exp); }

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, m_exp); }

        bool isLiteral() const
        {
            return std::visit([]<typename T>(T&) { 
                if constexpr (std::is_same_v<std::decay_t<T>, Literal>) return true;
                return false; }
            , m_exp);
        }

        bool isBlock() const
        {
            return std::visit([]<typename T>(T&) { 
                if constexpr (std::is_same_v<std::decay_t<T>, Block>) return true;
                return false; }
            , m_exp);
        }
    };
    
    class VarDeclaration
    {
    private:
        VarDecl m_varDecl{};

    public:
        VarDeclaration() = default;

        VarDeclaration(VarDecl& varDecl): m_varDecl{ std::move(varDecl) }{}

        VarDeclaration(VarDecl&& varDecl): m_varDecl{ std::move(varDecl) }{}

        void setVarDecl(VarDecl& varDecl)
        { m_varDecl = std::move(varDecl); }
        void setVarDecl(VarDecl&& varDecl)
        { m_varDecl = std::move(varDecl); }

        auto& getVarDecl()
        { return m_varDecl; }
        const auto& getVarDecl() const
        { return m_varDecl; }

        void clear()
        { m_varDecl.clear(); }

        bool isEmpty() const
        { return m_varDecl.isEmpty(); }
    };
    
    class Statement
    {
    private:
        std::variant<JumpStatement, IfCondition, ForLoop, WhileLoop, DoWhileLoop, Expression> m_statement{};

    public:
        Statement() = default;

        Statement(JumpStatement& statement): m_statement{ std::move(statement) } {}
        Statement(JumpStatement&& statement): m_statement{ std::move(statement) } {}
        Statement(IfCondition& statement): m_statement{ std::move(statement) } {}
        Statement(IfCondition&& statement): m_statement{ std::move(statement) } {}
        Statement(ForLoop& statement): m_statement{ std::move(statement) } {}
        Statement(ForLoop&& statement): m_statement{ std::move(statement) } {}
        Statement(WhileLoop& statement): m_statement{ std::move(statement) } {}
        Statement(WhileLoop&& statement): m_statement{ std::move(statement) } {}
        Statement(DoWhileLoop& statement): m_statement{ std::move(statement) } {}
        Statement(DoWhileLoop&& statement): m_statement{ std::move(statement) } {}
        Statement(Expression& statement): m_statement{ std::move(statement) } {}
        Statement(Expression&& statement): m_statement{ std::move(statement) } {}

        void setStatement(JumpStatement& statement)
        { m_statement = std::move(statement); }
        void setStatement(JumpStatement&& statement)
        { m_statement = std::move(statement); }
        void setStatement(IfCondition& statement)
        { m_statement = std::move(statement); }
        void setStatement(IfCondition&& statement)
        { m_statement = std::move(statement); }
        void setStatement(ForLoop& statement)
        { m_statement = std::move(statement); }
        void setStatement(ForLoop&& statement)
        { m_statement = std::move(statement); }
        void setStatement(WhileLoop& statement)
        { m_statement = std::move(statement); }
        void setStatement(WhileLoop&& statement)
        { m_statement = std::move(statement); }
        void setStatement(DoWhileLoop& statement)
        { m_statement = std::move(statement); }
        void setStatement(DoWhileLoop&& statement)
        { m_statement = std::move(statement); }
        void setStatement(Expression& statement)
        { m_statement = std::move(statement); }
        void setStatement(Expression&& statement)
        { m_statement = std::move(statement); }

        auto& getStatement()
        { return m_statement; }
        const auto& getStatement() const
        { return m_statement; }

        void clear()
        { std::visit([](auto& alt) { std::remove_reference_t<decltype(alt)>{ std::move(alt) }; }, m_statement); }

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, m_statement); }
    };

    class BlockItem
    {
    private:
        std::variant<Statement, VarDeclaration>  m_blockItem{};

    public:
        BlockItem() = default;

        BlockItem(Statement& blockItem): m_blockItem{ std::move(blockItem) } {}
        BlockItem(Statement&& blockItem): m_blockItem{ std::move(blockItem) } {}
        BlockItem(VarDeclaration& blockItem): m_blockItem{ std::move(blockItem) } {}
        BlockItem(VarDeclaration&& blockItem): m_blockItem{ std::move(blockItem) } {}
    

        void setBlockItem(Statement& blockItem)
        { m_blockItem = std::move(blockItem); }
        void setBlockItem(Statement&& blockItem)
        { m_blockItem = std::move(blockItem); }
        void setBlockItem(VarDeclaration& blockItem)
        { m_blockItem = std::move(blockItem); }
        void setBlockItem(VarDeclaration&& blockItem)
        { m_blockItem = std::move(blockItem); }

        auto& getBlockItem()
        { return m_blockItem; }
        const auto& getBlockItem() const
        { return m_blockItem; }

        void clear()
        { std::visit([](auto& item){ item.clear(); }, m_blockItem); }

        bool isEmpty() const
        { return std::visit([](const auto& alt) { return alt.isEmpty(); }, m_blockItem); }
    };
    
    class FuncDeclaration
    {
    private:
        std::variant<FunctionDecl, FunctionDef> m_funcDeclaration{};

    public:
        FuncDeclaration() = default;

        FuncDeclaration(FunctionDecl& funcDeclaration): m_funcDeclaration{ std::move(funcDeclaration) } {}
        FuncDeclaration(FunctionDecl&& funcDeclaration): m_funcDeclaration{ std::move(funcDeclaration) } {}
        FuncDeclaration(FunctionDef& funcDeclaration): m_funcDeclaration{ std::move(funcDeclaration) } {}
        FuncDeclaration(FunctionDef&& funcDeclaration): m_funcDeclaration{ std::move(funcDeclaration) } {}

        void setFuncDeclaration(FunctionDecl& funcDeclaration)
        { m_funcDeclaration = std::move(funcDeclaration); }
        void setFuncDeclaration(FunctionDecl&& funcDeclaration)
        { m_funcDeclaration = std::move(funcDeclaration); }
        void setFuncDeclaration(FunctionDef& funcDeclaration)
        { m_funcDeclaration = std::move(funcDeclaration);  }
        void setFuncDeclaration(FunctionDef&& funcDeclaration)
        { m_funcDeclaration = std::move(funcDeclaration);  }


        auto& getFuncDeclaration()
        { return m_funcDeclaration; }
        const auto& getFuncDeclaration() const
        { return m_funcDeclaration; }

        void clear()
        { std::visit([](auto& item){ item.clear(); }, m_funcDeclaration); }

        bool isEmpty() const
        { return std::visit([](const auto& item){ return item.isEmpty(); }, m_funcDeclaration); }
    };
    
    class Root
    {
    private:
        NcFile m_ncf{};

    public:
        Root() = default;
        Root(NcFile& ncf): m_ncf{ std::move(ncf) }
        {}
        Root(NcFile&& ncf): m_ncf{ std::move(ncf) }
        {}

        void setNcf(NcFile& ncf)
        { m_ncf = std::move(ncf); }
        void setNcf(NcFile&& ncf)
        { m_ncf = std::move(ncf); }

        auto& getNcf()
        { return m_ncf; }
        const auto& getNcf() const
        { return m_ncf; }

        void clear()
        { m_ncf.clear(); }

        bool isEmpty() const
        { return m_ncf.isEmpty(); }
    };

    /*───────────────────────────────────────────────────TypeEquality Overloads─────────────────────────────────────────────────────────────────────────────*/

    bool operator==(const Type::FunctionType& fT1, const Type::FunctionType& fT2);
    bool operator!=(const Type::FunctionType& fT1, const Type::FunctionType& fT2);
    bool operator==(const Type& t1, const Type& t2);
    bool operator!=(const Type& t1, const Type& t2);


    inline bool operator==(const Type::FunctionType& fT1, const Type::FunctionType& fT2)
    {
        if (fT1.isEmpty() or fT2.isEmpty())
        throw std::runtime_error{ "in function Nc::operator==(const Type::FunctionType&, const Type::FunctionType&), fT1 or fT2 is empty" };

        if (fT1.getParameterTypeList().size() != fT2.getParameterTypeList().size())
        return false;
        
        if (fT1.getGiveType() != fT2.getGiveType())
        return false;
        
        //since by this point i know that the number of parameters are equal, checking if the parameterList is empty for one is okay
        if (fT1.getParameterTypeList().empty())
        return true;

        for (auto i = fT1.getParameterTypeList().begin(), j = fT2.getParameterTypeList().begin(); i != fT2.getParameterTypeList().end(); i++, j++)
        {
            if (**i != **j)
            return false;
        }
        
        return true;
    }

    inline bool operator!=(const Type::FunctionType& fT1, const Type::FunctionType& fT2)
    { return !(fT1 == fT2); }

    inline bool operator==(const Type& t1, const Type& t2)
    {
        //lambda to fisrt check if both t1 and t2 hold the same base type, by using a common type
        auto lambda1 = []<typename T>(T&)
        {
            if constexpr (std::is_same_v<Type::NormalType, std::decay_t<T>>)
            return true;
            else
            return false;
        };

        if (std::visit(lambda1, t1.getType()) != std::visit(lambda1, t2.getType()))
        return false;
        
        //lambda to check if that same base type are actually equal
        // auto lambda2 = [&]<typename T>(T&)
        // {
        //     if constexpr (std::is_same_v<std::decay_t<T>, Type::NormalType>)
        //     return std::get<Type::NormalType>(t1.getType()) == std::get<Type::NormalType>(t2.getType());
        //     else
        //     return std::get<Type::FunctionType>(t1.getType()) == std::get<Type::FunctionType>(t2.getType());
        // };

        // std::regex reg{ t1.getTypeAttributes(), std::regex_constants::icase };

        // if (std::visit(lambda2, t1.getType()) and std::regex_match(t2.getTypeAttributes(), reg))
        // return true;

        return false;
    }

    inline bool operator!=(const Type& t1, const Type& t2)
    { return !(t1 == t2); }
    
    /*──────────────────────────────────────────────────────────MakeDynamicNode─────────────────────────────────────────────────────────────────────────────*/

    template <typename NodeType>
    auto mk_dyNode(NodeType& nT)
    { return DynamicNode<NodeType>{ new NodeType{std::move(nT)} }; }

    /*──────────────────────────────────────────────────────────OutputStream Overloads───────────────────────────────────────────────────────────────────────*/


    std::ostream& operator<<(std::ostream& os, const Type::FunctionType& d);
    std::ostream& operator<<(std::ostream& os, const Type& d);
    
    std::ostream& operator<<(std::ostream& os, const Block& d);
    std::ostream& operator<<(std::ostream& os, const R_And_L_Exp& d);
    std::ostream& operator<<(std::ostream& os, const AnyTypeListExp& d);
    std::ostream& operator<<(std::ostream& os, const TempVarDecl& d);
    std::ostream& operator<<(std::ostream& os, const Literal& d);
    std::ostream& operator<<(std::ostream& os, const Identifier& d);
    std::ostream& operator<<(std::ostream& os, const UPrefixOperation& d);
    std::ostream& operator<<(std::ostream& os, const UPostfixOperation& d);
    std::ostream& operator<<(std::ostream& os, const BinaryOperation& d);
    std::ostream& operator<<(std::ostream& os, const TenaryOperation& d);
    std::ostream& operator<<(std::ostream& os, const Expression& d);


    std::ostream& operator<<(std::ostream& os, const Give& d);
    std::ostream& operator<<(std::ostream& os, const Break& d);
    std::ostream& operator<<(std::ostream& os, const Continue& d);
    std::ostream& operator<<(std::ostream& os, const JumpStatement& d);
    std::ostream& operator<<(std::ostream& os, const IfCondition& d);
    std::ostream& operator<<(std::ostream& os, const ForLoop& d);
    std::ostream& operator<<(std::ostream& os, const WhileLoop& d);
    std::ostream& operator<<(std::ostream& os, const DoWhileLoop& d);
    std::ostream& operator<<(std::ostream& os, const Statement& d);

    std::ostream& operator<<(std::ostream& os, const VarDecl& d);
    std::ostream& operator<<(std::ostream& os, const VarDeclaration& d);

    std::ostream& operator<<(std::ostream& os, const BlockItem& d);

    std::ostream& operator<<(std::ostream& os, const FunctionDecl& d);
    std::ostream& operator<<(std::ostream& os, const FunctionDef& d);
    std::ostream& operator<<(std::ostream& os, const FuncDeclaration& d);

    std::ostream& operator<<(std::ostream& os, const NcFile& d);
    std::ostream& operator<<(std::ostream& os, const Root& d);

    //purpose is to add whitespace
    inline std::string whitespace{};

    inline std::ostream& operator<<(std::ostream& os, const Type::FunctionType& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Type::FunctionType&): object d is empty" };

        os << "!((";
        for (std::size_t count{1}; auto &&i : d.getParameterTypeList())
        {
            os << *i;
            if (count != d.getParameterTypeList().size())
            os << ", ";

            ++count;
        }
        return os << ')' << ' ' << *d.getGiveType() << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const Type& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Type&): object d is empty" };

        std::visit([&](const auto& item){ os << item;}, d.getType());

        if (!d.getTypeAttributesExp().empty())
        os << ": (" << d.getTypeAttributesExp() << ')';

        return os;
    }


    inline std::ostream& operator<<(std::ostream& os, const R_And_L_Exp& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const R_And_L_Exp&): object d is empty" };

        os << "R&L_exp(" << d.getRelationalOp() << ", " << d.getLogicalOp() << ',';

        if (d.getExp()->isBlock())
        os << *d.getExp() << whitespace << ")(";
        else
        os << ' ' << *d.getExp() << ")(";

        for (auto &&i : d.getExpList())
        {
            os << *i;

            if (i->isBlock())
            os << whitespace;

            if (i != d.getExpList().back())
            os << ", ";
        }
        return os << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const AnyTypeListExp& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const AnyTypeListExp&): object d is empty" };

        os << "!{";

        auto lambda = [&](const auto& alt) -> std::ostream&
        {
            if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
            return os << "";
            else
            {
                for (auto&& i : alt)
                {
                    os << *i;

                    if (i->isBlock())
                    os << whitespace;

                    if (i != alt.back())
                    os << ", ";
                }
                return os;
            }
        };

        return std::visit(lambda, d.getListContents()) << "}";
    }

    inline std::ostream& operator<<(std::ostream& os, const TempVarDecl& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const TempVarDecl&): object d is empty" };

        return os << "TempVarDecl(" << d.getVariableType() << " := " << d.getAnyTypeListExp() << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const Literal& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Literal&): object d is empty" };

        os << "Literal(" << d.getLiteralConstant();

        if (!d.getLiteralType().empty())
        os <<  ':' << d.getLiteralType();

        return os << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const Identifier& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Identifier&): object d is empty" };

        os << "Ident(" << d.getIdentName();

        if (!d.getIdentType().isEmpty())
        os << ':' << d.getIdentType();

        return os << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const UPrefixOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const UPrefixOperation&): object d is empty" };

        os << "UPrefix_op⟪" << d.getOperator();

        if (d.getRightExp()->isBlock())
        os << *d.getRightExp() << whitespace;
        else
        os << ' ' << *d.getRightExp();
        
        return os << "⟫";
    }

    inline std::ostream& operator<<(std::ostream& os, const UPostfixOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const UPostfixOperation&): object d is empty" };
        
        os << "UPostfix_op⟨" << *d.getLeftExp();

        if (d.getLeftExp()->isBlock())
        os << whitespace;
        else
        os << ' ';

        std::visit([&](const auto& alt){
            if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Operator>)
            os << alt;
            else
            {
                os << "Call(";
                for (auto &&i : alt)
                {
                    os << *i;

                    if (i->isBlock())
                    os << whitespace;

                    if (i != alt.back())
                    {    
                        os << ", ";
                    }
                }
                os << ')';
            }
            
        }, d.getOperator());
        return os << "⟩";
    }

    inline std::ostream& operator<<(std::ostream& os, const BinaryOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const BinaryOperation&): object d is empty" };

        os << "B_op⟦";

        if (d.getLeftExp()->isBlock())
        os << *d.getLeftExp() << whitespace << d.getOperator();
        else
        os << *d.getLeftExp() << ' ' << d.getOperator();
        
        if (d.getRightExp()->isBlock())
        os << *d.getRightExp() << whitespace;
        else
        os << ' ' << *d.getRightExp();

        return os << "⟧";
        //⟯⟦ ⟧⟬ ⟭ ⁅ ⁆⟪ ⟫
    }

    inline std::ostream& operator<<(std::ostream& os, const TenaryOperation& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const TenaryOperation&): object d is empty" };

        os << "T_op⟬";

        if (d.getConditionExp()->isBlock())
        os << *d.getConditionExp() << whitespace << d.getOperator();
        else
        os << *d.getConditionExp() << ' ' << d.getOperator();

        if (d.getLeftExp()->isBlock())
        os << *d.getLeftExp() << whitespace << ':';
        else
        os << ' ' << *d.getLeftExp() << ' ' << ':';

        if (d.getRightExp()->isBlock())
        os << *d.getRightExp() << whitespace;
        else
        os << ' ' << *d.getRightExp();

        return os << "⟭";
    }

    inline std::ostream& operator<<(std::ostream& os, const Expression& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Expression&): object d is empty" };

        return std::visit( [&](const auto& item) -> std::ostream& {
            // if constexpr (std::is_same_v<std::decay_t<decltype(item)>, Block>)
            // comingFromBlockExp = true;

            return os << item; 
        }, d.getExp());
    }

    inline std::ostream& operator<<(std::ostream& os, const Give& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Give&): object d is empty" };

        os << "Give";
        std::visit([&](const auto& alt)
        {
            if constexpr (std::is_same_v<Void, std::decay_t<decltype(alt)>>)
            os << "";
            else
            {
                if (alt->isBlock())
                os << *alt;
                else
                os << ' ' << *alt;
            }
        }, d.get_exp_void());

        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Break& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Break&): object d is empty" };

        return os << "Break";
    }

    inline std::ostream& operator<<(std::ostream& os, const Continue& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Continue&): object d is empty" };

        return os << "Continue";
    }

    inline std::ostream& operator<<(std::ostream& os, const JumpStatement& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const JumpStatement&): object d is empty" };

        return std::visit([&](const auto& item) -> std::ostream& { return os << item; }, d.getJStatement());
    }
    
    inline std::ostream& operator<<(std::ostream& os, const IfCondition& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const IfCondition&): object d is empty" };

        os << whitespace << "If (" << *d.getConditionExp() << ")\n" << whitespace << *d.getStatementIfTrue();

        if (d.getStatementIfFalse().has_value())
        os << '\n' << whitespace << "Else\n" << whitespace << **d.getStatementIfFalse();
        
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ForLoop& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const ForLoop&): object d is empty" };

        os << whitespace << "For (" << *d.getLocalVarDecl() << ';' << *d.getExp1() << ';' << *d.getExp2() << ")\n" << whitespace <<  *d.getStatement();

        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const WhileLoop& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const WhileLoop&): object d is empty" };

        return os << whitespace << "While (" << *d.getCondtionExp() << ")\n" << whitespace << *d.getStatement();
    }

    inline std::ostream& operator<<(std::ostream& os, const DoWhileLoop& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const DoWhileLoop&): object d is empty" };

        return os << whitespace << "Do\n" << whitespace << *d.getStatement() << '\n' << whitespace  << "While" << *d.getWhileConditionExp();
    }

    inline std::ostream& operator<<(std::ostream& os, const Block& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Block&): object d is empty" };

        auto lambda = [&](const auto& alt) -> std::ostream&
        {
            if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, Void>)
            return os << '\n' << whitespace << "Block {}\n";
            else
            {
                os << '\n' << whitespace << "Block\n" << whitespace << "{\n";
                for (auto &&i : alt)
                {
                    os << *i;
                }
                return os << whitespace << '}' << '\n';
            }
        };

        return std::visit(lambda, d.getBlockContents());
    }

    inline std::ostream& operator<<(std::ostream& os, const Statement& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Statement&): object d is empty" };
        
        std::visit( [&](const auto& alt){
            if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, JumpStatement>)
            os << "Statement[Jump]: ";
            else if constexpr (std::is_same_v<std::decay_t<decltype(alt)>, IfCondition>)
            os << "Statement[IfCondition]:\n";
            else if (std::is_same_v<std::decay_t<decltype(alt)>, ForLoop>)
            os << "Statement[ForLoop]:\n";
            else if (std::is_same_v<std::decay_t<decltype(alt)>, WhileLoop>)
            os << "Statement[WhileLoop]:\n";
            else if (std::is_same_v<std::decay_t<decltype(alt)>, DoWhileLoop>)
            os << "Statement[DoWhileLoop]:\n";
            else if (std::is_same_v<std::decay_t<decltype(alt)>, Expression>)
            os << "Statement[Expression]: ";

            os << alt;
        }, d.getStatement());

        return os;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const VarDecl& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const VarDecl&): object d is empty" };

        os << "VarDelcare(" << d.getVarType();

        for (std::size_t count{1}; auto &&i : d.getIdentInitPairList())
        {
            os << ' ' << i.first;

            if (i.second.has_value())
            os << " := " << *i.second.value();

            if (count != d.getIdentInitPairList().size())
            os << ',' << ' ';

            ++count;
        }

        return os << ')';
    }

    inline std::ostream& operator<<(std::ostream& os, const VarDeclaration& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const VarDeclaration&): object d is empty" };

        return os << "VarDelcaration: " << d.getVarDecl();
    }

    inline std::ostream& operator<<(std::ostream& os, const BlockItem& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const BlockItem&): object d is empty" };

        auto old_ws = whitespace;

        std::visit( [&](const auto& alt) -> std::ostream& { return os << whitespace << "BlockItem:\n" << (whitespace += {'\t'}) << alt << '\n'; }, d.getBlockItem());
        whitespace = old_ws;

        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const FunctionDef& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const FunctionDef&): object d is empty" };

        os << "FunctionDef: " << d.getGiveType() << ' ' << d.getFunctionIdent() << '(';

        for (std::size_t count{}; auto &&i : d.getParameterList())
        {
            os << i.first;

            if (i.second.has_value())
            os << ' ' << i.second.operator*();

            if (count != (d.getParameterList().size() - 1))
            os << ", ";
            ++count;
        }
        
        return os << ')' << d.getBlock();
    }

    inline std::ostream& operator<<(std::ostream& os, const FunctionDecl& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const FunctionDecl&): object d is empty" };

        os << "FunctionDecl: " << d.getGiveType() << ' ' << d.getFunctionIdent() << '(';

        for (std::size_t count{}; auto &&i : d.getParameterList())
        {
            os << i.first;
            if (i.second.has_value())
            os << i.second.operator*();

            if (count != (d.getParameterList().size() - 1))
            os << ", ";
            ++count;
        }
        
        return os << ')' << '\n';
    }

    inline std::ostream& operator<<(std::ostream& os, const FuncDeclaration& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const FuncDeclaration&): object d is empty" };

        os << "Function Declaration:\n";
        return std::visit([&](const auto& item) -> std::ostream& { return os << item << '\n'; }, d.getFuncDeclaration());
    }

    inline std::ostream& operator<<(std::ostream& os, const NcFile& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const NcFile&): object d is empty" };

        os << "NcFile:\n\n";
        os << "===========================[Start of NcFile]===========================\n\n";

        for (auto &&i : d.getGlobalContents())
        {
            std::visit([&](const auto& alt){ os << *alt; }, i);
        }
        
        return os << "==============================[End of NcFile]==============================";
    }

    inline std::ostream& operator<<(std::ostream& os, const Root& d)
    {
        if (d.isEmpty())
        throw std::runtime_error{ "Nc::operator<<(std::ostream&, const Root&): object d is empty" };

        return os << "Root -> " << d.getNcf();
    }

} // namespace nc

#endif