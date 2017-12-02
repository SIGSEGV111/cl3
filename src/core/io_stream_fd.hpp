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
#include "system_task_synchronization.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	stream
		{
			namespace	fd
			{
				class CL3PUBT TWaitable : public system::task::synchronization::IWaitable
				{
					protected:
						fd_t fd;
						bool input;
						bool output;
						bool error;

					public:
						system::task::synchronization::waitinfo_t WaitInfo() const final override CL3_GETTER;

						CL3PUBF CLASS TWaitable();
						CL3PUBF CLASS TWaitable(fd_t fd, bool input, bool output, bool error);
				};

				//	generic buffered io-stream for interaction with operating systems io-structures (POSIX-fd, Windows HANDLE, etc.)
				class CL3PUBT TFDStream : public virtual IIn<byte_t>, public virtual IOut<byte_t>
				{
					protected:
						fd_t fd;

					public:
						CL3PUBF TWaitable OnOutputReady() const;
						CL3PUBF TWaitable OnInputReady() const;

						CL3PUBF	fd_t	FD		() const CL3_GETTER;
						CL3PUBF	void	FD		(fd_t) CL3_SETTER;

						CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1) final override;
						CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1) final override;

						CL3PUBF	usys_t	Remaining	() const  final override CL3_GETTER;
						CL3PUBF	void	Close		();
						CL3PUBF	fd_t	Claim		();

						CL3PUBF TFDStream& operator=(fd_t);
						CL3PUBF TFDStream& operator=(const TFDStream&);
						CL3PUBF TFDStream& operator=(TFDStream&&);
						inline operator fd_t() const CL3_GETTER { return this->fd; }

						CL3PUBF	CLASS	TFDStream	();
						CL3PUBF	CLASS	TFDStream	(fd_t fd);	//	TFDStream takes ownership of the file-descriptor fd
						CL3PUBF	CLASS	TFDStream	(const TFDStream&);
						CL3PUBF	CLASS	TFDStream	(TFDStream&&);
						CL3PUBF	CLASS	~TFDStream	();
				};
			}
		}
	}
}

#endif
