#pragma once

#include <vector>
#include "Gamestate.h"
#include "Move.h"
#include "Player.h"

// top level info on the game, who's playing, and how did the games end
class Game { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
private:
    Player* playerWhite;
    Player* playerBlack;
    Player* currentPlayer;
    GameState gameState;
    std::vector<Replay> matchHistory; // when a game concludes it is added here

public:
    void playMove();
};
