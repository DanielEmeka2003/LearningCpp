#ifndef CARD_H
#define CARD_H

#pragma once

#include <iostream>
#include <array>

enum class CardRank {two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, maxRanks};
enum class CardSuit {clubs, diamonds, hearts, spades, maxSuits};

CardRank& operator++(CardRank& r);
CardSuit& operator++(CardSuit& s);

class Card
{
public:
    using Up_or_Down = bool;
    enum class AceChoice {one, eleven, maxAceChoice};

    Up_or_Down m_face{true};

private:

    CardRank m_rank{};
    CardSuit m_suit{};
    AceChoice m_choice{AceChoice::eleven};
    int m_sum_value{};

public:

    Card(CardRank rank, CardSuit suit, AceChoice choice = AceChoice::eleven)
    : m_rank{rank}, m_suit{suit}, m_choice{choice}
    {
        m_sum_value = this->operator()();
    }
    
    Card(): Card{CardRank::two, CardSuit::clubs}
    {}

    Card(const Card&) = default;
    Card(Card&&) = default;

    Card& operator=(const Card&) = default;
    Card& operator=(Card&&) = default;
    
    ~Card() = default;

    //Returns the value of a patricualr card.
    int operator()() const;
    operator bool() const;
    //Changes Ace card from default value 11 to 1, only to be called you have an Ace.
    void changeAceChoice(AceChoice c);
    //Returns the total value of two Card types. Only to called when an addition is performed between to objects of type Card.
    //Example: Card{ Card{ } + Card{ } }.getAdditionValue;
    int getAdditionValue() const;
    
    friend std::ostream& operator<<(std::ostream& out, const Card& temp);
    friend Card operator+(const Card& x, const Card& y);
    
};

struct Deck
{
    using DeckOfCards = std::array<Card, 52U>;
    
    //deck is of alias type DeckOfCards -> std::array<Card, 52U>.
    DeckOfCards deck{};

    //Creates a deck of cards.
    void createDeck();
    //Shuffles a deck of cards.
    void shuffleDeck();
    friend std::ostream& operator<<(std::ostream& out, const Deck& temp);
};


#endif