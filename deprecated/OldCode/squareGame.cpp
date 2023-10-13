#include "squareGame.h"

namespace squareGame
{
    void displayVector(std::vector<int>& d)
    {
        for (auto &&i : d)
        {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }
    
    void succesiveVector(int startElement, std::vector<int>& V)
    {
        std::iota(V.begin(), V.end(), startElement);
        std::for_each(V.begin(), V.end(), [] (int &i) -> void
        {
            i = (int)(std::pow(i, 2));
        }
        );
    }
    
    void multiplyByRandomNumber(int R, std::vector<int>& container)
    {
        std::for_each(container.begin(), container.end(), [R] (int &i) -> void
        {
            i *= R;
        }
        );
    }
    
    bool checkGuesses(std::vector<int>& C)
    {
        int ssize{std::ssize(C)};
        for (int i{0}; i < ssize; ++i)
        {
            int guess{ (int) numeric::numbers::getInput("> ")};
            std::vector<int>::iterator find{std::find(C.begin(), C.end(), guess)};
            if (guess == *find)
            {
                C.erase(find);
                std::cout << "Nice! " << std::ssize(C) << " number(s) left" << '\n';
            }
            else
            {
                std::vector<int>::iterator find_if{std::find_if(C.begin(), C.end(), [=] (int a) -> bool
                {
                    return (guess >= (a - 4)) && (guess <= (a + 4));
                }
                )
                };
                
                if (find_if == C.end())
                {
                    std::cout << guess << " is wrong!\n";
                    break;
                }
                else
                {
                    std::cout << guess << " is wrong!";
                    std::cout << " Try " << *find_if << " next time\n";
                    break;
                }
            }
        }
        if (C.empty())
        return true;
        else 
        return false;
    }
    
    void squareGameBody()
    {
        char userChoice{};
        do
        {
            int startE{ (int) numeric::numbers::getInput("Start where? ")};    
            int vectorSize{ (int) numeric::numbers::getInput("How many? ")};    
    
            std::vector<int> vGame( (size_t) vectorSize);
    
            succesiveVector(startE, vGame); 
    
            std::cout << "I generated " << vectorSize << " square numbers.\n";
    
            int random_multiplier{random::get(2, 4)};
            multiplyByRandomNumber(random_multiplier, vGame);
    
            std::cout << "Do you know what each number is after multipying it by " << random_multiplier << '\n';
    
            if (checkGuesses(vGame))
            std::cout << "You found all the numbers. Good Job!\n";
            else
            std::cout << "You failed!\n";
    
            userChoice = guess::getChoice();
            if (userChoice == 'y')
            {
                system("cls");
                std::cout << "GREAT\n";
            }
        } while (userChoice == 'y' || userChoice == 'Y');
        
        if (userChoice == 'n' || userChoice == 'N')
        {
            std::cout << "Okay\n" << "MAYBE SOMEOTHER TIME THEN, BYE\n";
        }
         
    }

} // namespace squareBody
