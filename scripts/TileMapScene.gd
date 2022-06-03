extends Node2D


# Declare member variables here. Examples:
onready var piece_tilemap = $NodeBoard/PieceTileMap
onready var board_tilemap = $NodeBoard/BoardTileMap

onready var piece_tilemap_floating = $FloatingNodeBoard/PieceTileMap
onready var board_tilemap_floating = $FloatingNodeBoard/BoardTileMap

################# Selection and cursor state variables ####################
enum {NO_SEL, PIECE_SEL, TILES_SEL}
# What type of selection is made, if any. 
var selectionState = NO_SEL

# as integer board coords
var selectionStart = Vector2.ZERO
var selectionEnd = Vector2.ZERO

enum {PIECE_MODE, TILES_MODE, DELETE_MODE}
# Which 'tool' is selected, defines what clicking does
var cursorMode = PIECE_MODE

# Are we currently moving either a piece or a set of tiles
var dragging = false
################################################################################

# Constants for tilemaps
const TM_EMPTY = -1
# Board Tilemaps
enum {TM_TILE = 1, TM_TILE_HIGHLIGHTED}
# Piece Tilemaps
enum {TM_BB, TM_BK, TM_BN, TM_BP, TM_BQ, TM_BR, TM_WB, TM_WK, TM_WN, TM_WP, TM_WQ, TM_WR}

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

# remove highlight and other stuff
func deselect():
	match selectionState:
		PIECE_SEL:
			# TODO
			pass
		TILES_SEL:
			# TODO
			pass

func _input(event):
	# Mouse in viewport coordinates.
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			# position of mouse as NodeBoard indices 
			var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
			print("Mouse Click/Unclick at: ", get_global_mouse_position(), "\t which is: ", mousePosNB)
			selectionStart = mousePosNB
			if selectionState == NO_SEL and event.pressed:
				match cursorMode:
					PIECE_MODE:
						selectionState = PIECE_SEL
						dragging = true
						board_tilemap.set_cellv(mousePosNB, TM_TILE_HIGHLIGHTED)
					TILES_MODE:
						selectionState = TILES_MODE
						dragging = true
					DELETE_MODE:
						# TODO: implement deletions
						pass
	elif event is InputEventMouseMotion:
		#print("Mouse Motion at: ", event.position)
		#print("Mouse Motion at: ", get_viewport().get_mouse_position())
		#print("Mouse Motion at: ", get_global_mouse_position())
		if dragging:
			var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
			selectionEnd = mousePosNB
			# TODO: update graphics
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)
