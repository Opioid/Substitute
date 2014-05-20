#pragma once

#include "Entity_manipulator.hpp"

namespace scene
{

class Editor
{

public:

	void update(const Scene& scene, const Ray3 &ray, const Camera& camera, const Input_state &mouse);

	const Entity_manipulator& get_entity_manipulator() const;

private:

	Entity_manipulator entity_manipulator_;
};

}
