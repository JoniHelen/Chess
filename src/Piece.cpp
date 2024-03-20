#include <pch.hpp>
#include <Piece.hpp>

Piece::Piece(const PieceFlag& type) noexcept : m_Type(type) {
    m_Transform = Matrix::CreateTranslation(Vector3(0, 0, 0.01f));
}

Piece::Piece(const PieceFlag& type, const Vector2& pos) noexcept : m_Type(type) {
    m_Transform = Matrix::CreateTranslation(Vector3(pos.x, pos.y, 0.01f));
}

bool Piece::PointInside(const Vector2 &point) const noexcept {
    const auto pos = m_Transform.Translation();
    return point.x > pos.x && point.x < pos.x + 1 && point.y > pos.y && point.y < pos.y + 1;
}

void Piece::SetPosition(const Vector2& pos) noexcept {
    m_Transform.Translation(Vector3(pos.x, pos.y, 0.01f));
}

void Piece::SetPickUpPosition(const Vector2& pos) noexcept{
    m_PickUpPosition = pos;
}

Matrix Piece::GetModelMatrix() const noexcept {
    return m_Transform;
}

PieceFlag Piece::GetType() const noexcept {
    return m_Type;
}