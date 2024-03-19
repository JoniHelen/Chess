#pragma once

class Piece final {
public:
    enum class Type {
        Pawn = 1 << 0,
        Rook = 1 << 1,
        Knight = 1 << 2,
        Bishop = 1 << 3,
        King = 1 << 4,
        Queen = 1 << 5,
        White = 1 << 6,
        Black = 1 << 7
    };

    explicit Piece(const Type& type) noexcept;
    Piece(const Type& type, const Vector2& pos) noexcept;
    void SetPosition(const Vector2& pos) noexcept;
    void SetImage(const ComPtr<ID3D11ShaderResourceView>& srv) noexcept;
    void SetPickUpPosition(const Vector2& pos) noexcept;
    [[nodiscard]] bool PointInside(const Vector2& point) const noexcept;
    [[nodiscard]] ID3D11ShaderResourceView* const* GetImage() const;
    [[nodiscard]] Matrix GetModelMatrix() const noexcept;

private:
    ComPtr<ID3D11ShaderResourceView> m_Image;
    Matrix m_Transform;
    Type m_Type;
    Vector2 m_PickUpPosition;
};

constexpr Piece::Type operator &(const Piece::Type& a, const Piece::Type& b) {
    return static_cast<Piece::Type>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr Piece::Type operator |(const Piece::Type& a, const Piece::Type& b) {
    return static_cast<Piece::Type>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr Piece::Type operator ^(const Piece::Type& a, const Piece::Type& b) {
    return static_cast<Piece::Type>(static_cast<int>(a) ^ static_cast<int>(b));
}

constexpr Piece::Type operator ~(const Piece::Type& a) {
    return static_cast<Piece::Type>(~static_cast<int>(a));
}

constexpr Piece::Type& operator &=(Piece::Type& a, const Piece::Type& b) {
    a = a & b;
    return a;
}

constexpr Piece::Type& operator |=(Piece::Type& a, const Piece::Type& b) {
    a = a | b;
    return a;
}

constexpr Piece::Type& operator ^=(Piece::Type& a, const Piece::Type& b) {
    a = a ^ b;
    return a;
}