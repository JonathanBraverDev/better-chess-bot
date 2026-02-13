#pragma once

#include <cstdint>
#include <string>
#include <iostream>

// Structure to hold detailed perft statistics
struct PerftStats {
    uint64_t nodes = 0;
    uint64_t captures = 0;
    uint64_t enPassant = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t checkmates = 0;

    // Operator overloads for easy accumulation
    PerftStats& operator+=(const PerftStats& other) {
        nodes += other.nodes;
        captures += other.captures;
        enPassant += other.enPassant;
        castles += other.castles;
        promotions += other.promotions;
        checks += other.checks;
        checkmates += other.checkmates;
        return *this;
    }

    PerftStats operator+(const PerftStats& other) const {
        PerftStats result = *this;
        result += other;
        return result;
    }

    // Pretty print the statistics
    void print() const {
        std::cout << "Nodes:      " << nodes << std::endl;
        std::cout << "Captures:   " << captures << std::endl;
        std::cout << "E.P.:       " << enPassant << std::endl;
        std::cout << "Castles:    " << castles << std::endl;
        std::cout << "Promotions: " << promotions << std::endl;
        std::cout << "Checks:     " << checks << std::endl;
        std::cout << "Checkmates: " << checkmates << std::endl;
    }

    // Print in a compact one-line format
    void printCompact() const {
        std::cout << "N:" << nodes 
                  << " C:" << captures 
                  << " EP:" << enPassant 
                  << " CS:" << castles 
                  << " P:" << promotions 
                  << " CH:" << checks 
                  << " CM:" << checkmates;
    }
};
