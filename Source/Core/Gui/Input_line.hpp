#pragma once

#include "gui_Component.hpp"
#include "Event.hpp"
#include <string>

namespace platform
{

class Input_signal;

}

namespace gui
{

class Input_line : public Component
{

public:

	Input_line();

	virtual void update(const float speed);

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	const std::string& text() const;
	void set_text(const std::string& text);
		
	Event m_on_accept;

private:

	void insert(int pos, const std::string& text);
	void erase (int pos, int length);
	void move  (int delta);
	void move_to(int pos);

	void add_to_history(const std::string& text);
	void scroll(int delta);

	void process_signal(const platform::Input_signal& signal);

	void erase_marked_region();
	void resolve_marked_region();

	static bool is_banned_key(uint32_t key);

	struct Marked_region
	{
		Marked_region();

		bool is_marked() const;
		void reset();

		int position;
		int length;

		int started_position;
		int stopped_position;

		bool marking;
	};

	int cursor_position_;

	Marked_region marked_;

	std::string text_;
	std::list<std::string> m_history;
	std::string            m_current_line;
	size_t                 m_maxHistory;
	int                    m_history_pos;

	int m_character_width;

	float m_cursor_blink;
};

}
