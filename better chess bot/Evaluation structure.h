#pragma once

/*
idea dump :

open king penalty for each attack 'lane' (row,col,diagonal) without cover
bishop supiriority - bonus for having both and discoraging 'equal' trades vs knights (done, given slightly higher value)
penatly for stacked pawns - multiple pawns on the same columb = bad bad
bonus for high queen activity near the enemy king / side of the board
bonus for attacking more times than a square/piece is defended
bonus to oponents piece weight to encorage 'fair' trades when having material advantage, bonus increasing mildly as advantage/total piece value goes up
bonus for spicy moves like pins, forks (the cheaper the forking piece in relation to the forked the better) and revealed attacks
bonus for passed pawns, especially ones with a promotion tile off color to the enemy bishop(s)
bonus for outposts (pieces coverd by pawns that can't be kicked out by enemy pawns)
bonus for total area avalible for pieces to move into and/or standing on center tiles (chould be dangerous in endgames, redundant with good moves?)
bonus for rooks on open/semi open columns
bonus for knights in closed positions, bonus for bishops in open positions (again, bishiop supiriority)
bonus/penalty for bishop based on pawn color (white bishop bonus if 12 pawns are on dark somehow)
castling, accounting for side and current advantage (same in equal, opposite with initiative)
denying infiltration squares with pawn for knights in closed position
*/

// trying to avoind floats during computation (but also avoid overflow)
constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 310; // so the bot dosent want to go rook + pawn for 2 knights/bishops
constexpr int BISHOP_VALUE = 320; // slightly over a knight
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;
constexpr int WIN_VALUE = 100000; // aka 'stupidly large number' that you can't achive even with 64 queens
constexpr int DRAW_VALUE = 0; // possibly set negative to discourage draws

struct BasicParameters {
	int pawn_value;
	int knight_value;
	int bishop_value;
	int rook_value;
	int queen_value;
	int win_value;
	int draw_value;
};

constexpr BasicParameters CENTIPAWNS = { PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE, WIN_VALUE, DRAW_VALUE };

// for piece specialists, intended mainly for bots
struct PieceBonuses {
	int pawn_bonus;
	int knight_bonus;
	int bishop_bonus;
	int rook_bonus;
	int queen_bonus;
	int king_bonus;
};

struct AdvancedPieceBonuses {
	int multi_queen_bonus;
	int knight_pair_bonus;
	int bishop_pair_bonus;
	int rook_pair_bonus;
};

// things regarding the current player's pieces and thier defence
struct DefensiveBonuses {
	int trade_penalty; // MY pieces are better, becouse they're MINE
};

// things regarding the opponent's pieces and thier weaknesses
struct OffensiveBonuses {
	int capture_bonus;
	int opponent_king_open;
	int attacks_near_opponent_king;
	int queen_activity;
	int quenn_activity_scaling; // how does the base score adapts, will need a more complex structure
	int overkill_bonus; // more attacker
};

// not entirly alien but a semantic diferantiatin
struct PositionalBonuses {
	int closed_threshold; // some value that cases the bot to consider the pisition as closed
	int closed_knight_bonus;
	int bishop_color_bonus; // looks at blocking pawns on both sides
	int castling_bonus;
	int defender_threshold; // some value that cases the bot to consider itself defending
	int attacker_threshold; // some value that cases the bot to consider itself attacking
	int castling_same_bonus; // should consider attacker
	int castling_diff_bonus; // should consider attacker
	int pawn_space_bonus; // tiles behind pawns
	int max_pawn_space; // a point in whic hthe bot is not exited by more tiles
	int pawn_space_scale; // weight per line/tile taken
	int movment_freedom_bonus; // point bonsu for each avalible move
	int max_movment_freedom; // same as pawn limit
	int multipawn_penalty;
	int pawn_structure_bonus; // again, pretty complex
};

// idea is to pass 2 sets of parameters to functions, one for each player, to allow for asymetric evaluation
// super advanced idea is to make a bot that slowly adapts its play to the current opponent, adjusting the player's parameters on the fly
struct EvalParameters {
	BasicParameters basic;
	PieceBonuses piece_bonuses;
};
