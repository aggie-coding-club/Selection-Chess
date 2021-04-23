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
    m_minCoords = std::make_pair(0, 0);
    m_maxCoords = std::make_pair(0, 0); // the maxCoords will be updated as we create the board
    // Parse position section (until first space)
    int i; // which character of _sfen we are on

    // ----------- loop through and count number of tiles ----------- //
    m_grid_size = 0;
    for (i = 0; i < _sfen.length() && _sfen[i] != ' '; i++) {
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
    // dout << "Done counting." << std::endl;
    // check for multiplication overflow
    if (m_grid_size > SIZE_MAX / m_grid_size) {
        std::cerr << "Board is too large to be stored as ArrayBoard!" << std::endl;
        throw "Board is too large!";
    }
    m_grid = new PieceEnum[m_grid_size*m_grid_size];
    // initialize all to non-tiles
    for (size_t i = 0; i < m_grid_size*m_grid_size; i++) {
        m_grid[i] = INVALID;
    }

    // ----------- loop through again to initialize the grid ----------- //
    const size_t STARTING_X = 0;
    const size_t STARTING_Y = 0;
    size_t currentX = STARTING_X; // Which coord we are currently on
    size_t currentY = STARTING_Y;
    for (i = 0; i < _sfen.length() && _sfen[i] != ' '; i++) {
        const char c = _sfen[i];
        if (c == '/') { // Next row
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

            for (int k = 0; k < num_empty_tiles; k++) {
                m_grid[currentX + currentY*m_grid_size] = EMPTY;
                m_maxCoords.first = std::max(m_maxCoords.first, currentX);
                m_maxCoords.second = std::max(m_maxCoords.second, currentY);
                currentX++;
            }
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        m_grid[currentX + currentY*m_grid_size] = thisTile;
        m_maxCoords.first = std::max(m_maxCoords.first, currentX);
        m_maxCoords.second = std::max(m_maxCoords.second, currentY);
        currentX++;
    }
    // Parse remaining fields
    // TODO: implement
    dout << "Done parsing." << std::endl;
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

std::string ArrayBoard::getAsciiBoard() { //TODO: make const
    // ----------- Generate array of printable space. Safer than just printing ----------- //
    short MIN_X = m_minCoords.first;
    short MAX_X = m_maxCoords.first;
    short MIN_Y = m_minCoords.second;
    short MAX_Y = m_maxCoords.second;

    short printSpaceWidth = MAX_X - MIN_X + 1;
    short printSpaceHeight = MAX_Y - MIN_Y + 1;

    std::vector<std::string> lines; // each element is line of output

    // ----------- Drawing controls ----------- //
    const char V_SEP = '|'; // sides of tile, repeated m_printSettings.m_height times
    const char CORNER = '+'; // corner where H_SEP and V_SEP meet
    const char H_SEP = '-'; // top/bottom of tile, repeated m_printSettings.m_width times

    // Margin drawing controls //TODO: parameterize?
    const unsigned short LEFT_MARGIN_SIZE = 5; // width of left margin
    const unsigned short LEFT_MARGIN_PAD = 3; // space between left margin and leftmost tiles
    const char MARGIN_V_SEP = '|'; // vertical boundary for margin
    const char MARGIN_H_SEP = '='; // horizontal boundary for margin
    const char MARGIN_H_LABEL_DIV = ' '; // What separates the labels on the x axis from eachother

    // ----------- Generate the board itself ----------- //

    // fill printable space with spaces. We will replace these as we go along
    std::string emptyLine(1 + (m_printSettings.m_width+1)*printSpaceWidth, ' ');
    int numLines = 1 + (m_printSettings.m_height+1)*printSpaceHeight;
    for (int i = 0; i < numLines; i++) {
        lines.push_back(emptyLine);
    }

    // loop through board positions in printable space
    for(int gridY = 0; gridY < printSpaceHeight; gridY++) {
        int caY = 1 + (m_printSettings.m_height+1)*gridY; // which row in this string array this cell starts. Does not include separators
        for(int gridX = 0; gridX < printSpaceWidth; gridX++) {
            int caX = 1 + (m_printSettings.m_width+1)*gridX; // which position in the string this cell starts. Does not include separators

            int gridXY = (MIN_X + gridX) + (MIN_Y + gridY) * m_grid_size; // 1D coords

            // draw the tile
            if (m_grid[gridXY] != INVALID) {
                // draw the fill
                for (int caFillLine = caY; caFillLine < caY+ m_printSettings.m_height; caFillLine++) {
                    lines[caFillLine] = lines[caFillLine].replace(caX, m_printSettings.m_width, m_printSettings.m_width, m_printSettings.m_tileFillChar);
                }

                // draw the piece
                if (m_grid[gridXY] != EMPTY) {
                    int yOff = (m_printSettings.m_height-1) / 2; // center offsets
                    int xOff = (m_printSettings.m_width-1) / 2; // -1 means that when m_printSettings.m_width is even, slight left justification is prefered
                    lines[caY + yOff] = lines[caY + yOff].replace(caX + xOff, 1, 1, PIECE_LETTERS[m_grid[gridXY]]);
                }
            }
            
            // draw the borders
            if (m_grid[gridXY] != INVALID) {
                // horizontal borders
                for (size_t yOff : std::vector<size_t>{0, m_printSettings.m_height+1}) {
                    lines[caY + yOff - 1] = lines[caY + yOff - 1].replace(caX, m_printSettings.m_width, m_printSettings.m_width, H_SEP);
                }
                // 2 sides
                for (size_t xOff : std::vector<size_t>{0, m_printSettings.m_width+1}) {
                    // vertical borders
                    for (size_t yOff = 0; yOff < m_printSettings.m_height; yOff++) {
                        lines[caY + yOff] = lines[caY + yOff].replace(caX + xOff - 1, 1, 1, V_SEP);
                    }
                    // 4 corners
                    for (size_t yOff : std::vector<size_t>{0, m_printSettings.m_height+1}) {
                        lines[caY + yOff - 1] = lines[caY + yOff - 1].replace(caX + xOff - 1, 1, 1, CORNER);
                    }
                }
            }

        }
    }

    std::string result = "";
    std::string dividerLine;

    // ----------- Add stuff to the top of output ----------- //
    if (m_printSettings.m_showCoords) {
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
            while (labelString.size() < m_printSettings.m_width) {
                labelString += " "; // label filler
            }
            result += labelString + MARGIN_H_LABEL_DIV;
        }

        // long horizontal line
        dividerLine = std::string(result.size(), MARGIN_H_SEP);
        result = dividerLine + "\n" + result + "\n" + dividerLine + "\n";
    }
    // ----------- Add stuff to left side of output ----------- //
    short currentY = MIN_Y;
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
    // ----------- Add stuff to the bottom of output ----------- //
    if (m_printSettings.m_showCoords) {
        result += dividerLine + "\n";
    }
    dout << "Length of printable string = " << result.length() << std::endl;
    return result;
}
