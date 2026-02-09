#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
TEST_CLASS(SearchTests) {
  public:
    TEST_METHOD(MateInOne_ScholarsMate) {
      // Scholar's Mate: 1. e4 e5 2. Qh5 Nc6 3. Bc4 Nf6?? 4. Qxf7#
      Position pos = Position::fromFen("r1bqk1nr/pppp1ppp/2n5/2b1p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 4");
      Move bestMove = Search::search(pos, 2); // depth 2 to find mate in 1
      
      // Expected: Qxf7# (h5 -> f7)
      Assert::AreEqual((int)H5_index, (int)bestMove.getOriginIndex());
      Assert::AreEqual((int)F7_index, (int)bestMove.getDestinationIndex());
    }

    TEST_METHOD(MateInZero_BlackMated) {
        // Black is mated. Turn is black.
        // White Q at f7, K at e1, B at c4. Black K at e8.
        Position pos = Position::fromFen("r1bqk1nr/pppp1Qpp/2n5/2b1p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
        
        // Starting position is in check
        Assert::IsTrue(pos.isInCheck());
        
        // With no legal moves
        Assert::AreEqual((size_t)0, pos.getLegalMoves().size());
        
        // Search should return invalid move (or potentially handle it gracefully)
        Move bestMove = Search::search(pos, 1);
        Assert::AreEqual(0, (int)bestMove.getEncodedMove()); // Assuming 0 is invalid/empty move
    }
  };
}
