#pragma once

#include <string>

namespace logging
{

class Message
{

public:

	enum class Type
	{
		Input,
		Output,
		Ok,
		Warning,
		Error
	};

	Message(const std::string& text, Type type, const void* source = nullptr);

	const std::string& get_text() const;
	Type get_type() const;
	const void* get_source() const;

private:

	std::string text_;
	Type type_;
	const void* source_;
};

}
