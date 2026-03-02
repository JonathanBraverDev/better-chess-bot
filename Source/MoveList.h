#pragma once

#include "Move.h"
#include <array>

// A fixed-size array to store moves during generation, bypassing heap
// allocation. 256 is a safe maximum for chess moves from a single position.
struct MoveList {
  std::array<Move, 256> moves;
  int count = 0;

  inline void push_back(const Move &move) { moves[count++] = move; }

  inline void clear() { count = 0; }

  inline Move *begin() { return moves.data(); }
  inline Move *end() { return moves.data() + count; }
  inline const Move *begin() const { return moves.data(); }
  inline const Move *end() const { return moves.data() + count; }

  inline Move &operator[](int index) { return moves[index]; }
  inline const Move &operator[](int index) const { return moves[index]; }

  inline int size() const { return count; }
  inline bool empty() const { return count == 0; }
};
