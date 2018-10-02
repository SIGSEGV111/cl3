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
#include <dirent.h>
#include <string.h>

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
			using namespace io::collection;
			using namespace io::collection::list;

			const TAccess TAccess::READ = { true, false, false };
			const TAccess TAccess::WRITE = { false, true, false };
			const TAccess TAccess::EXECUTE = { false, false, true };
			const TAccess TAccess::RW = { true, true, false };

			const TIOFlags TIOFlags::NONE = { false, false };
			const TIOFlags TIOFlags::SYNC = { true, false };
			const TIOFlags TIOFlags::NO_CACHE = { false, true };

			void	TMapping::Remap		(uoff_t new_index, usys_t new_count)
			{
				const uoff_t sz_file = file->Size();
				if(new_count == MAP_COUNT_FULLFILE)
					new_count = sz_file - new_index;

				const int access	= ((file->access & TAccess::READ) != 0 ? PROT_READ : 0)
									| ((file->access & TAccess::WRITE) != 0 ? PROT_WRITE : 0)
									| ((file->access & TAccess::EXECUTE) != 0 ? PROT_EXEC : 0);

				CL3_CLASS_ERROR(new_index + new_count > sz_file, collection::TIndexOutOfBoundsException, index, sz_file);

				if(this->arr_items != NULL && this->arr_items != (byte_t*)-1L)
					CL3_CLASS_SYSERR(::munmap(this->arr_items, this->n_items));

				if(new_count != 0)
					CL3_CLASS_SYSERR(this->arr_items = (byte_t*)::mmap(NULL, new_count, access, MAP_SHARED, file->fd, new_index));

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
			}

			CLASS	TMapping::~TMapping	()
			{
				if(this->arr_items != NULL && this->arr_items != (byte_t*)-1L)
				{
					::munmap(this->arr_items, this->n_items);
					Claim();
				}
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
					CL3_CLASS_SYSERR(status = ::pread(this->file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->pos));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read, 0);
					this->pos += status;
					n_items_read += status;
				}

				while(n_items_read < n_items_read_max)
				{
					CL3_CLASS_SYSERR(status = ::pread(this->file->fd, arr_items_read + n_items_read, n_items_read_max - n_items_read, this->pos));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					if(status == 0) break;
					this->pos += status;
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
					CL3_CLASS_SYSERR(status = ::pwrite(this->file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->pos));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					CL3_CLASS_ERROR(status == 0, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_written, 0);
					this->pos += status;
					n_items_written += status;
				}

				while(n_items_written < n_items_write_max)
				{
					CL3_CLASS_SYSERR(status = ::pwrite(this->file->fd, arr_items_write + n_items_written, n_items_write_max - n_items_written, this->pos));
					CL3_CLASS_LOGIC_ERROR(status < 0);
					if(status == 0) break;
					this->pos += status;
					n_items_written += status;
				}

				return n_items_written;
			}

			CLASS	TStream::TStream	(TFile* file) : file(file), pos(0)
			{
				//	nothing else to do
			}

			CLASS	TStream::TStream	(const TStream& other) : file(other.file), pos(other.pos)
			{
				//	nothing else to do
			}

			/**********************************************************************/

			usys_t	TFile::Size		() const
			{
				off_t sz;
				CL3_CLASS_SYSERR(sz = lseek(this->fd, 0, SEEK_END));
				return sz;
			}

			void	TFile::Size		(uoff_t new_count)
			{
				CL3_CLASS_SYSERR(::ftruncate(this->fd, new_count));
			}

			CLASS	TFile::TFile	(const text::string::TString& name, TAccess access, ECreate create, const TDirectoryBrowser& directory, TIOFlags ioflags) : access(access)
			{
				int flags = O_LARGEFILE | O_NOCTTY | O_CLOEXEC | (ioflags.disable_cache ? O_DIRECT : 0) | (ioflags.synchronous_io ? O_DSYNC : 0);
				const int mode = (access & TAccess::EXECUTE) ? 0777 : 0666;

				CL3_CLASS_ERROR( (access & TAccess::READ) == 0 && (access & TAccess::WRITE) == 0, TException, "file access mode is invalid");

				if( (access & TAccess::READ) != 0 && (access & TAccess::WRITE) != 0 )
					flags |= O_RDWR;
				else if( (access & TAccess::READ) != 0)
					flags |= O_RDONLY;
				else if( (access & TAccess::WRITE) != 0)
					flags |= O_WRONLY;

				switch(create)
				{
					case ECreate::ALWAYS:
						flags |= (O_CREAT | O_EXCL);
						break;
					case ECreate::NEVER:
						break;
					case ECreate::CAN:
						flags |= O_CREAT;
						break;
				}

				const TCString cstr(name, CODEC_CXX_CHAR);

				if(flags & O_EXCL)
				{
					for(;;)
					{
						this->fd = ::openat(directory.Handle(), cstr.Chars(), flags, mode);
						if(this->fd != -1)
							break;

						if(errno == EEXIST)
							CL3_CLASS_SYSERR(::unlinkat(directory.Handle(), cstr.Chars(), 0));
						else
							CL3_CLASS_SYSERR(-1);
					}
				}
				else
					CL3_CLASS_SYSERR(this->fd = ::openat(directory.Handle(), cstr.Chars(), flags, mode));
			}

			usys_t	TFile::Read		(u64_t pos, byte_t* arr_items_read, usys_t n_items_read_max)
			{
				ssize_t sz;
				CL3_CLASS_SYSERR(sz = pread(this->fd, arr_items_read, n_items_read_max, pos));
				return sz;
			}

			usys_t	TFile::Write	(u64_t pos, const byte_t* arr_items_write, usys_t n_items_write_max)
			{
				ssize_t sz;
				CL3_CLASS_SYSERR(sz = pwrite(this->fd, arr_items_write, n_items_write_max, pos));
				return sz;
			}

			CLASS	TFile::TFile	(TFile&& other) : fd(system::def::move(other.fd))
			{
			}

			CLASS	TFile::~TFile	()
			{
			}

			/**********************************************************************/

			#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)
				static const int DIRECTORY_BROWSER_FLAGS = O_RDONLY | O_CLOEXEC | O_NOCTTY | O_DIRECTORY;
			#else
				static const int DIRECTORY_BROWSER_FLAGS = O_RDONLY | O_CLOEXEC | O_NOCTTY;
			#endif

			fd_t	TDirectoryBrowser::Handle				() const
			{
				return this->fd;
			}

			void	TDirectoryBrowser::EnterDirectory		(const text::string::TString& name)
			{
				stream::fd::TFDStream new_fd;
				CL3_CLASS_SYSERR(new_fd = openat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), DIRECTORY_BROWSER_FLAGS));

				#if (CL3_OS_DERIVATIVE != CL3_OS_DERIVATIVE_POSIX_LINUX)
					struct ::stat st = {};
					CL3_CLASS_SYSERR(::fstat(new_fd, &st));
					if(!S_ISDIR(st.st_mode))
						CL3_CLASS_FAIL(TException, "the specified file-system object is not a directory");
				#endif

				this->fd = system::def::move(new_fd);
			}

			TList<TString> TDirectoryBrowser::Entries() const
			{
				TList<TString> list;
				this->EnumEntries(list);
				return system::def::move(list);
			}

			bool	TDirectoryBrowser::IsRoot				() const
			{
				return this->AbsolutePath() == "/";
			}

			TFile	TDirectoryBrowser::OpenFile				(const text::string::TString& name, TAccess access, ECreate create, TIOFlags flags)
			{
				return TFile(name, access, create, *this, flags);
			}

			void	TDirectoryBrowser::Delete				(const text::string::TString& name, const bool file_ok, const bool dir_ok, const bool ignore_not_exist, const bool recursive) const
			{
				int ret = -1;

				CL3_CLASS_ERROR(!dir_ok && !file_ok, TException, "at least one of file_ok or dir_ok must be true");

				if(recursive && this->GetFileInfo(name).type == EEntryType::DIRECTORY)
				{
					CL3_CLASS_ERROR(!dir_ok, TException, "requested to recursively delete, but did not allow to remove directories");
					TDirectoryBrowser(*this, name).EnumEntries( [ignore_not_exist, file_ok](const TDirectoryBrowser& dir, const text::string::TString& name) -> bool {
						dir.Delete(name, file_ok, true, ignore_not_exist, true);
						return true;
					});
				}

				if(file_ok)
					ret = unlinkat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), 0);

				if(!file_ok || (dir_ok && file_ok && ret == -1 && errno == EISDIR))
					ret = unlinkat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), AT_REMOVEDIR);

				if(ret == -1)
				{
					if(errno == ENOENT && ignore_not_exist)
						return;

					CL3_CLASS_FAIL(TSyscallException, errno);
				}
			}

			CLASS	TDirectoryBrowser::TDirectoryBrowser	()
			{
				CL3_CLASS_SYSERR(this->fd = open(".", DIRECTORY_BROWSER_FLAGS));
				CL3_CLASS_SYSERR(fstat(this->fd, &this->st_dir));
			}

			CLASS	TDirectoryBrowser::TDirectoryBrowser	(const text::string::TString& path)
			{
				CL3_CLASS_SYSERR(this->fd = open(TCString(path, CODEC_CXX_CHAR).Chars(), DIRECTORY_BROWSER_FLAGS, 0));
				CL3_CLASS_SYSERR(::fstat(this->fd, &this->st_dir));

				#if (CL3_OS_DERIVATIVE != CL3_OS_DERIVATIVE_POSIX_LINUX)
					if(!S_ISDIR(this->st_dir.st_mode))
						CL3_CLASS_FAIL(TException, "the specified file-system object is not a directory");
				#endif
			}

			CLASS	TDirectoryBrowser::TDirectoryBrowser	(const TDirectoryBrowser& dir_parent, const text::string::TString& path)
			{
				CL3_CLASS_SYSERR(this->fd = openat(dir_parent.fd, TCString(path, CODEC_CXX_CHAR).Chars(), DIRECTORY_BROWSER_FLAGS, 0));
				CL3_CLASS_SYSERR(::fstat(this->fd, &this->st_dir));

				#if (CL3_OS_DERIVATIVE != CL3_OS_DERIVATIVE_POSIX_LINUX)
					if(!S_ISDIR(this->st_dir.st_mode))
						CL3_CLASS_FAIL(TException, "the specified file-system object is not a directory");
				#endif
			}

			CLASS	TDirectoryBrowser::TDirectoryBrowser	(const TDirectoryBrowser& other)
			{
				CL3_CLASS_LOGIC_ERROR(other.fd < 0);
				CL3_CLASS_SYSERR(this->fd = openat(other.fd, ".", DIRECTORY_BROWSER_FLAGS));
				CL3_CLASS_SYSERR(::fstat(this->fd, &this->st_dir));
			}

			CLASS	TDirectoryBrowser::TDirectoryBrowser	(TDirectoryBrowser&& other) : fd(system::def::move(other.fd))
			{
			}

			CLASS	TDirectoryBrowser::~TDirectoryBrowser	()
			{
			}
		}
	}
}

#endif
