// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <vector>

#include "Board structure.h"
#include "Board movment.h"
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

B64 generate_bishop_moves(B64& same_color, B64& diff_color, int origin) {
    B64 moves = 0;

    B64 bishop = 1ULL << origin;
    slide_up_left(same_color, diff_color, moves, origin);


    bishop = 1ULL << origin;
    slide_up_right(same_color, diff_color, moves, origin);


    bishop = 1ULL << origin;
    slide_down_left(same_color, diff_color, moves, origin);


    bishop = 1ULL << origin;
    slide_down_right(same_color, diff_color, moves, origin);

    return moves;
}

B64 generate_rook_moves(B64& same_color, B64& diff_color, int origin) {
    B64 moves = 0;

    B64 rook = 1ULL << origin;
    slide_up(same_color, diff_color, moves, origin);


    rook = 1ULL << origin;
    slide_down(same_color, diff_color, moves, origin);


    rook = 1ULL << origin;
    slide_left(same_color, diff_color, moves, origin);


    rook = 1ULL << origin;
    slide_right(same_color, diff_color, moves, origin);

    return moves;
}

B64 generate_queen_moves(B64& same_color, B64& diff_color, int origin) {
    B64 moves = 0;
    moves |= generate_bishop_moves(same_color, diff_color, origin);
    moves |= generate_rook_moves(same_color, diff_color, origin);
    return moves;
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

// returns avector of all pices on the board
std::vector<B64> extract_pieces(B64 board) {

    B64 piece = 0;
    uint8_t location;

    std::vector<B64> pieces;

    location = lowestBitIndex64(board);

    while (location) { // will be 0 is no bits are set
        set_bit(piece, location); // setting the location of the piece on the empty board
        board ^= piece; // using the piece on the board to flip the bit on the original board
        pieces.push_back(piece);

        location = lowestBitIndex64(board); // find the next active bit
    }

    return pieces;
}


int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    prepare_king_moves();
    prepare_knight_moves();
    prepare_pawn_moves();

    auto end = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    //for (size_t i = 0; i < 64; i++)
    //{
    //    visualize_board(king_moves[i]);
    //}
    //std::cout << '\n';
    //for (size_t i = 0; i < 64; i++)
    //{
    //    visualize_board(knight_moves[i]);
    //}
    //std::cout << '\n';
    //for (size_t i = 0; i < 64; i++)
    //{
    //    visualize_board(pawn_moves[i] | pawn_attacks[i]);
    //}
    
    unsigned long bit = 0;

    
    B64 blank = 0;

    std::cout << (double)microseconds << std::endl;
    visualize_board(generate_queen_moves(blank, blank, 5*8+2));
}
