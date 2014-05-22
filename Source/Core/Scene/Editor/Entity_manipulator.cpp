#include "Entity_manipulator.hpp"
#include "Scene/Scene.hpp"
#include "Math/Ray.hpp"
#include "Math/Intersection/Sphere.hpp"
#include "Math/Intersection/Cylinder.hpp"
#include "Controls/Input_state.hpp"

namespace scene
{

Entity_manipulator::Entity_manipulator() : hit_in_previous_tick_(false), dragging_(false), selected_entity_(nullptr), focused_entity_(nullptr), translation_axis_(Axis::None), rotation_axis_(Axis::None)
{}

void Entity_manipulator::update(const Scene& scene, const Ray3 &ray, const Camera& camera, const Input_state &mouse)
{
	auto& entities = scene.get_entities();

	focused_entity_ = nullptr;

//	if (!mouse.isKeyDown(0))
	{
		for (Entity* entity : entities)
		{
			if (entity->get_parent())
			{
				continue;
			}

			float dist = distance(camera.get_world_position(), entity->get_world_position());

			if (ray.intersect(Sphere(entity->get_world_position(), dist * 0.006f)))
			{
				focused_entity_ = entity;
				break;
			}
		}
	}

	if (Axis::None == translation_axis_ && mouse.is_key_click(0))
	{
		if (focused_entity_)
		{
			selected_entity_ = focused_entity_;
		}
		else
		{
			selected_entity_ = nullptr;
			translation_axis_ = Axis::None;
		}
	}

	if (selected_entity_ && !dragging_)
	{
		float3 pos = selected_entity_->get_world_position();

		translation_axis_ = Axis::None;
		/*
		if (ray.intersect(Cylinder(pos + 0.175f * selected_entity_->get_world_right(), selected_entity_->get_world_right(), 0.03f, 0.35f)))
		{
			translation_axis_ = Axis::X;
		}
		else if (ray.intersect(Cylinder(pos + 0.175f * selected_entity_->get_world_up(), selected_entity_->get_world_up(), 0.03f, 0.35f)))
		{
			translation_axis_ = Axis::Y;
		}
		else if (ray.intersect(Cylinder(pos + 0.175f * selected_entity_->get_world_dir(), selected_entity_->get_world_dir(), 0.03f, 0.35f)))
		{
			translation_axis_ = Axis::Z;
		}
		*/

		if (ray.intersect(Sphere(pos + 0.175f * selected_entity_->get_world_right(), 0.1f)))
		{
			translation_axis_ = Axis::X;
			hit_in_previous_tick_ = true;
		}
		else if (ray.intersect(Sphere(pos + 0.175f * selected_entity_->get_world_up(), 0.1f)))
		{
			translation_axis_ = Axis::Y;
			hit_in_previous_tick_ = true;
		}
		else if (ray.intersect(Sphere(pos + 0.175f * selected_entity_->get_world_direction(), 0.1f)))
		{
			translation_axis_ = Axis::Z;
			hit_in_previous_tick_ = true;
		}
	}

	if (mouse.is_key_down(0) && Axis::None != translation_axis_)
	{
		float3 a;

		switch (translation_axis_)
		{
		case Axis::X:
			a = selected_entity_->get_world_right();
			break;
		case Axis::Y:
			a = selected_entity_->get_world_up();
			break;
		case Axis::Z:
		default:
			a = selected_entity_->get_world_direction();
			break;
		}

		Plane p(cross(a, camera.get_world_right()), selected_entity_->get_world_position());

		float dist;
		ray.intersect(p, dist);

		float3 hit = ray.org + dist * ray.dir;

		if (dragging_)
		{
			float3 dif = hit - old_hit_;

			selected_entity_->set_local_position(selected_entity_->get_world_position() + project(dif, a));
		}

		old_hit_ = hit;

		dragging_ = true;

		if (!selected_entity_->is_interpolated())
		{
			selected_entity_->fix_world_transformation();
		}
	}
	else if (!mouse.is_key_down(0))
	{
		dragging_ = false;
	}

	/*
	float dist = 0.f;

	size_t i = 0;

	if (!old_hit_)
	{
		for (; i < entities.size(); ++i)
		{
			Entity* entity = entities[i];

			if (entity->get_parent())
			{
				continue;
			}

			dist = distance(camera.get_world_position(), entity->get_world_position());

			if (ray.intersectSphere(entity->get_world_position(), dist * 0.0055f))
			{
				selected_entity_ = entity;
				old_hit_ = true;
				m_rotateAxis = Axis::None;
				break;
			}
			else if (ray.intersectSphere(entity->get_world_position() + 0.25f * entity->get_world_dir(), dist * 0.0055f))
			{
				selected_entity_ = entity;
				old_hit_ = true;
				m_rotateAxis = Axis::X;
				break;
			}
			else if (ray.intersectSphere(entity->get_world_position() + 0.25f * entity->get_world_up(), dist * 0.0055f))
			{
				selected_entity_ = entity;
				old_hit_ = true;
				m_rotateAxis = Axis::Y;
				break;
			}
			else if (ray.intersectSphere(entity->get_world_position() + 0.25f * entity->get_world_right(), dist * 0.0055f))
			{
				selected_entity_ = entity;
				old_hit_ = true;
				m_rotateAxis = Axis::Z;
				break;
			}
		}
	}

	if (i == entities.size() && !old_hit_)
	{
		selected_entity_ = nullptr;
		return;
	}


	if (mouse.isKeyDown(0))
	{
		if (Axis::None == m_rotateAxis)
		{
			float3 dir = float3::identity;

			switch (m_axis)
			{
				case Axis::X: dir.y = -1.f; break;
				case Axis::Y: dir.z =  1.f; break;
				case Axis::Z: dir.x =  1.f; break;
			}

			float dist = 0.0f;
			if (ray.intersect(Plane(dir, selected_entity_->get_world_position()), dist))
			{
				float3 pos = ray.org + dist * ray.dir;

				//TODO: take parents rotation into account
				float3 dif = selected_entity_->get_world_position() - pos;

				selected_entity_->set_local_position(selected_entity_->get_local_position() - dif);
			}
		}
		else
		{

			float3 v0, v1;

			switch (m_rotateAxis)
			{
			case axis_X: v0 = m_selectedObject->getRight(); v1 = m_selectedObject->get_world_up();    break;
			case axis_Y: v0 = m_selectedObject->getDir();   v1 = m_selectedObject->get_world_right(); break;
			case axis_Z: v0 = m_selectedObject->getUp();    v1 = m_selectedObject->get_world_dir();   break;
			}

			float dist = 0.f;
			if (ray.intersect(Plane(v0, m_selectedObject->getWorldPos()), dist))
			{
			float3 dir = normalize((ray.org + dist * ray.dir) - m_selectedObject->getWorldPos());

			float angle = dot(dir, v1);
			float3x3 rot;
			set_local_rotation(rot, v0, angle);

			rot = normalize(rot);

			m_selectedObject->setRot(m_selectedObject->getRot() * rot);
			}
		}
	}
	else
	{
		old_hit_ = false;
	}
	*/
}

const Entity* Entity_manipulator::get_selected_entity() const
{
	return selected_entity_;
}

const Entity* Entity_manipulator::get_focused_entity() const
{
	return focused_entity_;
}

Entity_manipulator::Axis Entity_manipulator::get_translation_axis() const
{
	return translation_axis_;
}

}
