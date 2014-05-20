#pragma once

#include "gui_Component.hpp"
#include <list>

namespace gui
{

class Frame;

class Frame_manager : public Component
{

public:

	void clear();
	void add(Frame* frame);

	virtual void update(const float speed);

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	void frame_change_state(Frame* frame);

	Frame* get_selected_frame() const;

private:

	std::list<Frame*> m_frames;

	std::list<Frame*> m_frames_render_order;

	Frame* m_hovering_over;

	static const float s_task_bar_height;
	static const float s_task_item_width;
};

}
