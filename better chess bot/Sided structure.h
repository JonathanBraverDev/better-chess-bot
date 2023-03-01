#pragma once

#include "Board structure.h"
#include "Move structure.h"

// this is either a flash of genius or a complete waste of time converting back and forth
// structure with added color semantics
struct SidedPosition {
    // everything here is a 'bitmap of...'
    B64 own_pawns;
    B64 own_knights;
    B64 own_bishops;
    B64 own_rooks;
    B64 own_queens;
    B64 own_king; // no 's' this time, budget cuts
    B64 oppenent_pawns;
    B64 oppenent_knights;
    B64 oppenent_bishops;
    B64 oppenent_rooks;
    B64 oppenent_queens;
    B64 oppenent_king;
    B64 special_move_rigths; // en passant AND castle right bot both sides, data cant overlap anyway
    bool is_white;
};

// error error 
inline SidedPosition conv_to_sided(const BoardPosition board_position, const bool is_white) {
    SidedPosition sided_position;

    sided_position.is_white = is_white;
    sided_position.own_pawns = is_white ? board_position.white_pawns : board_position.black_pawns;
    sided_position.own_knights = is_white ? board_position.white_knights : board_position.black_knights;
    sided_position.own_bishops = is_white ? board_position.white_bishops : board_position.black_bishops;
    sided_position.own_rooks = is_white ? board_position.white_rooks : board_position.black_rooks;
    sided_position.own_queens = is_white ? board_position.white_queens : board_position.black_queens;
    sided_position.own_king = is_white ? board_position.white_king : board_position.black_king;

    sided_position.oppenent_pawns = is_white ? board_position.black_pawns : board_position.white_pawns;
    sided_position.oppenent_knights = is_white ? board_position.black_knights : board_position.white_knights;
    sided_position.oppenent_bishops = is_white ? board_position.black_bishops : board_position.white_bishops;
    sided_position.oppenent_rooks = is_white ? board_position.black_rooks : board_position.white_rooks;
    sided_position.oppenent_queens = is_white ? board_position.black_queens : board_position.white_queens;
    sided_position.oppenent_king = is_white ? board_position.black_king : board_position.white_king;

    sided_position.special_move_rigths = board_position.special_move_rigths;
    return sided_position;
}
// go away
inline BoardPosition convert_from_sided(const SidedPosition sided_position) {
    BoardPosition board_position;

    board_position.white_pawns = sided_position.is_white ? sided_position.own_pawns : sided_position.oppenent_pawns;
    board_position.white_knights = sided_position.is_white ? sided_position.own_knights : sided_position.oppenent_knights;
    board_position.white_bishops = sided_position.is_white ? sided_position.own_bishops : sided_position.oppenent_bishops;
    board_position.white_rooks = sided_position.is_white ? sided_position.own_rooks : sided_position.oppenent_rooks;
    board_position.white_queens = sided_position.is_white ? sided_position.own_queens : sided_position.oppenent_queens;
    board_position.white_king = sided_position.is_white ? sided_position.own_king : sided_position.oppenent_king;

    board_position.black_pawns = sided_position.is_white ? sided_position.oppenent_pawns : sided_position.own_pawns;
    board_position.black_knights = sided_position.is_white ? sided_position.oppenent_knights : sided_position.own_knights;
    board_position.black_bishops = sided_position.is_white ? sided_position.oppenent_bishops : sided_position.own_bishops;
    board_position.black_rooks = sided_position.is_white ? sided_position.oppenent_rooks : sided_position.own_rooks;
    board_position.black_queens = sided_position.is_white ? sided_position.oppenent_queens : sided_position.own_queens;
    board_position.black_king = sided_position.is_white ? sided_position.oppenent_king : sided_position.own_king;
    
    board_position.special_move_rigths = sided_position.special_move_rigths;
    return board_position;
}
// just compile
inline SidedPosition switch_sides(const SidedPosition& position) {
    SidedPosition switched_position;

    switched_position.is_white = !position.is_white;
    switched_position.own_pawns = position.oppenent_pawns;
    switched_position.own_knights = position.oppenent_knights;
    switched_position.own_bishops = position.oppenent_bishops;
    switched_position.own_rooks = position.oppenent_rooks;
    switched_position.own_queens = position.oppenent_queens;
    switched_position.own_king = position.oppenent_king;

    switched_position.oppenent_pawns = position.own_pawns;
    switched_position.oppenent_knights = position.own_knights;
    switched_position.oppenent_bishops = position.own_bishops;
    switched_position.oppenent_rooks = position.own_rooks;
    switched_position.oppenent_queens = position.own_queens;
    switched_position.oppenent_king = position.own_king;

    switched_position.special_move_rigths = position.special_move_rigths;
    return switched_position;
}
// i'm tired
inline B64* own_piece_board(SidedPosition& sided_position, const PieceType type) {

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

inline B64 own_pieces(const SidedPosition sided_position) {
    return (sided_position.own_pawns | sided_position.own_knights | sided_position.own_bishops | sided_position.own_rooks | sided_position.own_queens | sided_position.own_king);
}

inline B64 opponent_pieces(const SidedPosition sided_position) {
    return (sided_position.oppenent_pawns | sided_position.oppenent_knights | sided_position.oppenent_bishops | sided_position.oppenent_rooks | sided_position.oppenent_queens | sided_position.oppenent_king);
}

inline B64 all_pieces(const SidedPosition sided_position) { return (own_pieces(sided_position) | opponent_pieces(sided_position)); }