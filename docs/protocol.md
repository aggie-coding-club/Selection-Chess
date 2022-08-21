# Protocol for Selection Chess

## State Representation

There are standards for how to represent various state elements in chess as strings.
In this document, we describe how we extend those standards for Selection Chess.

### Move Notation

#### Coordinates

Chess has algebraic notation to describe coordinates, which we adopt.
To handle larger number of tiles, we extend the number of "characters".
Here is the pattern on each:

For rank, it just counts from 0 to 999, without leading zeros:
```
a0, a1, ..., a9, a10, a11, ..., a99, a100, a101, ..., a999, a0, ...
```

For file, it counts using letters from a to zz.
Note that this is *not* just base 26.
```
a0, b0, ..., z0, aa0, ab0, ac0, ..., zy0, zz0, a0, ...
```

For both of these, the coordinates wrap-around.

#### Piece Moves

Like standard chess, regular moves are represented by two positions, 
the old position and the new position:
```
<from><to>
```
For example, `zz0a2` describes the piece at `zz0` moving to `a2`.

We do not use castling notation or any other notation from chess to describe moves.

#### Tile Moves

To handle moving selections of tiles, we use the following format:
```
S<fromCorner1><fromCorner2><toCorner>[R<rotations>][F]
```
Where `fromCorner1` and `fromCorner2` are the (inclusive) minimum and maximum corners of the selection, and `toCorner` is the minimum corner of the destination.

For example, 
- `Sa0a0c0` moves the tile at `a0` right two spaces.
- `Sa0c1zy0` moves a selection 3 tiles wide and 2 tiles tall to the left by 2 spaces.

`R<rotations>` and `F` have not been implemented yet, but allow for rotating and mirroring the selection of tiles. For example, `a0c2a0R2` will rotate the 2x2 selection in-place. 
<!-- TODO: specify standard, such as clockwise/ccw, mirroing axis, rotation or flip first -->

#### Tile Deletions

To handle deleting tiles, just list the tiles to delete as follows:
```
D<tile1>[tile2[tile3...]]
```

For example, 
- `Da0` deletes the tile at `a0`.
- `Da0a1a2a3` deletes a line of tiles from `a0` to `a3`.

### Static Representation (FEN)

To represent the state of the board at a given point in time, chess uses FEN notation. Selection chess FEN (SFEN) is similar to this. It has the following form:
```
<placementString> <minCorner> <turn> <50-move-rule> <fullMoveCount>
```

The `placementString` describes the relative locations of pieces and tiles.
For example, 
```
rnbqkbnr/pppppppp/8/8(4)4/8(6)2/18/PPPPPPPP/RNBQKBNR
```
This is essentially instructions for a cursor writing pieces and tiles into spaces from left-to-right using the following rules:
- `/` is a newline character, moving the cursor down a rank and resetting its file. 
- Letters are tiles with pieces on them. Uppercase corresponds to white pieces, lower to black. The cursor writes this and advances right to the next file.
- Numbers outside of parentheses are empty tiles. The cursor writes an empty tile and advances right this many times.
- Numbers inside of parentheses are void, where there is no tiles. The cursor advances this many spaces to the right without adding anything. These numbers should always be followed by a tile or piece, as trailing void tiles are meaningless.

`minCorner` specifies the minimum coordinates of start of the `placementString` in algebraic notation.
Note that this space may be void, such as `(1)1/2`.
When `/` resets the file, it uses the file specified in `minCorner`.
CAUTION: The cursor starts on the highest row and works towards the minimum row, which means that the rank of `minCorner` actually specifies the last row in the SFEN string.

`turn` is either `b` or `w`, and specifies whose turn it is.

`50-move-rule` is a count of the number of half moves used by the 50 move rule. This rule might be changed by the ruleset, but by default this number resets to 0 when a capture or tile deletion occurs.

`fullMoveCount` just counts how many full moves have occurred this game. It starts at 1, and increments after black's turn.
