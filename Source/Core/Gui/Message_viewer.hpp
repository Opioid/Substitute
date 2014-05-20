#pragma once

#include "gui_Component.hpp"
#include "Scroll_bar.hpp"
#include "../Logging/Message_receiver.hpp"
#include "../Logging/Message.hpp"
#include <string>
#include <list>

namespace gui
{

class Message_viewer : public Component, public logging::Message_receiver
{
	struct Entry
	{
		Entry(const logging::Message &message, size_t rows);

		logging::Message m_message;
		size_t           m_rows;
	};

	Scroll_bar m_scrollBar;

	size_t           m_maxEntries;
	std::list<Entry> m_entries;

public:

	Message_viewer();

	void set_line_height(int line_height);

	virtual void update(const float speed);

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

private:

	void on_set_size();

	virtual void on_receive(const logging::Message &message);

	void print(const std::string& text, size_t offset, rendering::Printer& printer);

	size_t count_rows(const std::string& text) const;

	size_t m_printed_rows;
	size_t m_num_rows;
	size_t m_num_columns;

	size_t m_startRow;
	size_t m_occupiedRows;

    int m_character_width;
	int m_line_height;
};

}
