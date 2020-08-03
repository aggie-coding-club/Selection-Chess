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
    // std::cout << "SD _dir:" << getCharFromDirection(_dir) << std::endl;
    if (_adj != nullptr) {
        // std::cout << "SD flipped _dir:" << getCharFromDirection(flipDirection(_dir)) << std::endl;
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
bool Board::operator==(const Board& _other) const {
    //TODO: implement
    return true;
}

bool Board::updatePieceInPL(PieceEnum _piece, Tile* _oldLocation, Tile* _newLocation) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == oldLocation) { // find the match
    //         pieceLocations[piece][i] = newLocation;
    //         return true;
    //     }
    // }
    return false;
}

bool Board::removePieceFromPL(PieceEnum _piece, Tile* _location) {
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

std::string Board::getAsciiBoard(bool _showCoords, size_t _width, size_t _height, char _tileFillChar) {
    // Sort by x coords
    std::sort(m_tiles.begin(), m_tiles.end(), [](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        return _t1->m_coords.first < _t2->m_coords.first;
    });
    // FIXME: temporary hack to get minimum.
    const short MIN_X = m_tiles.front()->m_coords.first;
    const short MAX_X = m_tiles.back()->m_coords.first;

    // Sort by y coords. Note that stablity is needed to ensure x elements are not scrambled.
    std::stable_sort(m_tiles.begin(), m_tiles.end(), [](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        return _t1->m_coords.second < _t2->m_coords.second;
    });
    const short MIN_Y = m_tiles.front()->m_coords.second;

    std::vector<std::string> lines; // each element is line of output

    // DRAWING CONTROLS
    const std::string V_SEP = "|"; // sides of tile, repeated _height times
    const std::string CORNER = "+"; // corner where h_sep and V_SEP meet

    std::string h_sep = ""; // top/bottom of tiles. Must be odd since piece must be centered.
    std::string h_fill_out = ""; // filler used for outside of cell. Must be same size as h_sep
    std::string h_fill_in = ""; // filler used for outside of cell. Must be same size as h_sep
    std::string h_non_sep = ""; // Used like an h_sep but for when there is no adjacent cell
    for (int i = 0; i < _width; i++) {
        h_sep += "-";
        h_fill_out += " ";
        h_fill_in += _tileFillChar;
        h_non_sep += " ";
    }
    std::string h_pad_left = std::string((_width - 1) / 2, _tileFillChar); // Used between V_SEP and the piece character. Sizes must satisfy h_sep = h_pad + 1 + h_pad.
    std::string h_pad_right = std::string(_width / 2, _tileFillChar);

    // Margin drawing controls //TODO: parameterize?
    const unsigned short LEFT_MARGIN_SIZE = 5; // width of left margin
    const unsigned short LEFT_MARGIN_PAD = 3; // space between left margin and leftmost tiles
    const char MARGIN_V_SEP = '|'; // vertical boundary for margin
    const char MARGIN_H_SEP = '='; // horizontal boundary for margin
    const char MARGIN_H_LABEL_DIV = ' '; // What separates the labels on the x axis from eachother

    for (int i = 0; i < 2 + _height; i++) { // +2 for V_SEP on both sides
        lines.push_back("");
    }
    short lastY = MIN_Y; // Which Y position the previous tile we printed is in. Keeps track if we start on a new row.
    short currentX = MIN_X; // Which X position this tile correspond to. Keeps track if we need to print empty cells.
    size_t activeLineNum = 0; // First line of output we are currently modifying
    bool trailingEdge = false; // Whether the last tile of this row already has printed the edge we share with it
    for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
        // We've started the next line
        if ((*tilesIter)->m_coords.second != lastY) {
            lastY = (*tilesIter)->m_coords.second;
            currentX = MIN_X;
            trailingEdge = false;
            for (int i = 0; i < 1 + _height; i++) { // +1 for V_SEP
                activeLineNum++;
                lines.push_back("");
            }
        }
        size_t lowerLine = activeLineNum + _height + 1;

        // Move along to our X position if needed, printing empty space as we go
        while ((*tilesIter)->m_coords.first != currentX++) {
            for (int i = 1; i <= _height; i++) {
                lines[activeLineNum + i] += (trailingEdge ? "" : " ") + h_fill_out;
            }
            lines[lowerLine] += (trailingEdge ? "" : " ") + h_non_sep;
            // We did not print righthand edge in case our neighbor is a tile and needs to print its edge there
            trailingEdge = false;
        }

        // Output our tile's bottom border and center.
        for (int i = 1; i <= _height; i++) {
            if (i == _height / 2 + 1) { // Is this the row which contains the piece
                lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_pad_left + getCharFromEnum((*tilesIter)->m_contents, _tileFillChar) + h_pad_right + V_SEP;
            } else {
                lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_fill_in + V_SEP;
            }
        }
        lines[lowerLine] += (trailingEdge ? "" : CORNER) + h_sep + CORNER;

        // Place upper line of our tile. This one is tricky bcz it is also the lower line of the previous row.
        // Fill in the line if needed so we can just run the same replacement and not worry about out-of-bounds
        for (int i = lines[activeLineNum + 1].size() - lines[activeLineNum].size(); i > 0; i--) {
            lines[activeLineNum] += " ";
        }
        // Override upper border with an edge, whether it is empty or there is already a tile with a lower edge there.
        size_t starting = lines[activeLineNum + 1].size() - (_width + 2);
        lines[activeLineNum].replace(starting, (_width + 2), CORNER + h_sep + CORNER);

        // We printed the righthand edge
        trailingEdge = true;
    }

    std::string result = "";
    std::string dividerLine;

    // Add stuff to top of output;
    if (_showCoords) {
        // the cornerpiece
        result += MARGIN_V_SEP;
        result += std::string(LEFT_MARGIN_SIZE - 1, ' ');
        result += MARGIN_V_SEP;

        // pad before labels start
        result += std::string(LEFT_MARGIN_PAD, ' ');

        // labels
        result += MARGIN_H_LABEL_DIV;
        for (auto xLabel = MIN_X; xLabel != MAX_X + 1; xLabel++) {
            std::string labelString = std::to_string(xLabel);
            while (labelString.size() < _width) {
                labelString += " "; // label filler
            }
            result += labelString + MARGIN_H_LABEL_DIV;
        }

        // long horizontal line
        dividerLine = std::string(result.size(), MARGIN_H_SEP);
        result = dividerLine + "\n" + result + "\n" + dividerLine + "\n";
    }
    // Add stuff to left side of output
    short currentY = MIN_Y;
    for (int i = 0; i < lines.size(); i++) {
        if (_showCoords) {
            std::string leftMargin = std::string(1, MARGIN_V_SEP) + " ";
            if (i % (_height+1) == _height / 2 + 1) {
                leftMargin += std::to_string(currentY++);
            }
            while (leftMargin.size() < LEFT_MARGIN_SIZE) {
                leftMargin += " ";
            }
            leftMargin += MARGIN_V_SEP;
            leftMargin += std::string(LEFT_MARGIN_PAD, ' ');
            result += leftMargin;
        }
        result += lines[i] + "\n";
    }
    if (_showCoords) {
        result += dividerLine + "\n";
    }
    return result;
}

Tile* Board::getTile(std::pair<short, short> _coords) {
    for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
        if ((*tilesIter)->m_coords == _coords) return *tilesIter;
    }
    std::cout << "returned nullptr" << std::endl;
    return nullptr;
}
