
#include "Position generation.h"

bool is_check(const BoardPosition position, const PlayerColor attacker_color) {
	const bool is_white = attacker_color == WHITE;
	const B64 attacked_king = (is_white ? position.black_king : position.white_king);
	const int tile = lowest_single_bit_index(attacked_king);
	B64 slide_attackes;

	bool check = false;

	// perform fastest bit check first
	if ((knight_moves[tile] & (is_white ? position.white_knights : position.black_knights)) ||
		(pawn_attacks[tile] & (is_white ? position.white_pawns : position.black_pawns))) {

		check = true;

	}
	else {
		// can be futher split into multiple checks but this is WAY more readable
		slide_attackes = generate_queen_moves(position.white | position.black, attacked_king);

		check = ((slide_attackes & (is_white ? position.white_queens : position.black_queens)) ||
			(slide_attackes & (is_white ? position.white_rooks : position.black_rooks)) ||
			(slide_attackes & (is_white ? position.white_bishops : position.black_bishops)));
	}

	return check;
}

B64 attacking_pieces(const BoardPosition position, const B64 target_board, const PlayerColor attacker_color) {
	const bool is_white = attacker_color == WHITE;
	const int tile = lowest_single_bit_index(target_board);
	const B64 slide_attackes = generate_queen_moves(position.white | position.black, target_board);

	return (knight_moves[tile] & (is_white ? position.white_knights : position.black_knights)) |
		(pawn_attacks[tile] & (is_white ? position.white_pawns : position.black_pawns)) |
		(king_moves[tile] & (is_white ? position.white_king : position.black_king)) | // irrelevent for checks but quick and generic
		(slide_attackes & (is_white ? position.white_queens : position.black_queens)) |
		(slide_attackes & (is_white ? position.white_rooks : position.black_rooks)) |
		(slide_attackes & (is_white ? position.white_bishops : position.black_bishops));
}

void possible_piece_positions(std::vector<BoardPosition>& positions, const BoardPosition position, const PlayerColor color, const B64 pieces, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
	std::vector<B64> single_pieces;
	std::vector<B64> single_moves;
	BoardPosition new_position = {};
	B64 potential_moves = 0;
	const bool is_white = color == WHITE;

	// pointer tables for both colors
	B64* white_pieces[6] = { &new_position.white_pawns, &new_position.white_knights, &new_position.white_bishops,
							 &new_position.white_rooks, &new_position.white_queens, &new_position.white_king };

	B64* black_pieces[6] = { &new_position.black_pawns, &new_position.black_knights, &new_position.black_bishops,
							 &new_position.black_rooks, &new_position.black_queens, &new_position.black_king };

	// current piece board pointer
	B64& current_pieces = is_white ? *white_pieces[piece_type] : *black_pieces[piece_type];

	// shorthand enemy pieces
	const B64& enemy_pawns = (is_white ? *black_pieces[PAWN] : *white_pieces[PAWN]);
	const B64& enemy_knights = (is_white ? *black_pieces[KNIGHT] : *white_pieces[KNIGHT]);
	const B64& enemy_bishops = (is_white ? *black_pieces[BISHOP] : *white_pieces[BISHOP]);
	const B64& enemy_rooks = (is_white ? *black_pieces[ROOK] : *white_pieces[ROOK]);
	const B64& enemy_queens = (is_white ? *black_pieces[QUEEN] : *white_pieces[QUEEN]);
	const B64& enemy_king = (is_white ? *black_pieces[KING] : *white_pieces[KING]);


	seperate_bits(pieces, single_pieces); // get a vector of all pieces
	for (B64 piece : single_pieces) {
		new_position = position; // reset the new position
		current_pieces ^= piece; // delete the current piece from its origin
		if (move_generator != nullptr) {
			potential_moves = move_generator(blockers, pieces) & valid_destinations; // get the valid moves per piece
		}
		else {
			potential_moves = move_source[first_index + index_scale * lowest_single_bit_index(piece)];
		}
		seperate_bits(potential_moves, single_moves); // seperate the generated moves
		if (move_source != pawn_moves) {
			// handle moves that kill the target
			possible_capture_positions(positions, single_moves, new_position, position, color, piece, current_pieces, enemy_pawns, enemy_knights, enemy_bishops, enemy_rooks, enemy_queens, move_source);
		}
		else {
			// only one regular move is avalible to pawns, it is added if legal and a jump is considered
			if (potential_moves != 0) {
				possible_pawn_move_positions(positions, new_position, position, color, piece, blockers, potential_moves, current_pieces);
			}
		}
	}
}

void possible_pawn_move_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, const BoardPosition position, const PlayerColor color, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces) {
	const bool is_white = color == WHITE;

	new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant as none of the considered next moves use it
	current_pieces ^= potential_moves; // add the current piece to its destination

	if (is_white && (piece & ROW_8) || // check pawn promotion
		!is_white && (piece & ROW_1)) {
		possible_pawn_promotions(positions, position, color);
	}
	else {
		positions.push_back(new_position); // push the normal move

		if (is_white && (piece & ROW_2) || // check if the pawn is on its innitial row
			!is_white && (piece & ROW_7)) {
			current_pieces ^= potential_moves; // remove the piece before the variable is overwritten
			potential_moves = generate_pawn_jump(blockers, piece, color);

			if (potential_moves != 0) { // if a jump is legal, add it
				current_pieces ^= potential_moves;

				new_position.special_move_rigths ^= (is_white ? up(piece) : down(piece)); // add en passant
				// maybe only add it when it can be used? but it needs to be deleted each turn regardless
				positions.push_back(new_position);
			}
		}
	}
}

void possible_capture_positions(std::vector<BoardPosition>& positions, std::vector<B64>& single_moves, BoardPosition& new_position, const BoardPosition position, const PlayerColor color, const B64 piece, B64& current_pieces, const B64& enemy_pawns, const B64& enemy_knights, const B64& enemy_bishops, const B64& enemy_rooks, const B64& enemy_queens, const B64* move_source) {
	const bool is_white = color == WHITE;

	for (B64 move : single_moves) {
		current_pieces ^= move; // add the current piece to its destination
		// delete any enemy piece in the destination
		if ((move & position.special_move_rigths) && move_source == pawn_attacks) { // is en passant is used, remove the pawn
			clear_bit(enemy_pawns, (is_white ? down(piece) : up(piece))); // a white captures below it, a black above
		}
		else {
			clear_bit(enemy_pawns, move);
		}
		clear_bit(enemy_knights, move);
		clear_bit(enemy_bishops, move);
		clear_bit(enemy_rooks, move);
		clear_bit(enemy_queens, move);
		new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant after any (supposed) capture

		if (is_white && (piece & ROW_8) || // check pawn promotion
			!is_white && (piece & ROW_1)) {
			possible_pawn_promotions(positions, position, color);
		}
		else {
			positions.push_back(new_position);
		}
	}
}

void possible_pawn_promotions(std::vector<BoardPosition>& positions, BoardPosition position, const PlayerColor color) {
	const bool is_white = color == WHITE;

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

std::vector<BoardPosition> all_possible_positions(const BoardPosition position, const PlayerColor color) {
	std::vector<BoardPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	BoardPosition new_position = position;
	B64 potential_moves = 0;
	const bool is_white = color == WHITE;
	const B64 blockers = position.white | position.black;
	const B64 not_own = (is_white ? ~position.white : ~position.black); // valid destinations
	const B64 pawn_special = (is_white ? position.black : position.white) | (position.special_move_rigths & ROW_3 & ROW_6); // pawns, lovem

	// shorthand own pieces
	const B64 pawns = (is_white ? position.white_pawns : position.black_pawns);
	const B64 knights = (is_white ? position.white_knights : position.black_knights);
	const B64 bishops = (is_white ? position.white_bishops : position.black_bishops);
	const B64 rooks = (is_white ? position.white_rooks : position.black_rooks);
	const B64 queens = (is_white ? position.white_queens : position.black_queens);
	const B64 king = (is_white ? position.white_king : position.black_king);

	// ordered by number of expected avalible moves (max for queen is 27 vs 28 of 2 roosk but its hihgly unlikley to mater)
	// potentially change to a better preservation estimate
	if (queens != 0) {
		positions.reserve(count_bits64(queens) * MAX_QUEEN_MOVES); // streach the vector to the worst case
		possible_piece_positions(positions, position, color, queens, QUEEN, blockers, not_own, &generate_queen_moves);
	}
	if (rooks != 0) {
		positions.reserve(count_bits64(rooks) * MAX_ROOK_MOVES); // streach the vector to the worst case
		possible_piece_positions(positions, position, color, rooks, ROOK, blockers, not_own, &generate_rook_moves);
	}
	if (bishops != 0) {
		positions.reserve(count_bits64(bishops) * MAX_BISHOP_MOVES);
		possible_piece_positions(positions, position, color, bishops, BISHOP, blockers, not_own, &generate_bishop_moves);
	}
	if (knights != 0) {
		positions.reserve(count_bits64(knights) * MAX_KNIGHT_MOVES);
		possible_piece_positions(positions, position, color, knights, KNIGHT, blockers, not_own, nullptr, knight_moves);
	}
	if (pawns != 0) { // can I just say that I HATE how complicated this piece type is?
		positions.reserve(count_bits64(pawns) * EXPECTED_PAWN_MOVES);
		possible_piece_positions(positions, position, color, knights, KNIGHT, blockers, not_own, nullptr, knight_moves);

		// check for promoted pawns and spit out 4 boards for each
	}

	// king normals, assuming he didn't get brutally murdered (eveluation planned to end on unavaidable check)
	possible_piece_positions(positions, position, color, queens, KING, blockers, not_own, nullptr, king_moves);

	// need castling

	return positions;
	// note that validation for checks dosent happen here
}

// !!! WIP !!!
std::vector<BoardPosition> possible_evade_positions(const BoardPosition position, const PlayerColor attacker_color) {

	std::vector<BoardPosition> positions;

	const bool is_attacker_white = attacker_color == WHITE;
	const B64 attacked_king = (is_attacker_white ? position.black_king : position.white_king);
	const B64 attackers = attacking_pieces(position, attacked_king, attacker_color);
	const B64 possible_king_moves = king_moves[lowest_single_bit_index(attacked_king)] & ~(is_attacker_white ? position.black : position.white);

	B64 attack_path;

	std::vector<BoardPosition> positions;
	std::vector<B64> pieces;
	std::vector<B64> destinations;
	BoardPosition new_position = position;
	B64 potential_moves = 0;
	const B64 blockers = position.white | position.black;
	const B64 not_own = (is_attacker_white ? ~position.white : ~position.black); // valid destinations
	const B64 pawn_special = (is_attacker_white ? position.black : position.white) | (position.special_move_rigths & ROW_3 & ROW_6); // pawns, lovem

	// shorthand own pieces
	const B64 pawns = (is_attacker_white ? position.white_pawns : position.black_pawns);
	const B64 knights = (is_attacker_white ? position.white_knights : position.black_knights);
	const B64 bishops = (is_attacker_white ? position.white_bishops : position.black_bishops);
	const B64 rooks = (is_attacker_white ? position.white_rooks : position.black_rooks);
	const B64 queens = (is_attacker_white ? position.white_queens : position.black_queens);
	const B64 king = (is_attacker_white ? position.white_king : position.black_king);

	// add all king moves

	if (count_bits64(attacked_king) > 1) {
		// stop addign moves
	}
	else {

		// a knight's check cannot be blocked, but a single knight can be killed
		if (attackers & (is_attacker_white ? position.white_knights : position.black_knights)) {

			// needed function:
			// positions with any kill of target at single tile
			// add those kill positions

		}
		else { // the attacker is a sliding piece, block it or kill it
			attack_path = get_connecting_tiles(attackers, attacked_king);

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
		if (!is_check(position, determine_player(state))) {
			valid_positions.push_back(position);
		}
	}

	return valid_positions;
}