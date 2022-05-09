extends Camera2D


# Declare member variables here. Examples:
const MIN_ZOOM: float = 0.1
const MAX_ZOOM: float = 1.0
const ZOOM_INCREMENT: float = 0.02
const ZOOM_RATE: float = 8.0
const PAN_ACCELERATION_KEY_INPUT: float = 0.2
const MAX_VELOCITY: float = 40.0

var _target_zoom: float = MIN_ZOOM
var camera_velocity: Vector2 = Vector2.ZERO

func zoom_in() -> void:
	_target_zoom = max(_target_zoom - ZOOM_INCREMENT, MIN_ZOOM)
	#set_physics_process(true)

func zoom_out() -> void:
	_target_zoom = min(_target_zoom + ZOOM_INCREMENT, MAX_ZOOM)
	#set_physics_process(true)

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func _physics_process(delta: float) -> void:
	# TODO: zoom based on cursor position
	zoom = lerp(zoom, _target_zoom * Vector2.ONE, ZOOM_RATE * delta)
	#set_physics_process(not is_equal_approx(zoom.x, _target_zoom))

	if Input.is_action_just_pressed("ui_reset_camera"):
		# TODO: based on position of tiles
		# TODO: lerp
		# TODO: scale to fit tiles
		position = Vector2.ZERO
		camera_velocity = Vector2.ZERO

	# Translate camera by panning	
	if Input.is_action_pressed("ui_left"):
		camera_velocity.x = lerp(camera_velocity.x, -MAX_VELOCITY, delta * PAN_ACCELERATION_KEY_INPUT * sqrt(zoom.x))
	elif Input.is_action_pressed("ui_right"):
		camera_velocity.x = lerp(camera_velocity.x, MAX_VELOCITY, delta * PAN_ACCELERATION_KEY_INPUT * sqrt(zoom.x))
	else:
		camera_velocity.x = 0 # TODO: gentler stop
	if Input.is_action_pressed("ui_up"):
		camera_velocity.y = lerp(camera_velocity.y, -MAX_VELOCITY, delta * PAN_ACCELERATION_KEY_INPUT * sqrt(zoom.y))
	elif Input.is_action_pressed("ui_down"):
		camera_velocity.y = lerp(camera_velocity.y, MAX_VELOCITY, delta * PAN_ACCELERATION_KEY_INPUT * sqrt(zoom.y))
	else:
		camera_velocity.y = 0
	if Input.is_action_pressed("ui_page_up"):
		zoom_out()
	elif Input.is_action_pressed("ui_page_down"):
		zoom_in()

	position += camera_velocity


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		if event.button_mask == BUTTON_MASK_MIDDLE:
			position -= event.relative * zoom
	if event is InputEventMouseButton:
		if event.is_pressed():
			if event.button_index == BUTTON_WHEEL_UP:
				zoom_in()
			if event.button_index == BUTTON_WHEEL_DOWN:
				zoom_out()
