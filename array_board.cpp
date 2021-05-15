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

// arrayboard modulus
unsigned int ABModulus;

// This function is only meant to be used in init right now, unless it is cleaned up into something more general.
void ArrayBoard::updateExtrema(const ABModCoords& _new) {
    // FIXME: ugly hack to get min to update. Maybe this is clue to how to actually get min to update later on
    if (_new.first == m_minCoords.first-1) {
        m_minCoords.first = _new.first;
    }
    if (_new.second == m_minCoords.second-1) {
        m_minCoords.second = _new.second;
    }

    m_maxCoords.first = std::max(m_maxCoords.first, _new.first, 
    [&](const ABModInt& _a, const ABModInt& _b) {
        return _a.lessThan(_b, m_minCoords.first);
    });
    m_maxCoords.second = std::max(m_maxCoords.second, _new.second, 
    [&](const ABModInt& _a, const ABModInt& _b) {
        return _a.lessThan(_b, m_minCoords.second);
    });
}

void ArrayBoard::init(const std::string _sfen) {
    resetPL();
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
    ABModulus = m_grid_size;

    // ----------- loop through again to initialize the grid ----------- //
    tdout << "ArrayBoard::init counted " << m_grid_size << " size required and will now init" << std::endl;

    // some arbitrary starting point
    const ABModCoords STARTING_CORNER = std::make_pair(0, 0);
    m_minCoords = m_maxCoords = STARTING_CORNER; // TODO: remove STARTING CORNER, and just use m_minCoords?

    ABModCoords currentFR = STARTING_CORNER;
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
                m_grid[toIndex(currentFR)] = EMPTY;
                updateExtrema(currentFR);
                ++currentFR.first;
            }
            continue;
        }

        PieceEnum thisTile = getPieceFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
        m_grid[toIndex(currentFR)] = thisTile;
        addPieceToPL(thisTile, currentFR);
        updateExtrema(currentFR);
        ++currentFR.first;
    }
    // Parse remaining fields
    // TODO: implement
    dout << "min coords are " << m_minCoords.first << ", " << m_minCoords.second << " and max are " << m_maxCoords.first << ", " << m_maxCoords.second << std::endl;
    // dout << "Done parsing." << std::endl;
}

bool ArrayBoard::updatePieceInPL(PieceEnum _piece, ABModCoords _oldLocation, ABModCoords _newLocation) {
    for (int i = 0; i < m_pieceLocations[_piece].size(); i++) { // loop for all pieces of type _piece
        if (m_pieceLocations[_piece][i] == _oldLocation) { // find the match
            m_pieceLocations[_piece][i] = _newLocation;
            return true;
        }
    }
    return false;
}

bool ArrayBoard::removePieceFromPL(PieceEnum _piece, ABModCoords _location) {
    for (int i = 0; i < m_pieceLocations[_piece].size(); i++) { // loop for all pieces of type _piece
        if (m_pieceLocations[_piece][i] == _location) { // find the match
            m_pieceLocations[_piece].erase(m_pieceLocations[_piece].begin() + i);
            return true;
        }
    }
    return false;
}

bool ArrayBoard::apply(std::shared_ptr<Move> _move) {
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
bool ArrayBoard::undo(std::shared_ptr<Move> _move) {
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

bool ArrayBoard::apply(std::shared_ptr<PieceMove> _move) {
    // tdout << "applying move " << _move->algebraic() << std::endl;
    ABModCoords startCoords = toInternalCoords(_move->m_startPos);
    ABModCoords endCoords = toInternalCoords(_move->m_endPos);

    // Execute the move
    // Update PieceList
    if (isPiece(m_grid[toIndex(endCoords)])) { // if a piece in endCoords, capture it
        removePieceFromPL(m_grid[toIndex(endCoords)], endCoords);
    }
    updatePieceInPL(m_grid[toIndex(startCoords)], startCoords, endCoords);

    // Update array
    m_grid[toIndex(endCoords)] = m_grid[toIndex(startCoords)];
    m_grid[toIndex(startCoords)] = EMPTY;
    return true;
};

bool ArrayBoard::undo(std::shared_ptr<PieceMove> _move) {
    // tdout << "undoing move " << _move->algebraic() << std::endl;
    ABModCoords startCoords = toInternalCoords(_move->m_startPos);
    ABModCoords endCoords = toInternalCoords(_move->m_endPos);

    // Execute the undo
    // Update PieceList
    updatePieceInPL(m_grid[toIndex(endCoords)], endCoords, startCoords);
    if (isPiece(_move->m_capture)) { // if a piece was captured, add it back
        addPieceToPL(_move->m_capture, endCoords);
    }
    // Update array
    m_grid[toIndex(startCoords)] = m_grid[toIndex(endCoords)];
    m_grid[toIndex(endCoords)] = _move->m_capture; // Note this works since m_capture=EMPTY when no capture occurred
    return true;
};

bool ArrayBoard::apply(std::shared_ptr<TileMove> _move) {
    tdout << "applying move " << _move->algebraic() << std::endl;

    // Get the Internal Coords of the range of tiles in this selection
    ABModCoords moveSelFirst = toInternalCoords(_move->m_selFirst);
    ABModCoords moveSelSecond = toInternalCoords(_move->m_selSecond);

    // FIXME: does not update extrema in the case that we remove the last extrema tile. Need to keep a list of all extrema tiles or something.

    // TODO: update m_minCoords and m_maxCoords

    return false;
};

bool ArrayBoard::undo(std::shared_ptr<TileMove> _move) {
    // Literally just move the selection back.
    // FIXME: when m_minCoords update, we can't just do this since external to internal conversion depends on the previous min.
    auto reverseMove = std::make_shared<TileMove>(_move->m_selFirst + _move->m_translation, _move->m_selSecond + _move->m_translation, -_move->m_translation);
    return apply(reverseMove);
}

// Convert external coords to internal, e.g. (0,0) will be converted to m_minCoords
ABModCoords ArrayBoard::toInternalCoords(Coords _extern) {
    ABModCoords intern(_extern);
    intern.first += m_minCoords.first;
    intern.second += m_minCoords.second;
    return intern;
}
// Convert internal coords to external, e.g. m_minCoords will be converted to (0,0)
Coords ArrayBoard::toExternalCoords(ABModCoords _intern) {
    _intern.first -= m_minCoords.first;
    _intern.second -= m_minCoords.second;
    Coords external(_intern.first.m_value, _intern.second.m_value);
    return external;
}

size_t ArrayBoard::toIndex(ABModCoords _coords) {
    return _coords.first.m_value + m_grid_size * _coords.second.m_value;
}

StandardArray ArrayBoard::standardArray() {
    Coords dimensions = getDimensions();
    // standard array we will return
    StandardArray sa(dimensions);

    size_t i = 0; // current index of sa
    ABModCoords coords = std::make_pair(m_minCoords.first, m_maxCoords.second);
    // iterate over rows
    for (; coords.second != m_minCoords.second-1; --coords.second) {
        coords.first = m_minCoords.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; coords.first != m_maxCoords.first+1; ++coords.first) {
            sa.m_array[i++] = m_grid[toIndex(coords)];
        }
    }
    return sa;
}

std::string ArrayBoard::dumpAsciiArray() {
    std::string result = "[";
    for (int row = 0; row < m_grid_size; ++row) {
        result += "\n";
        for (int col = 0; col < m_grid_size; ++col) {
            result += getCharFromPiece(m_grid[toIndex(std::make_pair(col, row))], '=', '.');
        }
    }
    result += "\n]";
    return result;
}

int ArrayBoard::staticEvaluation() {
    //TODO: this is just a dumb implementation to test if minmax works. Find a better implementation in the future.
    int staticValue = 0;
    for (int i = 1; i < NUM_PIECE_TYPES*2+1; i++) {
        staticValue += PIECE_VALUES[i] * m_pieceLocations[i].size();
        for (ABModCoords t : m_pieceLocations[i]) {
            // add 20 centipawns for being on 'even' coordinates
            if ((t.first + t.second).m_value % 2 == 0) {
                staticValue += 20 * (isWhite(i) ? 1 : -1); // negate if black
            }
        }
    }
    return staticValue;
}

std::vector<std::unique_ptr<Move>> ArrayBoard::getMoves(PieceColor _color) {
    // TODO: assumes all pieces can move 1 tile forward, back, left, or right, for the sake of testing minmax.
    std::vector<std::unique_ptr<Move>> legalMoves;
    for (int pieceType = (_color==WHITE ? W_PAWN : B_PAWN); pieceType < NUM_PIECE_TYPES*2+1; pieceType+=2) { // iterate over pieces of _color in piece list
        for (ABModCoords startCoords : m_pieceLocations[pieceType]) { // for all coords of pieces of this type
            for (int direction = LEFT; direction <= DOWN; direction++) { // iterate over 4 directions
                ABModCoords endCoords = startCoords + DIRECTION_SIGNS[direction];
                // check if empty
                if (m_grid[toIndex(endCoords)] == EMPTY) {
                    std::unique_ptr<PieceMove> newMove (new PieceMove(toExternalCoords(startCoords), toExternalCoords(endCoords)));
                    legalMoves.push_back(std::move(newMove));
                // check if capture
                } else if (isPiece(m_grid[toIndex(endCoords)]) // Are we moving onto another piece
                && (isWhite(m_grid[toIndex(startCoords)]) != isWhite(m_grid[toIndex(endCoords)]))) { // Is this an enemy piece
                    std::unique_ptr<PieceMove> newMove (new PieceMove(toExternalCoords(startCoords), toExternalCoords(endCoords)));
                    newMove->m_capture = m_grid[toIndex(endCoords)];
                    legalMoves.push_back(std::move(newMove));
                }
            }
        }
    }
    return legalMoves;
}
