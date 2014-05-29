#include "Message.hpp"

namespace logging
{

Message::Message(const std::string& text, Type type, const void* source) : text_(text), type_(type), source_(source)
{}

const std::string& Message::get_text() const
{
	return text_;
}

Message::Type Message::type() const
{
	return type_;
}

const void* Message::get_source() const
{
	return source_;
}

}
