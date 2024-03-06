#pragma once

#include <exception>

class InvalidPieceTypeException : public std::exception {
    const char* what() const throw() {
        return "Invalid piece type";
    }
};

class InvalidColorException : public std::exception {
    const char* what() const throw() {
        return "Invalid color";
    }
};

class MultipleKingsException : public std::exception {
    const char* what() const throw() {
        return "Multiple kings found";
    }
};

class InvalidPieceCountException : public std::exception {
    const char* what() const throw() {
        return "Too many pieces found";
    }
};

class OverlappingPiecesException : public std::exception {
    const char* what() const throw() {
        return "Overlapping pieces found";
    }
};

class InvalidCastlingRights : public std::exception {
    const char* what() const throw() {
        return "Invalid castling rights found";
    }
};

class MultipleEnpasants : public std::exception {
    const char* what() const throw() {
        return "Multiple enpasants found";
    }
};

class InvalidPromotion : public std::exception {
    const char* what() const throw() {
        return "Inalid promotion found";
    }
};

class AttackerWithoutCapture : public std::exception {
    const char* what() const throw() {
        return "Attacker requested without capture";
    }
};

class PieceTypeCannonBeNone : public std::exception {
    const char* what() const throw() {
        return "Piece Type cannot be NONE";
    }
};

class KingCannonBeCaptured : public std::exception {
    const char* what() const throw() {
        return "King cannot be captured";
    }
};
