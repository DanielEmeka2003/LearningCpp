#if !defined(GUESSGAME_H)
#define GUESSGAME_H

#pragma once

#include "gfunctions.h"
#include "Random.h"

namespace guess
{
    int getInputs(int c);

    char getChoice(std::string_view choiceMessage = "\t\tWould you like to play again(y/n)? ");
    
    void guessGameIntro(std::string_view intro = " LET'S PLAY A GAME ", std::string_view gamePrompt = "THINKING OF A NUMBER BETWEEN 1 AND 100, WHAT IS IT?");  
    void guessGamerules();
    void guessGamebody();
} // namespace game

#endif // GUESSGAME_H
