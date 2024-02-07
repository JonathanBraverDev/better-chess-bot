#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {

    TEST_CLASS(BitboardMovementTests) {
    public:

        TEST_METHOD(Up) {
            // Arrange
            Bitboard testBoard(A1);

            // Act
            testBoard.move(Direction::UP);

            // Assert
            // Add assertions to check the expected state of the board after the move
            Assert::AreEqual(A2, testBoard.getBoard());
        }

        TEST_METHOD(UpOut) {
            Bitboard testBoard(C8);

            testBoard.move(Direction::UP);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(Down) {
            Bitboard testBoard(H5);

            testBoard.move(Direction::DOWN);

            Assert::AreEqual(H4, testBoard.getBoard());
        }

        TEST_METHOD(DownOut) {
            Bitboard testBoard(D1);

            testBoard.move(Direction::DOWN);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(Left) {
            Bitboard testBoard(D3);

            testBoard.move(Direction::LEFT);

            Assert::AreEqual(C3, testBoard.getBoard());
        }

        TEST_METHOD(LeftOut) {
            Bitboard testBoard(A5);

            testBoard.move(Direction::LEFT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(Right) {
            Bitboard testBoard(E2);

            testBoard.move(Direction::RIGHT);

            Assert::AreEqual(F2, testBoard.getBoard());
        }

        TEST_METHOD(RightOut) {
            Bitboard testBoard(H2);

            testBoard.move(Direction::RIGHT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(UpLeft) {
            Bitboard testBoard(B2);

            testBoard.move(Direction::UP_LEFT);

            Assert::AreEqual(A3, testBoard.getBoard());
        }

        TEST_METHOD(UpLeftOut) {
            Bitboard testBoard(A2);

            testBoard.move(Direction::UP_LEFT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(UpRight) {
            Bitboard testBoard(F4);

            testBoard.move(Direction::UP_RIGHT);

            Assert::AreEqual(G5, testBoard.getBoard());
        }

        TEST_METHOD(UpRightOut) {
            Bitboard testBoard(H7);

            testBoard.move(Direction::UP_RIGHT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(DownLeft) {
            Bitboard testBoard(F6);

            testBoard.move(Direction::DOWN_LEFT);

            Assert::AreEqual(E5, testBoard.getBoard());
        }

        TEST_METHOD(DownLeftOut) {
            Bitboard testBoard(C1);

            testBoard.move(Direction::DOWN_LEFT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }

        TEST_METHOD(DownRight) {
            Bitboard testBoard(B7);

            testBoard.move(Direction::DOWN_RIGHT);

            Assert::AreEqual(C6, testBoard.getBoard());
        }

        TEST_METHOD(DownRightOut) {
            Bitboard testBoard(A1);

            testBoard.move(Direction::DOWN_RIGHT);

            Assert::AreEqual(0ULL, testBoard.getBoard());
        }
    };
}
