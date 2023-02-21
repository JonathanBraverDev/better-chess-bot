
#include "Evaluation.h"
#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "MoveProvider.h"

/*
idea dump :

open king - each attack 'lane' (row,col,diagonal) without cover is + for the other side
bishop supiriority - bonus for having both and discoraging 'equal' trades vs knights (done, given slightly higher value)
penatly for stacked pawns - multiple pawns on the same columb = bad bad
bonus for high queen activity near the enemy king / side of the board
bonus for attacking more times than a square/piece is defended
bonus for attackign tiles near the enemy king
bonus for captures
bonus to oponents piece weight to encorage 'fair' trades when having material advantage, bonus increasing mildly as advantage/total piece value goes up
bonus for spicy moves like pins, forks (the cheaper the forking piece in relation to the forked the better) and revealed attacks
bonus for total area behind pawns
bonus for passed pawns, especially ones with a promotion tile off color to the enemy bishop(s)
bonus for outposts (pieces coverd by pawns that can't be kicked out by enemy pawns)
bonus for total area avalible for pieces to move into and/or standing on center tiles (chould be dangerous in endgames, redundant with good moves?)
bonus for rooks on open/semi open columns
bonus for knights in closed positions, bonus for bishops in open positions (again, bishiop supiriority)
bonus/penalty for bishop based on pawn color (white bishop bonus if 12 pawns are on dark somehow)

ideas:
castling, accounting for side and current adtantage (same in equal, opposite with initiative)
killing off long range pieces
long range attacks
denying infiltration squares with pawn for knights in closed position
any move with a pawn
any move with a knight
any move with a bishop
any more with a rook
any move with a queen
any move with the king (meme)


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
    is_draw = (current_state.draw_timer >= DRAW_MOVE_LIMIT) || // check if draw timer is up
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

bool is_check(BoardPosition position, PlayerColor attacker_color) {
    const bool is_white = attacker_color == WHITE;
    const B64 attacked_king = (is_white ? position.black_king : position.white_king);
    const int tile = lowestBitIndex64_s(attacked_king);
    B64 slide_attackes;

    bool check = false;

    // perform fastest bit check first
    if ((knight_moves[tile] & (is_white ? position.white_knights : position.black_knights)) ||
        (pawn_attacks[tile] & (is_white ? position.white_pawns : position.black_pawns))) {

        check = true;

    } else {
        // can be futher split into multiple checks but this is WAY more readable
        slide_attackes = generate_queen_moves(position.white | position.black, attacked_king);

        check = ((slide_attackes & (is_white ? position.white_queens : position.black_queens)) ||
                 (slide_attackes & (is_white ? position.white_rooks : position.black_rooks)) ||
                 (slide_attackes & (is_white ? position.white_bishops : position.black_bishops)));
    }
    
    return check;
}

B64 attacking_pieces(const BoardPosition position, const B64 target_board, const PlayerColor attacker_color) {
    const bool is_white = attacker_color == WHITE;
    const int tile = lowestBitIndex64_s(target_board);
    const B64 slide_attackes = generate_queen_moves(position.white | position.black, target_board);

    return (knight_moves[tile] & (is_white ? position.white_knights : position.black_knights)) |
           (pawn_attacks[tile] & (is_white ? position.white_pawns : position.black_pawns)) |
           (king_moves[tile] & (is_white ? position.white_king : position.black_king)) | // irrelevent for checks but quick and generic
           (slide_attackes & (is_white ? position.white_queens : position.black_queens)) |
           (slide_attackes & (is_white ? position.white_rooks : position.black_rooks)) |
           (slide_attackes & (is_white ? position.white_bishops : position.black_bishops));
}

bool can_king_run(BoardPosition position, PlayerColor attacker_color, B64 attacked_king) {
    const bool is_attacker_white = attacker_color == WHITE;
    B64 possible_king_moves = king_moves[lowestBitIndex64_s(attacked_king)] & ~(is_attacker_white ? position.black : position.white);
    B64 curr_move;
    bool can_run = false;

    // check all moves
    while (possible_king_moves) {
        curr_move = lowestBitBoard(possible_king_moves);

        // update the king position
        (is_attacker_white ? position.black_king = curr_move
            : position.white_king = curr_move);

        if (is_check(position, attacker_color)) {
            can_run = true;
            break;
        }

        possible_king_moves ^= curr_move; // delete the move
    }
    
    return can_run;
}

// !!! make sure this makes sence !!!
B64 get_connecting_tiles(const B64 sliding_moves, const B64 start, const B64 end) {

    // Compute the direction and distance between 'start' and 'end'
    const B64 direction = start ^ end;
    
    // Remove file boundaries
    B64 line = (direction & 0x7E7E7E7E7E7E7E00ULL) >> 1;
    
    // Compute the mask for the line between 'start' and 'end'
    line |= line >> 1;
    line |= line << 1;
    line &= sliding_moves;

    return line;
}

bool is_checkmate(BoardPosition position, PlayerColor attacker_color) {
    const bool is_attacker_white = attacker_color == WHITE;
    const B64 attacked_king = (is_attacker_white ? position.black_king : position.white_king);
    const B64 attackers = attacking_pieces(position, attacked_king, attacker_color);

    bool checkmate = false;

    if (attackers) {
        if (count_bits64(attacked_king) > 1) {
            checkmate = can_king_run(position, attacker_color, attacked_king);
        }
        // need the following functions:
        // kill target at tile X
        // find attack direction
        // move to any one of X tiles on a borad
        // both can use a very similar logic, with the pawn attack/move being toggles
    }

    return checkmate;
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

    return (white_queens  - black_queens)  * QUEEN_VALUE  +
           (white_rooks   - black_rooks)   * ROOK_VALUE   +
           (white_bishops - black_bishops) * BISHOP_VALUE +
           (white_knights - black_knights) * KNIGHT_VALUE +
           (white_pawns   - black_pawns)   * PAWN_VALUE;
}

int count_sliding_attacks(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 target_board, const B64 blockers) {
    return count_attacks(attacking_pieces, target_board, move_generator, nullptr, blockers);
}

int count_jumping_attacks(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale, const int first_index) {
    return count_attacks(attacking_pieces, target_board, nullptr, move_source, 0, index_scale, first_index);
}

int count_attacks(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64), const B64* move_source, const B64 blockers, const int index_scale, const int first_index) {
    std::vector<B64> potential_attackers;
    B64 attack_board;
    int attacks = 0;

    seperate_bits(attacking_pieces, potential_attackers);

    while (!potential_attackers.empty()) {
        if (move_generator) {
            attack_board = move_generator(blockers, potential_attackers.back());
        } else {
            attack_board = move_source[first_index + index_scale * lowestBitIndex64(potential_attackers.back())];
        }

        if (attack_board && target_board) {
            attacks++;
        }
        potential_attackers.pop_back();
    }

    return attacks;
}

int count_white_attacks( const BoardPosition position, const B64 target_board) {

    const B64 blockers = position.white | position.black;

    return count_sliding_attacks(generate_queen_moves, position.white_queens, target_board, blockers) +
           count_sliding_attacks(generate_rook_moves, position.white_rooks, target_board, blockers) +
           count_sliding_attacks(generate_bishop_moves, position.white_bishops, target_board, blockers) + 
           count_jumping_attacks(king_moves, position.white_king, target_board) +
           count_jumping_attacks(knight_moves, position.white_knights, target_board) +
           count_jumping_attacks(pawn_attacks, position.white_pawns, target_board, 2); // pawns have 2 moves per tile
}

int count_black_attacks(const BoardPosition position, const B64 target_board) {

    const B64 blockers = position.white | position.black;

    return count_sliding_attacks(generate_queen_moves, position.black_queens, target_board, blockers) +
           count_sliding_attacks(generate_rook_moves, position.black_rooks, target_board, blockers) +
           count_sliding_attacks(generate_bishop_moves, position.black_bishops, target_board, blockers) +
           count_jumping_attacks(king_moves, position.black_king, target_board) +
           count_jumping_attacks(knight_moves, position.black_knights, target_board) +
           count_jumping_attacks(pawn_attacks, position.black_pawns, target_board, 2, 1); // offset to blacks moves
}