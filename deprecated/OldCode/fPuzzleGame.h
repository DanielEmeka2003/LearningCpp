#if !defined(FPUZZLEGAME)
#define FPUZZLEGAME
#pragma once

#include "guessGame.h"
#include "customTypes.h"
#include "gfunctions.h"

namespace fiftheenPuzzle
{
    void Introduction(std::string_view username);
    char getUserInput();
    void commandImplementation(Board& board, PuzzleCommands cmd);
    void body();

} // namespace fiftheenPuzzle
#endif // FPUZZLEGAME