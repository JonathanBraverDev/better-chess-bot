
#include <cassert>
#include <vector>

#include "Board operation.h"

// finding the index of the least significant set bit in a 32 bit word
// De Bruijn Sequence
uint8_t lowestBitIndex(uint32_t v) {
    static const uint8_t BitPositionLookup[32] = // hash table
    {
      0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
      31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };
    assert(v != 0); // the next operation expects a nonzero board
    // using the De Bruijn Sequence and lookink at the last 5 bits
    return BitPositionLookup[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}

// finding the index of the least significant set bit in a 64 bit number
uint8_t lowestBitIndex32_2(uint64_t v) {
    // splitting the number into 2 words to use the De Bruijn Sequence on both halfs

    assert(v != 0); // the checks reqire a nonzero board

    uint8_t ans = lowestBitIndex(v); // cast down to 32 first bits

    if (!ans) {// if 0 is returned, no bits are set in the lower half
        ans = lowestBitIndex(v << 32); // shift the zeros out of this universe and procces the rest

        if (ans) // if its 0 again, no bits are set at all
            ans += 32; // if not, account for the bits shifted out in the first part
    }

    return ans;
}

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

    B64 piece = 0;

    while (board != 0) { // check that there are pieces left to exstract

        piece = lowestBitBoard(board);
        board ^= piece; // using the piece on to flip the bit on the original board
        pieces.push_back(piece);
    }
}

B64 slide(const B64(*direction)(B64), const B64 all_pieces, B64 piece) {
    B64 moves = 0;
    bool stop = false;

    do {
        piece = direction(piece); // running the passed function and moving the piece
        moves |= piece; // adding the move to the move board
    } while (piece && !(piece & all_pieces)); // stop if piece is 0 (shifted out) or a collision occured on the last move
    // colisions are added to be figured out by the color making the move, this calculation needs to be fast

    return moves;
}

// functions below are just for ease of use

B64 slide_up(const B64 all_pieces, B64 piece) {
    return slide(&up, all_pieces, piece);
}


B64 slide_down(const B64 all_pieces, B64 piece) {
    return slide(&down, all_pieces, piece);
}


B64 slide_left(const B64 all_pieces, B64 piece) {
    return slide(&left, all_pieces, piece);
}


B64 slide_right(const B64 all_pieces, B64 piece) {
    return slide(&right, all_pieces, piece);
}


B64 slide_up_left(const B64 all_pieces, B64 piece) {
    return slide(&up_left, all_pieces, piece);
}


B64 slide_up_right(const B64 all_pieces, B64 piece) {
    return slide(&up_right, all_pieces, piece);
}


B64 slide_down_left(const B64 all_pieces, B64 piece) {
    return slide(&down_left, all_pieces, piece);
}


B64 slide_down_right(const B64 all_pieces, B64 piece) {
    return slide(&down_right, all_pieces, piece);
}