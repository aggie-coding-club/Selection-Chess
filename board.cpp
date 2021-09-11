#include "board.h"
#include <algorithm>

std::string Board::toSfen() {
    // Convert into a standard array representation
    StandardArray sa = standardArray();

    if (sa.m_array.empty()) {
        return "[Empty board]"; // TODO: [Very Low Priority] There is probably a nicer way to display this. Probably should still to show the whole frame and everything.
    }

    std::string sfen = ""; // returned result

    unsigned int numOfEmpty = 0;
    unsigned int numOfInvalid = 0;

    // function to improve code readability
    auto endEmptySequence = [&]() {
        if (numOfEmpty != 0) {
            sfen += std::to_string(numOfEmpty);
            numOfEmpty = 0;
        }
    };
    auto endInvalidSequence = [&]() {
        if (numOfInvalid != 0) {
            sfen += "(" + std::to_string(numOfInvalid) + ")";
            numOfInvalid = 0;
        }
    };

    // iterate over rows
    for (int row = 0; row < sa.m_dimensions.second; row++) { // for each row
        // start of new line
        if (row != 0) { // check if this is not first iteration of loop, i.e. this is a line break
            endEmptySequence();
            numOfInvalid = 0; // we can ignore trailing void spaces
            sfen += "/";
        }

        // iterate over columns
        for (int col = 0; col < sa.m_dimensions.first; col++) { // for each element
            PieceEnum thisPiece = sa.m_array[col + sa.m_dimensions.first*row];
            if (thisPiece == VOID) {
                endEmptySequence();
                numOfInvalid++;
            } else if (thisPiece == EMPTY) {
                endInvalidSequence();
                numOfEmpty++;
            } else { // is a piece
                endEmptySequence();
                endInvalidSequence();
                sfen += PIECE_LETTERS[thisPiece];
            }
        }
    }
    endEmptySequence();
    return sfen;
};

/**
 * Note that comparing the hashes is MUCH faster that this, and should be used to compare boards
 * for most practical purposes. Really, this is just here to test if the hash function is working.
 */
bool Board::operator==(const Board& _other) const {
    // TODO: implement using StandardArray
    return false;
}

/** 
 * Print the current tiles and pieces in a nice ASCII format.
 */
std::string Board::getAsciiBoard() {
    // Convert into a standard array representation
    StandardArray sa = standardArray();

    if (sa.m_array.empty()) {
        return "[Empty board]"; // TODO: [Very Low Priority] There is probably a nicer way to display this. Probably should still to show the whole frame and everything.
    }

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

    // Margin drawing controls
    const unsigned int LEFT_MARGIN_SIZE = 6; // width of left margin
    const unsigned int LEFT_MARGIN_PAD = 3; // space between left margin and leftmost tiles
    const char MARGIN_V_SEP = '|'; // vertical boundary for margin
    const char MARGIN_H_SEP = '='; // horizontal boundary for margin
    const char MARGIN_H_LABEL_DIV = ' '; // What separates the labels on the x axis from eachother

    for (int i = 0; i < 2 + m_printSettings.m_height; i++) { // +2 for V_SEP on both sides
        lines.push_back("");
    }
    size_t activeLineNum = 0; // First line of output we are currently modifying
    bool trailingEdge = false; // Whether the last tile of this row already has printed the edge we share with it
    for (int row = 0; row < sa.m_dimensions.second; row++) { // for each row
        if (row != 0) { // start the next line
            trailingEdge = false;
            for (int i = 0; i < 1 + m_printSettings.m_height; i++) { // +1 for V_SEP
                activeLineNum++;
                lines.push_back("");
            }
        }
        for (int col = 0; col < sa.m_dimensions.first; col++) { // for each element
            PieceEnum thisPiece = sa.m_array[col + sa.m_dimensions.first*row];
            size_t lowerLine = activeLineNum + m_printSettings.m_height + 1;

            if (thisPiece == VOID) { // print empty space where there is no tiles
                for (int i = 1; i <= m_printSettings.m_height; i++) {
                    lines[activeLineNum + i] += (trailingEdge ? "" : " ") + h_fill_out;
                }
                lines[lowerLine] += (trailingEdge ? "" : " ") + h_non_sep;
                // We did not print righthand edge in case our neighbor is a tile and needs to print its edge there
                trailingEdge = false;

            } else { // Tile is here, may have a piece or be empty.
                // Output our tile's bottom border and center.
                for (int i = 1; i <= m_printSettings.m_height; i++) {
                    if (i == m_printSettings.m_height / 2 + 1) { // Is this the row which contains the piece
                        lines[activeLineNum + i] += (trailingEdge ? "" : V_SEP) + h_pad_left + getCharFromPiece(thisPiece, m_printSettings.m_tileFillChar) + h_pad_right + V_SEP;
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
        }
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
        for (auto xLabel = 0; xLabel != sa.m_dimensions.first; xLabel++) {
            std::string labelString = intToLetters(m_displayCoordsZero.first + xLabel);
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
    int currentY = sa.m_dimensions.second - 1;
    for (int i = 0; i < lines.size(); i++) {
        if (m_printSettings.m_showCoords) {
            std::string leftMargin = std::string(1, MARGIN_V_SEP) + " ";
            if (i % (m_printSettings.m_height+1) == m_printSettings.m_height / 2 + 1) {
                leftMargin += std::to_string((m_displayCoordsZero.second + currentY--).m_value);
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
    // tdout << "Length of printable string = " << result.length() << std::endl;
    return result;
}

uint64_t Board::getHash() const {
    // TODO: implement using StandardArray
    return 0u;
};

StandardArray::StandardArray() {
    m_dimensions = std::make_pair(0,0);
    m_array.clear();
}

StandardArray::StandardArray(Coords _size) {
    m_dimensions = _size;
    m_array.resize(m_dimensions.first * m_dimensions.second, VOID);
}

StandardArray::StandardArray(std::string _sfen) {
    //TODO: I feel like the two loops through the string can be simplified into some sort 
    // of function call instead. This could then be used if other board wanted to implement 
    // their own init from sfen function.
    // tdout << "created SA from Sfen" << std::endl;
    _sfen = _sfen.substr(0, _sfen.find(" ")); // remove game info, which we do not need
    auto sfenLines = split(_sfen, "/"); // split into lines
    m_dimensions.second = sfenLines.size();

    // get other dimension by counting max length
    m_dimensions.first = 0;
    for (std::string& line : sfenLines) { // for each line
        unsigned int spaceCount = 0; // how many spaces this line has
        for (size_t i = 0; i < line.length() && line[i] != ' '; i++) {
            const char c = line[i];
            if (c == '(') { // Space(s) have no tile(s)
                int j;
                // set j to be the next non-numeric character
                for (j = i+1; isdigit(line[j]); j++);
                // Get the next characters as integer
                spaceCount += std::stoi(line.substr(i+1, j)); //i+1 to ignore '('
                // make sure that it has this closing paren
                if (line[j] != ')') {
                    std::cerr << "Expected ')' after non tile sequence in SFen!" << std::endl;
                    throw "Expected ')' after non tile sequence in SFen!";
                }
                // update i to to account for the number of additional characters we read in
                i = j;

            } else if (isdigit(c)) { // Spaces are empty tiles
                int j;
                // set j to be the next non-numeric character
                for (j = i+1; isdigit(line[j]); j++);
                // Get the next characters as integer
                spaceCount += std::stoi(line.substr(i, j));
                // update i to to account for the number of additional characters we read in
                i = j-1;

            } else {
                // this is a tile with a piece on it
                spaceCount++;
            }
        }
        m_dimensions.first = std::max(m_dimensions.first, spaceCount);
    }
    dlog("Found dimension of " , m_dimensions.first , ", " , m_dimensions.second);
    // allocate the array
    m_array.resize(m_dimensions.first * m_dimensions.second, VOID);

    // iterate thru lines in reverse to initialize array
    unsigned int lineNumber = 0;
    for (auto lineIt = sfenLines.rbegin(); lineIt != sfenLines.rend(); ++lineIt) {
        // which index of standardArray we are on.
        unsigned int currentIndex = lineNumber * m_dimensions.first;

        for (size_t i = 0; i < lineIt->length() && (*lineIt)[i] != ' '; i++) {
            const char c = (*lineIt)[i];
            if (c == '(') { // Space(s) have no tile(s)
                int j;
                // set j to be the next non-numeric character
                for (j = i+1; isdigit((*lineIt)[j]); j++);
                // Get the next characters as integer
                unsigned int nonTileCount = std::stoi((*lineIt).substr(i+1, j)); //i+1 to ignore '('
                currentIndex += nonTileCount;
                // for (int ntci = 0; ntci < nonTileCount; ntci++) {
                //     m_array[currentIndex++] = VOID;
                // }
                // update i to to account for the number of additional characters we read in
                i = j;

            } else if (isdigit(c)) { // Spaces are empty tiles
                int j;
                // set j to be the next non-numeric character
                for (j = i+1; isdigit((*lineIt)[j]); j++);
                // Get the next characters as integer
                unsigned int emptyTileCount = std::stoi((*lineIt).substr(i, j));
                for (int etci = 0; etci < emptyTileCount; etci++) {
                    m_array[currentIndex++] = EMPTY;
                }

                // update i to to account for the number of additional characters we read in
                i = j-1;

            } else {
                PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
                if (thisTile != VOID) {
                    m_array[currentIndex++] = thisTile;
                } else {
                    std::cerr << "Invalid piece symbol '" << c << "' in SFen!" << std::endl;
                    throw "Invalid piece symbol in SFen!";
                }
            }
        }
        // fill trailing non-tile spaces with void
        for (; currentIndex % m_dimensions.first != 0; currentIndex++) {
            m_array[currentIndex++] = VOID;
        }
        lineNumber++;
    }
}

std::string StandardArray::dumpAsciiArray() {
    std::string result = "[";
    for (int row = 0; row < m_dimensions.second; row++) {
        result += "\n";
        for (int col = 0; col < m_dimensions.first; col++) {
            result += getCharFromPiece(m_array[col + m_dimensions.first*row], '=', '.');
        }
    }
    result += "\n]";
    return result;
}

DModCoords Board::SAtoDM(Coords _standard) const { // TODO: surely we can simplify these 4 conversion functions by just inlining with the += op?
    DModCoords dMod (_standard);
    dMod.first += m_displayCoordsZero.first;
    dMod.second += m_displayCoordsZero.second;
    return dMod;
}
Coords Board::DMtoSA(DModCoords _dMod) const {
    _dMod.first -= m_displayCoordsZero.first;
    _dMod.second -= m_displayCoordsZero.second;
    return std::make_pair(_dMod.first.m_value, _dMod.second.m_value);
}
