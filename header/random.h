#ifndef RANDOM_H
#define RANDOM_H

#pragma once

#include <chrono>
#include <random>
#include <cstdint>


namespace Myfcn::Random
{
    namespace clock = std::chrono;
    using mtType = std::mt19937::result_type; 

    // Not to be called explicitly
    inline std::mt19937 init()
    {
        std::random_device rd{};

        std::seed_seq ss
        {rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(), static_cast<mtType>(clock::steady_clock::now().time_since_epoch().count())};
        
        return std::mt19937{ ss }; 
    }
    
    inline std::mt19937 mt{ init() }; 
    
    inline std::int64_t get(std::int64_t min, std::int64_t max)
    {
        if (min > max)
        std::swap(min, max);

        return std::uniform_int_distribution{min, max}(mt);
    } 


} // namespace Random



#endif