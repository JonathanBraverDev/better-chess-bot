#pragma once

#include <vector>
#include "Bitboard.h"
#include "Move.h"
#include "Enums.h"

// all the needed information to make a legal move
class Position {
private:
    Bitboard white_pawns;
    Bitboard white_knights;
    Bitboard white_bishops;
    Bitboard white_rooks;
    Bitboard white_queens;
    Bitboard white_king; // no 's' this time, budget cuts
    Bitboard black_pawns;
    Bitboard black_knights;
    Bitboard black_bishops;
    Bitboard black_rooks;
    Bitboard black_queens;
    Bitboard black_king; // I swear it's not just discrimination

    Bitboard special_move_rigths; // en passant AND castle rights for both sides, they can't overlap anyway
    Color current_color;

    Color getOpponentColor() const;

    // moves the pieces could make
    std::vector<Move> getPotentialMoves() const;
    void getPawnMoves(std::vector<Move>& moves) const;
    void getKnightMoves(std::vector<Move>& moves) const;
    void getSlidingPieceMoves(std::vector<Move>& moves, const PieceType pieceType) const;
    void getBishopMoves(std::vector<Move>& moves) const;
    void getRookMoves(std::vector<Move>& moves) const;
    void getQueenMoves(std::vector<Move>& moves) const;
    void getKingMoves(std::vector<Move>& moves) const;
    void addDestinationMoves(std::vector<Move>& moves, Move baseMove, Bitboard destinations) const;
    void addCaptureMoves(std::vector<Move>& moves, Move baseMove, Bitboard captues) const;

public:
    void makeMove(Move move);

    Bitboard getPieces(Color color, PieceType type) const;
    Piece getPieceAtIndex(int index) const;
    bool validate() const;

    std::vector<Move> getLegalMoves() const;

    Bitboard getOwnPieces(PieceType type) const;
    Bitboard getOpponentPieces(PieceType type) const;
    Bitboard getAllOwnPieces() const;
    Bitboard getAllOpponentPieces() const;
};