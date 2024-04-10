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
    // use the bitboard to calculate BitRights and use for all moves from the position
    Color current_color;

    // greatly reducing cluttered calls during move generation
    // remember to wipe when ANYTHING changes on the board
    Bitboard own_pieces;
    Bitboard opponent_pieces;
    std::vector<Move> legal_moves;
    bool are_moves_valid;

    static PrecomputedMoves precomputed_moves;

    Color getOpponentColor() const;

    // moves the pieces could make
    void getPawnMoves();
    void checkAndAddPawnJump();
    void checkAndAddEnPassant();
    void addPromotionMoves();
    void addNormalPawnMoves(Move base_move, Bitboard step, Bitboard captures);
    void getKnightMoves();
    void getKingMoves();
    inline void getBishopMoves();
    inline void getRookMoves();
    inline void getQueenMoves();
    void getSlidingPieceMoves(const PieceType pieceType);
    Bitboard getSlideDestinations(const Bitboard piece, const PieceType pieceType) const;

    void finalizeMoves(Bitboard destinations, Move move_base);
    void addDestinationMoves(Bitboard destinations, Move move_base);
    void addCaptureMoves(Bitboard captures, Move move_base);

    void CheckAndSaveMove(Move proposed_move);
    bool selfCheckCheck(Move proposed_move) const;
    bool isAttackedBySlidePattern(Bitboard target, PieceType pattern, Bitboard blockers) const;
    bool isAttackedByJumpPattern(uint8_t target_index, PieceType pattern) const;
    bool enemyCheckCheck(Move proposed_move) const;

    static void PrepareKingMoves();
    static void PrepareKnightMoves();
    static void PrepareWhitePawnMoves();
    static void PrepareBlackPawnMoves();

public:
    // allow initialization from string

    // constructor that takes a vector of moves and created a position following them

    void makeMove(Move move);

    Bitboard getPieces(Color color, PieceType type) const;
    Piece getPieceAtIndex(uint8_t index) const;
    Piece getPieceAtTile(Bitboard tile) const;

    std::vector<Move> getLegalMoves();

    Bitboard getOwnPieces(PieceType type) const;
    Bitboard getOpponentPieces(PieceType type) const;
    Bitboard getAllOwnPieces();
    Bitboard getAllOpponentPieces();
    Bitboard getAllPieces();

    static void InitializeMoves();

    // Converts the special move board to a bitmove containig the appropriate BitRights
    BitMove currentBitRights();
};
