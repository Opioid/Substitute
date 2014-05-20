#include "Editor_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Editor/Editor.hpp"

namespace rendering
{

Editor_renderer::Editor_renderer(Rendering_tool &rendering_tool) : Scene_renderer(rendering_tool)
{}

Editor_renderer::~Editor_renderer()
{}

bool Editor_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	effect_ = resource_manager.load<Effect>("Effects/Help_visualization/Editor_renderer.effect");
	if (!effect_)
	{
		return false;
	}

	per_shape_color_technique_  = effect_->get_technique("Per_shape_color");
	per_vertex_color_technique_ = effect_->get_technique("Per_vertex_color");
	shape_technique_            = effect_->get_technique("Shape");

	auto& vertex_layout_cache = rendering_tool_.get_vertex_layout_cache();

	per_shape_color_input_layout_ = vertex_layout_cache.get_input_layout(*Vertex_position3x32::vertex_layout_description(), per_shape_color_technique_->get_program()->get_signature());
	if (!per_shape_color_input_layout_)
	{
		return false;
	}

	per_vertex_color_input_layout_ = vertex_layout_cache.get_input_layout(*Vertex_position3x32_color1x32::vertex_layout_description(), per_vertex_color_technique_->get_program()->get_signature());
	if (!per_vertex_color_input_layout_)
	{
		return false;
	}

	shape_input_layout_ = vertex_layout_cache.get_input_layout(*Vertex_position3x32_normal3x32::vertex_layout_description(), shape_technique_->get_program()->get_signature());
	if (!shape_input_layout_)
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

	if (!init_buffers())
	{
		return false;
	}

	return create_render_states();
}

void Editor_renderer::render(const scene::Scene& scene, const scene::Editor& editor, const Rendering_context& context)
{
	auto& device = rendering_tool_.get_device();

	device.set_framebuffer(context.get_framebuffer());
	device.clear_depth_stencil(context.get_framebuffer(), 1.f, 0);

	device.set_rasterizer_state(rasterizer_state_);
	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_primitive_topology(Primitive_topology::Line_list);
	device.set_input_layout(per_vertex_color_input_layout_);

	device.set_vertex_buffer(vertex_buffer_, sizeof(Vertex_position3x32_color1x32));

	effect_->use(device);

	const auto& camera = context.get_camera();

	auto& change_per_camera_data = change_per_camera_.get_data();
	change_per_camera_data.view_projection = camera.get_view_projection();
	change_per_camera_data.view            = camera.get_view();
	change_per_camera_.update(device);

	auto& entities = scene.get_entities();

	per_vertex_color_technique_->use();

	auto& change_per_object_data = change_per_object_.get_data();

	for (auto entity : entities)
	{
		if (entity->get_parent())
		{
			continue;
		}

		float4x4 world = float4x4::identity;
		set_basis(world, entity->get_world_rotation());
		set_origin(world, entity->get_world_position());

		change_per_object_data.world = world;
		change_per_object_.update(device);

		device.draw(6);
	}

	shape_technique_->use();

	const float3 sphere_scale(0.05f, 0.05f, 0.05f);

	const scene::Entity_manipulator& entity_manipulator = editor.get_entity_manipulator();

	const scene::Entity* entity = entity_manipulator.get_focused_entity();
	if (entity)
	{
		device.set_primitive_topology(Primitive_topology::Triangle_list);
		device.set_input_layout(shape_input_layout_);

		float4x4 world = float4x4::identity;
		set_origin(world, entity->get_world_position());

		scale(world, sphere_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color3::yellow;
		change_per_object_.update(device);

		sphere_.render(rendering_tool_);
	}

	entity = editor.get_entity_manipulator().get_selected_entity();
	if (entity)
	{
		device.set_primitive_topology(Primitive_topology::Triangle_list);
		device.set_input_layout(shape_input_layout_);

		float4x4 world = float4x4::identity;
		set_origin(world, entity->get_world_position());

		scale(world, sphere_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color3::yellow;
		change_per_object_.update(device);

		sphere_.render(rendering_tool_);
		const float3 cone_scale(0.06f, 0.15f, 0.06f);
		const float3 cylinder_scale(0.03f, 0.35f, 0.03f);

		float3x3 rot;

		// Dir cone
		Color3 color = scene::Entity_manipulator::Axis::Z == entity_manipulator.get_translation_axis() ? Color3(0.25f, 0.25f, 1.f) : Color3(0.f, 0.f, 1.f);

		set_rotation_x(rot, -0.5f * math::pi);
		set_basis(world, rot * entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.35f * entity->get_world_direction());
		scale(world, cone_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cone_.render(rendering_tool_);

		// Dir cylinder
		set_rotation_x(rot, -0.5f * math::pi);
		set_basis(world, rot * entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.5f * 0.35f * entity->get_world_direction());
		scale(world, cylinder_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cylinder_.render(rendering_tool_);

		// Up cone
		color = scene::Entity_manipulator::Axis::Y == entity_manipulator.get_translation_axis() ? Color3(0.25f, 1.f, 0.25f) : Color3(0.f, 1.f, 0.f);

		set_basis(world, entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.35f * entity->get_world_up());
		scale(world, cone_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cone_.render(rendering_tool_);

		// Up cylinder
		set_basis(world, entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.5f * 0.35f * entity->get_world_up());
		scale(world, cylinder_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cylinder_.render(rendering_tool_);

		// Right cone
		color = scene::Entity_manipulator::Axis::X == entity_manipulator.get_translation_axis() ? Color3(1.f, 0.25f, 0.25f) : Color3(1.f, 0.f, 0.f);

		set_rotation_z(rot, 0.5f * math::pi);
		set_basis(world, rot * entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.35f * entity->get_world_right());
		scale(world, cone_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cone_.render(rendering_tool_);

		// Right cylinder
		set_rotation_z(rot, 0.5f * math::pi);
		set_basis(world, rot * entity->get_world_rotation());
		set_origin(world, entity->get_world_position() + 0.5f * 0.35f * entity->get_world_right());
		scale(world, cylinder_scale);

		change_per_object_data.world = world;
		change_per_object_data.color = color;
		change_per_object_.update(device);

		cylinder_.render(rendering_tool_);

		if (scene::Entity_manipulator::Axis::None != entity_manipulator.get_translation_axis())
		{
			if (scene::Entity_manipulator::Axis::Z == entity_manipulator.get_translation_axis())
			{
				set_rotation_x(rot, -0.5f * math::pi);
				set_basis(world, rot * entity->get_world_rotation());

				change_per_object_data.color = color3::blue;
			}
			else if (scene::Entity_manipulator::Axis::Y == entity_manipulator.get_translation_axis())
			{
				set_basis(world, entity->get_world_rotation());

				change_per_object_data.color = color3::green;
			}
			else if (scene::Entity_manipulator::Axis::X == entity_manipulator.get_translation_axis())
			{
				set_rotation_z(rot, 0.5f * math::pi);
				set_basis(world, rot * entity->get_world_rotation());

				change_per_object_data.color = color3::red;
			}

			device.set_primitive_topology(Primitive_topology::Line_list);
			device.set_input_layout(per_shape_color_input_layout_);

			set_origin(world, entity->get_world_position());

			change_per_object_data.world = world;
			change_per_object_.update(device);

			per_shape_color_technique_->use();

			line_.render(rendering_tool_);
		}
	}
}

bool Editor_renderer::on_resize_targets(const uint2& /*size*/, const Handle<Depth_stencil_shader_resource_view>& /*depth_stencil*/)
{
	return true;
}

bool Editor_renderer::init_buffers()
{
	const size_t num_vertices = 6 + 20 + 48;
	Vertex_position3x32_color1x32* vertices = new Vertex_position3x32_color1x32[num_vertices];

	// axis
	vertices[0].position = float3::identity;
	vertices[0].color    = static_cast<unsigned int>(color4::red);

	vertices[1].position = float3(0.25f, 0.f, 0.f);
	vertices[1].color    = vertices[0].color;

	vertices[2].position = float3::identity;
	vertices[2].color    = static_cast<unsigned int>(color4::green);

	vertices[3].position = float3(0.f, 0.25f, 0.f);
	vertices[3].color    = vertices[2].color;

	vertices[4].position = float3::identity;
	vertices[4].color    = static_cast<unsigned int>(color4::blue);

	vertices[5].position = float3(0.f, 0.f, 0.25f);
	vertices[5].color    = vertices[4].color;

	vertex_buffer_ = rendering_tool_.get_device().create_vertex_buffer(sizeof(Vertex_position3x32_color1x32) * num_vertices, vertices);

	delete [] vertices;

	if (!vertex_buffer_)
	{
		return false;
	}

	if (!cone_.init(rendering_tool_))
	{
		return false;
	}

	if (!cylinder_.init(rendering_tool_))
	{
		return false;
	}

	if (!sphere_.init(rendering_tool_))
	{
		return false;
	}

	if (!line_.init(rendering_tool_))
	{
		return false;
	}

	return true;
}

bool Editor_renderer::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable     = true;
	ds_description.depth_write_mask = true;
	ds_description.comparison_func  = Depth_stencil_state::Description::Comparison::Less;

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

void Editor_renderer::Shape::render(Rendering_tool& rendering_tool) const
{
	auto& device = rendering_tool.get_device();

	device.set_vertex_buffer(vertex_buffer_, sizeof(Vertex_position3x32_normal3x32));
	device.set_index_buffer(index_buffer_);

	device.draw_indexed(num_indices_);
}

bool Editor_renderer::Cone::init(Rendering_tool& rendering_tool)
{
	const uint32_t num_segments = 64;
	const uint32_t num_vertices = 2 * num_segments + 2;
	const uint32_t end_vertices = 2 * num_segments;
	Vertex_position3x32_normal3x32 vertices[num_vertices];

	const size_t max = num_segments / 2;

	for (size_t i = 0; i < max; ++i)
	{
		vertices[i * 2].position  = normalize(float3(cos(math::pi_mul_2 * float(i) / max), 0.f, sin(math::pi_mul_2 * float(i) / max)));
		vertices[i * 2].normal = vertices[i * 2].position;

		vertices[i * 2 + 1].position  = normalize(float3(cos(math::pi_mul_2 * float(i + 1) / max), 0.f, sin(math::pi_mul_2 * float(i + 1) / max)));
		vertices[i * 2 + 1].normal = vertices[i * 2 + 1].position;

		vertices[i * 2 + max * 2].position  = vertices[i * 2].position;
		vertices[i * 2 + max * 2].normal = float3(0.f, -1.f, 0.f);

		vertices[i * 2 + 1 + max * 2].position  = vertices[i * 2 + 1].position;
		vertices[i * 2 + 1 + max * 2].normal = float3(0.f, -1.f, 0.f);
	}

	vertices[end_vertices].position      = float3(0.f, 1.f, 0.f);
	vertices[end_vertices].normal     = float3(0.f, 1.f, 0.f);
	vertices[end_vertices + 1].position  = float3(0.f,  0.f, 0.f);
	vertices[end_vertices + 1].normal = float3(0.f, -1.f, 0.f);

	vertex_buffer_ = rendering_tool.get_device().create_vertex_buffer(sizeof(Vertex_position3x32_normal3x32) * num_vertices, vertices);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = num_segments * 3 * 2;
	unsigned short indices[num_segments * 3 * 2];

	unsigned short num = static_cast<unsigned short>(num_segments);

	for (unsigned short i = 0; i < num; ++i)
	{
		indices[i * 3 + 0] = i;
		indices[i * 3 + 1] = end_vertices;
		indices[i * 3 + 2] = i < num - 1 ? i + 1 : 0;

		indices[i * 3 + 0 + num_segments * 3] = i < num - 1 ? i + 1 + num : num;
		indices[i * 3 + 1 + num_segments * 3] = end_vertices + 1;
		indices[i * 3 + 2 + num_segments * 3] = i + num;
	}

	index_buffer_ = rendering_tool.get_device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

bool Editor_renderer::Cylinder::init(Rendering_tool& rendering_tool)
{
	const uint32_t detail = 16;

	const uint32_t num_vertices = (detail + 1) * 2;
	Vertex_position3x32_normal3x32 vertices[num_vertices];

	vertices[0].position= float3(0.f,  0.5f, 0.f);
	vertices[0].normal = float3(0.f, 1.f, 0.f);

	vertices[detail + 1].position = float3(0.f, -0.5f, 0.f);
	vertices[detail + 1].normal = float3(0.f, -1.f, 0.f);

	for (uint32_t i = 1; i <= detail; ++i)
	{
		vertices[i].position = normalize(float3(cos(math::pi_mul_2 * float(i) / detail), 0.f, sin(math::pi_mul_2 * float(i) / detail)));
		vertices[i].normal = vertices[i].position;
		vertices[i].position.y = 0.5f;

		vertices[i + detail + 1] = vertices[i];
		vertices[i + detail + 1].position.y = -0.5f;
	}

	vertex_buffer_ = rendering_tool.get_device().create_vertex_buffer(sizeof(Vertex_position3x32_normal3x32) * num_vertices, vertices);
	if (!vertex_buffer_)
	{
			return false;
	}

	num_indices_ = detail * 2 * 3 * 2;
	unsigned short indices[detail * 2 * 3 * 2];

	const unsigned short num_indices = static_cast<unsigned short>(num_indices_);
	unsigned short v = 1;
	for (unsigned short i = 0; i < num_indices / 4; i += 3, ++v)
	{
		indices[i]     = v;
		indices[i + 1] = 0;
		indices[i + 2] = v == detail ? 1 : v + 1;

		indices[i     + detail * 3] = detail + 1;
		indices[i + 1 + detail * 3] = v + detail + 1;
		indices[i + 2 + detail * 3] = v == detail ? detail + 2 : v + detail + 2;
	}

	v = 1;
	for (unsigned short i = num_indices / 2; i < num_indices; i +=6, ++v)
	{
		indices[i]     = v + detail + 1;
		indices[i + 1] = v;
		indices[i + 2] = v == detail ? 1 : v + 1;

		indices[i + 3] = v + detail + 1;
		indices[i + 4] = v == detail ? 1 : v + 1;
		indices[i + 5] = v == detail ? detail + 2 : v + detail + 2;

	}

	index_buffer_ = rendering_tool.get_device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

bool Editor_renderer::Sphere::init(Rendering_tool& rendering_tool)
{
	const size_t num_vertices = 56;
	Vertex_position3x32_normal3x32 vertices[num_vertices];

	vertices[0].position = float3(-0.57735f, 0.57735f, -0.57735f);
	vertices[1].position = float3(-0.694945f, 0.184667f, -0.694945f);
	vertices[2].position = float3(-0.935295f, 0.250223f, -0.250223f);
	vertices[3].position = float3(-0.694945f, 0.694945f, -0.184667f);
	vertices[4].position = float3(-0.694945f, -0.184667f, -0.694945f);
	vertices[5].position = float3(-0.935295f, -0.250223f, -0.250223f);
	vertices[6].position = float3(-0.57735f, -0.57735f, -0.57735f);
	vertices[7].position = float3(-0.694945f, -0.694945f, -0.184667f);
	vertices[8].position = float3(-0.935295f, 0.250223f, 0.250223f);
	vertices[9].position = float3(-0.694945f, 0.694945f, 0.184667f);
	vertices[10].position = float3(-0.935295f, -0.250223f, 0.250223f);
	vertices[11].position = float3(-0.694945f, -0.694945f, 0.184667f);
	vertices[12].position = float3(-0.694945f, 0.184667f, 0.694945f);
	vertices[13].position = float3(-0.57735f, 0.57735f, 0.57735f);
	vertices[14].position = float3(-0.694945f, -0.184667f, 0.694945f);
	vertices[15].position = float3(-0.57735f, -0.57735f, 0.57735f);
	vertices[16].position = float3(-0.184667f, -0.694945f, -0.694945f);
	vertices[17].position = float3(-0.250223f, -0.935295f, -0.250223f);
	vertices[18].position = float3(0.184667f, -0.694945f, -0.694945f);
	vertices[19].position = float3(0.250223f, -0.935295f, -0.250223f);
	vertices[20].position = float3(0.57735f, -0.57735f, -0.57735f);
	vertices[21].position = float3(0.694945f, -0.694945f, -0.184667f);
	vertices[22].position = float3(-0.250223f, -0.935295f, 0.250223f);
	vertices[23].position = float3(0.250223f, -0.935295f, 0.250223f);
	vertices[24].position = float3(0.694945f, -0.694945f, 0.184667f);
	vertices[25].position = float3(-0.184667f, -0.694945f, 0.694945f);
	vertices[26].position = float3(0.184667f, -0.694945f, 0.694945f);
	vertices[27].position = float3(0.57735f, -0.57735f, 0.57735f);
	vertices[28].position = float3(-0.184667f, 0.694945f, -0.694945f);
	vertices[29].position = float3(-0.250223f, 0.250223f, -0.935295f);
	vertices[30].position = float3(0.184667f, 0.694945f, -0.694945f);
	vertices[31].position = float3(0.250223f, 0.250223f, -0.935295f);
	vertices[32].position = float3(0.57735f, 0.57735f, -0.57735f);
	vertices[33].position = float3(0.694945f, 0.184667f, -0.694945f);
	vertices[34].position = float3(-0.250223f, -0.250223f, -0.935295f);
	vertices[35].position = float3(0.250223f, -0.250223f, -0.935295f);
	vertices[36].position = float3(0.694945f, -0.184667f, -0.694945f);
	vertices[37].position = float3(0.694945f, 0.694945f, -0.184667f);
	vertices[38].position = float3(0.935295f, 0.250223f, -0.250223f);
	vertices[39].position = float3(0.694945f, 0.694945f, 0.184667f);
	vertices[40].position = float3(0.935295f, 0.250223f, 0.250223f);
	vertices[41].position = float3(0.57735f, 0.57735f, 0.57735f);
	vertices[42].position = float3(0.694945f, 0.184667f, 0.694945f);
	vertices[43].position = float3(0.935295f, -0.250223f, -0.250223f);
	vertices[44].position = float3(0.935295f, -0.250223f, 0.250223f);
	vertices[45].position = float3(0.694945f, -0.184667f, 0.694945f);
	vertices[46].position = float3(-0.250223f, 0.935295f, -0.250223f);
	vertices[47].position = float3(-0.250223f, 0.935295f, 0.250223f);
	vertices[48].position = float3(-0.184667f, 0.694945f, 0.694945f);
	vertices[49].position = float3(0.250223f, 0.935295f, -0.250223f);
	vertices[50].position = float3(0.250223f, 0.935295f, 0.250223f);
	vertices[51].position = float3(0.184667f, 0.694945f, 0.694945f);
	vertices[52].position = float3(-0.250223f, 0.250223f, 0.935295f);
	vertices[53].position = float3(-0.250223f, -0.250223f, 0.935295f);
	vertices[54].position = float3(0.250223f, 0.250223f, 0.935295f);
	vertices[55].position = float3(0.250223f, -0.250223f, 0.935295f);

	for (size_t i = 0; i < num_vertices; ++i)
	{
		vertices[i].normal = normalize(vertices[i].position);
	}

	vertex_buffer_ = rendering_tool.get_device().create_vertex_buffer(sizeof(Vertex_position3x32_normal3x32) * num_vertices, vertices);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = 324;
	unsigned short indices[324];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;
	indices[6] = 1;
	indices[7] = 4;
	indices[8] = 5;
	indices[9] = 5;
	indices[10] = 2;
	indices[11] = 1;
	indices[12] = 4;
	indices[13] = 6;
	indices[14] = 7;
	indices[15] = 7;
	indices[16] = 5;
	indices[17] = 4;
	indices[18] = 3;
	indices[19] = 2;
	indices[20] = 8;
	indices[21] = 8;
	indices[22] = 9;
	indices[23] = 3;
	indices[24] = 2;
	indices[25] = 5;
	indices[26] = 10;
	indices[27] = 10;
	indices[28] = 8;
	indices[29] = 2;
	indices[30] = 5;
	indices[31] = 7;
	indices[32] = 11;
	indices[33] = 11;
	indices[34] = 10;
	indices[35] = 5;
	indices[36] = 9;
	indices[37] = 8;
	indices[38] = 12;
	indices[39] = 12;
	indices[40] = 13;
	indices[41] = 9;
	indices[42] = 8;
	indices[43] = 10;
	indices[44] = 14;
	indices[45] = 14;
	indices[46] = 12;
	indices[47] = 8;
	indices[48] = 10;
	indices[49] = 11;
	indices[50] = 15;
	indices[51] = 15;
	indices[52] = 14;
	indices[53] = 10;
	indices[54] = 6;
	indices[55] = 16;
	indices[56] = 17;
	indices[57] = 17;
	indices[58] = 7;
	indices[59] = 6;
	indices[60] = 16;
	indices[61] = 18;
	indices[62] = 19;
	indices[63] = 19;
	indices[64] = 17;
	indices[65] = 16;
	indices[66] = 18;
	indices[67] = 20;
	indices[68] = 21;
	indices[69] = 21;
	indices[70] = 19;
	indices[71] = 18;
	indices[72] = 7;
	indices[73] = 17;
	indices[74] = 22;
	indices[75] = 22;
	indices[76] = 11;
	indices[77] = 7;
	indices[78] = 17;
	indices[79] = 19;
	indices[80] = 23;
	indices[81] = 23;
	indices[82] = 22;
	indices[83] = 17;
	indices[84] = 19;
	indices[85] = 21;
	indices[86] = 24;
	indices[87] = 24;
	indices[88] = 23;
	indices[89] = 19;
	indices[90] = 11;
	indices[91] = 22;
	indices[92] = 25;
	indices[93] = 25;
	indices[94] = 15;
	indices[95] = 11;
	indices[96] = 22;
	indices[97] = 23;
	indices[98] = 26;
	indices[99] = 26;
	indices[100] = 25;
	indices[101] = 22;
	indices[102] = 23;
	indices[103] = 24;
	indices[104] = 27;
	indices[105] = 27;
	indices[106] = 26;
	indices[107] = 23;
	indices[108] = 0;
	indices[109] = 28;
	indices[110] = 29;
	indices[111] = 29;
	indices[112] = 1;
	indices[113] = 0;
	indices[114] = 28;
	indices[115] = 30;
	indices[116] = 31;
	indices[117] = 31;
	indices[118] = 29;
	indices[119] = 28;
	indices[120] = 30;
	indices[121] = 32;
	indices[122] = 33;
	indices[123] = 33;
	indices[124] = 31;
	indices[125] = 30;
	indices[126] = 1;
	indices[127] = 29;
	indices[128] = 34;
	indices[129] = 34;
	indices[130] = 4;
	indices[131] = 1;
	indices[132] = 29;
	indices[133] = 31;
	indices[134] = 35;
	indices[135] = 35;
	indices[136] = 34;
	indices[137] = 29;
	indices[138] = 31;
	indices[139] = 33;
	indices[140] = 36;
	indices[141] = 36;
	indices[142] = 35;
	indices[143] = 31;
	indices[144] = 4;
	indices[145] = 34;
	indices[146] = 16;
	indices[147] = 16;
	indices[148] = 6;
	indices[149] = 4;
	indices[150] = 34;
	indices[151] = 35;
	indices[152] = 18;
	indices[153] = 18;
	indices[154] = 16;
	indices[155] = 34;
	indices[156] = 35;
	indices[157] = 36;
	indices[158] = 20;
	indices[159] = 20;
	indices[160] = 18;
	indices[161] = 35;
	indices[162] = 32;
	indices[163] = 37;
	indices[164] = 38;
	indices[165] = 38;
	indices[166] = 33;
	indices[167] = 32;
	indices[168] = 37;
	indices[169] = 39;
	indices[170] = 40;
	indices[171] = 40;
	indices[172] = 38;
	indices[173] = 37;
	indices[174] = 39;
	indices[175] = 41;
	indices[176] = 42;
	indices[177] = 42;
	indices[178] = 40;
	indices[179] = 39;
	indices[180] = 33;
	indices[181] = 38;
	indices[182] = 43;
	indices[183] = 43;
	indices[184] = 36;
	indices[185] = 33;
	indices[186] = 38;
	indices[187] = 40;
	indices[188] = 44;
	indices[189] = 44;
	indices[190] = 43;
	indices[191] = 38;
	indices[192] = 40;
	indices[193] = 42;
	indices[194] = 45;
	indices[195] = 45;
	indices[196] = 44;
	indices[197] = 40;
	indices[198] = 36;
	indices[199] = 43;
	indices[200] = 21;
	indices[201] = 21;
	indices[202] = 20;
	indices[203] = 36;
	indices[204] = 43;
	indices[205] = 44;
	indices[206] = 24;
	indices[207] = 24;
	indices[208] = 21;
	indices[209] = 43;
	indices[210] = 44;
	indices[211] = 45;
	indices[212] = 27;
	indices[213] = 27;
	indices[214] = 24;
	indices[215] = 44;
	indices[216] = 0;
	indices[217] = 3;
	indices[218] = 46;
	indices[219] = 46;
	indices[220] = 28;
	indices[221] = 0;
	indices[222] = 3;
	indices[223] = 9;
	indices[224] = 47;
	indices[225] = 47;
	indices[226] = 46;
	indices[227] = 3;
	indices[228] = 9;
	indices[229] = 13;
	indices[230] = 48;
	indices[231] = 48;
	indices[232] = 47;
	indices[233] = 9;
	indices[234] = 28;
	indices[235] = 46;
	indices[236] = 49;
	indices[237] = 49;
	indices[238] = 30;
	indices[239] = 28;
	indices[240] = 46;
	indices[241] = 47;
	indices[242] = 50;
	indices[243] = 50;
	indices[244] = 49;
	indices[245] = 46;
	indices[246] = 47;
	indices[247] = 48;
	indices[248] = 51;
	indices[249] = 51;
	indices[250] = 50;
	indices[251] = 47;
	indices[252] = 30;
	indices[253] = 49;
	indices[254] = 37;
	indices[255] = 37;
	indices[256] = 32;
	indices[257] = 30;
	indices[258] = 49;
	indices[259] = 50;
	indices[260] = 39;
	indices[261] = 39;
	indices[262] = 37;
	indices[263] = 49;
	indices[264] = 50;
	indices[265] = 51;
	indices[266] = 41;
	indices[267] = 41;
	indices[268] = 39;
	indices[269] = 50;
	indices[270] = 13;
	indices[271] = 12;
	indices[272] = 52;
	indices[273] = 52;
	indices[274] = 48;
	indices[275] = 13;
	indices[276] = 12;
	indices[277] = 14;
	indices[278] = 53;
	indices[279] = 53;
	indices[280] = 52;
	indices[281] = 12;
	indices[282] = 14;
	indices[283] = 15;
	indices[284] = 25;
	indices[285] = 25;
	indices[286] = 53;
	indices[287] = 14;
	indices[288] = 48;
	indices[289] = 52;
	indices[290] = 54;
	indices[291] = 54;
	indices[292] = 51;
	indices[293] = 48;
	indices[294] = 52;
	indices[295] = 53;
	indices[296] = 55;
	indices[297] = 55;
	indices[298] = 54;
	indices[299] = 52;
	indices[300] = 53;
	indices[301] = 25;
	indices[302] = 26;
	indices[303] = 26;
	indices[304] = 55;
	indices[305] = 53;
	indices[306] = 51;
	indices[307] = 54;
	indices[308] = 42;
	indices[309] = 42;
	indices[310] = 41;
	indices[311] = 51;
	indices[312] = 54;
	indices[313] = 55;
	indices[314] = 45;
	indices[315] = 45;
	indices[316] = 42;
	indices[317] = 54;
	indices[318] = 55;
	indices[319] = 26;
	indices[320] = 27;
	indices[321] = 27;
	indices[322] = 45;
	indices[323] = 55;

	index_buffer_ = rendering_tool.get_device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

bool Editor_renderer::Line::init(Rendering_tool& rendering_tool)
{
	Vertex_position3x32_normal3x32 vertices[2];

	vertices[0].position  = float3(0.f,  100.f, 0.f);
	vertices[1].position  = float3(0.f, -100.f, 0.f);

	vertex_buffer_ = rendering_tool.get_device().create_vertex_buffer(sizeof(Vertex_position3x32_normal3x32) * 2, vertices);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = 2;
	unsigned short indices[2] = { 0, 1 };

	index_buffer_ = rendering_tool.get_device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

}
