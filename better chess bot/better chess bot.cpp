// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    prepare_king_moves();
    prepare_knight_moves();
    prepare_pawn_moves();

    auto end = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    

    B64 empty_board = 0;
    B64 piece = 1ULL << (5 * BOARD_SIZE + 2);
    

    std::cout << (double)microseconds << std::endl;
   
    visualize_board(generate_bishop_moves(empty_board, piece));
    visualize_board(generate_rook_moves(empty_board, piece));
    visualize_board(generate_queen_moves(empty_board, piece));

    piece = 1;
    for (size_t i = 0; i < 8; i++) {


        visualize_board(piece);
        piece = up(piece);
    }

    visualize_board(2);
    visualize_board(right(1));
    visualize_board(up_left(1));
    visualize_board(up_right(1));

    visualize_board(pow(2, 62));
    visualize_board(left(pow(2, 63)));
    visualize_board(down_left(pow(2, 63)));
    visualize_board(down_right(pow(2, 63)));
}
