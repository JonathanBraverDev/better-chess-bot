#pragma once

#include <map>
#include <string>
#include "BoardConstants.h"

enum class Color { NONE, WHITE, BLACK };
enum class PieceType { NONE, KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
enum class AttackPattern { KING, PAWN, KNIGHT, LINE, DIAGONAL };
// inverted value for the capturing piece
enum class AttackerType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

const std::map<PieceType, AttackerType> pieceTypeToAttackerMap = {
  {PieceType::NONE, AttackerType::NONE},
  {PieceType::KING, AttackerType::KING},
  {PieceType::PAWN, AttackerType::PAWN},
  {PieceType::KNIGHT, AttackerType::KNIGHT},
  {PieceType::BISHOP, AttackerType::BISHOP},
  {PieceType::ROOK, AttackerType::ROOK},
  {PieceType::QUEEN, AttackerType::QUEEN},
};

const std::map<AttackerType, PieceType> attackerTypeToPieceTypeMap = {
  {AttackerType::NONE, PieceType::NONE},
  {AttackerType::KING, PieceType::KING},
  {AttackerType::PAWN, PieceType::PAWN},
  {AttackerType::KNIGHT, PieceType::KNIGHT},
  {AttackerType::BISHOP, PieceType::BISHOP},
  {AttackerType::ROOK, PieceType::ROOK},
  {AttackerType::QUEEN, PieceType::QUEEN},
};

enum class MoveType { NORMAL, PAWN_UNIQE, CASTLE_LONG, CASTLE_SHORT };

enum class GameOverCause { CHECKMATE, RESIGNATION, TIMEOUT, STALEMALTE, INSUFFICENT_MATERIAL,
                           FIFTY_MOVE_RULE, REPETITION, AGREED_DRAW };

// All directional names are are according to the output of "visualize"
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

const std::map<PieceType, std::string> pieceTypeToNameMap = {
  {PieceType::NONE, "None"},
  {PieceType::KING, "King"},
  {PieceType::PAWN, "Pawn"},
  {PieceType::KNIGHT, "Knight"},
  {PieceType::BISHOP, "Bishop"},
  {PieceType::ROOK, "Rook"},
  {PieceType::QUEEN, "Queen"},
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
