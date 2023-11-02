#pragma once

#include <vector>
#include "Board.h"
#include "Enums.h"

// GameState contains info that isn't inferable from the board alone, but is also agnostic to the active players
// it's like a joining spectator asking "who's turn is it?", "can white castle long?" or "did that pawn just jump?"
class GameState {
private:
    Board board;
    Bitboard special_move_rigths; // en passant AND castle rights for both sides, they can't overlap anyway
    Color current_color;
    int moveCounter; // used to count moves until the forced draw
    bool isCheck;

public:
    void makeMove(Move move);
    
    std::vector<Move> getLegalMoves() const;

    Bitboard getOwnPieces(Color color, PieceType type) const;
    Bitboard getOpponentPieces(Color color, PieceType type) const;

    // check check only if moving from any tile a queen can reach from the king (incliding first colision)
};


