#pragma once

class Piece;

template<>
struct std::hash<Vector2> {
    std::size_t operator ()(const Vector2& v) const noexcept {
        return static_cast<int>(v.x) * 73856093 ^ static_cast<int>(v.y) * 19349663;
    }
};

class Board final {
public:
    static void SetState(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    static std::unordered_map<Vector2, Piece>& GetBoard();
private:
    static bool FEN_IsEmptySpace(const char& c, int& space);
    static bool FEN_IsPiece(const char& c, Piece& piece);
    inline static std::unordered_map<Vector2, Piece> m_BoardData;
};