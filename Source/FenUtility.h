#pragma once

#include "Position.h"
#include "Types.h"
#include <string>

class FenUtility {
public:
    static Position fromFen(const std::string& fen);
    static std::string toFen(const Position& pos);
};
