extends Node2D


# Declare member variables here. Examples:
onready var piece_tilemap = $GridSystem/NodeBoard/PieceTileMap
onready var board_tilemap = $GridSystem/NodeBoard/BoardTileMap

onready var piece_tilemap_floating = $GridSystem/FloatingNodeBoard/PieceTileMap
onready var board_tilemap_floating = $GridSystem/FloatingNodeBoard/BoardTileMap

################# Selection and cursor state variables ####################
enum {NO_SEL, PIECE_SEL, TILES_SEL}
# What type of selection is made, if any. 
var selectionState = NO_SEL

# as integer board coords
var selectionStartPos = Vector2.ZERO
var selectionEndPos = Vector2.ZERO

enum {PIECE_MODE, TILES_MODE, DELETE_MODE}
# Which 'tool' is selected, defines what clicking does
var cursorMode = PIECE_MODE

# Stores state of whether cursor is pressed or not.
# In the case of piece_sel, means piece is actively following cursor.
# In the case of tile_sel, means rectangle is being drawn.
var dragging = false
################################################################################

# Constants for tilemaps
const TM_EMPTY = -1
# Board Tilemaps
enum {TM_TILE = 0, TM_TILE_HIGHLIGHTED}
# Piece Tilemaps
enum {TM_B_BISHOP, TM_B_KING, TM_B_KNIGHT, TM_B_PAWN, TM_B_QUEEN, TM_B_ROOK, TM_W_BISHOP, TM_W_KING, TM_W_KNIGHT, TM_W_PAWN, TM_W_QUEEN, TM_W_ROOK}

# Called when the node enters the scene tree for the first time.
func _ready():
	#for i in range(0, 10):
	#	for j in range (0, 10):
	#		piece_tilemap.set_cell(i, j, 0 if ((i+j) % 2 == 0) else 6)
	#		board_tilemap.set_cell(i, j, 1)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

# drops the thing currently being dragged, RESETTING to original position
func drop_dragged():
	if not dragging:
		return
	if selectionState == PIECE_SEL:
		#piece_tilemap.set_cellv(selectionStartPos, [PIECE])
		# TODO: get piece back from whatever is carrying it
		pass
	if selectionState == TILES_SEL:
		# TODO: 
		pass

# remove highlight and other stuff
func deselect():
	if dragging:
		drop_dragged()
	match selectionState:
		PIECE_SEL:
			# TODO
			pass
		TILES_SEL:
			# TODO
			pass

func _input(event):
#func _unhandled_input(event):
	# Mouse in viewport coordinates.
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			# position of mouse as NodeBoard indices 
			var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
			print("Mouse Click/Unclick at: ", get_global_mouse_position(), "\t which is: ", mousePosNB)
			if selectionState == NO_SEL and event.pressed:
				selectionStartPos = mousePosNB
				match cursorMode:
					PIECE_MODE:
						# TODO: check if valid selectable piece
						selectionState = PIECE_SEL
						#dragging = true # We don't drag piece until cursor moves
						board_tilemap.set_cellv(mousePosNB, TM_TILE_HIGHLIGHTED)
					TILES_MODE:
						selectionState = TILES_SEL
						dragging = true
					DELETE_MODE:
						# TODO: implement deletions
						pass
			if selectionState == PIECE_SEL:
				if event.pressed and mousePosNB == selectionStartPos:
					deselect()
				elif event.pressed and mousePosNB != selectionStartPos:
					# TODO: execute move
					pass
				elif not event.pressed:
					if mousePosNB == selectionStartPos:
						drop_dragged()
					else:
						# execute piece move
						pass
					
	elif event is InputEventMouseMotion:
		if selectionState == PIECE_SEL:
			# just started dragging a piece
			if not dragging and Input.is_mouse_button_pressed(BUTTON_LEFT):
				var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
				# TODO: remove from pieceTileMap, move to floating map
				dragging = true
			
			if dragging:
				var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
				selectionEndPos = mousePosNB
				# TODO: update graphics

		if selectionState == TILES_SEL:
			# TODO
			pass
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)
