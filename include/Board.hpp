#pragma once

class Move;
class Piece;

#include <Position.hpp>

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
    static auto SetState(std::string_view fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") -> void;

    /// <summary>
    /// Gets the underlying board data
    /// </summary>
    static auto GetBoard() -> std::unordered_map<Position, Piece>&;

    static auto MakeMove(const Move& move) -> void;

    static auto CalculateLegalMoves(const Position& pos, const Piece& piece, std::vector<Move>& moves) -> void;

    static auto CalculatePawnAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculatePawnMoves(Position position, std::vector<Move>& moves) -> void;

    static auto CalculateRookAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculateRookMoves(Position position, std::vector<Move>& moves) -> void;

    static auto CalculateBishopAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculateBishopMoves(Position position, std::vector<Move>& moves) -> void;

    static auto CalculateKnightAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculateKnightMoves(Position position, std::vector<Move>& moves) -> void;

    static auto CalculateQueenAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculateQueenMoves(Position position, std::vector<Move>& moves) -> void;

	static auto CalculateKingAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void;
    static auto CalculateKingMoves(Position position, std::vector<Move>& moves) -> void;

    static auto UpdateCheckState() -> void;

    static auto GetKingPosition(bool white) -> Position;

    static auto CheckForPins(const Position& ignorePos, std::vector<Position>& freeSquares) -> bool;

    static auto GetSquaresBetween(const Position& start, const Position& end, std::vector<Position>& squares) -> void;

private:

    struct CheckStateData {
	    bool IsInCheck;
		std::vector<Position> Threats;
        std::vector<Position> BlockingSquares;
    };

    /// <summary>
    /// Checks if the specified char is empty space in FEN notation and initializes the amount
    /// </summary>
    /// <param name="c"><c>char</c> The character to check</param>
    /// <param name="space"><c>int</c> The amount of space if the character was space</param>
    /// <returns><c>true</c> If the character was space</returns>
    /// <example>
    /// <code>
    /// char c = '5';
    /// if(int space = 0; IsEmptySpace(c, space)) {
    ///     // char c was 5 space -> space == 5
    /// }
    /// </code>
    /// </example>
    static auto IsEmptySpace(const char& c, int& space) -> bool;

    /// <summary>
    /// Checks if the specified char is a piece in FEN notation and initializes the piece
    /// </summary>
    /// <param name="c"><c>char</c> The character to check</param>
    /// <param name="piece"><c>Piece</c> The piece if the character was a piece</param>
    /// <returns><c>true</c> If the character was a piece</returns>
    /// <example>
    /// <code>
    /// char c = 'p';
    /// if(Piece p(PieceFlags::Rook | PieceFlags::White); IsPiece(c, p)) {
    ///     // char c was a black pawn piece -> p == Piece(PieceFlags::Pawn | PieceFlags::Black)
    /// }
    /// </code>
    /// </example>
    static auto IsPiece(const char& c, Piece& piece) -> bool;

    inline static CheckStateData s_CheckState;

    inline static bool s_WhiteToMove;
    inline static bool s_EnPassantAvailable;
    inline static bool s_WhiteCanCastleKingSide;
    inline static bool s_WhiteCanCastleQueenSide;
    inline static bool s_BlackCanCastleKingSide;
    inline static bool s_BlackCanCastleQueenSide;
    inline static Position s_EnPassantPosition;

    /// <summary>
    /// The board data containing pieces indexed by positions on the board
    /// </summary>
    inline static std::unordered_map<Position, Piece> s_BoardData;
};