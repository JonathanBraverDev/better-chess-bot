#pragma once

#include <map>
#include "BoardConstants.h"

enum class Color { NONE, WHITE, BLACK };
enum class PieceType { NONE, KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN };
enum class AttackerType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN }; // inverted value for capturing piece

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

enum class GameOverCause { CHECKMATE, RESIGNATION, TIMEOUT, STALEMALTE, INSUFFICENT_MATERIAL, FIFTY_MOVE_RULE, REPETITION, AGREED_DRAW };

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
