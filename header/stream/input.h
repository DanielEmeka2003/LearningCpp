#ifndef INPUT_H_
#define INPUT_H_

#pragma once

#include <iostream>
#include "inputstream.h"

namespace System
{
    /*Instantiations*/
    
    inline Streams::InputStream cinput{ std::cin };
    inline Streams::InputStream wcinput{ std::wcin };

} // namespace Streams::System


#endif