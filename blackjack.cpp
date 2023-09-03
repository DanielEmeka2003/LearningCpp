#include <vector>
#include <iomanip>
#include <format>
#include <thread>
#include <chrono>

#include "blackjack.h"
#include "myfunctions1.h"

using namespace std::chrono_literals;
namespace blackjack
{
    /*If any of the exit scenarios within a play is met by a player, this function returns the position of the player
    so he/she can be erased from the current round.*/
    std::vector<size_t> check_for_exit_scenarios_in_play(std::vector<Player>& players, const Dealer& dealer)
    { 
        std::vector<size_t> position{};

        for (size_t index{}; auto&& i : players)
        {
            auto outcomes{ dealer.playerPossibleOutcomes(i) };

            if (outcomes == Dealer::HigherThan21)
            {
                std::cout << std::format("[Dealer] {} busts\n", i.getName());
                dealer.manageBets(i, Dealer::IncreaseBy::zero_percent);
                position.push_back(index);
            }
            else if (outcomes == Dealer::EqualTo21)
            {
                std::cout << std::format("[Dealer] {}!\tYour hand is equal to 21\n", i.getName());   
                dealer.manageBets(i, Dealer::IncreaseBy::fifthy_percent);
                position.push_back(index);
            }
            ++index;
        } 

        return position;      
    }

    //Divides a section with a series of dotted dashes, mimicing a dotted line.
    void lineDivision()
    {
        std::cout << std::setw(80) << std::setfill('-') << '\n';
    }

    //Deals Start up cards to both the players and the dealer.
    void startUpDeals(std::vector<Player>& players, Dealer& dealer)
    {
        constexpr size_t numberofdeals{2};
        bool faceup{true};

        for (size_t i = 0; i < numberofdeals; i++)
        {
            for (auto &&j : players)
            {
                dealer.deal(j);
            }
            dealer.deal(faceup);

            lineDivision();

            faceup = false;
        }
    }

    //Asks For Play again choice.
    char playAgainChoice()
    {
        char choice{};
        while (true)
        {
            choice = Myfcn::getInput<char>("[Dealer] Play again(y/n)? ");
            switch (choice)
            {
            case 'y':
            case 'Y':
            case 'n':
            case 'N':
            return choice;
            
            default: std::cout << "Invalid Answer!\n"; break;
            }
        }
        return choice;
    }

    void body(int argumentCount, char** argumentVector)
    {
        std::system("chcp 65001");
        std::system("color 06");
        
        bool is_commandlineArguments{};

        if (argumentCount > 1)
        is_commandlineArguments = true;

        while (true)
        {
            std::vector<Player> players{};
            Dealer dealer{};

            /*--------------------------------------Intro-----------------------------------------*/
            {
                if (is_commandlineArguments)
                {
                    for (int i = 1; i < argumentCount; i++)
                    {
                        players.push_back(Player{argumentVector[i]});
                    }
                    std::cout << '\n';

                    is_commandlineArguments = false;
                }
                else
                {
                    auto maxPlayers{Myfcn::getInput<size_t>("[Dealer] How many wants to play? ")};
                    for (size_t i = 0; i < maxPlayers; i++)
                    {
                        auto name{Myfcn::getInput<std::string>("[Dealer] Name(s): ")};
                        players.push_back(Player{name});
                    } 
                    std::cout << '\n';  
                }
            }
            /*--------------------------------Intro-----------------------------------------------*/

            
            /*-------------------------------AskForBets-------------------------------------------*/
            {
                for (auto &&i : players)
                {
                    dealer.askForBet(i);
                }
                std::cout << '\n';
            }
            /*-------------------------------AskForBets-------------------------------------------*/

            /*-----------------------------ShuffleDeck-------------------------------------------*/
            {
                dealer.shuffleCards();
                std::cout << '\n';
            }
            /*-----------------------------ShuffleDeck-------------------------------------------*/

            /*----------------------------StartUpDeals------------------------------------------*/
            {
                startUpDeals(players, dealer);
            }
            /*----------------------------StartUpDeals------------------------------------------*/

            /*----------------------------RoundTable------------------------------------------*/
            {
                //First check if any possible exit scenatios is true for each individual player.
                {
                    for (auto exitscenarios{check_for_exit_scenarios_in_play(players, dealer)}; const auto &i : exitscenarios)
                    {
                        players.erase(players.begin() + static_cast<int>(i));
                    }
                    lineDivision();
                    std::cout << '\n';
                }
                //Round table now...........
                {
                    //Dramatic Pause........
                    dealer.dramaticPause(2s);

                    for (auto &&i : players)
                    {
                        dealer.roundTable(i);
                        std::cout << '\n';
                    }
                    std::cout << "\n\n";

                    //Dramatic Pause........
                    dealer.dramaticPause(2s);
                
                    //Check again, if any possible exit scenatios is true for each individual player.
                    for (auto exitscenarios{check_for_exit_scenarios_in_play(players, dealer)}; const auto &i : exitscenarios)
                    {
                        players.erase(players.begin() + static_cast<int>(i));
                    }
                    lineDivision();
                }
            }
            /*----------------------------RoundTable------------------------------------------*/

            /*--------------------------EndOfRounding-----------------------------------------*/
            {
                //Dramatic Pause............
                dealer.dramaticPause(6s);

                if (players.size() != 0)
                {   
                    std::cout << "[Dealer] Alright! Show of Hands....\n\n";
                    for (auto &&i : players)
                    {
                        //Dramatic Pause............
                        dealer.dramaticPause(3s);

                        i.displayHand();
                        std::cout << '\n';
                    }
                    dealer.displayHand();
                    std::cout << '\n';

                    /*-------------------CheckforWinnersOrLosers------------------------*/
                    {
                        while (dealer.possibleOutcomes() == Dealer::EqualToAndLessThan16)
                        {
                            dealer.deal();
                        }
                        std::cout << '\n';
                        if (dealer.possibleOutcomes() == Dealer::HigherThan21)
                        {
                            std::cout << "[Dealer] I bust\n";
                            for (auto &&i : players)
                            {
                                dealer.manageBets(i, Dealer::IncreaseBy::hunderd_percent);
                            }  
                        }
                        else
                        {
                            for (auto &&i : players)
                            {   
                                if (auto outcomes{ dealer.playerPossibleOutcomes(i) }; outcomes == Dealer::P_HandGreaterThan_D)
                                {
                                    std::cout << "[Dealer] " << i.getName() << "\tyour hand is greater than mine\n";
                                    dealer.manageBets(i, Dealer::IncreaseBy::hunderd_percent);
                                }
                                else
                                {
                                    std::cout << "[Dealer] " << i.getName() << "\tyour hand is less than mine\n";
                                    dealer.manageBets(i, Dealer::IncreaseBy::zero_percent);
                                }
                                std::cout << "\n\n"; 
                            } 
                        }
                    }
                    /*-------------------CheckforWinnersOrLosers------------------------*/
                }
            }
            /*--------------------------EndOfRounding-----------------------------------------*/

            /*--------------------------PlayAgainOption---------------------------------------*/
            {
                char choice{playAgainChoice()};
                if (choice == 'n' or choice == 'N')
                {
                    std::cout << "[Dealer] Goodbye.\n";
                    break;
                }
                else
                {
                    std::system("cls");
                    std::cout << "[Dealer] Wonderful\n\n";
                }
            }
            /*--------------------------PlayAgainOption---------------------------------------*/
            
            
        }/*--------------------------------------EndofWhileLoop-------------------------------*/
    }
} // namespace blackjack

