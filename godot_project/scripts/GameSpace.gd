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

var prevChunk
var sfen = ""

var sfen_placement
var sfen_min_corner
var sfen_turn
var sfen_50move
var sfen_moveCount

################# Selection and cursor state variables ####################
# What type of selection is made, if any. 
var selectionState = constants.NO_SEL

# as integer board coords
var selectionStartPos = Vector2i.new(0,0)
var selectionEndPos = Vector2i.new(0,0)

# Which 'tool' is selected, defines what clicking does
var cursorMode = constants.PIECE_MODE

# Stores state of whether cursor is pressed or not.
# In the case of piece_sel, means piece is actively following cursor.
# In the case of tile_sel, means rectangle is being drawn.
var dragging = false
################################################################################

# Called when the node enters the scene tree for the first time.
func _ready():
	prevChunk = get_chunk()
	redraw_board()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var chunk = get_chunk()
	if not prevChunk.equals(chunk):
		print("new chunk:", chunk, " old chunk:", prevChunk)
		prevChunk = chunk
		redraw_board()

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

func getCoordsFromGlobalPos(var p_global)->Vector2i:
	var indicesRaw = board_tilemap.world_to_map(board_tilemap.to_local(p_global));
	# Note that y is negated so that up is increased coords
	var indices = Vector2i.new(int(indicesRaw.x), int(-indicesRaw.y));
	return indices;

func get_chunk()->Vector2i:
	# TileMap indices where center of camera currently is
	var cameraIndices = getCoordsFromGlobalPos(camera.get_global_position());
	# Get chunk camera center is in. Have to convert to signed so division works as expected.
	var cameraChunk = Vector2i.new(
		util.div_floor(cameraIndices.x, constants.DAModulus), 
		util.div_floor(cameraIndices.y, constants.DDModulus))
	return cameraChunk;

func redraw_board():
	var x = constants.PIECE_SEL
	var why = sutil.letters_to_int("ac")
	print("sutils: ", why, " <-> ", sutil.int_to_letters(why))
	var vec2i = util.Vector2iFromArray(sutil.algebraic_to_coords("ac98"))
	print("sutils array: ", vec2i)
	print("<-> ", sutil.coords_to_algebraic(vec2i.x, vec2i.y))
	print("redrawing...")
	piece_tilemap.clear()
	board_tilemap.clear()
	highlights_tilemap.clear()
	board_tilemap_floating.clear()
	board_tilemap_floating.clear()

#	var baseCoords = util.Vector2iFromArray(sutil.algebraic_to_coords(sfen_min_corner))
#
#	var centerChunk = get_chunk()
#	print("centerchunk f=", centerChunk.x, " r=", centerChunk.y)

	# Draw board in each chunk. This achieves the 'wrap-around' effect, as the center chunk is surrounded by other
	# chunks, so if we are at the edge of the center chunk we see a duplicate of its other side.
	# This can also be interpretted as there are infinite chunks in each direction, and we only load the 9 that we 
	# might see.
#	for chunkNumOffsetR in range(-1, 1+1):
#		for chunkNumOffsetF in range(-1, 1+1):
#			# Draw board
#			var chunkOrigin = Vector2i.new(centerChunk.x + chunkNumOffsetF, centerChunk.y + chunkNumOffsetR)

#				for (auto f = 0; f < sa.m_dimensions.file; ++f) {
#					for (auto r = 0; r < sa.m_dimensions.rank; ++r) {
#						SquareEnum square = sa.at(f, r);
#						if (square >= VOID) continue;
#
#						// Copy and modify coords in DMod space
#						DModCoords tileCoordsDM = baseCoords;
#						tileCoordsDM.file += f;
#						tileCoordsDM.rank -= r; // FIXME: why is rank already negated here?
#
#						// Convert DMod coords into SignCoords
#						SignedCoords tileCoords (chunkOrigin.file*DAModulus + tileCoordsDM.file.m_value, chunkOrigin.rank*DDModulus + tileCoordsDM.rank.m_value);
#						setCell(tileCoords.file, -tileCoords.rank, square);
#					}
#				}
#				// setCell(chunkOrigin.file * DAModulus, -chunkOrigin.rank * DDModulus, B_ROOK);
#			}
#		}
#		// setCell(centerChunk.file * DAModulus, -centerChunk.rank * DDModulus, W_ROOK);

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

###################### Relay these function calls down #########################
func add_engine(enginePath, player):
	grid_system.add_engine(enginePath, player)

######################### Relay these signals up ###############################
func _on_GridSystem_engine_log(player_num, text):
	emit_signal("engine_log", player_num, text)

# Both update this sfen and relay it to parent
func _on_GridSystem_sfen_update(p_sfen):
	sfen = p_sfen
	redraw_board()
	emit_signal("sfen_update", sfen)
