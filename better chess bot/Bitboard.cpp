#include "Bitboard.h"
#include "BoardConstants.h"
#include "DeBruijn.h"
#include <cassert>
#include <iostream>

const DirectionCheck Bitboard::direction_check[] = {
    // UP and DOWN will be shifted out naturally, masked for consistency
    {BOARD_SIZE, ~ROW_8},         // UP
    {-BOARD_SIZE, ~ROW_1},        // DOWN
    {-1, ~COLUMN_A},              // LEFT
    {1, ~COLUMN_H},               // RIGHT
    {BOARD_SIZE - 1, ~COLUMN_A},  // UP_LEFT
    {BOARD_SIZE + 1, ~COLUMN_H},  // UP_RIGHT
    {-BOARD_SIZE - 1, ~COLUMN_A}, // DOWN_LEFT
    {-BOARD_SIZE + 1, ~COLUMN_H}, // DOWN_RIGHT

    // Composite moves for the knight
    {2 * BOARD_SIZE - 1, ~COLUMN_A},  // KNIGHT_UP_LEFT
    {2 * BOARD_SIZE + 1, ~COLUMN_H},  // KNIGHT_UP_RIGHT
    {-2 * BOARD_SIZE - 1, ~COLUMN_A}, // KNIGHT_DOWN_LEFT
    {-2 * BOARD_SIZE + 1, ~COLUMN_H}, // KNIGHT_DOWN_RIGHT
    {BOARD_SIZE - 2, ~COLUMN_AB},     // KNIGHT_LEFT_UP
    {-BOARD_SIZE - 2, ~COLUMN_AB},    // KNIGHT_LEFT_DOWN
    {BOARD_SIZE + 2, ~COLUMN_GH},     // KNIGHT_RIGHT_UP
    {-BOARD_SIZE + 2, ~COLUMN_GH}     // KNIGHT_RIGHT_DOWN
};

// Default constructor initializes data to 0
Bitboard::Bitboard() : board(0) {}

// Constructor with initial data
Bitboard::Bitboard(B64 initialData) : board(initialData) {}

void Bitboard::clear() { board = 0; }

void Bitboard::visualize() const {
  for (int i = BOARD_SIZE - 1; i >= 0; i--) {
    for (int j = 0; j <= BOARD_SIZE - 1; j++) {
      if (getBit(i * BOARD_SIZE + j)) {
        std::cout << "X ";
      } else {
        std::cout << "_ ";
      }
    }

    std::cout << '\n';
  }

  std::cout << '\n';
}

// any use is a potential danger to data integrity
B64 Bitboard::getBoard() const { return board; }

bool Bitboard::getBit(uint8_t index) const {
  // SHIFT 1 to position, use AND to extract the bit
  return (board & (1ULL << index)) != 0;
}

B64 Bitboard::lowestBitBoard() const {
  // AND with 2's compliment to get the lowest bit (very cool)
  return (board & -board);
}

int Bitboard::countSetBits() const {
  int count;
  B64 board_copy = board;
  // runs untill board is zeroed out, counting up
  for (count = 0; board; count++)
    // removes one bit per iteration
    board_copy = (board_copy & -board_copy);
  return count;
}

// returns the index of the active bit
// useble only on boards with a single active bit
uint8_t Bitboard::singleBitIndex() const {
  return DeBruijnPositionLookup[((board * DeBruijnMultiplier)) >> 58];
}

bool Bitboard::hasRemainingBits() const { return board != 0; }

bool Bitboard::isEmpty() const { return board == 0; }

void Bitboard::setBit(uint8_t index) {
  board |= (1ULL << index); // SHIFT 1 to position, set to OR
}

void Bitboard::clearBit(uint8_t index) {
  board &= ~(1ULL << index); // SHIFT 1 to position, INVERT, set to AND
}

void Bitboard::setBitsFrom(Bitboard otherBoard) {
  board |= otherBoard.getBoard();
}

void Bitboard::clearBitsFrom(Bitboard otherBoard) {
  board &= ~otherBoard.getBoard();
}

void Bitboard::toggleBit(uint8_t index) { board ^= (1ULL << index); }

void Bitboard::toggleBitsFrom(Bitboard otherBoard) {
  board ^= otherBoard.getBoard();
}

Bitboard Bitboard::getInverted() const { return ~board; }

Bitboard Bitboard::getCommonBitsWith(Bitboard otherBoard) const {
  return board & otherBoard.getBoard();
}

Bitboard Bitboard::getCombinedWith(Bitboard otherBoard) const {
  return board | otherBoard.getBoard();
}

Bitboard Bitboard::getWithoutBitsFrom(Bitboard otherBoard) const {
  return board & otherBoard.getInverted().getBoard();
}

Bitboard Bitboard::popLowestBit() {
  B64 lowest = lowestBitBoard();
  clearLowestBit();
  return Bitboard(lowest);
}

void Bitboard::clearLowestBit() {
  // AND after -1, garantees removal of exactly one lowest bit;
  board &= (board - 1);
}

void Bitboard::move(Direction direction) {
  int shift_amount = direction_check[direction].shiftAmount;
  board =
      board &
      direction_check[direction].boundCheck; // stops odd shifting teleportation
  // Avoid negative shifts by fliping the shift direction and value
  board = (shift_amount >= 0) ? (board << shift_amount)
                              : (board >> (-shift_amount));
}

void Bitboard::nextTile() { board <<= 1; }

Bitboard Bitboard::look(Direction direction) const {
  Bitboard copy = Bitboard(board);
  copy.move(direction);
  return copy;
}

Bitboard Bitboard::slidePath(Direction direction,
                             const Bitboard all_pieces) const {
  assert(direction == Direction::UP || direction == Direction::DOWN ||
         direction == Direction::LEFT || direction == Direction::RIGHT ||
         direction == Direction::UP_LEFT || direction == Direction::UP_RIGHT ||
         direction == Direction::DOWN_LEFT ||
         direction == Direction::DOWN_RIGHT);
  Bitboard piece = board;
  Bitboard path = Bitboard(0ULL);

  do {
    piece.move(direction);
    path.setBitsFrom(piece); // Add the new location to the path
  } while (piece.hasRemainingBits() &&
           Bitboard::findCommonBits(piece, all_pieces).isEmpty());
  // Stop if piece is 0 (shifted out) or a collision occurred on the last move
  // The first colision is added to be contextually figured out by the caller

  return path;
}

// returns a Bitboard of all bits lower than the active bit
// useble only on boards with a single active bit
Bitboard Bitboard::lowerThanSingleBit() const {
  return Bitboard(board - 1); // similar to clearLowestBit
}

// returns a Bitboard of all bits higher than the active bit
// useble only on boards with a single active bit
Bitboard Bitboard::higherThanSingleBit() const {
  return Bitboard(
      ~(board - 1) ^
      board); // INVERT lowerThanSingleBit, XOR with the single bit to remove it
}

Bitboard Bitboard::sameRowPathTo(Bitboard destination) const {
  assert(countSetBits() == 1 && destination.countSetBits() == 1);
  if (board < destination.getBoard()) {
    return higherThanSingleBit().getCommonBitsWith(
        destination.lowerThanSingleBit());
  } else {
    return lowerThanSingleBit().getCommonBitsWith(
        destination.higherThanSingleBit());
  }
}