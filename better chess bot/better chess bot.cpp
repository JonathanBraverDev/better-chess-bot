// better chess bot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();



    auto end = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();    
    std::cout << (double)microseconds << std::endl;
}
