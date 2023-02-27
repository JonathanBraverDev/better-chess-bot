
#include "Attack detection.h"

bool is_check(const BoardPosition position, const bool is_attacker_color) {
	const B64 attacked_king = (is_attacker_color ? position.black_king : position.white_king);
	const int tile = lowest_single_bit_index(attacked_king);
	B64 slide_attackes;

	bool check = false;

	// perform fastest bit check first
	if ((knight_moves[tile] & (is_attacker_color ? position.white_knights : position.black_knights)) ||
		(pawn_attacks[tile] & (is_attacker_color ? position.white_pawns : position.black_pawns))) {

		check = true;

	} else {
		// can be futher split into multiple checks but this is WAY more readable
		slide_attackes = generate_queen_moves(position.white | position.black, attacked_king);

		check = ((slide_attackes & (is_attacker_color ? position.white_queens : position.black_queens)) ||
			(slide_attackes & (is_attacker_color ? position.white_rooks : position.black_rooks)) ||
			(slide_attackes & (is_attacker_color ? position.white_bishops : position.black_bishops)));
	}

	return check;
}

bool is_slide_attacked(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 target_board, const B64 blockers) {
	return is_attacked(attacking_pieces, target_board, move_generator, nullptr, blockers);
}

bool is_jump_attacked(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale, const int first_index) {
	return is_attacked(attacking_pieces, target_board, nullptr, move_source, 0, index_scale, first_index);
}

bool is_attacked(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64), const B64* move_source, const B64 blockers, const int index_scale, const int first_index) {
	std::vector<B64> potential_attackers;
	B64 attack_board;
	bool attacked = false;

	seperate_bits(attacking_pieces, potential_attackers);

	while (!potential_attackers.empty() && !attacked) {
		if (move_generator) {
			attack_board = move_generator(blockers, potential_attackers.back());
		} else {
			attack_board = move_source[first_index + index_scale * lowest_bit_index(potential_attackers.back())];
		}

		if (attack_board && target_board) {
			attacked = true;
		}
		potential_attackers.pop_back();
	}

	return attacked;
}

bool is_attackd_by_color(const BoardPosition position, const B64 target_board, const bool is_attacker_white) {

	const B64 blockers = position.white | position.black;

	return is_slide_attacked(generate_queen_moves, (is_attacker_white ? position.white_queens : position.black_queens), target_board, blockers) ||
		is_slide_attacked(generate_rook_moves, (is_attacker_white ? position.white_rooks : position.white_rooks), target_board, blockers) ||
		is_slide_attacked(generate_bishop_moves, (is_attacker_white ? position.white_bishops : position.black_bishops), target_board, blockers) ||
		is_jump_attacked(king_moves, position.white_king, target_board) ||
		is_jump_attacked(knight_moves, position.white_knights, target_board) ||
		is_jump_attacked(pawn_attacks, position.white_pawns, target_board, 2); // pawns have 2 moves per tile
}

B64 attacking_pieces(const BoardPosition position, const B64 target_board_bit, const bool is_attacker_white) {
	const int tile = lowest_single_bit_index(target_board_bit);
	const B64 slide_attackes = generate_queen_moves(position.white | position.black, target_board_bit);

	return (knight_moves[tile] & (is_attacker_white ? position.white_knights : position.black_knights)) |
		(pawn_attacks[tile] & (is_attacker_white ? position.white_pawns : position.black_pawns)) |
		(king_moves[tile] & (is_attacker_white ? position.white_king : position.black_king)) | // irrelevent for checks but quick and generic
		(slide_attackes & (is_attacker_white ? position.white_queens : position.black_queens)) |
		(slide_attackes & (is_attacker_white ? position.white_rooks : position.black_rooks)) |
		(slide_attackes & (is_attacker_white ? position.white_bishops : position.black_bishops));
}

int count_sliding_attacks(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 target_board, const B64 blockers) {
	return count_attacks(attacking_pieces, target_board, move_generator, nullptr, blockers);
}

int count_jumping_attacks(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale, const int first_index) {
	return count_attacks(attacking_pieces, target_board, nullptr, move_source, 0, index_scale, first_index);
}

int count_attacks(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64), const B64* move_source, const B64 blockers, const int index_scale, const int first_index) {
	std::vector<B64> potential_attackers;
	B64 attack_board;
	int attacks = 0;

	seperate_bits(attacking_pieces, potential_attackers);

	while (!potential_attackers.empty()) {
		if (move_generator) {
			attack_board = move_generator(blockers, potential_attackers.back());
		} else {
			attack_board = move_source[first_index + index_scale * lowest_bit_index(potential_attackers.back())];
		}

		if (attack_board && target_board) {
			attacks++;
		}
		potential_attackers.pop_back();
	}

	return attacks;
}

int count_white_attacks(const BoardPosition position, const B64 target_board) {

	const B64 blockers = position.white | position.black;

	return count_sliding_attacks(generate_queen_moves, position.white_queens, target_board, blockers) +
		count_sliding_attacks(generate_rook_moves, position.white_rooks, target_board, blockers) +
		count_sliding_attacks(generate_bishop_moves, position.white_bishops, target_board, blockers) +
		count_jumping_attacks(king_moves, position.white_king, target_board) +
		count_jumping_attacks(knight_moves, position.white_knights, target_board) +
		count_jumping_attacks(pawn_attacks, position.white_pawns, target_board, 2); // pawns have 2 moves per tile
}

int count_black_attacks(const BoardPosition position, const B64 target_board) {

	const B64 blockers = position.white | position.black;

	return count_sliding_attacks(generate_queen_moves, position.black_queens, target_board, blockers) +
		count_sliding_attacks(generate_rook_moves, position.black_rooks, target_board, blockers) +
		count_sliding_attacks(generate_bishop_moves, position.black_bishops, target_board, blockers) +
		count_jumping_attacks(king_moves, position.black_king, target_board) +
		count_jumping_attacks(knight_moves, position.black_knights, target_board) +
		count_jumping_attacks(pawn_attacks, position.black_pawns, target_board, 2, 1); // offset to blacks moves
}