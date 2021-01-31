#include <cstdint>
#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>
#include <ctype.h>
#include <limits.h>

#include "constants.h"
#include "utils.h"

#include "array_board.h"

ArrayBoard::ArrayBoard(const std::string _sfen) {
    m_movesSinceLastCapture = 0;
    // Parse position section (until first space)
    dout << "Read board as: ";
    int i; // which character of _sfen we are on

    // ----------- loop through and count number of tiles ----------- //
    m_grid_size = 0;
    for (i = 0; i < _sfen.length() && _sfen[i] != ' '; i++) {
        dout << "Reading next character as [";
        const char c = _sfen[i];
        if (c == '/') { // Next row
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
            dout << num_empty_tiles << " empty tiles ";
            m_grid_size += num_empty_tiles;
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        if (thisTile != INVALID) {
            m_grid_size++;
            continue;
        } else {
            std::cerr << "Invalid piece symbol '" << c << "' in SFen!" << std::endl;
            throw "Invalid piece symbol in SFen!";
        }
    }
    dout << "Done counting." << std::endl;
    // check for multiplication overflow
    if (m_grid_size > SIZE_MAX / m_grid_size) {
        std::cerr << "Board is too large to be stored as ArrayBoard!" << std::endl;
        throw "Board is too large!";
    }
    m_grid = new PieceEnum[m_grid_size*m_grid_size];

    // ----------- loop through again to initialize the grid ----------- //
    const short STARTING_X = 0;
    const short STARTING_Y = 0;
    short currentX = STARTING_X; // Which coord we are currently on
    short currentY = STARTING_Y;
    for (i = 0; i < _sfen.length() && _sfen[i] != ' '; i++) {
        dout << "Reading next character as [";
        const char c = _sfen[i];
        dout << c << "] for position (" << currentX << ", " << currentY << ")" << std::endl;
        if (c == '/') { // Next row
            dout << "Starting next line" << std::endl;
            currentX = STARTING_X;
            currentY++; //FIXME: is anything weird going to happen here when we implement wrapping?
            continue;
        }
        if (c == '(') { // Space(s) have no tile(s)
            int j;
            // set j to be the next non-numeric character
            for (j = i+1; isdigit(_sfen[j]); j++);
            // If it is the ege case where there is no numbers, i.e. "()", we can skip this part
            if (j != i+1) {
                // Get the next characters as integer
                int num_no_tiles = std::stoi(_sfen.substr(i+1, j)); //i+1 to ignore '('
                dout << num_no_tiles << " no tiles ";
                currentX += num_no_tiles;
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

            dout << num_empty_tiles << " empty tiles ";
            for (int k = 0; k < num_empty_tiles; k++) {
                m_grid[currentX + currentY*m_grid_size] = EMPTY;
                currentX++;
            }
            dout << "Empty tiles added" << std::endl;
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        dout << "This tile is piece #" << (int)thisTile << std::endl;
        m_grid[currentX + currentY*m_grid_size] = thisTile;
        currentX++;
    }
    dout << "Done parsing." << std::endl;
    // Parse remaining fields
    // TODO: implement
}

/** 
 * Boards are equal if all independent fields except moveHistory are equal. 
 * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
 * for most practical purposes. Really, this is just here to test if the hash function is working.
 */
bool ArrayBoard::operator==(const Board& _other) const {
    //TODO: implement
    return true;
}

bool ArrayBoard::updatePieceInPL(PieceEnum _piece, coords _oldLocation, coords _newLocation) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == oldLocation) { // find the match
    //         pieceLocations[piece][i] = newLocation;
    //         return true;
    //     }
    // }
    return false;
}

std::string ArrayBoard::getAsciiBoard(bool _showCoords, size_t _width, size_t _height, char _tileFillChar) { //TODO: make const
    // FIXME: probably just use the print created for an array, in the other branch
    // std::vector<std::string> lines; // each element is line of output

    // // DRAWING CONTROLS
    // const std::string V_SEP = "|"; // sides of tile, repeated _height times
    // const std::string CORNER = "+"; // corner where h_sep and V_SEP meet

    // std::string h_sep = ""; // top/bottom of tiles. Must be odd since piece must be centered.
    // std::string h_fill_out = ""; // filler used for outside of cell. Must be same size as h_sep
    // std::string h_fill_in = ""; // filler used for outside of cell. Must be same size as h_sep
    // std::string h_non_sep = ""; // Used like an h_sep but for when there is no adjacent cell
    // for (int i = 0; i < _width; i++) {
    //     h_sep += "-";
    //     h_fill_out += " ";
    //     h_fill_in += _tileFillChar;
    //     h_non_sep += " ";
    // }
    // std::string h_pad_left = std::string((_width - 1) / 2, _tileFillChar); // Used between V_SEP and the piece character. Sizes must satisfy h_sep = h_pad + 1 + h_pad.
    // std::string h_pad_right = std::string(_width / 2, _tileFillChar);

    // // Margin drawing controls //TODO: parameterize?
    // const unsigned short LEFT_MARGIN_SIZE = 5; // width of left margin
    // const unsigned short LEFT_MARGIN_PAD = 3; // space between left margin and leftmost tiles
    // const char MARGIN_V_SEP = '|'; // vertical boundary for margin
    // const char MARGIN_H_SEP = '='; // horizontal boundary for margin
    // const char MARGIN_H_LABEL_DIV = ' '; // What separates the labels on the x axis from eachother

    // for (int i = 0; i < 2 + _height; i++) { // +2 for V_SEP on both sides
    //     lines.push_back("");
    // }
    // short lastY = MIN_Y; // Which Y position the previous tile we printed is in. Keeps track if we start on a new row.
    // short currentX = MIN_X; // Which X position this tile correspond to. Keeps track if we need to print empty cells.
    // size_t activeLineNum = 0; // First line of output we are currently modifying
    // bool trailingEdge = false; // Whether the last tile of this row already has printed the edge we share with it
    // for (auto tilesIter = m_tiles.begin(); tilesIter != m_tiles.end(); tilesIter++) {
    //     // We've started the next line
    //     if ((*tilesIter)->m_coords.second != lastY) {
    //         lastY = (*tilesIter)->m_coords.second;
    //         currentX = MIN_X;
    //         trailingEdge = false;
    //         for (int i = 0; i < 1 + _height; i++) { // +1 for V_SEP
    //             activeLineNum++;
    //             lines.push_back("");
    //         }
    //     }
    //     size_t lowerLine = activeLineNum + _height + 1;

    //     // Move along to our X position if needed, printing empty space as we go
    //     while ((*tilesIter)->m_coords.first != currentX++) {
    //         for (int i = 1; i <= _height; i++) {
    //             lines[activeLineNum + i] += (trailingEdge ? "" : " ") + h_fill_out;
    //         }
    //         lines[lowerLine] += (trailingEdge ? "" : " ") + h_non_sep;
    //         // We did not print righthand edge in case our neighbor is a tile and needs to print its edge there
    //         trailingEdge = false;
    //     }

    //     // Output our tile's bottom border and center.
    //     for (int i = 1; i <= _height; i++) {
    //         if (i == _height / 2 + 1) { // Is this the row which contains the piece
    //             lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_pad_left + getCharFromPiece((*tilesIter)->m_contents, _tileFillChar) + h_pad_right + V_SEP;
    //         } else {
    //             lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_fill_in + V_SEP;
    //         }
    //     }
    //     lines[lowerLine] += (trailingEdge ? "" : CORNER) + h_sep + CORNER;

    //     // Place upper line of our tile. This one is tricky bcz it is also the lower line of the previous row.
    //     // Fill in the line if needed so we can just run the same replacement and not worry about out-of-bounds
    //     for (int i = lines[activeLineNum + 1].size() - lines[activeLineNum].size(); i > 0; i--) {
    //         lines[activeLineNum] += " ";
    //     }
    //     // Override upper border with an edge, whether it is empty or there is already a tile with a lower edge there.
    //     size_t starting = lines[activeLineNum + 1].size() - (_width + 2);
    //     lines[activeLineNum].replace(starting, (_width + 2), CORNER + h_sep + CORNER);

    //     // We printed the righthand edge
    //     trailingEdge = true;
    // }

    std::string result = "";
    // std::string dividerLine;

    // // Add stuff to top of output;
    // if (_showCoords) {
    //     // the cornerpiece
    //     result += MARGIN_V_SEP;
    //     result += std::string(LEFT_MARGIN_SIZE - 1, ' ');
    //     result += MARGIN_V_SEP;

    //     // pad before labels start
    //     result += std::string(LEFT_MARGIN_PAD, ' ');

    //     // labels
    //     result += MARGIN_H_LABEL_DIV;
    //     for (auto xLabel = MIN_X; xLabel != MAX_X + 1; xLabel++) {
    //         std::string labelString = std::to_string(xLabel);
    //         while (labelString.size() < _width) {
    //             labelString += " "; // label filler
    //         }
    //         result += labelString + MARGIN_H_LABEL_DIV;
    //     }

    //     // long horizontal line
    //     dividerLine = std::string(result.size(), MARGIN_H_SEP);
    //     result = dividerLine + "\n" + result + "\n" + dividerLine + "\n";
    // }
    // // Add stuff to left side of output
    // short currentY = MIN_Y;
    // for (int i = 0; i < lines.size(); i++) {
    //     if (_showCoords) {
    //         std::string leftMargin = std::string(1, MARGIN_V_SEP) + " ";
    //         if (i % (_height+1) == _height / 2 + 1) {
    //             leftMargin += std::to_string(currentY++);
    //         }
    //         while (leftMargin.size() < LEFT_MARGIN_SIZE) {
    //             leftMargin += " ";
    //         }
    //         leftMargin += MARGIN_V_SEP;
    //         leftMargin += std::string(LEFT_MARGIN_PAD, ' ');
    //         result += leftMargin;
    //     }
    //     result += lines[i] + "\n";
    // }
    // if (_showCoords) {
    //     result += dividerLine + "\n";
    // }
    // dout << "Length of printable string = " << result.length() << std::endl;
    return result;
}
