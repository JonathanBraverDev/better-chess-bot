#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
TEST_CLASS(SearchTests) {
  public:
    TEST_METHOD(MateInOne_ScholarsMate) {
      // Scholar's Mate: 1. e4 e5 2. Qh5 Nc6 3. Bc4 Nf6?? 4. Qxf7#
      Position pos = Position::fromFen("r1bqk1nr/pppp1ppp/2n5/2b1p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 4");
      Move bestMove = Search::search(pos, 1); 
      
      // Expected: Qxf7# (h5 -> f7)
      Assert::AreEqual((int)H5_index, (int)bestMove.getOriginIndex());
      Assert::AreEqual((int)F7_index, (int)bestMove.getDestinationIndex());
    }

    TEST_METHOD(MateInTwo_RookRolling) {
      // Rook Rolling Mate in 2
      // White: King a5, Rook a1, Rook b4. Black: King a8.
      // 1. Rh1 ... Kb8 2. Rh8#
      
      Position pos = Position::fromFen("k7/8/8/K7/1R6/8/8/R7 w - - 0 1");
      Move bestMove = Search::search(pos, 2);
      
      // Expected: Rh1 (a1 -> h1)
      Assert::AreEqual((int)A1_index, (int)bestMove.getOriginIndex());
      Assert::AreEqual((int)H1_index, (int)bestMove.getDestinationIndex());
    }
  };
}
