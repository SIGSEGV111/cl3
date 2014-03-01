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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "util.hpp"
#include "system_memory.hpp"
#include "error.hpp"

namespace	cl3
{
	namespace	util
	{
		using namespace system::memory;

		TUniquePtr<char[],false> mprintf(const char* format, ...)
		{
			va_list list;
			va_start(list, format);
			int sz_need = vsnprintf(NULL, 0, format, list);
			va_end(list);

			CL3_ERROR(sz_need < 0, error::TCoreException, "::vsnprintf() failed (format-string: \"%s\")", format);

			TUniquePtr<char[],false> buffer((char*)system::memory::safe_malloc(sz_need + 1));	// one byte extra for '\0'

			va_start(list, format);
			/*const int sz_used =*/ vsnprintf(buffer.Array(), sz_need + 1, format, list);
			va_end(list);

			//CL3_IFDBG(CL3_ERROR(sz_used != sz_need || sz_used < 0, error::TCoreException, "your ::vsnprintf() implementation is broken, *OR* the memory that was referenced by the format string changed, which is an indicator for unguarded multi-threaded access to the same memory in your application - the output of the mprintf() function will be of limited use."));

			return buffer;
		}

		TUniquePtr<char[],false> mkstrcpy(const char* str)
		{
			size_t l = ::strlen(str) + 1;
			TUniquePtr<char[],false> cpy((char*)system::memory::safe_malloc(l));
			::memcpy(cpy.Array(), str, l);
			return cpy;
		}
	}
}
