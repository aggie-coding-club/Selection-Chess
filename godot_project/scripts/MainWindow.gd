extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
onready var fileDialog = $FileDialog
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
	var engineButton = 	menuBar.get_node("EngineButton")
	engineButton.get_popup().connect("id_pressed", self, "_on_engine_button_pressed")

	modeButtonList.append(pieceModeButton)
	modeButtonList.append(tileModeButton)
	modeButtonList.append(deleteModeButton)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

# warning-ignore:unused_argument
func _on_engine_button_pressed(id):
	#print(menuBar.get_node("EngineButton").get_popup().get_item_text(id), " pressed")
	fileDialog.popup_centered()

func _on_FileDialog_file_selected(path):
	gameSpace.add_engine(path, 0) # Remember to pass Player color/number, instead of zero. Also figure out what to use for this

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
