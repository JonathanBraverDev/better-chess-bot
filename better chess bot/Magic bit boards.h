#pragma once

#include "Board structure.h"
#include "Board generation.h"

// Yes, this IS happening.
//
// After thinking about it for a while, 
// a direct hash to index is by far the least computantiaonally expensive solution i can come up with
// it is fast but memory expensive, with the ammount i use currently it souldnt be an issue at all, however.
// anythign that makes a better use of the memory would cost more computation time which i'm not going to allow.
