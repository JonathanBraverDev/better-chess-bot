#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
  TEST_CLASS(MoveGenerationTests) {
  public:
    TEST_CLASS_INITIALIZE(ClassInitialize) {
      Position::InitializeMoves();
    }

    TEST_METHOD(Perft_Position1_StartPos) {
      // Starting Position
      // Depth 1: 20 moves
      Position pos = Position::fromFen(
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      Assert::AreEqual((size_t)20, pos.getLegalMoves().size());
    }

    TEST_METHOD(Perft_Position2_Kiwipete) {
      // Kiwipete
      // Depth 1: 48 moves
      Position pos = Position::fromFen(
          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
      Assert::AreEqual((size_t)48, pos.getLegalMoves().size());
    }

    TEST_METHOD(Perft_Position3) {
      // Position 3
      // Depth 1: 14 moves
      Position pos = Position::fromFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
      Assert::AreEqual((size_t)14, pos.getLegalMoves().size());
    }

    TEST_METHOD(Perft_Position4) {
      // Position 4
      // Depth 1: 6 moves
      Position pos = Position::fromFen(
          "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
      Assert::AreEqual((size_t)6, pos.getLegalMoves().size());
    }

    TEST_METHOD(Perft_Position5) {
      // Position 5
      // Depth 1: 44 moves
      Position pos = Position::fromFen(
          "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
      Assert::AreEqual((size_t)44, pos.getLegalMoves().size());
    }

    TEST_METHOD(Perft_Position6) {
      // Position 6
      // Depth 1: 46 moves
      Position pos = Position::fromFen(
          "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
      Assert::AreEqual((size_t)46, pos.getLegalMoves().size());
    }

    TEST_METHOD(Castling_White_BothSides) {
      // White Rooks on a1/h1, King on e1. No blockers, no checks.
      // E1 can castle both sides.
      // Expected: 26 moves (10 Ra1 + 9 Rh1 + 5 Ke1 + 2 Castles)
      Position pos = Position::fromFen("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
      Assert::AreEqual((size_t)26, pos.getLegalMoves().size());
    }

    TEST_METHOD(EnPassant_White) {
      // White Pawn c5, Black Pawn b5 (just moved from b7 -> ep target b6)
      // White King e1.
      // Expected: 7 moves (5 King + 1 Push + 1 EP Capture)
      Position pos = Position::fromFen("8/8/8/kpP5/8/8/8/4K3 w - b6 0 1");
      for (auto& move : pos.getLegalMoves()) {
        std::cout << move.verboseDecode() << std::endl;
      }
      Assert::AreEqual((size_t)7, pos.getLegalMoves().size());
    }

    TEST_METHOD(Promotion_White) {
      // White Pawn a7, White King e1. Black King a1.
      // Expected: 9 moves (5 King + 4 Promotions)
      Position pos = Position::fromFen("8/P7/8/8/8/8/8/k3K3 w - - 0 1");
      Assert::AreEqual((size_t)9, pos.getLegalMoves().size());
    }

    TEST_METHOD(Stalemate_Black) {
      // Black King h8 stuck. White King h6, Queen c4 covers g8.
      // Expected: 0 legal moves (Stalemate)
      Position pos = Position::fromFen("7k/8/7K/8/2Q5/8/8/8 b - - 0 1");
      Assert::AreEqual((size_t)0, pos.getLegalMoves().size());
    }

    TEST_METHOD(Checkmate_Black) {
      // Black King h8. White King h6, Queen g7 covers everything and checks.
      // Expected: 0 legal moves (Checkmate)
      Position pos = Position::fromFen("7k/6Q1/7K/8/8/8/8/8 b - - 0 1");
      Assert::AreEqual((size_t)0, pos.getLegalMoves().size());
    }
  };
}
