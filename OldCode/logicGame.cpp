#include "logicGame.h"

namespace logicGame
{
    void introduction(std::string_view player_name)
    {
        std::cout << "Welcome, " << player_name << '\n';
        rules();
    }
    void rules()
    {
        std::cout << "\t\tRULES\n";
        std::cout << "\tFor Each monster you encounter, you have to decide whether to \"Run or Fight\".\n";
        std::cout << "\tIf you decide to run, you have 50percent chance of escaping, if you escape\n";
        std::cout << "\tElse you decide to fight\n";
        std::cout << "\n\n";
    }

    char getChoice()
    {
        char choice{};
        do
        {
            choice = text::getChar("Run or Fight?: ");
            if(choice != 'r' and choice != 'R' and choice != 'f' and choice != 'F')
            std::cout << "Invalid input! Try again.\n";

        } while (choice != 'r' and choice != 'R' and choice != 'f' and choice != 'F');
        
        return choice;
    }

    char output(Monster& m)
    {
        std::cout << "You have encountered a(an) " << m.getName() << ".\n";
        auto option{getChoice()};
        std::cout << '\n';

        return option;
    }
    
    void attack(Player& p, Monster& m, TurnForAttack a)
    {
        if (a)
        m.reduceHealth(p.get_damagePerAttack());
        else
        p.reduceHealth(m.get_damagePerAttack());
    }
    void levelUp(Player& p)
    {
        p.levelUp();
    }
    void collectGold(Player& p, Monster& m)
    {
        p.addGold(m.get_goldAmount());
    }

    void logicGameBody()
    {
        char exit_choice{};
        do
        {
            Player p{static_cast<std::string>(text::getStringInput("Enter your name: "))};
            introduction(p.getName());
            Monster rm{};

            std::cout << p << ".\n\n";

            while (true)
            {
                if (p.hasWon()) 
                {
                    std::cout << "Congratulations!\n";
                    std::cout << "You have won the game by reaching level 20\n";
                    break;
                }
                if (p.isDead())
                {
                    std::cout << "You died at level " << p.getLevel() << " and with " << p.get_goldAmount() << " gold.\n\n";
                    break;
                }

                rm = Monster::generateRandomMonster();

                while (true)
                {
                    if (p.isDead())
                    {
                        std::cout << "You died at level " << p.getLevel() << " and with " << p.get_goldAmount() << "\n\n";
                        break;
                    }
                    char userOption{output(rm)};
                    if (userOption == 'r' or userOption == 'R')
                    {
                        if (random::get(0, 1))
                        {
                            std::cout << "You ran away from the " << rm.getName() << " successfully.\n\n";
                            break;
                        }
                        else
                        {
                            attack(p, rm, false);
                            std::cout << "Sorry, you cannot ran away from the " << rm.getName() << " .\n";
                            std::cout << "The " << rm.getName() << " attacks you with a damage level of " 
                            << rm.get_damagePerAttack() << ".\n";
                            std::cout << "Your health has been reduced by " << rm.get_damagePerAttack() << ".\n\n";
                        }
                        
                    }
                    else if (userOption == 'f' or userOption == 'F')
                    {
                        attack(p, rm, true);
                        std::cout << "You hit the " << rm.getName()
                        << " with a damage level of " << p.get_damagePerAttack() << ".\n";
                        if (rm.isDead())
                        {
                            collectGold(p, rm);
                            levelUp(p);
                            std::cout << "You have kiiled the " << rm.getName() << " and collected it's gold.\n";
                            std::cout << "You are now in level " << p.getLevel()
                            << " and have " << p.get_goldAmount() << " gold.\n\n";
                            break;
                        }
                        else
                        {
                            attack(p, rm, false);
                            std::cout << "Your attack was not enough to kill the " << rm.getName() << ".\n";
                            std::cout << "The " << rm.getName() << " attacks you with a damage level of " 
                            << rm.get_damagePerAttack() << ".\n";
                            std::cout << "Your health has been reduced by " << rm.get_damagePerAttack() << ".\n\n";
                        }
                    }

                }
                #if 0
                {
                    std::cout << "You wish to be healed, hope you have enough gold\n";
                    if (p.buyHealthWithGold())
                    {
                        std::cout << "Purchase successfull\n";
                        std::cout << "gold(-10): " << p.get_goldAmount() << " health(+1): " << p.getHealth() << '\n';
                    }
                    else
                    {
                        std::cout << "Purchase unsuccessfull\n";
                        std::cout << "Not enough gold\n";
                    }
                }
                #endif
                std::cout << "Current State:\n" << p << ".\n\n";
            }
            
            exit_choice = guess::getChoice();
            
            if (exit_choice == 'y' or exit_choice == 'Y')
            {
                system("cls");
                std::cout << "Excellent\n\n";
            }
            
        } while (exit_choice == 'y' or exit_choice == 'Y');
        

    }
    
} // namespace logicGame
