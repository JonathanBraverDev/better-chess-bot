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
      std::cout << bestMove.verboseDecode(); // for debugging
      Assert::AreEqual((int)H5_index, (int)bestMove.getOriginIndex());
      Assert::AreEqual((int)F7_index, (int)bestMove.getDestinationIndex());
    }
  };
}
