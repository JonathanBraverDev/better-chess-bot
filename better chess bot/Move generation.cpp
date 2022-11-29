
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