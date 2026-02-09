#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
  TEST_CLASS(EvaluationTests) {
  public:
    TEST_METHOD(MaterialCount_StartPos) {
        // Start position
        Position pos = Position::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        int score = Evaluator::evaluate(pos);
        Assert::AreEqual(0, score);
    }

    TEST_METHOD(MaterialCount_WhiteAdvantage) {
      // White has extra pawn
      // Score should be +100 (from white perspective)
      Position pos = Position::fromFen("8/8/8/8/8/8/P7/8 w - - 0 1");
      int score = Evaluator::evaluate(pos);
      Assert::AreEqual(CENTIPAWNS.pawn_value, score);
    }

    TEST_METHOD(MaterialCount_BlackAdvantage) {
      // Black has extra pawn.
      // White to move.
      // Score should be -100.
      Position pos = Position::fromFen("8/p7/8/8/8/8/8/8 w - - 0 1");
      int score = Evaluator::evaluate(pos);
      Assert::AreEqual(-CENTIPAWNS.pawn_value, score);
    }

    TEST_METHOD(MaterialCount_BlackToMove_Relative) {
      // Black has extra pawn.
      // Black to move.
      // Own pieces: Black. (Advantage).
      // Score should be +100 (from black perspective).
      Position pos = Position::fromFen("8/p7/8/8/8/8/8/8 b - - 0 1");
      int score = Evaluator::evaluate(pos);
      Assert::AreEqual(CENTIPAWNS.pawn_value, score);
    }
  };
}
