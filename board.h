#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <stack>
#include <vector>
#include <string>
#include <map>

#include "constants.h"
#include "pieces.h"

class Tile {
    public:
        PieceEnum m_contents;
        std::pair<short, short> m_coords;
        static const size_t NUM_ADJACENT_DIRECTIONS = 8; // 4 cardinal, 4 diagonal //TODO: may remove diagonal if unused
        Tile* m_adjacents[NUM_ADJACENT_DIRECTIONS]; // Links to adjacent tiles.

        Tile(PieceEnum _contents, std::pair<short, short> _coords);

        // Sets m_adjacents of this tile and the tile given. Also takes nullptr for no adjacent
        void SetAdjacent(DirectionEnum _dir, Tile* _adj);
        inline bool HasAdjacent(DirectionEnum _dir) {
            return m_adjacents[_dir] != nullptr;
        }
};

class Board {
    public:
        /* ------- independent fields, provide necessary information about board ------- */
        std::vector<Tile*> m_tiles; // Every tile, which contains piece and coords.
        std::map<std::pair<short, short>, Tile*> m_tiles_map; // map of coords to tile pntr
        std::vector<Piece*> m_pieces; // store all 32 pieces in this vector

        // minimum x and y of this board. Because of wrap-around, the literal min integer value is not 
        // guaranteed to be the furtherest "left" or "down".
        std::pair<short, short> m_minCoords;

        short m_movesSinceLastCapture; // 50 move rule
        bool m_turnWhite; // whose turn it is
        // std::stack<Move> moveHistory; // list of moves applied to starting FEN.

        /* ------- dependent fields, store information about board that is derived from independent fields -------- */
        short m_material; // changed material score to just be material for both
        uint64_t m_hashCode;
        
        /** For looking up where pieces are at by their type and color. */
        std::vector<Tile*> pieceLocations[2 * NUM_PIECE_TYPES];

        /** 
         * Creates a new board from SFEN.
         */
        Board(const std::string _sfen);

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
        std::string getAsciiBoard(bool _showCoords = false, size_t _width = 3, size_t _height = 1, char _tileFillChar = '`');

        // class row_iterator { // FIXME: this is really ugly
        //     public:
        //         row_iterator(const Board& _board) : m_board(_board) {
        //             // TODO:
        //         }
        //         row_iterator& operator++() {
        //             // TODO:
        //             return *this;
        //         }
        //         row_iterator operator++(int) {
        //             row_iterator retval = *this;
        //             ++(*this);
        //             return retval;
        //         }
        //         bool operator==(row_iterator _other) const {
        //             // TODO:
        //             return false;
        //         }
        //         bool operator!=(row_iterator _other) const {
        //             return !(*this == _other);
        //         }
        //         long operator*() {
        //             // TODO: gets the value upon dereference
        //         };
        //         // iterator traits //FIXME: what do these do?
        //         // using difference_type = long;
        //         // using value_type = long;
        //         // using pointer = const long*;
        //         // using reference = const long&;
        //         // using iterator_category = std::forward_iterator_tag;
        //     private:
        //         const Board& m_board;
        // };

        Tile* getTileNew(std::pair<short, short> _coords); // TODO: make this replace getTile once it works
        Tile* getTile(std::pair<short, short> _coords);

};

#endif