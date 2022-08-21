extends Node2D


# Declare member variables here. Examples:
onready var piece_tilemap = $GridSystem/NodeBoard/PieceTileMap
onready var board_tilemap = $GridSystem/NodeBoard/BoardTileMap

onready var piece_tilemap_floating = $GridSystem/FloatingNodeBoard/PieceTileMap
onready var board_tilemap_floating = $GridSystem/FloatingNodeBoard/BoardTileMap

const constants = preload("res://scripts/Constants.gd")

# used to relay signal
signal engine_log(player_num, text)

################# Selection and cursor state variables ####################
# What type of selection is made, if any. 
var selectionState = constants.NO_SEL

# as integer board coords
var selectionStartPos = Vector2.ZERO
var selectionEndPos = Vector2.ZERO

# Which 'tool' is selected, defines what clicking does
var cursorMode = constants.PIECE_MODE

# Stores state of whether cursor is pressed or not.
# In the case of piece_sel, means piece is actively following cursor.
# In the case of tile_sel, means rectangle is being drawn.
var dragging = false
################################################################################

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
	if selectionState == constants.PIECE_SEL:
		#piece_tilemap.set_cellv(selectionStartPos, [PIECE])
		# TODO: get piece back from whatever is carrying it
		pass
	if selectionState == constants.TILES_SEL:
		# TODO: 
		pass

# remove highlight and other stuff
func deselect():
	if dragging:
		drop_dragged()
	match selectionState:
		constants.PIECE_SEL:
			# TODO
			pass
		constants.TILES_SEL:
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
			if selectionState == constants.NO_SEL and event.pressed:
				selectionStartPos = mousePosNB
				match cursorMode:
					constants.PIECE_MODE:
						# TODO: check if valid selectable piece
						selectionState = constants.PIECE_SEL
						#dragging = true # We don't drag piece until cursor moves
						board_tilemap.set_cellv(mousePosNB, constants.TM_TILE_HIGHLIGHTED)
					constants.TILES_MODE:
						selectionState = constants.TILES_SEL
						dragging = true
					constants.DELETE_MODE:
						# TODO: implement deletions
						pass
			if selectionState == constants.PIECE_SEL:
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
		if selectionState == constants.PIECE_SEL:
			# just started dragging a piece
			if not dragging and Input.is_mouse_button_pressed(BUTTON_LEFT):
				var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
				# TODO: remove from pieceTileMap, move to floating map
				dragging = true
			
			if dragging:
				var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
				selectionEndPos = mousePosNB
				# TODO: update graphics

		if selectionState == constants.TILES_SEL:
			# TODO
			pass
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)

func add_engine(enginePath, player):
	print("TODO: add engine to GDNative code. [", enginePath, "]")

# Relay the signal to anyone who instantiates the GameSpace.
func _on_GridSystem_engine_log(player_num, text):
	emit_signal("engine_log", player_num, text)
