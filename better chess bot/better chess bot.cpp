// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

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
    B64 castle_rigths; // both colors together, the data dosent interfere
    B64 en_passant_right; // no s, this is the loneliest bit here, the king is at least presistant...
    // if im exstra space aware i can probalby merge those 2, they will never have overlap anyway
    // but for now i wont, for readability
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

void generate_king_moves() {
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

void generate_knight_moves() {
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

void visualize_board(B64 board) {
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            if (get_bit(board, i*8+j))
            {
                std::cout << 'X';
            }
            else {
                std::cout << '_';
            }
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}


int main()
{
    generate_king_moves();
    generate_knight_moves();

    std::cout << "Hello World!\n";
    for (size_t i = 0; i < 64; i++)
    {
        visualize_board(king_moves[i]);
    }
    std::cout << '\n';
    for (size_t i = 0; i < 64; i++)
    {
        visualize_board(knight_moves[i]);
    }
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
