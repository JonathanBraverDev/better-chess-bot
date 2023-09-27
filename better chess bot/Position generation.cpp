
#include "Position generation.h"

// prints a human readable version of the position, is A1 being 1(2^0) and H8 a lot (2^64)
void visualize_position(const SidedPosition position) {
	for (int i = 7; i >= 0; i--) { // it's VERY stupid but it works...
		for (int j = 0; j <= 7; j++) {
			if (get_bit(position.own_pawns, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "P " : "p ");
			} else if (get_bit(position.opponent_pawns, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "p " : "P ");
			} else if (get_bit(position.own_knights, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "H " : "h ");
			} else if (get_bit(position.opponent_knights, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "h " : "H ");
			} else if (get_bit(position.own_bishops, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "B " : "b ");
			} else if (get_bit(position.opponent_bishops, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "b " : "B ");
			} else if (get_bit(position.own_rooks, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "R " : "r ");
			} else if (get_bit(position.opponent_rooks, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "r " : "R ");
			} else if (get_bit(position.own_queens, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "Q " : "q ");
			} else if (get_bit(position.opponent_queens, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "q " : "Q ");
			} else if (get_bit(position.own_king, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "K " : "k ");
			} else if (get_bit(position.opponent_king, i * BOARD_SIZE + j)) {
				std::cout << (position.is_white ? "k " : "K ");
			} else {
				std::cout << "_ ";
			}
		}

		std::cout << '\n';
	}

	std::cout << '\n';
}

void possible_piece_moves(SearchPreallocation& allocation, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
	
	// clear out preallocated memory before use
	std::vector<B64>& single_pieces = allocation.single_pieces;

	SidedPosition new_position;

	B64& current_pieces = *own_piece_board_ref(new_position, piece_type);
	const B64 base_pieces = own_piece_board_copy(sided_position, piece_type);
	B64 potential_moves;

	seperate_bits(base_pieces, single_pieces, true); // get a vector of all pieces
	for (const B64& piece : single_pieces) {

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
			possible_capture_moves(allocation, new_position, piece, potential_moves);
		}
	}
}

void possible_pawn_moves(SearchPreallocation& allocation, const SidedPosition& sided_position, const B64 free_tiles, const B64 opponent_pieces) {
	
	// use preallocated memory, clear out everything but the position vector
	std::vector<BitMove>& moves = allocation.all_moves;
	std::vector<B64>& single_pieces = allocation.single_pieces;
	std::vector<B64>& attacks = allocation.single_boards;

	const B64 base_pieces = own_piece_board_copy(sided_position, PAWN);
	const int first_index = (sided_position.is_white ? 0 : 1);
	
	BitMove move_base{}; // zero out a move
	BitMove new_move;

	int move_index;
	B64 normal_move;
	B64 en_passant;
	B64 attack_moves;
	bool promoting; // a pawn can only more towards promotion so any more from the row before is promoting
	B64 jump;
	PieceType captured_type;

	set_white_move(move_base, sided_position.is_white);
	
	seperate_bits(base_pieces, single_pieces, true); // get a vector of all pieces
	for (const B64& piece : single_pieces) {

		set_origin_index(move_base, lowest_single_bit_index(piece)); // shared for all moves

		// calculate all information needed for the moves
		move_index = first_index + 2 * lowest_single_bit_index(piece);
		normal_move = pawn_moves[move_index] & free_tiles;
		en_passant = sided_position.special_move_rigths & (sided_position.is_white ? BLACK_PAWN_EN_PASSANT : WHITE_PAWN_EN_PASSANT);
		attack_moves = pawn_attacks[move_index] & (opponent_pieces | en_passant);
		promoting = piece & (sided_position.is_white ? WHITE_PAWN_PRE_PROMOTION : BLACK_PAWN_PRE_PROMOTION);

		// add normal move if possible
		if (normal_move) {
			
			// only one normal move can exist so its safe to use the 'single' variant
			set_destination_index(move_base, normal_move);

			if (promoting) { // check pawn promotion
				// !!! check check only discovered, then, if not, check the promoted attacks !!!
				possible_pawn_promotions(moves, move_base);
			} else {
				set_moving_type(move_base, PAWN);
				moves.push_back(move_base);

				// then check for jumps
				if (sided_position.own_pawns & (sided_position.is_white ? WHITE_PAWN_JUMP_START : BLACK_PAWN_JUMP_START)) { // check if the pawn is on its initial row
					// waste no information, only check the jump target tile
					jump = free_tiles & (sided_position.is_white ? up(normal_move) : down(normal_move));

					if (jump) { // if a jump is legal, add it
						set_destination_index(move_base, jump);
						moves.push_back(move_base); 
					}
				}
			}
		}
	
		// add captures if any are possible
		if (attack_moves) {
			seperate_bits(attack_moves, attacks, true);

			for (const B64 attack : attacks) {

				// attacks are handles one by one so it's safe to use single variant
				set_destination_index(move_base, attack);

				// the move is guranteed to be a capture, check en passant
				if (en_passant & attack) {
					new_move = copy_changed(move_base, set_full_capture, PAWN);
				} else {
					captured_type = check_capture_type(sided_position, attack);
					if (captured_type != NONE) {
						new_move = copy_changed(move_base, set_full_capture, captured_type);
						if (captured_type == QUEEN ||
							captured_type == ROOK) {
							set_misc_move_type(move_base, GOOD_GENERIC);
						}
					} else {
						new_move = move_base;
					}
				}

				// check pawn promotion, send a capture copy
				if (promoting) { 
					possible_pawn_promotions(moves, new_move);
				} else {
					moves.push_back(new_move);
				}
			}
		}

		// add smart check check (haha) that looks at discovered checkes (rook/bishop hits from mover) or those caused by the moving piece
		// will be faster than a full scan from the king's location and the best time to look for things is when i already have the board
	}
}

void possible_capture_moves(SearchPreallocation& allocation, SidedPosition& sided_position, B64 piece, B64 potential_moves) {
	
	// use preallocated memory, clear out everything but the position vector
	std::vector<BitMove>& moves = allocation.all_moves;
	std::vector<B64>& destination_boards = allocation.single_boards;
	PieceType captured_type;

	BitMove move_base{}; // zero out a move
	BitMove new_move; // might have more properties than the base move
	set_white_move(move_base, sided_position.is_white);
	set_origin_index(move_base, piece);

	seperate_bits(potential_moves, destination_boards, true); // seperate the generated moves
	for (const B64& destination : destination_boards) {
		set_destination_index(move_base, destination);

		captured_type = check_capture_type(sided_position, destination);
		if (captured_type != NONE) {
			new_move = copy_changed(move_base, set_full_capture, captured_type);
			if (captured_type == QUEEN ||
				captured_type == ROOK) {
				set_misc_move_type(new_move, GOOD_GENERIC);
			}
		} else {
			new_move = move_base;
		}

		moves.push_back(move_base);
	}
}

// add all possible promotion moves, the base must have all other properties set
void possible_pawn_promotions(std::vector<BitMove>& moves, BitMove base_move) {

	set_promote(base_move, true);

	moves.push_back(copy_changed(base_move, set_moving_type, KNIGHT));
	moves.push_back(copy_changed(base_move, set_moving_type, BISHOP));
	moves.push_back(copy_changed(base_move, set_moving_type, ROOK));
	// todo: add checks for move rating
	set_misc_move_type(base_move, GOOD_GENERIC);
	moves.push_back(copy_changed(base_move, set_moving_type, QUEEN));
}

bool is_castle_legal(const SidedPosition sided_position, const B64 king_start, const B64 king_end) {
	const B64 king_path = get_connecting_tiles(king_start, king_end); // !!! BUG !!! king start is MULTIPLE BITS

	bool is_legal = false;

	if (king_path & ~(all_pieces(sided_position))) { // check for anything in the king's path
		is_legal = is_attackd_by_side(sided_position, king_path);
	}

	return is_legal;
}

// check all enemy boards for a piece on the targetes square
PieceType check_capture_type(const SidedPosition& sided_position, B64 target) {
	PieceType captured_type = NONE;

	if (sided_position.opponent_pawns & target) {
		captured_type = PAWN;
	} else if (sided_position.opponent_knights & target) {
		captured_type = KNIGHT;
	} else if (sided_position.opponent_bishops & target) {
		captured_type = BISHOP;
	} else if (sided_position.opponent_rooks & target) {
		captured_type = ROOK;
	} else if (sided_position.opponent_queens & target) {
		captured_type = QUEEN;
	}

	return captured_type;
}

void possible_castle_moves(std::vector<BitMove>& moves, SidedPosition sided_position) {

	const B64 short_rook_valid = higher_than_bit(sided_position.own_king) & sided_position.special_move_rigths & sided_position.own_rooks;
	const B64 long_rook_valid = lower_than_bit(sided_position.own_king) & sided_position.special_move_rigths & sided_position.own_rooks;

	BitMove base_move{};
	BitMove new_move;
	set_white_move(base_move, sided_position.is_white);
	set_origin_index(base_move, lowest_single_bit_index(sided_position.own_king));

	sided_position.special_move_rigths &= (sided_position.is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castle rights after checking them

	B64 castled_king;

	if (short_rook_valid) { // check if a caste move is valid
		castled_king = (sided_position.is_white ? WHITE_KING_SHORT_CASTLE : BLACK_KING_SHORT_CASTLE);

		if (is_castle_legal(sided_position, sided_position.own_king, castled_king)) {
			new_move = copy_changed(base_move, set_destination_index, short_rook_valid);
			moves.push_back(new_move);
		}
	}

	if (long_rook_valid) { // check if a caste move is valid
		castled_king = (sided_position.is_white ? WHITE_KING_LONG_CASTLE : BLACK_KING_LONG_CASTLE);

		if (is_castle_legal(sided_position, sided_position.own_king, castled_king)) {
			new_move = copy_changed(base_move, set_destination_index, long_rook_valid);
			moves.push_back(new_move);
		}
	}
}

void all_possible_moves(SearchPreallocation& allocation, const SidedPosition sided_position) {

	const B64 opponent = opponent_pieces(sided_position);
	const B64 own = own_pieces(sided_position);
	const B64 blockers = own | opponent; // for collision detection
	const B64 not_own = ~own; // valid destinations for most pieces

	// for every exisintg piece type, add it's moves
	if (sided_position.own_pawns) { // can I just say that I HATE how complicated this piece type is?
		possible_pawn_moves(allocation, sided_position, ~blockers, opponent);
	}
	if (sided_position.own_knights) {
		possible_piece_moves(allocation, sided_position, KNIGHT, blockers, not_own, nullptr, knight_moves);
	}
	if (sided_position.own_bishops) {
		possible_piece_moves(allocation, sided_position, BISHOP, blockers, not_own, &generate_bishop_moves);
	}
	if (sided_position.own_rooks) {
		possible_piece_moves(allocation, sided_position, ROOK, blockers, not_own, &generate_rook_moves);
	}
	if (sided_position.own_queens) {
		possible_piece_moves(allocation, sided_position, QUEEN, blockers, not_own, &generate_queen_moves);
	}

	// king normals, assuming he didn't get brutally murdered (eveluation planned to end on unavaidable check)
	possible_piece_moves(allocation, sided_position, KING, blockers, not_own, nullptr, king_moves);
	if (sided_position.own_king & sided_position.special_move_rigths) { // add castles is legal
		possible_castle_moves(allocation.all_moves, sided_position);
	}

	// validation for checks dosent happen here
}

// moves that get here are garanteed to be possible, all killing the target one way or another
void tile_capture_positions(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target, B64 killing_pieces, const PieceType piece_type) {

	// use preallocated memory, clear out everything but the position vector
	std::vector<BitMove>& moves = allocation.all_moves;
	std::vector<B64>& single_pieces = allocation.single_pieces;

	B64 en_passant_tile;
	int destination_index;

	BitMove base_move{};
	BitMove new_move;
	set_white_move(base_move, sided_position.is_white);
	set_capture(base_move, true);
	set_captured_type(base_move, check_capture_type(sided_position, target));
	set_moving_type(base_move, piece_type);

	seperate_bits(killing_pieces, single_pieces, true);

	for (const B64& piece : single_pieces) {

		new_move = copy_changed(base_move, set_origin_index, lowest_single_bit_index(piece));

		if (piece_type == PAWN) {
			en_passant_tile = (sided_position.is_white ? up(target) : down(target)); // dark pawns have an enpassant ABOVE them
			if (en_passant_tile & sided_position.special_move_rigths) {
				destination_index = lowest_single_bit_index(en_passant_tile); // add the pawn to the en passant tile
			} else {
				destination_index = lowest_single_bit_index(target);
			}
		} else {
			destination_index = lowest_single_bit_index(target);
		}

		moves.push_back(copy_changed(base_move, set_origin_index, lowest_single_bit_index(piece)));
	}
}

void kills_to_tile(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit) {

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
		tile_capture_positions(allocation, sided_position, target_board_bit, killing_pawns, PAWN);
	}
	if (killing_knights) {
		tile_capture_positions(allocation, sided_position, target_board_bit, killing_knights, KNIGHT);
	}
	if (killing_bishops) {
		tile_capture_positions(allocation, sided_position, target_board_bit, killing_bishops, BISHOP);
	}
	if (killing_rooks) {
		tile_capture_positions(allocation, sided_position, target_board_bit, killing_rooks, ROOK);
	}
	if (killing_queens) {
		tile_capture_positions(allocation, sided_position, target_board_bit, killing_queens, QUEEN);
	}
}

void moves_to_tile(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit, const PieceType piece_type, B64 possible_moves) {
	
	// use preallocated memory, clear out everything but the position vector
	std::vector<BitMove>& moves = allocation.all_moves;
	std::vector<B64>& single_pieces = allocation.single_pieces;

	SidedPosition new_position;
	BitMove new_move;
	bool add_move;

	B64& current_pieces = *own_piece_board_ref(new_position, piece_type);
	seperate_bits(current_pieces, single_pieces, true);

	for (const B64& piece : single_pieces) {
		add_move = false;
		new_position = sided_position; // reset position
		new_move = { 0 }; // clear out move
		if ((piece_type == PAWN) && // check for pawn jump
			(piece & (new_position.is_white ? WHITE_PAWN_JUMP_START : BLACK_PAWN_JUMP_START)) &&
			(target_board_bit & (new_position.is_white ? WHITE_PAWN_JUMP_END : BLACK_PAWN_JUMP_END))) {
			add_move = true;
		} else {
			if (possible_moves == NULL) { // only 2 piece types get here
				possible_moves = (piece_type == PAWN ? pawn_moves[lowest_single_bit_index(piece)] : knight_moves[lowest_single_bit_index(piece)]);
			}
			
			// check if the possible moves hit the target tile
			if (possible_moves & target_board_bit) {
				current_pieces ^= target_board_bit; // add the piece to the target tile
				add_move = true;
			}
		}

		if (add_move) {
			set_moving_type(new_move, piece_type);
			set_origin_index(new_move, lowest_single_bit_index(piece));
			set_destination_index(new_move, lowest_single_bit_index(target_board_bit));

			// check pawn promotion
			if (new_position.own_pawns & (new_position.is_white ? WHITE_PAWN_PROMOTION : BLACK_PAWN_PROMOTION)) {
				possible_pawn_promotions(moves, new_move);
			} else {
				moves.push_back(new_move);
			}
		}
	}
}

void moves_to_tiles(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board) {
	
	// clear out preallocated memory before use
	std::vector<B64>& single_targets = allocation.single_boards;

	B64 slide_moves;
	int target_index;
	int pawn_index;

	seperate_bits(target_board, single_targets, true);

	for (const B64& target : single_targets) {
		// maybe an approach like with the kills is better, i'm looping everything here, that can't be good.
		// i'm not sure how to handle multiple origin points at the same time tho, what if a piece can block 2 tiles?
		slide_moves = generate_queen_moves(all_pieces(sided_position), target);
		target_index = lowest_single_bit_index(target);
		pawn_index = (sided_position.is_white ? 1 : 0) + 2 * target_index; // look for the OPOSITE color of pawn moves
		moves_to_tile(allocation, sided_position, target, QUEEN, slide_moves);
		moves_to_tile(allocation, sided_position, target, ROOK, slide_moves);
		moves_to_tile(allocation, sided_position, target, BISHOP, slide_moves);
		moves_to_tile(allocation, sided_position, target, PAWN, pawn_attacks[pawn_index]); // pawns can also block by jumping.
		moves_to_tile(allocation, sided_position, target, KNIGHT, knight_moves[target_index]);
	}
}

void possible_evade_moves(SearchPreallocation& allocation, const SidedPosition sided_position) {

	const B64 attackers = attacking_pieces(sided_position, sided_position.own_king); // find attackers of the oposite color
	const B64 blockers = all_pieces(sided_position);
	const B64 not_own = ~own_pieces(sided_position); // valid destinations

	B64 attack_path;
	B64 potential_moves = 0;

	// add all king moves besides castles
	possible_piece_moves(allocation, sided_position, KING, blockers, not_own, nullptr, king_moves);

	if (count_bits64(attackers) == 1) { // its either 1 or 2, given that the position starts in a check

		// a knight's check cannot be blocked, but a single knight can be killed
		if (attackers & sided_position.opponent_knights) {
			kills_to_tile(allocation, sided_position, attackers);
		}
		else { // the attacker is a sliding piece, block it or kill it
			attack_path = get_connecting_tiles(attackers, sided_position.own_king); // garanteed to be checked

			moves_to_tiles(allocation, sided_position, attack_path);
			kills_to_tile(allocation, sided_position, attackers);
		}
	}
}

void valid_moves(SearchPreallocation& allocation, std::vector<BitMove>& valid_moves, const SidedPosition sided_position) {

	// use preallocated memory, clear out position vectors
	std::vector<BitMove>& moves = allocation.all_moves;
	valid_moves.clear();
	moves.clear();

	(is_check(sided_position) ? possible_evade_moves(allocation, sided_position)
							  : all_possible_moves(allocation, sided_position));

	for (const BitMove& move : moves) {
		if (!is_check(move)) {
			valid_moves.push_back(move);
		}
	}
}