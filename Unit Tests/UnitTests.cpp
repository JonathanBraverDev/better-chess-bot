#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {

    TEST_CLASS(BitboardMovementTests) {
    public:

        TEST_METHOD(Up) {
            // Arrange
            Bitboard test_board(A1);

            // Act
            test_board.shift(Direction::UP);

            // Assert
            // Add assertions to check the expected state of the board after the move
            Assert::AreEqual(A2, test_board.getBoard());
        }

        TEST_METHOD(UpOut) {
            Bitboard test_board(C8);

            test_board.shift(Direction::UP);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(Down) {
            Bitboard test_board(H5);

            test_board.shift(Direction::DOWN);

            Assert::AreEqual(H4, test_board.getBoard());
        }

        TEST_METHOD(DownOut) {
            Bitboard test_board(D1);

            test_board.shift(Direction::DOWN);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(Left) {
            Bitboard test_board(D3);

            test_board.shift(Direction::LEFT);

            Assert::AreEqual(C3, test_board.getBoard());
        }

        TEST_METHOD(LeftOut) {
            Bitboard test_board(A5);

            test_board.shift(Direction::LEFT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(Right) {
            Bitboard test_board(E2);

            test_board.shift(Direction::RIGHT);

            Assert::AreEqual(F2, test_board.getBoard());
        }

        TEST_METHOD(RightOut) {
            Bitboard test_board(H2);

            test_board.shift(Direction::RIGHT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(UpLeft) {
            Bitboard test_board(B2);

            test_board.shift(Direction::UP_LEFT);

            Assert::AreEqual(A3, test_board.getBoard());
        }

        TEST_METHOD(UpLeftOut) {
            Bitboard test_board(A2);

            test_board.shift(Direction::UP_LEFT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(UpRight) {
            Bitboard test_board(F4);

            test_board.shift(Direction::UP_RIGHT);

            Assert::AreEqual(G5, test_board.getBoard());
        }

        TEST_METHOD(UpRightOut) {
            Bitboard test_board(H7);

            test_board.shift(Direction::UP_RIGHT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(DownLeft) {
            Bitboard test_board(F6);

            test_board.shift(Direction::DOWN_LEFT);

            Assert::AreEqual(E5, test_board.getBoard());
        }

        TEST_METHOD(DownLeftOut) {
            Bitboard test_board(C1);

            test_board.shift(Direction::DOWN_LEFT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }

        TEST_METHOD(DownRight) {
            Bitboard test_board(B7);

            test_board.shift(Direction::DOWN_RIGHT);

            Assert::AreEqual(C6, test_board.getBoard());
        }

        TEST_METHOD(DownRightOut) {
            Bitboard test_board(A1);

            test_board.shift(Direction::DOWN_RIGHT);

            Assert::AreEqual(0ULL, test_board.getBoard());
        }
    };

    TEST_CLASS(BitboardSlideTests) {
        TEST_METHOD(UpEdge) {
            Bitboard test_board(C4);

            Bitboard slide_result = test_board.slidePath(Direction::UP, 0ULL);

            Assert::AreEqual(C5 | C6 | C7 | C8, slide_result.getBoard());
        }

        TEST_METHOD(UpBlocked) {
            Bitboard test_board(H5);

            Bitboard slide_result = test_board.slidePath(Direction::UP, H7);

            Assert::AreEqual(H6 | H7, slide_result.getBoard());
        }

        TEST_METHOD(DownEdge) {
            Bitboard test_board(A3);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN, 0ULL);

            Assert::AreEqual(A2 | A1, slide_result.getBoard());
        }

        TEST_METHOD(DownBlocked) {
            Bitboard test_board(F2);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN, F1);

            Assert::AreEqual(F1, slide_result.getBoard());
        }

        TEST_METHOD(LeftEdge) {
            Bitboard test_board(D8);

            Bitboard slide_result = test_board.slidePath(Direction::LEFT, 0ULL);

            Assert::AreEqual(C8 | B8 | A8, slide_result.getBoard());
        }

        TEST_METHOD(LeftBlocked) {
            Bitboard test_board(H5);

            Bitboard slide_result = test_board.slidePath(Direction::LEFT, F5);

            Assert::AreEqual(G5 | F5, slide_result.getBoard());
        }

        TEST_METHOD(RightEdge) {
            Bitboard test_board(F2);

            Bitboard slide_result = test_board.slidePath(Direction::RIGHT, 0ULL);

            Assert::AreEqual(G2 | H2, slide_result.getBoard());
        }

        TEST_METHOD(RightBlocked) {
            Bitboard test_board(E4);

            Bitboard slide_result = test_board.slidePath(Direction::RIGHT, G4);

            Assert::AreEqual(F4 | G4, slide_result.getBoard());
        }

        TEST_METHOD(UpLeftEdge) {
            Bitboard test_board(C1);

            Bitboard slide_result = test_board.slidePath(Direction::UP_LEFT, 0ULL);

            Assert::AreEqual(B2 | A3, slide_result.getBoard());
        }

        TEST_METHOD(UpLeftBlocked) {
            Bitboard test_board(H6);

            Bitboard slide_result = test_board.slidePath(Direction::UP_LEFT, F8);

            Assert::AreEqual(G7 | F8, slide_result.getBoard());
        }

        TEST_METHOD(UpRightEdge) {
            Bitboard test_board(B5);

            Bitboard slide_result = test_board.slidePath(Direction::UP_RIGHT, 0ULL);

            Assert::AreEqual(C6 | D7 | E8, slide_result.getBoard());
        }

        TEST_METHOD(UpRightBlocked) {
            Bitboard test_board(A2);

            Bitboard slide_result = test_board.slidePath(Direction::UP_RIGHT, E6);

            Assert::AreEqual(B3 | C4 | D5 | E6, slide_result.getBoard());
        }

        TEST_METHOD(DownLeftEdge) {
            Bitboard test_board(E5);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN_LEFT, 0ULL);

            Assert::AreEqual(D4 | C3 | B2 | A1, slide_result.getBoard());
        }

        TEST_METHOD(DownLeftBlocked) {
            Bitboard test_board(D2);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN_LEFT, C1);

            Assert::AreEqual(C1, slide_result.getBoard());
        }

        TEST_METHOD(DownRightEdge) {
            Bitboard test_board(C4);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN_RIGHT, 0ULL);

            Assert::AreEqual(D3 | E2 | F1, slide_result.getBoard());
        }

        TEST_METHOD(DownRightBlocked) {
            Bitboard test_board(D5);

            Bitboard slide_result = test_board.slidePath(Direction::DOWN_RIGHT, G2);

            Assert::AreEqual(E4 | F3 | G2, slide_result.getBoard());
        }
    };
}
