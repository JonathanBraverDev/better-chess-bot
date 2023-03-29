#pragma once

#include <vector>

#include "Position structure.h"

struct SearchPreallocation {
	std::vector<SidedPosition>* valid_positions;
	std::vector<SidedPosition> all_positions;
	std::vector<B64> single_pieces; // posiblle to use a solid array length 218 (with an external index?)
	std::vector<B64> single_moves;
};