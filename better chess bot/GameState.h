#pragma once

#include <vector>
#include "Position.h"
#include "Enums.h"
#include "Replay.h"

// GameState contains info that isn't inferable from the board alone, but is also agnostic to the active players
// it's like a joining spectator asking "who's turn is it?", "can white castle long?" or "did that pawn just jump?"
class GameState {
private:
    Position position;
    int moveCounter; // used to count moves until the forced draw
    bool isCheck;
    Replay history;
    // add a hash to sort out repeting moves, also need a function to hash the position

public:
    void makeMove(Move move);

    // check check only if moving from any tile a queen can reach from the king (incliding first colision)

    // moves that can be legally played
    std::vector<Move> getLegalMoves() const; // update the check for the move itself during this check
};
