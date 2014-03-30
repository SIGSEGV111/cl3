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
#include "system_types.hpp"
#include "io_stream.hpp"

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
				class	TFDStream : virtual IIn<byte_t>, virtual IOut<byte_t>
				{
					protected:
						byte_t* p_buffer;
						usys_t sz_buffer;
						TFD fd;

					public:
						CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(IOut<byte_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);

						CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(IIn<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						CL3PUBF	CLASS	TFDStream	(TFD fd);	//	TFDStream takes ownership of the file-descriptor fd
						CL3PUBF	CLASS	TFDStream	(const TFDStream&);
						CL3PUBF	CLASS	~TFDStream	();
				};
			}
		}
	}
}

#endif
