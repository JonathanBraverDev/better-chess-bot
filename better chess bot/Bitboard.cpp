#include <iostream>
#include "Bitboard.h"
#include "BoardConstants.h"
#include "Consts.h"

Bitboard Bitboard::slide(void (Bitboard::* const direction)(), const Bitboard allPieces) {
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
void Bitboard::moveLeft() { (board & COLUMN_H_INV) >> 1; }
void Bitboard::moveRight() { (board & COLUMN_A_INV) << 1; }
void Bitboard::moveUpLeft() { (board & COLUMN_H_INV) << BOARD_SIDE_SUB1; }
void Bitboard::moveUpRight() { (board & COLUMN_A_INV) << BOARD_SIDE_ADD1; }
void Bitboard::moveDownLeft() { board = (board & COLUMN_H_INV) >> BOARD_SIDE_ADD1; }
void Bitboard::moveDownRight() { board = (board & COLUMN_A_INV) >> BOARD_SIDE_SUB1; }

Bitboard Bitboard::slideUp(const Bitboard allPieces) { return slide(&Bitboard::moveUp, allPieces); }
Bitboard Bitboard::slideDown(const Bitboard allPieces) { return slide(&Bitboard::moveDown, allPieces); }
Bitboard Bitboard::slideLeft(const Bitboard allPieces) { return slide(&Bitboard::moveLeft, allPieces); }
Bitboard Bitboard::slideRight(const Bitboard allPieces) { return slide(&Bitboard::moveRight, allPieces); }
Bitboard Bitboard::slideUpLeft(const Bitboard allPieces) { return slide(&Bitboard::moveUpLeft, allPieces); }
Bitboard Bitboard::slideUpRight(const Bitboard allPieces) { return slide(&Bitboard::moveUpRight, allPieces); }
Bitboard Bitboard::slideDownLeft(const Bitboard allPieces) { return slide(&Bitboard::moveDownLeft, allPieces); }
Bitboard Bitboard::slideDownRight(const Bitboard allPieces) { return slide(&Bitboard::moveDownRight, allPieces); }

namespace BitboardOperations {
    Bitboard combineBoards(const Bitboard& board1, const Bitboard& board2) {
        return Bitboard(board1.getBoard() | board2.getBoard());
    }

    template <typename... Boards>
    Bitboard combineBoards(const Bitboard& board1, const Bitboard& board2, const Boards&... boards) {
        return combineBoards(board1, combineBoards(board2, boards...));
    }


    Bitboard findCommonBits(const Bitboard& board1, const Bitboard& board2) {
        return Bitboard(board1.getBoard() & board2.getBoard());
    }

    template <typename... Boards>
    Bitboard findCommonBits(const Bitboard& board1, const Bitboard& board2, const Boards&... boards) {
        return findCommonBits(board1, findCommonBits(board2, boards...));
    }
}