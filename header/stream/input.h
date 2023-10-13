#ifndef INPUT_H_
#define INPUT_H_

#pragma once

#include <iostream>
#include "inputstream.h"

namespace System
{
    /*Instantiations*/
    
    inline Streams::InputStream cinput{ std::forward<decltype(std::cin)>(std::cin) };
    inline Streams::InputStream wcinput{ std::forward<decltype(std::wcin)>(std::wcin) };

} // namespace Streams::System


#endif