
#include "Move search.h"

GameState generate_state(GameState state, const BoardPosition position) {
    GameState new_state = {};
    bool draw_reset = false;

    new_state.turn = state.turn + 1;
    new_state.previous_state = &state;
    new_state.position = position;

    if (state.position.black_pawns != position.black_pawns || // check if any pawn change has occured
        state.position.white_pawns != position.white_pawns || // any pawn move or capture is a garanteed reset
        is_piece_taken(state, position)) { // check for any captures on the opposing side of the board
        draw_reset = true;
    }

    new_state.draw_timer = (draw_reset ? 0 : state.draw_timer + 1);
    return new_state;
}

GameState generate_state(GameState state, const SidedPosition sided_position) {

    const BoardPosition converted_position = convert_from_sided(sided_position);
    GameState new_state;
    int draw_timer;

    new_state.turn = state.turn + 1;
    new_state.previous_state = &state;
    new_state.position = converted_position;

    if (state.position.black_pawns != converted_position.black_pawns || // check if any pawn change has occured
        state.position.white_pawns != converted_position.white_pawns || // any pawn move or capture is a garanteed reset
        is_piece_taken(state, converted_position)) { // check for any captures on the opposing side of the board
        draw_timer = 0;
    } else {
        draw_timer = state.draw_timer + 1;
    }

    new_state.draw_timer = draw_timer;
    return new_state;
}

bool is_piece_taken(const GameState state, const BoardPosition position) {

    return (is_white_player(state) ? (count_bits64(all_black_pieces(state)) == count_bits64(all_black_pieces(position)))
                                   : (count_bits64(all_white_pieces(state)) == count_bits64(all_white_pieces(position))));
}

int alphabeta(GameState state, int depth, int alpha, int beta) {

    const bool is_white = is_white_player(state);
    std::vector<SidedPosition> positions;
    int eval = 0;

    // end search conditions
    if (depth == 0) {
        eval = material_eval(state.position); // calculate current position // the most basic function is used for now
    } else if (is_checkmate(state.position, is_white)) {
        eval = score_by_player(is_white, -WIN_VALUE); // set value as losing for the current player
    } else if (is_draw(state)) {
        eval = DRAW_VALUE;
    } else {
        positions = all_possible_positions(state.position, is_white);
        // move ordering goes here, better first = more pruning
        // perhaps i can use the "Move" structure to prefer captures, especially with pawns
        // that might reqire considerable proccesing
    }

    // continue search
    for (const SidedPosition& sided_position : positions) {

        // Recursively search the resulting position from the perspective of the opposite player
        eval = std::max(eval, -alphabeta(generate_state(state, sided_position), depth - 1, -beta, -alpha)); // alpha and beta are passed inverted

        // Update alpha
        alpha = std::max(alpha, std::max(eval, WIN_VALUE - 100 * depth)); // last argument to set a clear preference for early wins

        // If alpha is greater than or equal to beta, terminate the branch early
        if (beta <= alpha)
            break;
    }

    // Return the final evaluation
    return eval;
}
