#ifndef DRAWBOXAROUNDTEXT_H
#define DRAWBOXAROUNDTEXT_H

#pragma once

#include <utility>
#include <string_view>

#include "stream/outputstream.h"

#include "myfunctions1.h"

namespace Myfcn
{
    class DrawBoxAroundText
    {
    public:
        enum class Style{none, plain, round, bold, doublelined};

    private:
        Streams::OutputStream<char>& m_out;
        Style m_s{};

    public:
        ~DrawBoxAroundText() = default;
        DrawBoxAroundText(Streams::OutputStream<char>& out): m_out{out}
        {}
        
        void style(Style s)
        { m_s = s; }

        void topLine(size_t lineAmount)
        {
            switch (m_s)
            {
            case Style::none:
                break;

            case Style::plain:
                m_out.write("\u250c");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");
                
                m_out.write_endl("\u2510");
                break;

            case Style::round:
                m_out.write("\u256d");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");
                
                m_out.write_endl("\u256e");
                break;

            case Style::bold:
                m_out.write("\u250f");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2501");
                
                m_out.write_endl("\u2513");
                break;

            case Style::doublelined:
                m_out.write("\u2554");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2550");
                
                m_out.write_endl("\u2557");
                break;
                
            default:
                throw std::invalid_argument{"value given to function style is not valid in DrawBoxAroundText Class"};
                break;
            }
        }

        void downLine(size_t lineAmount)
        {
            switch (m_s)
            {
            case Style::none:
                
                break;
            case Style::plain:
                m_out.write("\u2514");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");
                
                m_out.write_endl("\u2518");
                break;

            case Style::round:
                m_out.write("\u2570");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");
                
                m_out.write_endl("\u256f");
                break;

            case Style::bold:
                m_out.write("\u2517");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2501");
                
                m_out.write_endl("\u251b");
                break;

            case Style::doublelined:
                m_out.write("\u255a");

                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2550");
                
                m_out.write_endl("\u255d");
                break;

            default:
                throw std::invalid_argument{"value given to function style is not valid in DrawBoxAroundText Class"};
                break;
            }
        }

        void spacing(std::string_view text, size_t spaceAmount, size_t optional_TextSize = {})
        {
            auto size{optional_TextSize == 0? text.size() : optional_TextSize};
            size = (size > spaceAmount? size - spaceAmount : spaceAmount - size);

            switch (m_s)
            {
            case Style::none:
                m_out.write_endl(text);
                break;

            case Style::plain:
                m_out.write("\u2502", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2502");
                break;

            case Style::round:
                m_out.write("\u2502", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2502");
                break;

            case Style::bold:
                m_out.write("\u2503", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2503");
                break;

            case Style::doublelined:
                m_out.write("\u2551", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2551");
                break;

            default:
                throw std::invalid_argument{"value given to function style is not valid in DrawBoxAroundText Class"};
                break;
            }
        }

        void spacing(const U8string& text, size_t spaceAmount)
        {
            auto size{text.size() > spaceAmount? text.size() - spaceAmount : spaceAmount - text.size()};
            
            switch (m_s)
            {
            case Style::none:
                m_out.write_endl(text);
                break;

            case Style::plain:
                m_out.write("\u2502", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2502");
                break;

            case Style::round:
                m_out.write("\u2502", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2502");
                break;

            case Style::bold:
                m_out.write("\u2503", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2503");
                break;

            case Style::doublelined:
                m_out.write("\u2551", text);

                for (size_t i = 0; i < size; i++)
                m_out.write(' ');
                
                m_out.write_endl("\u2551");
                break;

            default:
                throw std::invalid_argument{"value given to function style is not valid in DrawBoxAroundText Class"};
                break;
            }
        }

        void division(size_t lineAmount)
        {
            switch (m_s)
            {
            case Style::none:
                
                break;
            case Style::plain:
                m_out.write("\u251c");
                
                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");

                m_out.write_endl("\u2524");
                break;
            case Style::round:
                m_out.write("\u251c");
                
                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2500");

                m_out.write_endl("\u2524");
                break;
            case Style::bold:
                m_out.write("\u2523");
                
                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2501");

                m_out.write_endl("\u252b");
                break;
            case Style::doublelined:
                m_out.write("\u2560");
                
                for (size_t i = 0; i < lineAmount; i++)
                m_out.write("\u2550");

                m_out.write_endl("\u2563");
                break;
            default:
                throw std::invalid_argument{"value given to function style is not valid in DrawBoxAroundText Class"};
                break;
            }
        }
    };
} // namespace Myfcn


#endif