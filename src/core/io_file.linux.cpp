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
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

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
			using namespace error;

			CLASS	TFile::TFile	(bool executeable) : fd(-1), access(FILE_ACCESS_READ | FILE_ACCESS_WRITE | (executeable ? FILE_ACCESS_EXECUTE : 0))
			{
				const int mode = (access & FILE_ACCESS_EXECUTE) ? 0777 : 0666;
				const int flags = O_LARGEFILE | O_NOCTTY | O_CLOEXEC | O_TMPFILE | O_RDWR;
				const TDirectoryBrowser& cwd = TDirectoryBrowser::ThreadCurrentWorkingDirectory();

				this->fd = open(TCString(cwd.AbsolutePath(), CODEC_CXX_CHAR).Chars(), flags, mode);
				if(this->fd == -1)
				{
					if(errno == EOPNOTSUPP || errno == EINVAL)
					{
						const TCString tmpfile_name(cwd.AbsolutePath() + L"/.cl3tmp-XXXXXX", CODEC_CXX_CHAR);
						CL3_CLASS_SYSERR(this->fd = mkostemp((char*)tmpfile_name.Chars(), O_CLOEXEC|O_NOCTTY|O_LARGEFILE|O_RDWR));
						CL3_CLASS_SYSERR(::unlink(tmpfile_name.Chars()));
					}
					else
						CL3_CLASS_FAIL(TSyscallException, errno);
				}
			}

			text::string::TString
					TDirectoryBrowser::AbsolutePath			() const
					{
						char name_link[32];
						struct ::stat st;
						snprintf(name_link, 32, "/proc/self/fd/%d", this->fd);
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
