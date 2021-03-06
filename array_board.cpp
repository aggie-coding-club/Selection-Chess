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
#include <tuple>

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
    case TILE_DELETION:
        return apply(std::static_pointer_cast<TileDeletion>(_move));
    default:
        dout << "Unknown Move Type [" << _move->m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}
bool ArrayBoard::undo(std::shared_ptr<Move> _move) {
    switch (_move->m_type) {
    case PIECE_MOVE:
        return undo(std::static_pointer_cast<PieceMove>(_move));
    case TILE_MOVE:
        return undo(std::static_pointer_cast<TileMove>(_move));
    case TILE_DELETION:
        return undo(std::static_pointer_cast<TileDeletion>(_move));
    default:
        dout << "Unknown Move Type [" << _move->m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}

bool ArrayBoard::apply(std::shared_ptr<PieceMove> _move) {
    // tdout << "applying move " << _move->algebraic() << std::endl;
    ABModCoords startCoords = toInternalCoords(dModCoordsToStandard(_move->m_startPos));
    ABModCoords endCoords = toInternalCoords(dModCoordsToStandard(_move->m_endPos));

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
    ABModCoords startCoords = toInternalCoords(dModCoordsToStandard(_move->m_startPos));
    ABModCoords endCoords = toInternalCoords(dModCoordsToStandard(_move->m_endPos));

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
    // tdout << "applying move " << _move->algebraic() << std::endl;
    ABModCoords moveSelFirst = toInternalCoords(dModCoordsToStandard(_move->m_selFirst));
    ABModCoords moveSelSecond = toInternalCoords(dModCoordsToStandard(_move->m_selSecond));

    // ----------- Cut out the selection ----------- //
    // Save old coords in case we have to undo the cut operation
    ABModCoords oldMin = m_minCoords, oldMax = m_maxCoords; 
    DModCoords oldDZero = m_displayCoordsZero;
    StandardArray cut = getSelection(moveSelFirst, moveSelSecond, true);
    // dout << "Cut out " << cut.dumpAsciiArray() << std::endl;
    // dout << "Now board is \n" << getAsciiBoard() << std::endl;

    // ----------- Paste in the selection ----------- //
    // Figure out if this move updates the minima
    // Check if m_min - 1 is in the range of the destination rectangle. //TODO: document the proof that this works.
    // We have to do this in DMod space so it does not depend on our board's minimum, because that is what we are trying to find.
    DModCoords selectionSize = (_move->m_selSecond - _move->m_selFirst);
    DModCoords dModDestSecond = _move->m_destFirst + selectionSize;
    // What our m_minCoords is in DMod space
    DModCoords minCoordsInDMod = standardToDModCoords(toStandardCoords(m_minCoords));
    if ( //.first
        (minCoordsInDMod.first - 1)
        .lessThanOrEqual(dModDestSecond.first, _move->m_destFirst.first)) {
            int difference = (minCoordsInDMod.first - _move->m_destFirst.first).m_value;
            m_minCoords.first -= difference;
            m_displayCoordsZero.first -= difference;
    }
    if ( //.second //TODO: combine these into one function somehow for readability
        (minCoordsInDMod.second - 1)
        .lessThanOrEqual(dModDestSecond.second, _move->m_destFirst.second)) {
            int difference = (minCoordsInDMod.second - _move->m_destFirst.second).m_value;
            m_minCoords.second -= difference;
            m_displayCoordsZero.second -= difference;
    }
    
    // Now that our m_minCoords is updated, these operations are valid.
    ABModCoords moveDestFirst = toInternalCoords(dModCoordsToStandard(_move->m_destFirst));
    ABModCoords moveDestSecond = toInternalCoords(dModCoordsToStandard(dModDestSecond));

    // Update m_maxCoords now. Same idea as updating the m_minCoords, but we can do this in ABCoord space now.
    if ( //.first
        (m_maxCoords.first + 1)
        .lessThanOrEqual(moveDestSecond.first, moveDestFirst.first)) {
            int difference = (moveDestSecond.first - m_maxCoords.first).m_value;
            m_maxCoords.first += difference;
    }
    if ( //.second
        (m_maxCoords.second + 1)
        .lessThanOrEqual(moveDestSecond.second, moveDestFirst.second)) {
            int difference = (moveDestSecond.second - m_maxCoords.second).m_value;
            m_maxCoords.second += difference;
    }

    // Check destination rectangle is empty
    for (ABModCoords i = moveDestFirst; i.second != moveDestSecond.second + 1; ++i.second) { // iterate rows
        for (i.first = moveDestFirst.first; i.first != moveDestSecond.first + 1; ++i.first) { // iterate columns
            if (m_grid[toIndex(i)] != INVALID) {
                dout << WHERE << "FOUND PIECE [" << getCharFromPiece(m_grid[toIndex(i)]) << "] in DESTINATION, at " << coordsToAlgebraic(standardToDModCoords(toStandardCoords(i))) << std::endl;
                paste(cut, moveSelFirst); // paste back in original position
                m_minCoords = oldMin;
                m_maxCoords = oldMax;
                m_displayCoordsZero = oldDZero;
                return false;
            }
        }
    }
    paste(cut, moveDestFirst); // paste back in new position
    return true;
};

bool ArrayBoard::undo(std::shared_ptr<TileMove> _move) {
    // Literally just move the selection back.
    DModCoords translationDist = (_move->m_selSecond - _move->m_selFirst);
    DModCoords destSecond = _move->m_destFirst + translationDist;
    auto reverseMove = std::make_shared<TileMove>(_move->m_destFirst, destSecond, _move->m_selFirst);
    return apply(reverseMove);
}

bool ArrayBoard::apply(std::shared_ptr<TileDeletion> _move) {
    // tdout << "applying move " << _move->algebraic() << std::endl;
    for (DModCoords& dModDeletion : _move->m_deleteCoords) {
        ABModCoords deletionCoords = toInternalCoords(dModCoordsToStandard(dModDeletion));
        // if (m_grid[toIndex(deletionCoords)] == EMPTY) { // TODO: check if empty. Want to make sure we can undo it if part way we find it is illegal move.
        m_grid[toIndex(deletionCoords)] = INVALID;
    }

    // it is possible we cut off the min/max, so we need to update it accordingly
    // tdout << "minCoords=" << m_minCoords << " maxCoords=" << m_maxCoords << "before cut," << std::endl;
    ABModCoords updatedMin = std::make_pair( //TODO: this is repeated code from getSelection -- consider making this into a function, as well as its counterpart in apply, too
        nextTileByColOrder(std::make_pair(m_minCoords.first, 0), false).first, 
        nextTileByRowOrder(std::make_pair(0, m_minCoords.second), false).second
    );
    ABModCoords updatedMax = std::make_pair(
        nextTileByColOrder(std::make_pair(m_maxCoords.first, 0), true, true).first,
        nextTileByRowOrder(std::make_pair(0, m_maxCoords.second), true, true).second
    );
    ABModCoords minUpdate = updatedMin - m_minCoords; // since we are cutting tiles, updatedMin >= m_minCoords aka change in min is positive.
    m_displayCoordsZero += std::make_pair(minUpdate.first.m_value, minUpdate.second.m_value);
    m_minCoords = updatedMin;
    m_maxCoords = updatedMax;
    // tdout << "now minCoords=" << m_minCoords << " maxCoords=" << m_maxCoords << "before after the cut" << std::endl;

    return true;
}

bool ArrayBoard::undo(std::shared_ptr<TileDeletion> _move) {
    // tdout << "applying move " << _move->algebraic() << std::endl;
    // Note: this assumes that 2*[max number of tiles per deletion] + m_grid_size < 27*26 (the DModCoord space).
    // TODO: would be nice if there was a better way to figure out which side it corresponds to easier than checking which is closer so our assumption is not needed.
    for (DModCoords& dModAddition : _move->m_deleteCoords) {
        // check if this addition would change m_minCoords
        DModCoords dModMin = standardToDModCoords(toStandardCoords(m_minCoords));
        DModCoords dModMax = standardToDModCoords(toStandardCoords(m_maxCoords));
        // .first
        if (!dModAddition.first.lessThanOrEqual(dModMax.first, dModMin.first)) { // check if this is outside of our current bounds
            unsigned int distToMin = (dModMin.first - dModAddition.first).m_value;
            unsigned int distToMax = (dModAddition.first - dModMax.first).m_value;
            if (distToMin < distToMax) {
                m_minCoords.first -= distToMin;
                m_displayCoordsZero.first -= distToMin;
            } else {
                m_maxCoords.first += distToMax;
            }
        }
        // .second
        if (!dModAddition.second.lessThanOrEqual(dModMax.second, dModMin.second)) { // check if this is outside of our current bounds
            unsigned int distToMin = (dModMin.second - dModAddition.second).m_value;
            unsigned int distToMax = (dModAddition.second - dModMax.second).m_value;
            if (distToMin < distToMax) {
                m_minCoords.second -= distToMin;
                m_displayCoordsZero.second -= distToMin;
            } else {
                m_maxCoords.second += distToMax;
            }
        }

        // Now that m_minCoords is updated, this conversion is safe.
        ABModCoords addedCoords = toInternalCoords(dModCoordsToStandard(dModAddition));
        m_grid[toIndex(addedCoords)] = EMPTY;
    }

    return false;
}

bool ArrayBoard::isLegal(std::shared_ptr<Move> _move) {
    switch (_move->m_type) {
    case PIECE_MOVE:
        return isLegal(std::static_pointer_cast<PieceMove>(_move));
    case TILE_MOVE:
        return isLegal(std::static_pointer_cast<TileMove>(_move));
    case TILE_DELETION:
        return isLegal(std::static_pointer_cast<TileDeletion>(_move));
    default:
        dout << "Unknown Move Type [" << _move->m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}
bool ArrayBoard::isLegal(std::shared_ptr<PieceMove> _move) {
    //TODO: implement
    return true;
}
bool ArrayBoard::isLegal(std::shared_ptr<TileMove> _move) {
    // check selection rectangle is not oversized //TODO:
    // check space we are copying to is actually empty. Handle the coords conversions. //TODO:
    // check that connectedness is maintained //TODO: I think "Maintenance of a minimum spanning forest in a dynamic plane graph" by Eppstein et al. may give optimal results, although its probably easier to just run A* or smth
    return true;
}
bool ArrayBoard::isLegal(std::shared_ptr<TileDeletion> _move) {
    // check number of deletions OK
    if (_move->m_deleteCoords.empty()) {
        return false;
    }
    if (_move->m_deleteCoords.size() > m_rules.m_numDeletionsPerTurn) {
        return false;
    }
    // check there are actually EMPTY tiles to delete at given coords //TODO
    // for (DModCoords& delCoords : _move->m_deleteCoords) {
    //     if ()
    // }
    // check deletions do not break connectedness //TODO

    return true;
}

// Convert external coords to internal, e.g. (0,0) will be converted to m_minCoords
ABModCoords ArrayBoard::toInternalCoords(Coords _extern) const {
    ABModCoords intern(_extern);
    intern.first += m_minCoords.first;
    intern.second += m_minCoords.second;
    return intern;
}
// Convert internal coords to external, e.g. m_minCoords will be converted to (0,0)
Coords ArrayBoard::toStandardCoords(ABModCoords _intern) const {
    _intern.first -= m_minCoords.first;
    _intern.second -= m_minCoords.second;
    Coords external(_intern.first.m_value, _intern.second.m_value);
    return external;
}

size_t ArrayBoard::toIndex(ABModCoords _coords) const {
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

std::string ArrayBoard::dumpAsciiArray() const {
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

bool ArrayBoard::moveIsFromMO(std::shared_ptr<Move> _move, const MoveOption& _mo) {
    switch (_mo.m_type) {
    case LEAP_MO_TYPE:
        return moveIsFromMO(_move, dynamic_cast<const LeapMoveOption&>(_mo));
    case SLIDE_MO_TYPE:
        return moveIsFromMO(_move, dynamic_cast<const SlideMoveOption&>(_mo));
    default:
        dout << "Unknown MoveOption Type [" << _mo.m_type << "]\n" << WHERE << std::endl;
        return false;
    }
}

std::vector<std::unique_ptr<Move>> ArrayBoard::getMovesFromMO(ABModCoords& _pieceCoords, const MoveOption& _mo) {
    switch (_mo.m_type) {
    case LEAP_MO_TYPE:
        return getMovesFromMO(_pieceCoords, dynamic_cast<const LeapMoveOption&>(_mo));
    case SLIDE_MO_TYPE:
        return getMovesFromMO(_pieceCoords, dynamic_cast<const SlideMoveOption&>(_mo));
    default:
        dout << "Unknown MoveOption Type [" << _mo.m_type << "]\n" << WHERE << std::endl;
        return std::vector<std::unique_ptr<Move>>();
    }
}

bool ArrayBoard::moveIsFromMO(std::shared_ptr<Move> _move, const LeapMoveOption& _mo) {
    // TODO: implement
    return false;
}

std::vector<std::unique_ptr<Move>> ArrayBoard::getMovesFromMO(ABModCoords& _pieceCoords, const LeapMoveOption& _mo) {
    // tdout << "getMovesFrom Leaping MO called" << std::endl;
    ABModCoords& startCoords = _pieceCoords;
    std::vector<std::unique_ptr<Move>> moves;
    DirectionEnum fwdLoopStartCond = LEFT;
    DirectionEnum fwdLoopEndCond = DOWN;

    if (_mo.m_properties.m_forwardOnly) {
        dout << "UNIMPLEMENTED FEATURE: forwardOnly" << WHERE << std::endl;
    }
    if (!_mo.m_properties.m_flyOverGaps || !_mo.m_properties.m_flyOverPieces) {
        dout << "UNIMPLEMENTED FEATURE: flyOverX=false" << WHERE << std::endl;
    }

    for (DirectionEnum direction = fwdLoopStartCond; direction <= fwdLoopEndCond; ++direction) { // iterate over 4 directions
        // tdout << "checking in direction " << getCharFromDirection(direction) << std::endl;
        DirectionEnum bwdLoopStartCond;
        DirectionEnum bwdLoopEndCond;
        // Get perpendicular directions
        if (direction == LEFT || direction == RIGHT) {
            bwdLoopStartCond = UP;
            bwdLoopEndCond = DOWN;
        } else {
            bwdLoopStartCond = LEFT;
            bwdLoopEndCond = RIGHT;
        }
        for (DirectionEnum perpDirection = bwdLoopStartCond; perpDirection <= bwdLoopEndCond; ++perpDirection) {
            SignedCoords leapAmount = (DIRECTION_SIGNS[direction] * _mo.m_forwardDist) + (DIRECTION_SIGNS[perpDirection] * _mo.m_sideDist);
            ABModCoords endCoords = startCoords + leapAmount;

            // check if void
            if (m_grid[toIndex(endCoords)] == INVALID) {
                // Do nothing

            // check if empty
            } else if (m_grid[toIndex(endCoords)] == EMPTY) {
                if (_mo.m_properties.m_captureMode != CAPTURE_ONLY) {
                    // tdout << "found valid moved" << std::endl;
                    std::unique_ptr<PieceMove> newMove (new PieceMove(standardToDModCoords(toStandardCoords(startCoords)), standardToDModCoords(toStandardCoords(endCoords))));//FIXME: gross syntax, need shorter expression
                    moves.push_back(std::move(newMove));
                } // TODO: this smells like repeated code...

            // check if capture
            } else if (isPiece(m_grid[toIndex(endCoords)])) { // Are we moving onto another piece
                if (isWhite(m_grid[toIndex(startCoords)]) != isWhite(m_grid[toIndex(endCoords)])) { // Is this an enemy piece
                    if (_mo.m_properties.m_captureMode != MOVE_ONLY) {
                        // tdout << "found valid capture" << std::endl;
                        std::unique_ptr<PieceMove> newMove (new PieceMove(standardToDModCoords(toStandardCoords(startCoords)), standardToDModCoords(toStandardCoords(endCoords))));
                        newMove->m_capture = m_grid[toIndex(endCoords)];
                        moves.push_back(std::move(newMove));
                    }
                }
            }
        }
    }
    return std::move(moves);
}

// Checks if the move could have been generated from current MoveOption
bool ArrayBoard::moveIsFromMO(std::shared_ptr<Move> _move, const SlideMoveOption& _mo) {
    // TODO: implement
    // TODO: If we don't care about efficiency, this can just call the general getMovesFromMO and we don't even need this function then
    return false;
}

// Returns list of moves that the piece at _pieceCoords can make using this MoveOption on the current _board.
std::vector<std::unique_ptr<Move>> ArrayBoard::getMovesFromMO(ABModCoords& _pieceCoords, const SlideMoveOption& _mo) { // FIXME: I think I broke this lol. Going to fix next commit probably
    // tdout << "getMovesFrom Sliding MO called" << std::endl;
    ABModCoords& startCoords = _pieceCoords;
    std::vector<std::unique_ptr<Move>> moves;
    DirectionEnum loopStartCond = _mo.m_isDiagonal ? DOWN_LEFT : LEFT;
    DirectionEnum loopEndCond = _mo.m_isDiagonal ? UP_LEFT : DOWN;

    if (_mo.m_properties.m_forwardOnly) {
        dout << "UNIMPLEMENTED FEATURE: forwardOnly" << WHERE << std::endl;
    }

    for (DirectionEnum direction = loopStartCond; direction <= loopEndCond; ++direction) { // iterate over 4 directions
        // tdout << "checking in direction " << getCharFromDirection(direction) << std::endl;
        ABModCoords endCoords = startCoords;
        for (int slideRemaining = _mo.m_maxDist; slideRemaining != 0; --slideRemaining) { // Loop until we can't slide any further. Note != is used instead of > to ensure -1 loops forever
            endCoords += DIRECTION_SIGNS[direction];

            // check if void
            if (m_grid[toIndex(endCoords)] == INVALID) {
                if (!_mo.m_properties.m_flyOverGaps) {
                    break; // stop searching in this direction
                }
            // check if empty
            } else if (m_grid[toIndex(endCoords)] == EMPTY) {
                if (_mo.m_properties.m_captureMode != CAPTURE_ONLY) {
                    // tdout << "found valid moved" << std::endl;
                    std::unique_ptr<PieceMove> newMove (new PieceMove(standardToDModCoords(toStandardCoords(startCoords)), standardToDModCoords(toStandardCoords(endCoords))));//FIXME: gross syntax, need shorter expression
                    moves.push_back(std::move(newMove));
                }

            // check if capture
            } else if (isPiece(m_grid[toIndex(endCoords)])) { // Are we moving onto another piece
                if (isWhite(m_grid[toIndex(startCoords)]) != isWhite(m_grid[toIndex(endCoords)])) { // Is this an enemy piece
                    if (_mo.m_properties.m_captureMode != MOVE_ONLY) {
                        // tdout << "found valid capture" << std::endl;
                        std::unique_ptr<PieceMove> newMove (new PieceMove(standardToDModCoords(toStandardCoords(startCoords)), standardToDModCoords(toStandardCoords(endCoords))));
                        newMove->m_capture = m_grid[toIndex(endCoords)];
                        moves.push_back(std::move(newMove));
                    }
                }
                if (!_mo.m_properties.m_flyOverPieces) {
                    break; // stop searching this direction
                }
            }
        }
    }
    return std::move(moves);
}

std::vector<std::unique_ptr<Move>> ArrayBoard::getMoves(PieceColor _color) {
    std::vector<std::unique_ptr<Move>> legalMoves; // TODO: a lot of legality checks are missing rn. Especially ensuring connectedness of board.
    // ----------- PieceMoves ----------- //
    // tdout << "Getting PieceMoves" << std::endl;
    for (int pieceType = (_color==WHITE ? W_PAWN : B_PAWN); pieceType < NUM_PIECE_TYPES*2+1; pieceType+=2) { // iterate over pieces of _color in piece list
        // tdout << "  getting moves for pieces of type [" << getCharFromPiece(pieceType) << "]" << std::endl;
        auto& moveOptions = m_rules.m_pieceMoveOptionLists.at(pieceType);
        for (ABModCoords pieceCoords : m_pieceLocations[pieceType]) { // for all coords of pieces of this type
            // tdout << "    checking MoveOptions for piece on coords " << standardToDModCoords(toStandardCoords(pieceCoords)) << std::endl;
            for (auto& mo : moveOptions) {
                // tdout << "      checking a MoveOption for piece on coords " << standardToDModCoords(toStandardCoords(pieceCoords)) << std::endl;
                auto newMoves = getMovesFromMO(pieceCoords, *(mo.get()));
                // Moves new moves into our vector
                legalMoves.insert(legalMoves.end(), std::make_move_iterator(newMoves.begin()), std::make_move_iterator(newMoves.end()));
            }
        }
    }
    // ----------- TileDeletions ----------- //
    // TODO: assumes only one tile can be deleted, implement for more tiles?
    // TODO: gross iteration over the entire space, would be nice if I only iterated over tiles.
    ABModCoords coords = std::make_pair(m_minCoords.first, m_maxCoords.second);
    // iterate over rows
    for (; coords.second != m_minCoords.second-1; --coords.second) {
        coords.first = m_minCoords.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; coords.first != m_maxCoords.first+1; ++coords.first) {
            if (m_grid[toIndex(coords)] == EMPTY) {
                std::unique_ptr<TileDeletion> newMove (new TileDeletion(standardToDModCoords(toStandardCoords(coords))));
                legalMoves.push_back(std::move(newMove));
            }
        }
    }

    // ----------- TileMoves ----------- //
    // TODO: assumes only one tile can be moved, implement for more tiles?
    // TODO: gross iteration over the entire space, would be nice if I only iterated over tiles.
    ABModCoords startCoords = std::make_pair(m_minCoords.first, m_maxCoords.second);
    // iterate over rows
    for (; startCoords.second != m_minCoords.second-1; --startCoords.second) {
        startCoords.first = m_minCoords.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; startCoords.first != m_maxCoords.first+1; ++startCoords.first) {
            if (m_grid[toIndex(startCoords)] != INVALID) {

                ABModCoords endCoords = std::make_pair(m_minCoords.first, m_maxCoords.second+1); // FIXME: limited to playing within extrema. I kinda fixed this for max, but its gross.
                for (; endCoords.second != m_minCoords.second-1; --endCoords.second) {
                    endCoords.first = m_minCoords.first; // reset each loop to start at beginning of the row
                    // iterate over columns
                    for (; endCoords.first != m_maxCoords.first+2; ++endCoords.first) {
                        if (m_grid[toIndex(endCoords)] == INVALID && startCoords != endCoords) {
                            std::unique_ptr<TileMove> newMove (new TileMove(
                                standardToDModCoords(toStandardCoords(startCoords)),
                                standardToDModCoords(toStandardCoords(startCoords)),
                                standardToDModCoords(toStandardCoords(endCoords))
                            ));
                            legalMoves.push_back(std::move(newMove));
                        }
                    }
                }
            }
        }
    }


    return legalMoves;
}

StandardArray ArrayBoard::getSelection(const ABModCoords& _bl, const ABModCoords& _tr, bool _cut) { // TODO: consider merging with standardArray() ?
    // dout << "getting selection " << std::endl;
    Coords dimensions = std::make_pair((_tr - _bl).first.m_value+1, (_tr - _bl).second.m_value+1);
    // standard array we will return
    StandardArray sa(dimensions);

    // dout << "filling stdArray" << std::endl;
    size_t i = 0; // current index of sa
    ABModCoords coords = std::make_pair(_bl.first, _tr.second);
    // iterate over rows
    for (; coords.second != _bl.second-1; --coords.second) {
        coords.first = _bl.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; coords.first != _tr.first+1; ++coords.first) {
            sa.m_array[i++] = m_grid[toIndex(coords)];
            if (_cut) {
                m_grid[toIndex(coords)] = INVALID;
            }
        }
    }
    if (_cut) {
        // tdout << "minCoords=" << m_minCoords << " maxCoords=" << m_maxCoords << "before cut," << std::endl;
        // it is possible we cut off the min/max, so we need to update it accordingly
        ABModCoords updatedMin = std::make_pair(
            nextTileByColOrder(std::make_pair(m_minCoords.first, 0), false).first, 
            nextTileByRowOrder(std::make_pair(0, m_minCoords.second), false).second
        );
        ABModCoords updatedMax = std::make_pair(
            nextTileByColOrder(std::make_pair(m_maxCoords.first, 0), true, true).first,
            nextTileByRowOrder(std::make_pair(0, m_maxCoords.second), true, true).second
        );
        ABModCoords minUpdate = updatedMin - m_minCoords; // since we are cutting tiles, updatedMin >= m_minCoords aka change in min is positive.
        m_displayCoordsZero += std::make_pair(minUpdate.first.m_value, minUpdate.second.m_value);
        m_minCoords = updatedMin;
        m_maxCoords = updatedMax;
        // tdout << "now minCoords=" << m_minCoords << " maxCoords=" << m_maxCoords << "before after the cut" << std::endl;
    }

    // dout << "got selection " << std::endl;
    return sa;
}

void ArrayBoard::paste(const StandardArray& _sa, const ABModCoords& _bl) {
    ABModCoords tr = _bl + _sa.m_dimensions - std::make_pair(1,1);
    size_t i = 0; // current index of sa

    ABModCoords coords = std::make_pair(_bl.first, tr.second);
    // iterate over rows
    for (; coords.second != _bl.second-1; --coords.second) {
        coords.first = _bl.first; // reset each loop to start at beginning of the row
        // iterate over columns
        for (; coords.first != tr.first+1; ++coords.first) {
            m_grid[toIndex(coords)] = _sa.m_array[i++];
        }
    }
}

ABModCoords ArrayBoard::nextTileByRowOrder(const ABModCoords& _start, bool _reverse, bool _colReversed) const {
    ABModCoords current = _start;
    // tdout << "nextTiles search:" << current;
    int rankIncr = (_reverse? -1:1);
    int fileIncr = (_reverse? -1:1) * (_colReversed? -1:1);
    while (m_grid[toIndex(current)] == INVALID) {
        // check if reached end of row. If positive fileIncr, end of row is m_grid_size-1, otherwise end of row is 0
        if (current.first.m_value == (fileIncr==1 ? m_grid_size-1 : 0)) { 
            current.second += rankIncr;
        }
        current.first += fileIncr;
        // tdout << current;
    }
    return current;
}

ABModCoords ArrayBoard::nextTileByColOrder(const ABModCoords& _start, bool _reverse, bool _rowReversed) const {
    ABModCoords current = _start;
    // tdout << "nextTiles search:" << current;
    int fileIncr = (_reverse? -1:1);
    int rankIncr = (_reverse? -1:1) * (_rowReversed? -1:1);
    while (m_grid[toIndex(current)] == INVALID) {
        // check if reached end of column. If positive rankIncr, end of column is m_grid_size-1, otherwise end of column is 0
        if (current.second.m_value == (rankIncr==1 ? m_grid_size-1 : 0)) { 
            current.first += fileIncr;
        }
        current.second += rankIncr;
        // tdout << current;
    }
    return current;
}
