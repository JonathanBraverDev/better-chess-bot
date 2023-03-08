
#include "Move search.h"

// returns the next gamestate, also 'flips' the board to the other side
GameState generate_next_state(GameState state, const SidedPosition new_position) {

    GameState new_state;
    int draw_timer;

    new_state.turn = state.turn + 1;
    new_state.previous_state = &state;
    new_state.sided_position = new_position;
    switch_sides(new_state.sided_position);

    if (state.sided_position.own_pawns != new_position.own_pawns || // check if any pawn change has occured
        state.sided_position.opponent_pawns != new_position.opponent_pawns || // any pawn move or capture is a garanteed reset
        was_piece_taken(state.sided_position, new_position)) { // check for any captures on the opposing side of the board
        draw_timer = 0;
    } else {
        draw_timer = state.draw_timer + 1;
    }

    new_state.draw_timer = draw_timer;
    return new_state;
}

bool was_piece_taken(const SidedPosition original_position, const SidedPosition end_turn_position) {

    return (original_position.is_white ? (count_bits64(opponent_pieces(original_position)) == count_bits64(opponent_pieces(end_turn_position)))
                                       : (count_bits64(own_pieces(original_position)) == count_bits64(own_pieces(end_turn_position))));
}

int alphabeta(GameState state, int depth) {
    PreAllocationVectors* depth_vectors = new PreAllocationVectors[depth];

    // reserve space for all vectors, if they end up growing the'll keep their size
    for (int i = 0; i < depth; i++) {
        depth_vectors->all_positions.reserve(EXPECTED_BRANCHING);
        depth_vectors->valid_positions.reserve(EXPECTED_BRANCHING);
        depth_vectors->single_pieces.reserve(EXPECTED_BRANCHING);
        depth_vectors->single_moves.reserve(EXPECTED_BRANCHING);
    }

    return alphabeta(depth_vectors, state, depth, -2 * WIN_VALUE, 2 * WIN_VALUE);
}

int alphabeta(PreAllocationVectors* depth_vectors, GameState state, int depth, int alpha, int beta) {

    // use prealocated vectors, clears happen to relevent vectors in the function that needs them
    PreAllocationVectors& current_vectors = depth_vectors[depth - 1];

    int eval = 0;

    // end search conditions
    if (depth == 0) {
        eval = material_eval(state.sided_position); // calculate current position // the most basic function is used for now
    } else if (is_checkmate(state.sided_position)) { // should never get here, checkmates can be found during the winnign move
        eval = score_by_player(state.sided_position.is_white, -WIN_VALUE); // set value as losing for the current player
    } else if (is_draw(state)) {
        eval = DRAW_VALUE;
    } else { // continue search
        valid_positions(current_vectors, state);
        // move ordering goes here, better first = more pruning
        // perhaps i can use the "Move" structure to prefer captures, especially with pawns
        // that might reqire considerable proccesing

        for (const SidedPosition& sided_position : current_vectors.valid_positions) {

            // Recursively search the resulting position from the perspective of the opposite player, alpha and beta are passed inverted
            eval = std::max(eval, -alphabeta(depth_vectors, generate_next_state(state, sided_position), depth - 1, -beta, -alpha));

            alpha = (state.sided_position.is_white ? std::max(alpha, eval) // min/max by the current player color
                : std::min(alpha, eval));

            // If alpha is greater than or equal to beta, terminate the branch early
            if (beta <= alpha)
                break;
        }
    }

    // Return the final evaluation
    return eval;
}
