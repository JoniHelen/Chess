#include <pch.hpp>
#include <Board.hpp>
#include <Piece.hpp>
#include <Move.hpp>

using MoveType = Move::MoveType;

void Board::SetState(const std::string_view fen) {
    s_BoardData.clear();

    Position currentSquare = { 0, 7 }; // Start from the top left square as per FEN specification
    bool piecesPlaced = false;

    // Iterate the characters
    for (int i = 0, n = fen.size(); i < n; i++) {

        const auto& c = fen[i];

        if (!piecesPlaced) {
            if (int space = 0; IsEmptySpace(c, space)) { // Process empty space
                currentSquare += { space, 0 };
            }
            else if (Piece p; IsPiece(c, p)) { // Process pieces
                p.SetPosition(currentSquare);
                s_BoardData.emplace(currentSquare, p);
                currentSquare += { 1, 0 };
            }
            else if (c == '/') { // New rank
                currentSquare = { 0, currentSquare.y - 1 };
            }
            else if (c == ' ') {
                piecesPlaced = true; // end of piece placement
            }
        }
        else {
	        switch (c)
	        {
				case 'w': {
                    s_WhiteToMove = true;
                    break;
                }
				case 'b': {
                    s_WhiteToMove = false;
                    break;
                }
				case 'K': {
                    s_WhiteCanCastleKingSide = true;
                    break;
                }
                case 'Q': {
                    s_WhiteCanCastleQueenSide = true;
                    break;
                }
                case 'k': {
                    s_BlackCanCastleKingSide = true;
                    break;
                }
				case 'q': {
                    s_BlackCanCastleQueenSide = true;
                    break;
                }
                case '-': {
                    s_EnPassantAvailable = false;
                    break;
                }
                default: {
                    break;
				}
	        }

            if (c >= 'a' && c <= 'h') {
                s_EnPassantAvailable = true;
                s_EnPassantPosition = { c - 'a',  fen[++i] - '1' };
            }
        }
    }
}

auto Board::IsEmptySpace(const char &c, int &space) -> bool {
    space = c - '0'; // Convert char to int
    return c > '0' && c < '9'; // Space can be in range [1, 8];
}

auto Board::IsPiece(const char &c, Piece& piece) -> bool {
    // Convert character into a piece
    switch(c) {
        case 'r':
            piece = Piece(PieceFlag::Rook | PieceFlag::Black);
            return true;

        case 'n':
            piece = Piece(PieceFlag::Knight | PieceFlag::Black);
            return true;

        case 'b':
            piece = Piece(PieceFlag::Bishop | PieceFlag::Black);
            return true;

        case 'q':
            piece = Piece(PieceFlag::Queen | PieceFlag::Black);
            return true;

        case 'k':
            piece = Piece(PieceFlag::King | PieceFlag::Black);
            return true;

        case 'p':
            piece = Piece(PieceFlag::Pawn | PieceFlag::Black);
            return true;

        case 'R':
            piece = Piece(PieceFlag::Rook | PieceFlag::White);
            return true;

        case 'N':
            piece = Piece(PieceFlag::Knight | PieceFlag::White);
            return true;

        case 'B':
            piece = Piece(PieceFlag::Bishop | PieceFlag::White);
            return true;

        case 'Q':
            piece = Piece(PieceFlag::Queen | PieceFlag::White);
            return true;

        case 'K':
            piece = Piece(PieceFlag::King | PieceFlag::White);
            return true;

        case 'P':
            piece = Piece(PieceFlag::Pawn | PieceFlag::White);
            return true;

        default:
            return false;
    }
}

auto Board::GetBoard() -> std::unordered_map<Position, Piece>& {
    return s_BoardData;
}

auto Board::MakeMove(const Move& move) -> void {
	switch (move.Type) {
		case MoveType::Normal: {
			auto piece = s_BoardData.find(move.From);
			if (piece != s_BoardData.end()) {
				s_BoardData.insert({ move.To, Piece(piece->second) });
				s_BoardData.erase(piece);
			}
            break;
		}

        case MoveType::Capture: {
			auto piece = s_BoardData.find(move.From);
			auto target = s_BoardData.find(move.To);

			if (piece != s_BoardData.end() && target != s_BoardData.end()) {
				s_BoardData.erase(target);
				s_BoardData.insert({ move.To, Piece(piece->second) });
				s_BoardData.erase(piece);
			}
			break;
		}

        // TODO: Implement EnPassant
		case MoveType::EnPassant: {
			auto piece = s_BoardData.find(move.From);
			auto target = s_BoardData.find(move.To);

			if (piece != s_BoardData.end() && target != s_BoardData.end()) {
				s_BoardData.erase(target);
				s_BoardData.insert({ move.To, Piece(piece->second) });
				s_BoardData.erase(piece);
			}
			break;
		}

        // TODO: Implement Castle
		case MoveType::Castle: {
			auto piece = s_BoardData.find(move.From);
			if (piece != s_BoardData.end()) {
				s_BoardData.insert({ move.To, Piece(piece->second) });
				s_BoardData.erase(piece);
			}
			break;
		}

		case MoveType::Promotion: {
			auto piece = s_BoardData.find(move.From);
			if (piece != s_BoardData.end()) {
				s_BoardData.insert({ move.To, Piece(PieceFlag::Queen | piece->second.GetColorFlag(), move.To) });
				s_BoardData.erase(piece);
			}
			break;
		}

        case MoveType::PromotionCapture: {
            auto piece = s_BoardData.find(move.From);
            auto target = s_BoardData.find(move.To);

            if (piece != s_BoardData.end() && target != s_BoardData.end()) {
                s_BoardData.erase(target);
                s_BoardData.insert({ move.To, Piece(PieceFlag::Queen | piece->second.GetColorFlag(), move.To) });
                s_BoardData.erase(piece);
            }
            break;
        }
	}
}

auto Board::CalculatePawnAttacks(Position position, const bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void {
    const auto directions = std::array<Position, 2> { {
        { -1, s_WhiteToMove ? 1 : -1 },
        { 1, s_WhiteToMove ? 1 : -1 }
	} };

	const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;

	for (const auto& direction : directions) {
        auto positionCopy = position;
        if (positionCopy.Advance(direction)) {
            if (const auto piece = s_BoardData.find(positionCopy); piece != s_BoardData.end()) {
                if (piece->second.Is(enemyFlag)) {
                    attackedSquares.emplace_back(positionCopy);
                }
            }
            else if (!ignoreEmptySquares) {
                attackedSquares.emplace_back(positionCopy);
			}
        }
	}
}

auto Board::CalculatePawnMoves(const Position position, std::vector<Move>& moves) -> void
{

}

auto Board::CalculateRookAttacks(Position position, const bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void {
    constexpr auto directions = std::array<Position, 4> { {
        { 1, 0 },
        { -1, 0 },
        { 0, 1 },
        { 0, -1 }
    } };

    const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;

    for (const auto& direction : directions) {
        auto positionCopy = position;
		while (positionCopy.Advance(direction) && !s_BoardData.contains(positionCopy)) {
            if (!ignoreEmptySquares) {
                attackedSquares.emplace_back(positionCopy);
            }
		}

        if (const auto piece = s_BoardData.find(positionCopy); piece != s_BoardData.end() &&  piece->second.Is(enemyFlag)) {
			attackedSquares.emplace_back(positionCopy);
		}
	}
}

auto Board::CalculateRookMoves(const Position position, std::vector<Move>& moves) -> void {
    if (s_CheckState.IsInCheck) {
        if (s_CheckState.Threats.size() > 1) {
            return;
        }

        std::vector<Position> attacks;
        CalculateRookAttacks(position, false, attacks);
        
        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
            for (const auto& attack : attacks) {
				if (!std::ranges::contains(freeSquares, attack)) {
                    continue;
				}

                if (std::ranges::contains(s_CheckState.Threats, attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }

                const auto square = std::ranges::find(s_CheckState.BlockingSquares, attack);

                if (square != s_CheckState.BlockingSquares.end()) {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
			}
        }
        else {
            for (const auto& attack : attacks) {
                if (std::ranges::contains(s_CheckState.Threats, attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }

                const auto square = std::ranges::find(s_CheckState.BlockingSquares, attack);

                if (square != s_CheckState.BlockingSquares.end()) {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
    }
    else {
        std::vector<Position> attacks;
        CalculateRookAttacks(position, false, attacks);

        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
            for (const auto& attack : attacks) {
                if (!std::ranges::contains(freeSquares, attack)) {
                    continue;
                }

                if (s_BoardData.contains(attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }
                else {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
        else {
            for (const auto& attack : attacks) {
                if (s_BoardData.contains(attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }
                else {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
    }
}

auto Board::CalculateBishopAttacks(Position position, const bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void {
    constexpr auto directions = std::array<Position, 4> { {
        { 1, 1 },
        { -1, 1 },
        { 1, -1 },
        { -1, -1 }
    } };

    const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;

    for (const auto& direction : directions) {
        auto positionCopy = position;
        while (positionCopy.Advance(direction) && !s_BoardData.contains(positionCopy)) {
            if (!ignoreEmptySquares) {
                attackedSquares.emplace_back(positionCopy);
            }
        }

        if (const auto piece = s_BoardData.find(positionCopy); piece != s_BoardData.end() && piece->second.Is(enemyFlag)) {
            attackedSquares.emplace_back(positionCopy);
        }
    }
}

auto Board::CalculateBishopMoves(const Position position, std::vector<Move>& moves) -> void {
    if (s_CheckState.IsInCheck) {
        if (s_CheckState.Threats.size() > 1) {
            return;
        }

        std::vector<Position> attacks;
        CalculateBishopAttacks(position, false, attacks);

        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
            for (const auto& attack : attacks) {
                if (!std::ranges::contains(freeSquares, attack)) {
                    continue;
                }

                if (std::ranges::contains(s_CheckState.Threats, attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }

                const auto square = std::ranges::find(s_CheckState.BlockingSquares, attack);

                if (square != s_CheckState.BlockingSquares.end()) {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
        else {
            for (const auto& attack : attacks) {
                if (std::ranges::contains(s_CheckState.Threats, attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }

                const auto square = std::ranges::find(s_CheckState.BlockingSquares, attack);

                if (square != s_CheckState.BlockingSquares.end()) {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
    }
    else {
        std::vector<Position> attacks;
        CalculateBishopAttacks(position, false, attacks);

        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
            for (const auto& attack : attacks) {
                if (!std::ranges::contains(freeSquares, attack)) {
                    continue;
                }

                if (s_BoardData.contains(attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }
                else {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
        else {
            for (const auto& attack : attacks) {
                if (s_BoardData.contains(attack)) {
                    moves.emplace_back(position, attack, MoveType::Capture);
                }
                else {
                    moves.emplace_back(position, attack, MoveType::Normal);
                }
            }
        }
    }
}

auto Board::CalculateKnightAttacks(Position position, const bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void {
    constexpr auto directions = std::array<Position, 8> { {
        { 1, 2 },
        { -1, 2 },
        { 1, -2 },
        { -1, -2 },
        { 2, 1 },
        { 2, -1 },
        { -2, 1 },
        { -2, -1 }
    } };

    const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;

    for (const auto& direction : directions) {
        auto positionCopy = position;
        if (positionCopy.Advance(direction)) {
            if (const auto piece = s_BoardData.find(positionCopy); piece != s_BoardData.end()) {
                if (piece->second.Is(enemyFlag)) {
                    attackedSquares.emplace_back(positionCopy);
                }
            }
            else if (!ignoreEmptySquares) {
                attackedSquares.emplace_back(positionCopy);
            }
        }
    }
}

auto Board::CalculateKnightMoves(const Position position, std::vector<Move>& moves) -> void {
    if (s_CheckState.IsInCheck) {
	    if (s_CheckState.Threats.size() > 1) {
		    return;
	    }

        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
			return;
		}

        std::vector<Position> attacks;
        CalculateKnightAttacks(position, false, attacks);

        for (const auto& attack : attacks) {
            if (std::ranges::contains(s_CheckState.Threats, attack)) {
				moves.emplace_back(position, attack, MoveType::Capture);
			}

            const auto square = std::ranges::find(s_CheckState.BlockingSquares, attack);

            if (square != s_CheckState.BlockingSquares.end()) {
                moves.emplace_back(position, attack, MoveType::Normal);
            }
		}
    }
    else {
        if (std::vector<Position> freeSquares; CheckForPins(position, freeSquares)) {
            return;
        }

        std::vector<Position> attacks;
        CalculateKnightAttacks(position, false, attacks);

        for (const auto& attack : attacks) {
            if (s_BoardData.contains(attack)) {
				moves.emplace_back(position, attack, MoveType::Capture);
            }
            else {
                moves.emplace_back(position, attack, MoveType::Normal);
            }
        }
    }
}

auto Board::CalculateQueenAttacks(Position position, const bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void
{
    CalculateRookAttacks(position, ignoreEmptySquares, attackedSquares);
	CalculateBishopAttacks(position, ignoreEmptySquares, attackedSquares);
}

auto Board::CalculateQueenMoves(const Position position, std::vector<Move>& moves) -> void
{
	CalculateRookMoves(position, moves);
	CalculateBishopMoves(position, moves);
}

auto Board::CalculateKingAttacks(Position position, bool ignoreEmptySquares, std::vector<Position>& attackedSquares) -> void {
	
}

auto Board::CalculateKingMoves(Position position, std::vector<Move>& moves) -> void {

}

auto Board::UpdateCheckState() -> void {
    s_CheckState.IsInCheck = false;
	s_CheckState.Threats.clear();
    s_CheckState.BlockingSquares.clear();

	const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;
    const auto king = GetKingPosition(s_WhiteToMove);

    // Pawn threats
	{
		std::vector<Position> threats;
    	CalculatePawnAttacks(king, true, threats);

    	for (const auto& pos : threats) {
    		if (auto piece = s_BoardData.find(pos); piece != s_BoardData.end() && piece->second.Is(PieceFlag::Pawn | enemyFlag)) {
    			s_CheckState.IsInCheck = true;
    			s_CheckState.Threats.emplace_back(pos);
    		}
    	}
	}

    // Rook & Queen threats
    {
	    std::vector<Position> threats;
        CalculateRookAttacks(king, true, threats);

		for (const auto& pos : threats) {
            auto piece = s_BoardData.find(pos);

            if (piece != s_BoardData.end()) {
                if (piece->second.Is(PieceFlag::Rook | enemyFlag)) {
                    s_CheckState.IsInCheck = true;
                    s_CheckState.Threats.emplace_back(pos);

                    GetSquaresBetween(king, pos, s_CheckState.BlockingSquares);
                }
                else if (piece->second.Is(PieceFlag::Queen | enemyFlag)) {
                    s_CheckState.IsInCheck = true;
                    s_CheckState.Threats.emplace_back(pos);

                    GetSquaresBetween(king, pos, s_CheckState.BlockingSquares);
                }
            }
		}
	}

    // Bishop & Queen threats
	{
		std::vector<Position> threats;
		CalculateBishopAttacks(king, true, threats);

		for (const auto& pos : threats) {
            auto piece = s_BoardData.find(pos);

			if (piece != s_BoardData.end()) {
                if (piece->second.Is(PieceFlag::Bishop | enemyFlag)) {
                    s_CheckState.IsInCheck = true;
                    s_CheckState.Threats.emplace_back(pos);

                    GetSquaresBetween(king, pos, s_CheckState.BlockingSquares);
                }
                else if (piece->second.Is(PieceFlag::Queen | enemyFlag)) {
                    s_CheckState.IsInCheck = true;
                    s_CheckState.Threats.emplace_back(pos);

                    GetSquaresBetween(king, pos, s_CheckState.BlockingSquares);
                }
			}
		}
	}

    // Knight threats
    {
        std::vector<Position> threats;
        CalculateKnightAttacks(king, true, threats);

        for (const auto& pos : threats) {
            if (auto piece = s_BoardData.find(pos); piece != s_BoardData.end() && piece->second.Is(PieceFlag::Knight | enemyFlag)) {
                s_CheckState.IsInCheck = true;
                s_CheckState.Threats.emplace_back(pos);
            }
        }
    }
}

auto Board::GetKingPosition(bool white) -> Position {
    const auto flag = white ? PieceFlag::White : PieceFlag::Black;
    const auto king = std::ranges::find_if(s_BoardData, [&](const auto& piece) -> bool {
        return piece.second.Is(PieceFlag::King | flag);
    });
    return king != s_BoardData.end() ? king->first : Position { -1, -1 };
}

auto Board::CheckForPins(const Position& ignorePos, std::vector<Position>& freeSquares) -> bool {
    const auto enemyFlag = s_WhiteToMove ? PieceFlag::Black : PieceFlag::White;
    auto king = GetKingPosition(s_WhiteToMove);

    auto direction = ignorePos - king;
    direction.Clamp();

    auto pos = king;
    while (pos.Advance(direction)) {
        if (pos != ignorePos && s_BoardData.contains(pos)) {
            return false;
        }

        if (pos == ignorePos) {
			break;
		}
    }

    if (!s_BoardData.contains(pos)) {
        return false;
	}

    const auto isDiagonal = direction.x != 0 && direction.y != 0;

    while(king.Advance(direction)) {
        if (king != ignorePos && !s_BoardData.contains(king)) {
			freeSquares.emplace_back(king);
		}
        else if (king != ignorePos && s_BoardData.contains(king)) {
            break;
        }
    }

    const auto piece = s_BoardData.find(king);

    if (piece != s_BoardData.end()) {
		if (piece->second.Is(enemyFlag)) {
			if (isDiagonal && (piece->second.Is(PieceFlag::Bishop) || piece->second.Is(PieceFlag::Queen))) {
                return true;
			}

			if (!isDiagonal && (piece->second.Is(PieceFlag::Rook) || piece->second.Is(PieceFlag::Queen))) {
                return true;
			}
		}
    }

    return false;
}

auto Board::GetSquaresBetween(const Position& start, const Position& end, std::vector<Position>& squares) -> void {
	auto direction = end - start;
	direction.Clamp();

	auto currentPos = start + direction;

	while (currentPos != end) {
		squares.emplace_back(currentPos);
		currentPos += direction;
	}
}

auto Board::CalculateLegalMoves(const Position& pos, const Piece& piece, std::vector<Move>& moves) -> void {

    moves.clear();

    if (s_WhiteToMove != piece.Is(PieceFlag::White)) {
    	return;
	}

    if (piece.Is(PieceFlag::Pawn)) {
	    //CalculatePawnMoves(pos, moves);
    }
	else if (piece.Is(PieceFlag::Rook)) {
		CalculateRookMoves(pos, moves);
	}
	else if (piece.Is(PieceFlag::Bishop)) {
		CalculateBishopMoves(pos, moves);
	}
	else if (piece.Is(PieceFlag::Knight)) {
		CalculateKnightMoves(pos, moves);
	}
	else if (piece.Is(PieceFlag::Queen)) {
		CalculateQueenMoves(pos, moves);
	}
	else if (piece.Is(PieceFlag::King)) {
		//CalculateKingMoves(pos, moves);
	}
}