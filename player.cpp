#include <format>
#include <sstream>
#include <numeric>

#include "player.h"
#include "myfunctions1.h"

Player::Player(std::string name): m_name{name} 
{ 
    ++m_individual_number; 
}
Player::Player(): m_name{"Unknown"}
{
    std::stringstream ss{};
    ss << ++m_individual_number;
    m_name += ss.str();
}

float& Player::placeBet(std::string_view prompt)
{
    return m_betAmount = Myfcn::getInput<float>(prompt);
}

const std::string& Player::getChoice_HitORStand(std::string_view prompt)
{
    while (true)
    {
        m_choice = Myfcn::getInput<std::string>(prompt);

        if ((m_choice != "hit" and m_choice != "Hit" and m_choice != "HIT") and (m_choice != "stand" and m_choice != "Stand" and m_choice != "STAND"))
        std::cout << "Choice is invalid; Try Again!\n";
        else
        return m_choice;
    }
    
    return m_choice;
}

Hand& Player::getHand()
{
    return m_hand;
}

float& Player::getBetAmount()
{
    return m_betAmount;
}

const std::string& Player::getName() const
{
    return m_name;
}

const Hand& Player::getHand() const
{
    return m_hand;
}

void Player::displayHand() const
{
    std::cout << std::format("[{}] I have in hand:\n", m_name);

    for (auto &&i : m_hand)
    std::cout << i << "\tvalue: " << i() << '\n';

    std::cout << "Total\tvalue: " << std::reduce(m_hand.cbegin(), m_hand.cend()).getAdditionValue() << '\n';
}

float Player::getBetAmount() const
{
    return m_betAmount;
}

size_t Player::getIndividualNumber()
{
    return m_individual_number;
}