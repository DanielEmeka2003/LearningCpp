#ifndef MYFUNCTIONS1_H
#define MYFUNCTIONS1_H

#pragma once

#include <string_view>
#include <cstdint> 
#include <type_traits>
#include <string>

#include "stream/i_o.h"

namespace Myfcn
{
    /* Function isPrime(int) runs through an algorithm designed to check for prime numbers 
    with the given number [n] -> of type int*/
    bool isPrime(int n) noexcept;
    
    /*Function getInput<T>(std::string_view) asks the user through a prompt message[string_view object prompt] 
    for an input of type T, determined by the caller.

    prompt - prompt message, uses Streams::System.printws(arg) to output the prompt message.

    werrorhandling(with error handling) - if true the function getInput performs error handling by calling
    function InputStream::read_ to read the user input, else it calls InputStream::read to read the user input.
    [Note] This parameter is useless in the case of the type to extract being a std::string, because at compile time it is replaced
    with InputStream::read_str, for reading user input to std::strings
    Behaves exactly like this:
    {
        if constexpr (std::is_same_v<T, std::string>)
        System::cinput.read_str(input);
        else
        {
            if (werrorhandling)
            System::cinput.read_(input);
            else
            System::cinput.read(input);
        }   
    }*/
    template <typename T>
    T getInput(std::string_view prompt, [[maybe_unused]] bool werrorhandling = true)
    {
        T input{};
        System::coutput.printws(prompt);

        if constexpr (std::is_same_v<T, std::string>)
        System::cinput.read_str(input);
        else
        {
            if (werrorhandling)
            System::cinput.read_(input);
            else
            System::cinput.read(input);
        }
        return input;
    }

    consteval int compileTime(auto value)
    { return value; }

    int sum_ofposbase10digits(int base10number);

    
    #if 0 //Sentiment Code

    // Performs selection sort[ascending order] on a fixed array of any type
    template <size_t size, typename Type>
    void selection_sort(Type (&array)[size])
    {
        constexpr size_t maximum_n_of_search{size - 1};

        for (size_t currentIndex{}; currentIndex < maximum_n_of_search; ++currentIndex)
        {
            size_t smallestIndex{currentIndex};

            for (size_t i{currentIndex + 1}; i < std::size(array); ++i)
            {
                if (array[i] < array[smallestIndex])
                smallestIndex = i;

            }

            std::swap(array[currentIndex], array[smallestIndex]);
        }
    }

    // Performs bubble sort[ascending order] on a fixed array of any type 
    template <size_t size, typename Type>
    void bubble_sort(Type (&array)[size])
    {
        const size_t maximumBubbleSort{std::size(array) - 1};
        bool swap_flag{};

        for (size_t n_of_times{}; n_of_times < (std::size(array) - 1); ++n_of_times)
        {
            for (size_t leftIndex{}, rightIndex{1}; leftIndex < (maximumBubbleSort - n_of_times); ++leftIndex, ++rightIndex)
            {
                if (array[leftIndex] > array[rightIndex])
                {
                    std::swap(array[leftIndex], array[rightIndex]);
                    swap_flag = true;
                }
            }
            if (!swap_flag)
            {
                std::cout << "Early Termination at the " << (n_of_times + 1) << " Iteration\n";
                break;
            }
            swap_flag = false;
        }
    }
    #endif


} // namespace Myfcn


#endif