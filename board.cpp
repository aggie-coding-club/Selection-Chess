#include <cstdint>
#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>

#include "constants.h"
#include "utils.h"

#include "board.h"

void Tile::SetAdjacent(DirectionEnum _dir, Tile* _adj) {
    m_adjacents[_dir] = _adj;
    if (_adj != nullptr) {
        _adj->m_adjacents[flipDirection(_dir)] = this;
    }
}

Tile::Tile(PieceEnum _contents, std::pair<short, short> _coords) {
    m_contents = _contents;
    m_coords = _coords;
    for (int i = 0; i < NUM_ADJACENT_DIRECTIONS; i++) {
        m_adjacents[i] = nullptr;
    }
};


/** //TODO: update description
 * Creates a new board with array set to INVALID. Usually you should call parseFen after creating a new board. 
 */
Board::Board() {
    // TODO: Implement
    m_movesSinceLastCapture = 0;
}

/** 
 * Boards are equal if all independent fields except moveHistory are equal. 
 * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
 * for most practical purposes. Really, this is just here to test if the hash function is working.
 */
bool Board::operator==(const Board& other) const {
    //TODO: implement
    return true;
}

bool Board::updatePieceInPL(PieceEnum piece, Tile* oldLocation, Tile* newLocation) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == oldLocation) { // find the match
    //         pieceLocations[piece][i] = newLocation;
    //         return true;
    //     }
    // }
    return false;
}

bool Board::removePieceFromPL(PieceEnum piece, Tile* location) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == location) { // find the match
    //         // override current position with last element in this row of PL
    //         pieceLocations[piece][i] = pieceLocations[piece][pieceNumbers[piece] - 1];
    //         // delete last element in this row of PL
    //         pieceNumbers[piece]--;
    //         return true;
    //     }
    // }
    return false;
}

std::string Board::getAsciiBoard(bool showCoords) {
    // Sort by x coords
    std::sort(m_tiles.begin(), m_tiles.end(), [](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        return _t1->m_coords.first < _t2->m_coords.first;
    });
    // FIXME: temporary hack to get minimum.
    const short MIN_X = m_tiles.front()->m_coords.first;

    // Sort by y coords. Note that stablity is needed to ensure x elements are not scrambled.
    std::stable_sort(m_tiles.begin(), m_tiles.end(), [](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        return _t1->m_coords.second < _t2->m_coords.second;
    });
    const short MIN_Y = m_tiles.front()->m_coords.second;

    std::vector<std::string> lines; // each element is line of output
    const unsigned short V_SIZE = 1u;
    const std::string H_SEP = "---";
    const std::string V_SEP = "|";
    const std::string CORNER = "+";

    const std::string H_FILL = "   ";
    const std::string H_PAD = " ";
    const std::string H_NON_SEP = "   ";

    for (int i = 0; i < 2 + V_SIZE; i++) { // +2 for V_SEP on both sides
        lines.push_back("");
    }
    short lastY = MIN_Y;
    short currentX = MIN_X;
    size_t activeLineNum = 0;
    size_t activeLineCursorPos = 0;
    for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
        if ((*tilesIter)->m_coords.second != lastY) { // We've started the next line
            lastY = (*tilesIter)->m_coords.second;
            currentX = MIN_X;
            for (int i = 0; i < 1 + V_SIZE; i++) { // +1 for V_SEP
                activeLineNum++;
                lines.push_back("");
            }

            activeLineCursorPos = 0;
        }
        size_t lowerLine = activeLineNum + V_SIZE + 1;

        // Move along to our X position
        while ((*tilesIter)->m_coords.first != currentX++) {
            for (int i = 1; i <= V_SIZE; i++) {
                lines[activeLineNum + i] += ((*tilesIter)->HasAdjacent(LEFT) ? "" : " ") + H_FILL;
            }
            lines[lowerLine] += ((*tilesIter)->HasAdjacent(LEFT) ? "" : " ") + H_NON_SEP;
        }

        // Place our square
        for (int i = 1; i <= V_SIZE; i++) {
            if (i == V_SIZE / 2 + 1) {
                lines[activeLineNum + i] += ((*tilesIter)->HasAdjacent(LEFT) ? "" : V_SEP) + H_PAD + getCharFromEnum((*tilesIter)->m_contents, '_') + H_PAD + V_SEP;
            } else {
                lines[activeLineNum + i] += ((*tilesIter)->HasAdjacent(LEFT) ? "" : V_SEP) + H_FILL + V_SEP;
            }
        }
        lines[lowerLine] += ((*tilesIter)->HasAdjacent(LEFT) ? "" : CORNER) + H_SEP + CORNER;

        // Place upper line. This one is tricky bcz it is also the lower line of the previous one.
        // Fill in the line if needed so we can just run the same replacement
        for (int i = lines[activeLineNum + 1].size() - lines[activeLineNum].size(); i > 0; i--) {
            lines[activeLineNum] += " ";
        }

        size_t square_length = H_SEP.size() + 2;
        size_t starting = lines[activeLineNum + 1].size() - square_length;
        lines[activeLineNum].replace(starting, square_length, CORNER + H_SEP + CORNER);

    }

    std::string result = "";
    for (auto& line : lines) {
        result += "$  " + line + "\n";
    }
    return result;
}
