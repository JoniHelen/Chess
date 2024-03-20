#include <pch.hpp>
#include <Board.hpp>
#include <Piece.hpp>

void Board::SetState(const std::string &fen) {
    m_BoardData.clear();

    Vector2 currentSquare(0, 7); // Start from the top left square as per FEN specification

    // Iterate the characters
    for(const auto& c : fen) {
        if(int space = 0; FEN_IsEmptySpace(c, space)) { // Process empty space
            currentSquare += Vector2::UnitX * static_cast<float>(space);
        }
        else if(Piece p(PieceFlag::Pawn); FEN_IsPiece(c, p)) { // Process pieces
            p.SetPosition(currentSquare);
            m_BoardData.insert({ currentSquare, p });
            currentSquare += Vector2::UnitX;
        }
        else if(c == '/') { // New rank
            currentSquare -= Vector2::UnitY;
            currentSquare.x = 0;
        }
        else if(c == ' ') {
            break; // end of piece placement
        }

        // TODO: Process game state
    }
}

bool Board::FEN_IsEmptySpace(const char &c, int &space) {
    space = c - '0'; // Convert char to int
    return c > '0' && c < '9'; // Space can be in range [1, 8];
}

bool Board::FEN_IsPiece(const char &c, Piece &piece) {
    // Convert character into a piece
    switch(c) {
        case 'r':
            piece = Piece(PieceFlag::Rook | PieceFlag::Black);
            return true;

        case 'n':
            piece = Piece(PieceFlag::Knight | PieceFlag::Black);
            return true;

        case 'b':
            piece = Piece(PieceFlag::Bishop | PieceFlag::Black);
            return true;

        case 'q':
            piece = Piece(PieceFlag::Queen | PieceFlag::Black);
            return true;

        case 'k':
            piece = Piece(PieceFlag::King | PieceFlag::Black);
            return true;

        case 'p':
            piece = Piece(PieceFlag::Pawn | PieceFlag::Black);
            return true;

        case 'R':
            piece = Piece(PieceFlag::Rook | PieceFlag::White);
            return true;

        case 'N':
            piece = Piece(PieceFlag::Knight | PieceFlag::White);
            return true;

        case 'B':
            piece = Piece(PieceFlag::Bishop | PieceFlag::White);
            return true;

        case 'Q':
            piece = Piece(PieceFlag::Queen | PieceFlag::White);
            return true;

        case 'K':
            piece = Piece(PieceFlag::King | PieceFlag::White);
            return true;

        case 'P':
            piece = Piece(PieceFlag::Pawn | PieceFlag::White);
            return true;

        default:
            return false;
    }
}

BoardMap& Board::GetBoard() {
    return m_BoardData;
}
