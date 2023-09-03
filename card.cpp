#include <algorithm>

#include "card.h"
#include "random.h"

namespace ranges = std::ranges;


/*----------------Enum CardRank & CardSuit---------------------*/
Card::CardRank& operator++(Card::CardRank& r)
{
    if (r == Card::CardRank::maxRanks)
    return r = Card::CardRank::maxRanks;

    return r = static_cast<Card::CardRank>(static_cast<int>(r) + 1);
}
Card::CardSuit& operator++(Card::CardSuit& s)
{
    if (s == Card::CardSuit::maxSuits)
    return s = Card::CardSuit::maxSuits;

    return s = static_cast<Card::CardSuit>(static_cast<int>(s) + 1);
}

/*---------------------Card----------------------------------*/
Card::Card(CardRank rank, CardSuit suit, AceChoice choice)
: m_rank{rank}, m_suit{suit}, m_choice{choice}
{
    m_sum_value = this->operator()();
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


void Card::changeAceValue(AceChoice c)
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

void Card::changeFace(bool faceup) const
{
    const_cast<bool&>(m_faceup) = faceup;
}

bool Card::getFace() const
{
    return m_faceup;
}

std::ostream& operator<<(std::ostream& out, const Card& temp)
{
    if (temp.m_faceup)
    {
        switch (temp.m_rank)
        {
        case Card::CardRank::two :    out << "[2 "; break;
        case Card::CardRank::three :  out << "[3 "; break;
        case Card::CardRank::four :   out << "[4 "; break;
        case Card::CardRank::five :   out << "[5 "; break;
        case Card::CardRank::six :    out << "[6 "; break;
        case Card::CardRank::seven :  out << "[7 "; break;
        case Card::CardRank::eight :  out << "[8 "; break;
        case Card::CardRank::nine :   out << "[9 "; break;
        case Card::CardRank::ten :    out << "[10"; break;
        case Card::CardRank::jack :   out << "[J "; break;
        case Card::CardRank::queen :  out << "[Q "; break;
        case Card::CardRank::king :   out << "[K "; break;
        case Card::CardRank::ace :    out << "[A "; break;
        default: out << "[N "; break;
        }
        switch (temp.m_suit)
        {
        case Card::CardSuit::clubs :      out << "\u2663]"; break;
        case Card::CardSuit::diamonds :   out << "\u2666]"; break;
        case Card::CardSuit::hearts :     out << "\u2665]"; break;
        case Card::CardSuit::spades :     out << "\u2660]"; break;  
        default: out << "/]"; break;
        }
    }
    else
    {
        out << "[FD]";
    }
    return out;
}

Card operator+(const Card& x, const Card& y)
{
    Card temp{}; 
    temp.m_sum_value = x.m_sum_value + y.m_sum_value;
    return temp;
}

/*------------------------DECK------------------------------*/

Deck::Deck()
{
    size_t static_counter{};
    for (Card::CardSuit s{}; s < Card::CardSuit::maxSuits; ++s)
    {
        for (Card::CardRank r{}; r < Card::CardRank::maxRanks; ++r)
        {
            m_deck.at(static_counter++) = {r, s};
        }
    }
}

void Deck::shuffleDeck()
{
    ranges::shuffle(m_deck, Myfcn::Random::mt);
}

Card& Deck::operator[](size_t index)
{
    return m_deck.at(index);
}

const Card& Deck::operator[](size_t index) const
{
    return m_deck.at(index);
}

size_t Deck::size() const
{
    return m_deck.size();
}

std::ostream& operator<<(std::ostream& out, const Deck& temp)
{
    for (size_t i = 0; i < temp.size(); i++)
    {
        out << temp[i] << '\t';

        if ( (i+1) % 13 == 0 )
        out << '\n';
    }
    
    return out;
}
