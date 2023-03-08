
#include "Move structure.h"
#include "Board operations.h"
#include "Piece structure.h"

Move expand_move(const SidedPosition& sided_position, const BitMove bitmove) {
	Move move;

	move.origin = bit_board_from_index((bitmove & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET);
	move.destination = bit_board_from_index((bitmove & DESTINATION_INDEX_MASK) >> DESTINATION_INDEX_OFFSET);

	move.piece.color = (sided_position.is_white ? WHITE : BLACK);
	if (sided_position.own_pawns & move.origin) {
		move.piece.type = PAWN;
	} else if (sided_position.own_knights & move.origin) {
		move.piece.type = KNIGHT;
	} else if (sided_position.own_bishops & move.origin) {
		move.piece.type = BISHOP;
	} else if (sided_position.own_rooks & move.origin) {
		move.piece.type = ROOK;
	} else if (sided_position.own_queens & move.origin) {
		move.piece.type = QUEEN;
	} else if (sided_position.own_king & move.origin) {
		move.piece.type = KING;
	}

	move.type = MoveType((bitmove & MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET);
	
	move.captured_type = PieceType((bitmove & CAPTURE_MASK) >> CAPTURE_OFFSET);
	move.promoted_type = PieceType((bitmove & PROMOTE_MASK) >> PROMOTE_OFFSET);

	// add castling rights
	move.special_move_rigths = 0ULL | ((bitmove & WHITE_CASTLE_MASK) >> WHITE_CASTLE_OFFSET) | ((bitmove & BLACK_CASTLE_MASK) >> WHITE_CASTLE_OFFSET);
	if (bitmove & EN_PASSANT_VALID_MASK) { // add en passant if exists
		move.special_move_rigths |= bit_board_from_index((bitmove & WHITE_CASTLE_MASK) >> EN_PASSANT_INDEX_OFFSET);
	}

	return move;
}
