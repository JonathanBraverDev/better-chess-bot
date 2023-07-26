#pragma once

#include <vector>

#include "Position structure.h"
#include "Move structure.h"

struct SearchPreallocation {
	std::vector<BitMove>* valid_moves;
	std::vector<BitMove> all_moves;
	std::vector<B64> single_pieces;
	std::vector<B64> single_boards; // posiblle to use a solid array length 218 (with an external index?)
};