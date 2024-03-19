#include <pch.hpp>
#include <Board.hpp>
#include <Piece.hpp>

void Board::SetState(const std::string &fen) {
    m_BoardData.clear();

    Vector2 currentSquare(0, 7);

    for(const auto& c : fen) {
        if(int space = 0; FEN_IsEmptySpace(c, space)) {
            currentSquare += Vector2::UnitX * static_cast<float>(space);
        }
        else if(Piece p(Piece::Type::Pawn); FEN_IsPiece(c, p)) {
            p.SetPosition(currentSquare);
            m_BoardData.insert({ currentSquare, p });
            currentSquare += Vector2::UnitX;
        }
        else if(c == '/') {
            currentSquare -= Vector2::UnitY;
            currentSquare.x = 0;
        }
        else if(c == ' ') {
            break; // end of piece placement
        }
    }
}

bool Board::FEN_IsEmptySpace(const char &c, int &space) {
    space = c - '0';
    return c > '0' && c < '9';
}

bool Board::FEN_IsPiece(const char &c, Piece &piece) {
    switch(c) {
        case 'r':
            piece = Piece(Piece::Type::Rook | Piece::Type::Black);
            return true;

        case 'n':
            piece = Piece(Piece::Type::Knight | Piece::Type::Black);
            return true;

        case 'b':
            piece = Piece(Piece::Type::Bishop | Piece::Type::Black);
            return true;

        case 'q':
            piece = Piece(Piece::Type::Queen | Piece::Type::Black);
            return true;

        case 'k':
            piece = Piece(Piece::Type::King | Piece::Type::Black);
            return true;

        case 'p':
            piece = Piece(Piece::Type::Pawn | Piece::Type::Black);
            return true;

        case 'R':
            piece = Piece(Piece::Type::Rook | Piece::Type::White);
            return true;

        case 'N':
            piece = Piece(Piece::Type::Knight | Piece::Type::White);
            return true;

        case 'B':
            piece = Piece(Piece::Type::Bishop | Piece::Type::White);
            return true;

        case 'Q':
            piece = Piece(Piece::Type::Queen | Piece::Type::White);
            return true;

        case 'K':
            piece = Piece(Piece::Type::King | Piece::Type::White);
            return true;

        case 'P':
            piece = Piece(Piece::Type::Pawn | Piece::Type::White);
            return true;

        default:
            return false;
    }
}

std::unordered_map<Vector2, Piece>& Board::GetBoard() {
    return m_BoardData;
}
