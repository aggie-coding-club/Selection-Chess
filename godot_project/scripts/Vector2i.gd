extends Reference
class_name Vector2i

###### Since Godot 4 is not out, we have to implement our own integer pair #####
var x : int
var y : int

func _init(var p_x:int, var p_y:int):
	x = p_x
	y = p_y

func _to_string():
	return "(" + str(x) + ", " + str(y) + ")"

############################# in-place operators ###############################
func add(var other:Vector2i)->Vector2i:
	x += other.x
	y += other.y
	return self

func negate()->Vector2i:
	x = -x
	y = -y
	return self

# modular reduction for both x and y in vector2i
func reduce(var x_mod:int, var y_mod:int)->Vector2i:
	x = posmod(x, x_mod)
	y = posmod(y, y_mod)
	return self

func subtract(var other:Vector2i)->Vector2i:
	x -= other.x
	y -= other.y
	return self
	
# CAUTION: == will still compile, but is actually just comparing references. Please use this instead
func equals(var other:Vector2i)->bool:
	return x == other.x and y == other.y
	
########################## not-in-place operators ##############################
# TODO: Godot 3 struggles with cloning functions, I cannot figure this out
#static func clone_vector2i(orig:Vector2i)->Vector2i:
#	return Vector2i.new(orig.x, orig.y)
#
#static func add_vectors2i(var one:Vector2i, var two:Vector2i)->Vector2i: # :( I wanna overload operators
#	var ret = clone_vector2i(one)
#	ret.add(two)
#	return ret
#
#static func subtract_vectors2i(var one:Vector2i, var two:Vector2i)->Vector2i:
#	var ret = one.clone()
#	ret.subtract(two)
#	return ret
#
#static func negative_of_vector2i(var vec:Vector2i)->Vector2i:
#	var ret = vec.clone()
#	return ret.negate()
