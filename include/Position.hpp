#pragma once
#include <cstdint>

struct Position {
	int x, y;

	auto Advance(const Position pos) -> bool {
		*this += pos;
		return x <= 7 && y <= 7 && x >= 0 && y >= 0;
	}

	auto Clamp() -> void {
		if (x < -1) x = -1;
		if (y < -1) y = -1;
		if (x > 1) x = 1;
		if (y > 1) y = 1;
	}

	auto operator+(const Position& rhs) const -> Position {
		return { x + rhs.x, y + rhs.y };
	}

	auto operator-(const Position& rhs) const -> Position {
		return { x - rhs.x, y - rhs.y };
	}

	auto operator+=(const Position& rhs) -> Position& {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	auto operator-=(const Position& rhs) -> Position& {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	auto operator==(const Position& rhs) const -> bool {
		return x == rhs.x && y == rhs.y;
	}

	auto operator!=(const Position& rhs) const -> bool {
		return x != rhs.x || y != rhs.y;
	}

	operator Vector2() const {
		return { static_cast<float>(x), static_cast<float>(y) };
	}
};

/// <summary>
/// Hash specialization for Position for use in unordered_map
/// </summary>
template<>
struct std::hash<Position> {
    auto operator ()(const Position& v) const noexcept -> std::size_t {
        return static_cast<uint32_t>(v.x) * 73856093U ^ static_cast<uint32_t>(v.y) * 19349663U;
    }
};