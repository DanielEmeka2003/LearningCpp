#ifndef NCCODEGEN_H
#define NCCODEGEN_H

#pragma once

#include <string_view>
#include <optional>
#include <map>

#include "stream/stringoutputstream.h"
#include "ncAST.h"
#include "myfunctions1.h"

namespace Nc
{
    class LLVMHelper
    {
    protected:
        using Constant = Myfcn::U8string;

        class LLVMIdentifier
        {
        public:
            enum IdentifierKind {local, global, none};
            using NcIdentifier = Myfcn::U8string;

        private:
            Myfcn::U8string m_llvmIdent{};
            IdentifierKind m_identKind{};
            
            LLVMIdentifier() = default;

        public:
            LLVMIdentifier(IdentifierKind identKind);
            LLVMIdentifier(NcIdentifier ident, IdentifierKind identKind);

            /*gets the transformed NcIdentifier[using NcIdentifier = std::string],
            which is now in LLVM format i.e. LLVMIdentifier*/
            Myfcn::U8string_view get() const;
            /*inserts the local identifier mark(%) to the begining of an identifier that has none as it's identifierKind
            [Note] if the local identifier mark(%) is already present, no operation would be done*/
            LLVMIdentifier& insertLocalMark();

            //generates random strings
            static Myfcn::U8string getRandomStr(std::size_t str_length = 4){}

        private:
            friend std::ostream& operator<<(std::ostream& os, const LLVMIdentifier& d) { return os << d.m_llvmIdent; }
            friend class LLVMHelper;
        };

        class LLVMType
        {
        public:
            using NcType = const Myfcn::U8string&;
            
        private:
            Myfcn::U8string m_llvmType{};
            
            LLVMType() = default;

        public:
            LLVMType(NcType ncT);

            Myfcn::U8string_view get() const;
            static Myfcn::U8string mapToLLVMType(NcType ncT);

            friend std::ostream& operator<<(std::ostream& os, const LLVMType& d) { return os << d.m_llvmType; }
            // friend bool operator==(LLVMType lt, NcType nt){ return nt == lt; }
            // friend bool operator==(NcType nt, LLVMType lt){ return nt == lt.m_llvmType; }
            friend bool operator==(LLVMType lt1, LLVMType lt2){ return lt1.m_llvmType == lt2.m_llvmType; }
            friend bool operator!=(LLVMType lt1, LLVMType lt2){ return lt1.m_llvmType != lt2.m_llvmType; }
            friend class LLVMHelper;
        };

        struct Value
        {
            std::variant<Constant, LLVMIdentifier> v{};
            LLVMType type{};
        };

        using Type_OptionalIdentifier_Pair = std::pair<Type, std::optional<LLVMIdentifier>>;
        using ArgumentList = List<Type_OptionalIdentifier_Pair>;
        using ValueLabelPair = std::pair<Value, LLVMIdentifier>;
        using ValueLabelList = List<ValueLabelPair>;

    private:
        Streams::StringOutputStream<char> m_stringBuffer{ std::ostringstream{} };
        Value m_value{};
        LLVMIdentifier m_currentLabel{};

    protected:
        LLVMHelper();
        ~LLVMHelper() = default;

        void endInfo();

        void beginFuncdef(const LLVMType& giveType, const LLVMIdentifier& funcI, const ArgumentList& args);
        void endFuncdef();

        void defIdentifier(const LLVMIdentifier& i, std::string_view instruction);
        void assignValue(const Value& v);
        const Value& getValue() const;

        const LLVMIdentifier& getCurrentLabel() const;

        void insertConst(const Constant& c, const LLVMType& t);
        void insertLabel(const LLVMIdentifier& label);
        void insertNewLine(std::size_t n = 1);
        void insertComment(std::string_view c);
        
        void retI(const Value& v);
        void brI(const LLVMIdentifier& label);
        void brI(const Value& conditionV, const LLVMIdentifier& true_label, const LLVMIdentifier& false_label);

        std::string addI(const Value& v1, const Value& v2);
        std::string faddI(const Value& v1, const Value& v2);
        std::string subI(const Value& v1, const Value& v2);
        std::string fsubI(const Value& v1, const Value& v2);
        std::string mulI(const Value& v1, const Value& v2);
        std::string fmulI(const Value& v1, const Value& v2);
        std::string udivI(const Value& v1, const Value& v2);
        std::string sdivI(const Value& v1, const Value& v2);
        std::string fdivI(const Value& v1, const Value& v2);
        std::string uremI(const Value& v1, const Value& v2);
        std::string sremI(const Value& v1, const Value& v2);
        std::string fremI(const Value& v1, const Value& v2);
        std::string shlI(const Value& v1, const Value& v2);
        std::string lshrI(const Value& v1, const Value& v2);
        std::string ashrI(const Value& v1, const Value& v2);
        std::string andI(const Value& v1, const Value& v2);
        std::string orI(const Value& v1, const Value& v2);
        std::string xorI(const Value& v1, const Value& v2);
        std::string icmpI(std::string_view condition_code, const Value& v1, const Value& v2);
        std::string zextI(const Value& v, const LLVMType& ty);

        std::string phiI(const ValueLabelList& vl_list);

        // std::string allocaI(const LLVMType& ty, std::optional<std::size_t> alignment){}
        // std::string storeI(const Value& v){}

        std::string getIRstr();
    };

    class NcCodeGen: private LLVMHelper
    {
    private:
        NcAST m_ast{};

    public:
        NcCodeGen(NcAST&& ast);
        ~NcCodeGen() = default;

        void generate();

    private:
        void root_gen(){}
        void ncFile_gen(NcFile& ncf){}

        void funcDeclaration_gen(FuncDeclaration& fd){}
        
        void functionDef_gen(FunctionDef& f){}
        void functionDecl_gen(FunctionDecl& f){}

        void blockItem_gen(BlockItem& bi){}
        void block(Block& block){}
        
        void statement_gen(Statement& s){}
        
        void ifCondtion_gen(IfCondition& ifCondition){}
        void forLoop_gen(ForLoop& forLoop){}
        void whileLoop_gen(WhileLoop& whileLoop){}
        void doWhileLoop_gen(DoWhileLoop& doWhileLoop){}
        void jumpStatement_gen(JumpStatement& js){}

        void varDeclaration_gen(VarDeclaration& vd){}
        void varDecl_gen(VarDecl& vdec){}

        void expression_gen(Expression& e){}

        void uPrefixOperation_gen(UPrefixOperation& exp){}
        void uPostfixOperation_gen(UPostfixOperation& exp){}
        void binaryOperation_gen(BinaryOperation& exp){}
        void literal_gen(Literal& exp){}
        void identifier_gen(Identifier& exp){}
        void r_and_l_exp_gen(R_And_L_Exp& exp){}
        void anyTypeListExp_gen(AnyTypeListExp& exp){}
        void tempVarDecl_gen(TempVarDecl& exp){}
    };
} // namespace Nc


#endif