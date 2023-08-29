#include <iostream>
#include <sstream>

#include "myfunctions1.h"
#include "Random.h"

namespace Myfcn::hi_lo
{
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

    int GuessValidation(size_t n_of_tries)
    {
        std::stringstream stringPrompt{};

        stringPrompt << "Guess #" << n_of_tries << ": ";
        int guess{Myfcn::getInput<int>(stringPrompt.str())};

        while (true)
        {

            if (guess < 0 or guess > 100)
            {
                std::cout << "Out range(Must be within 1 - 100)!\n";
                guess = Myfcn::getInput<int>(stringPrompt.str());
            }

            else
            {
                stringPrompt.str("");
                break;
            }
        }

        return guess;
    }
    
    void body()
    {
        char choice{};

        do
        {
            std::cout << "\nI am thinking of nmber between 1 and 100. " << " You have 7 tries to guess what it is.\n";
            int randomInteger{static_cast<int>(Random::get(1, 100))};

            //std::cout << "Random number: " << randomInteger << '\n';

            for (size_t n_of_tries = 1; n_of_tries <= 7; n_of_tries++)
            {
                int guess{GuessValidation(n_of_tries)};

                if (guess != randomInteger)
                std::cout << "Your guess is too " << ((guess > randomInteger)? "high" : "low") << '\n';
                else
                {
                    std::cout << "Correct! You win!....\n";
                    break;
                }

                if (n_of_tries == 7)
                std::cout << "Sorry, you lose. The correct number was " << randomInteger << ".\n";
            }

            choice = getChoice();

        } while (choice == 'y' or choice == 'Y');

        std::cout << "GoodBye!\n";
    }

    
} // namespace hi_lo


