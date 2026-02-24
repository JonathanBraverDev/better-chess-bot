#include "Perft.h"
#include "Reporter.h"
#include <chrono>
#include <vector>

// Simple Perft runner
long long perft(Position &pos, int depth) {
  if (depth == 0) {
    return 1;
  }

  MoveList moves = pos.getLegalMoves();
  long long nodes = 0;

  for (const auto &move : moves) {
    pos.makeMove(move);
    nodes += perft(pos, depth - 1);
    pos.undoMove(move);
  }

  return nodes;
}

// Detailed Perft that counts special events
PerftStats detailedPerft(Position &pos, int depth, bool isRoot) {
  PerftStats stats;

  MoveList moves = pos.getLegalMoves();

  if (depth == 1) {
    // At depth 1, count the leaf nodes and their properties
    for (const auto &move : moves) {
      stats.nodes++;

      // Count captures
      if (move.isCapture()) {
        stats.captures++;

        // Check if it's en passant
        if (move.isEnPassant()) {
          stats.enPassant++;
        }
      }

      // Count castling moves
      if (move.isCastle()) {
        stats.castles++;
      }

      // Count promotions
      if (move.isPromotion()) {
        stats.promotions++;
      }

      // Count checks
      if (move.isCheck()) {
        stats.checks++;

        // If there are no moves for a checked opponent, it's a checkmate
        pos.makeMove(move);
        MoveList opponentMoves = pos.getLegalMoves();
        if (opponentMoves.empty()) {
          stats.checkmates++;
        }
        pos.undoMove(move);
      }
    }
  } else {
    // Recurse deeper
    for (const auto &move : moves) {
      pos.makeMove(move);
      PerftStats childStats = detailedPerft(pos, depth - 1, false);
      stats += childStats;
      pos.undoMove(move);
    }
  }

  return stats;
}

// Detailed Perft Divide - shows stats for each root move
void detailedPerftDivide(Position &pos, int depth) {
  MoveList moves = pos.getLegalMoves();
  PerftStats totalStats;

  Reporter::printHeader("Detailed Perft Divide (Depth " + std::to_string(depth) + ")");

  for (const auto &move : moves) {
    pos.makeMove(move);

    PerftStats moveStats = detailedPerft(pos, depth - 1, false);
    totalStats += moveStats;
    
    pos.undoMove(move);

    Reporter::printPerftMove(move.getOriginIndex(), move.getDestinationIndex(), moveStats.nodes);
  }

  Reporter::printSeparator();
  Reporter::printMessage("TOTALS:");
  Reporter::printStats(totalStats);
  Reporter::printSeparator();
}

// Standard perft divide (kept for backward compatibility)
void perftDivide(Position &pos, int depth) {
  MoveList moves = pos.getLegalMoves();
  long long totalNodes = 0;

  for (const auto &move : moves) {
    pos.makeMove(move);
    long long nodes = perft(pos, depth - 1);
    pos.undoMove(move);
    
    totalNodes += nodes;

    Reporter::printPerftMove(move.getOriginIndex(), move.getDestinationIndex(), nodes);
  }

  Reporter::printMessage("\nTotal: " + std::to_string(totalNodes));
}

void runPerftTest(std::string fen, int depth, long long expectedNodes) {
  Position pos = Position::fromFen(fen);
  auto start = std::chrono::high_resolution_clock::now();
  long long nodes = perft(pos, depth);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  if (nodes == expectedNodes) {
    Reporter::printMessage("[PASS] Depth " + std::to_string(depth) + " Nodes: " + std::to_string(nodes) + " Time: " + std::to_string(elapsed.count()) + "s");
  } else {
    Reporter::printMessage("[FAIL] Depth " + std::to_string(depth) + " Expected: " + std::to_string(expectedNodes) + " Got: " + std::to_string(nodes));
  }
}

// Detailed perft test with timing
void runDetailedPerftTest(std::string fen, int depth,
                          PerftStats *expectedStats) {
  Position pos = Position::fromFen(fen);

  Reporter::printHeader("Running Detailed Perft Test");
  Reporter::printMessage("FEN: " + fen);
  Reporter::printMessage("Depth: " + std::to_string(depth));

  auto start = std::chrono::high_resolution_clock::now();
  PerftStats stats = detailedPerft(pos, depth, true);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  Reporter::printMessage("\nResults:");
  Reporter::printStats(stats);
  Reporter::printExecutionTime(elapsed.count());

  if (stats.nodes > 0 && elapsed.count() > 0) {
    Reporter::printMessage("Nodes/sec: " + std::to_string(static_cast<long long>(stats.nodes / elapsed.count())));
  }

  // Validate against expected stats
  bool allMatch = true;
  Reporter::printMessage("\nValidation:");

  if (stats.nodes != expectedStats->nodes) {
    Reporter::printMessage("  [FAIL] Nodes: expected " + std::to_string(expectedStats->nodes) + ", got " + std::to_string(stats.nodes));
    allMatch = false;
  }
  if (stats.captures != expectedStats->captures) {
    Reporter::printMessage("  [FAIL] Captures: expected " + std::to_string(expectedStats->captures) + ", got " + std::to_string(stats.captures));
    allMatch = false;
  }
  if (stats.enPassant != expectedStats->enPassant) {
    Reporter::printMessage("  [FAIL] En Passant: expected " + std::to_string(expectedStats->enPassant) + ", got " + std::to_string(stats.enPassant));
    allMatch = false;
  }
  if (stats.castles != expectedStats->castles) {
    Reporter::printMessage("  [FAIL] Castles: expected " + std::to_string(expectedStats->castles) + ", got " + std::to_string(stats.castles));
    allMatch = false;
  }
  if (stats.promotions != expectedStats->promotions) {
    Reporter::printMessage("  [FAIL] Promotions: expected " + std::to_string(expectedStats->promotions) + ", got " + std::to_string(stats.promotions));
    allMatch = false;
  }
  if (stats.checks != expectedStats->checks) {
    Reporter::printMessage("  [FAIL] Checks: expected " + std::to_string(expectedStats->checks) + ", got " + std::to_string(stats.checks));
    allMatch = false;
  }
  if (stats.checkmates != expectedStats->checkmates) {
    Reporter::printMessage("  [FAIL] Checkmates: expected " + std::to_string(expectedStats->checkmates) + ", got " + std::to_string(stats.checkmates));
    allMatch = false;
  }

  if (allMatch) {
    Reporter::printMessage("  [PASS] All statistics match expected values!");
  }

  Reporter::printSeparator();
}
