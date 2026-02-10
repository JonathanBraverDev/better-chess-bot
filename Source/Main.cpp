#include "Position.h"
#include <chrono>
#include <iostream>
#include <string>
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

int main() {
  Position::InitializeMoves();
  std::cout << "Running Perft Tests..." << std::endl;

  // Start Position
  runPerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1,
               20);
  runPerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2,
               400);
  runPerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3,
               8902);
  runPerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4,
               197281); // the fastest checkmate (fool's mate) is here, but it
                        // will not be detected at depth 4
  runPerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5,
               4865609);

  // Kiwipete
  runPerftTest(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1,
      48);

  // Position 5
  runPerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1,
               44);

  std::cout << "Perft Tests Completed." << std::endl;
  return 0;
}
