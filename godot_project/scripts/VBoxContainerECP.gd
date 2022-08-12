extends VBoxContainer


# Declare member variables here.
onready var console1 = $EngineConsolePanel1
onready var console2 = $EngineConsolePanel2

# Called when the node enters the scene tree for the first time.
func _ready():
	# TODO: there is probably a better way to set these
	console1.myPlayerNumber = 1 # White
	console2.myPlayerNumber = -1 # Black
	for console in self.get_children():
		print(console.logTextBody)
	



func _on_GameSpace_engine_log(player_num, text):
	var found_valid_num = false
	for console in self.get_children():
		if console.myPlayerNumber == player_num:
			found_valid_num = true
			console.engine_log(text)
	if not found_valid_num:
		print("Log for unknown player ", player_num, ": ", text)
