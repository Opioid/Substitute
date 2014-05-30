#include "Message_viewer.hpp"
#include "Rendering/Printer.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Resource_view.hpp"
#include "Math/Vector.inl"
#include <vector>
// To suppress a warning originating somewhere in boost - problematic if we treat warnings as errors
#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include "boost/algorithm/string.hpp"

namespace gui
{

	Message_viewer::Entry::Entry(const logging::Message &message, size_t rows) : m_message(message), m_rows(rows)
	{}

    Message_viewer::Message_viewer() :
        m_maxEntries(256), m_num_rows(36), m_num_columns(96),
        m_startRow(0), m_occupiedRows(0),
        m_character_width(10), m_line_height(10)
	{
	//	m_scrollBar.set_parent(this);
	//	m_scrollBar.setClientWindow(m_num_rows);
	//	m_scrollBar.setClientTotal(m_occupiedRows);
	//	m_scrollBar.setClientStart(m_startRow);
	}

	void Message_viewer::set_line_height(int line_height)
	{
		m_line_height = line_height;

		on_set_size();
	}

    void Message_viewer::update(const float /*speed*/)
	{
	//	m_scrollBar.update(speed);
	}
	
    void Message_viewer::update(const Gui_input& /*input*/)
	{
	//	m_scrollBar.update(keyboard, mouse, cursor, screen_size);

		m_startRow = m_scrollBar.getClientStart();
	}

	void Message_viewer::render(rendering::Printer& printer)
	{
		printer.set_texture(nullptr);
		printer.set_color(rendering::Color4(0.01f, 0.01f, 0.01f, 0.975f));

		printer.set_position(get_absolute_position());
		printer.draw_quad(size_);
		
		printer.flush(false);

	//	m_scrollBar.render(printer);

		printer.set_font("Consola", 18);

		m_printed_rows = 0;

		size_t currentRow = 0;

		for (auto& entry : m_entries)
		{
			size_t entryOffset = 0;

			if (currentRow < m_startRow)
			{
				entryOffset = m_startRow - currentRow;

				if (entryOffset > entry.m_rows)
				{
					currentRow += entry.m_rows;
					continue;
				}
				else
				{
					currentRow += entryOffset;
				}
			}
			
			switch (entry.m_message.type())
			{
				case logging::Message::Type::Output:
					printer.set_color(rendering::Color4(0.3f, 0.3f, 1.f));
					break;

				case logging::Message::Type::Ok:
					printer.set_color(rendering::Color4(0.f, 0.8f, 0.f));
					break;

				case logging::Message::Type::Warning:
					printer.set_color(rendering::Color4(1.f, 0.5f, 0.f));
					break;

				case logging::Message::Type::Error:
					printer.set_color(rendering::color4::red);
					break;

				default:
					printer.set_color(rendering::color4::white);
					break;
			}

			print(entry.m_message.get_text(), entryOffset, printer);

			if (m_printed_rows >= m_num_rows)
			{
				break;
			}
		}

		printer.flush();
	}

	void Message_viewer::on_set_size()
	{
		m_scrollBar.set_position(float2(size_.x - 14.f, 0.f));
		m_scrollBar.set_size(size_);
		
		m_num_columns = uint32_t(size_.x) / m_character_width;
		m_num_rows = uint32_t(size_.y) / m_line_height;
	}

	void Message_viewer::on_receive(const logging::Message &message)
	{
		Entry entry(message, count_rows(message.get_text()));

		m_entries.push_back(entry);

		if (m_entries.size() > m_maxEntries)
		{
			Entry front = m_entries.front();
			m_occupiedRows -= front.m_rows;
			m_entries.pop_front();
		}

		m_occupiedRows += entry.m_rows;

		if (m_occupiedRows > m_num_rows)
		{
			m_startRow = m_occupiedRows - m_num_rows;	
		}
		else m_startRow = 0;

		m_scrollBar.setClientTotal(m_occupiedRows);
		m_scrollBar.setClientStart(m_startRow);
	}

	void Message_viewer::print(const std::string& text, size_t offset, rendering::Printer& printer)
	{
		std::vector<std::string> lines;

		boost::split(lines, text, boost::is_any_of("\n"));

		size_t row = 0;

		for (auto& segment : lines)
		{
			for (;;)
			{
				if (row < offset) 
				{
					++row;
				}
				else
				{
					printer.set_position(get_absolute_position() + float2(2.f, 0 + printer.line_height() * float(m_printed_rows)));

					printer.print/*_raw*/(segment.length() > m_num_columns ? segment.substr(0, m_num_columns) : segment);

					++m_printed_rows;
				}

				if (m_printed_rows >= m_num_rows) 
				{
					return;
				}

				if (segment.length() > m_num_columns) 
				{
					segment = segment.substr(m_num_columns);
				}
				else
				{
					break;
				}
			}
		}
	}

	size_t Message_viewer::count_rows(const std::string& text) const
	{
		std::vector<std::string> lines;

		boost::split(lines, text, boost::is_any_of("\n"));

		size_t rows = 0;

		for (auto& line : lines)
		{
			rows += 1 + line.length() / m_num_columns;
		}

		return rows;
	}
}
