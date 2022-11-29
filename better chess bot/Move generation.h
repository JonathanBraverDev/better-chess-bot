#pragma once

#include "Board structure.h"

B64 generate_bishop_moves(const B64 all_pieces, B64 piece);
B64 generate_rook_moves(const B64 all_pieces, B64 piece);
B64 generate_queen_moves(const B64 all_pieces, B64 piece);
