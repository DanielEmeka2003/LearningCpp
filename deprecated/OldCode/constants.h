#if !defined(CONSTANTS_H)
#define CONSTANTS_H

#pragma once

/* Protype of inline global constants
**inline constexpr gravity{9.8};**
note: they should be inside a namespace */

namespace constants
{
    inline constexpr auto max_class_size{0};
    inline constexpr auto gravity{9.8};
} // namespace constants



#endif // CONSTANTS_H
