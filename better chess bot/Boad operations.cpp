
#include "Board operation.h"

// counts the active bits in a given board
int count_bits64(B64 board) {
    int count;
    for (count = 0; board; count++) // runs untill board is zeroed out, counting up
        board = removeLowestBit(board); // removes one bit per iteration
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
uint8_t lowestBitIndex64(B64 board) {
    // using the De Bruijn Sequence and checking the lookup table with the last 6 bits
    return BitPositionLookup[((uint64_t)(lowestBitBoard(board) * lookUpMultiplier)) >> 58];
}

// short version of the original, assumes input is ONLY ONE active bit and skips the board inversion
uint8_t lowestBitIndex64_s(B64 board) {
    return BitPositionLookup[((uint64_t)(board * lookUpMultiplier)) >> 58];
}

// fills a vector with all pieces on the given board
void extract_pieces(B64 board, std::vector<B64>& pieces) {
        
    B64 piece;

    while (board != 0) { // check that there are pieces left to exstract

        piece = lowestBitBoard(board);
        board ^= piece; // using the piece on to flip the bit on the original board
        pieces.push_back(piece);
    }
}

B64 slide(B64(*direction)(B64), const B64 all_pieces, B64 piece) {
    B64 moves = 0;
    bool stop = false;

    do {
        piece = direction(piece); // running the passed function and moving the piece
        moves |= piece; // adding the move to the move board
    } while (piece && !(piece & all_pieces)); // stop if piece is 0 (shifted out) or a collision occured on the last move
    // colisions are added to be figured out by the color making the move, this calculation needs to be fast

    return moves;
}