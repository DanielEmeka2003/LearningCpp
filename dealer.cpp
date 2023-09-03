#include <thread>
#include <format>
#include <numeric>

#include "dealer.h"
#include "myfunctions1.h"


namespace chrono = std::chrono;


void Dealer::askForBet(Player& p)
{
    std::string prompt{std::format("[Dealer] {}\tplace your bet in naria chips \u20A6: ", p.getName())};
    p.placeBet(prompt);
}

void Dealer::shuffleCards()
{
    for (chrono::seconds s = 0s; s < 4s; ++s)
    {
        std::cout << "[Dealer] Shuffling.......\n";
        dramaticPause(3s);
    }
    std::cout << "Done\n";
    m_deck.shuffleDeck();
}

Dealer& Dealer::deal(Player& p, size_t numberofcards)
{
    if (m_static_counter >= m_deck.size())
    {
        std::cout << "[Dealer] No more cards to deal\n";
        return *this;
    }

    while ((numberofcards + 10) >= m_deck.size())
    {
        numberofcards = Myfcn::getInput<size_t>( std::format("[Dealer] Not happening {}! Pick again: ", p.getName()) );

        if ((numberofcards + 10) <= m_deck.size())
        {
            std::cout << "Alright\n";
            break;
        } 
    }

    for (size_t i = 0; i < numberofcards; i++)
    {
        //Dramatic pause.. 
        dramaticPause(2s);

        if ( m_deck[m_static_counter] )
        {
            int choice{};
            while (true)
            { 
                choice = { Myfcn::getInput<int>(std::format("[Dealer] {}!\tYou have an Ace, 1 or 11? ", p.getName()) ) };

                if (choice != 1 and choice != 11)
                std::cout << "Inavild Input; Try Again!\n";
                else
                break;
            }
            if (choice == 1)
            m_deck[m_static_counter].changeAceValue(Card::AceChoice::one);

        }
        
        p.getHand().push_back(m_deck[m_static_counter]);
        std::cout << "[Dealer] " <<  p.getName() << "\thas been dealt " << p.getHand().back() << '\n';

        ++m_static_counter;  
    }
    return *this;
}

Dealer& Dealer::deal(bool flag)
{
    if (m_static_counter >= m_deck.size())
    {
        std::cout << "[Dealer] No more cards to draw\n";
        return *this;
    }

    if (!flag)
    m_deck[m_static_counter].changeFace(false);

    //Dramatic pause.. 
    dramaticPause(3s);

    m_hand.push_back(m_deck[m_static_counter]);
    std::cout << "[Dealer] drawn " << m_hand.back() << " (*by) the table\n";

    ++m_static_counter;

    return *this;
}

std::string_view Dealer::playerPossibleOutcomes(const Player& p) const
{
    auto total{std::reduce(p.getHand().cbegin(), p.getHand().cend()).getAdditionValue()};

    if (total > 21)
    return HigherThan21;
    else if (total == 21)
    return EqualTo21;
    else if( total > std::reduce(m_hand.cbegin(), m_hand.cend()).getAdditionValue() )
    return P_HandGreaterThan_D;
    else
    return "";
}

std::string_view Dealer::possibleOutcomes() const
{
    auto total{std::reduce(m_hand.cbegin(), m_hand.cend()).getAdditionValue()};

    if (total > 21)
    return HigherThan21;
    else if (total <= 16)
    return EqualToAndLessThan16;
    else
    return "";
}

void Dealer::roundTable(Player& p)
{
    auto answer
    {
        p.getChoice_HitORStand(std::format("[Dealer] {}\tdo you want another card?\n[{}] ", p.getName(), p.getName() ))
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

void Dealer::manageBets(Player& p, IncreaseBy amount) const
{
    if (amount == IncreaseBy::fifthy_percent)
    {
        p.getBetAmount() += p.getBetAmount() * 0.5f;
        /*------------------------------------------------------------------------------------*/
        std::cout << 
        std::format("[Dealer] {}\there's 1/2times your bet: \u20A6{} naria chips. Congratulations.\n", p.getName(), p.getBetAmount());
    }
    else if (amount == IncreaseBy::hunderd_percent)
    {
        p.getBetAmount() += p.getBetAmount() * 1.f;
        /*------------------------------------------------------------------------------------*/
        std::cout << 
        std::format("[Dealer] {}\there's 2times your bet: \u20A6{} naria chips. Congratulations.\n", p.getName(), p.getBetAmount());
    }
    else if (amount == IncreaseBy::zero_percent)
    {
        p.getBetAmount() = 0.f;
        /*------------------------------------------------------------------------------------*/
        std::cout << 
        std::format("[Dealer] {}\tyou lose your bet: \u20A6{} naria chips. Sorry.\n", p.getName(), p.getBetAmount());
    }
    
}

void Dealer::displayHand() const
{
    std::cout << "[Dealer] I have in hand:\n";

    for (auto &&i : m_hand)
    {
        if (i.getFace() == false)
        i.changeFace(true);

        std::cout << i << "\tvalue: " << i() << '\n';
    }
    std::cout << "Total\tvalue: " << std::reduce(m_hand.cbegin(), m_hand.cend()).getAdditionValue() << '\n';
}

void Dealer::dramaticPause(std::chrono::seconds pausetime) const
{
    std::this_thread::sleep_for(pausetime);
}