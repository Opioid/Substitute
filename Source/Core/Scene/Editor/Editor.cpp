#include "Editor.hpp"

namespace scene
{

void Editor::update(const Scene &scene, const Ray3 &ray, const Camera &camera, const Input_state &mouse)
{
	entity_manipulator_.update(scene, ray, camera, mouse);
}

const Entity_manipulator& Editor::get_entity_manipulator() const
{
	return entity_manipulator_;
}

}
