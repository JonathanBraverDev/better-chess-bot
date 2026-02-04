cmake_minimum_required(VERSION 3.10)

project(BetterChessBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the source directory to the include path
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/better chess bot")

# Define the source files
set(SOURCES
    "better chess bot/better chess bot.cpp"
    "better chess bot/Bitboard.cpp"
    "better chess bot/Game.cpp"
    "better chess bot/GameState.cpp"
    "better chess bot/Move.cpp"
    "better chess bot/Position.cpp"
)

# Add the executable
add_executable(BetterChessBot ${SOURCES})

# Handle platform-specific settings if necessary (none strictly needed for basic console app yet)
if(MSVC)
    target_compile_options(BetterChessBot PRIVATE /W3)
else()
    target_compile_options(BetterChessBot PRIVATE -Wall -Wextra)
endif()
