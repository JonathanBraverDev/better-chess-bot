
#include "Move search.h"

unsigned long long cutoffs = 0;
unsigned long long evals = 0;
unsigned long long nodes = 0;

// returns the next gamestate, also 'flips' the board to the other side
GameState generate_next_state(GameState state, const SidedPosition new_position) {

    // use move type to determine timer upon capture/promotiom
    // use captured/promoted piece to keep a running tally of the raw material balance
    GameState new_state;
    int draw_timer;

    new_state.turn = state.turn + 1;
    new_state.previous_state = &state;
    new_state.sided_position = new_position;
    switch_sides(new_state.sided_position);

    if (state.draw_timer == DRAW_MOVE_LIMIT - 1) { // check only needs to happen when the game is about to end
        draw_timer = find_last_draw_reset(state);
        (draw_timer == -1 ? draw_timer = DRAW_MOVE_LIMIT : NULL);
    } else {
        draw_timer = state.draw_timer + 1;
    }

    new_state.draw_timer = draw_timer;
    return new_state;
}

bool was_draw_reset(const SidedPosition original_position, const SidedPosition new_position) {

    return original_position.own_pawns      != new_position.own_pawns || // check if any pawn change has occured
           original_position.opponent_pawns != new_position.opponent_pawns || // any pawn move or capture is a garanteed reset
           // check for any captures, potential optimization to only check one side
           count_bits64(original_position.own_knights)      == count_bits64(new_position.own_knights) ||
           count_bits64(original_position.opponent_knights) == count_bits64(new_position.opponent_knights) ||
           count_bits64(original_position.own_bishops)      == count_bits64(new_position.own_bishops) ||
           count_bits64(original_position.opponent_bishops) == count_bits64(new_position.opponent_bishops) ||
           count_bits64(original_position.own_rooks)        == count_bits64(new_position.own_rooks) ||
           count_bits64(original_position.opponent_rooks)   == count_bits64(new_position.opponent_rooks) ||
           count_bits64(original_position.own_queens)       == count_bits64(new_position.own_queens) ||
           count_bits64(original_position.opponent_queens)  == count_bits64(new_position.opponent_queens);
           // king cannot be killed / missing
}

int find_last_draw_reset(const GameState& current_state) {
    GameState** state_arr = new GameState*[DRAW_MOVE_LIMIT];
    GameState* previous_state = current_state.previous_state;

    int min_range = 0; // min range for binary search
    int max_range = DRAW_MOVE_LIMIT - 1;
    int next_check; // index of next check
    int last_initialized = max_range; // lowest index in the array to be filled

    // worst case here is index 0 false cuse the penalty on pointers

    // untill the search area collapses
    while (min_range <= max_range) {
        next_check = (min_range + max_range) / 2;

        if (next_check < last_initialized) {
            // fill the array leading up to the check index
            for (; last_initialized >= next_check; last_initialized--) {
                state_arr[DRAW_MOVE_LIMIT - last_initialized - 1] = previous_state;
                previous_state = previous_state->previous_state;
            }
        }

        // adjsut range based on check
        if (was_draw_reset(state_arr[next_check]->sided_position, current_state.sided_position)) {
            max_range = next_check;
        } else {
            min_range = next_check + 1;
        }
    }

    delete[] state_arr;

    return (min_range == max_range ? max_range : -1);
}

// run the first search, recordign the moves 
PositionScore alphabeta_init(GameState state, int depth) {
    SearchPreallocation allocation;
    int opponent_eval;
    PositionScore final_eval;
    // create the valid move array
    allocation.valid_positions = new std::vector<SidedPosition>[depth];
    std::vector<SidedPosition>& next_positions = allocation.valid_positions[depth - 1];

    // end search conditions
    if (depth == 0) {
        final_eval.current_score = material_eval(state.sided_position); // calculate current position // the most basic function is used for now
        final_eval.next_position = state.sided_position;
        evals++;
    } else if (is_checkmate(state.sided_position)) { // should never get here, checkmates can be found during the winnign move
        final_eval.current_score = -WIN_VALUE; // set value as losing for the current player
        final_eval.next_position = state.sided_position;
    } else if (is_draw(state)) {
        final_eval.current_score = DRAW_VALUE;
        final_eval.next_position = state.sided_position;
    } else { // continue search

        // reserve space in the dynamic array
        for (size_t i = 0; i < depth; i++) {
            allocation.valid_positions[i].reserve(EXPECTED_BRANCHING);
        }

        // reserve space for the simple vectors, if they end up growing the'll keep their size
        allocation.all_positions.reserve(EXPECTED_BRANCHING);
        allocation.single_pieces.reserve(EXPECTED_BRANCHING);
        allocation.single_moves.reserve(EXPECTED_BRANCHING);

        final_eval.current_score = 2 * WIN_VALUE;

        valid_positions(allocation, depth, state);
        // move ordering goes here, better first = more pruning
        // perhaps i can use the "Move" structure to prefer captures, especially with pawns
        // that might reqire considerable proccesing

        for (const SidedPosition& sided_position : next_positions) {
            nodes++;

            // initialize the call with both playes having the worse possivbe score as their best
            opponent_eval = -alphabeta(allocation, generate_next_state(state, sided_position), depth - 1, -2 * WIN_VALUE, 2 * WIN_VALUE);

            // If current eval is less favorable to the oppenent, use it.
            if (opponent_eval < final_eval.current_score) {
                final_eval.current_score = opponent_eval;
                final_eval.next_position = sided_position;
            }
        }
    }
    
    delete[] allocation.valid_positions;

    return final_eval;
}

int alphabeta(SearchPreallocation& allocation, GameState state, int depth, int own_best, int opponent_best) {

    // use prealocated vectors, clears happen to relevent vectors in the function that needs them
    std::vector<SidedPosition>& next_positions = allocation.valid_positions[depth - 1];
    // note that at depth 0, the pointer is INVALID

    int eval;

    // end search conditions
    if (depth == 0) {
        eval = material_eval(state.sided_position); // calculate current position // the most basic function is used for now
        evals++;
    } else if (is_checkmate(state.sided_position)) { // should never get here, checkmates can be found during the winnign move
        eval = -WIN_VALUE; // set value as losing for the current player
    } else if (is_draw(state)) {
        eval = DRAW_VALUE;
    } else { // continue search

        eval = own_best; // initially set to the WORST possible score for the current player

        valid_positions(allocation, depth, state);
        // move ordering goes here, better first = more pruning
        // perhaps i can use the "Move" structure to prefer captures, especially with pawns
        // that might reqire considerable proccesing

        for (const SidedPosition& sided_position : next_positions) {
            nodes++;

            // Recursively search the resulting position from the perspective of the opposite player, own_best and opponent_best are passed inverted
            eval = std::max(eval, -alphabeta(allocation, generate_next_state(state, sided_position), depth - 1, -opponent_best, -own_best));

            // If current eval is "agreed upon" by both players, cut the brunch, nither will accept a differnt score.
            if (opponent_best == eval) {
                cutoffs++;
                break;
            }

            own_best = std::max(own_best, eval); // update own best eval
        }
    }

    // Return the final evaluation
    return eval;
}
