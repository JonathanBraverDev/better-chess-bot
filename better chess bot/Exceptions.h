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
