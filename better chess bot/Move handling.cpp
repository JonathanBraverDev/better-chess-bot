#include "Move handling.h"

BoardPosition Make_move(BoardPosition position, Move move) {
        
    // Clear the origin square
    position.white &= ~move.origin;
    position.white_pawns &= ~move.origin;
    position.white_bishops &= ~move.origin;
    position.white_knights &= ~move.origin;
    position.white_rooks &= ~move.origin;
    position.white_queens &= ~move.origin;
    position.white_king &= ~move.origin;
    position.black &= ~move.origin;
    position.black_pawns &= ~move.origin;
    position.black_bishops &= ~move.origin;
    position.black_knights &= ~move.origin;
    position.black_rooks &= ~move.origin;
    position.black_queens &= ~move.origin;
    position.black_king &= ~move.origin;

    // Set the destination square
    position.white |= move.destination & (~position.black);
    position.white_pawns |= move.destination & (~position.black_pawns);
    position.white_bishops |= move.destination & (~position.black_bishops);
    position.white_knights |= move.destination & (~position.black_knights);
    position.white_rooks |= move.destination & (~position.black_rooks);
    position.white_queens |= move.destination & (~position.black_queens);
    position.white_king |= move.destination & (~position.black_king);
    position.black |= move.destination & (~position.white);
    position.black_pawns |= move.destination & (~position.white_pawns);
    position.black_bishops |= move.destination & (~position.white_bishops);
    position.black_knights |= move.destination & (~position.white_knights);
    position.black_rooks |= move.destination & (~position.white_rooks);
    position.black_queens |= move.destination & (~position.white_queens);
    position.black_king |= move.destination & (~position.white_king);
}

BoardPosition Undo_move(BoardPosition position, Move move) {
    return Make_move(position, Invert_move(move));
}
