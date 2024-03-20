#pragma once

class Piece;

/// <summary>
/// Hash specialization for Vector2 for use in unordered_map
/// </summary>
template<>
struct std::hash<Vector2> {
    std::size_t operator ()(const Vector2& v) const noexcept {
        return static_cast<int>(v.x) * 73856093 ^ static_cast<int>(v.y) * 19349663;
    }
};

using BoardMap = std::unordered_map<Vector2, Piece>;

/// <summary>
/// Describes the chess board
/// </summary>
class Board final {
public:

    /// <summary>
    /// Sets the board state to the specified state using FEN notation.
    /// Wipes the board before the operation
    /// </summary>
    /// <param name="fen"><c>string</c> The FEN string to initialize the board with</param>
    static void SetState(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    /// <summary>
    /// Gets the underlying board data
    /// </summary>
    static BoardMap& GetBoard();

private:

    /// <summary>
    /// Checks if the specified char is empty space in FEN notation and initializes the amount
    /// </summary>
    /// <param name="c"><c>char</c> The character to check</param>
    /// <param name="space"><c>int</c> The amount of space if the character was space</param>
    /// <returns><c>true</c> If the character was space</returns>
    /// <example>
    /// <code>
    /// char c = '5';
    /// if(int space = 0; FEN_IsEmptySpace(c, space)) {
    ///     // char c was 5 space -> space == 5
    /// }
    /// </code>
    /// </example>
    static bool FEN_IsEmptySpace(const char& c, int& space);

    /// <summary>
    /// Checks if the specified char is a piece in FEN notation and initializes the piece
    /// </summary>
    /// <param name="c"><c>char</c> The character to check</param>
    /// <param name="piece"><c>Piece</c> The piece if the character was a piece</param>
    /// <returns><c>true</c> If the character was a piece</returns>
    /// <example>
    /// <code>
    /// char c = 'p';
    /// if(Piece p(PieceFlags::Rook | PieceFlags::White); FEN_IsPiece(c, p)) {
    ///     // char c was a black pawn piece -> p == Piece(PieceFlags::Pawn | PieceFlags::Black)
    /// }
    /// </code>
    /// </example>
    static bool FEN_IsPiece(const char& c, Piece& piece);

    /// <summary>
    /// The board data containing pieces indexed by positions on the board
    /// </summary>
    inline static BoardMap m_BoardData;
};