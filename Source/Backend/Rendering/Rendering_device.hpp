#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Data_format.hpp"
#include "Rendering/Primitive_topology.hpp"
#include "Rendering/Color.hpp"
#include "Data_buffer.hpp"
#include "Sampler_state.hpp"
#include "Shader.hpp"
#include "Shader_program.hpp"
#include "Render_state.hpp"
#include <string>

namespace platform
{

class Client;

}

namespace rendering
{

struct Viewport;
class Input_layout;
class Framebuffer;
class Texture;
class Texture_transfer;
struct Texture_description;
class Shader_resource_view;
class Render_tarview;
class Render_target_shader_resource_view;
class Cube_render_target_shader_resource_view;
class Depth_stencil_view;
class Depth_stencil_shader_resource_view;
class Vertex_layout_description;
class Shader_program;
class Texture_data_adapter;

class Rendering_device
{

public:

	Rendering_device();

	bool init(const platform::Client& client);

	Handle<Vertex_buffer> create_vertex_buffer(uint32_t num_bytes, void* data = nullptr) const;

	Handle<Index_buffer> create_index_buffer(uint32_t num_bytes, void* data, rendering::Data_format::Value format) const;

	Handle<Constant_buffer> create_constant_buffer(uint32_t num_bytes) const;

	Handle<Input_layout> create_input_layout(const Vertex_layout_description& vertex_description, const Shader_program::Signature& signature) const;

	Handle<Framebuffer> create_framebuffer() const;

	Handle<Texture> create_texture_2D(const Texture_description& description) const;
	Handle<Texture> create_texture_2D(const Texture_data_adapter& texture_data) const;

	Handle<Texture> create_texture_3D(const Texture_data_adapter& texture_data) const;

	Handle<Texture> create_texture_cube(const Texture_description& description) const;
	Handle<Texture> create_texture_cube(const Texture_data_adapter& texture_data) const;

	Handle<Texture_transfer> create_texture_transfer(const Texture_description& description) const;

	Handle<Shader_resource_view> create_shader_resource_view(const Handle<Texture>& texture, const std::string& name) const;
	Handle<Render_tarview> create_render_tarview(const Handle<Texture>& texture, uint32_t min_layer = 0, uint32_t num_layers = 1) const;
	Handle<Render_target_shader_resource_view> create_render_target_shader_resource_view(const Texture_description& description) const;
	Handle<Cube_render_target_shader_resource_view> create_cube_render_target_shader_resource_view(const Texture_description& description) const;
	Handle<Depth_stencil_view> create_depth_stencil_view(const Texture_description& description) const;
	Handle<Depth_stencil_shader_resource_view> create_depth_stencil_shader_resource_view(const Texture_description& description) const;

	Handle<Sampler_state> create_sampler_state(const Sampler_state::Description& description) const;

	Handle<Shader> create_shader(Shader::Type type) const;

	Handle<Shader_program> create_shader_program() const;

	Handle<Rasterizer_state> create_rasterizer_state(const Rasterizer_state::Description& description) const;
	Handle<Depth_stencil_state> create_depth_stencil_state(const Depth_stencil_state::Description& description) const;
	Handle<Blend_state> create_blend_state(const Blend_state::Description& description) const;

	void update_buffer(const Data_buffer& buffer, uint32_t offset, uint32_t num_bytes, const void* data, bool discard = false) const;

	void generate_mip_maps(const Handle<Texture>& texture) const;

	void copy(const Handle<Texture_transfer>& destination, const Handle<Render_tarview>& source) const;
	void map(unsigned char** buffer, const Handle<Texture_transfer>& texture) const;
	void unmap(const Handle<Texture_transfer>& texture) const;

	void set_viewports(uint32_t count, const Viewport* viewports);

	void set_vertex_buffer(const Handle<Vertex_buffer>& vb, uint32_t stride, uint32_t offset = 0, uint32_t slot = 0) const;
	void set_vertex_buffers(uint32_t num_buffers, const Handle<Vertex_buffer> vertex_buffers[], const uint32_t strides[], const uint32_t offsets[] = nullptr, uint32_t start_slot = 0) const;
	void set_index_buffer(const Index_buffer* index_buffer);
	void set_constant_buffer(const Handle<Constant_buffer>& constant_buffer, uint32_t slot) const;
	void set_input_layout(const Input_layout* input_layout);

	void set_framebuffer(const Handle<Framebuffer>& framebuffer);
	void clear_color(const Handle<Framebuffer>& framebuffer, uint32_t num_colors, Color4* colors);
	void clear_depth_stencil(const Handle<Framebuffer>& framebuffer, float depth, int stencil);

	void set_shader_resources(uint32_t num_resources, const Handle<Shader_resource_view> resources[], uint32_t start_slot = 0);
	void set_sampler_states(uint32_t num_states, const Handle<Sampler_state> state[], uint32_t start_slot = 0);

	void unbind_textures();

	void set_rasterizer_state(const Rasterizer_state* state);
	void set_depth_stencil_state(const Handle<Depth_stencil_state>& state, uint32_t stencil_ref = 0);
	void set_blend_state(const Handle<Blend_state>& state);

	Primitive_topology get_primitive_topology() const;
	void set_primitive_topology(Primitive_topology topology);

	void draw(uint32_t num_vertices, uint32_t start_vertex = 0) const;
	void draw_indexed(uint32_t num_indices, uint32_t start_index = 0) const;

private:

	void restore_current_input_layout() const;

	const platform::Client* client_;

	Primitive_topology primitive_topology_;
	uint32_t ogl_primitive_topology_;

	Handle<Framebuffer> current_framebuffer_;
	const Index_buffer* current_index_buffer_;
	const Input_layout* current_input_layout_;

	Handle<Depth_stencil_state> current_depth_stencil_state_;
	uint32_t current_stencil_ref_;

	Handle<Blend_state> current_blend_state_;

public:

	std::string get_api_info() const;

	uint32_t get_constant_buffer_offset_alignment() const;

private:

	static uint32_t s_ogl_primitive_topology[14];
};

}
