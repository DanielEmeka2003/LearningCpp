#ifndef HI_LO_H
#define HI_LO_H

#pragma once

#include <iostream>


namespace Myfcn::hi_lo
{
    // Returns the choice(y/n) of the user - contains error checking. 
    // Not to be called explicitly, refer to the body() function for that.
    char getChoice();

    /* Asks the user for a valid guess, if the user enters an invalid guess the function handles it 
    by printing an error message telling the user to enter a valid guess.
    Not to be called explicitly, refer to the body() function for that. */
    int GuessValidation(size_t n_of_tries);

    /* This is the main body of the hi-lo game, a call to this function is all you need to begin the game. */
    void body();
    
} // namespace hi_lo



#endif