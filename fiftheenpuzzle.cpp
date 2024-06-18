#include <thread>
#include <chrono>

#include "fiftheenpuzzle.h"
#include "misc_utility.h"
#include "stream/output.h"

using namespace std::chrono_literals;

namespace fiftheenpuzzle
{
    //Introduction to the fiftheenpuzzle game.
    void intro()
    {
        io::cout.write_nl("");
        io::cout.writewl("Welcome to FiftheenPuzzle");

        io::cout.writewl("w: slide tile up.", "s: slide tile down.", "a: slide tile left.", "d: right tile right.", "q: quit game.");
        io::cout.write_nl("");
    }

    void randomizeBoard(Board& b)
    {
        for (std::chrono::seconds s = 0s; s < 4s; ++s)
        io::cout.writewl("Randomizing Board......"), std::this_thread::sleep_for(2s);
        
        b.randomize();
    }

    //Performs input actions.
    void inputActions(Board& b, const std::optional<misc::Point2d>& maybeTilePosition)
    {
        if (maybeTilePosition.has_value())
        {
            Tile e_t{b}, u_t{b, maybeTilePosition.value()};
            e_t.swap(u_t);
        }
        io::cout.writewl(b);
        io::cout.write_nl("");
    }

    //Asks For Play again choice.
    char playAgainChoice()
    {
        char choice{};
        while (true)
        {
            choice = misc::getInput<char>("Want to play again(y/n)? ");
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
        while (true)
        {
            intro();

            Board b{};

            io::cout.writewl(b);
            io::cout.write_nl("");

            randomizeBoard(b);
            io::cout.writewl(b);
            io::cout.write_nl("");
            
            bool is_solved{false};
            char userinput{};
            size_t static_counter{};
            constexpr size_t nOfTriesTillCheck{15};

            while (!is_solved)
            {
                using namespace misc;

                userinput = misc::getInput<char>("[ w/a/d/q ]? ");

                switch (userinput)
                {
                case 'w': inputActions(b, b.slideTile(Point2d::Direction::up)); break;
                case 's': inputActions(b, b.slideTile(Point2d::Direction::down)); break;
                case 'a': inputActions(b, b.slideTile(Point2d::Direction::left)); break;
                case 'd': inputActions(b, b.slideTile(Point2d::Direction::right)); break;
                case 'q': io::cout.write_nl("GoodBye!"); return;
                }

                /*Since it is impossible for the puzzle to be solved under a max of [n < 15], where n is the numer of tries/moves.
                Let the program start checking for a win condition after the player has reached 15 tries/moves.
                Solely to facilitate optimization and can be removed if needed.*/
                if (static_counter > nOfTriesTillCheck)
                {
                    //Check for a win condition.
                    if (b == Board{})
                    {
                        io::cout.writewl("The Puzzle is Solved!", "Congratulations.");
                        io::cout.write_nl("");
                        io::cout.write_nl("");
                        is_solved = true;
                    }
                }
                ++static_counter;
            }/*------------------------EndofWhileLoop---------------------*/

            userinput = playAgainChoice();
            if (userinput == 'n' or userinput == 'N')
            {
                io::cout.write_nl("Goodbye!");
                break;
            }
            else
            std::system("cls");
            
            
        }/*------------------------EndofWhileLoop------------------------*/
        
    }
} // namespace fiftheenpuzzle
