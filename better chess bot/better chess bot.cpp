
#include <chrono>

#include "Board structure.h"
#include "Board operations.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Move search.h"
#include "Magic bit boards.h"

int main() {

    prepare_king_moves();
    prepare_knight_moves();
    prepare_pawn_moves();

    PositionScore position_score;
    GameState game = INITIAL_GAMESTATE;

    //position_score = alphabeta_init(game, 5);

    visualize_position(game.sided_position, true);
    visualize_position(generate_next_state(game, INITIAL_POSITION).sided_position, true);

    //while (game.turn <= 100) {
    //    auto start = std::chrono::high_resolution_clock::now();

    //    position_score = alphabeta_init(game, 5);

    //    auto end = std::chrono::high_resolution_clock::now();
    //    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    //    std::cout << "eval:" << position_score.current_score << std::endl;
    //    std::cout << "microseconds:" << (double)microseconds << std::endl;
    //    std::cout << "positions evaluated:" << evals << std::endl;
    //    std::cout << "parent nodes visited:" << nodes - evals << std::endl;
    //    std::cout << "cutoffs performed:" << cutoffs << std::endl;

    //    visualize_position(position_score.next_position);
    //    game.turn++;
    //    game.sided_position = position_score.next_position;
    //}



    //GameState game = INITIAL_GAMESTATE;
    //int initialRow, initialColumn, destRow, destColumn;
    //B64 initial, dest;
    //bool piece_found;

    //while (true) {
    //    visualize_position(game.sided_position);

    //    piece_found = false;
    //    while (!piece_found) {
    //        std::cout << "Enter the initial row: ";
    //        std::cin >> initialRow;
    //        std::cout << "Enter the initial column: ";
    //        std::cin >> initialColumn;

    //        initial = bit_board_from_index(initialRow * 8 + initialColumn);

    //        std::cout << "Enter the destination row: ";
    //        std::cin >> destRow;
    //        std::cout << "Enter the destination column: ";
    //        std::cin >> destColumn;

    //        dest = bit_board_from_index(destRow * 8 + destColumn);

    //        if (game.sided_position.own_pawns & initial) {
    //            game.sided_position.own_pawns ^= (initial | dest);
    //            piece_found = true;
    //        }
    //        else if (game.sided_position.own_knights & initial) {
    //            game.sided_position.own_knights ^= (initial | dest);
    //            piece_found = true;
    //        }
    //        else if (game.sided_position.own_bishops & initial) {
    //            game.sided_position.own_bishops ^= (initial | dest);
    //            piece_found = true;
    //        }
    //        else if (game.sided_position.own_rooks & initial) {
    //            game.sided_position.own_rooks ^= (initial | dest);
    //            piece_found = true;
    //        }
    //        else if (game.sided_position.own_queens & initial) {
    //            game.sided_position.own_queens ^= (initial | dest);
    //            piece_found = true;
    //        }
    //        else if (game.sided_position.own_king & initial) {
    //            game.sided_position.own_king ^= (initial | dest);
    //            piece_found = true;
    //        }

    //        clear_bits(game.sided_position.opponent_rooks, dest);
    //        clear_bits(game.sided_position.opponent_king, dest);
    //        clear_bits(game.sided_position.opponent_knights, dest);
    //        clear_bits(game.sided_position.opponent_pawns, dest);
    //        clear_bits(game.sided_position.opponent_queens, dest);
    //        clear_bits(game.sided_position.opponent_bishops, dest);
    //    }

    //    visualize_position(game.sided_position);

    //    switch_sides(game.sided_position);
    //    position_score = alphabeta_init(game, 5);

    //    game.sided_position = position_score.next_position;
    //    switch_sides(game.sided_position);
    //}
}
