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

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "io_file.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

namespace	cl3
{
	namespace	io
	{
		namespace	file
		{
			using namespace text::encoding;
			using namespace text::string;
			using namespace error;
			using namespace stream;

			usys_t	TStream::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				if(n_items_read_min == (usys_t)-1)
					n_items_read_min = n_items_read_max;

				usys_t n_items_read = 0;
				ssize_t status;

				while(n_items_read < n_items_read_min)
				{
					CL3_CLASS_SYSERR(status = ::pread(this->file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read, 0);
					this->index += status;
					n_items_read += status;
				}

				while(n_items_read < n_items_read_max)
				{
					CL3_CLASS_SYSERR(status = ::pread(this->file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					if(status == 0) break;
					this->index += status;
					n_items_read += status;
				}

				return n_items_read;
			}

			usys_t	TStream::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				if(n_items_write_min == (usys_t)-1)
					n_items_write_min = n_items_write_max;

				usys_t n_items_written = 0;
				ssize_t status;

				while(n_items_written < n_items_write_min)
				{
					CL3_CLASS_SYSERR(status = ::pwrite(this->file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_written, 0);
					this->index += status;
					n_items_written += status;
				}

				while(n_items_written < n_items_write_max)
				{
					CL3_CLASS_SYSERR(status = ::pwrite(this->file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					if(status == 0) break;
					this->index += status;
					n_items_written += status;
				}

				return n_items_written;
			}

			const byte_t&	TStream::Item	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			byte_t&			TStream::Item	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::FindNext	(const collection::IMatcher<byte_t>& matcher)
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::FindPrev	(const collection::IMatcher<byte_t>& matcher)
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::IsValid	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	TStream::MoveHead	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	TStream::MoveTail	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::MoveFirst	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::MoveLast	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::MoveNext	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool	TStream::MovePrev	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	TStream::Index		(uoff_t new_index)
			{
				this->index = new_index;
			}

			uoff_t	TStream::Index		() const
			{
				return index;
			}

			CLASS	TStream::TStream	(TFile* file) : file(file), index(0)
			{
				//	nothing else to do
			}

			CLASS	TStream::TStream	(const TStream& other) : file(other.file), index(other.index)
			{
				//	nothing else to do
			}

			/**********************************************************************/

			system::memory::TUniquePtr<collection::IStaticIterator<const byte_t> >	TFile::CreateStaticIterator	() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			system::memory::TUniquePtr<collection::IStaticIterator<byte_t> >		TFile::CreateStaticIterator	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t	TFile::Count		() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	TFile::Count	(uoff_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TString	TFile::Name		() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TFile::TFile	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TFile::TFile	(const text::string::TString& name, int access, ECreate create)
			{
				int flags = O_LARGEFILE|O_NOCTTY;
				int mode = (access & FILE_ACCESS_EXECUTE) ? 0777 : 0666;

				CL3_CLASS_ERROR( !((access & FILE_ACCESS_READ) || (access & FILE_ACCESS_WRITE)), TException, "file access mode is invalid");

				if( (access & FILE_ACCESS_READ) && (access & FILE_ACCESS_WRITE) )
					flags |= O_RDWR;
				else if(access & FILE_ACCESS_READ)
					flags |= O_RDONLY;
				else
					flags |= O_WRONLY;

				switch(create)
				{
					case FILE_CREATE_ALWAYS:
						flags |= (O_CREAT | O_EXCL);
						break;
					case FILE_CREATE_NEVER:
						break;
					case FILE_CREATE_CAN:
						flags |= O_CREAT;
						break;
				}

				const TCString cstr(name, CODEC_CXX_CHAR);

				if(flags & O_EXCL)
				{
					for(;;)
					{
						this->fd = ::open(cstr.Chars(), flags, mode);
						if(this->fd != -1)
							break;

						if(errno == EEXIST)
							CL3_CLASS_SYSERR(::unlink(cstr.Chars()));
						else
							CL3_CLASS_SYSERR(-1);
					}
				}
				else
					CL3_CLASS_SYSERR(this->fd = ::open(cstr.Chars(), flags, mode));
			}

			CLASS	TFile::TFile	(TFile&& other) : fd(other.fd)
			{
				other.fd = -1;
			}

			CLASS	TFile::~TFile	()
			{
				if(this->fd != -1)
					CL3_CLASS_SYSERR(::close(this->fd));
			}
		}
	}
}

#endif
