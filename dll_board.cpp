#include "dll_board.h"
#include "constants.h"
#include "utils.h"
#include "move.h"

#include <cstdint>
#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>
#include <ctype.h>

// FIXME: clear up coordinates naming convention. XY is ambiguous, should switch to rank-file.

void Tile::SetAdjacent(DirectionEnum _dir, Tile* _adj) {
    // break old connection if there is one
    if (m_adjacents[_dir] != nullptr) {
        m_adjacents[_dir]->m_adjacents[flipDirection(_dir)] = nullptr;
    }
    // update our side of the connection
    m_adjacents[_dir] = _adj;
    // create their side of the connection
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


DLLBoard::DLLBoard() { }
DLLBoard::DLLBoard(const std::string _sfen) : DLLBoard() {
    init(_sfen);
}
void DLLBoard::init(const std::string _sfen) {
    // delete old info, in case init called multiple times.
    m_tiles.clear();
    for (auto tileVector : m_extremaTiles) {
        tileVector.clear();
    }

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
            // If it is the edge case where there is no numbers, i.e. "()", we can skip this part
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
                newTile->SetAdjacent(LEFT, getTile(std::make_pair(currentFR.first - 1, currentFR.second), true));
                newTile->SetAdjacent(UP, getTile(std::make_pair(currentFR.first, currentFR.second + 1), true));
                m_tiles.push_back(newTile);
                currentFR.first++;
            }
            // dout << "Empty tiles added" << std::endl;
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        // dout << "This tile is piece #" << (int)thisTile << std::endl;
        if (thisTile != INVALID) {
            Tile* newTile = new Tile(thisTile, currentFR);
            updateExtrema(currentFR);
            newTile->SetAdjacent(LEFT, getTile(std::make_pair(currentFR.first - 1, currentFR.second), true));
            newTile->SetAdjacent(UP, getTile(std::make_pair(currentFR.first, currentFR.second + 1), true));
            currentFR.first++;
            m_tiles.push_back(newTile);
            m_pieceLocations[thisTile].push_back(newTile);
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
    for (int i = 0; i < m_pieceLocations[_piece].size(); i++) { // loop for all pieces of type _piece
        if (m_pieceLocations[_piece][i] == _oldLocation) { // find the match
            m_pieceLocations[_piece][i] = _newLocation;
            return true;
        }
    }
    return false;
}

bool DLLBoard::removePieceFromPL(PieceEnum _piece, Tile* _location) {
    for (int i = 0; i < m_pieceLocations[_piece].size(); i++) { // loop for all pieces of type _piece
        if (m_pieceLocations[_piece][i] == _location) { // find the match
            m_pieceLocations[_piece].erase(m_pieceLocations[_piece].begin() + i);
            // // override current position with last element in this row of PL
            // m_pieceLocations[_piece][i] = m_pieceLocations[_piece][pieceNumbers[piece] - 1];
            // // delete last element in this row of PL
            // pieceNumbers[piece]--;
            return true;
        }
    }
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
            std::string labelString = intToLetters(xLabel);
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
    int currentY = getDimensions().second - 1;
    for (int i = 0; i < lines.size(); i++) {
        if (m_printSettings.m_showCoords) {
            std::string leftMargin = std::string(1, MARGIN_V_SEP) + " ";
            if (i % (m_printSettings.m_height+1) == m_printSettings.m_height / 2 + 1) {
                leftMargin += std::to_string(currentY--);
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

Tile* DLLBoard::getTile(Coords _coords, bool _useInternal) {
    if (!_useInternal) {
        _coords.first += m_minCoords.first;
        _coords.second += m_minCoords.second;
    }
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

bool DLLBoard::apply(std::shared_ptr<Move> _move) {
    switch (_move->m_type) {
    case PIECE_MOVE:
        return apply(std::static_pointer_cast<PieceMove>(_move));
    case TILE_MOVE:
        return apply(std::static_pointer_cast<TileMove>(_move));
    default:
        dout << "FEATURE NOT IMPLEMENTED YET. Unknown Move Type [" << _move->m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}
bool DLLBoard::undo(std::shared_ptr<Move> _move) {
    switch (_move->m_type) {
    case PIECE_MOVE:
        return undo(std::static_pointer_cast<PieceMove>(_move));
    case TILE_MOVE:
        return undo(std::static_pointer_cast<TileMove>(_move));
    default:
        dout << "FEATURE NOT IMPLEMENTED YET. Unknown Move Type [" << _move->m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}

bool DLLBoard::apply(std::shared_ptr<PieceMove> _move) {
    // tdout << "applying move " << _move.algebraic() << std::endl;
    Tile* startTile = getTile(_move->m_startPos, false);
    Tile* endTile = getTile(_move->m_endPos, false);

    // Check this is valid
    if (startTile == nullptr || endTile == nullptr) {
        dout << "# INVALID MOVE, TILE MISSING " << _move->algebraic() << std::endl;
        return false;
    }
    if (endTile->m_contents != _move->m_capture) {
        dout << "# INVALID MOVE, CAPTURE MISMATCH " << _move->algebraic() << std::endl;
        return false;
    }
    // Execute the move
    if (isPiece(endTile->m_contents)) {
        removePieceFromPL(endTile->m_contents, endTile);
    }
    endTile->m_contents = startTile->m_contents;
    updatePieceInPL(startTile->m_contents, startTile, endTile);
    startTile->m_contents = EMPTY;
    return true;
};

bool DLLBoard::undo(std::shared_ptr<PieceMove> _move) {
    // tdout << "undoing move " << _move.algebraic() << std::endl;
    Tile* startTile = getTile(_move->m_startPos, false);
    Tile* endTile = getTile(_move->m_endPos, false);

    // Check this is valid
    if (startTile == nullptr || endTile == nullptr) {
        dout << "# INVALID UNDO, TILE MISSING " << _move->algebraic() << std::endl;
        return false;
    }
    if (startTile->m_contents != EMPTY) {
        dout << "# INVALID UNDO, MOVING FROM OCCUPIED SQUARE " << _move->algebraic() << std::endl;
        return false;
    }
    // Execute the undo
    updatePieceInPL(endTile->m_contents, endTile, startTile);
    startTile->m_contents = endTile->m_contents;
    if (isPiece(_move->m_capture)) {
        addPieceToPL(_move->m_capture, endTile);
    }
    endTile->m_contents = _move->m_capture;
    return true;
};

bool DLLBoard::apply(std::shared_ptr<TileMove> _move) {
    tdout << "applying move " << _move->algebraic() << std::endl;
    std::vector<Tile*> selection;

    // Get the Internal Coords (IC) of the range of tiles in this selection
    Coords moveSelFirstIC = _move->m_selFirst + m_minCoords;
    Coords moveSelSecondIC = _move->m_selSecond + m_minCoords;

    for (Tile* tile : m_tiles) {
        if ( // check if this tile is in the selection
            // compare first coord
            !coordGreaterThan(moveSelFirstIC.first, tile->m_coords.first, m_minCoords.first) && // selFirst <= tile
            !coordGreaterThan(tile->m_coords.first, moveSelSecondIC.first, m_minCoords.first) && // tile <= selSecond
            // compare second coord
            !coordGreaterThan(moveSelFirstIC.second, tile->m_coords.second, m_minCoords.second) && // selFirst <= tile
            !coordGreaterThan(tile->m_coords.second, moveSelSecondIC.second, m_minCoords.second) // tile <= selSecond
        ) {
            tdout << "selection includes " << tile->m_coords.first << ", " << tile->m_coords.second << std::endl;
            selection.push_back(tile);
        }
    }

    // FIXME: does not update extrema in the case that we remove the last extrema tile. Need to keep a list of all extrema tiles.


    for (Tile* tile : selection) {
        // Get displacement of move
        tile->m_coords += _move->m_translation;

    // FIXME: complete this section which checks if the new location of the tile updates the extrema. Probably should merge with updateExtrema function.
    //     // check if this is a new extrema
    //     // check first coord
    //     if (_move->m_translation.first < 0) { // left move
            
    //     } else if (_move->m_translation.first > 0) { // right move
    //         if (coordGreaterThan(tile->m_coords.first, m_maxCoords.first, m_minCoords.first)) {
    //             m_maxCoords.first = tile->m_coords.first;
    //         }
    //     }


        // break old connections on edge pieces and add new ones
        if (tile->m_coords.first == moveSelFirstIC.first) {
            tile->SetAdjacent(LEFT, getTile(tile->m_coords + DIRECTION_SIGNS[LEFT], true)); // TODO: there must be a more efficient way of getting these new adjacencies, right?
        }
        if (tile->m_coords.first == moveSelSecondIC.first) {
            tile->SetAdjacent(RIGHT, getTile(tile->m_coords + DIRECTION_SIGNS[RIGHT], true));
        }
        if (tile->m_coords.second == moveSelFirstIC.second) {
            tile->SetAdjacent(DOWN, getTile(tile->m_coords + DIRECTION_SIGNS[DOWN], true));
        }
        if (tile->m_coords.second == moveSelSecondIC.second) {
            tile->SetAdjacent(UP, getTile(tile->m_coords + DIRECTION_SIGNS[UP], true));
        }
    }
    // TODO: update m_minCoords and m_maxCoords
    // TODO: create some way of checking if all connections are valid

    return true;
};

bool DLLBoard::undo(std::shared_ptr<TileMove> _move) {
    //TODO:
    return false;
}

int DLLBoard::staticEvaluation() {
    //TODO: this is just a dumb implementation to test if minmax works. Find a better implementation in the future.
    int staticValue = 0;
    for (int i = 1; i < NUM_PIECE_TYPES*2+1; i++) {
        staticValue += PIECE_VALUES[i] * m_pieceLocations[i].size();
        for (Tile* t : m_pieceLocations[i]) {
            // add 20 centipawns for being on 'even' coordinates
            if ((t->m_coords.first + t->m_coords.second) % 2 == 0) {
                staticValue += 20 * (isWhite(i) ? 1 : -1); // negate if black
            }
        }
    }
    return staticValue;
}

std::vector<std::unique_ptr<Move>> DLLBoard::getMoves(PieceColor _color) {
    // TODO: assumes all pieces can move 1 tile forward, back, left, or right, for the sake of testing minmax.
    std::vector<std::unique_ptr<Move>> legalMoves;
    for (int pieceType = (_color==WHITE ? W_PAWN : B_PAWN); pieceType < NUM_PIECE_TYPES*2+1; pieceType+=2) { // iterate over pieces of _color in piece list
        for (Tile* t : m_pieceLocations[pieceType]) { // for all tiles of pieces of this type
            for (int direction = LEFT; direction <= DOWN; direction++) { // iterate over 4 directions
                if (t->HasAdjacent(direction)) {
                    // check if empty
                    if (t->m_adjacents[direction]->m_contents == EMPTY) {
                        std::unique_ptr<PieceMove> newMove (new PieceMove(externalCoords(t), externalCoords(t->m_adjacents[direction])));
                        legalMoves.push_back(std::move(newMove));
                    // check if capture
                    } else if (isPiece(t->m_adjacents[direction]->m_contents) && (isWhite(t->m_contents) != isWhite(t->m_adjacents[direction]->m_contents))) {
                        std::unique_ptr<PieceMove> newMove (new PieceMove(externalCoords(t), externalCoords(t->m_adjacents[direction])));
                        newMove->m_capture = t->m_adjacents[direction]->m_contents;
                        legalMoves.push_back(std::move(newMove));
                    }
                }
            }
        }
    }
    return legalMoves;
}
