#ifndef INPUT_H_
#define INPUT_H_

#pragma once

#include <iostream>
#include "inputstream.h"

namespace io
{
    /*Instantiations*/
    
    inline Streams::InputStream cin{ &std::cin };
    inline Streams::InputStream wcin{ &std::wcin };

    template <typename CharT>
    std::basic_istream<CharT>& (&ws_v) (std::basic_istream<CharT>&) = std::ws<CharT, std::char_traits<CharT>>;


    inline std::basic_istream<char>& (&ws_v_c) (std::basic_istream<char>&) = std::ws<char, std::char_traits<char>>;


    inline std::basic_istream<wchar_t>& (&ws_v_wc) (std::basic_istream<wchar_t>&) = std::ws<wchar_t, std::char_traits<wchar_t>>;

} // namespace Streams::System


#endif