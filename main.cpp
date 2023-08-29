#include <iostream>
#include <fstream>

#include <cstdlib> 
#include <cstdint>
#include <typeinfo>
#include <type_traits>
#include <cstddef>
#include <iomanip>
#include <utility>

#include <string>
#include <string_view>

#include <cassert>
#include <bitset> 
#include <format>

#include <chrono>
#include <thread>

#include <array>
#include <vector>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <functional>

#include <map>

#include "myfunctions1.h"
// #include "card.h"
// #include "player.h"
// #include "dealer.h"

using namespace std::string_literals;

//#include "timer.h"

int sumDigits(int dividend)
{
    /*Recursive termination condition.
    Returns the dividend back if it is not a base10 digit.*/
    if (dividend < 10)
    return dividend;
    //Holds the quotient and remainder of input dividend/divisor where the divisor is equal to 10.
    auto quotient_rem{std::div(dividend, 10)};
    //The quotient now becomes the dividend.
    return sumDigits(quotient_rem.quot) + quotient_rem.rem;
}

using namespace std::chrono;

void fcn()
{
    std::this_thread::sleep_for(2150000us);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{ 
    //std::system("chcp 65001");

    std::cout << std::boolalpha;

    
    
    
    return EXIT_SUCCESS;
}  











