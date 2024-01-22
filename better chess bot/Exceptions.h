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
