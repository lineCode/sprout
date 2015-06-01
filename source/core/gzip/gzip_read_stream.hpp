#pragma once

#include "miniz/miniz.hpp"
#include <streambuf>
#include <istream>
#include <memory>
#include <array>

namespace gzip {

class Filebuffer : public std::basic_streambuf<char, std::char_traits<char>> {

public:
	// Types:
	typedef char                   char_type;
	typedef std::char_traits<char> traits_type;
	typedef traits_type::int_type  int_type;
	typedef traits_type::pos_type  pos_type;
	typedef traits_type::off_type  off_type;

	typedef std::basic_streambuf<char, std::char_traits<char>>   __streambuf_type;

	Filebuffer();

	virtual ~Filebuffer();

	bool is_open() const;

	Filebuffer* open(const char* filename, std::ios_base::openmode mode);

	Filebuffer* open(std::unique_ptr<std::istream> stream);

	Filebuffer* close();

protected:
	// Read stuff:
	virtual int_type underflow();

	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode);

	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode);

	virtual int_type overflow(int_type c = traits_type::eof());

	virtual int sync();

	virtual std::streamsize showmanyc();

private:

	std::unique_ptr<std::istream> stream_;

	mz_stream z_stream_;

//	PHYSFS_File* m_file;
//	bool m_is_write_stream;
	std::array<char_type, 8192> read_buffer_;

	std::array<char_type, 8192> buffer_;
};

class Read_stream : public std::basic_istream<char, std::char_traits<char>> {

public:
	// Types:
	typedef char                   char_type;
	typedef std::char_traits<char> traits_type;
	typedef traits_type::int_type  int_type;
	typedef traits_type::pos_type  pos_type;
	typedef traits_type::off_type  off_type;

	typedef std::basic_istream<char, std::char_traits<char>> __istream_type;

	Read_stream();

	explicit Read_stream(const std::string& name, std::ios_base::openmode mode = std::ios_base::binary);
	explicit Read_stream(const char* name, std::ios_base::openmode mode = std::ios_base::binary);
	explicit Read_stream(std::unique_ptr<std::istream> stream);

	const Filebuffer* rdbuf() const;

	Filebuffer* rdbuf();

	bool is_open() const;

	void open(char const* name, std::ios_base::openmode mode = std::ios_base::binary);
	void open(std::unique_ptr<std::istream> stream);

	void close();

private:

	Filebuffer stream_buffer_;
};

}
