#ifndef RULESET_H
#define RULESET_H
// #pragma message("In Ruleset.h")

#include "constants.h"
#include "tokenizer.h"
#include "pieces.h"

#include <vector>
#include <iostream>

#pragma message("Returned to Ruleset.h")

class RulesetTokenizer : public AbstractStreamTokenizer {
    public:
        RulesetTokenizer(std::istream& _stream) : AbstractStreamTokenizer(_stream) {}

        std::string next();
        int nextInt();
        bool nextBool();

    private:
        inline void eatWhitespace(std::istream& _stream);
        std::string matchWordToken(std::istream& _stream);
        std::string matchDigitToken(std::istream& _stream);
};

class Ruleset {
    public:
        // initialize rules based on file
        bool init(std::string _ruleFile);
        Ruleset(std::string _ruleFile="default.rules");
        // How many tiles can be deleted using TileDeletion in a single move
        int m_numDeletionsPerTurn = 1;
        // Allow TileMoves to rotate the moved piece
        bool m_allowRotations = false;
        // Allow TileMoves to mirror the moved piece
        bool m_allowReflections = false;

        // Vectors of MoveOptions for each piece type. This is of size NUM_PIECE_TYPES_BY_COLOR + 1,
        // and is indexed by PieceEnum (the +1 reserves m_pieceMoveOptionLists[EMPTY] as placeholder so indices are not off by 1.)
        // E.g. m_pieceMoveOptionLists[B_ROOK] returns vector of all PMOs for black rooks.
        std::vector<std::vector<std::unique_ptr<MoveOption>>> m_pieceMoveOptionLists;
        // TODO: maybe timing info can be put here too?

    private:
        void match(std::string _given, std::string _expected);
        void updateMoveOptionProperties(RulesetTokenizer& _tokenizer, MoveOptionProperties& _mop);
        void addPieceMoveOptions(RulesetTokenizer& _tokenizer, std::vector<std::vector<std::unique_ptr<MoveOption>>>& _allPmoLists);

};

#endif