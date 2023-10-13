#include "stream/i_o.h"
#include "stringoutputstream.h"
#include "myfunctions1.h"
#include "Random.h"

namespace hi_lo
{
    // Returns the choice(y/n) of the user - contains error checking. 
    // Not to be called explicitly, refer to the body() function for that.
    char getChoice()
    {
        while (true)
        {
            char choice{Myfcn::getInput<char>("Would you like to play again(y/n): ")};
            switch (choice)
            {
            case 'y':
            case 'n':   
            case 'Y':
            case 'N':
            return choice;
            }
        }
        return '\0';
    }

    /* Asks the user for a valid guess, if the user enters an invalid guess the function handles it 
    by printing an error message telling the user to enter a valid guess.
    Not to be called explicitly, refer to the body() function for that. */
    int guessValidation(size_t n_of_tries)
    {
        StringOutputStream prompt{std::ostringstream{"Guess #", std::ios_base::app}};
        prompt.write(n_of_tries, ':');

        int guess{ Myfcn::getInput<int>(prompt.get_streamdata()) };

        while (true)
        {
            if (guess < 0 or guess > 100)
            {
                System::coutput.printwl("Out range(Must be within 1 - 100)!");
                guess = Myfcn::getInput<int>(prompt.get_streamdata());
            }

            else
            { break; }
        }

        return guess;
    }
    
    void body()
    {
        using namespace Myfcn;
        using namespace Streams;

        char choice{};

        do
        {
            System::coutput.printwl("");
            
            System::coutput.printws_endl("I am thinking of nmber between 1 and 100.", "You have 7 tries to guess what it is.");

            int randomInteger{static_cast<int>(Random::get(1, 100))};

            for (size_t n_of_tries = 1; n_of_tries <= 7; n_of_tries++)
            {
                int guess{guessValidation(n_of_tries)};

                if (guess != randomInteger)
                System::coutput.printws_endl("Your guess is too", (guess > randomInteger? "high" : "low"));
                else
                {
                    System::coutput.printwl("Correct! You win!....");
                    break;
                }

                if (n_of_tries == 7)
                System::coutput.printws_endl("Sorry, you lose. The correct number was", randomInteger);
            }

            choice = getChoice();

        } while (choice == 'y' or choice == 'Y');

        System::coutput.printwl("Goodbye!");
    }

    
} // namespace hi_lo


