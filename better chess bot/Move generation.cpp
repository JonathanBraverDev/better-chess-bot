
#include "Move generation.h"
#include "Board operation.h"

B64 generate_bishop_moves(const B64 all_pieces, B64 piece) {
    B64 moves = 0;

    moves |= slide_up_left(all_pieces, piece);
    moves |= slide_up_right(all_pieces, piece);
    moves |= slide_down_left(all_pieces, piece);
    moves |= slide_down_right(all_pieces, piece);

    return moves;
}

B64 generate_rook_moves(const B64 all_pieces, B64 piece) {
    B64 moves = 0;

    moves |= slide_up(all_pieces, piece);
    moves |= slide_down(all_pieces, piece);
    moves |= slide_left(all_pieces, piece);
    moves |= slide_right(all_pieces, piece);

    return moves;
}

B64 generate_queen_moves(const B64 all_pieces, B64 piece) {
    B64 moves = 0;

    moves |= generate_bishop_moves(all_pieces, piece);
    moves |= generate_rook_moves(all_pieces, piece);

    return moves;
}

B64 generate_white_pawn_jump(const B64 all_pieces, B64 piece) {
    B64 moves = 0;
    if (!((up(piece) & all_pieces) |
        ((up(up(piece)) & all_pieces)))) {
        moves |= up(up(piece));
    }

    return moves;
}

B64 generate_black_pawn_jump(const B64 all_pieces, B64 piece) {
    B64 moves = 0;
    if (!((down(piece) & all_pieces) |
        ((down(down(piece)) & all_pieces)))) {
        moves |= down(down(piece));
    }

    return moves;
}

void prepare_king_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64; i++) {
        king_moves[i] = up(current_board) |
            down(current_board) |
            left(current_board) |
            right(current_board) |
            up_left(current_board) |
            up_right(current_board) |
            down_left(current_board) |
            down_right(current_board);

        current_board = current_board << 1;
    }
}

void prepare_knight_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64; i++) {
        knight_moves[i] = (((current_board >> 6) | (current_board << 10)) & ~COLUMN_GH) | // when looping around the board on the other side
            (((current_board >> 10) | (current_board << 6)) & ~COLUMN_AB) |
            (((current_board >> 15) | (current_board << 17)) & ~COLUMN_H) |
            (((current_board >> 17) | (current_board << 15)) & ~COLUMN_A);

        current_board = current_board << 1;
    }
}

void prepare_white_pawn_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64 * 2; i += 2) {
        // missing promotion logic
        pawn_moves[i] = up(current_board);
        // pawn jumps require sliding piece logic without final capture
        // missing appassant logic

        pawn_attacks[i] = up_left(current_board) |
                          up_right(current_board);

        current_board = current_board << 1;
    }
}

void prepare_black_pawn_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 1; i < 64 * 2; i += 2) {
        // missing promotion logic
        pawn_moves[i] = down(current_board);
        // pawn jumps require sliding piece logic without final capture
        // missing appassant logic

        pawn_attacks[i] = down_left(current_board) |
                          down_right(current_board);

        current_board = current_board << 1;
        // yes, black pawns can't get to the last row, not in a NORMAL game at least
    }
}

void prepare_pawn_moves() {
    // promotions and en passant will probably be handled by the actual move proccesing, not in generation
    // if a pawn has no POTENTIAL moves, he promoted
    prepare_white_pawn_moves();
    prepare_black_pawn_moves();
}