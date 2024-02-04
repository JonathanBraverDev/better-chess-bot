#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests {

    TEST_CLASS(UnitTests) {
    public:

        TEST_METHOD(MoveTest) {
            // Arrange
            Bitboard testBoard(A1);

            // Act
            testBoard.move(Direction::UP);

            // Assert
            // Add assertions to check the expected state of the board after the move
            Assert::AreEqual(B1, testBoard.getBoard());
        }
    };
}
