#include <iostream>
#include "Bitboard.h"
#include "BoardConstants.h"
#include "Consts.h"

Bitboard Bitboard::slidePath(void (Bitboard::* const direction)(), const Bitboard allPieces) {
    Bitboard path;

    do {
        (this->*direction)(); // call provided member function
        path.setBitsFrom(board); // add the move to the move board
    } while (board && !(board & allPieces.getBoard())); // stop if piece is 0 (shifted out) or a collision occurred on the last move
    // the first colision is added to be contextually figured out by the caller

    return path;
}

// Default constructor initializes data to 0
Bitboard::Bitboard() : board(0) {}

// Constructor with initial data
Bitboard::Bitboard(B64 initialData) : board(initialData) {}

 void Bitboard::clear() {
    board = 0;
}

void Bitboard::visualize() {
    for (int i = BOARD_SIDE_SUB1; i >= 0; i--) {
        for (int j = 0; j <= BOARD_SIDE_SUB1; j++) {
            if (getBit(i * BOARD_SIZE + j)) {
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

// any use is a potential danger to data integrity
B64 Bitboard::getBoard() const {
    return board;
}

bool Bitboard::getBit(int index) const {
    return (board & (1ULL << index)) != 0; // SHIFT 1 to position, use AND to extract the bit
}

B64 Bitboard::lowestBitBoard() const {
    return (board & -board); // AND with 2's compliment to get the lowest bit (very cool)
}

int Bitboard::countSetBits() const {
    int count;
    B64 board_copy = board;
    for (count = 0; board; count++) // runs untill board is zeroed out, counting up
        board_copy = (board_copy & -board_copy); // removes one bit per iteration
    return count;
}

// use only on SINGLE BIT boards
int Bitboard::singleBitIndex() const {
    return DeBruijnPositionLookup[((board * DeBruijnMultiplier)) >> 58];
}

bool Bitboard::hasRemainingBits() const {
    return board != 0;
}


void Bitboard::setBit(int index) {
    board |= (1ULL << index); // SHIFT 1 to position, set to OR
}

void Bitboard::clearBit(int index) {
    board &= ~(1ULL << index); // SHIFT 1 to position, INVERT, set to AND
}

void Bitboard::setBitsFrom(Bitboard otherBoard) {
    board |= otherBoard.getBoard();
}

void Bitboard::clearBitsFrom(Bitboard otherBoard) {
    board &= ~otherBoard.getBoard();
}

Bitboard Bitboard::popLowestBit() {
    B64 lowest = lowestBitBoard();
    clearLowestBit();
    return Bitboard(lowest);
}

void Bitboard::clearLowestBit() {
    board &= (board - 1); // AND after -1, garantees removal of exactly one lowest bit;
}

void Bitboard::moveUp() { board = board << BOARD_SIZE; }
void Bitboard::moveDown() { board = board >> BOARD_SIZE; }
void Bitboard::moveLeft() { board = (board & COLUMN_H_INV) >> 1; }
void Bitboard::moveRight() { board = (board & COLUMN_A_INV) << 1; }
void Bitboard::moveUpLeft() { board = (board & COLUMN_H_INV) << BOARD_SIDE_SUB1; }
void Bitboard::moveUpRight() { board = (board & COLUMN_A_INV) << BOARD_SIDE_ADD1; }
void Bitboard::moveDownLeft() { board = (board & COLUMN_H_INV) >> BOARD_SIDE_ADD1; }
void Bitboard::moveDownRight() { board = (board & COLUMN_A_INV) >> BOARD_SIDE_SUB1; }

Bitboard Bitboard::slidePathUp(const Bitboard allPieces) { return slidePath(&Bitboard::moveUp, allPieces); }
Bitboard Bitboard::slidePathDown(const Bitboard allPieces) { return slidePath(&Bitboard::moveDown, allPieces); }
Bitboard Bitboard::slidePathLeft(const Bitboard allPieces) { return slidePath(&Bitboard::moveLeft, allPieces); }
Bitboard Bitboard::slidePathRight(const Bitboard allPieces) { return slidePath(&Bitboard::moveRight, allPieces); }
Bitboard Bitboard::slidePathUpLeft(const Bitboard allPieces) { return slidePath(&Bitboard::moveUpLeft, allPieces); }
Bitboard Bitboard::slidePathUpRight(const Bitboard allPieces) { return slidePath(&Bitboard::moveUpRight, allPieces); }
Bitboard Bitboard::slidePathDownLeft(const Bitboard allPieces) { return slidePath(&Bitboard::moveDownLeft, allPieces); }
Bitboard Bitboard::slidePathDownRight(const Bitboard allPieces) { return slidePath(&Bitboard::moveDownRight, allPieces); }
