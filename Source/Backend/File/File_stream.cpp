#include "File_stream.hpp"
#include <physfs.h>

namespace file
{

Physfs_filebuf::Physfs_filebuf() : m_file(nullptr), m_is_write_stream(false)
{}

Physfs_filebuf::~Physfs_filebuf()
{
	close();
}

bool Physfs_filebuf::is_open() const
{
	return m_file != nullptr;
}

Physfs_filebuf* Physfs_filebuf::open(const char* name, std::ios_base::openmode mode)
{
	// Return failure if this file buf is open already
	if (is_open())
	{
		return nullptr;
	}

	// Return failure if we are requested to open a file in an unsupported mode
	if (!(mode & std::ios_base::binary)
	||   ((mode & std::ios_base::in) && (mode & std::ios_base::out)))
	{
		return nullptr;
	}

	// Open the file
	if ((mode & std::ios_base::out) && (mode & std::ios_base::app))
	{
		m_file = PHYSFS_openAppend(name);
		m_is_write_stream = true;
	}
	else if (mode & std::ios_base::out)
	{
		m_file = PHYSFS_openWrite(name);
		m_is_write_stream = true;
	}
	else if (mode & std::ios_base::in)
	{
		m_file = PHYSFS_openRead(name);
		m_is_write_stream = false;
	}
	else
	{
		return nullptr;
	}

	if (!m_file)
	{
		return nullptr;
	}

	if ((mode & std::ios_base::ate) && (mode & std::ios_base::in))
	{
		if (!PHYSFS_seek(m_file, PHYSFS_fileLength(m_file)))
		{
			close();
			return nullptr;
		}
	}

	return this;
}

Physfs_filebuf* Physfs_filebuf::close()
{
	// Return failure if this file buf is closed already
	if (!is_open())
	{
		return nullptr;
	}

	sync();

	if (!PHYSFS_close(m_file))
	{
		return nullptr;
	}

	m_file = nullptr;

	return this;
}

// Read stuff:
Physfs_filebuf::int_type Physfs_filebuf::underflow()
{
	if (!is_open() || m_is_write_stream)
	{
		return traits_type::eof();
	}

	if (PHYSFS_eof(m_file))
	{
		return traits_type::eof();
	}

	PHYSFS_sint64 objects_read = PHYSFS_read(m_file, &*buffer_.begin(), PHYSFS_uint32(sizeof(char_type)), buffer_.size());

	if (objects_read <= 0)
	{
		return traits_type::eof();
	}

	char_type* xend = (static_cast<size_t>(objects_read) == buffer_.size()) ? &(buffer_.back()) + 1 : &buffer_[objects_read];
	setg(&*buffer_.begin(), &*buffer_.begin(), xend);

	return traits_type::to_int_type(buffer_.front());
}

Physfs_filebuf::pos_type Physfs_filebuf::seekpos(pos_type pos, std::ios_base::openmode)
{
	if (!is_open() || m_is_write_stream)
	{
		return pos_type(off_type(-1));
	}

	if (PHYSFS_seek(m_file, static_cast<PHYSFS_uint64>(pos)) == 0)
	{
		return pos_type(off_type(-1));
	}

	// the seek invalidated the buffer
	setg(&*buffer_.begin(), &*buffer_.begin(), &*buffer_.begin());

	return pos;
}

Physfs_filebuf::pos_type Physfs_filebuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
{
	if (!is_open() || m_is_write_stream)
	{
		return pos_type(off_type(-1));
	}

	off_type pos = off;

	switch (dir)
	{
	case std::ios_base::beg:
		break;

	case std::ios_base::cur:
		{
			PHYSFS_sint64 ptell = PHYSFS_tell(m_file);
			pos_type buf_tell = static_cast<pos_type>(ptell) - static_cast<pos_type>(__streambuf_type::egptr() - __streambuf_type::gptr());

			if (!off)
			{
				return buf_tell;
			}

			pos += static_cast<off_type> (buf_tell);
			break;
		}

	case std::ios_base::end:
		pos = static_cast<off_type> (PHYSFS_fileLength(m_file)) - pos;
		break;

	default:
		//_ASSERTE(!"invalid seekdirection");
		return pos_type(off_type(-1));
	}

	return seekpos(static_cast<pos_type> (pos), mode);
}

// Write stuff:
Physfs_filebuf::int_type Physfs_filebuf::overflow(int_type c)
{
	if (!is_open() || !m_is_write_stream)
	{
		return traits_type::eof();
	}

	size_t size = __streambuf_type::pptr() - __streambuf_type::pbase();
		
	if (!size)
	{
		return 0;
	}

	PHYSFS_sint64 res = PHYSFS_write(m_file, __streambuf_type::pbase(), PHYSFS_uint32(sizeof(char_type)), size);

	if (res <= 0)
	{
		return traits_type::eof();
	}

	if (!traits_type::eq_int_type(c, traits_type::eof()))
	{
		PHYSFS_sint64 res = PHYSFS_write(m_file, &c, sizeof(char_type), 1);

		if (res <= 0)
		{
			return traits_type::eof();
		}
	}

	char_type* xend = (static_cast<size_t> (res) == buffer_.size()) ? &*buffer_.end() : &buffer_[res];
	setp(&*buffer_.begin(), xend);
		
	return 0;
}

int Physfs_filebuf::sync()
{
	if (!is_open() || !m_is_write_stream)
	{
		return -1;
	}

	return overflow(traits_type::eof());
}

std::streamsize Physfs_filebuf::showmanyc()
{
	if (!is_open() || m_is_write_stream)
	{
		return -1;
	}

	PHYSFS_sint64 fileSize = PHYSFS_fileLength(m_file);

	return static_cast<int>(fileSize);
}

//-----------------

Input_stream::Input_stream() : __istreatype_(&m_stream_buffer)
{}

Input_stream::Input_stream(const std::string& name, std::ios_base::openmode mode) : __istreatype_(&m_stream_buffer), name_(name)
{
    open(name.c_str(), mode);
}

Input_stream::Input_stream(const char* name, std::ios_base::openmode mode) : __istreatype_(&m_stream_buffer), name_(name)
{
	open(name, mode);
}

const Physfs_filebuf* Input_stream::rdbuf() const
{
	return &m_stream_buffer;
}

Physfs_filebuf* Input_stream::rdbuf()
{
	return &m_stream_buffer;
}

bool Input_stream::is_open() const
{
	return rdbuf()->is_open();
}

void Input_stream::open(const char* name, std::ios_base::openmode mode)
{
	if (!rdbuf()->open(name, mode | std::ios_base::in))
	{
		__istreatype_::setstate(std::ios_base::failbit);
	}
}

void Input_stream::close()
{
	if (!rdbuf()->close())
	{
		__istreatype_::setstate(std::ios_base::failbit);
	}
}

const std::string& Input_stream::name() const
{
		return name_;
}

}
