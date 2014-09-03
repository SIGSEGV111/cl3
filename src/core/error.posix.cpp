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

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include <string.h>
#include <errno.h>
#include "util.hpp"
#include "error.hpp"

namespace	cl3
{
	namespace	error
	{
		using namespace util;

		//	not all versions of strerror_r return the buffer (POSIX and GNU define different return values)
		static inline char* strerror_helper(char* buffer, int err_no)
		{
			char* msg = ::strerror_r(err_no, buffer, 256);
			if(msg < buffer || msg > buffer + 256)	//	check if msg is within buffer, or if a independet char* was returned
			{
				::strncpy(buffer, msg, 255);
				buffer[255] = '\0';
			}
			return buffer;
		}

		CLASS	TSyscallException::TSyscallException	() : TException("syscall failed (errno: %d, message: \"%s\")", errno, strerror_helper(message_buffer, errno)), err_no(errno)
		{
		}

		CLASS	TSyscallException::TSyscallException	(int err_no) : TException("syscall failed (errno: %d, message: \"%s\")", err_no, strerror_helper(message_buffer, err_no)), err_no(err_no)
		{
		}

		CLASS	TSyscallException::TSyscallException	(const TSyscallException& other) : TException(other), err_no(other.err_no)
		{
			memcpy(message_buffer, other.message_buffer, sizeof(message_buffer));
		}

		CLASS	TSyscallException::~TSyscallException	()
		{
		}
	}
}

#endif
