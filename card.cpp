#include <algorithm>

#include "card.h"
#include "random.h"

namespace ranges = std::ranges;


/*----------------Enum CardRank & CardSuit---------------------*/
CardRank& operator++(CardRank& r)
{
    return r = static_cast<CardRank>(static_cast<int>(r) + 1);
}
CardSuit& operator++(CardSuit& s)
{
    return s = static_cast<CardSuit>(static_cast<int>(s) + 1);
}

/*---------------------Card----------------------------------*/
std::ostream& operator<<(std::ostream& out, const Card& temp)
{
    if (temp.m_face)
    {
        switch (temp.m_rank)
        {
        case CardRank::two :    out << '2'; break;
        case CardRank::three :  out << '3'; break;
        case CardRank::four :   out << '4'; break;
        case CardRank::five :   out << '5'; break;
        case CardRank::six :    out << '6'; break;
        case CardRank::seven :  out << '7'; break;
        case CardRank::eight :  out << '8'; break;
        case CardRank::nine :   out << '9'; break;
        case CardRank::ten :    out << "10"; break;
        case CardRank::jack :   out << 'J'; break;
        case CardRank::queen :  out << 'Q'; break;
        case CardRank::king :   out << 'K'; break;
        case CardRank::ace :    out << 'A'; break;
        default: return out; break;
        }
        switch (temp.m_suit)
        {
        case CardSuit::clubs :      out << " \u2663"; break;
        case CardSuit::diamonds :   out << " \u2666"; break;
        case CardSuit::hearts :     out << " \u2665"; break;
        case CardSuit::spades :     out << " \u2660"; break;  
        default: return out; break;
        }
    }
    else
    {
        out << "[FD]";
    }
    return out;
}

int Card::operator()() const
{
    switch (m_rank)
    {
    case CardRank::two :    return 2;
    case CardRank::three :  return 3;
    case CardRank::four :   return 4;
    case CardRank::five :   return 5;
    case CardRank::six :    return 6;
    case CardRank::seven :  return 7;
    case CardRank::eight :  return 8;
    case CardRank::nine :   return 9;
    case CardRank::ten :    return 10;
    case CardRank::jack :   return 10;
    case CardRank::queen :  return 10;
    case CardRank::king :   return 10;
    case CardRank::ace :    return ( (m_choice == AceChoice::eleven)? 11 : 1 );
    default: return 0;
    }
}

Card::operator bool() const
{
    if (m_rank == CardRank::ace)
    return true; 
    else
    return false;
}

void Card::changeAceChoice(AceChoice c)
{
    if (m_rank == CardRank::ace)
    {
        m_choice = c;
        m_sum_value = this->operator()();    
    }
}

int Card::getAdditionValue() const
{
    return m_sum_value;
}

Card operator+(const Card& x, const Card& y)
{
    Card temp{}; 
    temp.m_sum_value = x.m_sum_value + y.m_sum_value;
    return temp;
}

/*------------------------DECK------------------------------*/
std::ostream& operator<<(std::ostream& out, const Deck& temp)
{
    for (size_t i = 0; i < temp.deck.size(); i++)
    {
        out << temp.deck[i] << '\t';

        if ( (i+1) % 13 == 0 )
        out << '\n';
    }
    
    return out;
}

void Deck::createDeck()
{
    size_t static_counter{};
    for (CardSuit s{}; s < CardSuit::maxSuits; ++s)
    {
        for (CardRank r{}; r < CardRank::maxRanks; ++r)
        {
            deck.at(static_counter++) = {r, s};
        }
    }
}

void Deck::shuffleDeck()
{
    ranges::shuffle(deck, Random::mt);
}

