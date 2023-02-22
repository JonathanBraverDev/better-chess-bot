
#include "Board generation.h"

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
B64 generate_pawn_jump(const B64 blockers, B64 piece, B64(*direction)(B64)) {
	B64 moves = 0;
	if (!((direction(piece) & blockers) |
		((direction(direction(piece)) & blockers)))) {
		moves |= direction(direction(piece));
	}

	return moves;
}

inline B64 generate_pawn_jump(const B64 blockers, B64 piece, PlayerColor color) {
	return generate_pawn_jump(blockers, piece, (color == WHITE ? &up : &down));
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
	for (size_t i = 0; i < 64; i++) {
		knight_moves[i] = (((current_board >> 6) | (current_board << 10)) & ~COLUMN_GH) | // when looping around the board on the other side
			(((current_board >> 10) | (current_board << 6)) & ~COLUMN_AB) |
			(((current_board >> 15) | (current_board << 17)) & ~COLUMN_H) |
			(((current_board >> 17) | (current_board << 15)) & ~COLUMN_A);

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

void possible_capture_positions(std::vector<BoardPosition>& positions, std::vector<B64>& single_moves, BoardPosition &new_position, const BoardPosition position, const PlayerColor color, const B64 piece, B64* current_pieces, const B64* enemy_pawns, const B64* enemy_knights, const B64* enemy_bishops, const B64* enemy_rooks, const B64* enemy_queens, const B64* move_source) {
	const bool is_white = color == WHITE;

	for (B64 move : single_moves) {
        *current_pieces ^= move; // add the current piece to its destination
        // delete any enemy piece in the destination
        if ((move & position.special_move_rigths) && move_source == pawn_attacks) { // is en passant is used, remove the pawn
            clear_bit(*enemy_pawns, (is_white ? down(piece) : up(piece))); // a white captures below it, a black above
        } else {
            clear_bit(*enemy_pawns, move);
        }
        clear_bit(*enemy_knights, move);
        clear_bit(*enemy_bishops, move);
        clear_bit(*enemy_rooks, move);
        clear_bit(*enemy_queens, move);
        new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant after any (supposed) capture

		if (is_white && (piece & ROW_8) || // check pawn promotion
			!is_white && (piece & ROW_1)) {
			possible_pawn_promotions(positions, position, color);
		} else {
			positions.push_back(new_position);
		}
    }
}

void possible_pawn_move_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, const BoardPosition position, const PlayerColor color, const B64 piece, const B64 blockers, B64 potential_moves ,B64* current_pieces) {
	const bool is_white = color == WHITE;

	new_position.special_move_rigths &= VOID_EN_PASSANT; // void en passant as none of the considered next moves use it
	*current_pieces ^= potential_moves; // add the current piece to its destination

	if (is_white && (piece & ROW_8) || // check pawn promotion
		!is_white && (piece & ROW_1)) {
		possible_pawn_promotions(positions, position, color);
	} else {
		positions.push_back(new_position); // push the normal move

		if (is_white && (piece & ROW_2) || // check if the pawn is on its innitial row
			!is_white && (piece & ROW_7)) {
			*current_pieces ^= potential_moves; // remove the piece before the variable is overwritten
			potential_moves = generate_pawn_jump(blockers, piece, color);

			if (potential_moves != 0) { // if a jump is legal, add it
				*current_pieces ^= potential_moves;

				new_position.special_move_rigths ^= (is_white ? up(piece) : down(piece)); // add en passant
				// maybe only add it when it can be used? but it needs to be deleted each turn regardless
				positions.push_back(new_position);
			}
		}
	}
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
    B64* current_pieces = is_white ? white_pieces[piece_type] : black_pieces[piece_type];

    // shorthand enemy pieces
    const B64* enemy_pawns = (is_white ? black_pieces[PAWN] : white_pieces[PAWN]);
    const B64* enemy_knights = (is_white ? black_pieces[KNIGHT] : white_pieces[KNIGHT]);
    const B64* enemy_bishops = (is_white ? black_pieces[BISHOP] : white_pieces[BISHOP]);
    const B64* enemy_rooks = (is_white ? black_pieces[ROOK] : white_pieces[ROOK]);
    const B64* enemy_queens = (is_white ? black_pieces[QUEEN] : white_pieces[QUEEN]);
    const B64* enemy_king = (is_white ? black_pieces[KING] : white_pieces[KING]);


    seperate_bits(pieces, single_pieces); // get a vector of all pieces
    for (B64 piece : single_pieces) {
        new_position = position; // reset the new position
        *current_pieces ^= piece; // delete the current piece from its origin
        if (move_generator != nullptr) {
            potential_moves = move_generator(blockers, pieces) & valid_destinations; // get the valid moves per piece
        } else {
            potential_moves = move_source[first_index + index_scale * lowest_single_bit_index(piece)];
        }
        seperate_bits(potential_moves, single_moves); // seperate the generated moves
        if (move_source != pawn_moves) {
            // handle moves that kill the target
            possible_capture_positions(positions, single_moves, new_position, position, color, piece, current_pieces, enemy_pawns, enemy_knights, enemy_bishops, enemy_rooks, enemy_queens, move_source);
        } else {
            // only one regular move is avalible to pawns, it is added if legal and a jump is considered
            if (potential_moves != 0) {
                possible_pawn_move_positions(positions, new_position, position, color, piece, blockers, potential_moves, current_pieces);
            }
        }
    }
}

std::vector<BoardPosition> possible_positions(const BoardPosition position, const PlayerColor color) {
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

inline std::vector<BoardPosition> possible_positions(const GameState state) {
	return possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK));
}