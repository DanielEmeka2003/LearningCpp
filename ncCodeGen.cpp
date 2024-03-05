#include <cctype>
#include <format>
#include <type_traits>
#include <iostream>
#include <stdexcept>

#include "ncCodeGen.h"
#include "ncLexer.h"
#include "random.h"
#include "stream/stringinputstream.h"

namespace Nc
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    /*--------------------------------------------------------------LLVM::LLVMIdentifier-------------------------------------------------------------*/

    LLVMHelper::LLVMIdentifier::LLVMIdentifier(IdentifierKind identKind): m_identKind{identKind}
    {
        U8string ident = getRandomStr();

        if (m_identKind == local)
        ident.insert(ident.begin(), '%'_u8), m_llvmIdent.assign(ident);
        else if (m_identKind == global)
        {
            ident.insert(ident.begin(), '@'_u8), m_llvmIdent.assign(ident);

            if (ident != "main"_u8str)
            m_llvmIdent.append("..nc"_u8str);
        }
        else if (m_identKind == none)
        m_llvmIdent = "l_"_u8str + ident;
        else
        throw std::runtime_error{ "in function LLVMHelper::LLVMIdentifier::LLVMIdentifier: no match" };

        m_llvmIdent.insert(m_llvmIdent.begin(), '"'_u8);
        m_llvmIdent.insert(m_llvmIdent.end(), '"'_u8);
    }

    LLVMHelper::LLVMIdentifier::LLVMIdentifier(NcIdentifier ident, IdentifierKind identKind): m_identKind{identKind}
    {   
        if (m_identKind == local)
        ident.insert(ident.begin(), '%'_u8), m_llvmIdent.assign(ident);
        else if (m_identKind == global)
        {
            ident.insert(ident.begin(), '@'_u8), m_llvmIdent.assign(ident);

            if (ident != "main"_u8str)
            m_llvmIdent.append("..nc"_u8str);
        }
        else if (m_identKind == none)
        m_llvmIdent = "l_"_u8str + ident;
        else
        throw std::runtime_error{ "in function LLVMHelper::LLVMIdentifier::LLVMIdentifier: no match" };

        m_llvmIdent.insert(m_llvmIdent.begin(), '"'_u8);
        m_llvmIdent.insert(m_llvmIdent.end(), '"'_u8);
    }
    
    U8string_view LLVMHelper::LLVMIdentifier::get() const
    { return m_llvmIdent; }

    LLVMHelper::LLVMIdentifier& LLVMHelper::LLVMIdentifier::insertLocalMark()
    {
        if (m_identKind != none or m_llvmIdent.starts_with('%'_u8))
        return *this;

        m_llvmIdent.insert(m_llvmIdent.begin(), '%'_u8);
        return *this;
    }

    /*--------------------------------------------------------------LLVM::LLVMType------------------------------------------------------------------*/

    LLVMHelper::LLVMType::LLVMType(NcType ncT)
    { m_llvmType = mapToLLVMType(ncT); }

    U8string_view LLVMHelper::LLVMType::get() const
    { return m_llvmType; }

    U8string LLVMHelper::LLVMType::mapToLLVMType(NcType ncT)
    {
        if (ncT == NcLexer::li8 or ncT == NcLexer::li16 or ncT == NcLexer::li32 or ncT == NcLexer::li64 or ncT == NcLexer::li128)
        return ncT;

        if (ncT == NcLexer::lui8 or ncT == NcLexer::lui16 or ncT == NcLexer::lui32 or ncT == NcLexer::lui64 or ncT == NcLexer::lui128)
        return "i32"_u8str;
        else if (ncT == NcLexer::rInt8 or ncT == NcLexer::rInt8)
        return "i8"_u8str;
        else if (ncT == NcLexer::rBool or ncT == NcLexer::rBool)
        return "i1"_u8str;
        else if (ncT == ""_u8str)
        return "(undertermined type)"_u8str;
        
        
        throw std::runtime_error{ "in function Nc::LLVMHelper::mapToLLVMType: unreachable" };
        return ""_u8str;
    }


    /*--------------------------------------------------------------LLVM-----------------------------------------------------------------------------*/

    LLVMHelper::LLVMHelper()
    {
        m_stringBuffer.write_endl("target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"");
        m_stringBuffer.write_endl("target triple = \"x86_64-w64-windows-gnu\"");
        m_stringBuffer.write_endl("");   
    }

    void LLVMHelper::endInfo()
    {
        m_stringBuffer.writewl("", "");
        m_stringBuffer.write_endl("; MetaData section: this code was written by [Emeka Daniel] and he says thank you LLVM");
        m_stringBuffer.write_endl("!llvm.ident = !{!0}");
        m_stringBuffer.write_endl("!0 = !{!\"(built by Emeka Daniel) llvm version 17.0.5\"}");
    }
    
    void LLVMHelper::beginFuncdef(const LLVMType& giveType, const LLVMIdentifier& funcI, const ArgumentList& args)
    {
        m_stringBuffer.writews("define", "dso_local", giveType, funcI);
        
        m_stringBuffer.write('(');
        for (std::size_t count{1}; auto &&i : args)
        {
            if (i.second.has_value())
            m_stringBuffer.write(i.first, i.second.value());
            else
            m_stringBuffer.write(i.first);

            if (count != args.size())
            m_stringBuffer.writews(',');

            ++count;
        }
        m_stringBuffer.writewl(')', '{', "");

        insertLabel(LLVMIdentifier{"entry"_u8str, LLVMIdentifier::none});
    }
    void LLVMHelper::endFuncdef()
    {
        m_stringBuffer.write_endl('}');
        m_stringBuffer.write_endl("");
    }

    void LLVMHelper::defIdentifier(const LLVMIdentifier& i, std::string_view instruction)
    {
        m_stringBuffer.writewt("");
        m_stringBuffer.writews_endl(i, '=', instruction);
        m_value.v = i;
    }
    
    void LLVMHelper::assignValue(const Value& v)
    { m_value = v; }

    const LLVMHelper::Value& LLVMHelper::getValue() const
    { return m_value; }

    const LLVMHelper::LLVMIdentifier& LLVMHelper::getCurrentLabel() const
    { return m_currentLabel;  }

    void LLVMHelper::insertConst(const Constant& c, const LLVMType& t)
    { m_value = { c, t }; }
    
    void LLVMHelper::insertLabel(const LLVMIdentifier& label)
    {
        m_stringBuffer.write_endl("    ", label.get(), ':');
        m_currentLabel = label;
    }

    void LLVMHelper::insertNewLine(std::size_t n)
    {
        for (size_t i = 0; i < n; i++)
        m_stringBuffer.write_endl("");
    }

    void LLVMHelper::insertComment(std::string_view c)
    {
        m_stringBuffer.writews_endl(';', c);
    }

    void LLVMHelper::retI(const Value& v)
    {
        m_stringBuffer.writewt("");
        m_stringBuffer.writews("ret");

        std::visit([&](auto& item) { m_stringBuffer.writews_endl(v.type, item); }, v.v);
    }

    void LLVMHelper::brI(const LLVMIdentifier& label)
    {
        m_stringBuffer.writewt("");
        m_stringBuffer.writews_endl("br", "label", LLVMIdentifier{label}.insertLocalMark());
    }

    void LLVMHelper::brI(const Value& conditionV, const LLVMIdentifier& true_label, const LLVMIdentifier& false_label)
    {
        if (conditionV.type != LLVMType{NcLexer::rBool})
        throw std::invalid_argument{ "in function Nc::LLVMHelper::brI: parameter [condition] member, type is not a 1bit integer" };

        m_stringBuffer.writewt("");
        m_stringBuffer.writews("br");

        std::visit([&](auto& item) { m_stringBuffer.writews(conditionV.type, item, ','); }, conditionV.v);

        m_stringBuffer.writews_endl("label", LLVMIdentifier{true_label}.insertLocalMark().get(), ',', "label", LLVMIdentifier{false_label}.insertLocalMark());
    }


    std::string LLVMHelper::addI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::addI v1.type and v2.type are not equal" };

        sos.writews("add");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::faddI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::faddI v1.type and v2.type are not equal" };

        sos.writews("fadd");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::subI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::subI v1.type and v2.type are not equal" };

        sos.writews("sub");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::fsubI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::fsubI v1.type and v2.type are not equal" };

        sos.writews("fsub");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::mulI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::mulI v1.type and v2.type are not equal" };

        sos.writews("mul");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::fmulI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::fmulI v1.type and v2.type are not equal" };

        sos.writews("fmul");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::udivI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::udivI v1.type and v2.type are not equal" };

        sos.writews("udiv");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::sdivI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::sdivI v1.type and v2.type are not equal" };

        sos.writews("sdiv");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::fdivI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::fdivI v1.type and v2.type are not equal" };

        sos.writews("fdiv");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::uremI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::uremI v1.type and v2.type are not equal" };

        sos.writews("urem");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::sremI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::sremI v1.type and v2.type are not equal" };

        sos.writews("srem");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::fremI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::fremI v1.type and v2.type are not equal" };

        sos.writews("frem");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::shlI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::shlI v1.type and v2.type are not equal" };

        sos.writews("shl");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::lshrI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::lshrI v1.type and v2.type are not equal" };

        sos.writews("lshr");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::ashrI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::ashrI v1.type and v2.type are not equal" };

        sos.writews("ashr");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::andI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::andI v1.type and v2.type are not equal" };

        sos.writews("and");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::orI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::orI v1.type and v2.type are not equal" };

        sos.writews("or");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }
    
    std::string LLVMHelper::xorI(const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::xorI v1.type and v2.type are not equal" };

        sos.writews("xor");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = v1.type;

        return sos.get_str();
    }

    std::string LLVMHelper::icmpI(std::string_view condition_code, const Value& v1, const Value& v2)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        if (v1.type != v2.type)
        throw std::runtime_error{ "in function Nc::LLVMHelper::icmpI v1.type and v2.type are not equal" };

        sos.writews("icmp", condition_code, "");
        
        std::visit([&sos, &v1](auto& item){ sos.writews(v1.type, item); }, v1.v);
        sos.writews(',');
        std::visit([&sos](auto& item){ sos.write(item); }, v2.v);

        m_value.type = LLVMType{ NcLexer::rBool };

        return sos.get_str();
    }

    std::string LLVMHelper::zextI(const Value& v, const LLVMType& ty)
    {
        Streams::StringOutputStream sos{ std::ostringstream{} };

        sos.writews("zext", v.type, "");
        
        std::visit([&sos](auto& item){ sos.writews(item); }, v.v);
        sos.writews("to", ty);

        m_value.type = ty;

        return sos.get_str();
    }

    std::string LLVMHelper::phiI(const ValueLabelList& vl_list)
    {
        {
            LLVMType valueTy{};
            for (std::size_t count{0}; auto &&i : vl_list)
            {
                if (count != 0)
                {
                    if (i.first.type != valueTy)
                    throw std::runtime_error{ "in function Nc::LLVMHelper::phiI: one of the values in vl_list does not have the same type as the others" };
                }

                valueTy = i.first.type;
                ++count;
            }
        }

        Streams::StringOutputStream sos{ std::ostringstream{} };
        
        sos.writews("phiI", vl_list.front().first.type, "");

        for (std::size_t count{1}; auto&& i: vl_list)
        {
            sos.writews('[');
            std::visit([&sos](auto& item){ sos.writews(item); }, i.first.v);
            sos.writews(',', LLVMIdentifier{i.second}.insertLocalMark(), ']');

            if (count != vl_list.size())
            sos.writews(',');

            ++count;
        }

        return sos.get_str();
    }

    std::string LLVMHelper::getIRstr()
    { return m_stringBuffer.get_str(); }

    

    /*----------------------------------------------------------------------NcCodeGen-----------------------------------------------------------*/

    NcCodeGen::NcCodeGen(NcAST&& ast): m_ast{ std::move(ast) }{}

    void NcCodeGen::generate()
    {
        root_gen();
        std::cout << "\n\n" << LLVMHelper::getIRstr() << '\n';
    }


    void NcCodeGen::root_gen()
    {
        ncFile_gen(static_cast<Root&>(m_ast).getNcf());
        LLVMHelper::endInfo();
    }
    void NcCodeGen::ncFile_gen(NcFile& ncf)
    {
        // for (auto &&i : ncf.funcList)
        // {
        //     funcDeclaration_gen(*i);            
        // }
    }

    void NcCodeGen::funcDeclaration_gen(FuncDeclaration& fd)
    {
        std::visit([&]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, FunctionDef>)
            functionDef_gen(item);
            else if constexpr (std::is_same_v<T, FunctionDecl>)
            functionDecl_gen(item);
            
        }, fd.getFuncDeclaration());
    }

    void NcCodeGen::functionDef_gen(FunctionDef& f)
    {
        // beginFuncdef(LLVMType{f.type}, LLVMIdentifier{f.ident, LLVMHelper::LLVMIdentifier::global}, ArgumentList{});
        
        // for (auto &&i : f.blockItemList)
        // {
        //     BlockItem_gen(*i);
        // }
        
        // endFuncdef();
    }
    void NcCodeGen::functionDecl_gen(FunctionDecl& f){}

    void NcCodeGen::blockItem_gen(BlockItem& bi)
    {
        std::visit([&]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, Statement>)
            statement_gen(item);
            else if constexpr (std::is_same_v<T, VarDeclaration>)
            varDeclaration_gen(item);
            
        }, bi.getBlockItem());
    }

    void NcCodeGen::block(Block& block){}
    
    void NcCodeGen::statement_gen(Statement& s)
    {
        // std::visit([&]<typename T>(T& item)
        // {
        //     if constexpr (std::is_same_v<T, Give>)
        //     give_gen(item);
        //     else if constexpr (std::is_same_v<T, Expression>)
        //     Expression_gen(item);
            
        // }, s.statement);
    }

    void NcCodeGen::ifCondtion_gen(IfCondition& ifCondition){}
    void NcCodeGen::forLoop_gen(ForLoop& forLoop){}
    void NcCodeGen::whileLoop_gen(WhileLoop& whileLoop){}
    void NcCodeGen::doWhileLoop_gen(DoWhileLoop& doWhileLoop){}    
    void NcCodeGen::jumpStatement_gen(JumpStatement& js)
    {
        // Expression_gen(*g.exp);
        // auto value = LLVMHelper::getValue();
        // LLVMHelper::retI(value);
    }

    void NcCodeGen::varDeclaration_gen([[maybe_unused]] VarDeclaration& vd)
    {
        // VarDeclare_gen(vd.var_declare);
    }

    void NcCodeGen::varDecl_gen([[maybe_unused]] VarDecl& vdec){}

    void NcCodeGen::expression_gen(Expression& e)
    {
        std::visit([&]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, UPrefixOperation>)
            uPrefixOperation_gen(item);
            else if constexpr (std::is_same_v<T, UPostfixOperation>)
            uPostfixOperation_gen(item);
            else if constexpr (std::is_same_v<T, BinaryOperation>)
            binaryOperation_gen(item);
            else if constexpr (std::is_same_v<T, Literal>)
            literal_gen(item);
            else if constexpr (std::is_same_v<T, Identifier>)
            identifier_gen(item);
            else if constexpr (std::is_same_v<T, R_And_L_Exp>)
            r_and_l_exp_gen(item);
            else if constexpr (std::is_same_v<T, AnyTypeListExp>)
            anyTypeListExp_gen(item);
            else if constexpr (std::is_same_v<T, TempVarDecl>)
            tempVarDecl_gen(item);
        }, e.getExp());
    }

    void NcCodeGen::uPrefixOperation_gen(UPrefixOperation& upre)
    {
        // Expression_gen(*upre.right_exp);
        // auto expValue = LLVMHelper::getValue();

        // if (upre.op == NcLexer::sMinus)
        // {
        //     LLVMHelper::insertComment("two's complement begin");

        //     auto valueX = Value{ Constant{"0"}, expValue.type };
            
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::subI(valueX, expValue));

        //     LLVMHelper::insertComment("two's complement end");
        // }
        // else if (upre.op == NcLexer::sBitwiseNot or upre.op == NcLexer::rBitwiseNot)
        // {
        //     LLVMHelper::insertComment("bit-negation begin");

        //     auto valueX = Value{ Constant{"-1"}, expValue.type };
            
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::xorI(expValue, valueX));

        //     LLVMHelper::insertComment("bit-negation end");
        // }
        // else if (upre.op == NcLexer::sNot or upre.op == NcLexer::rNot)
        // {
        //     LLVMHelper::insertComment("logical-negation begin");

        //     auto valueX = Value{ Constant{"0"}, expValue.type };

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("eq", expValue, valueX));
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::zextI(LLVMHelper::getValue(), expValue.type));

        //     LLVMHelper::insertComment("logical-negation end");
        // }
        // else if (upre.op == NcLexer::rOdd)
        // {
        //     LLVMHelper::insertComment("odd-check begin");

        //     auto valueX = Value{ Constant{"1"}, expValue.type };

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::xorI(expValue, valueX));
        //     valueX.v = Constant{"0"};
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("eq", LLVMHelper::getValue(), valueX));
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::zextI(LLVMHelper::getValue(), expValue.type));

        //     LLVMHelper::insertComment("odd-check end");
        // }
        // else if (upre.op == NcLexer::sPlus)
        // {
        //     LLVMHelper::insertComment("plus begin");
        //     LLVMHelper::insertComment("plus end");
        // }
        // else if (upre.op == NcLexer::sIncrement)
        // {
        //     LLVMHelper::insertComment("pre-increment begin");
        //     LLVMHelper::insertComment("pre-increment end");
        // }
        // else if (upre.op == NcLexer::sDecrement)
        // {
        //     LLVMHelper::insertComment("pre-decrement begin");
        //     LLVMHelper::insertComment("pre-decrement end");
        // }
        // else
        // throw std::runtime_error{ "in function Nc::NcCodeGen::UPrefixOperation_gen: no match" };

    }

    void NcCodeGen::uPostfixOperation_gen([[maybe_unused]] UPostfixOperation& upost)
    {
        
    }

    void NcCodeGen::binaryOperation_gen([[maybe_unused]] BinaryOperation& b)
    {
        // Expression_gen(*b.left_exp);
        // auto left_expValue = LLVMHelper::getValue();

        // if (b.op != NcLexer::sAnd and b.op != NcLexer::rAnd and b.op != NcLexer::sOr and b.op != NcLexer::rOr)
        // Expression_gen(*b.right_exp);

        // auto right_expValue = LLVMHelper::getValue();
        
        // if (b.op == NcLexer::sMultiply)
        // {
        //     LLVMHelper::insertComment("multiplication begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::mulI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("multipication end");
        // }
        // else if (b.op == NcLexer::sDivide)
        // {
        //     LLVMHelper::insertComment("signed-division begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::sdivI(left_expValue, right_expValue));
        //     //figure out a solution to implement type checks so that udiv could be used instead of sdiv for unsigned integers

        //     LLVMHelper::insertComment("signed-division end");
        // }
        // else if (b.op == NcLexer::sModuluo)
        // {
        //     LLVMHelper::insertComment("signed-division-remainder begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::sremI(left_expValue, right_expValue));
        //     //figure out a solution to implement type checks so that udiv could be used instead of sdiv for unsigned integers

        //     LLVMHelper::insertComment("signed-division-remainder end");
        // }
        // else if (b.op == NcLexer::sPlus)
        // {
        //     LLVMHelper::insertComment("addition begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::addI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("addition end");
        // }
        // else if (b.op == NcLexer::sMinus)
        // {
        //     LLVMHelper::insertComment("subtraction begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::subI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("subtraction end");
        // }
        // else if (b.op == NcLexer::sBitwiseShiftLeft)
        // {
        //     LLVMHelper::insertComment("bit-left-shift begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::shlI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("bit-left-shift end");
        // }
        // else if (b.op == NcLexer::sBitwiseShiftRight)
        // {
        //     LLVMHelper::insertComment("bit-right-shift begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::ashrI(left_expValue, right_expValue));
        //     //figure out a solution to implement type checks so that udiv could be used instead of sdiv for unsigned integers

        //     LLVMHelper::insertComment("bit-right-shift end");
        // }
        // else if (b.op == NcLexer::sLessthan)
        // {
        //     LLVMHelper::insertComment("lessThan begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("slt", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("lessThan end");
        // }
        // else if (b.op == NcLexer::sGreaterthan)
        // {
        //     LLVMHelper::insertComment("greaterThan begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("sgt", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("greaterThan end");
        // }
        // else if (b.op == NcLexer::sLessthan_equalto)
        // {
        //     LLVMHelper::insertComment("lessThan_or_equal begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("sle", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("lessThan_or_equal end");
        // }
        // else if (b.op == NcLexer::sGreaterthan_equalto)
        // {
        //     LLVMHelper::insertComment("greaterThan_or_equal begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("sge", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("greaterThan_or_equal end");
        // }
        // else if (b.op == NcLexer::sEqual)
        // {
        //     LLVMHelper::insertComment("equal begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("eq", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("equal end");
        // }
        // else if (b.op == NcLexer::sNotEqual)
        // {
        //     LLVMHelper::insertComment("not-equal begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("ne", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("not-equal end");
        // }
        // else if (b.op == NcLexer::sBitwiseAnd or b.op == NcLexer::rBitwiseAnd)
        // {
        //     LLVMHelper::insertComment("bit-and begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::andI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("bit-and end");
        // }
        // else if (b.op == NcLexer::sBitwiseXor or b.op == NcLexer::rBitwiseXor)
        // {
        //     LLVMHelper::insertComment("bit-xor begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::xorI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("bit-xor end");
        // }
        // else if (b.op == NcLexer::sBitwiseOr or b.op == NcLexer::rBitwiseOr)
        // {
        //     LLVMHelper::insertComment("bit-or begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::orI(left_expValue, right_expValue));

        //     LLVMHelper::insertComment("bit-or end");
        // }
        // else if (b.op == NcLexer::sAnd or b.op == NcLexer::rAnd)
        // {
        //     LLVMHelper::insertComment("logical-and begin");

        //     LLVMIdentifier and2exp{ "and2exp", LLVMIdentifier::none };
        //     LLVMIdentifier postAnd{ "postAnd", LLVMIdentifier::none };
        //     LLVMIdentifier labelPredecessor{ LLVMHelper::getCurrentLabel() };
        //     Value value0{ Constant{"0"}, left_expValue.type };

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("eq", left_expValue, value0));
        //     LLVMHelper::brI(LLVMHelper::getValue(), postAnd, and2exp);

        //     LLVMHelper::insertLabel(and2exp);
        //     Expression_gen(*b.right_exp);
        //     right_expValue = LLVMHelper::getValue();

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("ne", right_expValue, value0));
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::zextI(LLVMHelper::getValue(), right_expValue.type));
        //     LLVMHelper::brI(postAnd);

        //     LLVMHelper::insertLabel(postAnd);
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, 
        //     LLVMHelper::phiI(ValueLabelList{ ValueLabelPair{value0, labelPredecessor}, ValueLabelPair{LLVMHelper::getValue(), and2exp} }));

        //     LLVMHelper::insertComment("logical-and end");
        // }
        // else if (b.op == NcLexer::sXor or b.op == NcLexer::rXor)
        // {
        //     LLVMHelper::insertComment("logical-xor begin");

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("ne", left_expValue, right_expValue));

        //     LLVMHelper::insertComment("logical-xor end");
        // }
        // else if (b.op == NcLexer::sOr or b.op == NcLexer::rOr)
        // {
        //     LLVMHelper::insertComment("logical-or begin");

        //     LLVMIdentifier or2exp{ "or2exp", LLVMIdentifier::none };
        //     LLVMIdentifier postOr{ "postor", LLVMIdentifier::none };
        //     LLVMIdentifier labelPredecessor{ LLVMHelper::getCurrentLabel() };
        //     Value value0{ Constant{"0"}, left_expValue.type };

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("ne", left_expValue, value0));
        //     LLVMHelper::brI(LLVMHelper::getValue(), postOr, or2exp);

        //     LLVMHelper::insertLabel(or2exp);
        //     Expression_gen(*b.right_exp);
        //     right_expValue = LLVMHelper::getValue();

        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::icmpI("ne", right_expValue, value0));
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local}, LLVMHelper::zextI(LLVMHelper::getValue(), right_expValue.type));
        //     LLVMHelper::brI(postOr);

        //     LLVMHelper::insertLabel(postOr);
        //     LLVMHelper::defIdentifier(LLVMIdentifier{LLVMIdentifier::local},
        //     LLVMHelper::phiI(ValueLabelList{ ValueLabelPair{Value{Constant{"1"}, value0.type}, labelPredecessor}, ValueLabelPair{LLVMHelper::getValue(), or2exp} }));


        //     LLVMHelper::insertComment("logical-or end");
        // }
        // else if (b.op == NcLexer::sAssign)
        // {

        // }
        // else if (b.op == NcLexer::sAdditionAssign)
        // {

        // }
    
    }

    void NcCodeGen::literal_gen(Literal& literal)
    {
        // LLVMHelper::insertConst(l.l, LLVMType{l.ltype});
    }

    void NcCodeGen::identifier_gen([[maybe_unused]] Identifier& ident){}
    void NcCodeGen::r_and_l_exp_gen(R_And_L_Exp& r_and_l_exp){}
    void NcCodeGen::anyTypeListExp_gen(AnyTypeListExp& anyTypeListExp){}
    void NcCodeGen::tempVarDecl_gen(TempVarDecl& tempVarDecl){}

} // namespace Nc
