#pragma once

/// <summary>
/// Describes a chess piece with flags for type and color
/// </summary>
enum class PieceFlag {
    Pawn = 1 << 0,
    Rook = 1 << 1,
    Knight = 1 << 2,
    Bishop = 1 << 3,
    King = 1 << 4,
    Queen = 1 << 5,
    White = 1 << 6,
    Black = 1 << 7
};

// Bitwise operator overloads

constexpr PieceFlag operator &(const PieceFlag& a, const PieceFlag& b) {
    return static_cast<PieceFlag>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr PieceFlag operator |(const PieceFlag& a, const PieceFlag& b) {
    return static_cast<PieceFlag>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr PieceFlag operator ^(const PieceFlag& a, const PieceFlag& b) {
    return static_cast<PieceFlag>(static_cast<int>(a) ^ static_cast<int>(b));
}

constexpr PieceFlag operator ~(const PieceFlag& a) {
    return static_cast<PieceFlag>(~static_cast<int>(a));
}

constexpr PieceFlag& operator &=(PieceFlag& a, const PieceFlag& b) {
    a = a & b;
    return a;
}

constexpr PieceFlag& operator |=(PieceFlag& a, const PieceFlag& b) {
    a = a | b;
    return a;
}

constexpr PieceFlag& operator ^=(PieceFlag& a, const PieceFlag& b) {
    a = a ^ b;
    return a;
}

/// <summary>
/// A Chess Piece object
/// </summary>
class Piece final {
public:
    //Ctors

    explicit Piece(const PieceFlag& type) noexcept;
    Piece(const PieceFlag& type, const Vector2& pos) noexcept;

    /// <summary>
    /// Sets the position if the piece in world space
    /// </summary>
    /// <param name="pos"><c>Vector2</c> The position to set the piece to</param>
    void SetPosition(const Vector2& pos) noexcept;

    void SetPickUpPosition(const Vector2& pos) noexcept; // TODO: Integrate into game logic

    /// <summary>
    /// Checks if the given point is inside the square that the piece occupies
    /// </summary>
    /// <param name="point"><c>Vector2</c> The point to check</param>
    /// <returns><c>true</c> if the point was inside the square</returns>
    [[nodiscard]] bool PointInside(const Vector2& point) const noexcept;

    /// <summary>
    /// Gets the transformation matrix of the piece
    /// </summary>
    [[nodiscard]] Matrix GetModelMatrix() const noexcept;

    /// <summary>
    /// Gets the <c>PieceFlag</c> property of the piece
    /// </summary>
    [[nodiscard]] PieceFlag GetType() const noexcept;

private:
    Matrix m_Transform;
    PieceFlag m_Type;
    Vector2 m_PickUpPosition;
};