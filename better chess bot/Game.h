#pragma once

#include <vector>
#include "Board.h"
#include "Move.h"
#include "Player.h"

// info on gamestate
class Game { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
private:
    Player* playerWhite;
    Player* playerBlack;
    Player* currentPlayer;
    int moveCounter; // used to count moves untill the forced draw
    Board board;
    std::vector<Move> moveHistory; // for easy export
    std::vector<Board> boardHistory; // for 3 time repetition

public:
    void makeMove();
};
