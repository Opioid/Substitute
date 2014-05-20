#include "Message.hpp"

namespace logging
{
	Message::Message(const std::string& text, Type::Value type, const void* source) : text_(text), type_(type), m_source(source)
	{}

	const std::string& Message::get_text() const
	{
		return text_;
	}

	Message::Type::Value Message::get_type() const
	{
		return type_;
	}

	const void* Message::get_source() const
	{
		return m_source;
	}
}
