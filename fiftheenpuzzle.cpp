#include <thread>
#include <chrono>

#include "fiftheenpuzzle.h"
#include "myfunctions1.h"

using namespace std::chrono_literals;

namespace fiftheenpuzzle
{
    //Introduction to the fiftheenpuzzle game.
    void intro()
    {
        std::cout << "\nWelcome to FiftheenPuzzle\n\n";
        std::cout 
        << "w - slide tile up.\n"
        << "s - slide tile down.\n"
        << "a - slide tile left.\n"
        << "d - right tile right.\n"
        << "q - quit fiftheenpuzzle.\n\n";
    }

    void randomizeBoard(Board& b)
    {
        for (std::chrono::seconds s = 0s; s < 4s; ++s)
        {
            std::cout << "Randomizing Board.......\n";
            std::this_thread::sleep_for(2s);
        }
        b.randomize();
        std::cout << '\n';
    }

    //Performs input actions.
    void inputActions(Board& b, const std::optional<Myfcn::Point2d>& maybeTilePosition)
    {
        if (maybeTilePosition.has_value())
        {
            Tile e_t{b}, u_t{b, *maybeTilePosition};
            e_t.swap(u_t);
        }
        std::cout << b << '\n';
    }

    //Asks For Play again choice.
    char playAgainChoice()
    {
        char choice{};
        while (true)
        {
            choice = Myfcn::getInput<char>("Want to play again(y/n)? ");
            switch (choice)
            {
            case 'y':
            case 'Y':
            case 'n':
            case 'N':
            return choice;
            }
        }
        return choice;
    }

    void body()
    {
        std::system("chcp 65001");
        std::system("color 06");

        while (true)
        {
            intro();

            Board b{};

            std::cout << b << "\n\n";
            randomizeBoard(b);
            std::cout << b << "\n\n";
            
            bool is_solved{false};
            char userinput{};
            size_t static_counter{};
            constexpr size_t nOfTriesTillCheck{15};

            while (!is_solved)
            {
                using namespace Myfcn;

                userinput = Myfcn::getInput<char>("[ w/a/d/q ]? ");

                switch (userinput)
                {
                case 'w': inputActions(b, b.slideTile(Point2d::Direction::up)); break;
                case 's': inputActions(b, b.slideTile(Point2d::Direction::down)); break;
                case 'a': inputActions(b, b.slideTile(Point2d::Direction::left)); break;
                case 'd': inputActions(b, b.slideTile(Point2d::Direction::right)); break;
                case 'q': std::cout << "GoodBye\n"; return;
                }

                /*Since it is impossible for the puzzle to be solved under a max of [n < 15], where n is the numer of tries/moves.
                Let the program start checking for a win condition after the player has reached 15 tries/moves.
                Solely to facilitate optimization and canbe removed if needed.*/
                if (static_counter > nOfTriesTillCheck)
                {
                    //Check for a win condition.
                    if (b == Board{})
                    {
                        std::cout << "The Puzzle is Solved!\nCongratulations.\n\n";
                        is_solved = true;
                    }
                }
                ++static_counter;
            }/*------------------------EndofWhileLoop---------------------*/

            userinput = playAgainChoice();
            if (userinput == 'n' or userinput == 'N')
            {
                std::cout << "Goodbye.\n";
                break;
            }
            else
            {
                std::system("cls");
            }
            
        }/*------------------------EndofWhileLoop------------------------*/
        
    }
} // namespace fiftheenpuzzle
