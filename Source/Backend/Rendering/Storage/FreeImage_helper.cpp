#include "FreeImage_helper.hpp"
#include <istream>

namespace fi
{

std::string error_message_;

void error_handler(FREE_IMAGE_FORMAT /*fif*/, const char* message)
{
	error_message_ = message;
}

const std::string& error_message()
{
	return error_message_;
}

unsigned int read(void* buffer, unsigned int size, unsigned int count, fi_handle handle)
{
	std::istream* stream = reinterpret_cast<std::istream*>(handle);

	stream->read(reinterpret_cast<char*>(buffer), count * size);

	return stream->gcount();
}

int write(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	return fwrite(buffer, size, count, (FILE *)handle);
}

int seek(fi_handle handle, long offset, int origin)
{
	std::istream* stream = reinterpret_cast<std::istream*>(handle);

	std::ios::seekdir direction;

	switch (origin)
	{
	case SEEK_SET:
		direction = std::ios::beg;
		break;
	case SEEK_CUR:
		direction = std::ios::cur;
		break;
	case SEEK_END:
		direction = std::ios::end;
		break;
	default:
		direction = std::ios::beg;
		break;
	}

	stream->seekg(offset, direction);

	return *stream ? 0 : 1;
}

long tell(fi_handle handle)
{
	std::istream* stream = reinterpret_cast<std::istream*>(handle);
	return long(stream->tellg());
}

}
