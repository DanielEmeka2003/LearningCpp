#if !defined(MISCELLANY_H)
#define MISCELLANY_H

#pragma once

#include "guessGame.h"
#include "gfunctions.h"
#include "customTypes.h"
#include "Random.h"

namespace miscellany
{
    bool gradeSort_students(Students a, Students b);
    bool ascending_order(int a, int b);
    bool descending_order(int a, int b);
    bool even_order(int a, int b);
    int factorial(int N);
    int sumOfdigits(int Quotient);
    
    template <Sizeofarray array_size, typename T>
    void selection_sort(T (&array) [array_size], bool (*predicate) (T, T) = ascending_order)
    {
        for (int i = 0; i < (static_cast<int> (array_size)) - 1; ++i)
        {
            int smallest_index{i};

            for (int current_index = i; current_index < (static_cast<int> (array_size)); ++current_index)
            {
                if (predicate(array[current_index], array[smallest_index]))
                {
                    smallest_index = current_index;
                }

            }
            //std::cout << '\n' << array[smallest_index] << '\t' <<  array[i] << '\n';
            std::swap(array[smallest_index], array[i]);
            //std::cout << array[smallest_index] << '\t' <<  array[i]; 
        }
    }

    template <Sizeofarray array_size>
    void bubble_sort(auto (&array) [array_size])
    {
        for (int n_of_iterations_till_sort{1}; n_of_iterations_till_sort < (static_cast<int> (array_size)); ++n_of_iterations_till_sort)
        {
            bool flag{false};
            for (int lesser_index{0}, greater_index{1}; greater_index < (static_cast<int> (array_size)); ++lesser_index, ++greater_index)
            {
                if (array[lesser_index] > array[greater_index])
                {
                    std::swap(array[lesser_index], array[greater_index]);
                    flag = true;
                }
            }
            if (flag == false)
            {
                std::cout << "Terminated at the " << n_of_iterations_till_sort << "th iteration\n"; 
                break;
            }

        }
    }

    template <Sizeof2Darray_m main_size, Sizeof2Darray_i inner_size>
    void print2DArray(auto (&array2D) [main_size] [inner_size])
    {
        for (int main_array{0}; main_array < (static_cast<int> (main_size)); ++main_array)
        {
            for (int inner_array{0}; inner_array < (static_cast<int> (inner_size)); ++inner_array)
            {
                std::cout << array2D[main_array] [inner_array] << '\t';
            }
            std::cout << '\n';
        }
    }

} // namespace miscellany


#endif // MISCELLANY_H
