#include <iostream>
#include "Bitboard.h"
#include "Game constants.h"

Bitboard Bitboard::slide(void (Bitboard::* const direction)(), const Bitboard& allPieces) {
    Bitboard path;

    do {
        (this->*direction)(); // call provided member function
        path.setBitsFrom(board); // add the move to the move board
    } while (board && !(board & allPieces.getBoard())); // stop if piece is 0 (shifted out) or a collision occurred on the last move
    // collisions are added to be figured out by the color making the move, this calculation needs to be fast

    return path;
}

// Default constructor initializes data to 0
Bitboard::Bitboard() : board(0) {}

// Constructor with initial data
Bitboard::Bitboard(B64 initialData) : board(initialData) {}

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


B64 Bitboard::getBoard() const {
    return board;
}

bool Bitboard::getBit(int index) const {
    return (board & (1ULL << index)) != 0; // SHIFT 1 to position, use AND to extract the bit
}

B64 Bitboard::lowestBitBoard() const
{
    return (board & -board); // AND with 2's compliment to get the lowest bit (very cool)
}

int Bitboard::countSetBits() const{
    int count;
    B64 board_copy = board;
    for (count = 0; board; count++) // runs untill board is zeroed out, counting up
        board_copy = (board_copy & -board_copy); // removes one bit per iteration
    return count;
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

void Bitboard::clearLowestBit()
{
    board &= (board - 1); // AND after -1, garantees removal of exactly one lowest bit;
}

void Bitboard::moveUp() { board = board << BOARD_SIZE; }
void Bitboard::moveDown() { board = board >> BOARD_SIZE; }
void Bitboard::moveLeft() { (board & COLUMN_H_INV) >> 1; }
void Bitboard::moveRight() { (board & COLUMN_A_INV) << 1; }
void Bitboard::moveUpLeft() { (board & COLUMN_H_INV) << BOARD_SIDE_SUB1; }
void Bitboard::moveUpRight() { (board & COLUMN_A_INV) << BOARD_SIDE_ADD1; }
void Bitboard::moveDownLeft() { board = (board & COLUMN_H_INV) >> BOARD_SIDE_ADD1; }
void Bitboard::moveDownRight() { board = (board & COLUMN_A_INV) >> BOARD_SIDE_SUB1; }

Bitboard Bitboard::slideUp(const Bitboard& allPieces) { return slide(&moveUp, allPieces); }
Bitboard Bitboard::slideDown(const Bitboard& allPieces) { return slide(&moveDown, allPieces); }
Bitboard Bitboard::slideLeft(const Bitboard& allPieces) { return slide(&moveLeft, allPieces); }
Bitboard Bitboard::slideRight(const Bitboard& allPieces) { return slide(&moveRight, allPieces); }
Bitboard Bitboard::slideUpLeft(const Bitboard& allPieces) { return slide(&moveUpLeft, allPieces); }
Bitboard Bitboard::slideUpRight(const Bitboard& allPieces) { return slide(&moveUpRight, allPieces); }
Bitboard Bitboard::slideDownLeft(const Bitboard& allPieces) { return slide(&moveDownLeft, allPieces); }
Bitboard Bitboard::slideDownRight(const Bitboard& allPieces) { return slide(&moveDownRight, allPieces); }
