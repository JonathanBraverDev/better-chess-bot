
#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Move search.h"
#include "Magic bit boards.h"

int main() {

    prepare_king_moves();
    prepare_knight_moves();
    prepare_pawn_moves();

    PositionScore position_score;
    auto start = std::chrono::high_resolution_clock::now();

    position_score = alphabeta_init(INITIAL_GAMESTATE, 7);

    auto end = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();   

    std::cout << "eval:" << position_score.current_score << std::endl;
    std::cout << "microseconds:" << (double)microseconds << std::endl;
    std::cout << "positions evaluated:" << evals << std::endl;
    std::cout << "parent nodes visited:" << nodes - evals << std::endl;
    std::cout << "cutoffs performed:" << cutoffs << std::endl;
    visualize_position(position_score.next_position);
}
