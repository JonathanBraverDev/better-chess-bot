
#include <cassert>
#include <vector>

#include "Board operation.h"

// finding the index of the least significant set bit in a 64 bit long word
// De Bruijn Sequence
uint8_t lowestBitIndex64(uint64_t v) {
    static const uint8_t BitPositionLookup[64] = // hash table
    {
      0, 1, 2, 4, 8, 17, 34, 5, 11, 23, 47, 31, 63, 62, 61, 59,
      55, 46, 29, 58, 53, 43, 22, 44, 24, 49, 35, 7, 15, 30, 60, 57,
      51, 38, 12, 25, 50, 36, 9, 18, 37, 10, 21, 42, 20, 41, 19, 39,
      14, 28, 56, 48, 33, 3, 6, 13, 27, 54, 45, 26, 52, 40, 16, 32
    };
    // assert(v != 0); // the next operation expects a nonzero board BUT ill handle it in the caller
    // using the De Bruijn Sequence and lookink at the last 6 bits
    return BitPositionLookup[((uint64_t)((v & -v) * 0x022fdd63cc95386dU)) >> 58];
}

// returns a vector of all pieces on the given board
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