extends Resource
const constants = preload("res://scripts/Constants.gd")

class StyleAddon extends Reference:
	# Style elements
	var squareEnum
	var isSelected:bool
	var isFloating:bool
	var highlight=constants.HighlightsTM.EMPTY
	# Coordinates
	var sel_first_x:int
	var sel_first_y:int
	# For rectangular selection
	var is_multi_tile:bool
	var sel_second_x:int
	var sel_second_y:int
#	var dest_first_x:int
#	var dest_first_y:int

	func _init(
			p_sel_first_x:int, 
			p_sel_first_y:int, 
			p_squareEnum=false, 
			p_isSelected:bool=false, 
			p_isFloating:bool=false, 
			p_highlight=constants.HighlightsTM.EMPTY, 
			p_is_multi_tile:bool=false, 
			# -1 is invalid, ignored when is_multi_tile
			p_sel_second_x:int=-1,
			p_sel_second_y:int=-1
#			p_dest_first_x:int=-1,
#			p_dest_first_y:int=-1
			):
		squareEnum = p_squareEnum
		isSelected = p_isSelected
		isFloating = p_isFloating
		highlight = p_highlight
		sel_first_x = p_sel_first_x
		sel_first_y = p_sel_first_y
		is_multi_tile = p_is_multi_tile
		if is_multi_tile:
			sel_second_x = p_sel_second_x
			sel_second_y = p_sel_second_y
		else: # for single tiles
			sel_second_x = sel_first_x
			sel_second_y = sel_first_y
