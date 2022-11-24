
#include "Evaluation.h"
#include "Board operation.h"

/*
idea dump :

open king - each attack 'lane' (row,col,diagonal) without cover is + for the otehr side
bishop supiriority - bonus for having both and discoraging 'equal' trades vs knights (done)
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
bonus for knights in closed positions, bonus for bishops in open positions (again, bishiop supiriority)
bonus/penalty for bishop based on pawn color (white bishop bonus if 12 pawns are on dark somehow)

make everything as dynamic and generic as possible to allow for many, MANY playstyles for the bot

forced draw with insuffishent material, i'm leaning towards USCF over FIDE rules (chess.com style)

allow draw offer and make the bot accept if the accuracy and advantage of the player so far makes him likly to win, but also allow mercy draws sometimes ;)
*/

// low material rulings according to chess.com
inline bool insufficient_material(BoardPosition position) {

    bool low_material = false;

    int white_bishops, white_knights;
    int black_bishops, black_knights;

    // potential return guard refactor here
    if ( !(position.white_pawns || position.black_pawns || // where there's a pawn theres (sometimes) a way
           position.white_rooks || position.black_rooks || // immidiatly stop cases with rooks and queens
           position.white_queens || position.black_queens)) { // rook endgames are more likly than queen endgames
        
        // saving counts to avoit countign each time, other wise i'd just do a MASSIVE return
        white_bishops = count_bits64(position.white_bishops);
        white_knights = count_bits64(position.white_knights);

        black_bishops = count_bits64(position.black_bishops);
        black_knights = count_bits64(position.black_knights);


        // this CAN be rearanged with ifs, cutting off conflicting cases but having SOME readabiliy is prefered
        low_material = !(white_knights | white_bishops | black_knights | black_bishops) || // quick kings only check
                       (white_bishops >= 2 || black_bishops >= 2) || // a mate is possible with 2 bishops
                       (white_knights >= 3 || black_knights >= 3) || // this... is mate. but why? why would you do this to yourself?
                         ( // seperated for easier order changes ;)
                             ( white_knights + white_bishops == 1     && !(black_knights | black_bishops)) ||    // white has only bishop or knight
                             ( !(white_knights | white_bishops)       && black_knights + black_bishops == 1) ||  // black has only bishop or knight
                             ( white_knights + white_bishops == 1     && black_knights + black_bishops == 1) ||  // both have bishop or knight
                             ( (white_knights == 2 && !white_bishops) && !(black_knights | black_bishops)) ||    // white has only 2 knights
                             ( !(white_knights | white_bishops)       && (black_knights == 2 && !black_bishops)) // black has only 2 knights
                         );
    }

    return low_material;
}

bool is_draw(GameState& current_state) {
    GameState& original_state = current_state;
    bool is_draw = false;
    int repetion_count = 0;

    // this whole thing feels somewhat combersome
    is_draw = (current_state.draw_timer == DRAW_MOVES) || // check if draw timer is up
              insufficient_material(original_state.position); // or the current position is a legal draw

    while (current_state.previous_state != nullptr && !is_draw) {

        current_state = *current_state.previous_state;

        if (original_state.position == current_state.position) {
            repetion_count++; // yes this can be compressed to one line
            is_draw = (repetion_count == DRAW_REPETITIONS);
        }
    }

    return is_draw;
}

int material_eval(BoardPosition position) {
    int white_queens, white_rooks, white_bishops, white_knights, white_pawns;
    int black_queens, black_rooks, black_bishops, black_knights, black_pawns;

    white_queens  = count_bits64(position.white_queens);
    white_rooks   = count_bits64(position.white_rooks);
    white_bishops = count_bits64(position.white_bishops);
    white_knights = count_bits64(position.white_knights);
    white_pawns   = count_bits64(position.white_pawns);

    black_queens  = count_bits64(position.black_queens);
    black_rooks   = count_bits64(position.black_rooks);
    black_bishops = count_bits64(position.black_bishops);
    black_knights = count_bits64(position.black_knights);
    black_pawns   = count_bits64(position.black_pawns);

    return (white_queens  - black_queens) * QUEEN_VALUE +
           (white_rooks   - black_rooks) * ROOK_VALUE +
           (white_bishops - black_bishops) * BISHOP_VALUE +
           (white_knights - black_knights) * KNIGHT_VALUE +
           (white_pawns   - black_pawns) * PAWN_VALUE;
}