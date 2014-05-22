#include "Rendering_device.hpp"
#include "Viewport.hpp"
#include "Data_buffer.hpp"
#include "Input_layout.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Texture_data_adapter.hpp"
#include "Resource_view.hpp"
#include "Texture_transfer.hpp"
#include "Shader_program.hpp"
#include "Data_format_mapping.hpp"
#include "String/String.hpp"
#include "Rendering/Vertex_layout_description.hpp"
#include "Platform/Client.hpp"
#include <GL/glew.h>
#include "Platform/Platform.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace rendering
{

#ifdef _DEBUG
void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, void* user_param);
#endif

Rendering_device::Rendering_device() :
	client_(nullptr), ogl_primitive_topology_(0), current_framebuffer_(nullptr), current_index_buffer_(nullptr),
	current_input_layout_(nullptr), current_depth_stencil_state_(nullptr), current_stencil_ref_(0)
{}

bool Rendering_device::init(const platform::Client& client)
{
	client_ = &client;

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		return false;
	}

	// This might give an error because of glewExperimental, but we can ignore it
    /*GLenum error =*/ glGetError();

#ifdef _DEBUG
	glDebugMessageCallbackARB(error_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
#endif

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return true;
}

Handle<Vertex_buffer> Rendering_device::create_vertex_buffer(uint32_t num_bytes, void* data) const
{
	uint32_t id;
	glGenBuffers(1, &id);

	if (!id)
	{
		return nullptr;
	}

	glNamedBufferDataEXT(id, num_bytes, data, data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

	return Handle<Vertex_buffer>(new Vertex_buffer(id, data ? Data_buffer::Usage::Static : Data_buffer::Usage::Dynamic, num_bytes));
}

Handle<Index_buffer> Rendering_device::create_index_buffer(uint32_t num_bytes, void* data, rendering::Data_format::Value format) const
{
	uint32_t id;
	glGenBuffers(1, &id);

	if (!id)
	{
		return nullptr;
	}

	glNamedBufferDataEXT(id, num_bytes, data, GL_STATIC_DRAW);

	return Handle<Index_buffer>(new Index_buffer(id, Data_buffer::Usage::Static, num_bytes, format));
}

Handle<Constant_buffer> Rendering_device::create_constant_buffer(uint32_t num_bytes) const
{
	uint32_t id;
	glGenBuffers(1, &id);

	if (!id)
	{
		return nullptr;
	}

	glNamedBufferDataEXT(id, num_bytes, nullptr, GL_DYNAMIC_DRAW);

	return Handle<Constant_buffer>(new Constant_buffer(id, Data_buffer::Usage::Dynamic, num_bytes));
}

Handle<Input_layout> Rendering_device::create_input_layout(const Vertex_layout_description& vertex_description, const Shader_program::Signature& signature) const
{
	if (!signature.get_num_elements_())
	{
		return nullptr;
	}

	uint32_t id;
	glGenVertexArrays(1, &id);

	if (!id)
	{
		return nullptr;
	}

	glBindVertexArray(id);

	Handle<Input_layout> input_layout = Handle<Input_layout>(new Input_layout(id));

	for (uint32_t i = 0; i < signature.get_num_elements_(); ++i)
	{
		auto& se = signature[i];

		if (se.location < 0)
		{
			continue;
		}

		for (uint32_t j = 0; j < vertex_description.get_num_elements_(); ++j)
		{
			auto& ve = vertex_description[j];

			if (se.semantic == ve.semantic_name)
			{
				if (ve.slot >= vertex_description.get_num_streams())
				{
					return nullptr;
				}

				uint32_t location = uint32_t(se.location);

				glEnableVertexAttribArray(location);

				Data_format_mapping dfm = Data_format_mapping::map(ve.format);

				glVertexAttribFormat(location, dfm.num_elements_, dfm.type, dfm.normalized, ve.byte_offset);
				glVertexAttribBinding(location, ve.slot);

				input_layout->m_attribute_states[location] = true;

				break;
			}
		}
	}

	restore_current_input_layout();

	return input_layout;
}

Handle<Framebuffer> Rendering_device::create_framebuffer() const
{
	uint32_t id;
	glGenFramebuffers(1, &id);

	if (!id)
	{
		return nullptr;
	}

	return Handle<Framebuffer>(new Framebuffer(id));
}

Handle<Texture> Rendering_device::create_texture_2D(const Texture_description& description) const
{
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	bool multisample = description.num_samples > 1;

	if (multisample && Texture_description::Type::Texture_2D_multisample != description.type)
	{
#ifdef _DEBUG
		std::cout << "Rendering_device::create_texture_2D: num_samples and texture type don't match" << std::endl;
#endif
		return nullptr;
	}

	uint32_t id;

	if (description.shader_resource)
	{
		glGenTextures(1, &id);

		if (!id)
		{
			return nullptr;
		}

		if (multisample)
		{
			glTextureStorage2DMultisampleEXT(id, GL_TEXTURE_2D_MULTISAMPLE, description.num_samples, mapping.internal_format, description.dimensions.x, description.dimensions.y, true);
		}
		else
		{
			glTextureStorage2DEXT(id, GL_TEXTURE_2D, description.num_mip_levels, mapping.internal_format, description.dimensions.x, description.dimensions.y);
		}
	}
	else
	{
		glGenRenderbuffers(1, &id);

		if (!id)
		{
			return nullptr;
		}

		if (multisample)
		{
			glNamedRenderbufferStorageMultisampleEXT(id, description.num_samples, mapping.internal_format, description.dimensions.x, description.dimensions.y);
		}
		else
		{
			glNamedRenderbufferStorageEXT(id, mapping.internal_format, description.dimensions.x, description.dimensions.y);
		}
	}

	if (multisample)
	{
		return Handle<Texture>(new Texture_2D_multisample(id, description));
	}
	else
	{
		return Handle<Texture>(new Texture_2D(id, description));
	}
}

Handle<Texture> Rendering_device::create_texture_2D(const Texture_data_adapter& texture_data) const
{
	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	const Texture_description& description = texture_data.get_description();
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glTextureStorage2DEXT(id, GL_TEXTURE_2D, description.num_mip_levels, mapping.internal_format, description.dimensions.x, description.dimensions.y);

	Texture_description::Data data;

	if (Data_format::is_compressed(description.format))
	{
		for (uint32_t i = 0; i < description.num_mip_levels; ++i)
		{
			texture_data.get_level(data, i);

			glCompressedTextureSubImage2DEXT(id, GL_TEXTURE_2D, i, 0, 0, data.dimensions.x, data.dimensions.y, mapping.internal_format, data.num_bytes, static_cast<void*>(data.buffer));
		}
	}
	else
	{
		uint32_t bytes_per_pixel = uint32_t(Data_format::size_of(description.format));

		for (uint32_t i = 0; i < description.num_mip_levels; ++i)
		{
			texture_data.get_level(data, i);

			bool changed_pixel_store = false;

			if (bytes_per_pixel < 4 && data.dimensions.x % 4 != 0)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, bytes_per_pixel);

				changed_pixel_store = true;
			}

			glTextureSubImage2DEXT(id, GL_TEXTURE_2D, i, 0, 0, data.dimensions.x, data.dimensions.y, mapping.format, mapping.type, static_cast<void*>(data.buffer));

			if (changed_pixel_store)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			}
		}
	}

	return Handle<Texture>(new Texture_2D(id, description));
}

Handle<Texture> Rendering_device::create_texture_3D(const Texture_data_adapter& texture_data) const
{
	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	const Texture_description& description = texture_data.get_description();
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glTextureStorage3DEXT(id, GL_TEXTURE_3D, description.num_mip_levels, mapping.internal_format, description.dimensions.x, description.dimensions.y, description.dimensions.z);

	Texture_description::Data data;

	if (Data_format::is_compressed(description.format))
	{
		for (uint32_t i = 0; i < description.num_mip_levels; ++i)
		{
			texture_data.get_level(data, i);

			glCompressedTextureSubImage3DEXT(id, GL_TEXTURE_3D, i, 0, 0, 0, data.dimensions.x, data.dimensions.y, data.dimensions.z, mapping.internal_format, data.num_bytes, static_cast<void*>(data.buffer));
		}
	}
	else
	{
		uint32_t bytes_per_pixel = uint32_t(Data_format::size_of(description.format));

		for (uint32_t i = 0; i < description.num_mip_levels; ++i)
		{
			texture_data.get_level(data, i);

			bool changed_pixel_store = false;

			if (bytes_per_pixel < 4 && data.dimensions.x % 4 != 0)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, bytes_per_pixel);

				changed_pixel_store = true;
			}

			glTextureSubImage3DEXT(id, GL_TEXTURE_3D, i, 0, 0, 0, data.dimensions.x, data.dimensions.y, data.dimensions.z, mapping.format, mapping.type, static_cast<void*>(data.buffer));

			if (changed_pixel_store)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			}
		}
	}

	return Handle<Texture>(new Texture_3D(id, description));
}

Handle<Texture> Rendering_device::create_texture_cube(const Texture_description& description) const
{
	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glTextureStorage2DEXT(id, GL_TEXTURE_CUBE_MAP, description.num_mip_levels, mapping.internal_format, description.dimensions.x, description.dimensions.y);

	return Handle<Texture>(new Texture_cube(id, description));
}

Handle<Texture> Rendering_device::create_texture_cube(const Texture_data_adapter& texture_data) const
{
	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	const Texture_description& description = texture_data.get_description();
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glTextureStorage2DEXT(id, GL_TEXTURE_CUBE_MAP, description.num_mip_levels, mapping.internal_format, description.dimensions.x, description.dimensions.y);

	Texture_description::Data data;

	if (Data_format::is_compressed(description.format))
	{
		for (uint32_t f = 0; f < description.num_layers; ++f)
		{
			for (uint32_t i = 0; i < description.num_mip_levels; ++i)
			{
				texture_data.get_level(data, f, i);

				glCompressedTextureSubImage2DEXT(id, GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(f), i, 0, 0, data.dimensions.x, data.dimensions.y, mapping.internal_format, data.num_bytes, static_cast<void*>(data.buffer));
			}
		}
	}
	else
	{
		uint32_t bytes_per_pixel = uint32_t(Data_format::size_of(description.format));

		for (uint32_t f = 0; f < description.num_layers; ++f)
		{
			for (uint32_t i = 0; i < description.num_mip_levels; ++i)
			{
				texture_data.get_level(data, f, i);

				bool changed_pixel_store = false;

				if (bytes_per_pixel < 4 && data.dimensions.x % 4 != 0)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, bytes_per_pixel);

					changed_pixel_store = true;
				}

				glTextureSubImage2DEXT(id, GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(f), i, 0, 0, data.dimensions.x, data.dimensions.y, mapping.format, mapping.type, static_cast<void*>(data.buffer));

				if (changed_pixel_store)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				}
			}
		}
	}

	return Handle<Texture>(new Texture_cube(id, description));
}

Handle<Texture_transfer> Rendering_device::create_texture_transfer(const Texture_description& description) const
{
	return Handle<Texture_transfer>(new Texture_transfer(0, description));
}

Handle<Shader_resource_view> Rendering_device::create_shader_resource_view(const Handle<Texture>& texture, const std::string& name) const
{
	if (!texture)
	{
		return nullptr;
	}

	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	const Texture_description& description = texture->description_;
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glTextureView(id, texture->internal_type_, texture->id_, mapping.internal_format, 0, description.num_mip_levels, 0, description.num_layers);

	return Handle<Shader_resource_view>(new Shader_resource_view(id, description, texture, name));
}

Handle<Render_target_view> Rendering_device::create_render_target_view(const Handle<Texture>& texture, uint32_t min_layer, uint32_t num_layers) const
{
	if (!texture)
	{
		return nullptr;
	}

	uint32_t id;
	glGenTextures(1, &id);

	if (!id)
	{
		return nullptr;
	}

	const Texture_description& description = texture->description_;
	Data_format_mapping mapping = Data_format_mapping::map(description.format);
	bool multisample = description.num_samples > 1;

	GLenum type = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glTextureView(id, type, texture->id_, mapping.internal_format, 0, 1, min_layer, num_layers);

	Texture_description new_description = description;
	new_description.type = multisample ? Texture_description::Type::Texture_2D_multisample : Texture_description::Type::Texture_2D;
	new_description.num_layers = num_layers;
	new_description.num_mip_levels = 1;
	return Handle<Render_target_view>(new Render_target_view(id, new_description, texture));
}

Handle<Render_target_shader_resource_view> Rendering_device::create_render_target_shader_resource_view(const Texture_description& description) const
{
	Handle<Texture> texture = create_texture_2D(description);

	if (!texture)
	{
		return nullptr;
	}

	Handle<Render_target_view> render_target_view = create_render_target_view(texture);

	if (!render_target_view)
	{
		return nullptr;
	}

	Handle<Shader_resource_view> shader_resource_view = create_shader_resource_view(texture, "");

	if (!shader_resource_view)
	{
		return nullptr;
	}

	return Handle<Render_target_shader_resource_view>(new Render_target_shader_resource_view(render_target_view, shader_resource_view));
}

Handle<Cube_render_target_shader_resource_view> Rendering_device::create_cube_render_target_shader_resource_view(const Texture_description& description) const
{
	Handle<Texture> texture = create_texture_cube(description);

	if (!texture)
	{
		return nullptr;
	}

	Handle<Render_target_view> render_target_views[6];

	for (uint32_t i = 0; i < 6; ++i)
	{
		render_target_views[i] = create_render_target_view(texture, i);

		if (!render_target_views[i])
		{
			return nullptr;
		}
	}

	Handle<Shader_resource_view> shader_resource_view = create_shader_resource_view(texture, "");

	if (!shader_resource_view)
	{
		return nullptr;
	}

	return Handle<Cube_render_target_shader_resource_view>(new Cube_render_target_shader_resource_view(render_target_views, shader_resource_view));
}

Handle<Depth_stencil_view> Rendering_device::create_depth_stencil_view(const Texture_description& description) const
{
	Handle<Texture> depth_stencil = create_texture_2D(description);

	if (!depth_stencil)
	{
		return nullptr;
	}

	return Handle<Depth_stencil_view>(new Depth_stencil_view(depth_stencil->id_, description, depth_stencil));
}

Handle<Depth_stencil_shader_resource_view> Rendering_device::create_depth_stencil_shader_resource_view(const Texture_description& description) const
{
	Texture_description new_description = description;

	new_description.shader_resource = true;

	Handle<Depth_stencil_view> depth_stencil_view = create_depth_stencil_view(new_description);

	if (!depth_stencil_view)
	{
		return nullptr;
	}

	Handle<Shader_resource_view> shader_resource_view = create_shader_resource_view(depth_stencil_view->texture_, "");

	if (!shader_resource_view)
	{
		return nullptr;
	}

	return Handle<Depth_stencil_shader_resource_view>(new Depth_stencil_shader_resource_view(depth_stencil_view, shader_resource_view));
}

Handle<Sampler_state> Rendering_device::create_sampler_state(const Sampler_state::Description& description) const
{
	uint32_t id;
	glGenSamplers(1, &id);

	if (!id)
	{
		return nullptr;
	}

	uint32_t minification;
	uint32_t magnification;
	Sampler_state::Description::Filtering::map(minification, magnification, description.filtering);

	glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minification);
	glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magnification);

	glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, float(description.max_anisotropy));

	uint32_t address = Sampler_state::Description::Address::map(description.address);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_S, address);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_T, address);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_R, address);

	return Handle<Sampler_state>(new Sampler_state(id));
}

Handle<Shader> Rendering_device::create_shader(Shader::Type type) const
{
	GLenum gl_type;

	switch (type)
	{
	case Shader::Type::Vertex:
		gl_type = GL_VERTEX_SHADER;
		break;
	case Shader::Type::Geometry:
		gl_type = GL_GEOMETRY_SHADER;
		break;
	case Shader::Type::Pixel:
		gl_type = GL_FRAGMENT_SHADER;
		break;
	case Shader::Type::Unknown:
	default:
		return nullptr;
	}

	uint32_t shader = glCreateShader(gl_type);

	if (!shader)
	{
		return nullptr;
	}

	return Handle<Shader>(new Shader(shader));
}

Handle<Shader_program> Rendering_device::create_shader_program() const
{
	uint32_t program = glCreateProgram();

	if (!program)
	{
		return nullptr;
	}

	return Handle<Shader_program>(new Shader_program(program));
}

Handle<Rasterizer_state> Rendering_device::create_rasterizer_state(const Rasterizer_state::Description& description) const
{
	return Handle<Rasterizer_state>(new Rasterizer_state(description));
}

Handle<Depth_stencil_state> Rendering_device::create_depth_stencil_state(const Depth_stencil_state::Description& description) const
{
	return Handle<Depth_stencil_state>(new Depth_stencil_state(description));
}

Handle<Blend_state> Rendering_device::create_blend_state(const Blend_state::Description& description) const
{
	return Handle<Blend_state>(new Blend_state(description));
}

void Rendering_device::update_buffer(const Data_buffer& buffer, uint32_t offset, uint32_t num_bytes, const void* data, bool discard) const
{
	glNamedBufferSubDataEXT(buffer.id_, offset, num_bytes, data);

	if (discard)
	{
		glNamedBufferDataEXT(buffer.id_, buffer.size_, nullptr, buffer.usage_);
	}
}

void Rendering_device::generate_mip_maps(const Handle<Texture>& texture) const
{
	__glewGenerateTextureMipmapEXT(texture->id_, texture->internal_type_);
}

void Rendering_device::copy(const Handle<Texture_transfer>& destination, const Handle<Render_target_view>& source) const
{
	const Texture_description& description = source->description_;
	Data_format_mapping mapping = Data_format_mapping::map(description.format);

	glGetTextureImageEXT(source->id_, source->get_internal_type(), 0, mapping.format, mapping.type, destination->buffer_);
}

void Rendering_device::map(unsigned char** buffer, const Handle<Texture_transfer>& texture) const
{
	*buffer = texture->buffer_;
}

void Rendering_device::unmap(const Handle<Texture_transfer>& /*texture*/) const
{}

void Rendering_device::set_viewports(uint32_t count, const Viewport* viewports)
{
	static float  vp[6 * 4];
	static double dr[6 * 2];

	for (uint32_t i = 0; i < count; ++i)
	{
		vp[i * 4 + 0] = viewports[i].origin.x;
		vp[i * 4 + 1] = viewports[i].origin.y;
		vp[i * 4 + 2] = viewports[i].size.x;
		vp[i * 4 + 3] = viewports[i].size.y;

		dr[i * 2 + 0] = viewports[i].depth_range.x;
		dr[i * 2 + 1] = viewports[i].depth_range.y;
	}

	glViewportArrayv(0, count, vp);
	glDepthRangeArrayv(0, count, dr);
}

void Rendering_device::set_vertex_buffer(const Handle<Vertex_buffer>& vb, uint32_t stride, uint32_t offset, uint32_t slot) const
{
	set_vertex_buffers(1, &vb, &stride, &offset, slot);
}

void Rendering_device::set_vertex_buffers(uint32_t num_buffers, const Handle<Vertex_buffer> vertex_buffers[], const uint32_t strides[], const uint32_t offsets[], uint32_t start_slot) const
{
	static uint32_t temp_buffers[8];
	static GLintptr temp_offsets[8];

	for (uint32_t i = 0; i < num_buffers; ++i)
	{
		temp_buffers[i] = vertex_buffers[i]->id_;
		temp_offsets[i] = offsets ? offsets[i] : 0;
	}

	glBindVertexBuffers(start_slot, num_buffers, temp_buffers, temp_offsets, reinterpret_cast<const GLsizei*>(strides));
}

void Rendering_device::set_index_buffer(const Index_buffer* index_buffer)
{
	current_index_buffer_ = index_buffer;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id_);
}

void Rendering_device::set_constant_buffer(const Handle<Constant_buffer>& constant_buffer, uint32_t slot) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, constant_buffer->id_);
}

void Rendering_device::set_input_layout(const Input_layout* input_layout)
{
	current_input_layout_ = input_layout;

	restore_current_input_layout();
}

void Rendering_device::set_framebuffer(const Handle<Framebuffer>& framebuffer)
{
//	if (current_framebuffer_ != framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer ? framebuffer->id_ : 0);
		current_framebuffer_ = framebuffer;
	}
}

void Rendering_device::clear_color(const Handle<Framebuffer>& framebuffer, uint32_t num_colors, Color4* colors)
{
	Handle<Framebuffer> current_framebuffer = current_framebuffer_;

	set_framebuffer(framebuffer);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    for (uint32_t i = 0; i < num_colors; ++i)
	{
		glClearBufferfv(GL_COLOR, i, colors[i].v);
	}

	if (current_blend_state_)
	{
		if (current_blend_state_->independent_blend_enable_)
		{
			for (uint32_t i = 0; i < Blend_state::s_num_render_targets; ++i)
			{
				const auto& target = current_blend_state_->render_targets_[i];

				glColorMaski(i, target.enable_red, target.enable_green, target.enable_blue, target.enable_alpha);
			}
		}
		else
		{
			const auto& target = current_blend_state_->render_targets_[0];

			glColorMask(target.enable_red, target.enable_green, target.enable_blue, target.enable_alpha);
		}
	}

	set_framebuffer(current_framebuffer);
}

void Rendering_device::clear_depth_stencil(const Handle<Framebuffer>& framebuffer, float depth, int stencil)
{
	Handle<Framebuffer> current_framebuffer = current_framebuffer_;

	set_framebuffer(framebuffer);

	glDepthMask(GL_TRUE);
//	glEnable(GL_STENCIL_TEST);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);

	if (current_depth_stencil_state_ && !current_depth_stencil_state_->depth_write_mask_)
	{
		glDepthMask(current_depth_stencil_state_->depth_write_mask_);
	}

	set_framebuffer(current_framebuffer);
}

void Rendering_device::set_shader_resources(uint32_t num_resources, const Handle<Shader_resource_view> resources[], uint32_t start_slot)
{
	for (uint32_t i = 0; i < num_resources; ++i)
	{
		glBindMultiTextureEXT(GL_TEXTURE0 + start_slot + i, resources[i] ? resources[i]->get_internal_type() : GL_TEXTURE_2D, resources[i] ? resources[i]->id_ : 0);
	}
}

void Rendering_device::set_sampler_states(uint32_t num_states, const Handle<Sampler_state> state[], uint32_t start_slot)
{
	for (uint32_t i = 0; i < num_states; ++i)
	{
		glBindSampler(start_slot + i, state[i] ? state[i]->id_ : 0);
	}
}

void Rendering_device::unbind_textures()
{
//	static uint32_t null_textures[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



}

void Rendering_device::set_rasterizer_state(const Rasterizer_state* state)
{
	state->cull_ ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	glCullFace(state->cull__face);
	glFrontFace(state->front_face_);
	state->multisample_enable_ ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
}

void Rendering_device::set_depth_stencil_state(const Handle<Depth_stencil_state>& state, uint32_t stencil_ref)
{
	if (current_stencil_ref_ != stencil_ref || current_depth_stencil_state_ != state)
	{
		glStencilFuncSeparate(GL_FRONT, state->front_face_.comparison_func, static_cast<GLint>(stencil_ref), 0xFFFFFFFF);
		glStencilFuncSeparate(GL_BACK,  state->back_face_.comparison_func,  static_cast<GLint>(stencil_ref), 0xFFFFFFFF);

		current_stencil_ref_ = stencil_ref;
	}

	if (current_depth_stencil_state_ != state)
	{
		state->depth_enable_ ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		glDepthMask(state->depth_write_mask_);
		glDepthFunc(state->comparison_func_);

		state->stencil_enable_ ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);

		glStencilOpSeparate(GL_FRONT, state->front_face_.fail_op, state->front_face_.depth_fail_op, state->front_face_.pass_op);
		glStencilOpSeparate(GL_BACK,  state->back_face_.fail_op,  state->back_face_.depth_fail_op,  state->back_face_.pass_op);

		current_depth_stencil_state_ = state;
	}
}

void Rendering_device::set_blend_state(const Handle<Blend_state>& state)
{
	if (current_blend_state_ != state)
	{
		if (state->independent_blend_enable_)
		{
			for (uint32_t i = 0; i < Blend_state::s_num_render_targets; ++i)
			{
				const auto& target = state->render_targets_[i];

				target.blend_enable ? glEnablei(GL_BLEND, i) : glDisablei(GL_BLEND, i);

				glBlendFuncSeparatei(i, target.source_blend, target.destination_blend, target.source_blend_alpha, target.destination_blend_alpha);

				glBlendEquationSeparatei(i, target.blend_op, target.blend_op_alpha);

				glColorMaski(i, target.enable_red, target.enable_green, target.enable_blue, target.enable_alpha);
			}
		}
		else
		{
			const auto& target = state->render_targets_[0];

			target.blend_enable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

			glBlendFuncSeparate(target.source_blend, target.destination_blend, target.source_blend_alpha, target.destination_blend_alpha);

			glBlendEquationSeparate(target.blend_op, target.blend_op_alpha);

			glColorMask(target.enable_red, target.enable_green, target.enable_blue, target.enable_alpha);
		}

		current_blend_state_ = state;

	}
}

Primitive_topology Rendering_device::get_primitive_topology() const
{
	return primitive_topology_;
}

void Rendering_device::set_primitive_topology(Primitive_topology topology)
{
	primitive_topology_ = topology;
	ogl_primitive_topology_ = s_ogl_primitive_topology[static_cast<uint32_t>(topology)];
}

void Rendering_device::draw(uint32_t num_vertices, uint32_t start_vertex) const
{
	glDrawArrays(ogl_primitive_topology_, start_vertex, num_vertices);
}

void Rendering_device::draw_indexed(uint32_t num_indices, uint32_t start_index) const
{
	glDrawElements(ogl_primitive_topology_, num_indices, current_index_buffer_->format_, reinterpret_cast<void*>(start_index * current_index_buffer_->index_size_));
}

void Rendering_device::restore_current_input_layout() const
{
	if (current_input_layout_)
	{
		glBindVertexArray(current_input_layout_->id_);

		for (size_t i = 0; i < current_input_layout_->m_attribute_states.size(); ++i)
		{
			current_input_layout_->m_attribute_states[i] ? glEnableVertexAttribArray(i) : glDisableVertexAttribArray(i);
		}
	}
	else
	{
		glBindVertexArray(0);

		for (uint32_t i = 0; i < 8; ++i)
		{
			glDisableVertexAttribArray(i);
		}
	}
}

std::string Rendering_device::get_api_info() const
{
    int major;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);

    int minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);

    return "OpenGL " + string::to_string(major) + "." + string::to_string(minor);
}

uint32_t Rendering_device::get_constant_buffer_offset_alignment() const
{
	int alignment;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

	return static_cast<uint32_t>(alignment);
}

uint32_t Rendering_device::s_ogl_primitive_topology[14] =
{
	0,
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	0,
	0,
	0,
	0,
	GL_LINES_ADJACENCY,
	GL_LINE_STRIP_ADJACENCY,
	GL_TRIANGLES_ADJACENCY,
	GL_TRIANGLE_STRIP_ADJACENCY
};

#ifdef _DEBUG
void error_callback(GLenum /*source*/, GLenum type, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const char* message, void* /*user_param*/)
{
	std::cout << "OpenGL ";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "error: ";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "deprecated behavior: ";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "undefined behavior: ";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "portability: ";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "performance: ";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cout << "marker: ";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cout << "push group: ";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cout << "pop group: ";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "other: ";
		break;
	}

	std::cout << message << std::endl;
}
#endif

}
