extends Control


# Declare member variables here. 
# which player number this panel corresponds to. See chess/constants.hpp for 
# enums. Default is nether player.
var myPlayerNumber = 0
onready var logTextBody = $Panel/VBoxContainer/RichTextLabel
onready var btnCommentsEnabled = $Panel/VBoxContainer/HBoxContainer/CheckButton
# Also store the text so that we can add/remove comments from display
var consoleText = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

# Add lines of output to this console.
# Assumption: each comment does not contain leading whitespace and does not span
# multiple signals.
func engine_log(addedText):
	# TODO: format search results
	consoleText = addedText + consoleText
	
	if not is_instance_valid($Panel/VBoxContainer) or not is_instance_valid(btnCommentsEnabled) or not is_instance_valid(logTextBody):
		print("console still loading, dropped text:", addedText)
		return

	# add text to display unless it is a comment and not enabled
	if btnCommentsEnabled.pressed or addedText[0] != '#':
		logTextBody.text = addedText + logTextBody.text
	
