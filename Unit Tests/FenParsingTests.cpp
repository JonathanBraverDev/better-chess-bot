#include "pch.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {
  TEST_CLASS(FenParsingTests) {
  public:
    TEST_METHOD(StartingPosition) {
      Position pos = Position::fromFen(
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

      // White Pieces
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::PAWN).getBoard() == ROW_2);
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::ROOK).getBoard() == (A1 | H1));
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::KNIGHT).getBoard() == (B1 | G1));
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::BISHOP).getBoard() == (C1 | F1));
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::QUEEN).getBoard() == D1);
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::KING).getBoard() == E1);

      // Black Pieces
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::PAWN).getBoard() == ROW_7);
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::ROOK).getBoard() == (A8 | H8));
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::KNIGHT).getBoard() == (B8 | G8));
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::BISHOP).getBoard() == (C8 | F8));
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::QUEEN).getBoard() == D8);
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::KING).getBoard() == E8);
    }

    TEST_METHOD(EmptyBoard) {
      Position pos = Position::fromFen("8/8/8/8/8/8/8/8 w - - 0 1");
      Assert::IsTrue(pos.getAllPieces().isEmpty());
    }

    TEST_METHOD(SpecificPiecePlacement) {
      // White rook on A1, Black king on E8
      Position pos = Position::fromFen("4k3/8/8/8/8/8/8/R7 w - - 0 1");
      Assert::IsTrue(pos.getPieces(Color::WHITE, PieceType::ROOK).getBoard() == A1);
      Assert::IsTrue(pos.getPieces(Color::BLACK, PieceType::KING).getBoard() == E8);
      Assert::AreEqual(2, pos.getAllPieces().countSetBits());
    }
  };
}
