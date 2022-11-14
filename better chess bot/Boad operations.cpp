
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

// the true heavylifter, all collision checks happen here
void check_proposed_slide(B64& proposed, B64& all_pieces, B64& moves, bool& stop) {
    if (proposed & all_pieces) {
        moves |= proposed;
        stop = true;
    } else {
        moves |= proposed;
    }
}

void slide(B64(*direction)(B64), B64& all_pieces, B64& moves, B64 piece) {
    B64 proposed;
    bool stop = false;

    do {
        proposed = direction(piece); // running the passed function
        check_proposed_slide(proposed, all_pieces, moves, stop);
        piece = proposed;
    } while (!stop && piece);
}

// functions below are just for ease of use

void slide_up(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up, all_pieces, moves, piece);
}


void slide_down(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down, all_pieces, moves, piece);
}


void slide_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&left, all_pieces, moves, piece);
}


void slide_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&right, all_pieces, moves, piece);
}


void slide_up_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up_left, all_pieces, moves, piece);
}


void slide_up_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up_right, all_pieces, moves, piece);
}


void slide_down_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down_left, all_pieces, moves, piece);
}


void slide_down_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down_right, all_pieces, moves, piece);
}