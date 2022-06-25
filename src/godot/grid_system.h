#ifndef __GRID_SYSTEM_H__
#define __GRID_SYSTEM_H__

#include <Godot.hpp>
#include <Sprite.hpp>
#include <Node2D.hpp>

namespace godot {

class GridSystem : public Node2D {
	GODOT_CLASS(GridSystem, Node2D)

private:
	float time_passed;
	float time_emit;
	float amplitude;
	float speed;

public:
	static void _register_methods();

	GridSystem();
	~GridSystem();

	void _init(); // our initializer called by Godot

	void _process(float delta);
	void set_speed(float p_speed);
	float get_speed();
};

}

#endif
