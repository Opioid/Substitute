#include "Printer.hpp"
#include "Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "../Resources/Resource_manager.hpp"
#include "Effect/Effect.hpp"
#include "Fonts/Font_descriptor.hpp"

namespace rendering
{

Printer::Font::Instance::Instance(Printer::Font* font, uint32_t width, uint32_t height)
	: font(font), width(width), height(height)
{}

Printer::Font::Instance::~Instance()
{}

Printer::Font::Font(const std::string& name) : name(name)
{}

Printer::Font::~Font()
{
	for (auto i = instances.begin(); i != instances.end(); ++i)
	{
		delete *i;
	}
}

Printer::Printer(Rendering_tool& rendering_tool)
    : rendering_tool_(rendering_tool), m_column(0), m_has_texture(false),
      color_(0xffffffff), num_vertices_(1024), m_current_vertex(0), m_current_font_instance(nullptr)
{
    vertices_ = new Rect_vertex[num_vertices_];
}

Printer::~Printer()
{
	m_font_factory.release();

	delete [] vertices_;

	for (std::vector<Font*>::iterator i = m_fonts.begin(); i != m_fonts.end(); ++i)
	{
		delete *i;
	}

	m_fonts.clear();
}

bool Printer::init(Resource_manager& resource_manager)
{
	resource_manager_ = &resource_manager;

	effect_ = resource_manager.load<Effect>("Effects/Printer.effect");

	if (!effect_)
	{
		return false;
	}

	color__technique   = effect_->get_technique("Color");
	texture__technique = effect_->get_technique("Texture");
	m_font_technique    = effect_->get_technique("Font");

	Vertex_layout_description::Element layout[] =
	{
		Vertex_layout_description::Element("Position",  0, Data_format::R32G32B32A32_Float),
		Vertex_layout_description::Element("Tex_coord", 0, Data_format::R32G32B32A32_Float),
		Vertex_layout_description::Element("Color",     0, Data_format::R8G8B8A8_UNorm)
	};

	Vertex_layout_description description(3, layout);

	input_layout_ = rendering_tool_.get_vertex_layout_cache().get_input_layout(description, m_font_technique->get_program()->get_signature());

	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_source_.init(effect_, "Change_per_source"))
	{
		return false;
	}

	vertex_buffer_ = rendering_tool_.get_device().create_vertex_buffer(sizeof(Rect_vertex) * num_vertices_);
	if (!vertex_buffer_)
	{
		return false;
	}

	if (!m_font_factory.init())
	{
		return false;
	}

	return create_render_states();
}

void Printer::begin()
{
	auto& device = rendering_tool_.get_device();

	device.set_primitive_topology(Primitive_topology::Point_list);
	device.set_input_layout(input_layout_);
	device.set_vertex_buffer(vertex_buffer_, sizeof(Rect_vertex));

	device.set_rasterizer_state(rasterizer_state_);
	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	effect_->use(device);

	uint2 size = rendering_tool_.get_size();
	change_per_source_.get_data().inverse_half_source_size = float2(1.f / (0.5f * size.x), -1.f / (0.5f * size.y));
	change_per_source_.update(device);
}

void Printer::end()
{
	flush();

	set_texture(nullptr);
}

float2 Printer::get_screen_size() const
{
	return float2(rendering_tool_.get_size());
}

int Printer::get_line_height() const
{
	if (!m_current_font_instance)
	{
	return 0;
	}

	return int(m_current_font_instance->line_height);
}

bool Printer::set_font(const std::string& name, uint32_t width, uint32_t height)
{
	Font* font = nullptr;

	for (auto i = m_fonts.begin(); i != m_fonts.end(); ++i)
	{
		if ((*i)->name == name)
		{
			font = *i;
			break;
		}
	}

	if (!font)
	{
		m_fonts.push_back(font = new Font(name));
	}

	m_current_font_instance = nullptr;
	for (auto i = font->instances.begin(); i != font->instances.end(); ++i)
	{
		if ((*i)->width == width && (*i)->height == height)
		{
			m_current_font_instance = *i;
			break;
		}
	}

	if (!m_current_font_instance)
	{
		// This is where we finally have to create a new font texture...

		std::string resolved_name;
		if (!resource_manager_->get_virtual_file_system().get_resolved_name(resolved_name, "Fonts/" + font->name + ".ttf"))
		{
			return false;
		}

		fonts::Font_descriptor desc;
		if (!m_font_factory.fill_font_descriptor(&desc, resolved_name, width, height))
		{
			return false;
		}

		font->instances.push_back(m_current_font_instance = new Font::Instance(font, width, height));

		const float _1_div_width  = 1.f / float(desc.width);
		const float _1_div_height = 1.f / float(desc.height);

		for (size_t g = 0; g < 192; ++g)
		{
			const auto& glyph = desc.glyphs[g];
			auto& glyph_info = m_current_font_instance->glyph_infos[g];

			glyph_info.size = float2(float(glyph.width), float(glyph.height));
			glyph_info.offset = float2(float(glyph.offset_x), float(glyph.offset_y));
			glyph_info.advance = float(glyph.advance);

			const float2 left_top = float2(float(glyph.start_x) * _1_div_width, float(glyph.start_y) * _1_div_height);
			const float2 right_bottom = left_top + float2(float(glyph.width) * _1_div_width, float(glyph.height) * _1_div_height);
			glyph_info.tex_coord  = float4(left_top, right_bottom);
		}

		m_current_font_instance->line_height = float(desc.line_height);
		m_current_font_instance->tab_width   = float(desc.tab_width);

		Texture_description description;
		description.format = Data_format::Value::R8_UNorm;
		description.dimensions.x = desc.width;
		description.dimensions.y = desc.height;
		description.dimensions.z = 1;
		description.num_mip_levels = 1;

		Texture_description::Data data;
		data.dimensions = description.dimensions;
		data.buffer = desc.face_data;
		data.num_bytes = data.dimensions.x * data.dimensions.y * sizeof(unsigned char);

		Generic_texture_data_adapter adapter(description, &data, false);

		Handle<Texture> texture = rendering_tool_.get_device().create_texture_2D(adapter);

		if (!texture)
		{
			return false;
		}

		m_current_font_instance->texture_ = rendering_tool_.get_device().create_shader_resource_view(texture, resolved_name);

	//	exportFont(name, width, height, m_currentFontInstance->texture_->get_texture(), desc);
	}

	flush();

	rendering_tool_.get_device().set_shader_resources(1, &m_current_font_instance->texture_, 1);

	return true;
}

void Printer::set_position(const float2& position)
{
	position_ = position;
	m_column = 0;
}

void Printer::set_color(const rendering::Color4 &color)
{
	color_ = static_cast<unsigned int>(color); // using explicit tVector4 -> unsigned int cast operator
}

void Printer::set_texture_coordinates(const float2& left_top, const float2& right_bottom)
{
	texture_coordinates_ = float4(left_top, right_bottom);
}

void Printer::set_texture(const Handle<Shader_resource_view>& texture)
{
	rendering_tool_.get_device().set_shader_resources(1, &texture, 0);

	m_has_texture = texture != nullptr;
}

void Printer::print(const std::string& text)
{
	if (!m_current_font_instance)
	{
		return;
	}

	float2 position = position_;

	const char* chars = text.c_str();

	for (; *chars != '\0'; ++chars)
	{
		if ('\r' == *chars)
		{
			continue;
		}
		else if ('\n' == *chars)
		{
			position.x = position.x;
			position.y += m_current_font_instance->line_height;
			m_column = 0;
			continue;
		}
		else if ('\t' == *chars)
		{
			int tab = 4 - m_column % 4;
			position.x += m_current_font_instance->tab_width * tab;
			m_column += tab;
			continue;
		}

		const Font::Instance::Glyph_info& g = m_current_font_instance->glyph_infos[fonts::Font_descriptor::s_character_mappings[(unsigned char)(*chars)]];

		vertices_[m_current_vertex].pos_and_size = float4(position + g.offset, g.size);
		vertices_[m_current_vertex].tex_coord = g.tex_coord;
		vertices_[m_current_vertex].color = color_;

		position.x += g.advance;

		++m_column;

		if (++m_current_vertex == num_vertices_)
		{
			flush();
		}
	}

	position = position;
}

void Printer::flush(bool draw_as_font)
{
	if (0 == m_current_vertex)
	{
		return;
	}

	if (draw_as_font)
	{
		m_font_technique->use();
	}
	else if (m_has_texture)
	{
		texture__technique->use();
	}
	else
	{
		color__technique->use();
	}

	draw_all(m_current_vertex);

	m_current_vertex = 0;
}

void Printer::draw_quad(const float2& size)
{
	vertices_[m_current_vertex].pos_and_size = float4(position_, size);
	vertices_[m_current_vertex].tex_coord = texture_coordinates_;
	vertices_[m_current_vertex].color = color_;

	if (++m_current_vertex == num_vertices_)
	{
		flush(false);
	}
}

void Printer::draw_all(uint32_t count)
{
	auto& device = rendering_tool_.get_device();

	device.update_buffer(*vertex_buffer_, 0, sizeof(Rect_vertex) * count, vertices_);

	device.draw(count);
}

bool Printer::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_state_ = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.render_targets[0].blend_enable            = true;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::Inverse_source_alpha;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::Source_alpha;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;

	blend_state_ = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

}
