
#include "Move handling.h"
#include "Game constants.h"

// makes a given move, does not check its validity
SidedPosition Make_move(SidedPosition sided_position, Move move) {

	if (Is_castle(move)) {
		Toggle_castle(sided_position, move);
	} else {
		Move_piece(sided_position, move);

		if (Is_capture(move)) {
			Toggle_captured(sided_position, move);
		}

		if (Is_promotion(move)) {
			Toggle_promotion(sided_position, move);
		}
	}

	set_special_move_rights(sided_position, move);

	return sided_position;
}

void Toggle_castle(SidedPosition& sided_position, const Move& move) {

	const B64 king_dest = (sided_position.is_white ? (move.type == CASTLE_SHORT ? WHITE_KING_SHORT_CASTLE : WHITE_KING_LONG_CASTLE)
												   : (move.type == CASTLE_SHORT ? BLACK_KING_SHORT_CASTLE : BLACK_KING_LONG_CASTLE));
	const B64 rook_dest = (sided_position.is_white ? (move.type == CASTLE_SHORT ? WHITE_ROOK_SHORT_CASTLE : WHITE_ROOK_LONG_CASTLE)
								   				   : (move.type == CASTLE_SHORT ? BLACK_ROOK_SHORT_CASTLE : BLACK_ROOK_LONG_CASTLE));

	// toggle origins and destinations
	sided_position.own_king ^= (move.origin | king_dest);
	sided_position.own_rooks ^= (move.destination | rook_dest); // rook is saved in destination, as both land on predefined tiles
}

void Move_piece(SidedPosition& sided_position, const Move& move) {

	B64& moved_pieces = *own_piece_board_ref(sided_position, move.piece.type);

	moved_pieces ^= (move.origin | move.destination);
}

void Toggle_captured(SidedPosition& sided_position, const Move& move) {

	switch (move.captured_type) {
	case PAWN:
		sided_position.opponent_pawns ^= move.destination;
		break;

	case KNIGHT:
		sided_position.opponent_knights ^= move.destination;
		break;

	case BISHOP:
		sided_position.opponent_bishops ^= move.destination;
		break;

	case ROOK:
		sided_position.opponent_rooks ^= move.destination;
		break;

	case QUEEN:
		sided_position.opponent_queens ^= move.destination;
		break;
	}
}

void Toggle_promotion(SidedPosition& sided_position, const Move& move) {

	// find the promoted pawn and remove it
	const B64 promoted = sided_position.own_pawns & (sided_position.is_white ? ROW_1 : ROW_8);
	sided_position.own_pawns ^= promoted;

	// add the promoted piece in its place
	switch (move.promoted_type) {
	case QUEEN:
		sided_position.own_queens ^= promoted;
		break;

	case ROOK:
		sided_position.own_rooks ^= promoted;
		break;

	case BISHOP:
		sided_position.own_bishops ^= promoted;
		break;

	case KNIGHT:
		sided_position.own_knights ^= promoted;
		break;
	}
}

void set_special_move_rights(SidedPosition& sided_position, const Move& move) {
	switch (move.piece.type) {
	case PAWN:
		// this is very crude and pointless in undos as im recovering it from the moveitself
		if ((move.origin & ROW_2) && (move.destination | ROW_4)) {
			sided_position.special_move_rigths ^= up(move.origin);
		} else if ((move.origin & ROW_7) && (move.destination | ROW_5)) {
			sided_position.special_move_rigths ^= down(move.origin);
		}

	case ROOK:
		clear_bit(sided_position.special_move_rigths, move.origin); // void castling right
		break;

	case KING:
		sided_position.special_move_rigths &= (sided_position.is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castling rights
		break;
	}
}

// returns the board to the position before the move, no validity check
SidedPosition Undo_move(SidedPosition sided_position, Move move) {
	if (Is_castle(move)) {
		Toggle_castle(sided_position, move);
	} else {
		if (Is_promotion(move)) {
			Toggle_promotion(sided_position, move);
		}

		if (Is_capture(move)) {
			Toggle_captured(sided_position, move);
		}

		Move_piece(sided_position, invert_move(move));
	}

	// return the special move rights as they were before the move
	sided_position.special_move_rigths = move.special_move_rigths;

	return sided_position;
}
