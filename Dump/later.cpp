#include "functions.h"
#include "Random.h"
#include "customTypes.h"
#include "miscellany.h"
#include "blackJack.h"


struct Season
{
    std::string_view name{};
    double averageTemp{};
};

bool isVowel(char ch)
{
    switch (ch)
    {
    case 'A':
    case 'a':
    case 'E':
    case 'e':
    case 'I':
    case 'i':
    case 'O':
    case 'o':
    case 'U':
    case 'u':
        return true;
    default:
        return false;
    }
}

bool compare(Card& a, Card& b)
{
    return a.rank < b.rank;
}

int main() 
{
    
    [[maybe_unused]] int two_D_arrays[] [5] {{18, 24, 63, 54, 85}, {11, 21, 43, 46, 75}, {10, 32, 93, 14, 65}}; // the main array length can be ommited
    [[maybe_unused]] int D_arrays[10] [10] {}; // the main array length can be ommited


    for (int main_array{0}, constant{1}; main_array < std::ssize(D_arrays); ++main_array, ++constant)
    {
        for (int inner_array{0}, multipier{1}; inner_array < std::ssize(*D_arrays); ++inner_array, ++multipier)
        {
            D_arrays[main_array] [inner_array] = constant * multipier;
            std::cout << D_arrays[main_array] [inner_array] << '\t';
        }
        std::cout << '\n';
    }

    int array[] {12, 4, 6, 7, 8, 9};
    char name[] {"Daniel Emeka"};
    std::cout <<  std::count_if(std::begin(name), std::end(name), isVowel) << '\n';    
    std::cout << std::count(std::begin(array), std::end(array), 5);
    
    std::array<Students, 8> arr
    { 
        {
            {"Albert", 3},
            {"Ben", 5},
            {"Christine", 2},
            {"Dan", 8},
            {"Enchilada", 4},
            {"Francis", 1},
            {"Greg", 3},
            {"Herald", 5},
        }
    };

    std::array<Students, 8>::iterator maxElement;

    std::function<bool (Students, Students)> comp 
    {
        [](const Students& a, const Students& b)
        {
            return a.grade < b.grade;
        }
        
    };

    maxElement = std::max_element(arr.begin(), arr.end(), comp);
    
    std::cout << maxElement->name << " is the best student\n";

    std::array<Season, 4> seasons
    {
        {
            {"Spring", 285.0},
            {"Summer", 296.0},
            {"Fall", 288.0},
            {"Winter", 263.0}
        }
    };

    std::function comp2
    {
        [](const Season a, const Season b)
        {
            return a.averageTemp < b.averageTemp;
        }
    };

    std::sort(seasons.begin(), seasons.end(), comp2);

    for (auto &&i : seasons)
    {
        std::cout << i.name << ' ' << i.averageTemp << '\n';
    }
    
    

    return 0;
}
