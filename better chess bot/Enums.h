#pragma once

enum class Color { NONE, WHITE, BLACK };
enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class AttackerType { NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN }; // inverted value for capturing piece

enum class MoveType { NORMAL, PAWN_UNIQE, CASTLE_LONG, CASTLE_SHORT };

enum class GameOverCause { CHECKMATE, RESIGNATION, TIMEOUT, STALEMALTE, INSUFFICENT_MATERIAL, FIFTY_MOVE_RULE, REPETITION, AGREED_DRAW };
