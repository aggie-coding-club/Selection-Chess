extends Reference

const constants = preload("res://scripts/Constants.gd")

# Integer division of a / b that always rounds towards negative infinity, e.g. -12 / 10 = -2
# from https://stackoverflow.com/questions/39304681/division-with-negative-dividend-but-rounded-towards-negative-infinity
static func div_floor(var a:int, var b:int)->int:
	# warning-ignore:integer_division
	var res = a / b;
	var rem = a % b;
	# Correct division result downwards if up-rounding happened,
	# (for non-zero remainder of sign different than the divisor).
	var corr = int(rem != 0 and ((rem < 0) != (b < 0)));
	return res - corr;

# This SHOULD be just another constructor for Vector2i, but GDScript won't let me >:(
static func Vector2iFromArray(var arr: Array)->Vector2i:
	# Cheap hack: assume array is correct size, and just take first and last elements
	return Vector2i.new(arr[0], arr[-1])

# TODO: workaround function until we upgrade to Godot 3.5
static func get_slice(p_string:String, delimiter:String, index:int):
	return p_string.split(delimiter)[index]

# Reads the p_sfen, executing the lambda functions p_forX whenever X is encountered.
# Those lambdas expect the following parameters:
#      p_forPiece(piece : String)
#      p_forVoid(numVoid : int)
#      p_forEmpty(numEmpty int)
#      p_forNewline()
static func parse_sfen(p_sfen: String, p_forPiece:FuncRef, p_forVoid:FuncRef, p_forEmpty:FuncRef, p_forNewline:FuncRef):
	var i = 0
	while i < p_sfen.length() && p_sfen.substr(i,1) != ' ':
		var c = p_sfen.substr(i,1)
		if c == '/': # Next row
			p_forNewline.call_func();

		elif c == '(': # Void square(s)
			var j = i+1
			# set j to be the next non-numeric character
			while p_sfen.substr(j,1).is_valid_integer():
				j += 1
			# If it is the edge case where there is no numbers, i.e. "()", we can skip this part
			if j != i+1:
				# Get the next characters as integer
				var numVoidTiles = p_sfen.substr(i+1, j-(i+1)).to_int() # i+1 to ignore '('
				p_forVoid.call_func(numVoidTiles);
			# update i to to account for the number of additional characters we read in
			i = j;

		elif c.is_valid_integer(): # Empty tile(s)
			var j = i+1
			while p_sfen.substr(j,1).is_valid_integer():
				j += 1
			# Get the next characters as integer
			var numEmptyTiles = p_sfen.substr(i, j-i).to_int()
			# update i to to account for the number of additional characters we read in
			i = j-1;
			p_forEmpty.call_func(numEmptyTiles);

		else:
#	        SquareEnum thisTile = getSquareFromChar(c, ' '); // We look for empty as ' ' to ensure we never find empty this way, just in case.
#	        if (thisTile == VOID) {
#	            //FIXME: error handling
#	            dlog("Error parsing sfen position: unknown character '", c, "'");
#				i += 1
			p_forPiece.call_func(c);
		i += 1

# Waiting for Godot 3.5...
static func join(delim : String, arr: Array):
	var ret = ""
	var first = true
	for i in arr:
		if not first:
			ret += delim
		else:
			first = false
		ret += i
	return ret

# Reverses the order of rows in a sfen placement string
static func reverse_sfen(orig: String)->String:
	var arr = orig.split("/")
	arr.invert()
	return join("/", arr)

# Returns the TM code for this squareEnum, 
static func getTMFromSquare(_squareEnum):
	if _squareEnum == constants.SquareEnum.VOID:
		return constants.TM_ENUM_NULL
	return constants.SQUAREENUM_TO_TILEMAPENUM[_squareEnum]

# Assumes this is PieceTM, so it returns either EMPTY or a piece
static func getSquareFromTM(_tmEnum):
	var found = constants.SQUAREENUM_TO_TILEMAPENUM.find(_tmEnum)
	if (found == -1):
		return constants.SquareEnum.EMPTY
	return found
	

# Gets letter respresentation of square type enum
# Optionally set empty to desired character representation of EMPTY and VOID
static func getCharFromSquare(_enumValue, _empty=constants.TILE_LETTERS[constants.SquareEnum.EMPTY], _invalid=constants.TILE_LETTERS[constants.SquareEnum.VOID])->String:
	if _enumValue == constants.SquareEnum.EMPTY:
		return _empty
	elif _enumValue == constants.SquareEnum.VOID:
		return _invalid
	return constants.TILE_LETTERS[_enumValue];

# Gets square enum from letter
# Optionally set empty to desired character representation of EMPTY
# Returns VOID for unknown characters.
static func getSquareFromChar(_char:String, _empty=constants.TILE_LETTERS[constants.SquareEnum.EMPTY]):
	if _char == _empty:
		return constants.SquareEnum.EMPTY;
	var found = constants.TILE_LETTERS.find(_char);
	if found == -1:
		return constants.SquareEnum.VOID;
	return found;

class IFuckingHate3:
	var x : int
	
	func _init(p_x):
		x = p_x
	func clone():
		var t = IFuckingHate3.new(x)
		return t

# Since Godot 4 is not out, we have to implement our own integer pair
class Vector2i extends Reference:
	var x : int
	var y : int

	func _init(var p_x:int, var p_y:int):
		x = p_x
		y = p_y

	func _to_string():
		return "(" + str(x) + ", " + str(y) + ")"

	############################# in-place operators ###############################
	func add(var other:Vector2i)->void:
		x += other.x
		y += other.y

	func negate()->void:
		x = -x
		y = -y

	# modular reduction for both x and y in vector2i
	func reduce(var x_mod:int, var y_mod:int)->void:
		x = posmod(x, x_mod)
		y = posmod(y, y_mod)

	func subtract(var other:Vector2i)->void:
		x -= other.x
		y -= other.y

	# CAUTION: == will still compile, but is actually just comparing references. Please use this instead
	func equals(var other:Vector2i)->bool:
		return x == other.x and y == other.y

	########################## not-in-place operators ##############################
	func clone()->Vector2i:
		return Vector2i.new(x, y)

# NOTE: these are outside Vector2i's scope to shorten function call. If Godot allowed overloading, this wouldn't have to be like this >:(
static func add_vectors2i(var one:Vector2i, var two:Vector2i)->Vector2i:
	var ret = one.clone()
	ret.add(two)
	return ret
static func subtract_vectors2i(var one:Vector2i, var two:Vector2i)->Vector2i:
	var ret = one.clone()
	ret.subtract(two)
	return ret
static func negative_of_vector2i(var vec:Vector2i)->Vector2i:
	var ret = vec.clone()
	return ret.negate()
static func reduce_vector2i(var vec:Vector2i, var x_mod:int, var y_mod:int)->Vector2i:
	var ret = vec.clone()
	return ret.reduce(x_mod, y_mod)
