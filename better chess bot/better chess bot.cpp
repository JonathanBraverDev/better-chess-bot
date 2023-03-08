// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Move search.h"

int main() {

    prepare_king_moves();
    prepare_knight_moves();
    prepare_pawn_moves();

    int eval;
    auto start = std::chrono::high_resolution_clock::now();

    eval = alphabeta_init(INITIAL_GAMESTATE, 13);

    auto end = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();   

    std::cout << "eval:" << eval << std::endl;
    std::cout << "microseconds:" << (double)microseconds << std::endl;
}
