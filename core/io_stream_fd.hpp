/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	_include_cl3_core_io_stream_fd_hpp_
#define	_include_cl3_core_io_stream_fd_hpp_

#include "system_compiler.hpp"
#include "system_os.hpp"
#include "io_stream-util.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	stream
		{
			namespace	fd
			{
				#if (CL3_OS == CL3_OS_WINDOWS)
					typedef void* TFD;	//	HANDLE
				#elif (CL3_OS == CL3_OS_POSIX)
					typedef int TFD;	//	fd
				#endif

				//	generic buffered io-stream for interaction with operating systems io-structures (POSIX-fd, Windows HANDLE, etc.)
				class	TFDStream : virtual IIn<byte>, virtual IOut<byte>, virtual AWriteOut<byte>
				{
					protected:
						byte* p_buffer;
						size_t sz_buffer;
						TFD fd;

					public:
						CL3PUBF	size_t	Read	(byte* arr_items_read, size_t n_items_read_max, size_t n_items_read_min = (size_t)-1);
						CL3PUBF	off64_t	Left	(size_t sz_unit = 1) const;

						CL3PUBF	size_t	Write	(const byte* arr_items_write, size_t n_items_write_max, size_t n_items_write_min = (size_t)-1);
						CL3PUBF	off64_t	Space	(size_t sz_unit = 1) const;

						CL3PUBF	CLASS	TFDStream	(TFD fd);	//	TFDStream takes ownership of the file-descriptor fd
						CL3PUBF	CLASS	TFDStream	(const TFDStream&);
						CL3PUBF	CLASS	~TFDStream	();
				};
			}
		}
	}
}

#endif
