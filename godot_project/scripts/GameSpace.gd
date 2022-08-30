extends Node2D

const constants = preload("res://scripts/Constants.gd")
const util = preload("res://scripts/Utils.gd")
const sutilClass = preload("res://StaticUtils.gdns")
const styleAddon = preload("res://scripts/TMModifiers.gd")
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

var style_addons = []

################# Selection and cursor state variables ####################
# What type of selection is made, if any. 
var selectionState = constants.SEL.NONE

# as integer board coords
var selectionStartPos = util.Vector2i.new(0,0)
var selectionEndPos = util.Vector2i.new(0,0)

# Which 'tool' is selected, defines what clicking does
var cursorMode = constants.PIECE_MODE

# Stores state of whether cursor is is_pressed() or not.
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

# Calls set_cell on Godot boards to change the displayed cell.
# Use _isSelected, _isFloating, and _highlight to change decorators.
# _squareEnum is of enum constants.SquareEnum, or false to preserve current piece.
# _highlight is of enum constants.HighlightTM.
# Behavior for invalid combinations of decorators is undefined.
# CAUTION: just like set_cell on individual boards, the y needs to be negated
func set_cell_formatted(_x:int,_y:int, _squareEnum, _isSelected:bool=false, _isFloating:bool=false, _highlight=constants.HighlightsTM.EMPTY):
	var preserve_piece = typeof(_squareEnum) == TYPE_BOOL
	var square_tm_value
	if preserve_piece:
		# Set _squareEnum from existing board data, either regular or floating
		square_tm_value = piece_tilemap.get_cell(_x, _y) if piece_tilemap.get_cell(_x, _y) != constants.TM_ENUM_NULL else piece_tilemap_floating.get_cell(_x, _y)
	else: 
		square_tm_value = util.getTMFromSquare(_squareEnum)

	if typeof(_squareEnum) == TYPE_INT and _squareEnum == constants.SquareEnum.VOID:
		board_tilemap.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap_floating.set_cell(_x, _y, constants.PieceTM.EMPTY);
		highlights_tilemap.set_cell(_x, _y, constants.HighlightsTM.EMPTY);

	elif _isFloating:
		board_tilemap.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.TILE_HIGHLIGHTED if _isSelected else constants.BoardTM.TILE);
		piece_tilemap_floating.set_cell(_x, _y, square_tm_value)
		highlights_tilemap.set_cell(_x, _y, constants.HighlightsTM.EMPTY); # cannot highlight floating tiles

	else:
		board_tilemap_floating.set_cell(_x, _y, constants.BoardTM.EMPTY);
		piece_tilemap_floating.set_cell(_x, _y, constants.PieceTM.EMPTY);
		board_tilemap.set_cell(_x, _y, constants.BoardTM.TILE_HIGHLIGHTED if _isSelected else constants.BoardTM.TILE);
		piece_tilemap.set_cell(_x, _y, square_tm_value);
		highlights_tilemap.set_cell(_x, _y, _highlight);

# drops the thing currently being dragged, RESETTING to original position
func drop_dragged():
	if not dragging:
		return
	if selectionState == constants.SEL.FROM:
		#piece_tilemap.set_cellv(selectionStartPos, [PIECE])
		# TODO: get piece back from whatever is carrying it
		pass
	if selectionState == constants.SEL.FROM:
		# TODO: 
		pass

# remove highlight and other stuff
func deselect():
	if dragging:
		drop_dragged()
	match selectionState:
		constants.SEL.FROM:
			# TODO
			pass
		constants.SEL.FROM:
			# TODO
			pass
	selectionState = constants.SEL.NONE
	style_addons.clear()
	redraw_board()

################################################################################

func _input(event):
#func _unhandled_input(event):
	# Mouse in viewport coordinates.
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			# position of mouse as NodeBoard indices 
			var mousePosNB = getCoordsFromGlobalPos(get_global_mouse_position())
			print("Mouse Click/Unclick at: ", get_global_mouse_position(), "\t which is: ", mousePosNB)
			print("state is ", selectionState)
			match cursorMode:
				constants.PIECE_MODE:
					if selectionState == constants.SEL.NONE and event.is_pressed():
						selectionStartPos = mousePosNB.clone()
						# TODO: check if valid selectable piece

						selectionState = constants.SEL.FROM
						# We don't drag piece until cursor moves
						# Godot TileMap y is reverse of chess standard
						style_addons.append(styleAddon.StyleAddon.new(mousePosNB.x, -mousePosNB.y, false, true))
						redraw_board()
					elif selectionState == constants.SEL.FROM:
						if event.is_pressed() and mousePosNB.equals(selectionStartPos):
							deselect()
						elif event.is_pressed() and not mousePosNB.equals(selectionStartPos):
							print(mousePosNB, selectionStartPos, "..")
							# TODO: execute move. Make sure this works whether move is execute immediately, or it sits in the SEL.TO stage until submit button pressed.
							deselect() # TODO: temporary til move implemented
						elif not event.is_pressed():
							if mousePosNB.equals(selectionStartPos):
								drop_dragged()
							else:
								# execute piece move
								pass
				constants.TILES_MODE:
					if selectionState == constants.SEL.NONE and event.is_pressed():
						selectionStartPos = mousePosNB.clone()
						dragging = true
					elif selectionState == constants.SEL.NONE and dragging and not event.is_pressed():
						selectionEndPos = mousePosNB.clone()
						dragging = false
						# TODO: check if valid selection
						if true:
							selectionState = constants.SEL.FROM
							# Godot TileMap y is reverse of chess standard
							var sa = styleAddon.StyleAddon.new(selectionStartPos.x, -selectionStartPos.y,
								false, true, true, constants.HighlightsTM.EMPTY,
								true, selectionEndPos.x, -selectionEndPos.y)
							style_addons.append(sa)
							redraw_board()
						else:
							pass
					if selectionState == constants.SEL.FROM and event.is_pressed():
						# TODO: check if pressed down on tile within selection
						if true:
							# TODO: calculate where to offset floating selection based on this click
							dragging = true
						else:
							deselect()
					elif selectionState == constants.SEL.FROM and dragging and not event.is_pressed():
						# TODO: check if destination is legal
						if false:
							# drop selection onto the board, set corresponding variables about move info
							pass
						else:
							# otherwise, reset back to when we first made selection
							drop_dragged()
							dragging = false
				constants.DELETE_MODE:
					if event.is_pressed():
#						var tileClick = mousePosNB.clone()
						# TODO: check if clicked tile is already marked for deletion
						if true:
							# TODO: deselect just that tile
							pass
						else:
							# TODO: add that tile to deletion list
							pass
						# TODO: update selectionState if necessary (tile deletions might not even need this!)
					
	elif event is InputEventMouseMotion:
#		if selectionState == constants.SEL.FROM:
#			# just started dragging a piece
#			if not dragging and Input.is_mouse_button_pressed(BUTTON_LEFT):
#				var mousePosNB = getCoordsFromGlobalPos(get_global_mouse_position())
#				# TODO: remove from pieceTileMap, move to floating map
#				dragging = true
#
#			if dragging:
#				var mousePosNB = getCoordsFromGlobalPos(get_global_mouse_position())
#				selectionEndPos = mousePosNB.clone()
#				# TODO: update graphics
#
#		if selectionState == constants.SEL.FROM:
#			# TODO
			pass
	# Print the size of the viewport.
	#print("Viewport Resolution is: ", get_viewport_rect().size)

########### Math helper functions for coordinate conversion stuff ##############

# Get the gamespace coords from a global position (already crossed negation barrier)
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

########## functions for redraw_board, since lambdas not allowed ###############
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
################################################################################

func redraw_board():
	var why = sutil.letters_to_int("ac")
#	print("sutils: ", why, " <-> ", sutil.int_to_letters(why))
	var vec2i = util.Vector2iFromArray(sutil.algebraic_to_coords("ac98"))
#	print("sutils array: ", vec2i)
#	print("<-> ", sutil.coords_to_algebraic(vec2i.x, vec2i.y))
#	print("redrawing using sfen [", sfen, "]")
	piece_tilemap.clear()
	board_tilemap.clear()
	highlights_tilemap.clear()
	piece_tilemap_floating.clear()
	board_tilemap_floating.clear()

	mod_printhead = util.Vector2iFromArray(sutil.algebraic_to_coords(util.get_slice(sfen, " ", sfen_index_min_corner)))
	mod_printhead_reset = mod_printhead.clone()

	var sfen_placement = util.get_slice(sfen, " ", sfen_index_placement)
#	print("before: ", sfen_placement)
	sfen_placement = util.reverse_sfen(sfen_placement)
#	print("after: ", sfen_placement)

	var centerChunk = chunk
#	print("centerchunk f=", centerChunk.x, " r=", centerChunk.y)

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

	# Apply addons
	for addon in style_addons:
		for x_i in range(addon.sel_first_x, addon.sel_second_x+1):
			for y_i in range(addon.sel_first_y, addon.sel_second_y+1):
				set_cell_formatted(x_i, y_i, addon.squareEnum, addon.isSelected, addon.isFloating, addon.highlight)	

###################### Relay these function calls down #########################
func add_engine(enginePath, player):
	grid_system.add_engine(enginePath, player)

func reset_sfen(rawSfen):
	grid_system.reset_sfen(rawSfen)
	
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
