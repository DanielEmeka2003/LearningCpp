#include "blackJack.h"


namespace blackJack
{
    void printCard(const Card& two_letter_code)
    {
        switch (two_letter_code.rank)
        {
            case CRanks::c2:    std::cout << '2'; break;
            case CRanks::c3:    std::cout << '3'; break;
            case CRanks::c4:    std::cout << '4'; break;
            case CRanks::c5:    std::cout << '5'; break;
            case CRanks::c6:    std::cout << '6'; break;
            case CRanks::c7:    std::cout << '7'; break;
            case CRanks::c8:    std::cout << '8'; break;
            case CRanks::c9:    std::cout << '9'; break;
            case CRanks::c10:   std::cout << "10"; break;
            case CRanks::Jack:  std::cout << 'J'; break;
            case CRanks::Queen: std::cout << 'Q'; break;
            case CRanks::King:  std::cout << 'K'; break;
            case CRanks::Ace:   std::cout << 'A'; break;
            default:            std::cout << '?'; break;
        }
    
        switch (two_letter_code.suit)
        {
            case CSuits::clubs :    std::cout << 'C'; break;
            case CSuits::diamonds : std::cout << 'D'; break;
            case CSuits::hearts :   std::cout << 'H'; break;
            case CSuits::spades :   std::cout << 'S'; break;
            default:                std::cout << '?'; break;
        }
        
    }

    
    
} // namespace blackJack


