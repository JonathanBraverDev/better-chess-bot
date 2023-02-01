#include "Move handling.h"
#include "Board operation.h"

// makes a given move, does not check its validity
BoardPosition Make_move(BoardPosition position, Move move) {

	switch (move.type) {
	case CASTLE_SHORT:
	case CASTLE_LONG:
		position = Castle(position, move);
		break;

	case PROMOTION:
		position = Move_piece(position, move);
		position = Promote(position, move);
		break;

	case CAPTURE:
		position = Move_piece(position, move);
		position = Delete_captured(position, move);
		break;

	case PROMOTION_CAPTURE:
		position = Move_piece(position, move);
		position = Delete_captured(position, move);
		position = Promote(position, move);
		break;

	case NORMAL:
		position = Move_piece(position, move);
		break;
	}

	return position;
}

BoardPosition Castle(BoardPosition position, Move move) {

	// assume white short castle, just becouse
	B64 row = ROW_1;
	B64 king_col = COLUMN_C;
	B64 rook_col = COLUMN_D;

	if (move.type == CASTLE_LONG) { // fix castle side if wrong
		king_col = COLUMB_G;
		rook_col = COLUMB_F;
	}

	if (move.piece.color == BLACK) { // fix color if wrong and castle
		row = ROW_8;

		position.black ^= move.origin | move.destination; // delete both pieces from thier current locations
		position.black_rooks ^= move.destination; // rook is saved in destination, both land on predefined spots
		
		// add back to new locations
		position.black_king = row & king_col;
		position.black_rooks |= row & rook_col;
		position.black |= (row & king_col) | (row & rook_col);
	} else {
		position.white ^= move.origin | move.destination; // delete both pieces from thier current locations
		position.white_rooks ^= move.destination; // rook is saved in destination, both land on predefined spots

		// add back to new locations
		position.white_king = row & king_col;
		position.white_rooks |= row & rook_col;
		position.white |= (row & king_col) | (row & rook_col);
	}

	if (move.piece.color == WHITE) {
		position.white ^= move.origin;
		position.white |= move.destination;
	} else {
		position.black ^= move.origin;
		position.black |= move.destination;
	}
}

BoardPosition Move_piece(BoardPosition position, Move move) {
	// one more if for fewer
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

BoardPosition Delete_captured(BoardPosition position, Move move) {
	if (move.piece.color == WHITE) {
		switch (move.captured_type) {
		case PAWN:
			position.black_pawns ^= move.destination;
		case KNIGHT:
			position.black_knights ^= move.destination;
		case BISHOP:
			position.black_bishops ^= move.destination;
		case ROOK:
			position.black_rooks ^= move.destination;
		case QUEEN:
			position.black_queens ^= move.destination;
		}
	} else {
		switch (move.captured_type) {
		case PAWN:
			position.white_pawns ^= move.destination;
		case KNIGHT:
			position.white_knights ^= move.destination;
		case BISHOP:
			position.white_bishops ^= move.destination;
		case ROOK:
			position.white_rooks ^= move.destination;
		case QUEEN:
			position.white_queens ^= move.destination;
		}
	}

	return position;
}

BoardPosition Promote(BoardPosition position, Move move) {
	
	// not sure this variable is computationally worth it but its readable
	B64 promoted;

	if (move.piece.color == WHITE) { // fix color if wrong and promote
		promoted = position.white_pawns & ROW_1;

		position.white_pawns ^= promoted; // remove the promoted pawn

		// replace the pawn with the new piece
		switch (move.promoted_type) {
		case QUEEN:
			position.white_queens |= promoted;
			break;
		case ROOK:
			position.white_rooks |= promoted;
			break;
		case BISHOP:
			position.white_bishops |= promoted;
			break;
		case KNIGHT:
			position.white_knights |= promoted;
			break;
		}

	} else {
		promoted = position.black_pawns & ROW_8;

		position.black_pawns ^= promoted; // remove the promoted pawn

		// replace the pawn with the new piece
		switch (move.promoted_type) {
		case QUEEN:
			position.white_queens |= promoted;
			break;
		case ROOK:
			position.white_rooks |= promoted;
			break;
		case BISHOP:
			position.black_bishops |= promoted;
			break;
		case KNIGHT:
			position.black_knights |= promoted;
			break;
		}
	}
}


//Move Invert_move(Move move)
//{
//	std::swap(move.origin, move.destination);
//	return move;
//}
//
//BoardPosition Undo_move(BoardPosition position, Move move) {
//	return Make_move(position, Invert_move(move));
//}
