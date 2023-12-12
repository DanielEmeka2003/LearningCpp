#ifndef NCCODEGEN_H
#define NCCODEGEN_H

#pragma once

#include <string_view>
#include <optional>
#include <map>

#include "stream/stringoutputstream.h"
#include "ncAST.h"

namespace Nc
{
    class LLVMHelper
    {
    protected:
        using Constant = std::string;

        class LLVMIdentifier
        {
        public:
            enum IdentifierKind { local, global, none };
            using Counter = unsigned int;
            using NcIdentifier = std::string;

        private:
            std::string m_llvmIdent{};
            IdentifierKind m_identKind{};
            static inline Counter m_counter{};
            static inline std::map<std::string, IdentifierKind> m_tracker{};
            
            LLVMIdentifier() = default;

        public:
            LLVMIdentifier(IdentifierKind identKind);
            LLVMIdentifier(NcIdentifier ident, IdentifierKind identKind) ;

            /*gets the transformed NcIdentifier[using NcIdentifier = std::string],
            which is now in LLVM format i.e. LLVMIdentifier*/
            std::string_view get() const;
            /*dosen't actually mangles[distorts the Identifier] the NcIdentifier which has been transformed to an LLVMIdentifier,
            but simply appends string "_$" to the LLVMIdentifier*/
            LLVMIdentifier& mangle();
            /*inserts the local identifier mark(%) to the begining of an identifier that has none as it's identifierKind
            [Note] if the local identifier mark(%) is already present, no operation would be done*/
            LLVMIdentifier& insertLocalMark();

            //clears tracker related static variables, to be used at the end of each function
            static void clearTracker();
            //generates random strings
            static std::string getRandomStr(std::size_t str_length = 4);

        private:
            friend std::ostream& operator<<(std::ostream& os, const LLVMIdentifier& d) { return os << d.m_llvmIdent; }
            friend class LLVMHelper;
        };

        class LLVMType
        {
        public:
            using NcType = std::string_view;
            
        private:
            std::string m_llvmType{};
            
            LLVMType() = default;

        public:
            LLVMType(NcType ncT);

            std::string_view get() const;
            static std::string mapToLLVMType(NcType ncT);

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

        std::string phi(const ValueLabelList& vl_list);

        std::string getIRstr();

    private:
        
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
        void Root_gen();
        void NcFile_gen(NcFile& ncf);

        void FuncDeclaration_gen(FuncDeclaration& fd);
        
        void Function_gen(Function& f);

        void BlockItem_gen(BlockItem& bi);
        
        void Statement_gen(Statement& s);
        
        void Give_gen(Give& g);

        void Expression_gen(Expression& e);

        void UPrefixOperation_gen(UPrefixOperation& upre);
        void UPostfixOperation_gen(UPostfixOperation& upost);
        void BinaryOperation_gen(BinaryOperation& b);
        void Literal_gen(Literal& l);
        void Variable_gen(Variable& v);
        void Type_gen(Type& t);

        void VarDeclaration_gen(VarDeclaration& vd);

        void VarDeclare_gen(VarDeclare& vdec);
    };
} // namespace Nc


#endif