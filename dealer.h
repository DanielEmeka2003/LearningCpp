#ifndef DEALER_H
#define DEALER_H

#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <utility>

#include "player.h"

class Dealer
{
public:
    enum IncreaseBy{ fifthy_percent, hunderd_percent, max_IncreaseBy };
    using Bank = std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<float>> >; 
    
    static constexpr std::string_view HigherThan21{">21"};
    static constexpr std::string_view EqualTo21{"==21"};

private:
    Bank m_bank{};
    Deck m_deck{};
    size_t m_static_counter{};
    Hand m_hand{};
public:
    Dealer() {m_deck.createDeck();};
    Dealer(const Dealer&) = default;
    Dealer(Dealer&&) = default;

    Dealer& operator=(const Dealer&) = default;
    Dealer& operator=(Dealer&&) = default;
    ~Dealer() {}
    
    //Asks for a player's bet.
    void askForBet(Player& player);
    //Shuffle Cards.
    void shuffleCards();
    //Deals cards to individual players.
    Dealer& deal(Player& player, size_t numberofcards = 1);
    //The dealer deals card to him/her self, flag parameter should be set to true if card should be dealt FACEUP or false for FACEDOWN.
    Dealer& deal(Card::Up_or_Down flag = true);
    /*Checks if exit scenario for in player in a round is met. 
    Returns string_view literal ">21" for an exit scenario of a player's hand greater than one, Or
    string_view literal "==21" for an exit scenario of a player's hand equal to 21*/
    std::string_view checkexitscenarios(const Hand& player) const;
    /*Rounds the table asking each player in a particular round questions like:
    Do you want another card from the top of the deck?*/
    void roundTable(Player& p);
    //Increases a particular player's bet amount incase of a win scenarios.
    void manageBets(size_t playernumber, IncreaseBy amount);
    //Collects a particular player's entire bet amount if lose scenario is met.
    void manageBets(size_t playernumber);
    void displayHand() const;
};


#endif
