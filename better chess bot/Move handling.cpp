
#include "Move handling.h"
#include "Game constants.h"

// makes a given move, does not check its validity
SidedPosition make_move(SidedPosition& sided_position, BitMove bitmove) {
	// quite a lot of conversion are happening repeatedly to the board etc

	if (is_castle(bitmove)) {
		toggle_castle(sided_position, bitmove);
	} else { 
		if (is_capture(bitmove)) {
			toggle_captured(sided_position, bitmove);
		}

		if (is_promotion(bitmove)) {
			toggle_promotion(sided_position, bitmove);
		} else {
			toggle_move(sided_position, bitmove);
		}
	}

	//set_special_move_rights(sided_position, move);

	return sided_position;
}

void toggle_castle(SidedPosition& sided_position, const BitMove move) {

	B64 king_dest, rook_dest;

	bool is_short_castle = (get_misc_move_type(move) == CASTLE_SHORT);

	if (sided_position.is_white) {
		king_dest = (is_short_castle ? WHITE_KING_SHORT_CASTLE : WHITE_KING_LONG_CASTLE);
		rook_dest = (is_short_castle ? WHITE_ROOK_SHORT_CASTLE : WHITE_ROOK_LONG_CASTLE);
	} else {
		king_dest = (is_short_castle ? BLACK_KING_SHORT_CASTLE : BLACK_KING_LONG_CASTLE);
		rook_dest = (is_short_castle ? BLACK_ROOK_SHORT_CASTLE : BLACK_ROOK_LONG_CASTLE);
	}

	// toggle origins and destinations
	sided_position.own_king ^= (origin_board(move) | king_dest);
	sided_position.own_rooks ^= (destination_board(move) | rook_dest); // rook is saved in destination, as both land on predefined tiles
}

void toggle_move(SidedPosition& sided_position, const BitMove move) {

	B64& moved_pieces = *own_piece_board_ref(sided_position, (PieceType)get_moving_type(move));

	moved_pieces ^= (origin_board(move) | destination_board(move));
}

void toggle_captured(SidedPosition& sided_position, const BitMove move) {

	switch (get_captured_type(move)) {
	case PAWN:
		sided_position.opponent_pawns ^= destination_board(move);
		break;

	case KNIGHT:
		sided_position.opponent_knights ^= destination_board(move);
		break;

	case BISHOP:
		sided_position.opponent_bishops ^= destination_board(move);
		break;

	case ROOK:
		sided_position.opponent_rooks ^= destination_board(move);
		break;

	case QUEEN:
		sided_position.opponent_queens ^= destination_board(move);
		break;
	}
}

void toggle_promotion(SidedPosition& sided_position, const BitMove move) {

	// toggle the pawn
	sided_position.own_pawns ^= origin_board(move);

	// toggle the promoted piece type
	switch (get_moving_type(move)) {
	case QUEEN:
		sided_position.own_queens ^= destination_board(move);
		break;

	case ROOK:
		sided_position.own_rooks ^= destination_board(move);
		break;

	case BISHOP:
		sided_position.own_bishops ^= destination_board(move);
		break;

	case KNIGHT:
		sided_position.own_knights ^= destination_board(move);
		break;
	}
}

// DEPRACATED! this was make impossible to exstract from the move itself
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
		clear_bits(sided_position.special_move_rigths, move.origin); // void castling right
		break;

	case KING:
		sided_position.special_move_rigths &= (sided_position.is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castling rights
		break;
	}
}

// returns the board to the position before the move, no validity check
SidedPosition undo_move(SidedPosition& sided_position, BitMove bitmove) {

	if (is_castle(bitmove)) {
		toggle_castle(sided_position, bitmove);
	}
	else {
		if (is_capture(bitmove)) {
			toggle_captured(sided_position, bitmove);
		}

		if (is_promotion(bitmove)) {
			toggle_promotion(sided_position, bitmove);
		}
		else {
			toggle_move(sided_position, bitmove);
		}
	}

	//set_special_move_rights(sided_position, move);
	return sided_position;
}
