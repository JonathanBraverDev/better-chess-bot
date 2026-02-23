cmake_minimum_required(VERSION 3.10)
project(BetterChessBot)

set(CMAKE_CXX_STANDARD 17)

# Define the source files
file(GLOB SOURCES "Source/*.cpp")

add_executable(BetterChessBot ${SOURCES})
