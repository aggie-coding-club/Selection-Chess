extends Reference

const constants = preload("res://scripts/Constants.gd")

# Integer division of a / b that always rounds towards negative infinity, e.g. -12 / 10 = -2
# from https://stackoverflow.com/questions/39304681/division-with-negative-dividend-but-rounded-towards-negative-infinity
static func div_floor(var a:int, var b:int)->int:
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
