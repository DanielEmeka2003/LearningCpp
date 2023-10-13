#ifndef DEALER_H
#define DEALER_H

#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <utility>
#include <chrono>

#include "player.h"

using namespace std::chrono_literals;

class Dealer
{
public:
    enum IncreaseBy{ zero_percent, fifthy_percent, hunderd_percent, maxIncreaseBy };
    
    static constexpr std::string_view HigherThan21{">21"};
    static constexpr std::string_view EqualTo21{"==21"};
    static constexpr std::string_view P_HandGreaterThan_D{"<D"};
    static constexpr std::string_view EqualToAndLessThan16{"<=16"};

private:
    Deck m_deck{};
    size_t m_static_counter{};
    Hand m_hand{};
    
public:
    Dealer() = default;
    Dealer(const Dealer&) = default;
    Dealer(Dealer&&) = default;

    Dealer& operator=(const Dealer&) = default;
    Dealer& operator=(Dealer&&) = default;
    ~Dealer() {}
    
    //Asks for a player's bet.
    void askForBet(Player& p);
    //Shuffle Cards.
    void shuffleCards();
    //Deals cards to individual players.
    Dealer& deal(Player& p, size_t numberofcards = 1);
    //The dealer deals card to him/her self, flag parameter should be set to true if card should be dealt FACEUP or false for FACEDOWN.
    Dealer& deal(bool flag = true);
    /*Checks if possible outcomes for a player is met. 
    Possible return values:
    Returns static member variable HigherThan21 if player's hand is a bust.
    Returns static member variable EqualTo21 if player's hand is equal to 21.
    Returns static member variable P_HandGreaterThan_D if player's hand is less than the dealer's. 
    If non is met, it returns a null string.*/
    std::string_view playerPossibleOutcomes(const Player& p) const;
    /*Checks if possible outcomes for a dealer is met. 
    Possible return values:
    Returns static member variable HigherThan21 if dealer's hand is a bust.
    Returns static member variable EqualToAndLessThan16 if dealer's hand is equal to and less than 16.
    Returns static member variable EqualToAndGreaterThan17 if dealer's hand is equal to and greater than 17.
    If non is met, it returns a null string.*/
    std::string_view possibleOutcomes() const;
    /*Rounds the table asking each player in a particular round questions like:
    Do you want another card from the top of the deck?*/
    void roundTable(Player& p);
    //Increases a particular player's bet amount incase of a win scenarios.
    void manageBets(Player& p, IncreaseBy amount) const;
    //Displays dealer's hand.
    void displayHand() const;
    //Introduces dramatic pause during gameplay to facilitate real world scenarios.
    void dramaticPause(std::chrono::seconds pausetime) const;
};


#endif
