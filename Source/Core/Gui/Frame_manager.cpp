#include "Frame_manager.hpp"
#include "Gui_input.hpp"
#include "Frame.hpp"
#include "Application/Cursor.hpp"
#include "Rendering/Printer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "Controls/Input_state.hpp"
#include "Math/Vector.inl"
#include "Logging/Logging.hpp"

namespace gui
{

const float Frame_manager::s_task_bar_height = 40.f;
const float Frame_manager::s_task_item_width = 256.f;

void Frame_manager::clear()
{
	m_frames.clear();

	m_frames_render_order.clear();
}

void Frame_manager::add(Frame* frame)
{
	if (!m_frames_render_order.empty())
	{
		m_frames_render_order.back()->set_focus(false);
	}

	frame->set_focus(true);
	frame->setManager(this);

	m_frames.push_back(frame);

	m_frames_render_order.push_back(frame);
}

void Frame_manager::update(const float /*speed*/)
{}

void Frame_manager::update(const Gui_input& input)
{
	const float2& pos = input.cursor.coord();

	m_hovering_over = nullptr;
	std::list<Frame*>::const_iterator h;

	if (pos.y >= input.screen_size.y - s_task_bar_height && pos.y <= input.screen_size.y)
	{
        float2 p(0.f, input.screen_size.y - s_task_bar_height);

		for (auto i = m_frames.begin(); i != m_frames.end(); ++i)
		{
			if (pos.x >= p.x && pos.x <= p.x + s_task_item_width)
			{
				m_hovering_over = *i;
				h = i;
				break;
			}

			p += float2(s_task_item_width, 0.f);
		}
	}

	if (input.mouse.is_key_click(0))
	{
		if (m_hovering_over)
		{
			// Something is happening in the taskbar...

			if (m_hovering_over == get_selected_frame() && !m_hovering_over->is_minimized())
			{
				m_hovering_over->minimize();
			}
			else
			{
				get_selected_frame()->set_focus(false);

				m_frames_render_order.remove(m_hovering_over);
				m_frames_render_order.push_back(m_hovering_over);

				m_hovering_over->set_focus(true);
				m_hovering_over->restore();
			}
		}
		else
		{
			// Something is happening in the window area...

			for (auto i = m_frames_render_order.rbegin(); i != m_frames_render_order.rend(); ++i)
			{
				Frame* frame = *i;

				float2 apos = frame->get_absolute_position();

				if (pos.x >= apos.x && pos.x <= apos.x + frame->dimensions().x
				&&  pos.y >= apos.y && pos.y <= apos.y + frame->dimensions().y)
				{
					if (get_selected_frame() == frame)
					{
						break;
					}

						// Bring a background window to the front

					get_selected_frame()->set_focus(false);

					m_frames_render_order.erase(--i.base());
					m_frames_render_order.push_back(frame);

					frame->set_focus(true);
					frame->restore();

					break;
				}
			}
		}
	}

	Frame* selected_frame = get_selected_frame();

	if (selected_frame && !selected_frame->is_minimized())
	{
		selected_frame->update(input);

		if (selected_frame->is_close_pending())
		{
			for (auto f = m_frames.begin(); f != m_frames.end(); ++f)
			{
				if (*f == selected_frame)
				{
					m_frames.erase(f);
					break;
				}
			}

			m_frames_render_order.pop_back();

			delete selected_frame;

			selected_frame = get_selected_frame();

			if (selected_frame)
			{
				selected_frame->set_focus(true);
			}
		}
	}
}

void Frame_manager::render(rendering::Printer& printer)
{
	for (auto frame : m_frames_render_order)
	{
		if (!frame->is_minimized())
		{
			frame->render(printer);
		}
	}
			
	printer.set_texture(nullptr);
	printer.set_color(rendering::Color4(0.005f, 0.005f, 0.0075f, 1.f));

	const float2 screen_size = printer.screen_dimensions();

	float2 pos(0, screen_size.y - s_task_bar_height);

	printer.set_position(pos);
	printer.draw_quad(float2(screen_size.x, s_task_bar_height));

	printer.flush(false);

	for (auto frame : m_frames)
	{
		rendering::Color4 color;

		if (get_selected_frame() == frame)
		{
			color = Frame::s_frame_color_focus;
		}
		else
		{
			color = Frame::s_frame_color;
		}

		if (m_hovering_over == frame)
		{
			color += rendering::Color4(0.025f, 0.025f, 0.025f, 0.f);
		}

		printer.set_color(color);
		printer.set_position(pos);
		printer.draw_quad(float2(s_task_item_width, s_task_bar_height));

		printer.flush(false);
			
		printer.set_color(rendering::Color4(0.f, 0.3f, 0.6f));
		printer.set_font("segoeui", 28);
		printer.set_position(pos + float2(18.f, 3.f));
		printer.print(frame->get_caption());

		printer.flush();
			
		pos += float2(s_task_item_width, 0.f);
	}
}

void Frame_manager::frame_change_state(Frame* frame)
{
	if (frame->is_minimized() && get_selected_frame() == frame)
	{
		frame->set_focus(false);

		m_frames_render_order.pop_back();

		if (get_selected_frame())
		{
			get_selected_frame()->set_focus(true);
		}

		m_frames_render_order.push_front(frame);
	}
}

Frame* Frame_manager::get_selected_frame() const
{
	return m_frames_render_order.empty() ? nullptr : m_frames_render_order.back();
}

}
