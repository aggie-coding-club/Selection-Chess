#include "array_board.h"

#include "constants.h"
#include "utils.h"
#include "move.h"

#include <cstdint>
#include <stack>
#include <algorithm>
#include <vector>
#include <iostream>
#include <ctype.h>
#include <limits.h>

// This function is only meant to be used in init right now, unless it is cleaned up into something more general.
void ArrayBoard::updateExtrema(const ModCoords& _new) {
    // FIXME: ugly hack to get min to update. Maybe this is clue to how to actually get min to update later on
    if (_new.first == m_minCoords.first-1) {
        m_minCoords.first = _new.first;
    }
    if (_new.second == m_minCoords.second-1) {
        m_minCoords.second = _new.second;
    }

    m_maxCoords.first = std::max(m_maxCoords.first, _new.first, 
    [&](const ModularInt& _a, const ModularInt& _b) {
        return _a.lessThan(_b, m_minCoords.first);
    });
    m_maxCoords.second = std::max(m_maxCoords.second, _new.second, 
    [&](const ModularInt& _a, const ModularInt& _b) {
        return _a.lessThan(_b, m_minCoords.second);
    });
}

void ArrayBoard::init(const std::string _sfen) {
    int i; // which character of _sfen we are on
    tdout << "running ArrayBoard::init" << std::endl;

    // ----------- loop through and count number of tiles ----------- //
    m_grid_size = 1; // grid size = number of tiles + 1. See documentation on wrap-around for explanation of why
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
    dout << "Done counting." << std::endl;
    // check for multiplication overflow
    if (m_grid_size > SIZE_MAX / m_grid_size) {
        std::cerr << "Board is too large to be stored as ArrayBoard!" << std::endl;
        throw "Board is too large!";
    }
    // delete old m_grid
    if (m_grid != nullptr) {
        delete[] m_grid;
    }
    m_grid = new PieceEnum[m_grid_size*m_grid_size];
    // initialize all to non-tiles
    for (size_t i = 0; i < m_grid_size*m_grid_size; i++) {
        m_grid[i] = INVALID;
    }

    // set the class' modulus to match the wrap-around of our array.
    ModularInt::modulus = m_grid_size;

    // ----------- loop through again to initialize the grid ----------- //
    tdout << "ArrayBoard::init counted " << m_grid_size << " size required and will now init" << std::endl;

    // some arbitrary starting point
    const ModCoords STARTING_CORNER = std::make_pair(0, 0);
    m_minCoords = m_maxCoords = STARTING_CORNER; // TODO: remove STARTING CORNER, and just use m_minCoords?

    ModCoords currentFR = STARTING_CORNER;
    for (i = 0; i < _sfen.length() && _sfen[i] != ' '; i++) {
        const char c = _sfen[i];
        if (c == '/') { // Next row
            currentFR.first = STARTING_CORNER.first;
            --currentFR.second;
            continue;
        }
        if (c == '(') { // Space(s) have no tile(s)
            int j;
            // set j to be the next non-numeric character
            for (j = i+1; isdigit(_sfen[j]); j++);
            // If it is the edge case where there is no numbers, i.e. "()", we can skip this part
            if (j != i+1) {
                // Get the next characters as integer
                // FIXME: I've been using .substr wrong, need to go and change all occurences.
                int num_no_tiles = std::stoi(_sfen.substr(i+1, j)); //i+1 to ignore '('
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

            for (int k = 0; k < num_empty_tiles; k++) {
                m_grid[toIndex(currentFR, true)] = EMPTY;
                updateExtrema(currentFR);
                ++currentFR.first;
            }
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        m_grid[toIndex(currentFR, true)] = thisTile;
        updateExtrema(currentFR);
        m_pieceLocations[thisTile].push_back(currentFR);
        ++currentFR.first;
    }
    // Parse remaining fields
    // TODO: implement
    dout << "min coords are " << m_minCoords.first << ", " << m_minCoords.second << " and max are " << m_maxCoords.first << ", " << m_maxCoords.second << std::endl;
    dout << "Done parsing." << std::endl;
}

bool ArrayBoard::updatePieceInPL(PieceEnum _piece, ModCoords _oldLocation, ModCoords _newLocation) {
    //TODO: re implement
    // for (int i = 0; i < pieceNumbers[piece]; i++) { // loop for all pieces of type
    //     if (pieceLocations[piece][i] == oldLocation) { // find the match
    //         pieceLocations[piece][i] = newLocation;
    //         return true;
    //     }
    // }
    return false;
}

size_t ArrayBoard::toIndex(ModCoords _coords, bool _useInternal) {
    if (!_useInternal) {
        _coords.first -= m_minCoords.first;
        _coords.second -= m_minCoords.second;
    }
    return _coords.first.m_value + m_grid_size * _coords.second.m_value;
}

StandardArray ArrayBoard::standardArray() {
    Coords dimensions = getDimensions();
    // standard array we will return
    StandardArray sa(dimensions);

    size_t i = 0; // current index of sa
    ModCoords coords = std::make_pair(m_minCoords.first, m_maxCoords.second);
    // iterate over rows
    for (; coords.second != m_minCoords.second-1; --coords.second) {
        coords.first = m_minCoords.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; coords.first != m_maxCoords.first+1; ++coords.first) {
            sa.m_array[i++] = m_grid[toIndex(coords, true)];
        }
    }
    return sa;
}

std::string ArrayBoard::dumpAsciiArray() {
    std::string result = "[";
    for (int row = 0; row < m_grid_size; ++row) {
        result += "\n";
        for (int col = 0; col < m_grid_size; ++col) {
            result += getCharFromPiece(m_grid[toIndex(std::make_pair(col, row), true)], '=', '.');
        }
    }
    result += "\n]";
    return result;
}
