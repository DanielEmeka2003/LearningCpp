#include "fPuzzleGame.h"

namespace fiftheenPuzzle
{
    void Introduction(std::string_view username)
    {
        std::cout << "\t\tWelcome " << username << "\n\n";
        std::cout << "\tThis is a replica of the well known FIFTHEEN PUZZLE game.\n";
        std::cout << "\tIt may not be as sophisticated as other iterations, but i assure you that it's worth palying.\n\n";
        std::cout << "\tGUIDE\n";
        std::cout << "\tEnter these various commands to play:\n" << "\tw - " << "to move a tile up\n"
        << "\ta - " << "to move a tile down\n"
        << "\ts - " << "to move a tile left\n"
        << "\td - " << "to move a tile right\n"
        << "\tq - " << "to quit the game at any time\n"
        << "\tHope you enjoy\n\n";
    }

    char getUserInput()
    {
        char userinput{};
        do
        {
            userinput = text::getChar("cmd(w/a/s/d): ");

            if((userinput != 'w' and userinput != 'W') and 
            (userinput != 'a' and userinput != 'A') and 
            (userinput != 's' and userinput != 'S') and 
            (userinput != 'd' and userinput != 'D') and
            (userinput != 'q' and userinput != 'Q'))

            std::cout << "Invalid!\n";

        } while ((userinput != 'w' and userinput != 'W') and 
        (userinput != 'a' and userinput != 'A') and 
        (userinput != 's' and userinput != 'S') and 
        (userinput != 'd' and userinput != 'D') and
        (userinput != 'q' and userinput != 'Q'));

        return userinput;   
    }
    void random_direction()
    {
        std::cout << "Generating random...." << static_cast<PuzzleCommands>(random::get(up, right)) << '\n';
        std::cout << "Generating random...." << static_cast<PuzzleCommands>(random::get(up, right)) << '\n';
        std::cout << "Generating random...." << static_cast<PuzzleCommands>(random::get(up, right)) << '\n';
        std::cout << "Generating random...." << static_cast<PuzzleCommands>(random::get(up, right)) << '\n';
        std::cout << '\n';
    }
    void commandImplementation(Board& board, PuzzleCommands cmd)
    {
        Point2d emptyTilePoint{board.getPointsOfTile()};
        Point2d adjacentTile{emptyTilePoint.getAdjacentPoint(cmd)};
        // i am swapping here to get the indexes
        emptyTilePoint.swapPoints();
        adjacentTile.swapPoints();

        Tile empty{board[emptyTilePoint.getTile_X_FromPoints()][emptyTilePoint.getTile_Y_FromPoints()]};
        Tile adjacent{board[adjacentTile.getTile_X_FromPoints()][adjacentTile.getTile_Y_FromPoints()]};
        empty(adjacent);
        std::cout << board << '\n';
    }
    bool winMessage()
    {
        std::cout << "\nCongrats! You won.\n";
        char userChoice{guess::getChoice()};
        if(userChoice == 'y' || userChoice == 'Y')
        return true;
        else
        return false;
    }
    void body()
    {
        startAgain:
        Board board{};
        Board solved{};
        solved.fill();
        board.fill().randomize();

        std::string_view userName{text::getStringInput("Enter your name: ")};
        Introduction(userName);
        std::cout << "\nPuzzle\n";
        std::cout << solved << '\n';
        random_direction(); 
        std::cout << board << '\n';

        std::array<char, 5> cmd{'w', 'a', 's', 'd', 'q'};
        std::array<char, 5> capital_cmd{'W', 'A', 'S', 'D', 'Q'};
        size_t number_of_tries{};
        constexpr size_t max_nOfTries_tillChecking{16};

        while (true)
        {
            char userInput{getUserInput()};

            if(userInput == cmd[up] or userInput == capital_cmd[up])
            {
                ++number_of_tries;
                // opposite direction to get adjacent tile to the empty tile-> down
                commandImplementation(board, down);
            }
            else if(userInput == cmd[down] or userInput == capital_cmd[down])
            {
                ++number_of_tries;
                // opposite direction to get adjacent tile to the empty tile-> up
                commandImplementation(board, up);
            }
            else if(userInput == cmd[left] or userInput == capital_cmd[left])
            {
                ++number_of_tries;
                // opposite direction to get adjacent tile to the empty tile-> right
                commandImplementation(board, right);
            }
            else if(userInput == cmd[right] or userInput == capital_cmd[right])
            {
                ++number_of_tries;
                // opposite direction to get adjacent tile to the empty tile-> left
                commandImplementation(board, left);
            }
            else if(userInput == cmd[quit] or userInput == capital_cmd[quit])
            {
                std::cout << "Bye " << userName << "\n\n";
                return;
            }
            if (number_of_tries == max_nOfTries_tillChecking and board == solved)
            {
                if(winMessage())
                {
                    std::cout << "\t\tGreat\n\n";
                    system("cls");
                    goto startAgain;
                }
                else
                {
                    std::cout << "See you someother time " << userName << "\n\n";
                }
            }
        }

    }
    
} // namespace fiftheenPuzzle
