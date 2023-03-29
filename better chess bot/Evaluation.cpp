
#include "Evaluation.h"

/*
make everything as dynamic and generic as possible to allow for many, MANY playstyles for the bot

forced draw with insuffishent material, i'm leaning towards USCF over FIDE rules (chess.com style)

allow draw offer and make the bot accept if the accuracy and advantage of the player so far makes him likly to win, but also allow mercy draws sometimes ;)
*/

// low material rulings according to chess.com
inline bool insufficient_material(SidedPosition sided_position) {

    bool low_material = false;

    int own_bishops, own_knights;
    int opponent_bishops, opponent_knights;

    // potential return guard refactor here
    if ( !(sided_position.own_pawns || sided_position.opponent_pawns || // where there's a pawn theres (sometimes) a way
           sided_position.own_rooks || sided_position.opponent_rooks || // immidiatly stop cases with rooks and queens
           sided_position.own_queens || sided_position.opponent_queens)) { // rook endgames are more likly than queen endgames
        
        // saving counts to avoit countign each time, other wise i'd just do a MASSIVE return
        own_bishops = count_bits64(sided_position.own_bishops);
        own_knights = count_bits64(sided_position.own_knights);

        opponent_bishops = count_bits64(sided_position.opponent_bishops);
        opponent_knights = count_bits64(sided_position.opponent_knights);


        // this CAN be rearanged with ifs, cutting off conflicting cases but having SOME readabiliy is prefered
        low_material = !(own_knights | own_bishops | opponent_knights | opponent_bishops) || // quick kings only check
                       (own_bishops >= 2 || opponent_bishops >= 2) || // a mate is possible with 2 bishops
                       (own_knights >= 3 || opponent_knights >= 3) || // this... is mate. but why? why would you do this to yourself?
                         ( // seperated for easier order changes ;)
                             ( own_knights + own_bishops == 1     && !(opponent_knights | opponent_bishops)) ||    // white has only bishop or knight
                             ( !(own_knights | own_bishops)       && opponent_knights + opponent_bishops == 1) ||  // black has only bishop or knight
                             ( own_knights + own_bishops == 1     && opponent_knights + opponent_bishops == 1) ||  // both have bishop or knight
                             ( (own_knights == 2 && !own_bishops) && !(opponent_knights | opponent_bishops)) ||    // white has only 2 knights
                             ( !(own_knights | own_bishops)       && (opponent_knights == 2 && !opponent_bishops)) // black has only 2 knights
                         );
    }

    return low_material;
}

bool is_draw(GameState& current_state) {
    GameState& original_state = current_state;
    bool is_draw = false;
    int repetion_count = 0;

    // this whole thing feels somewhat combersome
    is_draw = (current_state.draw_timer >= DRAW_MOVE_LIMIT) || // check if draw timer is up
              insufficient_material(original_state.sided_position); // or the current position is a legal draw

    while (current_state.previous_state != nullptr && !is_draw) {

        current_state = *current_state.previous_state;

        if (original_state.sided_position == current_state.sided_position) {
            repetion_count++; // yes this can be compressed to one line
            is_draw = (repetion_count == DRAW_REPETITIONS);
        }
    }

    return is_draw;
}

bool can_king_run(SidedPosition sided_position) {
    B64 possible_king_moves = king_moves[lowest_single_bit_index(sided_position.own_king)] & ~own_pieces(sided_position);
    B64 curr_move;
    bool can_run = false;

    // check all moves
    while (possible_king_moves) {
        curr_move = lowest_bit_board(possible_king_moves);

        // update the king position
        sided_position.own_king = curr_move;

        if (!is_check(sided_position)) {
            can_run = true;
            break;
        }

        possible_king_moves ^= curr_move; // delete the move
    }

    return can_run;
}

// checkmate shouldn't be reached during evaluation
bool is_checkmate(SidedPosition sided_position) {
    const B64 attackers = attacking_pieces(sided_position, sided_position.own_king);

    B64 responce_attempts = 0;
    bool checkmate = false;
    B64 attack_path;

    // write a seprate function that returns only unchecked positions

    // if the king is attacked and can't save himself, the check is shared to all cases so is evaluated first
    if (attackers && !can_king_run(sided_position)) {

        // moving out of check is the only option with multiple threats, so... he dead, he real, real dead
        if (count_bits64(sided_position.own_king) > 1) {
            checkmate = true;
        } else {

            // a knight's check cannot be blocked, but a single knight can be killed
            if (attackers & sided_position.opponent_knights) {

                // needed function:
                // positions with any kill of target at single til

            } else { // the attacker is a sliding piece, block it or kill it
                attack_path = get_connecting_tiles(attackers, sided_position.own_king);

                // needed functions:
                // positions with any move to one of X tiles on a borad
                // positions with any kill of target at single tile
            }
        }
    }

    return checkmate;
}

int material_eval(SidedPosition sided_position) {
    int own_queens, own_rooks, own_bishops, own_knights, own_pawns;
    int opponent_queens, opponent_rooks, opponent_bishops, opponent_knights, opponent_pawns;

    own_queens  = count_bits64(sided_position.own_queens);
    own_rooks   = count_bits64(sided_position.own_rooks);
    own_bishops = count_bits64(sided_position.own_bishops);
    own_knights = count_bits64(sided_position.own_knights);
    own_pawns   = count_bits64(sided_position.own_pawns);

    opponent_queens  = count_bits64(sided_position.opponent_queens);
    opponent_rooks   = count_bits64(sided_position.opponent_rooks);
    opponent_bishops = count_bits64(sided_position.opponent_bishops);
    opponent_knights = count_bits64(sided_position.opponent_knights);
    opponent_pawns   = count_bits64(sided_position.opponent_pawns);

    return (own_queens  - opponent_queens)  * QUEEN_VALUE  +
           (own_rooks   - opponent_rooks)   * ROOK_VALUE   +
           (own_bishops - opponent_bishops) * BISHOP_VALUE +
           (own_knights - opponent_knights) * KNIGHT_VALUE +
           (own_pawns   - opponent_pawns)   * PAWN_VALUE;
}