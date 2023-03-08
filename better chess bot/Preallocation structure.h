#pragma once

#include <vector>

#include "Position structure.h"

struct SearchPreallocation {
	std::vector<SidedPosition>* valid_positions;
	std::vector<SidedPosition> all_positions;
	std::vector<B64> single_pieces;
	std::vector<B64> single_moves;
};