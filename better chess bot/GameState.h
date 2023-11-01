#pragma once

#include "Board.h"
#include "Enums.h"

// GameState contains info that isn't inferable from the board alone, but is also agnostic to the active players
// it's like a joining spectator asking "who's turn is it?", "can white castle long?" or "did that pawn just jump?"
class GameState {
private:
    Board board;
    Bitboard special_move_rigths; // en passant AND castle rights for both sides, they can't overlap anyway
    Color current_color;
    int moveCounter; // used to count moves untill the forced draw

public:
    void makeMove(Move move);

};


