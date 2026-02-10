#pragma once

#include "Structs.h"

// Forward declare the public free functions so they can be friended
class Bitboard; // Forward declaration as a regular class

template <typename T, typename... Boards>
Bitboard combineBoards(T board, Boards... boards);

template <typename T, typename... Boards>
Bitboard findCommonBits(T board, Boards... boards);

class Bitboard {
private:
  B64 bits;

  // B64 shouldn't be exposed
  B64 lowestBitBoard() const;

  static const DirectionCheck direction_check[];

  bool operator==(const Bitboard &other) const { return bits == other.bits; }
  bool operator!=(const Bitboard &other) const { return bits != other.bits; }

  // Helpers for free functions (Hidden)
  template <typename T> static constexpr bool areAllSame() { return true; }

  template <typename T, typename U, typename... Rest>
  static constexpr bool areAllSame() {
    return std::is_same<T, U>::value && areAllSame<T, Rest...>();
  }

  static B64 combineBoardsJoiner(Bitboard board) {
    return board.bits;
  }

  template <typename... Boards>
  static B64 combineBoardsJoiner(Bitboard board, Boards... boards) {
    return board.bits | combineBoardsJoiner(boards...);
  }

  static B64 findCommonBitsJoiner(Bitboard board) {
    return board.bits;
  }

  template <typename... Boards>
  static B64 findCommonBitsJoiner(Bitboard board, Boards... boards) {
    return board.bits & findCommonBitsJoiner(boards...);
  }

  // Friend the public free functions so they can see the private helpers
  template <typename T, typename... Boards>
  friend Bitboard combineBoards(T b, Boards... boards);

  template <typename T, typename... Boards>
  friend Bitboard findCommonBits(T b, Boards... boards);

public:
  Bitboard();                 // Default constructor
  Bitboard(B64 initialData);  // Constructor with initial data
  static Bitboard boardFromIndex(BoardIndex index); // creates a board with only the given index set
  void clear();

  void visualize() const;

  // Using this method is unsafe, avoid where possible
  B64 getBoard() const;
  bool getBit(BoardIndex index) const;
  int countSetBits() const;
  BoardIndex singleBitIndex() const;
  bool hasRemainingBits() const;
  bool isEmpty() const;

  void setBit(BoardIndex index);
  void setBitsFrom(Bitboard otherBoard);
  void clearBit(BoardIndex index);
  void clearLowestBit();
  void clearBitsFrom(Bitboard otherBoard);
  void toggleBit(BoardIndex index);
  void toggleBitsFrom(Bitboard otherBoard);
  Bitboard getInverted() const;
  Bitboard getCommonBitsWith(Bitboard otherBoard) const;
  Bitboard getCombinedWith(Bitboard otherBoard) const;
  Bitboard getWithoutBitsFrom(Bitboard otherBoard) const;

  // extracts the lowest bit, deleting it from the board
  Bitboard popLowestBit();

  // Shifts the entire board one tile in any direction
  void shift(Direction direction);

  // ONLY use to 'loop' a piece over the board
  // popLowestBit should be used where possible
  void nextTile();

  // Returns a copy of the board shifted one tile in any direction
  Bitboard look(Direction direction) const;

  // Traces the path of a sliding piece, including the first colision
  // Expected to be used on a board with only one active bit
  Bitboard slidePath(Direction direction, const Bitboard all_pieces) const;

  Bitboard lowerThanSingleBit() const;
  Bitboard higherThanSingleBit() const;

  // Returns linear path on a single row to destination
  // Expected to be used on a board with only one active bit
  Bitboard sameRowPathTo(Bitboard destination) const;
};

template <typename T, typename... Boards>
Bitboard combineBoards(T board, Boards... boards) {
  static_assert(Bitboard::areAllSame<Bitboard, T, Boards...>(),
                "All parameters must be of the Bitboard class");
  return Bitboard(Bitboard::combineBoardsJoiner(board, boards...));
}

template <typename T, typename... Boards>
Bitboard findCommonBits(T board, Boards... boards) {
  static_assert(Bitboard::areAllSame<Bitboard, T, Boards...>(),
                "All parameters must be of the Bitboard class");
  return Bitboard(Bitboard::findCommonBitsJoiner(board, boards...));
}