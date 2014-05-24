#pragma once

#include "Math/Vector.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Printer;
class Shader_resource_view;

}

class Input_state;
class Cursor;


namespace gui
{

struct Gui_input;

class Component
{

public:

	Component();
	virtual ~Component();

	void set_parent(const Component* parent);

	virtual void update(const float speed);
	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	float2 get_absolute_position() const;
	void set_position(const float2& pos);

	const float2& dimensions() const;
	void set_size(const float2& size);

	bool has_focus() const;
	void set_focus(bool focus);

	bool is_enabled() const;
	void set_enabled(bool enabled);

protected:

	virtual void on_set_size();

	const Component* parent_;

	float2 position;
	float2 size_;

	bool m_has_focus;
	bool m_enabled;

public:

	static void set_texture(const Handle<rendering::Shader_resource_view>& texture);

protected:

	static Handle<rendering::Shader_resource_view> s_texture;

};

}
