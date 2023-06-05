#pragma once

// Yes, this IS happening.
//
// I refuse to blindly copy someone else's work, especially because I have my own idea.
// What if, instead of using the full occupancy to hash the index of the moves, I get an EFFECTIVE occupancy hash first?
//
// 'Effective occupancy' are the tiles that ACTUALLY block the piece, so a queen behind a pawn will get YEETED out by the magic number.
// Then I'll have another number that takes the bits in question to be the index.
// That would essentially split the hashing into multiple parts but will possibly allow me to use it in a smarter way.
//
// I think that the Effective occupancy would be relatively simple to verify.
// It might allow me to then look for non-conflicting parts for the next stage of the hash.
// The first gets rid of the noise (all the false occupancy), and the next gives a clean result.
// I have no idea, we'll see.