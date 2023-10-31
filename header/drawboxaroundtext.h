#ifndef DRAWBOXAROUNDTEXT_H
#define DRAWBOXAROUNDTEXT_H

#pragma once

#include <utility>
#include <string_view>

#include <stream/outputstream.h>

namespace Myfcn
{
    class DrawBoxAroundText
    {
    public:
        enum class Style{plain, round, bold, doublelined};
    private:
        Streams::OutputStream<char>& m_out;
        Style m_s{};

    public:
        DrawBoxAroundText(Streams::OutputStream<char>& out);
        ~DrawBoxAroundText() = default;
        
        void style(Style s);
        void topLine(size_t lineAmount);
        void downLine(size_t lineAmount);
        void spacing(std::string_view text, size_t spaceAmount, size_t optional_TextSize = {});
        void division(size_t lineAmount);
    };
} // namespace Myfcn


#endif