#if !defined(SQAUREGAME_H)
#define SQAUREGAME_H

#include "gfunctions.h"
#include "Random.h"
#include "guessGame.h"

namespace squareGame
{
    void displayVector(std::vector<int>& d);

    void succesiveVector(int startElement, std::vector<int>& V);

    void multiplyByRandomNumber(int R, std::vector<int>& container);

    bool checkGuesses(std::vector<int>& C);
    
    void squareGameBody();
       
} // namespace squareGame


#endif // SQAUREGAME_H
