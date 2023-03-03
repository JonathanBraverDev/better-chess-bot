#pragma once

#include "Board structure.h"
#include "Move structure.h"


constexpr B64 INNITIAL_WHITE_PAWNS = ROW_2;
constexpr B64 INNITIAL_WHITE_KNIGHTS = B1 | G1;
constexpr B64 INNITIAL_WHITE_BISHOPS = C1 | F1;
constexpr B64 INNITIAL_WHITE_ROOKS = A1 | H1;
constexpr B64 INNITIAL_WHITE_QUEENS = E1;
constexpr B64 INNITIAL_WHITE_KING = D1;

constexpr B64 INNITIAL_BLACK_PAWNS = ROW_7;
constexpr B64 INNITIAL_BLACK_KNIGHTS = B8 | G8;
constexpr B64 INNITIAL_BLACK_BISHOPS = C8 | F8;;
constexpr B64 INNITIAL_BLACK_ROOKS = A8 | H8;
constexpr B64 INNITIAL_BLACK_QUEENS = E8;
constexpr B64 INNITIAL_BLACK_KING = D8;

constexpr B64 INNITIAL_SPECIAL_MOVE_RIGHTS = INNITIAL_WHITE_KING | INNITIAL_WHITE_ROOKS | INNITIAL_BLACK_KING | INNITIAL_BLACK_ROOKS;

constexpr SidedPosition INNITIAL_POSITION = { INNITIAL_WHITE_PAWNS,
					                          INNITIAL_WHITE_KNIGHTS,
											  INNITIAL_WHITE_BISHOPS,
											  INNITIAL_WHITE_ROOKS,
											  INNITIAL_WHITE_QUEENS,
											  INNITIAL_WHITE_KING,
											  INNITIAL_BLACK_PAWNS,
											  INNITIAL_BLACK_KNIGHTS,
											  INNITIAL_BLACK_BISHOPS,
											  INNITIAL_BLACK_ROOKS,
											  INNITIAL_BLACK_QUEENS,
											  INNITIAL_BLACK_KING,
											  INNITIAL_SPECIAL_MOVE_RIGHTS,
};

constexpr int INNITIAL_TURNS = 0;
constexpr int INNITIAL_DRAW_TIMER = 0;
constexpr GameState* INNITIAL_PREVIUS_STATE = nullptr;

constexpr GameState INNITIAL_GAMESTATE = { INNITIAL_TURNS,
										   INNITIAL_DRAW_TIMER,
										   INNITIAL_POSITION,
										   INNITIAL_PREVIUS_STATE
};

constexpr PlayerColor INNITIAL_COLOR = WHITE;

constexpr B64 WHITE_KING_SHORT_CASTLE = G1;
constexpr B64 WHITE_ROOK_SHORT_CASTLE = F1;
constexpr B64 WHITE_ROOK_SHORT = H1;
constexpr B64 WHITE_KING_LONG_CASTLE = C1;
constexpr B64 WHITE_ROOK_LONG_CASTLE = D1;
constexpr B64 WHITE_ROOK_LONG = A1;

constexpr B64 BLACK_KING_SHORT_CASTLE = G8;
constexpr B64 BLACK_ROOK_SHORT_CASTLE = F8;
constexpr B64 BLACK_ROOK_SHORT = H8;
constexpr B64 BLACK_KING_LONG_CASTLE = C8;
constexpr B64 BLACK_ROOK_LONG_CASTLE = D8;
constexpr B64 BLACK_ROOK_LONG = A8;

constexpr B64 WHITE_CASTLE_ROW = ROW_1;

constexpr B64 WHITE_PAWN_PROMOTION = ROW_8;
constexpr B64 WHITE_PAWN_JUMP_START = ROW_2;
constexpr B64 WHITE_PAWN_JUMP_END = ROW_4;

constexpr B64 BLACK_PAWN_PROMOTION = ROW_1;
constexpr B64 BLACK_PAWN_JUMP_START = ROW_7;
constexpr B64 BLACK_PAWN_JUMP_END = ROW_5;