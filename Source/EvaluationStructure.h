#pragma once

/*
 * TODO:
 * - SEE for attack/defend imbalance scoring.
 * - Pawn island detection and penalty.
 * - Backward pawn detection and penalty.
 * - Passed pawn promotion square off-color to enemy bishop(s).
 * - Dynamic parameter adaptation per opponent.
 */

// ---------------------------------------------------------------------------
//  Material Values
// ---------------------------------------------------------------------------

/// Piece values in centipawns.
namespace MaterialValues {
constexpr int PAWN = 100;
constexpr int KNIGHT = 315; // Discourages R+P for 2 minors
constexpr int BISHOP = 330; // Slightly over knight
constexpr int ROOK = 500;
constexpr int QUEEN = 900;
} // namespace MaterialValues

/// Game-outcome values (not material).
constexpr int WIN_VALUE = 100000;
constexpr int DRAW_VALUE = 0;

/// Core piece weights for material counting.
struct PieceValues {
  int pawn;
  int knight;
  int bishop;
  int rook;
  int queen;
};

constexpr PieceValues CENTIPAWNS = {
    MaterialValues::PAWN, MaterialValues::KNIGHT, MaterialValues::BISHOP,
    MaterialValues::ROOK, MaterialValues::QUEEN};

// ---------------------------------------------------------------------------
//  Piece Preference
// ---------------------------------------------------------------------------

/// Flat preference per piece type.
struct PieceAffinity {
  int pawn;
  int knight;
  int bishop;
  int rook;
  int queen;
};

/// Synergy from specific piece combinations.
struct PieceSynergy {
  int multi_queen; // Extra queens on the board
  int knight_pair; // Both knights retained
  int bishop_pair; // Classic bishop-pair edge
  int rook_pair;   // Complementary open-file control
};

// ---------------------------------------------------------------------------
//  Defense
// ---------------------------------------------------------------------------

/// Own piece safety, king protection, and trade logic.
struct Defense {
  int trade_penalty;      // Reluctance to trade own pieces
  int lead_trade;         // Incentive to trade when ahead
  int own_king_exposure;  // Concern for own king pawn cover
  int own_king_proximity; // Value of defenders near own king
  int threat_awareness;   // Concern for enemy threats
};

// ---------------------------------------------------------------------------
//  Offense
// ---------------------------------------------------------------------------

/// Pressure on the opponent.
struct Offense {
  int capture;         // Having a capture available
  int king_exposure;   // Enemy king lacking pawn cover
  int king_proximity;  // Attacks near enemy king
  int king_lane;       // Pieces on lanes toward enemy king
  int queen_activity;  // Queen centralization
  int queen_mobility;  // Queen movement freedom (scaled)
  int tactical_threat; // Pins, forks, discoveries
  int attack_density;  // Attackers outnumber defenders
  int force_trade;     // Forcing opponent into trades
};

// ---------------------------------------------------------------------------
//  Pawn Structure
// ---------------------------------------------------------------------------

/// Pawn formation quality.
struct PawnStructure {
  int doubled;     // Stacked on the same file
  int passed;      // No enemy blockers ahead
  int chain;       // Connected pawn chains
  int prophylaxis; // Controlling infiltration squares
};

// ---------------------------------------------------------------------------
//  King Safety
// ---------------------------------------------------------------------------

/// King safety evaluation.
struct KingSafety {
  int castling;         // Successfully castled
  int same_side;        // Both kings on the same side
  int opposite_side;    // Kings on opposite sides
  int pawn_shield;      // Squares behind front pawns
  int shield_depth_max; // Max depth for shield eval
  int shield_weight;    // Weight per shield depth unit
};

// ---------------------------------------------------------------------------
//  Mobility
// ---------------------------------------------------------------------------

/// Piece freedom / available moves.
struct Mobility {
  int per_move; // Score per pseudo-legal move
  int cap;      // Maximum mobility score
};

// ---------------------------------------------------------------------------
//  Positional
// ---------------------------------------------------------------------------

/// Position type and piece-placement adjustments.
struct Positional {
  int closed_threshold;  // Pawn count for "closed" position
  int closed_knight;     // Knights in cramped positions
  int open_bishop;       // Bishops in open positions
  int bishop_pawn_color; // Bishop opposite to own pawn color
  int outpost;           // Protected square, immune to pawns
  int rook_open_file;    // No own pawns on file
  int rook_semi_open;    // Only enemy pawns on file
};

// ---------------------------------------------------------------------------
//  Aggregate
// ---------------------------------------------------------------------------

/// Full evaluation config for asymmetric per-side tuning.
struct EvalParameters {
  PieceValues basic;
  PieceAffinity piece;
  PieceSynergy synergy;
  Defense defense;
  Offense offense;
  PawnStructure pawn_structure;
  KingSafety king_safety;
  Mobility mobility;
  Positional positional;
  int defensive_pivot; // Score threshold to favor defense
  int offensive_pivot; // Score threshold to favor attack
};

// ---------------------------------------------------------------------------
//  Defaults
// ---------------------------------------------------------------------------

/// Baseline parameters — starting point for tuning.
constexpr EvalParameters DEFAULT_EVAL = {
    // basic
    {MaterialValues::PAWN, MaterialValues::KNIGHT, MaterialValues::BISHOP,
     MaterialValues::ROOK, MaterialValues::QUEEN},
    // piece
    {0, 0, 0, 0, 0},
    // synergy
    {20, 5, 30, 10},
    // defense
    {-10, 15, -30, 15, -20},
    // offense
    {10, -40, 15, 10, 20, 3, 25, 10, 5},
    // pawn_structure
    {-15, 30, 10, 8},
    // king_safety
    {30, 15, 25, 10, 3, 5},
    // mobility
    {3, 50},
    // positional
    {6, 20, 20, 15, 20, 25, 15},
    // posture pivots
    -200, // defensive_pivot
    200,  // offensive_pivot
};
