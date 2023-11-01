#pragma once

enum class Color { WHITE, BLACK };
enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class AttackerType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN }; // inverted value for capturing piece

enum class MoveType { NORMAL, PAWN_UNIQE, CASTLE_LONG, CASTLE_SHORT };
