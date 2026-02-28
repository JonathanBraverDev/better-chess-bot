#pragma once


#include <string>
#include "BoardConstants.h"

enum class Color { WHITE, BLACK };
enum class PieceType { NONE, KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
// inverted value for the capturing piece
enum class AttackerType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

enum class AttackPattern { KING, PAWN, KNIGHT, LINE, DIAGONAL };

constexpr Color Colors[] = {Color::WHITE, Color::BLACK};
constexpr PieceType PieceTypes[] = {
    PieceType::KING, PieceType::PAWN, PieceType::KNIGHT,
    PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN};

// Helper for FEN parsing
inline char pieceToChar(PieceType type) {
    switch (type) {
        case PieceType::PAWN:   return 'p';
        case PieceType::KNIGHT: return 'n';
        case PieceType::BISHOP: return 'b';
        case PieceType::ROOK:   return 'r';
        case PieceType::QUEEN:  return 'q';
        case PieceType::KING:   return 'k';
        default:                return '?';
    }
}

inline PieceType charToPiece(char c) {
    char lower = static_cast<char>(tolower(c));
    switch (lower) {
        case 'p': return PieceType::PAWN;
        case 'n': return PieceType::KNIGHT;
        case 'b': return PieceType::BISHOP;
        case 'r': return PieceType::ROOK;
        case 'q': return PieceType::QUEEN;
        case 'k': return PieceType::KING;
        default:  return PieceType::NONE;
    }
}


// Conversion between PieceType and AttackerType (MVV-LVA ordering)
inline constexpr AttackerType toAttackerType(PieceType type) {
    constexpr AttackerType table[] = {
        AttackerType::NONE,
        AttackerType::KING,
        AttackerType::PAWN,
        AttackerType::KNIGHT,
        AttackerType::BISHOP,
        AttackerType::ROOK,
        AttackerType::QUEEN,
    };
    return table[static_cast<int>(type)];
}

inline constexpr PieceType toPieceType(AttackerType type) {
    constexpr PieceType table[] = {
        PieceType::NONE,
        PieceType::KING,
        PieceType::QUEEN,
        PieceType::ROOK,
        PieceType::BISHOP,
        PieceType::KNIGHT,
        PieceType::PAWN,
    };
    return table[static_cast<int>(type)];
}

inline const std::string& pieceTypeToName(PieceType type) {
    static const std::string names[] = {
        "None", "King", "Pawn", "Knight", "Bishop", "Rook", "Queen"
    };
    return names[static_cast<int>(type)];
}

enum class MoveType { NORMAL, PAWN_UNIQUE, CASTLE_LONG, CASTLE_SHORT };

enum class GameOverCause { CHECKMATE, RESIGNATION, TIMEOUT, STALEMATE, INSUFFICIENT_MATERIAL,
                           FIFTY_MOVE_RULE, REPETITION, AGREED_DRAW };

// All directional names are according to the output of "visualize"
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
    KNIGHT_UP_LEFT,
    KNIGHT_UP_RIGHT,
    KNIGHT_DOWN_LEFT,
    KNIGHT_DOWN_RIGHT,
    KNIGHT_LEFT_UP,
    KNIGHT_LEFT_DOWN,
    KNIGHT_RIGHT_UP,
    KNIGHT_RIGHT_DOWN,
};

const std::string tileNames[] = {
  "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
  "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
  "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
  "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
  "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
  "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
  "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
  "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8"
};
