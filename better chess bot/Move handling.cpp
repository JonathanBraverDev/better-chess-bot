#include "Move handling.h"
#include "Board operation.h"

BoardPosition Make_move(BoardPosition position, Move move) {

	if (move.piece.color == WHITE) {
		position.white ^= move.origin;
		position.white |= move.destination;
	} else {
		position.black ^= move.origin;
		position.black |= move.destination;
	}

	switch (move.piece.type) {
	case PAWN:
		if (move.piece.color == WHITE) {
			position.white_pawns ^= move.origin;
			position.white_pawns |= move.destination;
		} else {
			position.black_pawns ^= move.origin;
			position.black_pawns |= move.destination;
		}
		break;
	case KNIGHT:
		if (move.piece.color == WHITE) {
			position.white_knights ^= move.origin;
			position.white_knights |= move.destination;
		} else {
			position.black_knights ^= move.origin;
			position.black_knights |= move.destination;
		}
		break;
	case BISHOP:
		if (move.piece.color == WHITE) {
			position.white_bishops ^= move.origin;
			position.white_bishops |= move.destination;
		} else {
			position.black_bishops ^= move.origin;
			position.black_bishops |= move.destination;
		}
		break;
	case ROOK:
		if (move.piece.color == WHITE) {
			position.white_rooks ^= move.origin;
			position.white_rooks |= move.destination;
		} else {
			position.black_rooks ^= move.origin;
			position.black_rooks |= move.destination;
		}
		break;
	case QUEEN:
		if (move.piece.color == WHITE) {
			position.white_queens ^= move.origin;
			position.white_queens |= move.destination;
		} else {
			position.black_queens ^= move.origin;
			position.black_queens |= move.destination;
		}
		break;
	case KING:
		if (move.piece.color == WHITE) {
			position.white_queens ^= move.origin;
			position.white_queens |= move.destination;
		} else {
			position.black_queens ^= move.origin;
			position.black_queens |= move.destination;
		}
		break;
	}
}

Move Invert_move(Move move)
{
	std::swap(move.origin, move.destination);
	return move;
}

BoardPosition Undo_move(BoardPosition position, Move move) {
	return Make_move(position, Invert_move(move));
}
