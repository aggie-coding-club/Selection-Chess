# Pass on gui input recieved by viewport panel to viewport. 
# See https://docs.godotengine.org/en/stable/tutorials/inputs/inputevent.html for help

extends ViewportContainer

# Declare member variables here.
onready var viewport = get_node("GameSpaceViewport")

func _ready():
	set_process_input(true)

func _gui_input(event):
	viewport.input(event)
#	viewport.unhandled_input(event)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
