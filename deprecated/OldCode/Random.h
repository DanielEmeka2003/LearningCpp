#if !defined(RANDOM_H)
#define RANDOM_H

#pragma once

#include "includes.h"

namespace random
{ 
    inline auto init() 
    {   
       std::random_device rd; 
       std::seed_seq ss{(unsigned int)std::chrono::steady_clock::now().time_since_epoch().count(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
       return std::mt19937{ss};
    }
    
    inline std::mt19937 random_numbers{init()};

    inline int get(int min, int max)
    {
        if(min > max) std::swap(min, max);
        
        std::uniform_int_distribution range{min, max}; 

        return range(random_numbers); 
    } 


} // namespace random

#endif // RANDOM_H
