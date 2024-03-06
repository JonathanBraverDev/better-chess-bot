#pragma once

#include "Structs.h"

class Bitboard {
private:
    B64 board;

    // B64 shouldn't be exposed
    B64 lowestBitBoard() const;

    static const DirectionCheck direction_check[];

public:
    Bitboard(); // Default constructor
    Bitboard(B64 initialData); // Constructor with initial data
    void clear();

    void visualize() const;

    // Using this method is unsafe, avoid where possible
    B64 getBoard() const;
    bool getBit(int index) const;
    int countSetBits() const;
    int singleBitIndex() const;
    bool hasRemainingBits() const;
    bool isEmpty() const;

    void setBit(int index);
    void clearBit(int index);
    void clearLowestBit();
    void setBitsFrom(Bitboard otherBoard);
    void clearBitsFrom(Bitboard otherBoard);

    // extracts the lowest bit, deleting it from the board
    Bitboard popLowestBit();

    // Move the entire board one tile in any direction
    void move(Direction direction);

    // ONLY use to 'loop' a piece over the board
    // popLowestBit should be used where possible
    void nextTile();

    // Return a copy of the entire board moved one tile in any direction
    Bitboard look(Direction direction) const;

    // Traces the path of a sliding piece, including the first colision
    // Expected to be used on a board with only one active bit
    Bitboard slidePath(Direction direction, const Bitboard allPieces) const;
};

// hiding B64 return types, all operations should be done on Bitboard objects
namespace {

    template <typename T>
    constexpr bool areAllSame() {
        return true;
    }

    // Checks if all types in the variadic template are Bitboards (using another variadic template, C++14 is funny)
    template <typename T, typename U, typename... Rest>
    constexpr bool areAllSame() {
        return std::is_same<T, U>::value&& areAllSame<T, Rest...>();
    }

        
    template <typename T>
    B64 combineBoardsJoiner(T board) {
        return board.getBoard();
    }

    template <typename T, typename... Boards>
    B64 combineBoardsJoiner(T board, Boards... boards) {
        return board.getBoard() | combineBoardsJoiner(boards...);
    }

    template <typename T>
    B64 findCommonBitsJoiner(T board) {
        return board.getBoard();
    }

    template <typename T, typename... Boards>
    B64 findCommonBitsJoiner(T board, Boards... boards) {
        return board.getBoard() & findCommonBitsJoiner(boards...);
    }
}

namespace BitboardOperations {
    // Create a board containing all bits from inputs
    template <typename T, typename... Boards>
    Bitboard combineBoards(T board, Boards... boards) {
        static_assert(areAllSame<Bitboard, T, Boards...>(), "All parameters must be of the Bitboard class");
        return Bitboard(combineBoardsJoiner(board, boards...));
    }

    // Create a bitboard of overlaping bits on all inputs
    template <typename T, typename... Boards>
    Bitboard findCommonBits(T board, Boards... boards) {
        static_assert(areAllSame<Bitboard, T, Boards...>(), "All parameters must be of the Bitboard class");
        return Bitboard(findCommonBitsJoiner(board, boards...));
    }
}
