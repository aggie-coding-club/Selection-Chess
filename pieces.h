#ifndef PIECES_H
#define PIECES_H

#include <vector>

#include "constants.h"

class PieceType {
    public:
    	PieceEnum m_type;
    	Tile* m_tile;
    	std::vector<Tile*> m_moves;

    	// seems redundant to give type as well
    	PieceType(PieceEnum _type, Tile* _tile);

    private:
    	void addDirectionalMoves(DirectionEnum _dir);
    	void addRookMoves();
    	void addBishopMoves();
    	void populateMoves();
};


#endif