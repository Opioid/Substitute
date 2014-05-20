#pragma once

#include "Math/Vector.hpp"

class Input_state;
struct Ray3;

namespace scene
{

class Scene;
class Camera;
class Entity;

class Entity_manipulator
{

public:
	
	enum class Axis
	{
		X,
		Y,
		Z,
		None
	};

	Entity_manipulator();

	void update(const Scene& scene, const Ray3 &ray, const Camera& camera, const Input_state &mouse);

	const Entity* get_selected_entity() const;
	const Entity* get_focused_entity() const;

	Axis get_translation_axis() const;

private:

	bool hit_in_previous_tick_;
	bool dragging_;

	float3 old_hit_;

	Entity* selected_entity_;
	Entity* focused_entity_;

	Axis translation_axis_;
	Axis rotation_axis_;
};

}
