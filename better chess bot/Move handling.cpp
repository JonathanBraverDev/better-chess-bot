#include "Move handling.h"
#include "Board operation.h"

// makes a given move, does not check its validity
BoardPosition Make_move(BoardPosition position, Move move) {

	if (Is_castle(move)) {
		Castle(position, move);
	} else {
		Move_piece(position, move);

		if (Is_capture(move)) {
			Delete_captured(position, move);
		}

		if (Is_promotion(move)) {
			Promote(position, move);
		}
	}

	return position;
}

void Castle(BoardPosition& position, const Move& move) {

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
}

void Move_piece(BoardPosition& position, const Move& move) {
	const bool is_white = move.piece.color == WHITE;
	const B64 origin = move.origin;
	const B64 destination = move.destination;

	// update the color board
	(is_white ? position.white : position.black) ^= origin;
	(is_white ? position.white : position.black) |= destination;
	
	// update the piece board
	switch (move.piece.type) {
	case PAWN:
		(is_white ? position.white_pawns : position.black_pawns) ^= origin;
		(is_white ? position.white_pawns : position.black_pawns) |= destination;
		break;

	case KNIGHT:
		(is_white ? position.white_knights : position.black_knights) ^= origin;
		(is_white ? position.white_knights : position.black_knights) |= destination;
		break;

	case BISHOP:
		(is_white ? position.white_bishops : position.black_bishops) ^= origin;
		(is_white ? position.white_bishops : position.black_bishops) |= destination;
		break;

	case ROOK:
		(is_white ? position.white_rooks : position.black_rooks) ^= origin;
		(is_white ? position.white_rooks : position.black_rooks) |= destination;
		break;

	case QUEEN:
		(is_white ? position.white_queens : position.black_queens) ^= origin;
		(is_white ? position.white_queens : position.black_queens) |= destination;
		break;

	case KING:
		(is_white ? position.white_king : position.black_king) ^= origin;
		(is_white ? position.white_king : position.black_king) |= destination;
		break;
	}
}

void Delete_captured(BoardPosition& position, const Move& move) {
	const bool is_black = move.piece.color == BLACK;
	const B64 origin = move.origin;
	const B64 destination = move.destination;

	switch (move.captured_type) {
	case PAWN:
		(is_black ? position.white_pawns : position.black_pawns) ^= destination;
		break;

	case KNIGHT:
		(is_black ? position.white_knights : position.black_knights) ^= destination;
		break;

	case BISHOP:
		(is_black ? position.white_bishops : position.black_bishops) ^= destination;
		break;

	case ROOK:
		(is_black ? position.white_rooks : position.black_rooks) ^= destination;
		break;

	case QUEEN:
		(is_black ? position.white_queens : position.black_queens) ^= destination;
		break;
	}
}

void Promote(BoardPosition& position, const Move& move) {
	const bool is_white = move.piece.color == WHITE;
	const B64 origin = move.origin;
	const B64 destination = move.destination;

	// find the promoted pawn and remove it
	const B64 promoted = is_white ? (position.white_pawns & ROW_1) : (position.black_pawns & ROW_8);
	(is_white ? position.white_pawns : position.black_pawns) ^= promoted;

	// add the promoted piece in its place
	switch (move.promoted_type) {
	case QUEEN:
		(is_white ? position.white_queens : position.black_queens) |= promoted;
		break;

	case ROOK:
		(is_white ? position.white_rooks : position.black_rooks) |= promoted;
		break;

	case BISHOP:
		(is_white ? position.white_bishops : position.black_bishops) |= promoted;
		break;

	case KNIGHT:
		(is_white ? position.white_knights : position.black_knights) |= promoted;
		break;
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
