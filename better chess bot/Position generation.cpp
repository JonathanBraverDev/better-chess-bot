
#include "Position generation.h"

bool is_castle_legal(const BoardPosition position, const bool is_king_white, const B64 king_start, const B64 king_end) {
	const B64 king_path = get_connecting_tiles(king_start, king_end);

	bool is_legal = false;

	if (king_path & ~(position.white | position.black)) { // check for anything in the king's path
		is_legal = is_attackd_by_color(position, king_path, !is_king_white);
	}

	return is_legal;
}

void possible_piece_positions(std::vector<BoardPosition>& positions, const BoardPosition position, const bool is_white, const B64 pieces, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
	std::vector<B64> single_pieces;
	BoardPosition new_position = {};
	B64 potential_moves = 0;

	// pointer tables for both colors
	B64* white_pieces[6] = { &new_position.white_pawns, &new_position.white_knights, &new_position.white_bishops,
							 &new_position.white_rooks, &new_position.white_queens, &new_position.white_king };

	B64* black_pieces[6] = { &new_position.black_pawns, &new_position.black_knights, &new_position.black_bishops,
							 &new_position.black_rooks, &new_position.black_queens, &new_position.black_king };

	// current piece board pointer
	B64& current_pieces = (is_white ? *white_pieces[piece_type] : *black_pieces[piece_type]);

	// shorthand enemy pieces
	const B64& enemy_pawns = (is_white ? *black_pieces[PAWN] : *white_pieces[PAWN]);
	const B64& enemy_knights = (is_white ? *black_pieces[KNIGHT] : *white_pieces[KNIGHT]);
	const B64& enemy_bishops = (is_white ? *black_pieces[BISHOP] : *white_pieces[BISHOP]);
	const B64& enemy_rooks = (is_white ? *black_pieces[ROOK] : *white_pieces[ROOK]);
	const B64& enemy_queens = (is_white ? *black_pieces[QUEEN] : *white_pieces[QUEEN]);
	const B64& enemy_king = (is_white ? *black_pieces[KING] : *white_pieces[KING]);

	if (move_source == king_moves) { // add king castles if needed
		new_position = position;
		new_position.special_move_rigths &= (is_white ? VOID_WHITE_CASTLE : VOID_BLACK_CASTLE); // void all castle rights on any kign move
		possible_castle_positions(positions, new_position, is_white, pieces);
	}

	seperate_bits(pieces, single_pieces); // get a vector of all pieces
	for (B64 piece : single_pieces) {
		new_position = position; // reset the new position
		current_pieces ^= piece; // delete the current piece from its origin
		if (move_generator != nullptr) {
			potential_moves = move_generator(blockers, pieces) & valid_destinations; // get the valid moves per piece
			if (piece_type == ROOK) {
				new_position.special_move_rigths ^= piece & new_position.special_move_rigths & (is_white ? ROW_1 : ROW_8); // void castle right of a moving rook
			}
		} else {
			potential_moves = move_source[first_index + index_scale * lowest_single_bit_index(piece)] & valid_destinations;
		}

		if (potential_moves != 0) {
			if (move_source != pawn_moves) { // handle moves that kill the target
				possible_capture_positions(positions, new_position, potential_moves, is_white, piece, current_pieces, enemy_pawns, enemy_knights, enemy_bishops, enemy_rooks, enemy_queens, move_source);
			} else { // only one regular move is avalible to pawns, it is added if legal and a jump is considered
				possible_pawn_move_positions(positions, new_position, is_white, piece, blockers, potential_moves, current_pieces);
			}
		}
	}
}

void possible_pawn_move_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, const bool is_white, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces) {

	const BoardPosition base_position = new_position;

	new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant as none of the considered next moves use it
	current_pieces ^= potential_moves; // add the current piece to its destination

	if (is_white && (piece & ROW_8) || // check pawn promotion
		!is_white && (piece & ROW_1)) {
		possible_pawn_promotions(positions, base_position, is_white);
	} else {
		positions.push_back(new_position); // push the normal move

		if (is_white && (piece & ROW_2) || // check if the pawn is on its innitial row
			!is_white && (piece & ROW_7)) {
			current_pieces ^= potential_moves; // remove the piece before the variable is overwritten
			potential_moves = generate_pawn_jump(blockers, piece, is_white);

			if (potential_moves != 0) { // if a jump is legal, add it
				current_pieces ^= potential_moves;

				new_position.special_move_rigths ^= (is_white ? up(piece) : down(piece)); // add en passant
				// maybe only add it when it can be used? but it needs to be deleted each turn regardless
				positions.push_back(new_position);
			}
		}
	}
}

void possible_capture_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, B64 potential_moves, const bool is_white, const B64 piece, B64& current_pieces, const B64& enemy_pawns, const B64& enemy_knights, const B64& enemy_bishops, const B64& enemy_rooks, const B64& enemy_queens, const B64* move_source) {

	const BoardPosition base_position = new_position;

	std::vector<B64> single_moves;
	seperate_bits(potential_moves, single_moves); // seperate the generated moves

	for (B64 move : single_moves) {
		new_position = base_position;
		current_pieces ^= move; // add the current piece to its destination
		// delete any enemy piece in the destination
		if ((move & base_position.special_move_rigths) && move_source == pawn_attacks) { // is en passant is used, remove the pawn
			clear_bit(enemy_pawns, (is_white ? down(piece) : up(piece))); // a white captures below it, a black above
		} else {
			// clear whatever was there if not en passant
			clear_bit(enemy_pawns, move);
		clear_bit(enemy_knights, move);
		clear_bit(enemy_bishops, move);
		clear_bit(enemy_rooks, move);
		clear_bit(enemy_queens, move);
		}
		new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant after any (supposed) capture

		if (is_white && (piece & ROW_8) || // check pawn promotion
			!is_white && (piece & ROW_1)) {
			possible_pawn_promotions(positions, base_position, is_white);
		} else {
			positions.push_back(new_position);
		}
	}
}

void possible_pawn_promotions(std::vector<BoardPosition>& positions, BoardPosition position, const bool is_white) {

	// find the promoted pawn and remove it
	const B64 promoted = is_white ? (position.white_pawns & ROW_1) : (position.black_pawns & ROW_8);
	(is_white ? position.white_pawns : position.black_pawns) ^= promoted;

	// add the a position with each possible promotion to the vector and remove the piece after
	// ugly but effective
	(is_white ? position.white_queens : position.black_queens) ^= promoted;
	positions.push_back(position); // add queen
	(is_white ? position.white_queens : position.black_queens) ^= promoted;

	(is_white ? position.white_rooks : position.black_rooks) ^= promoted;
	positions.push_back(position); // add rook
	(is_white ? position.white_rooks : position.black_rooks) ^= promoted;

	(is_white ? position.white_bishops : position.black_bishops) ^= promoted;
	positions.push_back(position); // add bishop
	(is_white ? position.white_bishops : position.black_bishops) ^= promoted;

	(is_white ? position.white_knights : position.black_knights) ^= promoted;
	positions.push_back(position); // add knight
}

void possible_castle_positions(std::vector<BoardPosition>& positions, BoardPosition& position, const bool is_white, const B64 king) {

	const B64 rooks = (is_white ? position.white_rooks : position.black_rooks);

	const B64 short_rook_valid = higher_than_bit(king) & position.special_move_rigths & rooks;
	const B64 long_rook_valid = lower_than_bit(king) & position.special_move_rigths & rooks;

	B64& new_king = (is_white ? position.white_king : position.white_king);
	B64& new_rooks = (is_white ? position.white_rooks : position.black_rooks);

	if (short_rook_valid) { // check if a caste move is valid
		new_king = (is_white ? WHITE_KING_SHORT_CASTLE : BLACK_KING_SHORT_CASTLE);

		if (is_castle_legal(position, is_white, king, new_king)) {
			new_rooks = rooks ^ (short_rook_valid | (is_white ? WHITE_ROOK_SHORT_CASTLE : BLACK_ROOK_SHORT_CASTLE)); // flip both old and new location
			positions.push_back(position);
		}
	}

	if (long_rook_valid) { // check if a caste move is valid
		new_king = (is_white ? WHITE_KING_LONG_CASTLE : BLACK_KING_LONG_CASTLE);

		if (is_castle_legal(position, is_white, king, new_king)) {
			new_rooks ^= long_rook_valid | (is_white ? WHITE_ROOK_LONG_CASTLE : BLACK_ROOK_LONG_CASTLE);
			positions.push_back(position);
		}
	}
}

std::vector<BoardPosition> all_possible_positions(const BoardPosition position, const bool is_white) {
	std::vector<BoardPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	BoardPosition new_position = position;
	B64 potential_moves = 0;
	const B64 blockers = position.white | position.black;
	const B64 not_own = (is_white ? ~position.white : ~position.black); // valid destinations
	const B64 en_passant = (is_white ? position.black : position.white) | (position.special_move_rigths & ROW_3 & ROW_6); // pawns, lovem

	// shorthand own pieces
	const B64 pawns = (is_white ? position.white_pawns : position.black_pawns);
	const B64 knights = (is_white ? position.white_knights : position.black_knights);
	const B64 bishops = (is_white ? position.white_bishops : position.black_bishops);
	const B64 rooks = (is_white ? position.white_rooks : position.black_rooks);
	const B64 queens = (is_white ? position.white_queens : position.black_queens);
	const B64 king = (is_white ? position.white_king : position.black_king);

	// ordered by number of expected avalible moves (max for queen is 27 vs 28 of 2 roosk but its highly unlikley to mater)
	// potentially change to a better preservation estimate
	positions.reserve(EXPECTED_BRANCHING);
	if (queens) {
		possible_piece_positions(positions, position, is_white, queens, QUEEN, blockers, not_own, &generate_queen_moves);
	}
	if (rooks) {
		possible_piece_positions(positions, position, is_white, rooks, ROOK, blockers, not_own, &generate_rook_moves);
	}
	if (bishops) {
		possible_piece_positions(positions, position, is_white, bishops, BISHOP, blockers, not_own, &generate_bishop_moves);
	}
	if (knights) {
		possible_piece_positions(positions, position, is_white, knights, KNIGHT, blockers, not_own, nullptr, knight_moves);
	}
	if (pawns) { // can I just say that I HATE how complicated this piece type is?
		possible_piece_positions(positions, position, is_white, pawns, PAWN, blockers, not_own | en_passant, nullptr, knight_moves);
		// promotions handled in the general function
	}

	// king normals, assuming he didn't get brutally murdered (eveluation planned to end on unavaidable check)
	possible_piece_positions(positions, position, is_white, queens, KING, blockers, not_own, nullptr, king_moves);
	// castling handled in the general function

	return positions;
	// note that validation for checks dosent happen here
}

void kills_to_tile(std::vector<BoardPosition>& positions, const BoardPosition position, const B64 target, const bool is_white) {

	// shorthand own pieces
	const B64 pawns = (is_white ? position.white_pawns : position.black_pawns);
	const B64 knights = (is_white ? position.white_knights : position.black_knights);
	const B64 bishops = (is_white ? position.white_bishops : position.black_bishops);
	const B64 rooks = (is_white ? position.white_rooks : position.black_rooks);
	const B64 queens = (is_white ? position.white_queens : position.black_queens);
	const B64 king = (is_white ? position.white_king : position.black_king); // king starts in check

	const int target_index = lowest_single_bit_index(target);
	const B64 sliding_attacks = generate_queen_moves(position.white & position.black, target);

	const B64 blockers = position.white | position.black;
	const B64 not_own = (is_white ? ~position.white : ~position.black); // valid destinations
	const B64 en_passant = (is_white ? position.black : position.white) | (position.special_move_rigths & ROW_3 & ROW_6); // pawns, lovem


	if (queens) {
		possible_piece_positions(positions, position, is_white, queens, QUEEN, blockers, not_own, &generate_queen_moves);
	}
	if (rooks) {
		possible_piece_positions(positions, position, is_white, rooks, ROOK, blockers, not_own, &generate_rook_moves);
	}
	if (bishops) {
		possible_piece_positions(positions, position, is_white, bishops, BISHOP, blockers, not_own, &generate_bishop_moves);
	}
	if (knights) {
		possible_piece_positions(positions, position, is_white, knights, KNIGHT, blockers, not_own, nullptr, knight_moves);
	}
	if (pawns) {
		possible_piece_positions(positions, position, is_white, knights, PAWN, blockers, not_own, nullptr, knight_moves);

		// check for promoted pawns and spit out 4 boards for each
	}
}

// !!! WIP !!! - literly everything
std::vector<BoardPosition> possible_evade_positions(const BoardPosition position, const bool is_defender_white) {

	// shorthand own pieces
	const B64 pawns = (is_defender_white ? position.white_pawns : position.black_pawns);
	const B64 knights = (is_defender_white ? position.white_knights : position.black_knights);
	const B64 bishops = (is_defender_white ? position.white_bishops : position.black_bishops);
	const B64 rooks = (is_defender_white ? position.white_rooks : position.black_rooks);
	const B64 queens = (is_defender_white ? position.white_queens : position.black_queens);
	const B64 king = (is_defender_white ? position.white_king : position.black_king); // king starts in check

	const B64 attackers = attacking_pieces(position, king, (is_defender_white ? BLACK : WHITE)); // find attackers of the oposite color
	const B64 possible_king_moves = king_moves[lowest_single_bit_index(king)] & ~(position.black & position.white); // non kill moves, they are added elsewhere

	B64 attack_path;

	std::vector<BoardPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	BoardPosition new_position = position;
	B64 potential_moves = 0;
	const B64 blockers = position.white | position.black;
	const B64 not_own = (is_defender_white ? ~position.white : ~position.black); // valid destinations
	const B64 pawn_special = (is_defender_white ? position.black : position.white) | (position.special_move_rigths & ROW_3 & ROW_6); // pawns, lovem

	// add all king moves

	if (count_bits64(attackers) > 1) {
		// add king kills
	}
	else {

		// a knight's check cannot be blocked, but a single knight can be killed
		if (attackers & (is_defender_white ? position.white_knights : position.black_knights)) {

			// needed function:
			// positions with any kill of target at single tile
			// add those kill positions

		}
		else { // the attacker is a sliding piece, block it or kill it
			attack_path = get_connecting_tiles(attackers, king); // garanteed to be checked

			// needed functions:
			// positions with any move to one of X tiles on a borad
			// add those move positions
			// positions with any kill of target at single tile
			// add those kill positions
		}
	}

	return positions;
}

std::vector<BoardPosition> valid_positions(const GameState state) {

	std::vector<BoardPosition> all_positions = (is_check(state) ? possible_evade_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK))
																: all_possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)));
	std::vector<BoardPosition> valid_positions;

	for (const BoardPosition& position : all_positions) {
		if (!is_check(position, is_white_player(state))) {
			valid_positions.push_back(position);
		}
	}

	return valid_positions;
}