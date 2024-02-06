#pragma once

#include <vector>
#include "Bitboard.h"
#include "Move.h"
#include "Structs.h"

// out of structs.h cuse codependancy mess
struct PrecomputedMoves {
    Bitboard king_moves[64];
    Bitboard knight_moves[64];
    Bitboard pawn_moves[64 * 2];
    Bitboard pawn_attacks[64 * 2];
};

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

    // collection of moves possible from the position
    std::vector<Move> moves;

    static PrecomputedMoves precomputed_moves;

    Color getOpponentColor() const;

    // moves the pieces could make
    std::vector<Move> getPotentialMoves();
    void getPawnMoves();
    void getKnightMoves();
    void getSlidingPieceMoves(const PieceType pieceType);
    void getBishopMoves();
    void getRookMoves();
    void getQueenMoves();
    void getKingMoves();
    void addDestinationMoves(Move baseMove, Bitboard destinations);
    void addCaptureMoves(Move baseMove, Bitboard captues);

    static void PrepareKingMoves();
    static void PrepareKnightMoves();
    static void PrepareWhitePawnMoves();
    static void PrepareBlackPawnMoves();

public:
    // allow initialization from string

    void makeMove(Move move);

    Bitboard getPieces(Color color, PieceType type) const;
    Piece getPieceAtIndex(int index) const;

    // check that the position representation is not messed up
    bool validate() const;

    std::vector<Move> getLegalMoves() const;

    Bitboard getOwnPieces(PieceType type) const;
    Bitboard getOpponentPieces(PieceType type) const;
    Bitboard getAllOwnPieces() const;
    Bitboard getAllOpponentPieces() const;

    static void InitializeMoves();
};
