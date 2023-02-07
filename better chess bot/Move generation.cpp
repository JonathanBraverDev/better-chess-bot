
#include "Move generation.h"

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

B64 generate_pawn_jump(const B64 blockers, B64 piece, B64(*direction)(B64)) {
    B64 moves = 0;
    if (!((direction(piece) & blockers) |
        ((direction(direction(piece)) & blockers)))) {
        moves |= direction(direction(piece));
    }

    return moves;
}

inline B64 generate_pawn_jump(const B64 blockers, B64 piece, PlayerColor color) {
    generate_pawn_jump(blockers, piece, (color == WHITE ? &up : &down));
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

void possible_simple_positions(std::vector<BoardPosition> positions, const BoardPosition position, const PlayerColor color, const B64 pieces, const B64 blockers, const B64 valid_destinations, B64(*move_generator)(B64, B64), const B64* move_source, const int index_scale, const int first_index) {
    std::vector<B64> single_pieces;                                                                                                                                                                   
    std::vector<B64> destinations;                                                                                                                                                                     
    BoardPosition new_position = {};                                                                                                                                                                  
    B64 potential_moves = 0;    
    const bool is_white = color == WHITE;

    // shorthand enemy pieces
    const B64* enemy_pawns = (is_white ? &new_position.black_pawns : &new_position.white_pawns);
    const B64* enemy_knights = (is_white ? &new_position.black_knights : &new_position.white_knights);
    const B64* enemy_bishops = (is_white ? &new_position.black_bishops : &new_position.white_bishops);
    const B64* enemy_rooks = (is_white ? &new_position.black_rooks : &new_position.white_rooks);
    const B64* enemy_queens = (is_white ? &new_position.black_queens : &new_position.white_queens);
    const B64* enemy_king = (is_white ? &new_position.black_king : &new_position.white_king);

    seperate_bits(pieces, single_pieces); // get a vector of all pieces
    positions.reserve(count_bits64(pieces) * MAX_QUEEN_MOVES); // streach the vector to the worst case
    for (B64 piece : single_pieces) {
        new_position = position; // reset the new position
        if (move_generator != nullptr) {
            potential_moves = move_generator(blockers, pieces) & valid_destinations; // get the valid moves per piece
        } else {
            potential_moves = move_source[first_index + index_scale * lowestBitIndex64_s(piece)];
        }
        seperate_bits(piece, destinations); // seperate the generated moves
        for (B64 move : destinations) {
            // delete any enemy piece in the destination
            clear_bit(*enemy_pawns, move);
            clear_bit(*enemy_knights, move);
            clear_bit(*enemy_bishops, move);
            clear_bit(*enemy_rooks, move);
            clear_bit(*enemy_queens, move);
            positions.push_back(new_position);
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

    // shorthand own pieces
    const B64 pawns = (is_white ? position.white_pawns : position.black_pawns);
    const B64 knights = (is_white ? position.white_knights : position.black_knights);
    const B64 bishops = (is_white ? position.white_bishops : position.black_bishops);
    const B64 rooks = (is_white ? position.white_rooks : position.black_rooks);
    const B64 queens = (is_white ? position.white_queens : position.black_queens);
    const B64 king = (is_white ? position.white_king : position.black_king);

    if (queens != 0) {
        possible_simple_positions(positions, position, color, queens, blockers, not_own, &generate_queen_moves);
    }
    if (rooks != 0) {
        possible_simple_positions(positions, position, color, queens, blockers, not_own, &generate_rook_moves);
    }
    if (bishops != 0) {
        possible_simple_positions(positions, position, color, queens, blockers, not_own, &generate_bishop_moves);
    }
    if (knights != 0) {
        possible_simple_positions(positions, position, color, queens, blockers, not_own, nullptr, knight_moves);
    }

    // king nornals
    possible_simple_positions(positions, position, color, queens, blockers, not_own, nullptr, king_moves);

    // need non kil pawn moves
    // need castling
    // need pawn kills
    // need en passant pawn kills

    return positions;
}

inline std::vector<BoardPosition> possible_positions(const GameState state) {
    return possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK));
}