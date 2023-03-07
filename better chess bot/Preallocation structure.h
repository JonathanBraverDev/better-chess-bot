#pragma once

#include <vector>

#include "Position structure.h"

struct PreAllocationVectors { // ~75% of my runtime is reallocations, the rest feels amazing in comparison, let's get to work
	std::vector<SidedPosition> valid_positions;
	std::vector<SidedPosition> all_positions;
	std::vector<B64> single_boards;
	std::vector<B64> pawn_attacks;
};

inline void clear(PreAllocationVectors& allocation) {
	allocation.valid_positions.clear();
	allocation.all_positions.clear();
	allocation.single_boards.clear();
	allocation.pawn_attacks.clear();
}