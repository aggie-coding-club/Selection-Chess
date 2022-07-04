extends ViewportContainer


# Declare member variables here. Examples:
onready var viewport = get_node("GameSpaceViewport")

func _ready():
	set_process_input(true)

func _unhandled_input(event):
	viewport.input(event)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
