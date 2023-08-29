#include <thread>
#include <chrono>
#include <format>
#include <numeric>

#include "dealer.h"
#include "myfunctions1.h"

namespace chrono = std::chrono;
using namespace std::chrono_literals;


void Dealer::askForBet(Player& player)
{
    for (size_t i = 0; i < player.individual_number; i++)
    {
        std::string prompt{std::format("[Dealer] {} place your bet in naria[NGN] chips: ", player.getName())};
        m_bank.emplace_back(player.getName(), player.placeBet(prompt));
    }
    
}

void Dealer::shuffleCards()
{
    for (chrono::seconds s = 0s; s < 4s; ++s)
    {
        std::cout << "[Dealer] Shuffling.......\n";
        std::this_thread::sleep_for(3s);
    }
    std::cout << "Done\n";
    m_deck.shuffleDeck();
}

Dealer& Dealer::deal(Player& player, size_t numberofcards)
{
    if (m_static_counter >= m_deck.deck.size())
    {
        std::cout << "[Dealer] No more cards to deal\n";
        return *this;
    }

    while ((numberofcards + 10) >= m_deck.deck.size())
    {
        numberofcards = Myfcn::getInput<size_t>( std::format("[Dealer] Not happening {}! Pick again: ", player.getName()) );

        if ((numberofcards + 10) <= m_deck.deck.size())
        {
            std::cout << "Alright\n";
            break;
        } 
    }

    for (size_t i = 0; i < numberofcards; i++)
    {
        //Dramatic pause.. 
        std::this_thread::sleep_for(2s);

        if (m_deck.deck.at(m_static_counter))
        {
            int choice{};
            while (true)
            { 
                choice = {Myfcn::getInput<int>("[Dealer] You have an Ace! 1 or 11? ")};

                if (choice != 1 and choice != 11)
                std::cout << "Inavild Input; Try Again!\n";
                else
                break;
            }
            if (choice == 1)
            m_deck.deck[m_static_counter].changeAceChoice(Card::AceChoice::one);
        }

        player.getHand().push_back({m_deck.deck.at(m_static_counter)});
        std::cout << "[Dealer] " <<  player.getName() << " has been dealt " << player.getHand().at(m_static_counter) << '\n';

        ++m_static_counter;  
    }
    return *this;
}

Dealer& Dealer::deal(Card::Up_or_Down flag)
{
    if (m_static_counter >= m_deck.deck.size())
    {
        std::cout << "[Dealer] No more cards to draw\n";
        return *this;
    }

    if (!flag)
    m_deck.deck[m_static_counter].m_face = false;

    m_hand.push_back(m_deck.deck[m_static_counter]);
    std::cout << "[Dealer] I have dealt " << m_hand[m_static_counter] << " *by the table\n";

    ++m_static_counter;

    return *this;
}

std::string_view Dealer::checkexitscenarios(const Hand& player) const
{
    auto total{std::reduce(player.begin(), player.end()).getAdditionValue()};

    if (total > 21)
    return HigherThan21;

    else if (total == 21)
    return EqualTo21;

    else
    return "";
}

void Dealer::roundTable(Player& p)
{
    auto answer
    {
        p.getChoice_HitoRStand(std::format("[Dealer] {} do you want another card?\n[{}] ", p.getName(), p.getName() ))
    };

    if (answer == "hit" or answer == "Hit" or answer == "HIT")
    {
        std::cout << "[Dealer] " << p.getName() << " hits\n";
        deal(p, Myfcn::getInput<size_t>("How many? "));
    }
    else
    {
        std::cout << "[Dealer] " << p.getName() << " stands\n";   
    }
}

void Dealer::manageBets(size_t playernumber, IncreaseBy amount)
{
    if (amount == IncreaseBy::fifthy_percent)
    {
        m_bank.at(playernumber - 1).second.get() += m_bank[playernumber - 1].second.get() * 0.5f;
        /*------------------------------------------------------------------------------------*/
        std::cout << 
        std::format("[Dealer] {} here's 1/2times your bet: {} naria chips. Congratulations.\n",
        m_bank.at(playernumber - 1).first.get(), m_bank.at(playernumber - 1).second.get());
    }
    else if (amount == IncreaseBy::hunderd_percent)
    {
        m_bank.at(playernumber - 1).second.get() += m_bank[playernumber - 1].second.get() * 1.f;
        /*------------------------------------------------------------------------------------*/
        std::cout << 
        std::format("[Dealer] {} here's 2times your bet: {} naria chips. Congratulations.\n",
        m_bank.at(playernumber - 1).first.get(), m_bank.at(playernumber - 1).second.get());
    }
}

void Dealer::manageBets(size_t playernumber)
{
    m_bank[playernumber - 1].second.get() = 0.f;
    /*------------------------------------------------------------------------------------*/
    std::cout << 
        std::format("[Dealer] {} you lose your bet: {} naria chips. Sorry.\n", 
        m_bank.at(playernumber - 1).first.get(), m_bank.at(playernumber - 1).second.get());
}

void Dealer::displayHand() const
{
    std::cout << "[Dealer] I have in hand:\n";

    for (auto &&i : m_hand)
    std::cout << i << " value: " << i() << '\n';
}