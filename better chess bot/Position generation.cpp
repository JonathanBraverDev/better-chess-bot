
#include "Position generation.h"

void possible_piece_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
	std::vector<B64> single_pieces;
	SidedPosition new_position;

	B64& current_pieces = *own_piece_board(new_position, piece_type);
	const B64 base_pieces = current_pieces;
	B64 potential_moves;

	if (move_source == king_moves && // add king castles if he didnt move yet
		sided_position.own_king & sided_position.special_move_rigths) {
		possible_castle_positions(positions, new_position);
	}

	seperate_bits(base_pieces, single_pieces); // get a vector of all pieces
	for (B64 piece : single_pieces) {
		new_position = sided_position; // reset the new position
		current_pieces = base_pieces ^ piece; // delete the current piece from its origin
		if (move_generator != nullptr) {
			potential_moves = move_generator(blockers, base_pieces) & valid_destinations; // get the valid moves per piece
			if (piece_type == ROOK) {
				new_position.special_move_rigths ^= piece & new_position.special_move_rigths & (sided_position.is_white ? ROW_1 : ROW_8); // void castle right of a moving rook
			}
		} else {
			potential_moves = move_source[first_index + index_scale * lowest_single_bit_index(piece)] & valid_destinations;
		}

		if (potential_moves) {
			// handle moves that kill the target
			possible_capture_positions(positions, new_position, potential_moves, piece, current_pieces, move_source);
			if (move_source != pawn_moves) { // only one regular move is avalible to pawns, it is added if legal and a jump is considered
				possible_pawn_move_positions(positions, new_position, piece, blockers, potential_moves, current_pieces);
			}
		}
	}
}

void possible_pawn_move_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces) {

	SidedPosition new_position = sided_position;

	new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant as none of the considered next moves use it
	current_pieces ^= potential_moves; // add the current piece to its destination

	if (new_position.own_pawns & (new_position.is_white ? ROW_8 : ROW_1)) { // check pawn promotion
		possible_pawn_promotions(positions, sided_position);
	} else {
		positions.push_back(new_position); // push the normal move

		if (new_position.own_pawns & (new_position.is_white ? ROW_8 : ROW_1)) { // check if the pawn is on its innitial row
			current_pieces ^= potential_moves; // remove the piece before the variable is overwritten
			potential_moves = generate_pawn_jump(blockers, piece, new_position.is_white);

			if (potential_moves != 0) { // if a jump is legal, add it
				current_pieces ^= potential_moves;

				new_position.special_move_rigths ^= (new_position.is_white ? up(piece) : down(piece)); // add en passant
				// maybe only add it when it can be used? but it needs to be deleted each turn regardless
				positions.push_back(new_position);
			}
		}
	}
}

void possible_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, B64 potential_moves, const B64 piece, B64& current_pieces, const B64* move_source) {
	
	SidedPosition new_position;

	std::vector<B64> single_moves;
	seperate_bits(potential_moves, single_moves); // seperate the generated moves

	for (B64 move : single_moves) {
		new_position = sided_position;
		current_pieces ^= move; // add the current piece to its destination
		// delete any enemy piece in the destination
		if ((move & sided_position.special_move_rigths) && move_source == pawn_attacks) { // is en passant is used, remove the pawn
			clear_bit(new_position.opponent_pawns, (new_position.is_white ? up(move) : down(move))); // white is captured from below it, black above
		} else {
			// clear whatever was there if not en passant
			clear_bit(new_position.opponent_pawns, move);
			clear_bit(new_position.opponent_knights, move);
			clear_bit(new_position.opponent_bishops, move);
			clear_bit(new_position.opponent_rooks, move);
			clear_bit(new_position.opponent_queens, move);
		}
		new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant after any (supposed) capture

		if (new_position.own_pawns & (new_position.is_white ? ROW_8 : ROW_1)) { // check pawn promotion
			possible_pawn_promotions(positions, sided_position);
		} else {
			positions.push_back(new_position);
		}
	}
}

void possible_pawn_promotions(std::vector<SidedPosition>& positions, SidedPosition sided_position) {

	// find the promoted pawn and remove it
	const B64 promote_tile = (sided_position.own_pawns & (sided_position.is_white ? ROW_8 : ROW_1));
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

void possible_castle_positions(std::vector<SidedPosition>& positions, SidedPosition& sided_position) {

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

std::vector<SidedPosition> all_possible_positions(const SidedPosition sided_position) {
	std::vector<SidedPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	const B64 blockers = all_pieces(sided_position);
	const B64 not_own = ~own_pieces(sided_position); // valid destinations
	B64 en_passant; // pawns, lovem

	// ordered by number of expected avalible moves
	positions.reserve(EXPECTED_BRANCHING);
	if (sided_position.own_pawns) { // can I just say that I HATE how complicated this piece type is?
		en_passant = sided_position.special_move_rigths & (sided_position.is_white ? ROW_6 : ROW_3); // assuming perfect updae of en passant
		possible_piece_positions(positions, sided_position, PAWN, blockers, not_own | en_passant, nullptr, pawn_moves);
		// promotions handled in the general function
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
	// castling handled in the general function

	return positions;
	// note that validation for checks dosent happen here
}

// moves that get here are garanteed to be possible, all killing hte target one way or another
void tile_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target, const PieceType piece_type) {

	std::vector<B64> single_pieces;
	SidedPosition new_position;
	B64 en_passant_tile;

	B64& current_pieces = *own_piece_board(new_position, piece_type);

	seperate_bits(current_pieces, single_pieces);

	for (const B64& piece : single_pieces) {
		new_position = sided_position; // reset position
		current_pieces ^= piece; // remove piece from its origin
		// delete any enemy piece in the destination
		if (&current_pieces == &sided_position.own_pawns) { // check the adress' to see if te pieces are pawns
			en_passant_tile = (sided_position.is_white ? up(target) : down(target)); // dark pawns have an enpassant ABOVE them
			if (en_passant_tile & new_position.special_move_rigths) {
				current_pieces ^= en_passant_tile; // add the pawn to the en passant tile
			}
		} else {
			current_pieces ^= target; // add the piece to the target tile
		}

		// clear the target
		clear_bit(new_position.opponent_pawns, target);
		clear_bit(new_position.opponent_knights, target);
		clear_bit(new_position.opponent_bishops, target);
		clear_bit(new_position.opponent_rooks, target);
		clear_bit(new_position.opponent_queens, target);

		new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant

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
	const B64 killing_king = (king_moves[tile_index] & sided_position.own_king);

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
		tile_capture_positions(positions, sided_position, target_board_bit, PAWN);
	}
	if (killing_knights) {
		tile_capture_positions(positions, sided_position, target_board_bit, KNIGHT);
	}
	if (killing_bishops) {
		tile_capture_positions(positions, sided_position, target_board_bit, BISHOP);
	}
	if (killing_rooks) {
		tile_capture_positions(positions, sided_position, target_board_bit, ROOK);
	}
	if (killing_queens) {
		tile_capture_positions(positions, sided_position, target_board_bit, QUEEN);
	}
	if (killing_king) {
		tile_capture_positions(positions, sided_position, target_board_bit, KING);
	}
}

// !!! WIP !!! - literly everything
std::vector<SidedPosition> possible_evade_positions(const SidedPosition sided_position) {

	const B64 attackers = attacking_pieces(sided_position, sided_position.own_king); // find attackers of the oposite color
	const B64 possible_king_moves = king_moves[lowest_single_bit_index(sided_position.own_king)] & ~all_pieces(sided_position) ; // non kill moves, they are added elsewhere

	B64 attack_path;

	std::vector<SidedPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	B64 potential_moves = 0;
	const B64 blockers = all_pieces(sided_position);
	const B64 not_own = ~own_pieces(sided_position); // valid destinations

	// add all king moves

	if (count_bits64(attackers) > 1) {
		// king kills
	}
	else {

		// a knight's check cannot be blocked, but a single knight can be killed
		if (attackers & sided_position.opponent_knights) {
			kills_to_tile(positions, sided_position, attackers);
		}
		else { // the attacker is a sliding piece, block it or kill it
			attack_path = get_connecting_tiles(attackers, sided_position.own_king); // garanteed to be checked

			// needed functions:
			// positions with any non-king move to one of X tiles on a borad
			// add those move positions
			kills_to_tile(positions, sided_position, attackers);
		}
	}

	return positions;
}

std::vector<SidedPosition> valid_positions(const GameState state) {

	std::vector<SidedPosition> all_positions = (is_check(state) ? possible_evade_positions(state.sided_position)
																: all_possible_positions(state.sided_position));
	std::vector<SidedPosition> valid_positions;

	for (const SidedPosition& Sided_position : all_positions) {
		if (!is_check(Sided_position)) {
			valid_positions.push_back(Sided_position);
		}
	}

	return valid_positions;
}