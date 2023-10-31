#ifndef NCCODEGEN_H
#define NCCODEGEN_H

#pragma once

#include <string_view>
#include <vector>

#include "stream/fileoutputstream.h"


namespace Nc
{
    class NcCodeGen
    {
    private:
        Streams::FileOutputStream<char> m_assemblyFile{std::ofstream{"assembly.s"}};

    public:
        NcCodeGen() = default;
        ~NcCodeGen() = default;

        void fcn(std::string_view fcn_name);
        void mov(std::vector<std::string_view> x);
        void ret();
        void neg(std::string_view operand);
        void _not(std::string_view operand);
        void comp(std::vector<std::string_view> x);
        void sete(std::string_view operand);

        void comment(std::string_view commment);
    };
} // namespace Nc


#endif