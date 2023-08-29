#include "guessGame.h"

namespace guess
{
    int getInputs(int c)
    {
        int x{};
        while (true)
        {
            std::cout << "Guess #" << c << ": ";
            std::cin >> x;
            numeric::ignoreInputBuffer();
            if (numeric::numbers::NumericInputValidation("Invalid guess")) std::cout << '\n';
            else if(x < 1 || x > 100) std::cout << "Out of Range(1 - 100)\n"; 
            else
            {
                std::cout << '\n';
                return x;
            }
        }
       
    }

    char getChoice(std::string_view choiceMessage)
    {
        while (true)
        {
            std::cout << choiceMessage;
            char choice{};
            std::cin >> choice;
            numeric::ignoreInputBuffer();
            switch (choice)
            {
                case 'y':
                case 'n':
                case 'Y':
                case 'N':
                return choice;

                default:
                break;
            }
        }
        
    } 
     
    void guessGameIntro(std::string_view intro, std::string_view gamePrompt)
    {
        std::cout << '\n' << std::setw(40) << std::setfill('*') << intro << std::setw(21) << " " << "\n\n";
        std::cout << std::setw(55) << std::setfill(' ') << gamePrompt << '\n';
    }

    void guessGamerules()
    {
        std::cout << "\n\t\t\tRULES:\n";
        std::cout << "\t1. You are expected to guess a number between 1 and 100.\n";
        std::cout << "\t2. You have a maximum of 7 guess.\n";
        std::cout << "\t3. You would be given a hint as to how close you are to the number. i.e. \"too high\" or \"too low\". \n";
        std::cout << "\t4. If you guess the number before the maximum number of tries allowed, you win else you lose.\n";
        std::cout << "\t\t\t Hope you Enjoy :).\n";
    } 

    void guessGamebody()
    {
        char userChoice{};
        guessGameIntro();
        guessGamerules();
        do
        {
            auto guessTries{0};
            auto N{random::get(1, 100)};
            
            while (true)
            {
                ++guessTries;
                auto guess{getInputs(guessTries)};
                
                if(guess < N && guessTries <= 7) std::cout << "Too Low\n";
                else if(guess > N && guessTries <= 7)  std::cout << "Too High\n";
                else if(guess == N && guessTries <= 7)
                {
                    std::cout << "CORRECT! YOU WIN!\n";
                    break;
                } 

                if(guess !=N && guessTries == 7)
                {
                    std::cout << "SORRY, YOU LOSE! THE CORRECT ANSWER WAS: " << N << '\n';
                    break;
                } 
            }
            userChoice = getChoice();
            if (userChoice == 'y')
            {
                system("cls");
                std::cout << "\t\t\tWonderful.\n";
                guessGameIntro();
                guessGamerules();
            }
        } while (userChoice == 'y' || userChoice == 'Y');

        if (userChoice == 'n' || userChoice == 'N')
        {
            std::cout << "\t\t\tTOO BAD :(\n" << "\t\tMAYBE SOMEOTHER TIME, BYE\n";
        }
        
        
    }

} // namespace game
