#pragma once
#include <Position.hpp>

/// <summary>
/// Describes a chess piece with flags for type and color
/// </summary>
enum class PieceFlag : uint8_t {
    None = 0U,
    Pawn = 1U << 0U,
    Rook = 1U << 1U,
    Knight = 1U << 2U,
    Bishop = 1U << 3U,
    King = 1U << 4U,
    Queen = 1U << 5U,
    White = 1U << 6U,
    Black = 1U << 7U
};

// Bitwise operator overloads

constexpr auto operator &(const PieceFlag& a, const PieceFlag& b) -> PieceFlag {
    return static_cast<PieceFlag>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

constexpr auto operator |(const PieceFlag& a, const PieceFlag& b) -> PieceFlag {
    return static_cast<PieceFlag>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr auto operator ^(const PieceFlag& a, const PieceFlag& b) -> PieceFlag {
    return static_cast<PieceFlag>(static_cast<uint8_t>(a) ^ static_cast<uint8_t>(b));
}

constexpr auto operator ~(const PieceFlag& a) -> PieceFlag {
    return static_cast<PieceFlag>(~static_cast<uint8_t>(a));
}

constexpr auto operator &=(PieceFlag& a, const PieceFlag& b) -> PieceFlag& {
    a = a & b;
    return a;
}

constexpr auto operator |=(PieceFlag& a, const PieceFlag& b) -> PieceFlag& {
    a = a | b;
    return a;
}

constexpr auto operator ^=(PieceFlag& a, const PieceFlag& b) -> PieceFlag& {
    a = a ^ b;
    return a;
}

/// <summary>
/// A Chess Piece object
/// </summary>
class Piece final {
public:
    //Ctors

    Piece() noexcept :
        m_Transform(Matrix::CreateTranslation(Vector3(0.0F, 0.0F, 0.01F))),
		m_Type(PieceFlag::None)
	{}

    explicit Piece(const PieceFlag& type) noexcept :
		m_Transform(Matrix::CreateTranslation(Vector3(0.0F, 0.0F, 0.01F))),
		m_Type(type)
    {}

    Piece(const PieceFlag& type, const Vector2& pos) noexcept :
        m_Transform(Matrix::CreateTranslation(Vector3(pos.x, pos.y, 0.01F))),
		m_Type(type)
	{}

    /// <summary>
    /// Sets the position of the piece in world space
    /// </summary>
    /// <param name="pos"><c>Vector2</c> The position to set the piece to</param>
    auto SetPosition(const Vector2& pos) noexcept -> void {
        const auto currentPos = m_Transform.Translation();
        m_Transform.Translation(Vector3(pos.x, pos.y, currentPos.z));
    }

    /// <summary>
    /// Gets the position of the piece in world space
    /// </summary>
    [[nodiscard]] auto GetPosition() const noexcept -> Vector2 {
        const auto currentPos = m_Transform.Translation();
        return { currentPos.x, currentPos.y };
    }

    /// <summary>
    /// Sets the Z index of the piece
    /// </summary>
    /// <param name="index"><c>float</c> The index to set the piece to</param>
    auto SetZIndex(float index) noexcept -> void {
        const auto pos = m_Transform.Translation();
        m_Transform.Translation(Vector3(pos.x, pos.y, index));
    }

    /// <summary>
    /// Checks if the given point is inside the square that the piece occupies
    /// </summary>
    /// <param name="point"><c>Vector2</c> The point to check</param>
    /// <returns><c>true</c> if the point was inside the square</returns>
    [[nodiscard]] auto PointInside(const Vector2& point) const noexcept -> bool {
        const auto pos = m_Transform.Translation();
        return point.x > pos.x && point.x < pos.x + 1 && point.y > pos.y && point.y < pos.y + 1;
    }

    /// <summary>
    /// Gets the transformation matrix of the piece
    /// </summary>
    [[nodiscard]] auto GetModelMatrix() const noexcept -> Matrix {
        return m_Transform;
    }

    /// <summary>
    /// Gets the <c>PieceFlag</c> property of the piece
    /// </summary>
    [[nodiscard]] auto GetType() const noexcept -> PieceFlag {
        return m_Type;
    }

    /// <summary>
    /// Checks the <c>PieceFlag</c> property of the piece
    /// </summary>
    /// <param name="flag"><c>PieceFlag</c> The flag to check</param>
    [[nodiscard]] auto Is(PieceFlag flag) const -> bool {
        return (m_Type & flag) == flag;
    }

    /// <summary>
    /// Gets the color <c>PieceFlag</c> of the piece
    /// </summary>
    [[nodiscard]] auto GetColorFlag() const noexcept -> PieceFlag {
		return Is(PieceFlag::White) ? PieceFlag::White : PieceFlag::Black;
	}

private:
    Matrix m_Transform;
    PieceFlag m_Type;
};