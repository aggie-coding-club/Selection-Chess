######### CAUTION: these must match the constants in the cpp source! ###########

enum SEL {NONE, FROM, TO}
enum {PIECE_MODE, TILES_MODE, DELETE_MODE}

# Constants for tilemaps
# Since all these tilemap enums start at -1 and count up, we use -2 for error/invalid return type for these
const TM_ENUM_NULL = -2
# Board Tilemaps
enum BoardTM {EMPTY = -1, TILE = 0, TILE_HIGHLIGHTED}
# Piece Tilemaps
enum PieceTM {EMPTY = -1, B_BISHOP = 0, B_KING, B_KNIGHT, B_PAWN, B_QUEEN, B_ROOK, W_BISHOP, W_KING, W_KNIGHT, W_PAWN, W_QUEEN, W_ROOK}
enum HighlightsTM {EMPTY = -1, CIRCLE = 0, DOT}

const DAModulus = 26*27
const DDModulus = 1000

enum SquareEnum {EMPTY=0, W_PAWN=1, B_PAWN, W_ROOK, B_ROOK, W_KNIGHT, B_KNIGHT, W_BISHOP, B_BISHOP, W_QUEEN, B_QUEEN, W_KING, B_KING, VOID};
# How we represent tiles with characters. Parallel to enum above
const TILE_LETTERS = [".", "P", "p", "R", "r", "N", "n", "B", "b", "Q", "q", "K", "k", "?"]
const SQUAREENUM_TO_TILEMAPENUM = [PieceTM.EMPTY, PieceTM.W_PAWN, PieceTM.B_PAWN, PieceTM.W_ROOK, PieceTM.B_ROOK, PieceTM.W_KNIGHT, PieceTM.B_KNIGHT, PieceTM.W_BISHOP, PieceTM.B_BISHOP, PieceTM.W_QUEEN, PieceTM.B_QUEEN, PieceTM.W_KING, PieceTM.B_KING]
