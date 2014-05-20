#pragma once

#include "gui_Component.hpp"
#include "Border_button.hpp"
#include "Container.hpp"
#include "Rendering/Color.hpp"
#include <string>

namespace gui
{

class Frame_manager;

class Frame : public Component
{
		
public:

	enum class State
	{
		Minimized,
		Windowed,
		Maximized,
		Close_pending
	};

	Frame(bool drawBackground = true);

	void setManager(Frame_manager *manager);

	void update(const float speed);

	void update(const Gui_input& input);

	void render(rendering::Printer& printer);

	const std::string& get_caption() const;
	void set_caption(const std::string& m_caption);

	float2 get_client_size() const;
	void set_client_size(const float2& size);

	float2 get_client_offset() const;

	bool is_minimized() const;
	bool is_close_pending() const;

	void minimize();
	void restore();
	void close();

protected:

	virtual void on_set_size();

	void set_state(State state);

	virtual void on_minimize_button_clicked(Component* source);
	virtual void on_close_button_clicked(Component* source);

	virtual void render_private(rendering::Printer& printer);

	Container m_container;

	Border_button m_minimize_button;
	Border_button m_close_button;

private:

	Frame_manager *m_manager;

	std::string m_caption;

	State m_current_state;

	bool m_drawBackground;

	bool m_grabbed;

	static const float s_top_border_height;
	static const float s_bottom_border_height;
	static const float s_side_boder_width;

public:

	static const rendering::Color4 s_frame_color;
	static const rendering::Color4 s_frame_color_focus;
};

}
