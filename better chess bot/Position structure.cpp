
#include "Position structure.h"

void switch_sides(SidedPosition& sided_position) {
    // Switch piece positions
    swap(sided_position.own_pawns, sided_position.opponent_pawns);
    swap(sided_position.own_knights, sided_position.opponent_knights);
    swap(sided_position.own_bishops, sided_position.opponent_bishops);
    swap(sided_position.own_rooks, sided_position.opponent_rooks);
    swap(sided_position.own_queens, sided_position.opponent_queens);
    swap(sided_position.own_king, sided_position.opponent_king);

    // Invert is_white
    sided_position.is_white = !sided_position.is_white;
}

B64* own_piece_board(SidedPosition& sided_position, const PieceType type) {

    B64* piece_board;

    switch (type) {
    case PAWN:
        piece_board = &sided_position.own_pawns;
    case KNIGHT:
        piece_board = &sided_position.own_knights;
    case BISHOP:
        piece_board = &sided_position.own_bishops;
    case ROOK:
        piece_board = &sided_position.own_rooks;
    case QUEEN:
        piece_board = &sided_position.own_queens;
    case KING:
        piece_board = &sided_position.own_king;
    default:
        piece_board = nullptr; // if it gets here, BIG doodoo
    }

    return piece_board;
}