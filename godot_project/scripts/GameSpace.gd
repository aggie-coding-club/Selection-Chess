extends Node2D

const constants = preload("res://scripts/Constants.gd")
const util = preload("res://scripts/Utils.gd")
const sutilClass = preload("res://StaticUtils.gdns")
var sutil = sutilClass.new()

# Declare member variables here. Examples:
onready var grid_system = $GridSystem

onready var piece_tilemap = $GridSystem/NodeBoard/PieceTileMap
onready var board_tilemap = $GridSystem/NodeBoard/BoardTileMap
onready var highlights_tilemap = $GridSystem/NodeBoard/HighlightsTileMap

onready var piece_tilemap_floating = $GridSystem/FloatingNodeBoard/PieceTileMap
onready var board_tilemap_floating = $GridSystem/FloatingNodeBoard/BoardTileMap

onready var camera = $Camera2D

# used to relay signals
signal engine_log(player_num, text)
signal sfen_update(text)

var chunk
var sfen  = ""

const sfen_index_placement = 0
const sfen_index_min_corner = 1
const sfen_index_turn = 2
const sfen_index_50move = 3
const sfen_index_moveCount = 4

# Variables used by redraw_board, keeping track of where we are drawing
# Keeps track of the modular coordinates the printhead is at WITHIN the chunk
var mod_printhead : util.Vector2i
# Keeps track of the (0,0) position of the chunk the printhead is in
var printhead_chunk_origin : util.Vector2i
var mod_printhead_reset : util.Vector2i

################# Selection and cursor state variables ####################
# What type of selection is made, if any. 
var selectionState = constants.NO_SEL

# as integer board coords
var selectionStartPos = util.Vector2i.new(0,0)
var selectionEndPos = util.Vector2i.new(0,0)

# Which 'tool' is selected, defines what clicking does
var cursorMode = constants.PIECE_MODE

# Stores state of whether cursor is pressed or not.
# In the case of piece_sel, means piece is actively following cursor.
# In the case of tile_sel, means rectangle is being drawn.
var dragging = false
################################################################################

# Called when the node enters the scene tree for the first time.
func _ready():
	chunk = get_chunk()
	redraw_board()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var currentChunk = get_chunk()
	if not chunk.equals(currentChunk):
		print("new chunk:", currentChunk, " old chunk:", chunk)
		chunk = currentChunk
		redraw_board()

func _input(event):
#func _unhandled_input(event):
	# Mouse in viewport coordinates.
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			# position of mouse as NodeBoard indices 
			var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
			mousePosNB.y = - mousePosNB.y # Godot TileMap y is reverse of chess standard
			print("Mouse Click/Unclick at: ", get_global_mouse_position(), "\t which is: ", mousePosNB)
			if selectionState == constants.NO_SEL and event.pressed:
				selectionStartPos = mousePosNB
				match cursorMode:
					constants.PIECE_MODE:
						# TODO: check if valid selectable piece
						selectionState = constants.PIECE_SEL
						#dragging = true # We don't drag piece until cursor moves
						mousePosNB.y = - mousePosNB.y # Godot TileMap y is reverse of chess standard
						board_tilemap.set_cellv(mousePosNB, constants.BoardTM.TILE_HIGHLIGHTED)
					constants.TILES_MODE:
						selectionState = constants.TILES_SEL
						dragging = true
					constants.DELETE_MODE:
						# TODO: implement deletions
						pass
			elif selectionState == constants.PIECE_SEL:
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
				mousePosNB.y = - mousePosNB.y # Godot TileMap y is reverse of chess standard
				# TODO: remove from pieceTileMap, move to floating map
				dragging = true
			
			if dragging:
				var mousePosNB = board_tilemap.world_to_map(board_tilemap.to_local(get_global_mouse_position()))
				mousePosNB.y = - mousePosNB.y # Godot TileMap y is reverse of chess standard
				selectionEndPos = mousePosNB
				# TODO: update graphics

		if selectionState == constants.TILES_SEL:
			# TODO
			pass
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)

# Get the tilemap coords from a global position (already crossed negation barrier)
func getCoordsFromGlobalPos(var p_global)->util.Vector2i:
	var indicesRaw = board_tilemap.world_to_map(board_tilemap.to_local(p_global));
	# Note that y is negated so that up is increased coords
	var indices = util.Vector2i.new(int(indicesRaw.x), int(-indicesRaw.y));
	return indices;

func get_chunk()->util.Vector2i:
	# TileMap indices where center of camera currently is
	var cameraIndices = getCoordsFromGlobalPos(camera.get_global_position());
	# Get chunk camera center is in. Have to convert to signed so division works as expected.
	var cameraChunk = util.Vector2i.new(
		util.div_floor(cameraIndices.x, constants.DAModulus), 
		util.div_floor(cameraIndices.y, constants.DDModulus))
	return cameraChunk;

func forPiece(c):
	var sqr_enum = util.getSquareFromChar(c)
	var print_coords = util.add_vectors2i(mod_printhead, printhead_chunk_origin)
#	print("writing ", c, " to ", print_coords, ", which is enum: ", sqr_enum)
	set_cell_formatted(print_coords.x, -print_coords.y, sqr_enum)
	mod_printhead.x = posmod(mod_printhead.x + 1, constants.DAModulus)
func forVoid(count):
#	print("V[", count, "] ", " to ", mod_printhead)
	mod_printhead.x = posmod(mod_printhead.x + count, constants.DAModulus)
func forEmpty(count):
#	print("emp[", count,"] ", " to ", mod_printhead)
	for _i in range(count):
		var print_coords = util.add_vectors2i(mod_printhead, printhead_chunk_origin)
		set_cell_formatted(print_coords.x, -print_coords.y, constants.SquareEnum.EMPTY)
		mod_printhead.x = posmod(mod_printhead.x + 1, constants.DAModulus)

func forNewline():
#	print("/ ", " to ", mod_printhead)
	mod_printhead.y = posmod(mod_printhead.y + 1, constants.DDModulus)
	mod_printhead.x = mod_printhead_reset.x

func redraw_board():
	var x = constants.PIECE_SEL
	var why = sutil.letters_to_int("ac")
	print("sutils: ", why, " <-> ", sutil.int_to_letters(why))
	var vec2i = util.Vector2iFromArray(sutil.algebraic_to_coords("ac98"))
	print("sutils array: ", vec2i)
	print("<-> ", sutil.coords_to_algebraic(vec2i.x, vec2i.y))
	print("redrawing using sfen [", sfen, "]")
	piece_tilemap.clear()
	board_tilemap.clear()
	highlights_tilemap.clear()
	piece_tilemap_floating.clear()
	board_tilemap_floating.clear()

	mod_printhead = util.Vector2iFromArray(sutil.algebraic_to_coords(util.get_slice(sfen, " ", sfen_index_min_corner)))
	mod_printhead_reset = mod_printhead.clone()

	var sfen_placement = util.get_slice(sfen, " ", sfen_index_placement)
	print("before: ", sfen_placement)
	sfen_placement = util.reverse_sfen(sfen_placement)
	print("after: ", sfen_placement)
#
	var centerChunk = chunk
	print("centerchunk f=", centerChunk.x, " r=", centerChunk.y)

	# Draw board in each chunk. This achieves the 'wrap-around' effect, as the center chunk is surrounded by other
	# chunks, so if we are at the edge of the center chunk we see a duplicate of its other side.
	# This can also be interpretted as there are infinite chunks in each direction, and we only load the 9 that we 
	# might see.
	for chunkNumOffsetR in range(-1, 1+1):
		for chunkNumOffsetF in range(-1, 1+1):
			# Draw board
#			print("draing with offset: ", chunkNumOffsetF, ", ", chunkNumOffsetR)
#			var chunkOrigin = util.Vector2i.new(, )
			printhead_chunk_origin = util.Vector2i.new(
				(centerChunk.x + chunkNumOffsetF) * constants.DAModulus, 
				(centerChunk.y + chunkNumOffsetR) * constants.DDModulus)
			mod_printhead = mod_printhead_reset.clone()
			util.parse_sfen(sfen_placement, 
			funcref(self, "forPiece"),
			funcref(self, "forVoid"),
			funcref(self, "forEmpty"),
			funcref(self, "forNewline"))
#			// setCell(chunkOrigin.file * DAModulus, -chunkOrigin.rank * DDModulus, B_ROOK);
#	// setCell(centerChunk.file * DAModulus, -centerChunk.rank * DDModulus, W_ROOK);

func reset_sfen(rawSfen):
	grid_system.reset_sfen(rawSfen)
	
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

# Calls set_cell on Godot boards to change the displayed cell.
# Use _isSelected, _isFloating, and _highlight to change decorators.
# _squareEnum is of enum constants.SquareEnum.
# _highlight is of enum constants.HighlightTM.
# Behavior for invalid combinations of decorators is undefined.
# CAUTION: just like set_cell on individual boards, the y needs to be negated
func set_cell_formatted(_x:int,_y:int, _squareEnum, _isSelected:bool=false, _isFloating:bool=false, _highlight=constants.HighlightsTM.EMPTY):
	if _squareEnum == constants.SquareEnum.VOID:
		board_tilemap.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap_floating.set_cell(_x, _y, constants.PieceTM.EMPTY);
		highlights_tilemap.set_cell(_x, _y, constants.HighlightsTM.EMPTY);

	elif _isFloating:
		board_tilemap.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.TILE_HIGHLIGHTED if _isSelected else constants.BoardTM.TILE);
		piece_tilemap_floating.set_cell(_x, _y, util.getTMFromSquare(_squareEnum))
		highlights_tilemap.set_cell(_x, _y, constants.HighlightsTM.EMPTY); # cannot highlight floating tiles

	else:
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap_floating.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap.set_cell(_x, _y, constants.BoardTM.TILE_HIGHLIGHTED if _isSelected else constants.BoardTM.TILE);
		piece_tilemap.set_cell(_x, _y, util.getTMFromSquare(_squareEnum));
		highlights_tilemap.set_cell(_x, _y, _highlight);

###################### Relay these function calls down #########################
func add_engine(enginePath, player):
	grid_system.add_engine(enginePath, player)

######################### Relay these signals up ###############################
func _on_GridSystem_engine_log(player_num, text):
	emit_signal("engine_log", player_num, text)

# Both update this sfen and relay it to parent
func _on_GridSystem_sfen_update(p_sfen):
	sfen = p_sfen
	# Don't redraw board if it is not ready yet
	if is_instance_valid(grid_system):
		redraw_board()
	emit_signal("sfen_update", sfen)
