#ifndef NClOG_H
#define NClOG_H

#pragma once

#include <list>
#include <string>
#include <string_view>
#include <utility>
#include <algorithm>

#include "drawboxaroundtext.h"
#include "stream/stringoutputstream.h"

namespace Nc
{
    class NcLog: public Streams::StringOutputStream<char>
    {
    public:
        using BoxLineStyle = Myfcn::DrawBoxAroundText::Style;

    private:
        std::list<std::string> m_logEntries{};
        BoxLineStyle m_boxLineStyle{ BoxLineStyle::doublelined };

        using Streams::StringOutputStream<char>::get_str;
        using Streams::StringOutputStream<char>::empty_str;
        using Streams::StringOutputStream<char>::endTime_of_writeFcn;

    public:

        NcLog(): Streams::StringOutputStream<char>{ std::ostringstream{} }
        {}
        ~NcLog() {}

        void log()
        {
            m_logEntries.push_back(get_str());
            empty_str();
        }

        void changeBoxLineStyle(BoxLineStyle s)
        { m_boxLineStyle = s; }

        void outputLogEntries(Streams::OutputStream<char>& output)
        {
            const auto listSize = m_logEntries.size();
            const auto maxBoxSpace = maxLengthOfErorrInContainer();
            
            Myfcn::DrawBoxAroundText boxify{output};
            boxify.style(m_boxLineStyle);

            boxify.topLine(maxBoxSpace);

            for (std::size_t count{1}; auto &&i : m_logEntries)
            {
                if (count == listSize)
                boxify.division(maxBoxSpace);

                boxify.spacing(i, maxBoxSpace);

                ++count;
            }
            
            boxify.downLine(maxBoxSpace);
        }
        
        bool isLogEmpty()
        { return m_logEntries.empty(); }

    private:
        std::size_t maxLengthOfErorrInContainer()
        {
            std::size_t maxLength{};

            std::ranges::for_each(m_logEntries, [ &maxLength ](std::string_view i) { maxLength = std::max(maxLength, i.length()); });

            return maxLength;
        }
    };
} // namespace Nc



#endif
