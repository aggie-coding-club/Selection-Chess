#include <cstdint>
#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>
#include <ctype.h>

#include "constants.h"
#include "utils.h"

#include "dll_board.h"

// FIXME: clear up coordinates naming convention. XY is ambiguous, should switch to rank-file.

void Tile::SetAdjacent(DirectionEnum _dir, Tile* _adj) {
    m_adjacents[_dir] = _adj;
    if (_adj != nullptr) {
        _adj->m_adjacents[flipDirection(_dir)] = this;
    }
}

Tile::Tile(PieceEnum _contents, Coords _coords) {
    m_contents = _contents;
    m_coords = _coords;
    for (int i = 0; i < NUM_ADJACENT_DIRECTIONS; i++) {
        m_adjacents[i] = nullptr;
    }
};

void DLLBoard::sortByCoords(bool _priorityRank, bool _reverseFile, bool _reverseRank) {
    // Sort by low priority
    std::sort(m_tiles.begin(), m_tiles.end(), [&](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        if (_priorityRank) {
            return compareTileByFile(_t1, _t2, _reverseFile);
        } else {
            return compareTileByRank(_t1, _t2, _reverseRank);
        }
    });

    // Sort by high priority. Note that stablity is needed to ensure low priority is not scrambled.
    std::stable_sort(m_tiles.begin(), m_tiles.end(), [&](Tile* _t1, Tile* _t2) {
        if (_t1 == nullptr || _t2 == nullptr) return false;
        if (_priorityRank) {
            return compareTileByRank(_t1, _t2, _reverseRank);
        } else {
            return compareTileByFile(_t1, _t2, _reverseFile);
        }
    });
}


DLLBoard::DLLBoard() { 
    m_movesSinceLastCapture = 0; // TODO: should be set by SFEN instead of initialized here
}
DLLBoard::DLLBoard(const std::string _sfen) {
    DLLBoard();
    init(_sfen);
}
void DLLBoard::init(const std::string _sfen) {
    // delete old info, in case init called multiple times.
    m_tiles.clear();

    // Parse position section (until first space)

    // Define starting corner like this so we don't have to fuss with wrap around.
    // The rank coord is -1 because we FEN reads rank in backwards, and -1 of unsigned is the max.
    const Coords STARTING_CORNER = std::make_pair(0,-1);
    dout << "STARTING_CORNER is " << STARTING_CORNER.first << ", " << STARTING_CORNER.second << std::endl;

    // Set extrema so that they are guaranteed to update
    m_minCoords = std::make_pair(-1, -1);
    m_maxCoords = std::make_pair(0, 0);

    Coords currentFR = STARTING_CORNER;
    int i = 0; // which character of _sfen we are on
    for (; i < _sfen.length() && _sfen[i] != ' '; i++) {
        // dout << "Reading next character as [";
        const char c = _sfen[i];
        // dout << c << "] for position (" << currentF << ", " << currentFR.second << ")" << std::endl;
        if (c == '/') { // Next row
            // dout << "Starting next line" << std::endl;
            currentFR.first = STARTING_CORNER.first;
            currentFR.second--;
            continue;
        }
        if (c == '(') { // Space(s) have no tile(s)
            int j;
            // set j to be the next non-numeric character
            for (j = i+1; isdigit(_sfen[j]); j++);
            // make sure that it has this closing paren
            if (_sfen[j] != ')') {
                std::cerr << "Expected ')' after non tile sequence in SFen!" << std::endl;
                throw "Expected ')' after non tile sequence in SFen!";
            }
            // If it is the ege case where there is no numbers, i.e. "()", we can skip this part
            if (j != i+1) {
                // Get the next characters as integer
                int num_no_tiles = std::stoi(_sfen.substr(i+1, j)); //i+1 to ignore '('
                // dout << num_no_tiles << " no tiles ";
                currentFR.first += num_no_tiles;
            }
            // update i to to account for the number of additional characters we read in
            i = j;
            continue;
        }
        if (isdigit(c)) { // Spaces are empty tiles
            int j;
            // set j to be the next non-numeric character
            for (j = i+1; isdigit(_sfen[j]); j++);
            // Get the next characters as integer
            int num_empty_tiles = std::stoi(_sfen.substr(i, j));
            // update i to to account for the number of additional characters we read in
            i = j-1;

            // dout << num_empty_tiles << " empty tiles ";
            for (int k = 0; k < num_empty_tiles; k++) {
                Tile* newTile = new Tile(EMPTY, currentFR);
                updateExtrema(currentFR);
                currentFR.first++;
                newTile->SetAdjacent(LEFT, getTile(std::make_pair(currentFR.first - 1, currentFR.second)));
                newTile->SetAdjacent(UP, getTile(std::make_pair(currentFR.first, currentFR.second + 1)));
                m_tiles.push_back(newTile);
            }
            // dout << "Empty tiles added" << std::endl;
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        // dout << "This tile is piece #" << (int)thisTile << std::endl;
        if (thisTile != INVALID) {
            Tile* newTile = new Tile(thisTile, currentFR);
            updateExtrema(currentFR);
            currentFR.first++;
            newTile->SetAdjacent(LEFT, getTile(std::make_pair(currentFR.first - 1, currentFR.second)));
            newTile->SetAdjacent(UP, getTile(std::make_pair(currentFR.first, currentFR.second + 1)));
            m_tiles.push_back(newTile);
            continue;
        } else {
            std::cerr << "Invalid piece symbol '" << c << "' in SFen!" << std::endl;
            throw "Invalid piece symbol in SFen!";
        }
    }
    dout << "Done parsing FEN." << std::endl;
    // Parse remaining fields
    // TODO: implement
}

/** 
 * Boards are equal if all independent fields except moveHistory are equal. 
 * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
 * for most practical purposes. Really, this is just here to test if the hash function is working.
 */
bool DLLBoard::operator==(const Board& _other) const {
    //TODO: implement
    return true;
}

bool DLLBoard::updatePieceInPL(PieceEnum _piece, Tile* _oldLocation, Tile* _newLocation) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == oldLocation) { // find the match
    //         pieceLocations[piece][i] = newLocation;
    //         return true;
    //     }
    // }
    return false;
}

bool DLLBoard::removePieceFromPL(PieceEnum _piece, Tile* _location) {
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

std::string DLLBoard::toSfen() {
    if (m_tiles.empty()) {
        return "[Empty board]"; // TODO: [Very Low Priority] There is probably a nicer way to display this. Probably should still to show the whole frame and everything.
    }
    sortByCoords(true, false, true);

    std::string sfen = ""; // returned result

    Coords lastCoords = std::make_pair(m_minCoords.first, m_maxCoords.second); // Starting at corner
    unsigned int numOfEmpty = 0;

    // function to improve code readability
    auto endEmptySequence = [&]() {
        if (numOfEmpty != 0) {
            sfen += std::to_string(numOfEmpty);
            numOfEmpty = 0;
        }
    };

    for (Tile* tile : m_tiles) {
        int fileDiff = tile->m_coords.first - lastCoords.first - 1; // size of gap between tiles. -1 to avoid edge case with overflow later on
        int rankDiff = lastCoords.second - tile->m_coords.second; // Difference in rank. Note this is negated because rank is done in reverse
        for (int i = 0; i < rankDiff; i++) { // this is start of new row
            endEmptySequence();
            sfen += "/";
            fileDiff = tile->m_coords.first - m_minCoords.first; // reset since this is new row
        }
        if (fileDiff > 0) {
            endEmptySequence();
            sfen += "(" + std::to_string(fileDiff) + ")";
        }
        if (tile->m_contents == EMPTY) {
            numOfEmpty++;
        } else {
            endEmptySequence();
        }
        if (isPiece(tile->m_contents)) {
            sfen += PIECE_LETTERS[tile->m_contents];
        }
        lastCoords = tile->m_coords;
    }

    return sfen;
}

std::string DLLBoard::getAsciiBoard() {
    if (m_tiles.empty()) {
        return "[Empty board]"; // TODO: [Very Low Priority] There is probably a nicer way to display this. Probably should still to show the whole frame and everything.
    }
    sortByCoords(true, false, true);

    std::vector<std::string> lines; // each element is line of output

    // DRAWING CONTROLS
    const std::string V_SEP = "|"; // sides of tile, repeated m_printSettings.m_height times
    const std::string CORNER = "+"; // corner where h_sep and V_SEP meet

    std::string h_sep = ""; // top/bottom of tiles. Must be odd since piece must be centered.
    std::string h_fill_out = ""; // filler used for outside of cell. Must be same size as h_sep
    std::string h_fill_in = ""; // filler used for outside of cell. Must be same size as h_sep
    std::string h_non_sep = ""; // Used like an h_sep but for when there is no adjacent cell
    for (int i = 0; i < m_printSettings.m_width; i++) {
        h_sep += "-";
        h_fill_out += " ";
        h_fill_in += m_printSettings.m_tileFillChar;
        h_non_sep += " ";
    }
    std::string h_pad_left = std::string((m_printSettings.m_width - 1) / 2, m_printSettings.m_tileFillChar); // Used between V_SEP and the piece character. Sizes must satisfy h_sep = h_pad + 1 + h_pad.
    std::string h_pad_right = std::string(m_printSettings.m_width / 2, m_printSettings.m_tileFillChar);

    // Margin drawing controls //TODO: parameterize?
    const unsigned int LEFT_MARGIN_SIZE = 5; // width of left margin
    const unsigned int LEFT_MARGIN_PAD = 3; // space between left margin and leftmost tiles
    const char MARGIN_V_SEP = '|'; // vertical boundary for margin
    const char MARGIN_H_SEP = '='; // horizontal boundary for margin
    const char MARGIN_H_LABEL_DIV = ' '; // What separates the labels on the x axis from eachother

    for (int i = 0; i < 2 + m_printSettings.m_height; i++) { // +2 for V_SEP on both sides
        lines.push_back("");
    }
    int lastY = m_maxCoords.second; // Which Y position the previous tile we printed is in. Keeps track if we start on a new row.
    int currentX = m_minCoords.first; // Which X position this tile correspond to. Keeps track if we need to print empty cells.
    size_t activeLineNum = 0; // First line of output we are currently modifying
    bool trailingEdge = false; // Whether the last tile of this row already has printed the edge we share with it
    for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
        // We've started the next line
        if ((*tilesIter)->m_coords.second != lastY) {
            lastY = (*tilesIter)->m_coords.second;
            currentX = m_minCoords.first;
            trailingEdge = false;
            for (int i = 0; i < 1 + m_printSettings.m_height; i++) { // +1 for V_SEP
                activeLineNum++;
                lines.push_back("");
            }
        }
        size_t lowerLine = activeLineNum + m_printSettings.m_height + 1;

        // Move along to our X position if needed, printing empty space as we go
        while ((*tilesIter)->m_coords.first != currentX++) {
            for (int i = 1; i <= m_printSettings.m_height; i++) {
                lines[activeLineNum + i] += (trailingEdge ? "" : " ") + h_fill_out;
            }
            lines[lowerLine] += (trailingEdge ? "" : " ") + h_non_sep;
            // We did not print righthand edge in case our neighbor is a tile and needs to print its edge there
            trailingEdge = false;
        }

        // Output our tile's bottom border and center.
        for (int i = 1; i <= m_printSettings.m_height; i++) {
            if (i == m_printSettings.m_height / 2 + 1) { // Is this the row which contains the piece
                lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_pad_left + getCharFromPiece((*tilesIter)->m_contents, m_printSettings.m_tileFillChar) + h_pad_right + V_SEP;
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
        size_t starting = lines[activeLineNum + 1].size() - (m_printSettings.m_width + 2);
        lines[activeLineNum].replace(starting, (m_printSettings.m_width + 2), CORNER + h_sep + CORNER);

        // We printed the righthand edge
        trailingEdge = true;
    }

    std::string result = "";
    std::string dividerLine;

    // Add stuff to top of output;
    if (m_printSettings.m_showCoords) {
        // the cornerpiece
        result += MARGIN_V_SEP;
        result += std::string(LEFT_MARGIN_SIZE - 1, ' ');
        result += MARGIN_V_SEP;

        // pad before labels start
        result += std::string(LEFT_MARGIN_PAD, ' ');

        // labels
        result += MARGIN_H_LABEL_DIV;
        for (auto xLabel = m_minCoords.first; xLabel != m_maxCoords.first + 1; xLabel++) {
            std::string labelString = std::to_string(xLabel);
            while (labelString.size() < m_printSettings.m_width) {
                labelString += " "; // label filler
            }
            result += labelString + MARGIN_H_LABEL_DIV;
        }

        // long horizontal line
        dividerLine = std::string(result.size(), MARGIN_H_SEP);
        result = dividerLine + "\n" + result + "\n" + dividerLine + "\n";
    }
    // Add stuff to left side of output
    int currentY = m_minCoords.second;
    for (int i = 0; i < lines.size(); i++) {
        if (m_printSettings.m_showCoords) {
            std::string leftMargin = std::string(1, MARGIN_V_SEP) + " ";
            if (i % (m_printSettings.m_height+1) == m_printSettings.m_height / 2 + 1) {
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
    if (m_printSettings.m_showCoords) {
        result += dividerLine + "\n";
    }
    dout << "Length of printable string = " << result.length() << std::endl;
    return result;
}

Tile* DLLBoard::getTile(Coords _coords) {
    for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
        if ((*tilesIter)->m_coords == _coords) return *tilesIter;
    }
    return nullptr;
}

void DLLBoard::updateExtrema(const Coords& _new) {
    m_minCoords.first = std::min(m_minCoords.first, _new.first);
    m_maxCoords.first = std::max(m_maxCoords.first, _new.first);
    m_minCoords.second = std::min(m_minCoords.second, _new.second);
    m_maxCoords.second = std::max(m_maxCoords.second, _new.second);
}
