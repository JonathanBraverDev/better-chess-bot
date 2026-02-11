#include "Perft.h"
#include "Position.h"

int main() {
  Position::InitializeMoves();

  // Standard starting position FEN
  std::string startFen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  std::cout << "=== PERFT VALIDATION TEST ===" << std::endl;

  // Test with correct expected values
  std::cout << "\n--- Test 1: Depth 4 ---" << std::endl;
  PerftStats expected4;
  expected4.nodes = 197281;
  expected4.captures = 1576;
  expected4.enPassant = 0;
  expected4.castles = 0;
  expected4.promotions = 0;
  expected4.checks = 469;
  expected4.checkmates = 8;
  runDetailedPerftTest(startFen, 4, &expected4);

  // Test with incorrect expected values to show validation works
  std::cout << "\n--- Test 2: Depth 5 ---" << std::endl;
  PerftStats expected5;
  expected5.nodes = 4865609;
  expected5.captures = 82719;
  expected5.enPassant = 258;
  expected5.castles = 0;
  expected5.promotions = 0;
  expected5.checks = 27351;
  expected5.checkmates = 347;
  runDetailedPerftTest(startFen, 5, &expected5);

  return 0;
}
