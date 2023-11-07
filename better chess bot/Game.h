#pragma once

#include <vector>
#include "Gamestate.h"
#include "Move.h"
#include "Player.h"

// info on gamestate
class Game { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
private:
    Player* playerWhite;
    Player* playerBlack;
    Player* currentPlayer;
    GameState gameState;
    std::vector<Replay> matchHistory; // when a game concludes it is added here

public:
    void makeMove();
};
