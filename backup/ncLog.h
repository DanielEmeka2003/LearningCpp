#ifndef Log_H
#define Log_H

#pragma once

#include <list>
#include <vector>
#include <string>
#include <string_view>
#include <utility>
#include <algorithm>
#include <functional>

#include "stream/stringoutputstream.h"
#include "stream/output.h"
#include "misc_utility.h"

namespace Nc
{
    class Log: public Streams::StringOutputStream<char>
    {
    public:
        using BoxLineStyle = misc::DrawBoxAroundText::Style;

    private:
        using IgnoreTextAmount = std::uint32_t;

        std::vector<U8string> m_logEntries{};/* change to nc implementation of list data structure later */
        std::vector<IgnoreTextAmount> m_ignoreLogAmountList{};
        BoxLineStyle m_boxLineStyle{ BoxLineStyle::doublelined };
        std::size_t m_counter{1};
        std::size_t m_textSizeLimit{130};

        using Streams::StringOutputStream<char>::get_str;
        using Streams::StringOutputStream<char>::empty_str;
        using Streams::StringOutputStream<char>::endTime_of_writeFcn;

    public:
        Log(): Streams::StringOutputStream<char>{ std::ostringstream{} }{}

        void log(bool number = false)//move the strings stop copying them
        {
            auto log_as_str = convertAsciiStrToU8Str(get_str());
            std::uint32_t temp_ignoreLogAmount{};
            
            if (log_as_str.starts_with('\002'_u8))
            {
                auto r_paren_iterator = std::ranges::find(log_as_str, ')'_u8);
                temp_ignoreLogAmount = std::stoul(convertU8StrToAsciiStr(U8string_view{log_as_str.begin() + 2, r_paren_iterator}));
                log_as_str.erase(log_as_str.begin(), r_paren_iterator + 1);
            }

            auto slipt = [&]
            {
                std::vector<U8string> u8str_list{}; //change to nc_list implementation later
                std::size_t current_slipt_index{};
                auto saved_log_related_agnostic_last_element_index = m_ignoreLogAmountList.size() - 1; //m_logEntries can be used here too

                using position_pair = std::pair</*start_position*/std::uint32_t, /*new_end_position*/std::uint32_t>;
                // io::cout.writews_nl("log_size:", log_as_str.size());
                std::list<std::pair</*terminal_text*/U8string, position_pair>> specialTerminaTextStore{};
                //remove the special_text_text and store in another location
                {
                    /*auto count_seq_if = [&](U8string::iterator begin, U8string::iterator end, std::function<bool(U8char&)> lambda)
                    {
                        std::uint32_t count{};
                        for (auto i = begin; i != end; i++)
                            if (lambda(*i)) ++count; else break;
                        return count;
                    };*/

                    U8string::iterator escape_ch_loc{}, delimeter_loc{}/*delimeter is [m]*/;
                    
                    while (escape_ch_loc = std::ranges::find(log_as_str, '\033'_u8), escape_ch_loc != log_as_str.end())
                    {
                        delimeter_loc = std::find(escape_ch_loc, log_as_str.end(), 'm'_u8);
                        U8string terminal_text{escape_ch_loc + 2/*skip '['*/, delimeter_loc};

                        specialTerminaTextStore.push_back(
                            std::pair{
                                terminal_text,
                                position_pair{
                                    (std::uint32_t)std::distance(log_as_str.begin(), escape_ch_loc) + 1,//we want the position
                                    (std::uint32_t)std::distance(log_as_str.begin(), std::find(delimeter_loc + 1, log_as_str.end(), '\033'_u8)) + 1//we want the position
                                }
                            }
                        );
                        
                        log_as_str.erase(escape_ch_loc, delimeter_loc + 1);
                        log_as_str.erase(log_as_str.find('\033'_u8), 4);/*removal of the special_terminal_delimeter '\033[0m', different from the normal delimeter 'm'*/
                    }
                }
                // io::cout.writews_nl("log_size:", log_as_str.size(), "log:", log_as_str);
                auto determine_slipt_size = [&](std::size_t x){
                    auto temp = x / m_textSizeLimit;

                    if ((x % m_textSizeLimit) != 0) temp += 1;
                    return temp;
                };
                
                for (size_t i = 1, n = determine_slipt_size(log_as_str.size()); i <= n; i++)
                {
                    if (i != 1)
                    {
                        u8str_list.push_back(U8string{log_as_str, current_slipt_index, m_textSizeLimit});
                        current_slipt_index += m_textSizeLimit;
                    }
                    else if (i == n)
                    u8str_list.push_back(U8string{log_as_str, current_slipt_index});
                    else
                    {
                        u8str_list.push_back(U8string{log_as_str, current_slipt_index, m_textSizeLimit});
                        current_slipt_index += m_textSizeLimit;
                    }
                    
                    m_ignoreLogAmountList.push_back(0);
                }
                // for (auto &&i : u8str_list)
                // io::cout.write_nl(i);
                // io::cout.write('\n');
                U8string* current_str_ptr{};
                std::uint32_t* current_ignoreLogAmount_ptr{};

                for (auto slipt_number = 0u, new_start_position = 0u, new_end_position = 0u; auto &&i : specialTerminaTextStore)
                {
                    /*
                    * slipt_number - used to store the calculated number of where the slipt_text is in
                    * new_start_position - used to store the new start position of the now removed escape-sequence
                    * new_end_position - used to store the new end position of the now removed escape-sequence
                    * 
                    * Example:
                    * [0123456789] (slipt with a max_text_limit of 4) is
                    * [0123] [4567] [89]
                    * let's say i.second.first which stores the position of the escape sequence were to point at pos(7), to find the slipt
                    * number it is in would require doing a bit of calculation i.e.
                    * auto temp = pos(7) / max_text_limit(4) = 1
                    * and
                    * (pos(7) % max_text_limit(4)) != 0? temp += 1 : none;
                    * So, the slipt_number is (2), which is the slipt where pso(7) is located
                    * 
                    * Now to get the new position in the slipt, we still have to do some calculation i.e
                    * an offset of -(max_text_limit(4) * (slipt_number - 1)) is needed because the orginal text was slipt and now the pos(7)
                    * is invalid without correctly offsetting the value.
                    * eg:
                    *  1(0)  2(-4) 3(-8)
                    * [0123] [4567] [89]
                    * As seen from above elements position in slipt_number(1) need no offset because it's position is still valid,
                    * but elements position in other slipt_numbers need because they were slipt from the orginal and now their positions are
                    * invalid
                    */
                    
                    //start position insertion
                    {
                        slipt_number = determine_slipt_size(i.second.first);
                        new_start_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), i.second.first);

                        current_str_ptr = &u8str_list[slipt_number - 1];
                        current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[saved_log_related_agnostic_last_element_index + slipt_number];

                        /*-1 because index starts from 0*/
                        current_str_ptr->insert((new_start_position - 1) + *current_ignoreLogAmount_ptr, "\033["_u8str + i.first + 'm'_u8);
                    }

                    //end position insertion
                    {
                        auto current_slipt_number = slipt_number;
                        slipt_number = determine_slipt_size(i.second.second);
                        new_end_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), i.second.second);
                        
                        std::size_t iteration_count{1};
                        while (true)
                        {
                            /*
                            * Note: the end_position(i.second.second) is captured while the begin(i.second.first) terminal text still present in the
                            * orginal text, meaning the new_end_position relies on the new_start_position being inserted first, hence why the
                            * new_start_position is inserted before the new_end_position, this could be avoided if i make the end_position(i.second.second)
                            * independent of the start_position(i.second.first) but that would incure extra overhead that deserves to be avoided
                            */
                            if (iteration_count == 1)
                            {
                                /*Note: new_end_position is positively offset by the previous added terminal-special-text in a particular slipt*/
                                if (current_slipt_number == slipt_number)
                                {
                                    current_str_ptr->insert((new_end_position - 1) + *current_ignoreLogAmount_ptr, "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += (3 + i.first.size()) + 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += (3 + i.first.size()) + 4;
                                }
                            }
                            else
                            {
                                current_str_ptr->insert(0, "\033["_u8str + i.first + 'm'_u8);

                                if (current_slipt_number == slipt_number)
                                {   
                                    current_str_ptr->insert((new_end_position - 1) + *current_ignoreLogAmount_ptr, "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += (3 + i.first.size()) + 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += (3 + i.first.size()) + 4;
                                }
                            }
                            
                            ++iteration_count;
                            current_str_ptr = &u8str_list[++current_slipt_number - 1];
                            current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[saved_log_related_agnostic_last_element_index + current_slipt_number];
                        }
                    }
                }
                
                for (auto& i : u8str_list)
                {
                    for (auto&& j : i)
                    {
                        if (j == '\n'_u8)
                        io::cout.write("🌹");
                        else
                        io::cout.write(j);
                    }
                    io::cout.write('\n');
                }panic();

                return u8str_list;
            };

            if (!number)
            {
                if (log_as_str.size() - temp_ignoreLogAmount > m_textSizeLimit)
                {
                    auto u8str_list = slipt();
                    bool already_found_begin_arrow{};
                    std::size_t first_arrow_position{};
                    
                    for (auto &&i : u8str_list)
                    {
                        if (!already_found_begin_arrow and i.starts_with("                │\033[1m"_u8str))
                        {
                            first_arrow_position = m_logEntries.size();
                            already_found_begin_arrow = true;
                        }

                        m_logEntries.push_back(std::move(i));
                    }

                    /*
                    * To order the arrows to match the text they are pointing to
                    *0   line: 4 column: 1 - 281                                                                                                                  
                    *1   line contents│3.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
                    *2⤷  00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
                    *3⤷  000000000₃₀@+345`d32                                                                                                                     
                    *4                │⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧
                    *5   ⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧
                    *6   ⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧     
                    *
                    * 
                    *0   line: 4 column: 1 - 281                                                                                                                  
                    *1   line contents│3.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
                    *2                │⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧
                    *3⤷  00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
                    *4   ⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧
                    *5⤷  000000000₃₀@+345`d32                                                                                                                     
                    *6   ⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧     
                    */

                    auto shift_left = [&]<typename T>(std::vector<T>& ds, std::uint32_t old_shiftee_pos, std::uint32_t new_shiftee_pos)
                    {
                        if (old_shiftee_pos < new_shiftee_pos)
                        panic("in lambda function shift_left, arg old_shiftee_pos(", old_shiftee_pos, ") is < arg new_shiftee_pos(", new_shiftee_pos, ')');
                        else if (old_shiftee_pos == new_shiftee_pos)
                        return;

                        auto number_of_elements_to_displace = misc::safe_unsigned_sub(old_shiftee_pos, new_shiftee_pos);

                        for (auto i = 0uz; i < number_of_elements_to_displace; i++)
                        {
                            std::swap(ds[old_shiftee_pos], ds[misc::safe_unsigned_sub(old_shiftee_pos, 1)]);
                            old_shiftee_pos = misc::safe_unsigned_sub(old_shiftee_pos, 1);
                        }
                    };

                    if (already_found_begin_arrow)
                    {
                        std::size_t n_of_arrows{ m_logEntries.size() - first_arrow_position/* + 1 meant to be here, but 0 based indexing has it's perks*/ };
                        auto current_arrow_pos = first_arrow_position;
                        auto normalText_arrowMatchup_pos = misc::safe_unsigned_sub(current_arrow_pos, n_of_arrows);

                        for (auto i = 1uz; i <= n_of_arrows/*meant to be this(i <= n_arrows), but we're intentionally skipping shifting the last element*/; i++)
                        {
                            shift_left(m_logEntries, current_arrow_pos, normalText_arrowMatchup_pos + 1uz);
                            shift_left(m_ignoreLogAmountList, current_arrow_pos, normalText_arrowMatchup_pos + 1uz);

                            ++current_arrow_pos;
                            normalText_arrowMatchup_pos = misc::safe_unsigned_sub(current_arrow_pos, n_of_arrows) + i /*to make up for the shifting of the previous arrow to the current normalText position*/;
                        }
                    }
                }
                else
                m_ignoreLogAmountList.push_back(temp_ignoreLogAmount), m_logEntries.push_back(std::move(log_as_str));
            }
            else
            {
                /*pushing back a null-terminator to insert a space(demarkation) after numbered logs only if before m_counter > 1*/
                if (m_counter > 1)
                m_logEntries.push_back(U8string{'\0'_u8}), m_ignoreLogAmountList.push_back(0);
                
                log_as_str = convertAsciiStrToU8Str(std::to_string(m_counter).append({')', '.'})) + log_as_str;

                if (log_as_str.size() - temp_ignoreLogAmount > m_textSizeLimit)
                {
                    auto u8str_list = slipt();

                    for (auto &&i : u8str_list)
                    m_logEntries.push_back(std::move(i));
                    
                    //➥ ⤷    
                }
                else
                m_ignoreLogAmountList.push_back(temp_ignoreLogAmount), m_logEntries.push_back(std::move(log_as_str));

                ++m_counter;
            }
            
            empty_str();
        }

        void changeBoxLineStyle(BoxLineStyle s)
        { m_boxLineStyle = s; }

        void outputLogEntries(std::string LogHeading, Streams::OutputStream<char>& output)
        {
            //Footer insertion
            write("\002(14)⟨\033[40;31;1mcompilation terminated\033[0m⟩ (⌐■_■) ψ(._. )>"), log();

            //Heading insertion
            {
                --m_counter;
                write(LogHeading, '(', m_counter, " found)\033[0m");

                auto log_as_str = convertAsciiStrToU8Str(get_str());
                std::uint32_t temp_ignoreLogAmount{};

                if (log_as_str.starts_with('\002'_u8))
                {
                    auto r_paren_iterator = std::ranges::find(log_as_str, ')'_u8);
                    temp_ignoreLogAmount = std::stoul(convertU8StrToAsciiStr(U8string_view{log_as_str.begin() + 2, r_paren_iterator}));
                    log_as_str.erase(log_as_str.begin(), r_paren_iterator + 1);
                }

                m_logEntries.insert(m_logEntries.begin(), std::move(log_as_str));
                m_ignoreLogAmountList.insert(m_ignoreLogAmountList.begin(), temp_ignoreLogAmount);
            }

            const auto listSize = m_logEntries.size();
            auto maxBoxSpace = maxLengthOfErorrInContainer();
            std::uint32_t ignore_text_amount{};

            if (maxBoxSpace > m_textSizeLimit)
            maxBoxSpace = m_textSizeLimit;

            misc::DrawBoxAroundText boxify{output};
            boxify.style(m_boxLineStyle);

            boxify.topLine(maxBoxSpace);
               
            for (std::size_t count{1}; auto &&i : m_logEntries)
            {
                ignore_text_amount = m_ignoreLogAmountList[count - 1];
                if (count == 2 or count == listSize)
                boxify.division(maxBoxSpace), boxify.spacing(i, maxBoxSpace, i.size() - ignore_text_amount);
                else if (i.starts_with('\0'_u8))
                boxify.division(maxBoxSpace);
                else
                boxify.spacing(i, maxBoxSpace, i.size() - ignore_text_amount);

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

            std::ranges::for_each(m_logEntries, [ &maxLength ](const U8string& i) { maxLength = std::max(maxLength, i.length()); });

            return maxLength;
        }
    };
} // namespace Nc

#endif