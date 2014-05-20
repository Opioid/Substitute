#pragma once

#include <string>

namespace logging
{
	class Message
	{

	public:

		struct Type
		{
			enum Value
			{
				Input,
				Output,
				Ok,
				Warning,
				Error
			};
		};

		Message(const std::string& text, Type::Value type, const void* source = nullptr);

		const std::string& get_text() const;
		Type::Value get_type() const;
		const void* get_source() const;

	private:

		std::string text_;
		Type::Value type_;
		const void* m_source;
	};
}
