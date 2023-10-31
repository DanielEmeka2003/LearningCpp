#include <thread>
#include <chrono>

#include "fiftheenpuzzle.h"
#include "myfunctions1.h"
#include "stream/output.h"

using namespace std::chrono_literals;

namespace fiftheenpuzzle
{
    //Introduction to the fiftheenpuzzle game.
    void intro()
    {
        System::coutput.write_endl("");
        System::coutput.writewl("Welcome to FiftheenPuzzle");

        System::coutput.writewl("w: slide tile up.", "s: slide tile down.", "a: slide tile left.", "d: right tile right.", "q: quit game.");
        System::coutput.write_endl("");
    }

    void randomizeBoard(Board& b)
    {
        for (std::chrono::seconds s = 0s; s < 4s; ++s)
        {
            System::coutput.writewl("Randomizing Board......");
            std::this_thread::sleep_for(2s);
        }
        b.randomize();
    }

    //Performs input actions.
    void inputActions(Board& b, const std::optional<Myfcn::Point2d>& maybeTilePosition)
    {
        if (maybeTilePosition.has_value())
        {
            Tile e_t{b}, u_t{b, *maybeTilePosition};
            e_t.swap(u_t);
        }
        System::coutput.writewl(b);
        System::coutput.write_endl("");
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
        while (true)
        {
            intro();

            Board b{};

            System::coutput.writewl(b);
            System::coutput.write_endl("");

            randomizeBoard(b);
            System::coutput.writewl(b);
            System::coutput.write_endl("");
            
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
                case 'q': System::coutput.write_endl("GoodBye!"); return;
                }

                /*Since it is impossible for the puzzle to be solved under a max of [n < 15], where n is the numer of tries/moves.
                Let the program start checking for a win condition after the player has reached 15 tries/moves.
                Solely to facilitate optimization and can be removed if needed.*/
                if (static_counter > nOfTriesTillCheck)
                {
                    //Check for a win condition.
                    if (b == Board{})
                    {
                        System::coutput.writewl("The Puzzle is Solved!", "Congratulations.");
                        System::coutput.write_endl("");
                        System::coutput.write_endl("");
                        is_solved = true;
                    }
                }
                ++static_counter;
            }/*------------------------EndofWhileLoop---------------------*/

            userinput = playAgainChoice();
            if (userinput == 'n' or userinput == 'N')
            {
                System::coutput.write_endl("Goodbye!");
                break;
            }
            else
            std::system("cls");
            
            
        }/*------------------------EndofWhileLoop------------------------*/
        
    }
} // namespace fiftheenpuzzle
