#include "Text_stream.hpp"
#include "File/File_stream.hpp"

namespace file
{

bool read_text_file(const std::string& name, std::string& text)
{
	file::Input_stream stream(name);

	if (!stream)
	{
		return false;
	}

	char buffer[4096];

	while (stream.read(buffer, sizeof(buffer)))
	{
		text.append(buffer, sizeof(buffer));
	}

	text.append(buffer, stream.gcount());

	return true;
}

}
