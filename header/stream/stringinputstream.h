#ifndef STRINGINPUTSTREAM_H
#define STRINGINPUTSTREAM_H

#pragma once

#include <sstream>
#include <utility>
#include <cctype>
#include <algorithm>
#include <chrono>

#include "inputstream.h"

namespace Streams
{
    template <typename CharT>
    class StringInputStream: public InputStream<CharT>
    {
    private:
        //Holds string input stream object.
        std::basic_istringstream<CharT> m_isstream{};
        
    public:
        StringInputStream(std::basic_istringstream<CharT>&& isstream) noexcept;
        
        StringInputStream(const StringInputStream&) = delete;
        StringInputStream(StringInputStream&&) = delete;

        StringInputStream& operator=(const StringInputStream&) = delete;
        StringInputStream& operator=(StringInputStream&&) = delete;

        ~StringInputStream() noexcept = default;

        /*
        Inserts data(const std::string&) to the string used by the input stream and resets the input stream indicator to the position given
        as the std::ios_base::openmode argument in the std::istringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::istringstring is{}
            is.str(arg); 
        }*/
        void insert_str(const InputStream<CharT>::StringType& data) noexcept;

        /*
        Inserts data(std::string&&) to the string used by the input stream and resets the input stream indicator to the position given
        as the std::ios_base::openmode argument in the std::istringstream constructor.
        Behaves exactly like this:
        { 
            std::string arg{"Emeka Daniel"};
            std::istringstring is{}
            is.str(arg); 
        }*/
        void insert_str(InputStream<CharT>::StringType&& data) noexcept;

        /*Returns the string used by the input stream.
        Behaves like this:
        {
            std::istringstream is{"Emeka Daniel"};
            is.str();
        }*/
        InputStream<CharT>::StringType get_str() noexcept;

        /*Clears the string used by the input stream.
        Behaves exactly like this:
        {
            std::istringstream is{"Emeka Daniel"};
            is.str("");
        }*/
        void empty_str() noexcept;

        /*Returns a reference to the string input stream object given.(Preferred)*/
        constexpr std::basic_istringstream<CharT>& get() noexcept;

        /*Returns a reference to the string input stream object given.(Not Preferred)*/
        constexpr std::basic_istringstream<CharT>& operator*() noexcept;

        /*Prepares a string for insertion into the string used by the input stream by parsing the string for space characters and assigning them
        the end of extraction/reading indicator character - which is the new line character and returns the parsed and modified string.

        Strings given to this function should generally be in this format: 
        {"$ $ $ $ $ $....."} where $ is any potential fixed or multibyte character.
        Each extractable character[$] should be separated by one or more leading space characters and can optionally include a begin and end
        space character like so: {" $ $ $ $ "}.
        Example:
        {
            using namespace Streams;

            std::string str{"1 Daniel 2 5.6 Emeka"};

            //would equal this: {"1#Daniel#2#5.6#Emeka#"} where # = the newline character
            str = System::cinput.prepare_str_for_insertion(str);
        }
        */
        InputStream<CharT>::StringType prepare_str_for_insertion(InputStream<CharT>::StringType str) noexcept;
    };

    template <typename CharT>
    StringInputStream<CharT>::StringInputStream(std::basic_istringstream<CharT>&& isstream) noexcept
    :InputStream<CharT>{},
    m_isstream{ std::forward<std::basic_istringstream<CharT>>(isstream) }
    {
        InputStream<CharT>::set_m_in(&m_isstream); 
    }
        
    template <typename CharT>
    void StringInputStream<CharT>::insert_str(const InputStream<CharT>::StringType& data) noexcept
    { m_isstream.str(data);}

    template <typename CharT>
    void StringInputStream<CharT>::insert_str(InputStream<CharT>::StringType&& data) noexcept
    { m_isstream.str(data); }
        
    template <typename CharT>
    InputStream<CharT>::StringType StringInputStream<CharT>::get_str() noexcept
    { return m_isstream.str(); }

    template <typename CharT>
    void StringInputStream<CharT>::empty_str() noexcept
    { return m_isstream.str(""); }

    template <typename CharT>
    constexpr std::basic_istringstream<CharT>& StringInputStream<CharT>::get() noexcept
    { return m_isstream; }

    template <typename CharT>
    constexpr std::basic_istringstream<CharT>& StringInputStream<CharT>::operator*() noexcept
    { return m_isstream; }

    template <typename CharT>
    InputStream<CharT>::StringType StringInputStream<CharT>::prepare_str_for_insertion
    (InputStream<CharT>::StringType str) noexcept
    {
        //If the last element is not a space character, push_back a space character to str
        if (!std::isspace(str.back()))
        {
            str.push_back(' ');
        }
        //Replace space characters with the new line character
        //Neccessary for making the string extractable
        std::ranges::replace(str, ' ', '\n');
        
        return str;
    }
} // namespace Myfcn


#endif