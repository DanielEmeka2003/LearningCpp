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
#include "u8char.h"
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
            auto log_as_str = u8_misc::convertAsciiStrToU8Str(get_str());
            std::uint32_t temp_ignoreLogAmount{};

            auto find_from = [&](U8string& ds, std::size_t index_to_start_from, const U8char& ch_to_find)
            {
                for (size_t i = index_to_start_from, n = ds.size(); i < n; i++)
                {
                    if (ds[i] == ch_to_find)
                    {
                        return i;
                        break;
                    }   
                }
                return U8string::npos;
            };

            auto count_seq_if = [&](U8string& ds, std::size_t index_to_start_from, std::function<bool(U8char&)> lambda)
            {
                std::uint32_t count{};
                for (auto i = index_to_start_from, n = ds.size(); i < n; i++)
                if (lambda(ds[i])) ++count; else break;

                return count;
            };

            //both conditional are for capturing the amount of special-terminal-text in a log
            if (log_as_str.starts_with('\002'_u8))//captures statically
            {
                auto r_paren_iterator = std::ranges::find(log_as_str, ')'_u8);
                temp_ignoreLogAmount = std::stoul(u8_misc::convertU8StrToAsciiStr(U8string_view{log_as_str.begin() + 2, r_paren_iterator}));
                log_as_str.erase(log_as_str.begin(), r_paren_iterator + 1);
            }
            else if (log_as_str.contains('\033'_u8))//captures dynamically
            {
                for (auto i = 0uz, n = log_as_str.size(), specialTextEnd = 0uz; i < n; ++i)//include the \033 and m
                {
                    if (log_as_str[i] == '\033'_u8)
                    {
                        specialTextEnd = find_from(log_as_str, i+3/*just to skip a few characters*/, '\033'_u8);
                        
                        temp_ignoreLogAmount += count_seq_if(log_as_str, i, [&](U8char& x){ return x != 'm'_u8; }) + 1/*[m]*/ + 4/*[\033[0m]*/,
                        i = specialTextEnd; //jump ahead
                    }
                }
            }

            auto slipt = [&]
            {
                std::vector<U8string> u8str_list{}; //change to nc_list implementation later
                std::size_t current_slipt_index{};
                auto ignoreLogAmountList_offset = m_ignoreLogAmountList.size() - 1; //m_logEntries can be used here too

                using position_pair = std::pair</*start_position*/std::uint32_t, /*new_end_position*/std::uint32_t>;
                
                std::list<std::pair</*terminal_text*/U8string, position_pair>> specialTerminalTextStore{};

                //remove the special-terminal-text and store in another location
                {
                    if (log_as_str.contains('\033'_u8))
                    {
                        std::list<std::size_t> delimeter_loc_list{};

                        for (auto i = 0uz, n = log_as_str.size(), specialTextEnd = 0uz; i < n; ++i)//include the \033 and m
                        {
                            if (log_as_str[i] == '\033'_u8)
                            {
                                delimeter_loc_list.push_back(find_from(log_as_str, i, 'm'_u8));
                                U8string terminal_text{log_as_str.begin() + i, log_as_str.begin() + delimeter_loc_list.back() + 1};
                                specialTextEnd = find_from(log_as_str, delimeter_loc_list.back(), '\033'_u8);

                                specialTerminalTextStore.push_back(
                                    std::pair{
                                        terminal_text,
                                        position_pair{
                                            (std::uint32_t)i + 1,//i want the position
                                            (std::uint32_t)specialTextEnd + 1//i want the position
                                        }
                                    }
                                );

                                i = specialTextEnd; //jump ahead
                            }
                        }

                        //removal of the specialTerminal texts from the string
                        auto dM = delimeter_loc_list.rbegin();
                        for (auto sT = specialTerminalTextStore.rbegin(); sT != specialTerminalTextStore.rend(); ++sT, ++dM)
                        {
                            log_as_str.erase(sT->second.second - 1, 4); //remove the special-terminal-delimeter first
                            log_as_str.erase((sT->second.first - 1), (*dM + 2) - sT->second.first);
                        }
                    }
                }
                
                auto determine_slipt_pos = [&](std::size_t pos){
                    /*pos - 1 is for adjusting for possible outliers
                    * Outliers are specialTerminalText delimeters that have a position just one greater than the end of a particular slipt_pos
                    * example with the maxTextSizeLimit being (130):
                    * - 131 is an outlier for the first slipt, it calculated slipt is the second
                    * - 261 is an outlier for the second slipt, it's calculated slipt is the third
                    * e.t.c
                    * 
                    * Outliers are only dangerous when they are the last position e.g with a textSize of 260 after removal of the specialTerminalText
                    * and a position for the delimeter just happens to be 261, it would result in a slipt of 3, while only 2 slipts are avialiable
                    * 260/130 = 2, apart from that, they still cause a certain overhead by inserting needless specialTerminalText, so with that
                    * notion, i have decided to reduce the positions by one so that outliers when calculated would be in a slipt before it's
                    * calculated slipt.
                    * With that in mind, there is the problem of zero(0), which dosen't result in any slipt position, so one is returned when
                    * that case is encoutered
                    */

                    auto pos_minus_one = pos - 1;
                    if (pos_minus_one != 0)
                    {
                        auto temp = pos_minus_one / m_textSizeLimit;

                        if (pos_minus_one % m_textSizeLimit != 0)
                        temp += 1;
                        
                        return temp;
                    }
                    else
                    return 1uz;
                };
                
                for (size_t i = 1, n = determine_slipt_pos(log_as_str.size()); i <= n; i++)
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
                
                U8string* current_str_ptr{};
                std::uint32_t* current_ignoreLogAmount_ptr{};
                std::uint32_t accumulated_ignoreLogAmount{};

                /*...............................................DoneFromTheEnd....................................................*/
                /*for (auto &&i : specialTerminalTextStore)
                accumulated_ignoreLogAmount += i.first.size() + 4;
                
                auto slipt_number = 0u, new_start_position = 0u, new_end_position = 0u;
                for (auto i = specialTerminalTextStore.rbegin(); i != specialTerminalTextStore.rend(); ++i)
                {
                    accumulated_ignoreLogAmount -= 4;
                    auto pure_old_endPos = misc::safe_unsigned_sub(i->second.second, accumulated_ignoreLogAmount);

                    accumulated_ignoreLogAmount -= i->first.size();
                    auto pure_old_startPos = misc::safe_unsigned_sub(i->second.first, accumulated_ignoreLogAmount);

                    //end position insertion
                    {
                        //holding the slipt number of the end_position
                        auto current_slipt_number = slipt_number = determine_slipt_pos(pure_old_endPos);
                        
                        current_str_ptr = &u8str_list[slipt_number - 1];
                        current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[ignoreLogAmountList_offset + slipt_number];

                        new_end_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), pure_old_endPos);

                        slipt_number = determine_slipt_pos(pure_old_startPos);
                        
                        std::size_t iteration_count{1};
                        while (true)
                        {
                            if (iteration_count == 1)
                            {
                                if (current_slipt_number == slipt_number)
                                {
                                    current_str_ptr->insert(new_end_position - 1, "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(new_end_position - 1, "\033[0m"_u8str);
                                    current_str_ptr->insert(0, i->first);

                                    *current_ignoreLogAmount_ptr += i->first.size() + 4;
                                }
                            }
                            else
                            {
                                if (current_slipt_number == slipt_number)
                                {   
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    current_str_ptr->insert(0, i->first);

                                    *current_ignoreLogAmount_ptr += i->first.size() + 4;
                                }
                            }
                            
                            ++iteration_count;
                            current_str_ptr = &u8str_list[--current_slipt_number - 1];
                            current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[ignoreLogAmountList_offset + current_slipt_number];                            
                        }
                    }

                    //start position insertion
                    {
                        current_str_ptr = &u8str_list[slipt_number - 1];
                        current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[ignoreLogAmountList_offset + slipt_number];

                        new_start_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), pure_old_startPos);

                        current_str_ptr->insert((new_start_position - 1), i->first);

                        *current_ignoreLogAmount_ptr += i->first.size();
                    }
                }
                */

                /*..............................................DoneFromTheStart...................................................*/
                for (auto slipt_number = 0u, new_start_position = 0u, new_end_position = 0u; auto &&i : specialTerminalTextStore)
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
                        auto pure_old_startPos = misc::safe_unsigned_sub(/*impure_oldPos*/ i.second.first, /*amount_of_impurities to remove*/ accumulated_ignoreLogAmount);

                        slipt_number = determine_slipt_pos(pure_old_startPos);

                        current_str_ptr = &u8str_list[slipt_number - 1];
                        current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[ignoreLogAmountList_offset + slipt_number];

                        new_start_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), pure_old_startPos);

                        /*-1 because index starts from 0*/
                        current_str_ptr->insert((new_start_position - 1) + *current_ignoreLogAmount_ptr, i.first);

                        *current_ignoreLogAmount_ptr += i.first.size();
                        accumulated_ignoreLogAmount += i.first.size();
                    }

                    //end position insertion
                    {
                        auto pure_old_endPos = misc::safe_unsigned_sub(/*impure_oldPos*/ i.second.second, /*amount_of_impurities_to_remove*/ accumulated_ignoreLogAmount);
                    
                        auto current_slipt_number = slipt_number;
                        slipt_number = determine_slipt_pos(pure_old_endPos);
                        
                        new_end_position = misc::safe_unsigned_sub((m_textSizeLimit * (slipt_number - 1)), pure_old_endPos);
                        
                        std::size_t iteration_count{1};
                        while (true)
                        {
                            if (iteration_count == 1)
                            {
                                /*Note: new_end_position is positively offset by the previous added terminal-special-text in a particular slipt*/
                                if (current_slipt_number == slipt_number)
                                {
                                    current_str_ptr->insert((new_end_position - 1) + *current_ignoreLogAmount_ptr, "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                }
                            }
                            else
                            {
                                current_str_ptr->insert(0, i.first);
                                *current_ignoreLogAmount_ptr += i.first.size();

                                if (current_slipt_number == slipt_number)
                                {   
                                    current_str_ptr->insert((new_end_position - 1) + *current_ignoreLogAmount_ptr, "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                    break;
                                }
                                else
                                {
                                    current_str_ptr->insert(current_str_ptr->size(), "\033[0m"_u8str);
                                    *current_ignoreLogAmount_ptr += 4;
                                }
                            }
                            
                            ++iteration_count;
                            current_str_ptr = &u8str_list[++current_slipt_number - 1];
                            current_ignoreLogAmount_ptr = &m_ignoreLogAmountList[ignoreLogAmountList_offset + current_slipt_number];                            
                        }
                    }

                    accumulated_ignoreLogAmount += 4;
                }
                
                return u8str_list;
            };

            if (!number)
            {
                bool already_found_begin_arrow{};
                std::size_t first_arrow_position{};

                //To be used when sorting arrow position up
                static std::size_t logEntry_start_insert_pos_for_slipt{};
                //pretty self-explanatory
                static bool is_previous_logEntry_slipt = false;
                //pretty self-explanatory
                bool was_previous_logEntry_slipt = is_previous_logEntry_slipt;
                //pretty self-explanatory
                std::size_t previous_logEntry_start_insert_pos_for_slipt = logEntry_start_insert_pos_for_slipt;

                if (log_as_str.size() - temp_ignoreLogAmount > m_textSizeLimit)
                {
                    auto u8str_list = slipt();
                    
                    for (auto &&i : u8str_list)
                    {
                        if (!already_found_begin_arrow and i.starts_with("                │\033[1m"_u8str))
                        {
                            first_arrow_position = m_logEntries.size()+1;
                            already_found_begin_arrow = true;
                        }

                        m_logEntries.push_back(std::move(i));
                    }
                    
                    logEntry_start_insert_pos_for_slipt = m_logEntries.size() - u8str_list.size() + 1, is_previous_logEntry_slipt = true;
                }
                else
                m_ignoreLogAmountList.push_back(temp_ignoreLogAmount), m_logEntries.push_back(std::move(log_as_str)), is_previous_logEntry_slipt = false;

                //fix up the arrow situation
                {
                    if (!already_found_begin_arrow and was_previous_logEntry_slipt and m_logEntries.back().starts_with("                │\033[1m"_u8str))
                    {
                        first_arrow_position = m_logEntries.size();
                        already_found_begin_arrow = true;
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

                    auto shift_left = [&]<typename T>(std::vector<T>& ds, std::uint32_t old_shiftee_index, std::uint32_t new_shiftee_index)
                    {
                        if (old_shiftee_index < new_shiftee_index)
                        panic("in lambda function shift_left, arg old_shiftee_index(", old_shiftee_index, ") is < arg new_shiftee_index(", new_shiftee_index, ')');
                        else if (old_shiftee_index == new_shiftee_index)
                        return;

                        auto number_of_elements_to_displace = misc::safe_unsigned_sub(old_shiftee_index, new_shiftee_index);

                        for (auto i = 0uz; i < number_of_elements_to_displace; i++)
                        {
                            std::swap(ds[old_shiftee_index], ds[misc::safe_unsigned_sub(old_shiftee_index, 1)]);
                            old_shiftee_index = misc::safe_unsigned_sub(old_shiftee_index, 1);
                        }
                    };

                    if (already_found_begin_arrow)
                    {
                        std::size_t n_of_arrows{ m_logEntries.size() - (first_arrow_position - 1) };
                        auto current_arrow_index = first_arrow_position - 1;
                        auto normalText_arrowMatchup_index = previous_logEntry_start_insert_pos_for_slipt - 1;

                        for (auto i = 1uz; i <= n_of_arrows/*meant to be this(i <= n_arrows), but we're intentionally skipping shifting the last element*/; i++)
                        {
                            shift_left(m_logEntries, current_arrow_index, ++normalText_arrowMatchup_index);
                            shift_left(m_ignoreLogAmountList, current_arrow_index, normalText_arrowMatchup_index);

                            ++current_arrow_index, ++normalText_arrowMatchup_index;
                        }
                    }
                }
            }
            else
            {
                /*pushing back a null-terminator to insert a space(demarkation) after numbered logs only if before m_counter > 1*/
                if (m_counter > 1)
                m_logEntries.push_back(U8string{'\0'_u8}), m_ignoreLogAmountList.push_back(0);
                
                log_as_str = u8_misc::convertAsciiStrToU8Str(std::to_string(m_counter).append({')', '.'})) + log_as_str;

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
            write("\002(10)\033[5;1mCompilation terminated (⌐■_■) ψ(._. )>\033[0m"), log();

            //Heading insertion
            {
                --m_counter;
                write(LogHeading);

                auto log_as_str = u8_misc::convertAsciiStrToU8Str(get_str());
                std::uint32_t temp_ignoreLogAmount{};

                if (log_as_str.starts_with('\002'_u8))
                {
                    auto r_paren_iterator = std::ranges::find(log_as_str, ')'_u8);
                    temp_ignoreLogAmount = std::stoul(u8_misc::convertU8StrToAsciiStr(U8string_view{log_as_str.begin() + 2, r_paren_iterator}));
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

            // std::ranges::for_each(m_logEntries, [ &maxLength ](const U8string& i) { maxLength = std::max(maxLength, i.length()); });
            
            for (size_t i = 0, n = m_logEntries.size(), current_size{}; i < n; i++)
            {
                current_size = m_logEntries[i].size() - m_ignoreLogAmountList[i];

                if (maxLength < current_size)
                maxLength = current_size;
                
            }
            
            return maxLength;
        }
    };
} // namespace Nc

#endif