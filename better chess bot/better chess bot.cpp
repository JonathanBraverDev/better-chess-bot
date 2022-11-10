// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>

typedef uint64_t B64; // 64 bit board

// EVERYTHING needed for quick operations on the position
struct BoardPosition
{
    // everything here is a 'bitmap of...'
    B64 white;
    B64 white_pawns;
    B64 white_bishops;
    B64 white_knights;
    B64 white_rooks;
    B64 white_queens;
    B64 white_king; // no s this time, budget cuts
    B64 black;
    B64 black_pawns;
    B64 black_bishops;
    B64 black_knights;
    B64 black_rooks;
    B64 black_queens;
    B64 black_king;
    B64 special_move_rigths; // en passant AND caslte right bot both sides, data cant overlap anyway
};

// info on gamestate
struct GameState // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
{
    int turn; // idk why i need this actually, but why not, it's kinda cute
    BoardPosition position;
    int draw_timer; // used to count moves untill the forced draw
    GameState* previous_state; // used for 3 time repetition checks and move history
};

bool operator==(const BoardPosition& lhs, const BoardPosition& rhs)
{
    return memcmp(&lhs, &rhs, sizeof(BoardPosition));
}

// useful for out of bounds detection
const B64 COLUMN_A = 0x8080808080808080ULL;
const B64 COLUMN_B = 0x4040404040404040ULL;
const B64 COLUMN_AB = COLUMN_A | COLUMN_B;
const B64 COLUMB_G = 0x0202020202020202ULL;
const B64 COLUMN_H = 0x0101010101010101ULL;
const B64 COLUMN_GH = COLUMB_G | COLUMN_H;
// used for pawn first move/promotion
const B64 ROW_1 = 0xFF00000000000000ULL;
const B64 ROW_2 = 0x00FF000000000000ULL;
const B64 ROW_7 = 0x000000000000FF00ULL;
const B64 ROW_8 = 0x00000000000000FFULL;

// constexpr save the function call so they're faster than regular function
// general bit manipulation
constexpr B64 set_bit(B64 board, int bit) { return (board |= (1ULL << bit)); } // shift 1 to position and set to OR
constexpr bool get_bit(B64 board, int bit) { return (board & (1ULL << bit)); } // shift 1 to position and use AND as a boolean check on the bit
constexpr B64 clear_bit(B64 board, int bit) { return (board &= ~(1ULL << bit)); } // shift 1 to position, INVERT and set to AND

#define SKIPCOMPILE
#ifndef SKIPCOMPILE
// it just refuses to compile, not sure what to do with that, a very useful function
constexpr int find_first_bit(B64 board) { return __builtin_ctzll(board); } // finds the first active bit
// not sure if this one is suposed to be a constepr
constexpr int use_first_bit(B64 board) { // returns the location of the first bit and turns it off
    int first = find_first_bit(board);
    board &= board - 1; // get rid of the first bit, shlould be faster than 'clear bit'
    return first;
}
#endif // !SKIPCOMPILE
#undef SKIPCOMPILE

// piece movment assists, with bound protections
constexpr B64 up(B64 board) { return board << 8; } // shifts out of the board would just be 0 anyway
constexpr B64 down(B64 board) { return board >> 8; }
constexpr B64 left(B64 board) { return (board & ~COLUMN_A) << 1; }
constexpr B64 right(B64 board) { return (board & ~COLUMN_H) >> 1; }
constexpr B64 up_left(B64 board) { return (board & ~COLUMN_A) << 9; }
constexpr B64 up_right(B64 board) { return (board & ~COLUMN_H) << 7; }
constexpr B64 down_left(B64 board) { return (board & ~COLUMN_A) >> 7; }
constexpr B64 down_right(B64 board) { return (board & ~COLUMN_H) >> 9; }

B64 king_moves[64];
B64 knight_moves[64];
B64 pawn_moves[64*2]; // the 2 colors move in opposite direction
B64 pawn_attacks[64*2]; // further conditional split
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
    for (size_t i = 0; i < 64*2; i+=2)
    {
        // missing promotion logic
        pawn_moves[i] = up(current_board);
                        // pawn jumps require sliding piece logic
                        // missing appassant logic

        pawn_attacks[i] = up_left(current_board) |
                          up_right(current_board);

        current_board = current_board << 1;
    }
}

void prepare_black_pawn_moves() {
    B64 current_board = 1ULL;
    for (size_t i = 1; i < 64*2; i+=2)
    {
        // missing promotion logic
        pawn_moves[i] = down(current_board);
                        // pawn jumps require sliding piece logic
                        // missing appassant logic

        pawn_attacks[i] = down_left(current_board) |
                          down_right(current_board);

        current_board = current_board << 1;
        // yes, black pawns can't get to the last row, not in a NORMAL game at least
    }
}

void check_proposed_slide(B64& proposed, B64& same_color, B64& diff_color, B64& moves, bool& stop) {
    if (proposed & same_color) {
        stop = true;
    } else if (proposed & diff_color) {
        moves |= proposed;
        stop = true;
    } else {
        moves |= proposed;
    }
}


B64 generate_bishop_moves(B64& same_color, B64& diff_color, int origin) {
    B64 moves = 0;
    B64 proposed;

    B64 bishop = 0;
    set_bit(bishop, origin);
    bool stop = false;
    do {// up left movemnt
        proposed = up_left(bishop);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);

    B64 bishop = 0;
    set_bit(bishop, origin);
    bool stop = false;
    do { // up right
        proposed = up_right(bishop);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);

    B64 bishop = 0;
    set_bit(bishop, origin);
    bool stop = false;
    do { // down left
        proposed = down_left(bishop);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);

    B64 bishop = 0;
    set_bit(bishop, origin);
    bool stop = false;
    do { // down right
        proposed = down_right(bishop);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);

    return moves;
}

void prepare_pawn_moves() {
    // promotions and en passant will probably be handled by the actual more proccesing, not in generation
    // if a pawn has no POTENTIAL moves, he promoted
    prepare_white_pawn_moves();
    prepare_black_pawn_moves();
}

void visualize_board(B64 board) {
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
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

    for (size_t i = 0; i < 64; i++)
    {
        visualize_board(king_moves[i]);
    }
    std::cout << '\n';
    for (size_t i = 0; i < 64; i++)
    {
        visualize_board(knight_moves[i]);
    }
    std::cout << '\n';
    for (size_t i = 0; i < 64; i++)
    {
        visualize_board(pawn_moves[i] | pawn_attacks[i]);
    }
    
    std::cout << (double)microseconds;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
