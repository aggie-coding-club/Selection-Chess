extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
onready var fileDialog = $FileDialog
onready var alertDialog = $AlertDialog
onready var importSfenDialog = $ImportSfenDialog

onready var gameSpaceViewport = $PanelContainer/VBoxContainer/VSplitContainer/HSplitContainer/Panel/GameSpaceViewportContainer/GameSpaceViewport
onready var gameSpace = gameSpaceViewport.get_node("GameSpace")
onready var menuBar = $PanelContainer/VBoxContainer/MenuBar

onready var modeButtonContainer = $PanelContainer/VBoxContainer/VSplitContainer/HSplitContainer/Panel/HBoxContainer
onready var pieceModeButton = modeButtonContainer.get_node("ButtonPieceMode")
onready var tileModeButton = modeButtonContainer.get_node("ButtonTileMode")
onready var deleteModeButton = modeButtonContainer.get_node("ButtonDeleteMove")
var modeButtonList = []

const constants = preload("res://scripts/Constants.gd")

# Called when the node enters the scene tree for the first time.
func _ready():
	menuBar.get_node("EngineButton").get_popup().connect("id_pressed", self, "_on_engine_button_pressed")
	menuBar.get_node("FileButton").get_popup().connect("id_pressed", self, "_on_file_button_pressed")

	# For exclusive mode buttons
	modeButtonList.append(pieceModeButton)
	modeButtonList.append(tileModeButton)
	modeButtonList.append(deleteModeButton)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

###############################################################################

func _on_file_button_pressed(id):
	match id:
		0:
			alertDialog.alert("Import PGN not implemented yet!")
		1:
			alertDialog.alert("Export PGN not implemented yet!")
		2: # import FEN
			importSfenDialog.popup_centered()
		3: # export FEN to clipboard
			
			var sfen = "this is a sfen lmao"

			OS.set_clipboard(sfen)
			alertDialog.alert("SFEN copied to clipboard")
	pass

# warning-ignore:unused_argument
func _on_engine_button_pressed(id):
	#print(menuBar.get_node("EngineButton").get_popup().get_item_text(id), " pressed")
	fileDialog.popup_centered()

# warning-ignore:unused_argument
func _on_game_button_pressed(id):
	pass

# warning-ignore:unused_argument
func _on_rules_button_pressed(id):
	pass

###############################################################################

func _on_FileDialog_file_selected(path):
	gameSpace.add_engine(path, 0) # Remember to pass Player color/number, instead of zero. Also figure out what to use for this

func _on_ImportSfenDialog_confirmed():
	var rawSfen = importSfenDialog.get_node("LineEdit").text
	print("got sfen [", rawSfen, "]")
	gameSpace.reset_sfen(rawSfen)
	# TODO: send to gameSpace cpp to initialize it, 
	# then ask for sfen back via redraw board.

###############################################################################
# TODO: Button section probably should be moved to Button's HBox container. 

func deselectModeButtons():
	for btn in modeButtonList:
		btn.pressed = false

func _on_ButtonPieceMode_pressed():
	deselectModeButtons()
	pieceModeButton.pressed = true
	gameSpace.cursorMode = constants.PIECE_MODE

func _on_ButtonTileMode_pressed():
	deselectModeButtons()
	tileModeButton.pressed = true
	gameSpace.cursorMode = constants.TILES_MODE

func _on_ButtonDeleteMove_pressed():
	deselectModeButtons()
	deleteModeButton.pressed = true
	gameSpace.cursorMode = constants.DELETE_MODE

func _on_GameSpace_sfen_update(text):
	print("recieved from cpp: sfen=",text)
