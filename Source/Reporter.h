#pragma once

#include "Enums.h"
#include "PerftStats.h"
#include "Structs.h"
#include <iostream>
#include <string>
#include <vector>

class Reporter {
public:
  static void printHeader(const std::string &title) {
    std::cout << "\n=== " << title << " ===" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
  }

  static void printSeparator() {
    std::cout << std::string(80, '=') << std::endl;
  }

  static void printPerftMove(BoardIndex from, BoardIndex to, long long nodes) {
    std::cout << tileNames[from] << "->" << tileNames[to] << ": " << nodes
              << std::endl;
  }

  static void printStats(const PerftStats &stats) {
    std::cout << "Nodes:      " << stats.nodes << std::endl;
    std::cout << "Captures:   " << stats.captures << std::endl;
    std::cout << "EP:         " << stats.enPassant << std::endl;
    std::cout << "Castles:    " << stats.castles << std::endl;
    std::cout << "Promotions: " << stats.promotions << std::endl;
    std::cout << "Checks:     " << stats.checks << std::endl;
    std::cout << "Checkmates: " << stats.checkmates << std::endl;
  }

  static void printStatsCompact(const PerftStats &stats) {
    std::cout << "N:" << stats.nodes << " C:" << stats.captures
              << " EP:" << stats.enPassant << " CS:" << stats.castles
              << " P:" << stats.promotions << " CH:" << stats.checks
              << " CM:" << stats.checkmates;
  }

  static void printExecutionTime(double seconds) {
    std::cout << "Time:       " << seconds << " s" << std::endl;
    if (seconds > 0) {
      // Placeholder for nodes/sec if we pass nodes
    }
  }

  static void printLine(const std::string &line) {
    std::cout << line << std::endl;
  }

  static void printMessage(const std::string &msg) {
    std::cout << msg << std::endl;
  }
};
