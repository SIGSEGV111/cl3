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
#include "error-base.hpp"

namespace	cl3
{
	namespace	util
	{
		using namespace system::memory;

// 		TUniquePtr<char,UPTR_MALLOC> mprintf(const char* format, ...)
// 		{
// 			va_list list;
// 			va_start(list, format);
// 			int sz_need = vsnprintf(NULL, 0, format, list);
// 			va_end(list);
//
// 			CL3_NONCLASS_ERROR(sz_need < 0, error::TException, "::vsnprintf() failed (format-string: \"%s\")", format);
//
// 			TUniquePtr<char,UPTR_MALLOC> buffer(MakeUniquePtr<UPTR_MALLOC>((char*)system::memory::safe_malloc(sz_need + 1)));	// one byte_t extra for '\0'
//
// 			va_start(list, format);
// 			/*const int sz_used =*/ vsnprintf(&buffer.Object(), sz_need + 1, format, list);
// 			va_end(list);
//
// 			//CL3_IFDBG(CL3_CLASS_ERROR(sz_used != sz_need || sz_used < 0, error::TException, "your ::vsnprintf() implementation is broken, *OR* the memory that was referenced by the format string changed, which is an indicator for unguarded multi-threaded access to the same memory in your application - the output of the mprintf() function will be of limited use."));
//
// 			return buffer;
// 		}

		TUniquePtr<char,UPTR_MALLOC> mkstrcpy(const char* str)
		{
			usys_t l = ::strlen(str) + 1;
			TUniquePtr<char,UPTR_MALLOC> cpy(MakeUniquePtr<UPTR_MALLOC>((char*)Alloc(l)));
			::memcpy(&cpy.Object(), str, l);
			return cpy;
		}
	}
}
