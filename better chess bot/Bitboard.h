#pragma once

#include "Types.h"

class Bitboard {
private:
    B64 board;

public:
    Bitboard(); // Default constructor
    Bitboard(B64 initialData); // Constructor with initial data

    B64 getBoard() const;
    bool getBit(int index) const;
    B64 lowestBitBoard() const;
    int countSetBits() const;

    void setBit(int index);
    void clearBit(int index);
    void clearLowestBit();

    void setBitsFrom(Bitboard otherBoard);
    void clearBitsFrom(Bitboard otherBoard);
};
