
#include "Evaluation.h"

/*
idea dump :

open king - each attack 'lane' (row,col,diagonal) without cover is + for the otehr side
bishop supiriority - bonus for having both and discoraging 'equal' trades vs knights
penatly for stacked pawns - multiple pawns on the same columb = bad bad
bonus for high queen activity near the enemy king / side of the board
bonus for attacking more times than the square is defended
bonus for attackign tiles near the enemy king
bonus for captues
bonus to oponents piece weight to encorage 'fair' trades when having material advantage, bonus increasing mildly as advantage/total piece value goes up
bonus for spicy moves like pins, forks (the more and cheaper the forking piece the better) and revealed attacks
bonus for total area behind pawns
bonus for passed pawns
bonus for outposts (pieces coverd by pawns that can't be kicked out by enemy pawns)
bonus for total area avalible for pieces to move into and/or standing on center tiles (chould be dangerous in endgames, redundant with good moves?)
bonus for rooks on open/semi open columns
bonus for knights in closed positions, bonus for bishops n open positions (again, bishiop supiriority)
bonus/penalty for bishop based on pawn color (white bishop bonus if 12 pawns are on dark somehow)

make everything as dynamic and generic as possible to allow for many, MANY playstyles for the bot

forced draw with low insuffishent material, i'm leaning towards USCF over FIDE rules (chess.com style)

allow draw offer and make the bot accept if the accuracy and advantage of the player so far makes him likly to win, but also allow mercy draws sometimes ;)
*/

bool check_draw(GameState& current_state) {
    GameState& original_state = current_state;
    bool is_draw = false;
    int repetion_count = 0;

    // this whole thing feels somewhat combersome
    if (current_state.draw_timer == DRAW_MOVES) {
        is_draw = true;

    } else {
        while (current_state.previous_state != nullptr && !is_draw) {

            current_state = *current_state.previous_state;

            if (original_state.position == current_state.position) {
                repetion_count++; // yes this can be compressed to one line
                is_draw = (repetion_count == DRAW_REPETITIONS);
            }
        }
    }

    return is_draw;
}