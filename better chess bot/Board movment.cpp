
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

// there must be a way to write this better, only one thing is changing here
// suggested macro array
// in all honesty this is probably the FASTEST way computationally
void slide_up(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = up(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_down(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = down(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_left(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = left(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_right(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = right(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_up_left(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = up_left(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_up_right(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = up_right(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);

    
}


void slide_down_left(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = down_left(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}


void slide_down_right(B64& same_color, B64& diff_color, B64& moves, int origin) {
    B64 proposed;

    B64 piece = 0;
    set_bit(piece, origin);

    bool stop = false;
    do {
        proposed = down_right(piece);
        check_proposed_slide(proposed, same_color, diff_color, moves, stop);
    } while (!stop);
}