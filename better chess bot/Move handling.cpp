
#include "Move handling.h"

// makes a given move, does not check its validity
BoardPosition Make_move(BoardPosition position, Move move) {

	if (Is_castle(move)) {
		Toggle_castle(position, move);
	} else {
		Move_piece(position, move);

		if (Is_capture(move)) {
			Toggle_captured(position, move);
		}

		if (Is_promotion(move)) {
			Toggle_promotion(position, move);
		}
	}

	return position;
}

void Toggle_castle(BoardPosition& position, const Move& move) {

	const B64 king = move.origin;
	const B64 rook = move.destination; // rook is saved in destination, both land on predefined spots

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

		// toggle uncastled locations
		position.black ^= king | rook; // toggle both pieces off/on (casle/uncastle)
		position.black_rooks ^= rook; 
		position.black_king ^= king; // added to toggle the king back in during UNcastling

		// toggle castled locations
		position.black_king ^= row & king_col;
		position.black_rooks ^= row & rook_col;
		position.black ^= (row & king_col) | (row & rook_col);

	} else {
		// toggle uncastled locations
		position.white ^= king | rook;
		position.white_rooks ^= rook;
		position.white_king ^= king;

		// toggle castled locations
		position.white_king ^= row & king_col;
		position.white_rooks ^= row & rook_col;
		position.white ^= (row & king_col) | (row & rook_col);
	}
}

void Move_piece(BoardPosition& position, const Move& move, bool reverse) {
	const bool is_white = move.piece.color == WHITE;
	const B64 origin = (reverse ? move.destination : move.origin);
	const B64 destination = (reverse ? move.origin : move.destination);

	// update the color board
	(is_white ? position.white : position.black) ^= origin;
	(is_white ? position.white : position.black) |= destination;
	
	// update the piece board
	switch (move.piece.type) {
	case PAWN:
		// this is very crude and pointless in undos as im recovering it from the moveitself
		if ((origin & ROW_2) && (destination | ROW_4)) {
			position.special_move_rigths ^= up(origin);
		} else if ((origin & ROW_7) && (destination | ROW_5)) {
			position.special_move_rigths ^= down(origin);
		}

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
		clear_bit(position.special_move_rigths, (reverse ? move.destination : move.origin)); // void castling right
		// king should get voided too if both rooks moved, this is currently NOT done
		(is_white ? position.white_rooks : position.black_rooks) ^= origin;
		(is_white ? position.white_rooks : position.black_rooks) |= destination;
		break;

	case QUEEN:
		(is_white ? position.white_queens : position.black_queens) ^= origin;
		(is_white ? position.white_queens : position.black_queens) |= destination;
		break;

	case KING:
		position.special_move_rigths &= (is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castling rights
		(is_white ? position.white_king : position.black_king) ^= origin;
		(is_white ? position.white_king : position.black_king) |= destination;
		break;
	}
}

void Toggle_captured(BoardPosition& position, const Move& move) {
	const bool is_black = move.piece.color == BLACK;
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

void Toggle_promotion(BoardPosition& position, const Move& move) {
	const bool is_white = move.piece.color == WHITE;

	// find the promoted pawn and remove it
	const B64 promoted = is_white ? (position.white_pawns & ROW_1) : (position.black_pawns & ROW_8);
	(is_white ? position.white_pawns : position.black_pawns) ^= promoted;

	// add the promoted piece in its place
	switch (move.promoted_type) {
	case QUEEN:
		(is_white ? position.white_queens : position.black_queens) ^= promoted;
		break;

	case ROOK:
		(is_white ? position.white_rooks : position.black_rooks) ^= promoted;
		break;

	case BISHOP:
		(is_white ? position.white_bishops : position.black_bishops) ^= promoted;
		break;

	case KNIGHT:
		(is_white ? position.white_knights : position.black_knights) ^= promoted;
		break;
	}
}

// returns the board to the position before the move, no validity check
BoardPosition Undo_move(BoardPosition position, Move move) {
	if (Is_castle(move)) {
		Toggle_castle(position, move);
	} else {
		if (Is_promotion(move)) {
			Toggle_promotion(position, move);
		}

		if (Is_capture(move)) {
			Toggle_captured(position, move);
		}

		Move_piece(position, move, true);
	}

	return position;
}
