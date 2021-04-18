#ifndef DLL_BOARD_H
#define DLL_BOARD_H

#include <cstdint>
#include <stack>
#include <vector>
#include <string>

#include "constants.h"
#include "board.h"

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
        /* ------- independent fields, provide necessary information about board ------- */
        std::vector<Tile*> m_tiles; // Every tile, which contains piece and coords.
        
        /** For looking up where pieces are at by their type and color. */
        std::vector<Tile*> pieceLocations[2 * NUM_PIECE_TYPES];

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
         * Print the current tiles and pieces in a nice ASCII format.
         * @warning: If the board's tiles have errors in their adjacents arrays, this method may have unexpected behavior
         */
        std::string getAsciiBoard();

        std::string toSfen();

        Tile* getTile(Coords _coords);

        Coords getDimensions() const {
            //TODO: implement
            return std::make_pair(0u,0u);
        };

        PieceEnum getPiece(size_t _f, size_t _r) const {
            //TODO: implement
            return EMPTY;
        };

        bool moveSelection(Coords _select1, Coords _select2, Coords _goal1) {
            //TODO: implement
            return false;
        };

        bool movePiece(Coords _start, Coords _goal) {
            //TODO: implement
            return false;
        };

        bool undo(size_t _numMoves=1) {
            //TODO: implement
            return false;
        };

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
};

#endif