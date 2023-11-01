#pragma once

#include "Types.h"

class Bitboard {
private:
    B64 board;

    // expected to be used on a board with only one active bit
    Bitboard slide(void (Bitboard::* const direction)(), const Bitboard& allPieces);

public:
    Bitboard(); // Default constructor
    Bitboard(B64 initialData); // Constructor with initial data

    void visualize();

    B64 getBoard() const;
    bool getBit(int index) const;
    B64 lowestBitBoard() const;
    int countSetBits() const;

    void setBit(int index);
    void clearBit(int index);
    void clearLowestBit();
    void setBitsFrom(Bitboard otherBoard);
    void clearBitsFrom(Bitboard otherBoard);

    // piece movement assists, with bound protections
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveUpLeft();
    void moveUpRight();
    void moveDownLeft();
    void moveDownRight();
    // note: all directional names are are according to the output of "visualize"

    // aliases for the slide function, set the Bitboeard to all tiles along a path including the first collision
    Bitboard slideUp(const Bitboard& allPieces);
    Bitboard slideDown(const Bitboard& allPieces);
    Bitboard slideLeft(const Bitboard& allPieces);
    Bitboard slideRight(const Bitboard& allPieces);
    Bitboard slideUpLeft(const Bitboard& allPieces);
    Bitboard slideUpRight(const Bitboard& allPieces);
    Bitboard slideDownLeft(const Bitboard& allPieces);
    Bitboard slideDownRight(const Bitboard& allPieces);
};
