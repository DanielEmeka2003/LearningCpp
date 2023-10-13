#if !defined(LOGICGAME_H)
#define LOGICGAME_H

#pragma once

#include "gfunctions.h"
#include "Random.h"
#include "guessGame.h"
#include "customTypes.h"

namespace logicGame
{
    using TurnForAttack = bool;

    void introduction(std::string_view player_name);
    void rules();
    char getChoice();
    char output(Monster& m);

    // TurnForAttack: true if it's player's turn for attack, false if it's monster's.
    void attack(Player& p, Monster& m, TurnForAttack a);

    void levelUp(Player& p);
    void collectGold(Player& p, Monster& m);

    void logicGameBody();

} // namespace logicGame



#endif // LOGICGAME_H
