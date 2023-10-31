#include <cctype>

#include "ncCodeGen.h"

namespace Nc
{
    void NcCodeGen::fcn(std::string_view x)
    {
        m_assemblyFile.write_endl(".globl _", x);
        m_assemblyFile.write_endl('_', x, ':');
    }

    void NcCodeGen::mov(std::vector<std::string_view> x)
    {
        m_assemblyFile.writewt("movl");

        for (auto &&i : x)
        {
            m_assemblyFile.write(i);
        }

        m_assemblyFile.write_endl("");
    }

    void NcCodeGen::ret()
    {
        m_assemblyFile.write_endl("ret");
    }

    void NcCodeGen::neg(std::string_view operand)
    {
        m_assemblyFile.writewt_endl("neg", operand);   
    }

    void NcCodeGen::_not(std::string_view operand)
    {
        m_assemblyFile.writewt_endl("not", operand);   
    }

    void NcCodeGen::comp(std::vector<std::string_view> x)
    {
        m_assemblyFile.writewt("compl");

        for (auto &&i : x)
        {
            m_assemblyFile.write(i);
        }

        m_assemblyFile.write_endl("");
    }

    void NcCodeGen::sete(std::string_view operand)
    {
        m_assemblyFile.writewt_endl("sete", operand);   
    }


    void NcCodeGen::comment(std::string_view commment)
    {
        m_assemblyFile.write('<', comment, '>');
    }

} // namespace Nc
