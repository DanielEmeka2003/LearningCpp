#if !defined(BLACKJACK_H)
#define BLACKJACK_H

#pragma once

#include "functions.h"
#include "Random.h"
#include "customTypes.h"

namespace blackJack
{
    template <Sizeofarray size>
    void shuffleDeck(std::array<Card, size>& shuffle)
    {
        std::shuffle(shuffle.begin(), shuffle.end(), random::random_numbers); 
    }

    void printCard(const Card& two_letter_code);

    template <Sizeofarray size>
    void deckPrint(const std::array<Card, size>& output)
    {
        for (auto &&i : output)
        {
            printCard(i);
            std::cout << ' ';
        }

    }
    
    inline const auto& createDeck()
    {
        static std::array<Card, 52> Deck{};
        int suites{4};
        int ranks{(static_cast<int> (CRanks::Ace) + 1)};
        int k{0};

        for (int i{0}; i < suites; ++i)
        {
            for (int j{0}; j < ranks; ++j, ++k)
            {
                Deck.at( (size_t) k).suit = static_cast<CSuits> (i);
                Deck.at( (size_t) k).rank = static_cast<CRanks> (j);
            }

        }
        return Deck; 
    }

} // namespace blackJack


#endif // BLACKJACK_H
