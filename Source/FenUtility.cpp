#include "FenUtility.h"
#include "Bitboard.h"
#include <sstream>
#include <vector>

Position FenUtility::fromFen(const std::string& fen) {
  Position pos;

  std::stringstream ss(fen);
  std::string segment;

  // Piece Placement
  if (std::getline(ss, segment, ' ')) {
    int rank = BOARD_SIZE - 1;
    int file = 0;

    for (char c : segment) {
      if (c == '/') {
        rank--;
        file = 0;
      } else if (isdigit(c)) {
        file += c - '0';
      } else {
        BoardIndex index = rank * BOARD_SIZE + file;

        PieceType type = charToPiece(c);
        if (type != PieceType::NONE) {
            Color color = isupper(c) ? Color::WHITE : Color::BLACK;
            pos.getPieceBoardRef(color, type).setBit(index);
        }
        file++;
      }
    }
  }

  // Active Color
  if (std::getline(ss, segment, ' ')) {
      pos.setCurrentColor((segment == "w") ? Color::WHITE : Color::BLACK);
  } else {
      pos.setCurrentColor(Color::WHITE); // Default
  }

  // Castling Rights
  if (std::getline(ss, segment, ' ')) {
    if (segment != "-") {

      for (char c : segment) {
        switch (c) {
        case 'K':
            pos.getSpecialMoveRightsRef().setBit(E1_index);
            pos.getSpecialMoveRightsRef().setBit(H1_index);
          break;
        case 'Q':
            pos.getSpecialMoveRightsRef().setBit(E1_index);
            pos.getSpecialMoveRightsRef().setBit(A1_index);
          break;
        case 'k':
            pos.getSpecialMoveRightsRef().setBit(E8_index);
            pos.getSpecialMoveRightsRef().setBit(H8_index);
          break;
        case 'q':
            pos.getSpecialMoveRightsRef().setBit(E8_index);
            pos.getSpecialMoveRightsRef().setBit(A8_index);
          break;
        }
      }
    }
  }

  // En Passant Target
  if (std::getline(ss, segment, ' ')) {
    if (segment != "-") {
      // segment is like "e3"
      int file = segment[0] - 'a';
      int rank = segment[1] - '1';
      BoardIndex ep_index = rank * BOARD_SIZE + file;

      pos.getSpecialMoveRightsRef().setBit(ep_index);
    }
  }

  // Populate the incremental color_pieces cache from the individual piece boards
  pos.updateCachedPieces();

  return pos;
}

std::string FenUtility::toFen(const Position& pos) {
  std::stringstream ss;

  // 1. Piece Placement
  for (int rank = BOARD_SIZE - 1; rank >= 0; --rank) {
    int empty_count = 0;
    for (int file = 0; file < BOARD_SIZE; ++file) {
      BoardIndex index = rank * BOARD_SIZE + file;
      Piece piece = pos.getPieceAtIndex(index);

      if (piece.type == PieceType::NONE) {
        empty_count++;
      } else {
        if (empty_count > 0) {
          ss << empty_count;
          empty_count = 0;
        }

        char p = pieceToChar(piece.type);
        if (piece.color == Color::WHITE) {
          p = toupper(p);
        }
        ss << p;
      }
    }
    if (empty_count > 0) {
      ss << empty_count;
    }
    if (rank > 0) {
      ss << '/';
    }
  }

  // 2. Active Color
  ss << " " << (pos.getCurrentColor() == Color::WHITE ? "w" : "b");

  // 3. Castling Rights
  ss << " ";
  bool castling_available = false;
  Bitboard rights = pos.getSpecialMoveRights();
  
  if (rights.getBit(E1_index) && rights.getBit(H1_index)) {
      ss << "K"; castling_available = true;
  }
  if (rights.getBit(E1_index) && rights.getBit(A1_index)) {
      ss << "Q"; castling_available = true;
  }
  if (rights.getBit(E8_index) && rights.getBit(H8_index)) {
      ss << "k"; castling_available = true;
  }
  if (rights.getBit(E8_index) && rights.getBit(A8_index)) {
      ss << "q"; castling_available = true;
  }
  if (!castling_available) {
    ss << "-";
  }

  // 4. En Passant Target
  ss << " ";
  Bitboard ep_board = rights.getCommonBitsWith(Bitboard(ALL_EN_PASSANT));
  if (ep_board.hasRemainingBits()) {
    BoardIndex ep_index = ep_board.singleBitIndex();
    int file = ep_index % BOARD_SIZE;
    int rank = ep_index / BOARD_SIZE;
    char file_char = 'a' + file;
    char rank_char = '1' + rank;
    ss << file_char << rank_char;
  } else {
    ss << "-";
  }

  // 5. Halfmove Clock & 6. Fullmove Number (dummies)
  ss << " 0 1";

  return ss.str();
}
