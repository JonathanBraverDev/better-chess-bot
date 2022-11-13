
#include "Board operation.h"

// finding the index of the least significant set bit in a 32 bit word
// De Bruijn Sequence
uint8_t lowestBitIndex(uint32_t v) {
    static const uint8_t BitPositionLookup[32] = // hash table
    {
      0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
      31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };
    // using the De Bruijn Sequence and lookink at the last 5 bits
    return BitPositionLookup[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}

// finding the index of the least significant set bit in a 64 bit number
uint8_t lowestBitIndex32_2(uint64_t v) {
    // splitting the number into 2 words to use the De Bruijn Sequence on both halfs

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
    // using the De Bruijn Sequence and lookink at the last 6 bits
    return BitPositionLookup[((uint64_t)((v & -v) * 0x022fdd63cc95386dU)) >> 58];
}