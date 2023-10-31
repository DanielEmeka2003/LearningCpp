#ifndef OUTPUT_H_
#define OUTPUT_H_

#pragma once

#include <iostream>

#include "outputstream.h"

namespace System
{

    /*Pre-defined class template instantiations*/

    inline Streams::OutputStream coutput{ &std::cout };
    inline Streams::OutputStream cerror{ &std::cerr };

    inline Streams::OutputStream wcoutput{ &std::wcout };
    inline Streams::OutputStream wcerror{ &std::wcerr };
    
    /*std::basic_ostream function template manipulators implemented as function pointers*/

    template <typename CharT>
    std::basic_ostream<CharT>& (&endl_v)(std::basic_ostream<CharT>&) = std::endl<CharT, std::char_traits<CharT>>;
    template <typename CharT>
    std::basic_ostream<CharT>& (&ends_v)(std::basic_ostream<CharT>&) = std::ends<CharT, std::char_traits<CharT>>;
    template <typename CharT>
    std::basic_ostream<CharT>& (&flush_v)(std::basic_ostream<CharT>&) = std::flush<CharT, std::char_traits<CharT>>;


    /*std::basic_ostream function template manipulators implemented as function pointers full instantiations*/
    
    inline std::basic_ostream<char>& (&endl_v_c)(std::basic_ostream<char>&) = std::endl<char, std::char_traits<char>>;
    inline std::basic_ostream<char>& (&ends_v_c)(std::basic_ostream<char>&) = std::ends<char, std::char_traits<char>>;
    inline std::basic_ostream<char>& (&flush_v_c)(std::basic_ostream<char>&) = std::flush<char, std::char_traits<char>>;

    
    inline std::basic_ostream<wchar_t>& (&endl_v_wc)(std::basic_ostream<wchar_t>&) = std::endl<wchar_t, std::char_traits<wchar_t>>;
    inline std::basic_ostream<wchar_t>& (&ends_v_wc)(std::basic_ostream<wchar_t>&) = std::ends<wchar_t, std::char_traits<wchar_t>>;
    inline std::basic_ostream<wchar_t>& (&flush_v_wc)(std::basic_ostream<wchar_t>&) = std::flush<wchar_t, std::char_traits<wchar_t>>;



    
} // namespace Streams::System


#endif