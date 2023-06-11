
#include "Board generation.h"

// defining extern arrays
B64 king_moves[64];
B64 knight_moves[64];
B64 pawn_moves[64 * 2];
B64 pawn_attacks[64 * 2];


B64 generate_bishop_moves(const B64 blockers, B64 piece) {

	return slide_up_left(blockers, piece) |
		slide_up_right(blockers, piece) |
		slide_down_left(blockers, piece) |
		slide_down_right(blockers, piece);
}

B64 generate_rook_moves(const B64 blockers, B64 piece) {

	return slide_up(blockers, piece) |
		slide_down(blockers, piece) |
		slide_left(blockers, piece) |
		slide_right(blockers, piece);
}

B64 generate_queen_moves(const B64 blockers, B64 piece) {

	return generate_bishop_moves(blockers, piece) |
		generate_rook_moves(blockers, piece);
}

// assums that the pawn is allowed to jump
B64 generate_pawn_jump(const B64 blockers, B64 piece, B64(*const direction)(B64)) {
	B64 moves = 0;
	if (!((direction(piece) & blockers) |
		((direction(direction(piece)) & blockers)))) {
		moves |= direction(direction(piece));
	}

	return moves;
}

void prepare_king_moves() {
	B64 current_board = 1;
	for (size_t i = 0; i < 64; i++) {
		king_moves[i] = up(current_board) |
			down(current_board) |
			left(current_board) |
			right(current_board) |
			up_left(current_board) |
			up_right(current_board) |
			down_left(current_board) |
			down_right(current_board);

		current_board <<= 1;
	}
}

void prepare_knight_moves() {
	B64 current_board = 1;
	for (size_t i = 0; i < 64; i++) { // column inversions delete moves that loop around the board
		knight_moves[i] = (((current_board << 6)  | (current_board >> 10)) & ~COLUMN_GH) | // long left, delete right band
						  (((current_board << 10) | (current_board >> 6))  & ~COLUMN_AB) | // long right, delete left band
						  (((current_board << 15) | (current_board >> 17)) & ~COLUMN_H)  | // tall left, delete right edge
						  (((current_board << 17) | (current_board >> 15)) & ~COLUMN_A);   // tall right, delete left edge
		current_board <<= 1;
	}
}

void prepare_white_pawn_moves() {
	B64 current_board = 1ULL;
	for (size_t i = 0; i < 64 * 2; i += 2) {
		// missing promotion logic
		pawn_moves[i] = up(current_board);
		// pawn jumps require sliding piece logic without final capture
		// missing en passant logic

		pawn_attacks[i] = up_left(current_board) |
			up_right(current_board);

		current_board <<= 1;
	}
}

void prepare_black_pawn_moves() {
	B64 current_board = 1ULL;
	for (size_t i = 1; i < 64 * 2; i += 2) {
		// missing promotion logic
		pawn_moves[i] = down(current_board);
		// pawn jumps require sliding piece logic without final capture
		// missing en passant logic

		pawn_attacks[i] = down_left(current_board) |
			down_right(current_board);

		current_board <<= 1;
		// yes, black pawns can't get to the last row, not in a NORMAL game at least
	}
}

void prepare_pawn_moves() {
	// promotions and en passant will probably be handled by the actual move proccesing, not in generation
	// if a pawn has no POTENTIAL moves, he promoted
	prepare_white_pawn_moves();
	prepare_black_pawn_moves();
}

B64 rotate180(B64 board) {
	const B64 h1 = 0x5555555555555555;
	const B64 h2 = 0x3333333333333333;
	const B64 h4 = 0x0F0F0F0F0F0F0F0F;
	const B64 v1 = 0x00FF00FF00FF00FF;
	const B64 v2 = 0x0000FFFF0000FFFF;
	board = ((board >> 1) & h1) | ((board & h1) << 1);
	board = ((board >> 2) & h2) | ((board & h2) << 2);
	board = ((board >> 4) & h4) | ((board & h4) << 4);
	board = ((board >> 8) & v1) | ((board & v1) << 8);
	board = ((board >> 16) & v2) | ((board & v2) << 16);
	board = (board >> 32) | (board << 32);
	return board;
}