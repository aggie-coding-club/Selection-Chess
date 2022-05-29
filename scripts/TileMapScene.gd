extends Node2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
onready var piece_tilemap = $NodeBoard/PieceTileMap
onready var board_tilemap = $NodeBoard/BoardTileMap

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(0, 10):
		for j in range (0, 10):
			piece_tilemap.set_cell(i, j, 0 if ((i+j) % 2 == 0) else 6)
			board_tilemap.set_cell(i, j, 1)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
func _input(event):
	# Mouse in viewport coordinates.
	if event is InputEventMouseButton:
		#print("Mouse Click/Unclick at: ", event.position)
		#print("Mouse Click/Unclick at: ", get_viewport().get_mouse_position())
		print("Mouse Click/Unclick at: ", get_global_mouse_position(), "\t which is: ", board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position())))
	elif event is InputEventMouseMotion:
		#print("Mouse Motion at: ", event.position)
		#print("Mouse Motion at: ", get_viewport().get_mouse_position())
		#print("Mouse Motion at: ", get_global_mouse_position())
		pass
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)
