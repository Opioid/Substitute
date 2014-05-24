#pragma once

#include <streambuf>
#include <istream>
#include <ostream>
#include <array>

struct PHYSFS_File;

namespace file
{

class Physfs_filebuf : public std::basic_streambuf<char, std::char_traits<char>>
{

public:
	// Types:
	typedef char                   char_type;
	typedef std::char_traits<char> traits_type;
	typedef traits_type::int_type  int_type;
	typedef traits_type::pos_type  pos_type;
	typedef traits_type::off_type  off_type;

	typedef std::basic_streambuf<char, std::char_traits<char>>   __streambuf_type;

	Physfs_filebuf();

	virtual ~Physfs_filebuf();

	bool is_open() const;

	Physfs_filebuf* open(char const* name, std::ios_base::openmode mode);

	Physfs_filebuf* close();

protected:
	// Read stuff:
	virtual int_type underflow();

	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode);

	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode);

	virtual int_type overflow(int_type c = traits_type::eof());

	virtual int sync();

	virtual std::streamsize showmanyc();

private:

	PHYSFS_File* m_file;
	bool m_is_write_stream;
	std::array<char_type, 1024> buffer_;
};

class Input_stream : public std::basic_istream<char, std::char_traits<char>>
{

public:
	// Types:
	typedef char                   char_type;
	typedef std::char_traits<char> traits_type;
	typedef traits_type::int_type  int_type;
	typedef traits_type::pos_type  pos_type;
	typedef traits_type::off_type  off_type;

	typedef std::basic_istream<char, std::char_traits<char>> __istreatype_;

	Input_stream();

	explicit Input_stream(const std::string& name, std::ios_base::openmode mode = std::ios_base::binary);
	explicit Input_stream(char const* name, std::ios_base::openmode mode = std::ios_base::binary);

	const Physfs_filebuf* rdbuf() const;

	Physfs_filebuf* rdbuf();

	bool is_open() const;

	void open(char const* name, std::ios_base::openmode mode = std::ios_base::binary);

	void close();

	const std::string& name() const;

private:

	std::string name_;

	Physfs_filebuf m_stream_buffer;
};

}
