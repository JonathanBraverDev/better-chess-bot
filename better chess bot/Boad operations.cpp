
#include "Board operations.h"

// prints a human readable version of the board, is A1 being 1(2^0) and H8 a lot (2^64)
void visualize_board(B64 board) {
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j <= 7; j++) {
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

// counts the active bits in a given board, effective for almost empty boards
int count_bits64(B64 board) {
    int count;
    for (count = 0; board; count++) // runs untill board is zeroed out, counting up
        board = remove_lowest_bit(board); // removes one bit per iteration
    return count;
}

// De Bruijn Sequence
const uint64_t lookUpMultiplier = 0x022fdd63cc95386dU;

// De Bruijn Sequence lokup table
const uint8_t BitPositionLookup[64] = // hash table
{
  0, 1, 2, 4, 8, 17, 34, 5, 11, 23, 47, 31, 63, 62, 61, 59,
  55, 46, 29, 58, 53, 43, 22, 44, 24, 49, 35, 7, 15, 30, 60, 57,
  51, 38, 12, 25, 50, 36, 9, 18, 37, 10, 21, 42, 20, 41, 19, 39,
  14, 28, 56, 48, 33, 3, 6, 13, 27, 54, 45, 26, 52, 40, 16, 32
};

// finding the index of the least significant set bit in a 64 bit long word
// De Bruijn Sequence
int lowest_bit_index(const B64 board) {
    // using the De Bruijn Sequence and checking the lookup table with the last 6 bits
    return BitPositionLookup[((lowest_bit_board(board) * lookUpMultiplier)) >> 58];
}

// slithly faster version of the original, assumes input is ONLY ONE active bit and skips the board inversion
int lowest_single_bit_index(const B64 board) {
    return BitPositionLookup[((board * lookUpMultiplier)) >> 58];
}

// extract the (x,y) cords of a single bit board
void lowest_single_cords(const B64 board, int& x, int& y) {
    const int bit_index = lowest_single_bit_index(board);

    y = bit_index % BOARD_SIZE;
    x = bit_index / BOARD_SIZE;
}

// fills a vector with bitBoards of all the active bits on the given board
void seperate_bits(B64 board, std::vector<B64>& bit_boards) {

    // counting bits in caller to preserve enough sounds wastful
    bit_boards.reserve(8); // max piece count is a practicly unachivable 10 (2 minor and 8 pawns)

    while (board != 0) { // check that there are pieces left to exstract

        bit_boards.push_back(lowest_bit_board(board));
        board ^= bit_boards.back(); // using the piece just added to flip the bit on the original board
    }
}

B64 slide(B64(*direction)(B64), const B64 blockers, B64 piece) {
    B64 moves = 0;

    do {
        piece = direction(piece); // running the passed function and moving the piece
        moves |= piece; // adding the move to the move board
    } while (piece && !(piece & blockers)); // stop if piece is 0 (shifted out) or a collision occured on the last move
    // colisions are added to be figured out by the color making the move, this calculation needs to be fast

    return moves;
}

// return direcion function between 2 boards along a sliding piece move
B64(*get_direction(const B64 start, const B64 end))(B64) {

    int start_x, start_y, end_x, end_y;
    lowest_single_cords(start, start_x, start_y);
    lowest_single_cords(end, end_x, end_y);

    int dir_x = end_x - start_x;
    int dir_y = end_y - start_y;

    B64(*dir)(B64) = nullptr;

    if (dir_x < 0) {
        dir = (dir_y < 0) ? &down_left
                          : (dir_y == 0) ? &left : &up_left;
    } else if (dir_x == 0) {
        dir = (dir_y < 0) ? &down
                          : (dir_y > 0) ? &up : nullptr;
    } else {
        dir = (dir_y < 0) ? &down_right
                          : (dir_y == 0) ? &right : &up_right;
    }

    return dir;
}

// returns a board with all tiles between the 2 SINGLE BIT boards
B64 get_connecting_tiles(B64 start, const B64 end) {

    B64(*direction)(B64) = get_direction(start, end);
    B64 tiles = 0;

    // skiping the first check, at the very least one run is required
    do {
        start = direction(start);
        tiles |= start;
    } while (start ^ end); // zero eachother out

    return tiles;
}