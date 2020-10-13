#include <iostream>

#include "constants.h"
#include "utils.h"

#include "pieces.h"

void PieceType::addDirectionalMoves(DirectionEnum _dir) {
	Tile* tile = m_tile->m_adjacents[_dir];
	while(tile != nullptr) {
		// if the sqaure has a piece
		// might have to change '.'
		if(getCharFromPiece(tile->m_contents) != '.') {
			if(	isWhite(m_type) && isWhite(tile->m_contents) ||
				isBlack(m_type) && isBlack(tile->m_contents)) {
				break;
			} else {
				m_moves.push_back(tile);
				break;
			}
		}
		m_moves.push_back(tile);
		tile = tile->m_adjacents[_dir];
	}
}

void PieceType::addRookMoves() {
	// could also combine this with bishop moves and add 
	// a directional arguent
	// right now just adds in four directions
	addDirectionalMoves(LEFT);
	addDirectionalMoves(RIGHT);
	addDirectionalMoves(UP);
	addDirectionalMoves(DOWN);
}

void PieceType::addBishopMoves() {
	addDirectionalMoves(DOWN_LEFT);
	addDirectionalMoves(UP_RIGHT);
	addDirectionalMoves(DOWN_RIGHT);
	addDirectionalMoves(UP_LEFT);
}

void PieceType::populateMoves() {
	switch(m_type) {
		case W_ROOK:
			addRookMoves();
			break;
		case B_ROOK:
			addRookMoves();
			break;
		case W_BISHOP:
			addBishopMoves();
			break;
		case B_BISHOP:
			addBishopMoves();
			break;
		case W_QUEEN:
			addRookMoves();
			addBishopMoves();
			break;
		case B_QUEEN:
			addRookMoves();
			addBishopMoves();
			break;	
		default:
			// TODO: remove two debuggin prints directly below
			std::cout << "no piece recognized" << std::endl;
			std::cout << "piece value is " << getCharFromPiece(m_type) << std::endl;
			break;
	}
	for(Tile* tile : m_moves) {
		std::cout << tile << std::endl;
	}
}

PieceType::PieceType(PieceEnum _type, Tile* _tile) {
	m_type = _type;
	m_tile = _tile;

	// populate m_moves with legal moves
	populateMoves();
}