
#include "Attack detection.h"

bool is_check(const SidedPosition sided_position) {
	const int tile = lowest_single_bit_index(sided_position.own_king);
	const B64 blockers = all_pieces(sided_position);
	B64 bishop_attackes;
	B64 rook_attackes;

	// perform fastest bit check first
	bool check = ((knight_moves[tile] & sided_position.opponent_knights) ||
				  (pawn_attacks[tile] & sided_position.opponent_pawns));

	// then look at generated moves
	if (!check) {
		bishop_attackes = generate_bishop_moves(blockers, sided_position.own_king);
		check = (bishop_attackes & sided_position.opponent_bishops);

		if (!check) {
			rook_attackes = generate_rook_moves(blockers, sided_position.own_king);
			check = (rook_attackes & sided_position.opponent_rooks);

			if (!check) {
				check = ((bishop_attackes | rook_attackes) & sided_position.opponent_queens);
			}
		}
	}

	return check;
}

bool is_slide_attacked(B64(*const move_generator)(B64, B64), const B64 attacking_pieces, const B64 target_board, const B64 blockers) {
	return is_attacked(attacking_pieces, target_board, move_generator, nullptr, blockers);
}

bool is_jump_attacked(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale, const int first_index) {
	return is_attacked(attacking_pieces, target_board, nullptr, move_source, 0, index_scale, first_index);
}

bool is_attacked(const B64 attacking_pieces, const B64 target_board, B64(*const move_generator)(B64, B64), const B64* move_source, const B64 blockers, const int index_scale, const int first_index) {
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

bool is_attackd_by_side(const SidedPosition sided_position, const B64 target_board) {

	const B64 blockers = all_pieces(sided_position);

	return is_slide_attacked(generate_queen_moves, sided_position.opponent_queens, target_board, blockers) ||
		   is_slide_attacked(generate_rook_moves, sided_position.opponent_rooks, target_board, blockers) ||
		   is_slide_attacked(generate_bishop_moves, sided_position.opponent_bishops, target_board, blockers) ||
		   is_jump_attacked(king_moves, sided_position.opponent_king, target_board) ||
		   is_jump_attacked(knight_moves, sided_position.opponent_knights, target_board) ||
		   is_jump_attacked(pawn_attacks, sided_position.opponent_pawns, target_board, 2, (sided_position.is_white ? 1 : 0)); // pawns have 2 moves per tile
}

B64 attacking_pieces(const SidedPosition sided_position, const B64 target_board_bit) {
	const int tile_index = lowest_single_bit_index(target_board_bit);
	const B64 slide_attackes = generate_queen_moves(all_pieces(sided_position), target_board_bit);

	return (knight_moves[tile_index] & sided_position.opponent_knights) |
		   (pawn_attacks[tile_index * 2 + (sided_position.is_white ? 1 : 0)] & sided_position.opponent_pawns) |
		   (king_moves[tile_index] & sided_position.opponent_king) | // irrelevent for checks but quick and generic
		   (slide_attackes & sided_position.opponent_queens) |
		   (slide_attackes & sided_position.opponent_rooks) |
		   (slide_attackes & sided_position.opponent_bishops);
}

int count_sliding_attacks(B64(*const move_generator)(B64, B64), const B64 attacking_pieces, const B64 target_board, const B64 blockers) {
	return count_attacks(attacking_pieces, target_board, move_generator, nullptr, blockers);
}

int count_jumping_attacks(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale, const int first_index) {
	return count_attacks(attacking_pieces, target_board, nullptr, move_source, 0, index_scale, first_index);
}

int count_attacks(const B64 attacking_pieces, const B64 target_board, B64(*const move_generator)(B64, B64), const B64* move_source, const B64 blockers, const int index_scale, const int first_index) {
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

int count_own_attacks(const SidedPosition sided_position, const B64 target_board) {

	const B64 blockers = all_pieces(sided_position);

	return count_sliding_attacks(generate_queen_moves, sided_position.own_queens, target_board, blockers) +
		   count_sliding_attacks(generate_rook_moves, sided_position.own_rooks, target_board, blockers) +
		   count_sliding_attacks(generate_bishop_moves, sided_position.own_bishops, target_board, blockers) +
		   count_jumping_attacks(king_moves, sided_position.own_king, target_board) +
		   count_jumping_attacks(knight_moves, sided_position.own_knights, target_board) +
		   count_jumping_attacks(pawn_attacks, sided_position.own_pawns, target_board, 2); // pawns have 2 moves per tile
}

int count_opponent_attacks(const SidedPosition sided_position, const B64 target_board) {

	const B64 blockers = all_pieces(sided_position);

	return count_sliding_attacks(generate_queen_moves, sided_position.opponent_queens, target_board, blockers) +
		   count_sliding_attacks(generate_rook_moves, sided_position.opponent_rooks, target_board, blockers) +
		   count_sliding_attacks(generate_bishop_moves, sided_position.opponent_bishops, target_board, blockers) +
		   count_jumping_attacks(king_moves, sided_position.opponent_king, target_board) +
		   count_jumping_attacks(knight_moves, sided_position.opponent_knights, target_board) +
		   count_jumping_attacks(pawn_attacks, sided_position.opponent_pawns, target_board, 2, 1); // offset to blacks moves
}