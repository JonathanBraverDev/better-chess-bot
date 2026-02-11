#pragma once

#include "PerftStats.h"
#include "Position.h"
#include <string>

// Simple perft node counter
long long perft(Position &pos, int depth);

// Detailed perft that counts special events
PerftStats detailedPerft(Position &pos, int depth, bool isRoot = false);

// Shows detailed statistics for each root move
void detailedPerftDivide(Position &pos, int depth);

// Standard perft divide
void perftDivide(Position &pos, int depth);

// Run a perft test with expected node count validation
void runPerftTest(std::string fen, int depth, long long expectedNodes);

// Run a detailed perft test with timing and stats
void runDetailedPerftTest(std::string fen, int depth,
                          PerftStats *expectedStats);
