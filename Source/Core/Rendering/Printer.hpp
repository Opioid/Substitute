#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Vector.hpp"
#include "Fonts/Font_factory.hpp"
#include <vector>

class Resource_manager;

namespace rendering
{

class Rendering_tool;
class Effect;
class Effect_technique;
class Vertex_buffer;
class Shader_resource_view;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;

class Printer
{

public:

	Printer(Rendering_tool& rendering_tool);
	~Printer();

	bool init(Resource_manager& resource_manager);

	void begin();
	void end();

	float2 screen_dimensions() const;
	int line_height() const;

	bool set_font(const std::string& name, uint32_t width, uint32_t height = 0);

	void set_position(const float2& position);
	void set_color(const rendering::Color4 &color);
	void set_texture_coordinates(const float2& left_top = float2(0.f, 0.f), const float2& right_bottom = float2(1.f, 1.f));
	void set_texture(const Handle<Shader_resource_view>& texture);

	void print(const std::string& text);

	void draw_quad(const float2& size);

	void flush(bool draw_as_font = true);

private:

	void draw_all(uint32_t count);

	bool create_render_states();

    Rendering_tool& rendering_tool_;
    Resource_manager* resource_manager_;

	int  m_column;
	bool m_has_texture;

	float2 position_;
	float4 texture_coordinates_;
	unsigned int color_;

	fonts::Font_factory m_font_factory;

	Handle<Effect> effect_;
	Effect_technique* color_technique_;
	Effect_technique* texture_technique_;
	Effect_technique* font_technique_;

	Handle<Input_layout> input_layout_;

	struct Rect_vertex
	{
		float4 pos_and_size;
		float4 tex_coord;
		unsigned int color;
	};

	uint32_t     num_vertices_;
	Rect_vertex* vertices_;

	uint32_t     current_vertex_;

	Handle<Vertex_buffer> vertex_buffer_;

	struct Change_per_source
	{
		float2 inverse_half_source_size;
	};

	Constant_buffer_updater<Change_per_source> change_per_source_;

	Handle<Rasterizer_state> rasterizer_state_;
	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state> blend_state_;

	struct Font
	{
		struct Instance
		{
			Instance(Font *font, uint32_t width, uint32_t height);
			~Instance();

			struct Glyph_info
			{
				float2 size;
				float2 offset;
				float  advance;

				float4 tex_coord;
			};

			Font *const font;
			const uint32_t width, height;

			Handle<Shader_resource_view> texture_;

			float tab_width;

			float line_height;

			Glyph_info glyph_infos[192];
		};

		Font(const std::string& name);
		~Font();

		std::string name;
		std::vector<Instance*> instances;
	};

	std::vector<Font*> m_fonts;
	Font::Instance* m_current_font_instance;
};

}
