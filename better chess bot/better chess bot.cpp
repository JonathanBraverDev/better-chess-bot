// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"

void visualize_board(B64 board) {
    for (int i = 7; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            if (get_bit(board, i * BOARD_SIZE + j)) {
                std::cout << "X ";
            } else {
                std::cout << "_ ";
            }
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}


int main()
{
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
}
