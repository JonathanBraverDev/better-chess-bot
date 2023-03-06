
#include "Position generation.h"

void possible_piece_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
	std::vector<B64> single_pieces;
	SidedPosition new_position;

	B64& current_pieces = *own_piece_board_ref(new_position, piece_type);
	const B64 base_pieces = own_piece_board_copy(sided_position, piece_type);
	B64 potential_moves;

	seperate_bits(base_pieces, single_pieces); // get a vector of all pieces
	for (const B64& piece : single_pieces) {
		new_position = sided_position; // reset the new position
		current_pieces = base_pieces ^ piece; // delete the current piece from its origin

		if (move_generator == nullptr) { // the knight and king have precalculaed moves
			potential_moves = valid_destinations & (piece_type == KNIGHT ? knight_moves[lowest_single_bit_index(piece)]
																		 : king_moves[lowest_single_bit_index(piece)]);
		} else {
			potential_moves = move_generator(blockers, base_pieces) & valid_destinations; // get the valid moves per piece
			if (piece_type == ROOK) {
				// void castle right of a moving rook
				new_position.special_move_rigths ^= piece & new_position.special_move_rigths & (sided_position.is_white ? WHITE_CASTLE_ROW : BLACK_CASTLE_ROW);
			}
		}

		// add the mvoes if any exist, removing the opponent's piece
		if (potential_moves) {
			possible_capture_positions(positions, new_position, potential_moves, piece, current_pieces);
		}
	}
}

void possible_pawn_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const PieceType piece_type, const B64 free_tiles, const B64 opponent_pieces) {

	std::vector<B64> single_pieces;
	const B64 base_pieces = own_piece_board_copy(sided_position, piece_type);
	const int first_index = (sided_position.is_white ? 0 : 1);
	
	SidedPosition new_position;
	B64& current_pieces = *own_piece_board_ref(new_position, piece_type);

	int move_index;
	B64 normal_move;
	B64 en_passant;
	B64 attack_moves;
	bool promoting; // a pawn can only more towards promotion so any more from the row before is promoting
	B64 jump;
	std::vector<B64> attacks;
	

	seperate_bits(base_pieces, single_pieces); // get a vector of all pieces
	for (const B64& piece : single_pieces) {
		// calculate all information needed for the moves
		new_position = sided_position;
		move_index = first_index + 2 * lowest_single_bit_index(piece);
		normal_move = pawn_moves[move_index] & free_tiles;
		en_passant = sided_position.special_move_rigths & (sided_position.is_white ? BLACK_PAWN_EN_PASSANT : WHITE_PAWN_EN_PASSANT);
		attack_moves = pawn_attacks[move_index] & (opponent_pieces | en_passant);
		promoting = piece & (new_position.is_white ? WHITE_PAWN_PRE_PROMOTION : BLACK_PAWN_PRE_PROMOTION);
		attacks.clear();

		// add normal move if possible
		if (normal_move) {
			new_position.own_pawns ^= (piece | normal_move); // toggle pawn origin and move location
			if (promoting) { // check pawn promotion
				possible_pawn_promotions(positions, new_position);
			} else {
				new_position.special_move_rigths &= VOID_ALL_EN_PASSANT;
				positions.push_back(new_position); // push the normal move after voiding en passant

				// then check for jumps
				if (new_position.own_pawns & (new_position.is_white ? WHITE_PAWN_JUMP_START : BLACK_PAWN_JUMP_START)) { // check if the pawn is on its initial row
					// waste no information, only check the jump target tile
					jump = free_tiles & (sided_position.is_white ? up(normal_move) : down(normal_move));

					if (jump) { // if a jump is legal, add it
						new_position.own_pawns ^= (normal_move | jump); // remove pawn from normal move, add jump
						new_position.special_move_rigths ^= (new_position.is_white ? up(piece) : down(piece)); // add en passant
						positions.push_back(new_position); 
					}
				}
			}
		}
	
		// add captures if any are possible
		if (attack_moves) {
			seperate_bits(attack_moves, attacks);

			for (const B64 attack : attacks) {
				new_position = sided_position; // reset position
				new_position.own_pawns ^= (piece | attack); // toggle pawn origin and attack location
				if (attack & en_passant) { // check if the current attack is an en passant
					// white is captured from below, black from above
					new_position.opponent_pawns ^= (new_position.is_white ? up(attack) : down(attack));

					new_position.special_move_rigths &= VOID_ALL_EN_PASSANT;
					positions.push_back(new_position); // push after voiding en passant
				} else {
					if (promoting) { // check pawn promotion
						possible_pawn_promotions(positions, new_position);
					} else {
						// delete any enemy piece in the destination
						clear_bits(new_position.opponent_pawns, attack);
						clear_bits(new_position.opponent_knights, attack);
						clear_bits(new_position.opponent_bishops, attack);
						clear_bits(new_position.opponent_rooks, attack);
						clear_bits(new_position.opponent_queens, attack);

						new_position.special_move_rigths &= VOID_ALL_EN_PASSANT;
						positions.push_back(new_position); // push after voiding en passant
					}
				}
			}
		}
	}
}

void possible_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, B64 potential_moves, const B64 piece, B64& current_pieces) {
	
	std::vector<B64> single_moves;
	SidedPosition base_position = sided_position;
	base_position.special_move_rigths &= VOID_ALL_EN_PASSANT; // void en passant for all moves
	SidedPosition new_position;

	seperate_bits(potential_moves, single_moves); // seperate the generated moves

	for (const B64& move : single_moves) {
		new_position = base_position;
		current_pieces ^= move; // add the current piece to its destination
		// delete any enemy piece in the destination
		clear_bits(new_position.opponent_pawns, move);
		clear_bits(new_position.opponent_knights, move);
		clear_bits(new_position.opponent_bishops, move);
		clear_bits(new_position.opponent_rooks, move);
		clear_bits(new_position.opponent_queens, move);

		positions.push_back(new_position);
	}
}

void possible_pawn_promotions(std::vector<SidedPosition>& positions, SidedPosition sided_position) {

	// find the promoted pawn and remove it
	const B64 promote_tile = (sided_position.own_pawns & (sided_position.is_white ? ROW_8 : ROW_1));
	sided_position.special_move_rigths &= VOID_ALL_EN_PASSANT; // void en passant for all promotions
	sided_position.own_pawns ^= promote_tile;

	// add the a position with each possible promotion to the vector and remove the piece after
	// ugly but effective
	sided_position.own_queens ^= promote_tile;
	positions.push_back(sided_position); // add queen
	sided_position.own_queens ^= promote_tile;

	sided_position.own_rooks ^= promote_tile;
	positions.push_back(sided_position); // add rook
	sided_position.own_rooks ^= promote_tile;

	sided_position.own_bishops ^= promote_tile;
	positions.push_back(sided_position); // add bishop
	sided_position.own_bishops ^= promote_tile;

	sided_position.own_knights ^= promote_tile;
	positions.push_back(sided_position); // add knight
}

bool is_castle_legal(const SidedPosition sided_position, const B64 king_start, const B64 king_end) {
	const B64 king_path = get_connecting_tiles(king_start, king_end);

	bool is_legal = false;

	if (king_path & ~(all_pieces(sided_position))) { // check for anything in the king's path
		is_legal = is_attackd_by_side(sided_position, king_path);
	}

	return is_legal;
}

void possible_castle_positions(std::vector<SidedPosition>& positions, SidedPosition sided_position) {

	const B64 short_rook_valid = higher_than_bit(sided_position.own_king) & sided_position.special_move_rigths & sided_position.own_rooks;
	const B64 long_rook_valid = lower_than_bit(sided_position.own_king) & sided_position.special_move_rigths & sided_position.own_rooks;

	sided_position.special_move_rigths &= (sided_position.is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castle rights after checking them

	B64 castled_king;

	if (short_rook_valid) { // check if a caste move is valid
		castled_king = (sided_position.is_white ? WHITE_KING_SHORT_CASTLE : BLACK_KING_SHORT_CASTLE);

		if (is_castle_legal(sided_position, sided_position.own_king, castled_king)) {
			sided_position.own_rooks ^= (short_rook_valid | (sided_position.is_white ? WHITE_ROOK_SHORT_CASTLE : BLACK_ROOK_SHORT_CASTLE)); // flip both old and new location
			positions.push_back(sided_position);
		}
	}

	if (long_rook_valid) { // check if a caste move is valid
		castled_king = (sided_position.is_white ? WHITE_KING_LONG_CASTLE : BLACK_KING_LONG_CASTLE);

		if (is_castle_legal(sided_position, sided_position.own_king, castled_king)) {
			sided_position.own_rooks ^= long_rook_valid | (sided_position.is_white ? WHITE_ROOK_LONG_CASTLE : BLACK_ROOK_LONG_CASTLE);
			positions.push_back(sided_position);
		}
	}
}

void all_possible_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position) {
	const B64 opponent = opponent_pieces(sided_position);
	const B64 own = own_pieces(sided_position);
	const B64 blockers = own | opponent; // for collision detection
	const B64 not_own = ~own; // valid destinations for most pieces

	// for every exisintg piece type, add it's moves
	if (sided_position.own_pawns) { // can I just say that I HATE how complicated this piece type is?
		possible_pawn_positions(positions, sided_position, PAWN, ~blockers, opponent);
	}
	if (sided_position.own_knights) {
		possible_piece_positions(positions, sided_position, KNIGHT, blockers, not_own, nullptr, knight_moves);
	}
	if (sided_position.own_bishops) {
		possible_piece_positions(positions, sided_position, BISHOP, blockers, not_own, &generate_bishop_moves);
	}
	if (sided_position.own_rooks) {
		possible_piece_positions(positions, sided_position, ROOK, blockers, not_own, &generate_rook_moves);
	}
	if (sided_position.own_queens) {
		possible_piece_positions(positions, sided_position, QUEEN, blockers, not_own, &generate_queen_moves);
	}

	// king normals, assuming he didn't get brutally murdered (eveluation planned to end on unavaidable check)
	possible_piece_positions(positions, sided_position, KING, blockers, not_own, nullptr, king_moves);
	if (sided_position.own_king & sided_position.special_move_rigths) { // add castles is legal
		possible_castle_positions(positions, sided_position);
	}

	// validation for checks dosent happen here
}

// moves that get here are garanteed to be possible, all killing hte target one way or another
void tile_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target, B64 killing_pieces) {

	std::vector<B64> single_pieces;
	SidedPosition new_position;
	B64 en_passant_tile;

	seperate_bits(killing_pieces, single_pieces);

	for (const B64& piece : single_pieces) {
		new_position = sided_position; // reset position
		// delete any enemy piece in the destination
		if (killing_pieces & sided_position.own_pawns) { // if there's overlap, its a pawn
			en_passant_tile = (sided_position.is_white ? up(target) : down(target)); // dark pawns have an enpassant ABOVE them
			if (en_passant_tile & new_position.special_move_rigths) {
				killing_pieces ^= en_passant_tile; // add the pawn to the en passant tile
			}
		} else {
			killing_pieces ^= target; // add the piece to the target tile
		}
		killing_pieces ^= piece; // remove piece from its origin

		// clear the target
		clear_bits(new_position.opponent_pawns, target);
		clear_bits(new_position.opponent_knights, target);
		clear_bits(new_position.opponent_bishops, target);
		clear_bits(new_position.opponent_rooks, target);
		clear_bits(new_position.opponent_queens, target);

		new_position.special_move_rigths &= VOID_ALL_EN_PASSANT; // void en passant

		if (new_position.own_pawns & (new_position.is_white ? ROW_8 : ROW_1)) { // check pawn promotion
			possible_pawn_promotions(positions, new_position);
		} else {
			positions.push_back(new_position);
		}
	}
}

void kills_to_tile(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target_board_bit) {
	const int tile_index = lowest_single_bit_index(target_board_bit);
	const B64 slide_attackes = generate_queen_moves(all_pieces(sided_position), target_board_bit);

	const B64 killing_knights = (knight_moves[tile_index] & sided_position.own_knights);
	const B64 killing_bishops = (slide_attackes & sided_position.own_bishops);
	const B64 killing_rooks = (slide_attackes & sided_position.own_rooks);
	const B64 killing_queens = (slide_attackes & sided_position.own_queens);

	// trick here, look for pawns in the attack pattern of the opponent to find tiles from which pawns can attack to the target 
	// THEN also look for en passant opportunities, I LOVE PAWNS, have I said it enough times already?!?!
	B64 killing_pawns = (pawn_attacks[tile_index * 2 + (sided_position.is_white ? 1 : 0)]);
	B64 en_passant_tile;
	if (target_board_bit & sided_position.own_pawns) { // check if the target is a pawn
		en_passant_tile = (sided_position.is_white ? up(target_board_bit) : down(target_board_bit)); // dark pawns have an enpassant ABOVE them
		if (en_passant_tile & sided_position.special_move_rigths) { // check if an en passant is possible
			killing_pawns |= (pawn_attacks[lowest_single_bit_index(en_passant_tile) * 2 + (sided_position.is_white ? 1 : 0)]);
		}
	}
	// finalize pawn attackers
	killing_pawns &= sided_position.own_pawns;

	if (killing_pawns) {
		tile_capture_positions(positions, sided_position, target_board_bit, killing_pawns);
	}
	if (killing_knights) {
		tile_capture_positions(positions, sided_position, target_board_bit, killing_knights);
	}
	if (killing_bishops) {
		tile_capture_positions(positions, sided_position, target_board_bit, killing_bishops);
	}
	if (killing_rooks) {
		tile_capture_positions(positions, sided_position, target_board_bit, killing_rooks);
	}
	if (killing_queens) {
		tile_capture_positions(positions, sided_position, target_board_bit, killing_queens);
	}
}

void tile_move_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target_board_bit, const PieceType piece_type, B64 possible_moves) {
	
	std::vector<B64> single_pieces;
	SidedPosition new_position;
	bool add_move;

	B64& current_pieces = *own_piece_board_ref(new_position, piece_type);
	seperate_bits(current_pieces, single_pieces);

	for (const B64& piece : single_pieces) {
		add_move = false;
		new_position = sided_position; // reset position
		if ((piece_type == PAWN) && // check for pawn jump
			(piece & (new_position.is_white ? WHITE_PAWN_JUMP_START : BLACK_PAWN_JUMP_START)) &&
			(target_board_bit & (new_position.is_white ? WHITE_PAWN_JUMP_END : BLACK_PAWN_JUMP_END))) {
				current_pieces ^= generate_pawn_jump(all_pieces(sided_position), piece, sided_position.is_white);
				add_move = true;
		} else {
			if (possible_moves == NULL) { // only 2 piece types get here
				possible_moves = (piece_type == PAWN ? pawn_moves[lowest_single_bit_index(piece)] : knight_moves[lowest_single_bit_index(piece)]);
			}
			
			if (possible_moves & target_board_bit) {
				current_pieces ^= target_board_bit; // add the piece to the target tile
				add_move = true;
			}
		}

		if (add_move) {
			current_pieces ^= piece; // remove piece from its origin
			new_position.special_move_rigths &= VOID_ALL_EN_PASSANT; // void en passant

			// check pawn promotion
			if (new_position.own_pawns & (new_position.is_white ? WHITE_PAWN_PROMOTION : BLACK_PAWN_PROMOTION)) {
				possible_pawn_promotions(positions, new_position);
			} else {
				positions.push_back(new_position);
			}
		}
	}
}

void moves_to_tiles(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target_board) {
	std::vector<B64> single_targets;
	B64 slide_moves;

	seperate_bits(target_board, single_targets);

	for (const B64& target : single_targets) {
		// maybe an approach like with the kills is better, i'm looping everything here, that can't be good.
		// i'm not sure how to handle multiple origin points at the same time tho, what if a piece can block 2 tiles?
		slide_moves = generate_queen_moves(all_pieces(sided_position), target);
		tile_move_positions(positions, sided_position, target, QUEEN, slide_moves);
		tile_move_positions(positions, sided_position, target, ROOK, slide_moves);
		tile_move_positions(positions, sided_position, target, BISHOP, slide_moves);
		tile_move_positions(positions, sided_position, target, PAWN); // pawns can also block by jumping.
		tile_move_positions(positions, sided_position, target, KNIGHT);
	}
}

void possible_evade_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position) {

	const B64 attackers = attacking_pieces(sided_position, sided_position.own_king); // find attackers of the oposite color
	const B64 blockers = all_pieces(sided_position);
	const B64 not_own = ~own_pieces(sided_position); // valid destinations

	std::vector<B64> pieces;
	std::vector<B64> destinations;
	B64 attack_path;
	B64 potential_moves = 0;

	// add all king moves besides castles
	possible_piece_positions(positions, sided_position, KING, blockers, not_own, nullptr, king_moves);

	if (count_bits64(attackers) == 1) { // its either 1 or 2, given that the position starts in a check

		// a knight's check cannot be blocked, but a single knight can be killed
		if (attackers & sided_position.opponent_knights) {
			kills_to_tile(positions, sided_position, attackers);
		}
		else { // the attacker is a sliding piece, block it or kill it
			attack_path = get_connecting_tiles(attackers, sided_position.own_king); // garanteed to be checked

			moves_to_tiles(positions, sided_position, attack_path);
			kills_to_tile(positions, sided_position, attackers);
		}
	}
}

std::vector<SidedPosition> valid_positions(std::vector<SidedPosition>& valid_positions, const GameState state) {
	std::vector<SidedPosition> all_positions;

	all_positions.reserve(EXPECTED_BRANCHING);

	(is_check(state) ? possible_evade_positions(all_positions, state.sided_position)
					 : all_possible_positions(all_positions, state.sided_position));

	for (const SidedPosition& Sided_position : all_positions) {
		if (!is_check(Sided_position)) {
			valid_positions.push_back(Sided_position);
		}
	}

	return valid_positions;
}