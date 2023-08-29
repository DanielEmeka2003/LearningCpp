#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <vector>
#include <string>

#include "card.h"

using namespace std::string_literals;
using Hand = std::vector<Card>;

class Player
{
private:
    std::string m_name{};
    std::string m_choice{};
    float m_betAmount{};
    Hand m_hand{};

public:
    size_t individual_number{};

    Player(std::string name): m_name{name} { ++individual_number; }
    Player(const Player&) = default;
    Player(Player&&) = default;

    Player& operator=(const Player&) = default;
    Player& operator=(Player&&) = default;
    ~Player() {}

private:
    //To be called explicitly in the dealer class.
    float& placeBet(std::string_view prompt);
    //To be called explicitly in the dealer class.
    const std::string& getChoice_HitoRStand(std::string_view prompt);
public:
    //Gets player's name.
    const std::string& getName() const;
    //Get's player's hand.
    Hand& getHand();
    const Hand& getHand() const;
    //Displays player's hand.
    void displayHand() const;

    friend class Dealer;
};




#endif
