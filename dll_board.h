#ifndef DLL_BOARD_H
#define DLL_BOARD_H

#include "board.h"
#include "constants.h"
#include "move.h"

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

class Tile {
    public:
        PieceEnum m_contents;
        // We use unsigned to represent coords so we can utilize overflow.
        Coords m_coords;
        static const size_t NUM_ADJACENT_DIRECTIONS = 8; // 4 cardinal, 4 diagonal //TODO: may remove diagonal if unused
        Tile* m_adjacents[NUM_ADJACENT_DIRECTIONS]; // Links to adjacent tiles.

        Tile(PieceEnum _contents, Coords _coords);

        // Sets m_adjacents of this tile and the tile given. Also takes nullptr for no adjacent
        void SetAdjacent(DirectionEnum _dir, Tile* _adj);
        inline bool HasAdjacent(DirectionEnum _dir) {
            return m_adjacents[_dir] != nullptr;
        }
};

class DLLBoard : public Board {
    public:
        // Every tile, which contains piece and coords.
        std::vector<Tile*> m_tiles;

        // Tiles which are at the max or min coords in an axis.
        // indexed by the direction in which they are an extrema, 
        // i.e. UP DOWN LEFT RIGHT
        std::vector<Tile*> m_extremaTiles[4];
        /* FIXME: oh god how on earth do I keep track of this? When this naturally empties (TileMove removes last tile with extrema coord), then we somehow have to get 
         * all of the new tiles at the new extrema, and the only way we can look that up is by coords which is really dumb.
         * I don't see how the natural conclusion isn't to just use arrays, and try and use the sparseness to come up with some clever tricks.
        */

        
        /** 
         * For looking up where pieces are at by their type and color. 
         * Organized in same order as PieceEnum: 
         *      since PieceEnum starts pieces on 1, the zero element of this is a placeholder.
        */
        std::vector<Tile*> m_pieceLocations[NUM_PIECE_TYPES * 2+1];

        /** 
         * Creates a new board from SFEN.
         */
        DLLBoard();
        void init(const std::string _sfen);
        DLLBoard(const std::string _sfen);

        /**
         * Boards are equal if all independent fields except moveHistory are equal. 
         * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
         * for most practical purposes. Really, this is just here to test if the hash function is working.
         */
        bool operator==(const Board& _other) const;

        /** 
         * Update the position oldLocation to be newLocation for type piece.
         * Returns false if it does not find such a piece to update.
         */
        bool updatePieceInPL(PieceEnum _piece, Tile* _oldLocation, Tile* _newLocation);

        /** 
         * Remove the piece at location for type piece.
         * Returns false if it does not find such a piece to remove.
         */
        bool removePieceFromPL(PieceEnum _piece, Tile* _location);
        /** 
         * Adds the piece at location for type piece.
         */
        void addPieceToPL(PieceEnum _piece, Tile* _location) {
            m_pieceLocations[_piece].push_back(_location);
        }

        /** 
         * Print the current tiles and pieces in a nice ASCII format.
         * @warning: If the board's tiles have errors in their adjacents arrays, this method may have unexpected behavior
         */
        std::string getAsciiBoard();

        std::string toSfen();

        // Gets the tile at _coords. 
        // If _useInternal=true, it will get the tile whose m_coords=_coords.
        // If _useInternal=false, it will get the tile whose displayed coords = _coords.
        Tile* getTile(Coords _coords, bool _useInternal);

        PieceEnum getPiece(size_t _f, size_t _r) const {
            //TODO: implement
            return EMPTY;
        };

        bool apply(std::shared_ptr<Move> _move);
        bool apply(std::shared_ptr<PieceMove> _move);
        bool apply(std::shared_ptr<TileMove> _move);
        bool apply(std::shared_ptr<TileDeletion> _move);

        bool undo(std::shared_ptr<Move> _move);
        bool undo(std::shared_ptr<PieceMove> _move);
        bool undo(std::shared_ptr<TileMove> _move);
        bool undo(std::shared_ptr<TileDeletion> _move);

        uint64_t getHash() const {
            //TODO: implement
            return 0u;
        };
    protected:
        // Sort pieces by their coordinates.
        // when _priorityRank is true, all entries of first rank will be listed, followed by second rank, etc., e.g. a1, b1, c1, ..., g8, h8
        // when _priorityRank is false, all entries of first file will be listed, followed by second file, etc., e.g. a1, a2, a3, ..., h7, h8
        // when _reverseRank is true, rank order will be reversed, i.e. 8,7,6,...,2,1
        // when _reverseFile is true, file order will be reversed, i.e. h,g,f,...,b,a
        void sortByCoords(bool _priorityRank, bool _reverseFile=false, bool _reverseRank=false);

        inline bool compareTileByFile(Tile* _t1, Tile* _t2, bool _reverse=false) {
            if (_reverse) {
                return coordLessThan(_t2->m_coords.first, _t1->m_coords.first, m_minCoords.first);
            } else {
                return coordLessThan(_t1->m_coords.first, _t2->m_coords.first, m_minCoords.first);
            }
        }
        inline bool compareTileByRank(Tile* _t1, Tile* _t2, bool _reverse=false) {
            if (_reverse) {
                return coordLessThan(_t2->m_coords.second, _t1->m_coords.second, m_minCoords.second);
            } else {
                return coordLessThan(_t1->m_coords.second, _t2->m_coords.second, m_minCoords.second);
            }
        }

        void updateExtrema(const Coords& _new);

        int staticEvaluation();

        // For debugging purposes. Prints all pieces in the m_pieceLocations list
        std::string printPieces() {
            std::string result = "[";
            for (int i = 1; i < NUM_PIECE_TYPES*2+1; i++) {
                result += PIECE_LETTERS[i];
                result += "=" + std::to_string(m_pieceLocations[i].size()) + "{";
                for (Tile* t : m_pieceLocations[i]) {
                    result += PIECE_LETTERS[t->m_contents];
                    result += " ";
                }
                result += "} ";
            }
            result += "]";
            return result;
        }

        std::vector<std::unique_ptr<Move>> getMoves(PieceColor _color);

        // Gets the external coords of a given tile.
        // External coords are used by things like moves and stuff.
        // Assumes _tile is valid, and not nullpointer
        Coords externalCoords(Tile* _tile) {
            Coords ext = _tile->m_coords; // copy
            ext.first -= m_minCoords.first;
            ext.second -= m_minCoords.second;
            return ext;
        }
};

#endif