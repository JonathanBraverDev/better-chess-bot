// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>

#include "Board structure.h"
#include "Board operation.h"

B64 king_moves[64];
B64 knight_moves[64];
B64 pawn_moves[64 * 2]; // the 2 colors move in opposite direction
B64 pawn_attacks[64 * 2]; // further conditional split
// rooks always have 14 potential moves, 7 horizontal and 7 vertical
// bishops can have as little as 7 moves and up to 13 (corner and center respectivly)

void prepare_king_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64; i++)
    {
        king_moves[i] = up(current_board) |
                        down(current_board) |
                        left(current_board) |
                        right(current_board) |
                        up_left(current_board) |
                        up_right(current_board) |
                        down_left(current_board) |
                        down_right(current_board);

        current_board = current_board << 1;
    }
}

void prepare_knight_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64; i++)
    {
        knight_moves[i] = (((current_board >> 6) | (current_board << 10)) & ~COLUMN_GH) | // when looping around the board on the other side
            (((current_board >> 10) | (current_board << 6)) & ~COLUMN_AB) |
            (((current_board >> 15) | (current_board << 17)) & ~COLUMN_H) |
            (((current_board >> 17) | (current_board << 15)) & ~COLUMN_A);

        current_board = current_board << 1;
    }
}

void prepare_white_pawn_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 0; i < 64 * 2; i += 2)
    {
        // missing promotion logic
        pawn_moves[i] = up(current_board);
        // pawn jumps require sliding piece logic wihtout final capture
        // missing appassant logic

        pawn_attacks[i] = up_left(current_board) |
            up_right(current_board);

        current_board = current_board << 1;
    }
}

void prepare_black_pawn_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 1; i < 64 * 2; i += 2)
    {
        // missing promotion logic
        pawn_moves[i] = down(current_board);
        // pawn jumps require sliding piece logic wihtout final capture
        // missing appassant logic

        pawn_attacks[i] = down_left(current_board) |
            down_right(current_board);

        current_board = current_board << 1;
        // yes, black pawns can't get to the last row, not in a NORMAL game at least
    }
}

void prepare_pawn_moves() {
    // promotions and en passant will probably be handled by the actual more proccesing, not in generation
    // if a pawn has no POTENTIAL moves, he promoted
    prepare_white_pawn_moves();
    prepare_black_pawn_moves();
}

void generate_white_pawn_jump(B64& all_pieces, B64& moves, B64 piece) {
    if (!((up(piece) & all_pieces) |
        ((up(up(piece)) & all_pieces)))) {
        moves |= up(up(piece));
    }
}

void generate_black_pawn_jump(B64& all_pieces, B64& moves, B64 piece) {
    if (!((down(piece) & all_pieces) |
        ((down(down(piece)) & all_pieces)))) {
        moves |= down(down(piece));
    }
}

void generate_bishop_moves(B64& all_pieces, B64& moves, B64 piece) {

    slide_up_left(all_pieces, moves, piece);
    slide_up_right(all_pieces, moves, piece);
    slide_down_left(all_pieces, moves, piece);
    slide_down_right(all_pieces, moves, piece);
}

void generate_rook_moves(B64& all_pieces, B64& moves, B64 piece) {

    slide_up(all_pieces, moves, piece);
    slide_down(all_pieces, moves, piece);
    slide_left(all_pieces, moves, piece);
    slide_right(all_pieces, moves, piece);
}

void generate_queen_moves(B64& all_pieces, B64& moves, B64 piece) {

    generate_bishop_moves(all_pieces, moves, piece);
    generate_rook_moves(all_pieces, moves, piece);
}

void visualize_board(B64 board) {
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (get_bit(board, i*8+j))
            {
                std::cout << "X ";
            }
            else {
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
    
    unsigned long bit = 0;

    B64 blank = 0;
    B64 piece = 1ULL << (5*8+2);
    B64 moves = 0;

    std::cout << (double)microseconds << std::endl;
    generate_bishop_moves(blank, moves, piece);
    visualize_board(moves);
    moves = 0;
    generate_rook_moves(blank, moves, piece);
    visualize_board(moves);
    moves = 0;
    generate_queen_moves(blank, moves, piece);
    visualize_board(moves);
    moves = 0;
    generate_white_pawn_jump(blank, moves, piece);
    generate_black_pawn_jump(blank, moves, piece);
    visualize_board(moves);

    piece = 1;
    for (size_t i = 0; i < 64 * 2; i++) {
        moves = pawn_moves[i] | pawn_attacks[i];
        if (i % 2 == 0) {
            generate_white_pawn_jump(blank, moves, piece);
        } else {
            generate_black_pawn_jump(blank, moves, piece);
            piece = piece << 1;
        }

        visualize_board(moves);
    }
}
