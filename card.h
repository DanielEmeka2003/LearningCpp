#ifndef CARD_H
#define CARD_H

#pragma once

#include <iostream>
#include <array>



class Card
{
public:
    enum class CardRank {two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, maxRanks};
    enum class CardSuit {clubs, diamonds, hearts, spades, maxSuits};
    enum class AceChoice {one, eleven, maxAceChoice};


private:

    CardRank m_rank{};
    CardSuit m_suit{};
    AceChoice m_choice{AceChoice::eleven};
    int m_sum_value{};
    bool m_faceup{true};

public:

    Card(CardRank rank, CardSuit suit, AceChoice choice = AceChoice::eleven);
    
    Card() = default;

    Card(const Card&) = default;
    Card(Card&&) = default;

    Card& operator=(const Card&) = default;
    Card& operator=(Card&&) = default;
    
    ~Card() = default;

    //Returns the value of a patricualr card.
    int operator()() const;
    //Explicitly converts to a boolean; true for an Ace and false for anything else.
    explicit operator bool() const;
    //Changes Ace card from default value 11 to 1, only to be called you have an Ace.
    void changeAceValue(AceChoice c);
    //Returns the total value of two Card types. Only to called when an addition is performed between to objects of type Card.
    //Example: Card{ Card{ } + Card{ } }.getAdditionValue();
    int getAdditionValue() const;
    //True for a card dealt to be faced_up, false for a card to dealt to be faced_down.
    void changeFace(bool faceup) const;
    //Up_or_Down is an alias for type bool.
    bool getFace() const;

    friend std::ostream& operator<<(std::ostream& out, const Card& temp);
    friend Card operator+(const Card& x, const Card& y);


};

Card::CardRank& operator++(Card::CardRank& r);
Card::CardSuit& operator++(Card::CardSuit& s);

class Deck
{
    using DeckOfCards = std::array<Card, 52U>;
    
    //deck is of alias type DeckOfCards -> std::array<Card, 52U>.
    DeckOfCards m_deck{};

public:
    //Creates Deck of cards.
    Deck();

    Deck(const Deck&) = default;
    Deck(Deck&&) = default;

    Deck& operator=(const Deck&) = default;
    Deck& operator=(Deck&&) = default;

    ~Deck() = default;

    //Shuffles a deck of cards.
    void shuffleDeck();
    //Obtains individual card.
    Card& operator[](size_t index);
    //Obtains individual card.
    const Card& operator[](size_t index) const;
    //Returns the size of deck, always 52.
    size_t size() const;
};
//For displaying a Deck object to the console. 
std::ostream& operator<<(std::ostream& out, const Deck& temp);


#endif