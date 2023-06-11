
#include "Position structure.h"

void switch_sides(SidedPosition& sided_position) {
    // Switch piece positions
    swap(sided_position.own_pawns, sided_position.opponent_pawns);
    swap(sided_position.own_knights, sided_position.opponent_knights);
    swap(sided_position.own_bishops, sided_position.opponent_bishops);
    swap(sided_position.own_rooks, sided_position.opponent_rooks);
    swap(sided_position.own_queens, sided_position.opponent_queens);
    swap(sided_position.own_king, sided_position.opponent_king);
    sided_position.special_move_rigths = rotate180(sided_position.special_move_rigths);

    // Invert player
    sided_position.is_white = !sided_position.is_white;
}

B64* own_piece_board_ref(SidedPosition& sided_position, const PieceType type) {

    B64* piece_board;

    switch (type) {
    case PAWN:
        piece_board = &sided_position.own_pawns;
        break;
    case KNIGHT:
        piece_board = &sided_position.own_knights;
        break;
    case BISHOP:
        piece_board = &sided_position.own_bishops;
        break;
    case ROOK:
        piece_board = &sided_position.own_rooks;
        break;
    case QUEEN:
        piece_board = &sided_position.own_queens;
        break;
    case KING:
        piece_board = &sided_position.own_king;
        break;
    default:
        piece_board = nullptr; // if it gets here, BIG doodoo
        break;
    }

    return piece_board;
}

B64 own_piece_board_copy(const SidedPosition& sided_position, const PieceType type) {

    B64 piece_board;

    switch (type) {
    case PAWN:
        piece_board = sided_position.own_pawns;
        break;
    case KNIGHT:
        piece_board = sided_position.own_knights;
        break;
    case BISHOP:
        piece_board = sided_position.own_bishops;
        break;
    case ROOK:
        piece_board = sided_position.own_rooks;
        break;
    case QUEEN:
        piece_board = sided_position.own_queens;
        break;
    case KING:
        piece_board = sided_position.own_king;
        break;
    default:
        piece_board = NULL; // if it gets here, BIG doodoo too
        break;
    }

    return piece_board;
}