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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "io_file.hpp"
#include "io_collection_list.hpp"

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
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

			void	TMapping::Remap		(uoff_t new_index, usys_t new_count)
			{
				const uoff_t sz_file = file->Count();
				if(new_count == MAP_COUNT_FULLFILE)
					new_count = sz_file - new_index;

				const int access	= ((file->access & FILE_ACCESS_READ) != 0 ? PROT_READ : 0)
									| ((file->access & FILE_ACCESS_WRITE) != 0 ? PROT_WRITE : 0)
									| ((file->access & FILE_ACCESS_EXECUTE) != 0 ? PROT_EXEC : 0);

				CL3_CLASS_ERROR(new_index + new_count > sz_file, collection::TIndexOutOfBoundsException, index, sz_file);

				if(this->arr_items != NULL && this->arr_items != (byte_t*)-1L)
					CL3_CLASS_SYSERR(::munmap(this->arr_items, this->n_items));

				if(new_count != 0)
					CL3_CLASS_SYSERR(this->arr_items = (byte_t*)::mmap(NULL, new_count, access, MAP_SHARED|MAP_NONBLOCK, file->fd, new_index));

				this->n_items = new_count;
				this->index = new_index;
			}

			void	TMapping::Count		(usys_t new_count)
			{
				this->Remap(this->index, new_count);
			}

			void	TMapping::Index		(uoff_t new_index)
			{
				this->Remap(new_index, this->n_items);
			}

			uoff_t	TMapping::Index		() const
			{
				return index;
			}

			CLASS	TMapping::TMapping	(TFile* file, uoff_t index, usys_t count) : TArray<const byte_t>(NULL,0), TArray<byte_t>(NULL,0), file(file), index(index)
			{
				this->Remap(index, count);
			}

			CLASS	TMapping::TMapping	(TMapping&& other) : TArray<const byte_t>(NULL,0), TArray<byte_t>(NULL,0), file(other.file), index(other.index)
			{
				this->arr_items = other.arr_items;
				this->n_items = other.n_items;
				other.arr_items = NULL;
				other.n_items = 0;
				other.index = 0;
				other.file = NULL;
			}

			CLASS	TMapping::~TMapping	()
			{
				if(this->arr_items != NULL && this->arr_items != (byte_t*)-1L)
					CL3_CLASS_SYSERR(::munmap(this->arr_items, this->n_items));
			}

			/**********************************************************************/

			usys_t	TStream::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				if(n_items_read_min == (usys_t)-1)
					n_items_read_min = n_items_read_max;

				usys_t n_items_read = 0;
				ssize_t status;

				while(n_items_read < n_items_read_min)
				{
					CL3_CLASS_SYSERR(status = ::pread(this->map.file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read, 0);
					this->index += status;
					n_items_read += status;
				}

				while(n_items_read < n_items_read_max)
				{
					CL3_CLASS_SYSERR(status = ::pread(this->map.file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->index));
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
					CL3_CLASS_SYSERR(status = ::pwrite(this->map.file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_written, 0);
					this->index += status;
					n_items_written += status;
				}

				while(n_items_written < n_items_write_max)
				{
					CL3_CLASS_SYSERR(status = ::pwrite(this->map.file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->index));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					if(status == 0) break;
					this->index += status;
					n_items_written += status;
				}

				return n_items_written;
			}

			const byte_t&	TStream::Item	() const
			{
				CL3_CLASS_ERROR(!IsValid(), TException, "no current item");
				return this->map[this->index];
			}

			byte_t&			TStream::Item	()
			{
				CL3_CLASS_ERROR(!IsValid(), TException, "no current item");
				return this->map[this->index];
			}

			bool	TStream::FindNext	(const collection::IMatcher<byte_t>& matcher)
			{
				CL3_CLASS_ERROR(!IsValid(), TException, "no current item");
				collection::list::TList<usys_t> matches;
				matcher.Match(collection::DIRECTION_FORWARD, this->map.ItemPtr(this->index), this->map.Count() - this->index, matches, 1);
				if(matches.Count())
				{
					this->index = matches[0];
					return true;
				}
				else
					return false;
			}

			bool	TStream::FindPrev	(const collection::IMatcher<byte_t>& matcher)
			{
				CL3_CLASS_ERROR(!IsValid(), TException, "no current item");
				collection::list::TList<usys_t> matches;
				matcher.Match(collection::DIRECTION_BACKWARD, this->map.ItemPtr(0), this->index, matches, 1);
				if(matches.Count())
				{
					this->index = matches[0];
					return true;
				}
				else
					return false;
			}

			bool	TStream::IsValid	() const
			{
				if(this->index == INDEX_HEAD || this->index == INDEX_TAIL)
					return false;
				if(this->index >= this->map.Index() + this->map.Count())
					const_cast<TStream*>(this)->map.Remap(0, MAP_COUNT_FULLFILE);
				return this->index < this->map.Index() + this->map.Count();
			}

			void	TStream::MoveHead	()
			{
				this->index = INDEX_HEAD;
			}

			void	TStream::MoveTail	()
			{
				this->index = INDEX_TAIL;
			}

			bool	TStream::MoveFirst	()
			{
				const_cast<TStream*>(this)->map.Remap(0, MAP_COUNT_FULLFILE);

				if(this->map.Count())
				{
					this->index = 0;
					return true;
				}
				else
				{
					this->index = INDEX_HEAD;
					return false;
				}
			}

			bool	TStream::MoveLast	()
			{
				const_cast<TStream*>(this)->map.Remap(0, MAP_COUNT_FULLFILE);

				if(this->map.Count())
				{
					this->index = this->map.Count() - 1;
					return true;
				}
				else
				{
					this->index = INDEX_TAIL;
					return false;
				}
			}

			bool	TStream::MoveNext	()
			{
				switch(this->index)
				{
					case INDEX_HEAD:
						return MoveFirst();
					case INDEX_TAIL:
						return false;
					default:
						if(this->index + 1 < this->map.Count())
						{
							this->index++;
							return true;
						}
						else
						{
							const_cast<TStream*>(this)->map.Remap(0, MAP_COUNT_FULLFILE);

							if(this->index + 1 < this->map.Count())
							{
								this->index++;
								return true;
							}
							else
							{
								this->index = INDEX_TAIL;
								return false;
							}
						}
				}
			}

			bool	TStream::MovePrev	()
			{
				switch(this->index)
				{
					case INDEX_HEAD:
						return false;
					case INDEX_TAIL:
						return MoveLast();
					default:
						if(this->index > 0)
						{
							if(this->index - 1 < this->map.Count())
							{
								this->index--;
								return true;
							}
							else
							{
								const_cast<TStream*>(this)->map.Remap(0, MAP_COUNT_FULLFILE);

								if(this->index - 1 < this->map.Count())
								{
									this->index--;
									return true;
								}
							}
						}
						this->index = INDEX_HEAD;
						return false;
				}
			}

			void	TStream::Index		(uoff_t new_index)
			{
				this->index = new_index;
			}

			uoff_t	TStream::Index		() const
			{
				return index;
			}

			CLASS	TStream::TStream	(TFile* file) : index(0), map(file)
			{
				//	nothing else to do
			}

			CLASS	TStream::TStream	(const TStream& other) : index(other.index), map(other.map.file)
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

			usys_t	TFile::Count	() const
			{
				struct ::stat s;
				CL3_CLASS_SYSERR(::fstat(this->fd, &s));
				return (usys_t)s.st_size;

				/*
				off64_t off_end, off_current;
				CL3_CLASS_SYSERR(off_current = lseek64(this->fd, 0, SEEK_CUR));
				CL3_CLASS_SYSERR(off_end = lseek64(this->fd, 0, SEEK_END));
				CL3_CLASS_SYSERR(lseek64(this->fd, off_current, SEEK_SET));
				*/
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

			CLASS	TFile::TFile	(const text::string::TString& name, int access, ECreate create) : fd(-1), access(access)
			{
				int flags = O_LARGEFILE|O_NOCTTY;
				int mode = (access & FILE_ACCESS_EXECUTE) ? 0777 : 0666;

				CL3_CLASS_ERROR( (access & FILE_ACCESS_READ) == 0 && (access & FILE_ACCESS_WRITE) == 0, TException, "file access mode is invalid");

				if( (access & FILE_ACCESS_READ) != 0 && (access & FILE_ACCESS_WRITE) != 0 )
					flags |= O_RDWR;
				else if( (access & FILE_ACCESS_READ) != 0)
					flags |= O_RDONLY;
				else if( (access & FILE_ACCESS_WRITE) != 0)
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
