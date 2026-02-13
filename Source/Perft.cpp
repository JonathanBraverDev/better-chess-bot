#include "Perft.h"
#include <chrono>
#include <iostream>
#include <vector>

// Simple Perft runner
long long perft(Position &pos, int depth) {
  if (depth == 0) {
    return 1;
  }

  std::vector<Move> moves = pos.getLegalMoves();
  long long nodes = 0;

  for (const auto &move : moves) {
    Position nextPos = pos;
    nextPos.makeMove(move);
    nodes += perft(nextPos, depth - 1);
  }

  return nodes;
}

// Detailed Perft that counts special events
PerftStats detailedPerft(Position &pos, int depth, bool isRoot) {
  PerftStats stats;

  std::vector<Move> moves = pos.getLegalMoves();

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
        Position nextPos = pos;
        nextPos.makeMove(move);
        std::vector<Move> opponentMoves = nextPos.getLegalMoves();
        if (opponentMoves.empty()) {
          stats.checkmates++;
        }
      }
    }
  } else {
    // Recurse deeper
    for (const auto &move : moves) {
      Position nextPos = pos;
      nextPos.makeMove(move);
      PerftStats childStats = detailedPerft(nextPos, depth - 1, false);
      stats += childStats;
    }
  }

  return stats;
}

// Detailed Perft Divide - shows stats for each root move
void detailedPerftDivide(Position &pos, int depth) {
  std::vector<Move> moves = pos.getLegalMoves();
  PerftStats totalStats;

  std::cout << "\n=== Detailed Perft Divide (Depth " << depth
            << ") ===" << std::endl;
  std::cout << std::string(80, '-') << std::endl;

  for (const auto &move : moves) {
    Position nextPos = pos;
    nextPos.makeMove(move);

    PerftStats moveStats = detailedPerft(nextPos, depth - 1, false);
    totalStats += moveStats;

    // Print move info
    std::cout << tileNames[move.getOriginIndex()] << "->"
              << tileNames[move.getDestinationIndex()];

    // Add move type indicators
    if (move.isCapture())
      std::cout << " (capture)";
    if (move.isPromotion())
      std::cout << " (promotion)";
    if (move.getMiscMoveType() == MoveType::CASTLE_SHORT)
      std::cout << " (O-O)";
    if (move.getMiscMoveType() == MoveType::CASTLE_LONG)
      std::cout << " (O-O-O)";
    if (move.isCheck())
      std::cout << " (+)";

    std::cout << ": ";
    moveStats.printCompact();
    std::cout << std::endl;
  }

  std::cout << std::string(80, '=') << std::endl;
  std::cout << "TOTALS:" << std::endl;
  totalStats.print();
  std::cout << std::string(80, '=') << std::endl;
}

// Standard perft divide (kept for backward compatibility)
void perftDivide(Position &pos, int depth) {
  std::vector<Move> moves = pos.getLegalMoves();
  long long totalNodes = 0;

  for (const auto &move : moves) {
    Position nextPos = pos;
    nextPos.makeMove(move);
    long long nodes = perft(nextPos, depth - 1);
    totalNodes += nodes;

    // Simple output: origin->destination
    std::cout << tileNames[move.getOriginIndex()] << "->"
              << tileNames[move.getDestinationIndex()] << ": " << nodes
              << std::endl;
  }

  std::cout << "\nTotal: " << totalNodes << std::endl;
}

void runPerftTest(std::string fen, int depth, long long expectedNodes) {
  Position pos = Position::fromFen(fen);
  auto start = std::chrono::high_resolution_clock::now();
  long long nodes = perft(pos, depth);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  if (nodes == expectedNodes) {
    std::cout << "[PASS] Depth " << depth << " Nodes: " << nodes
              << " Time: " << elapsed.count() << "s" << std::endl;
  } else {
    std::cout << "[FAIL] Depth " << depth << " Expected: " << expectedNodes
              << " Got: " << nodes << std::endl;
  }
}

// Detailed perft test with timing
void runDetailedPerftTest(std::string fen, int depth,
                          PerftStats *expectedStats) {
  Position pos = Position::fromFen(fen);

  std::cout << "\n=== Running Detailed Perft Test ===" << std::endl;
  std::cout << "FEN: " << fen << std::endl;
  std::cout << "Depth: " << depth << std::endl;
  std::cout << std::string(80, '-') << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  PerftStats stats = detailedPerft(pos, depth, true);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "\nResults:" << std::endl;
  stats.print();
  std::cout << "\nTime: " << elapsed.count() << "s" << std::endl;

  if (stats.nodes > 0) {
    std::cout << "Nodes/sec: "
              << static_cast<long long>(stats.nodes / elapsed.count())
              << std::endl;
  }

  // Validate against expected stats
  bool allMatch = true;
  std::cout << "\nValidation:" << std::endl;

  if (stats.nodes != expectedStats->nodes) {
    std::cout << "  [FAIL] Nodes: expected " << expectedStats->nodes << ", got "
              << stats.nodes << std::endl;
    allMatch = false;
  }
  if (stats.captures != expectedStats->captures) {
    std::cout << "  [FAIL] Captures: expected " << expectedStats->captures
              << ", got " << stats.captures << std::endl;
    allMatch = false;
  }
  if (stats.enPassant != expectedStats->enPassant) {
    std::cout << "  [FAIL] En Passant: expected " << expectedStats->enPassant
              << ", got " << stats.enPassant << std::endl;
    allMatch = false;
  }
  if (stats.castles != expectedStats->castles) {
    std::cout << "  [FAIL] Castles: expected " << expectedStats->castles
              << ", got " << stats.castles << std::endl;
    allMatch = false;
  }
  if (stats.promotions != expectedStats->promotions) {
    std::cout << "  [FAIL] Promotions: expected " << expectedStats->promotions
              << ", got " << stats.promotions << std::endl;
    allMatch = false;
  }
  if (stats.checks != expectedStats->checks) {
    std::cout << "  [FAIL] Checks: expected " << expectedStats->checks
              << ", got " << stats.checks << std::endl;
    allMatch = false;
  }
  if (stats.checkmates != expectedStats->checkmates) {
    std::cout << "  [FAIL] Checkmates: expected " << expectedStats->checkmates
              << ", got " << stats.checkmates << std::endl;
    allMatch = false;
  }

  if (allMatch) {
    std::cout << "  [PASS] All statistics match expected values!" << std::endl;
  }

  std::cout << std::string(80, '=') << std::endl;
}
