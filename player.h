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
    size_t m_individual_number{};

public:

    Player(std::string name);
    Player();

    Player(const Player&) = default;
    Player(Player&&) = default;

    Player& operator=(const Player&) = default;
    Player& operator=(Player&&) = default;
    ~Player() = default; 

private:
    //To be called explicitly in the dealer class.
    float& placeBet(std::string_view prompt);
    //To be called explicitly in the dealer class.
    const std::string& getChoice_HitORStand(std::string_view prompt);
    //Get's player's hand.
    Hand& getHand();
    //Gets a player's bet amount.
    float& getBetAmount();
public:
    //Gets player's name.
    const std::string& getName() const;
    //Get's player's hand.
    const Hand& getHand() const;
    //Displays player's hand.
    void displayHand() const;
    //Gets a player's individual number.
    size_t getIndividualNumber();
    //Gets a player's bet amount.
    float getBetAmount() const;
    

    friend class Dealer;
};




#endif
