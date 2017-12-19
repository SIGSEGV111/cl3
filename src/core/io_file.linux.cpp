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

#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)

#include "io_file.hpp"
#include "system_types_typeinfo.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

namespace	cl3
{
	namespace	io
	{
		namespace	file
		{
			using namespace text::encoding;
			using namespace text::string;
			using namespace system::memory;
			using namespace system::types::typeinfo;
			using namespace collection::list;
			using namespace error;

			CLASS	TFile::TFile	() : access(FILE_ACCESS_READ | FILE_ACCESS_WRITE /*| (executeable ? FILE_ACCESS_EXECUTE : 0)*/)
			{
				const int mode = /*(access & FILE_ACCESS_EXECUTE) ? 0777 :*/ 0666;
				const int flags = O_LARGEFILE | O_NOCTTY | O_CLOEXEC | O_TMPFILE | O_RDWR;

				this->fd = open(".", flags, mode);
				if(this->fd == -1)
				{
					if(errno == EOPNOTSUPP || errno == EINVAL || errno == EISDIR)
					{
						perror("O_TMPFILE not supported by kernel/filesystem, falling back to workaround");
						char tmpfile_name[] = ".cl3tmp-XXXXXX";
						CL3_CLASS_SYSERR(this->fd = mkostemp(tmpfile_name, O_CLOEXEC|O_NOCTTY|O_LARGEFILE|O_RDWR));
						CL3_CLASS_SYSERR(::unlink(tmpfile_name));
					}
					else
					{
						perror("open call failed with unhandled error code");
						CL3_CLASS_FAIL(TSyscallException, errno);
					}
				}
			}

			struct linux_dirent64
			{
				ino64_t        d_ino;    /* 64-bit inode number */
				off64_t        d_off;    /* 64-bit offset to next structure */
				unsigned short d_reclen; /* Size of this dirent */
				unsigned char  d_type;   /* File type */
				char           d_name[]; /* Filename (null-terminated) */
			};

			inline static int getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count)
			{
				return ::syscall(SYS_getdents64, fd, dirp, count);
			}

			inline static bool TestStdDirs(const char* buffer)
			{
				// tests for "." and ".."
				// buffer is assumed to be at least 3 bytes long

				if(buffer[0] == '.' && buffer[1] == 0)
					return true;

				if(buffer[0] == '.' && buffer[1] == '.' && buffer[2] == 0)
					return true;

				return false;
			}

			#ifdef CL3_DEBUG
				static const usys_t GETDENTS_BUFFER_SIZE = 8;
			#else
				static const usys_t GETDENTS_BUFFER_SIZE = 4000;
			#endif

			usys_t	TDirectoryBrowser::EnumEntries	(collection::IDynamicCollection<text::string::TString>& collection) const
			{
				usys_t n = 0;
				int n_bytes_used;
				TList<byte_t> buffer;

				CL3_CLASS_SYSERR(lseek(this->fd, 0, SEEK_SET));
				buffer.Count(GETDENTS_BUFFER_SIZE);

				for(;;)
				{
					CL3_CLASS_ERROR( (n_bytes_used = getdents64(this->fd, (linux_dirent64*)buffer.ItemPtr(0), buffer.Count() - 2)) == -1 && errno != EINVAL, TSyscallException, errno );

					if(n_bytes_used == -1 && errno == EINVAL)
					{
						// buffer too small
						buffer.Count(buffer.Count() * 2);
						continue;
					}

					if(n_bytes_used == 0)
						break;

					const byte_t* const e = buffer.ItemPtr(0) + n_bytes_used;
					const byte_t* p = buffer.ItemPtr(0);
					for(;p < e;)
					{
						linux_dirent64* d = (linux_dirent64*)p;
						p += d->d_reclen;

						if(!TestStdDirs(d->d_name))
						{
							collection.Add(TString(d->d_name));
							n++;
						}
					}
				}

				return n;
			}

			text::string::TString
					TDirectoryBrowser::AbsolutePath			() const
					{
						char name_link[32];
						struct ::stat st;
						snprintf(name_link, 32, "/proc/self/fd/%d", this->fd.FD());
						CL3_CLASS_SYSERR(::lstat(name_link, &st));
						auto name_target = MakeUniquePtr<UPTR_ALLOC>((char*)Alloc(st.st_size + 1, &TCTTI<char>::rtti));
						ssize_t r;
						CL3_CLASS_SYSERR(r = ::readlink(name_link, name_target.Object(), st.st_size));
						CL3_CLASS_LOGIC_ERROR(r > st.st_size);
						name_target.Object()[r] = '\0';
						return TString(name_target.Object());
					}
		}
	}
}

#endif
