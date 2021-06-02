#ifndef PIECES_H
#define PIECES_H

#include "constants.h"

class PieceType {
public:
};

class Piece {
	PieceColor color;
public:
	inline PieceColor getColor() {
		return color;
	}

	virtual PieceType getType() = 0;

	virtual Tile[] getPossibleMoves() = 0;
};

/* SLIDING PIECES */

class SlidingPiece : Piece {
	DirectionEnum moveDirections[];
public:
	virtual PieceType getType() = 0;
};

class RookPiece : SlidingPiece {
public:
	inline PieceType getType() {
		return PieceType.ROOK;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}
};

class BishopPiece : SlidingPiece {
public:
	inline PieceType getType() {
		return PieceType.BISHOP;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}
};

class QueenPiece : SlidingPiece {
public:
	inline PieceType getType() {
		return PieceType.QUEEN;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}
};

/* NON-SLIDING PIECES */

class KingPiece : Piece {
public:
	inline PieceType getType() {
		return PieceType.KING;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}

};

class PawnPiece : Piece {
public:
	inline PieceType getType() {
		return PieceType.PAWN;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}
};

class KnightPiece : Piece {
public:
	inline PieceType getType() {
		return PieceType.KNIGHT;
	}

	inline Tile[] getPossibleMoves() {
		return nullptr;
	}
};
#endif