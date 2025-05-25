#pragma once
#include <Position.hpp>

class Move final {

public:
	enum class MoveType {
		Normal,
		Capture,
		EnPassant,
		Castle,
		Promotion,
		PromotionCapture
	};

	///<summary>
	/// Initializes a new instance of the <c>Move</c> class.
	/// </summary>
	/// <param name="from"><c>Vector2</c> The starting position of the move</param>
	/// <param name="to"><c>Vector2</c> The ending position of the move</param>
	/// <param name="type"><c>MoveType</c> The type of the move</param>
	explicit Move(Position from, Position to, const MoveType& type) noexcept : From(from), To(to), Type(type) {}

	Position From;
	Position To;
	MoveType Type;
};