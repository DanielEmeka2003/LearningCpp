#include <format>

#include "player.h"
#include "myfunctions1.h"

float& Player::placeBet(std::string_view prompt)
{
    return m_betAmount = Myfcn::getInput<float>(prompt);
}

const std::string& Player::getName() const
{
    return m_name;
}

const std::string& Player::getChoice_HitoRStand(std::string_view prompt)
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
const Hand& Player::getHand() const
{
    return m_hand;
}

void Player::displayHand() const
{
    std::cout << std::format("[{}] I have in hand:\n", m_name);

    for (auto &&i : m_hand)
    std::cout << i << " value: " << i() << '\n';
}
