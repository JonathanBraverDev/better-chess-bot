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
    std::vector<Move> moveHistory; // for easy export
    std::vector<GameState> gameHistory; // for 3 time repetition
    // add a hash to help count repeting moves? need a function

    // moves the pieces could make
    std::vector<Move> getPotentialMoves() const;
    void getPawnMoves(std::vector<Move>& moves);
    void getKnightMoves(std::vector<Move>& moves);
    void getBishopMoves(std::vector<Move>& moves) const;
    void getRookMoves(std::vector<Move>& moves);
    void getQueenMoves(std::vector<Move>& moves);
    void getKingMoves(std::vector<Move>& moves);
    void addDestinationMoves(std::vector<Move>& moves, Move baseMove, Bitboard destinations) const;
    void addCaptureMoves(std::vector<Move>& moves, Move baseMove, Bitboard captues) const;

    Color getOpponentColor() const;

public:
    void makeMove(Move move);
    
    std::vector<Move> getLegalMoves() const;

    Bitboard getOwnPieces(PieceType type) const;
    Bitboard getOpponentPieces(PieceType type) const;
    Bitboard getAllOwnPieces() const;
    Bitboard getAllOpponentPieces() const;

    // check check only if moving from any tile a queen can reach from the king (incliding first colision)

    // moves that can be legally played
    std::vector<Move> getLegalMoves(); // update the check for the move itself during this check
};


