#ifndef Log_H
#define Log_H

#pragma once

#include <list>
#include <string>
#include <string_view>
#include <utility>
#include <algorithm>

#include "drawboxaroundtext.h"
#include "stream/stringoutputstream.h"
#include "stream/output.h"
#include "myfunctions1.h"

namespace Nc
{
    class Log: public Streams::StringOutputStream<char>
    {
    public:
        using BoxLineStyle = Myfcn::DrawBoxAroundText::Style;

    private:
        std::list<Myfcn::U8string> m_logEntries{};
        BoxLineStyle m_boxLineStyle{ BoxLineStyle::doublelined };
        std::size_t m_counter{1};

        using Streams::StringOutputStream<char>::get_str;
        using Streams::StringOutputStream<char>::empty_str;
        using Streams::StringOutputStream<char>::endTime_of_writeFcn;

    public:
        Log(): Streams::StringOutputStream<char>{ std::ostringstream{} }{}

        void log(bool number = false)
        {
            using namespace Myfcn::u8literals;

            if (!number)
            {
                m_logEntries.push_back(Myfcn::convertAsciiStrToU8Str(get_str()));
            }
            else
            {
                using namespace Myfcn;

                if (m_counter > 1)
                m_logEntries.push_back(""_u8str); //pushing back a space before n > 1 numbered log starts

                m_logEntries.push_back( "("_u8str + Myfcn::convertAsciiStrToU8Str(std::to_string(m_counter).append({')', '.'}) + get_str()) );
                ++m_counter;
            }
            
            empty_str();
        }

        void changeBoxLineStyle(BoxLineStyle s)
        { m_boxLineStyle = s; }

        void outputLogEntries(std::string LogHeading, Streams::OutputStream<char>& output)
        {
            using namespace Myfcn::u8literals;

            //for new line after the topic
            m_logEntries.push_front(""_u8str);
            --m_counter;
            writews(LogHeading, ':', m_counter, m_counter == 1? "error found" : "errors found");
            m_logEntries.push_front(Myfcn::convertAsciiStrToU8Str(get_str()));

            const auto listSize = m_logEntries.size();
            const auto maxBoxSpace = maxLengthOfErorrInContainer() + 1;
            
            Myfcn::DrawBoxAroundText boxify{output};
            boxify.style(m_boxLineStyle);

            boxify.topLine(maxBoxSpace);

            for (std::size_t count{1}; auto &&i : m_logEntries)
            {
                //if the count is equal to either the last element or the second
                if (count == listSize or count == 2)
                boxify.division(maxBoxSpace);

                boxify.spacing(i, maxBoxSpace);

                ++count;
            }
            
            boxify.downLine(maxBoxSpace);
        }
        
        bool isLogEmpty()
        { return m_logEntries.empty(); }

        auto& popBack_logEntry(std::size_t howMany)
        {
            if (howMany >= m_logEntries.size())
            return *this;

            for (auto i = 0uz; i < howMany; ++i)
            m_logEntries.pop_back();

            return *this;
        }
        
        auto& getLogCounter(){ return m_counter; }

    private:
        std::size_t maxLengthOfErorrInContainer()
        {
            std::size_t maxLength{};

            std::ranges::for_each(m_logEntries, [ &maxLength ](const Myfcn::U8string& i) { maxLength = std::max(maxLength, i.length()); });

            return maxLength;
        }
    };
} // namespace Nc

#endif