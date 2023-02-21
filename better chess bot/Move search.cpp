
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

bool is_piece_taken(const GameState state, const BoardPosition position) {

    const bool is_white = determine_player(state) == WHITE;

    return (is_white ? (count_bits64(state.position.black) == count_bits64(position.black))
                     : (count_bits64(state.position.white) == count_bits64(position.white)));
}

int alphabeta(GameState state, int depth, int alpha, int beta) {

    PlayerColor current_player = determine_player(state);
    std::vector<BoardPosition> positions;
    int eval = 0;

    if (depth == 0) {
        eval = material_eval(state.position); // calculate current position // the most basic function is used for now
    } else if (is_checkmate(state.position, current_player)) {
        eval = adjust_by_player(current_player, -WIN_VALUE); // set value as losing for the current player
    } else if (is_draw(state)) {
        eval = 0;
    } else {
        positions = possible_positions(state.position, determine_player(state));
        // move ordering goes here, better first = more pruning
        // perhaps i can use the "Move" structure to prefer captures, especially with pawns
        // that might reqire considerable proccesing
    }

    for (const BoardPosition& position : positions) {

        // Recursively search the resulting position from the perspective of the opposite player
        eval = std::max(eval, -alphabeta(generate_state(state, position), depth - 1, -beta, -alpha)); // alpha and beta are passed inverted

        // Update alpha
        alpha = std::max(alpha, eval, WIN_VALUE - 100 * depth); // last argument to set a clear preference for early wins

        // If alpha is greater than or equal to beta, terminate the branch early
        if (beta <= alpha)
            break;
    }

    // Return the final evaluation
    return eval;
}
