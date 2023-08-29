#ifndef MYFUNCTIONS1_H
#define MYFUNCTIONS1_H

#pragma once

#include <iostream>  
#include <string_view>
#include <cstdint> 
#include <type_traits>
#include <string>

namespace Myfcn
{
    /* Function isPrime(int) runs through an algorithm designed to check for prime numbers 
    with the given number [n] -> of type int*/
    bool isPrime(int n);

    // Calls std::cin.ignore(std::numeric<std::streamsize>::max(), '\n') to clear input buffer.
    void ignoreBufferInput();

    /* Performs input validation by calling some of object std::cin of type std::istream's member function
    to perform checks on if the state of the buffer is alright.  */
    bool inputBufferValidation();
    
    /*Function getInput<T>(std::string_view) asks the user through a prompt message[string_view object prompt] 
    for an input of type T,determined by the caller. This function supports all input types given the template type parameter T and 
    does error checking, making sure the appropriate value is extracted to apt object.*/
    template <typename T>
    T getInput(std::string_view prompt)
    {
        T input{};

        while (true)
        {
            std::cout << prompt;
            std::cin >> input;
            
            ignoreBufferInput(); 

            if (!inputBufferValidation())
            std::cout << "Invalid Input. Try Again!\n";
            else
            break;
        }
    
        return input; 
    }
    template <>
    inline std::string getInput<std::string>(std::string_view prompt)
    {
        std::string input{};

        std::cout << prompt;
        std::getline(std::cin >> std::ws, input);
            
        return input; 
    }
    

    // returns true if parameter x of type int is even, false otherwise...
    bool isEven(int x);
    
    // performs positive exponentiation on integers alone..
    std::int64_t powint(int base, int exponent);

    constexpr int compileTime(auto value)
    {
        return value;
    }

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


} // namespace Myfcn


#endif