
#include "Board movment.h"

// the true heavylifter, all collision checks happen here
void check_proposed_slide(B64& proposed, B64& all_pieces, B64& moves, bool& stop) {
    if (proposed & all_pieces) {
        moves |= proposed;
        stop = true;
    } else {
        moves |= proposed;
    }
}

void slide(B64 (*direction)(B64), B64& all_pieces, B64& moves, B64 piece) {
    B64 proposed; 
    bool stop = false; 

    do {
        proposed = direction(piece); // running the passed function
        check_proposed_slide(proposed, all_pieces, moves, stop); 
        piece = proposed; 
    } while (!stop && piece);
}

// functions below are just for ease of use

void slide_up(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up, all_pieces, moves, piece);
}


void slide_down(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down, all_pieces, moves, piece);
}


void slide_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&left, all_pieces, moves, piece);
}


void slide_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&right, all_pieces, moves, piece);
}


void slide_up_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up_left, all_pieces, moves, piece);
}


void slide_up_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&up_right, all_pieces, moves, piece);
}


void slide_down_left(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down_left, all_pieces, moves, piece);
}


void slide_down_right(B64& all_pieces, B64& moves, B64 piece) {
    slide(&down_right, all_pieces, moves, piece);
}