
#include "Board movment.h"

// the true heavylifter, all collision checks happen here
void check_proposed_slide(B64& proposed, B64& same_color, B64& diff_color, B64& moves, bool& stop) {
    if (proposed & same_color) { // i consider it more likly that allied pieces would block move
        stop = true;
    }
    else if (proposed & diff_color) {
        moves |= proposed;
        stop = true;
    }
    else {
        moves |= proposed;
    }
}

void slide(B64 (*direction)(B64), B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    B64 proposed; 
    bool stop = false; 

    do {
        proposed = direction(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop); 
        piece = proposed; 
    } while (!stop && piece);
}

// functions below are just for ease of use

void slide_up(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&up, same_color, diff_color, moves, piece);
}


void slide_down(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&down, same_color, diff_color, moves, piece);
}


void slide_left(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&left, same_color, diff_color, moves, piece);
}


void slide_right(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&right, same_color, diff_color, moves, piece);
}


void slide_up_left(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&up_left, same_color, diff_color, moves, piece);
}


void slide_up_right(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&up_right, same_color, diff_color, moves, piece);
}


void slide_down_left(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&down_left, same_color, diff_color, moves, piece);
}


void slide_down_right(B64& same_color, B64& diff_color, B64& moves, B64 piece) {
    slide(&down_right, same_color, diff_color, moves, piece);
}