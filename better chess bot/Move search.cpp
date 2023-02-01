#include "Board structure.h"
#include "Evaluation.h"
#include <utility>
#include <vector>

// originally written by chatGPT 
int alphabeta(GameState state, int depth, int alpha, int beta, int eval, std::vector<Move> moves)
{
    // If the search has reached the maximum depth or the game has ended,
    // return the evaluation of the current position
    if (depth == 0 || state.gameOver())
        return eval;

    for (Move move : moves)
    {
        // Apply the current move
        makeMove(state, move);

        // Recursively search the resulting position from the perspective of the opposite player
        eval = std::max(eval, -alphabeta(state, depth - 1, -beta, -alpha, -eval, PossibleMoves(state.position)));

        // Unapply the current move
        unmakeMove(state, move);

        // Update alpha
        alpha = std::max(alpha, eval);

        // If alpha is greater than or equal to beta, terminate the loop early
        if (beta <= alpha)
            break;
    }

    // Return the final evaluation
    return eval;
}