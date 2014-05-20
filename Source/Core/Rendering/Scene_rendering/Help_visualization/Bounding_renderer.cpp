#include "Bounding_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Math/Intersection/Sphere.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Static_prop.hpp"
#include "Scene/AABB_tree/AABB_tree.hpp"
#include "Scene/AABB_tree/AABB_node.hpp"
#include "Scene/Light/Light.hpp"

namespace rendering
{

Bounding_renderer::Bounding_renderer(Rendering_tool& rendering_tool) : Scene_renderer(rendering_tool), vertices_(new float3[256])
{

}

Bounding_renderer::~Bounding_renderer()
{
	delete [] vertices_;
}

bool Bounding_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	effect_ = resource_manager.load<Effect>("Effects/Help_visualization/Wireframe.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position3x32::vertex_layout_description(), effect_->get_technique(0)->get_program()->get_signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_camera_.init(effect_, "Change_per_camera"))
	{
		return false;
	}

	if (!change_per_object_.init(effect_, "Change_per_object"))
	{
		return false;
	}

	if (!change_per_color_.init(effect_, "Change_per_color"))
	{
		return false;
	}

	vertex_buffer_ = rendering_tool_.get_device().create_vertex_buffer(sizeof(float3) * 256);
	if (!vertex_buffer_)
	{
		return false;
	}

	return create_render_states();
}

bool Bounding_renderer::on_resize_targets(const uint2& /*size*/, const Handle<Depth_stencil_shader_resource_view>& /*depth_stencil*/)
{
	return true;
}

void Bounding_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	auto& device = rendering_tool_.get_device();

	device.set_primitive_topology(Primitive_topology::Line_list);

	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);
	device.set_vertex_buffer(vertex_buffer_, sizeof(float3));

	const auto& camera = context.get_camera();

	effect_->use(device);

	change_per_camera_.get_data().view_projection = camera.get_view_projection();
	change_per_camera_.update(device);

	change_per_object_.get_data().world = float4x4::identity;
	change_per_object_.update(device);

	const Frustum& frustum = camera.get_frustum();

	render(scene.get_aabb_tree(), frustum);

	change_per_color_.get_data().color = color3::red;
	change_per_color_.update(device);

	auto& actors = scene.get_actors();

	for (auto a : actors)
	{
		if (frustum.intersect(a->get_aabb()))
		{
			render(a->get_aabb());
		}
	}

	change_per_color_.get_data().color = color3::yellow;
	change_per_color_.update(device);

	auto& lights = scene.get_lights();

	for (auto l : lights)
	{
		const scene::Light& light = *l;

		if (!light.is_visible())
		{
			continue;
		}

		if (scene::Light::Type::Point == light.get_type())
		{
			float radius = l->get_point_energy_and_range().w;
			Sphere sphere(l->get_world_position(), radius);

			if (frustum.intersect(sphere))
			{
				render(sphere);
			}
		}
		else if (scene::Light::Type::Spot == light.get_type())
		{
			Frustum frustum(light.calculate_view_projection());

			if (camera.get_frustum().intersect(frustum))
			{
				render(frustum);
			}
		}
	}
}

void Bounding_renderer::render(const scene::AABB_tree& tree, const Frustum& frustum)
{
	auto& device = rendering_tool_.get_device();

	{
		change_per_color_.get_data().color = color3::red;
		change_per_color_.update(device);

		auto& props = tree.get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				render(p->get_aabb());
			}
		}
	}

	const scene::AABB_node* node = tree.get_root();

	while (node)
	{
		if (!frustum.intersect(node->get_aabb()))
		{
			node = node->get_skip_node();
			continue;
		}

		change_per_color_.get_data().color = color3::red;
		change_per_color_.update(device);

		auto& props = node->get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				render(p->get_aabb());
			}
		}

		if (!node->has_children())
		{
			change_per_color_.get_data().color = color3::blue;
			change_per_color_.update(device);

			render(node->get_aabb());

			node = node->get_skip_node();
		}
		else
		{
			++node;
		}
	}
}

void Bounding_renderer::render(const AABB& aabb)
{
	float3 min = aabb.get_min();
	float3 max = aabb.get_max();

	// Front
	vertices_[0] = min;
	vertices_[1] = float3(max.x, min.y, min.z);
	vertices_[2] = float3(max.x, min.y, min.z);
	vertices_[3] = float3(max.x, max.y, min.z);
	vertices_[4] = float3(max.x, max.y, min.z);
	vertices_[5] = float3(min.x, max.y, min.z);
	vertices_[6] = float3(min.x, max.y, min.z);
	vertices_[7] = min;

	// Back
	vertices_[8]  = float3(min.x, min.y, max.z);
	vertices_[9]  = float3(max.x, min.y, max.z);
	vertices_[10] = float3(max.x, min.y, max.z);
	vertices_[11] = float3(max.x, max.y, max.z);
	vertices_[12] = float3(max.x, max.y, max.z);
	vertices_[13] = float3(min.x, max.y, max.z);
	vertices_[14] = float3(min.x, max.y, max.z);
	vertices_[15] = float3(min.x, min.y, max.z);

	// Bottom
	vertices_[16] = min;
	vertices_[17] = float3(min.x, min.y, max.z);
	vertices_[18] = float3(max.x, min.y, min.z);
	vertices_[19] = float3(max.x, min.y, max.z);

	// Top
	vertices_[20] = float3(min.x, max.y, min.z);
	vertices_[21] = float3(min.x, max.y, max.z);
	vertices_[22] = float3(max.x, max.y, min.z);
	vertices_[23] = max;

	auto& device = rendering_tool_.get_device();

	device.update_buffer(*vertex_buffer_, 0, 24 * sizeof(float3), vertices_);

	effect_->get_technique(0)->use();
	device.draw(24);
}

void Bounding_renderer::render(const Sphere& sphere)
{
	size_t maxi = 256 / 2 / 3;
	float3 tmp;

	for (size_t i = 0; i < maxi; ++i)
	{
		tmp = normalize(float3(sin(math::pi_mul_2 * float(i) / maxi), cos(math::pi_mul_2 * float(i) / maxi), 0));
		vertices_[i*2] = sphere.position + sphere.radius * tmp;

		tmp = normalize(float3(sin(math::pi_mul_2 * float(i + 1) / maxi), cos(math::pi_mul_2 * float(i + 1) / maxi), 0));
		vertices_[i*2+1] = sphere.position + sphere.radius * tmp;

		tmp = normalize(float3(0, cos(math::pi_mul_2 * float(i) / maxi), sin(math::pi_mul_2 * float(i) / maxi)));
		vertices_[i*2   + maxi*2] = sphere.position + sphere.radius * tmp;

		tmp = normalize(float3(0, cos(math::pi_mul_2 * float(i + 1) / maxi), sin(math::pi_mul_2 * float(i + 1) / maxi)));
		vertices_[i*2+1 + maxi*2] = sphere.position + sphere.radius * tmp;

		tmp = normalize(float3(cos(math::pi_mul_2 * float(i) / maxi), 0, sin(math::pi_mul_2 * float(i) / maxi)));
		vertices_[i*2   + maxi*4] = sphere.position + sphere.radius * tmp;

		tmp = normalize(float3(cos(math::pi_mul_2 * float(i + 1) / maxi), 0, sin(math::pi_mul_2 * float(i + 1) / maxi)));
		vertices_[i*2+1 + maxi*4] = sphere.position + sphere.radius * tmp;
	}

	auto& device = rendering_tool_.get_device();

	device.update_buffer(*vertex_buffer_, 0, 252 * sizeof(float3), vertices_);

	effect_->get_technique(0)->use();
	device.draw(252);
}

void Bounding_renderer::render(const Frustum& frustum)
{
	float3 p034 = Plane::intersection(frustum.planes_[0], frustum.planes_[3], frustum.planes_[4]);	// Right Bottom  Near
	float3 p134 = Plane::intersection(frustum.planes_[1], frustum.planes_[3], frustum.planes_[4]);	// Left  Bottom  Near

	float3 p024 = Plane::intersection(frustum.planes_[0], frustum.planes_[2], frustum.planes_[4]);	// Right Upper Near
	float3 p124 = Plane::intersection(frustum.planes_[1], frustum.planes_[2], frustum.planes_[4]);	// Left  Upper Near

	float3 p035 = Plane::intersection(frustum.planes_[0], frustum.planes_[3], frustum.planes_[5]);	// Right Bottom  Far
	float3 p135 = Plane::intersection(frustum.planes_[1], frustum.planes_[3], frustum.planes_[5]);	// Left  Bottom  Far

	float3 p025 = Plane::intersection(frustum.planes_[0], frustum.planes_[2], frustum.planes_[5]);	// Right Upper Far
	float3 p125 = Plane::intersection(frustum.planes_[1], frustum.planes_[2], frustum.planes_[5]);	// Left  Upper Far

	// Near
	vertices_[0] = p034;
	vertices_[1] = p134;
	vertices_[2] = p134;
	vertices_[3] = p124;
	vertices_[4] = p124;
	vertices_[5] = p024;
	vertices_[6] = p024;
	vertices_[7] = p034;

	// Far
	vertices_[8]  = p035;
	vertices_[9]  = p135;
	vertices_[10] = p135;
	vertices_[11] = p125;
	vertices_[12] = p125;
	vertices_[13] = p025;
	vertices_[14] = p025;
	vertices_[15] = p035;

	vertices_[16] = p034;
	vertices_[17] = p035;
	vertices_[18] = p134;
	vertices_[19] = p135;
	vertices_[20] = p024;
	vertices_[21] = p025;
	vertices_[22] = p124;
	vertices_[23] = p125;

	auto& device = rendering_tool_.get_device();

	device.update_buffer(*vertex_buffer_, 0, 24 * sizeof(float3), vertices_);

	effect_->get_technique(0)->use();
	device.draw(24);
}

bool Bounding_renderer::create_render_states()
{
	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;

	ds_state_ = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	blend_state_ = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

}
