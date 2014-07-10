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
#include "system_types_typeinfo.hpp"
#include "error.hpp"

namespace	cl3
{
	namespace	util
	{
		using namespace system::memory;
		using namespace system::types;

		TUniquePtr<char,UPTR_MALLOC> mkstrcpy(const char* str, system::memory::IDynamicAllocator* local_allocator)
		{
			if(local_allocator == NULL)
				local_allocator = CL3_PARAMETER_STACK_VALUE(allocator);
			CL3_PARAMETER_STACK_PUSH(allocator, local_allocator);

			usys_t l = ::strlen(str) + 1;
			auto cpy = MakeUniquePtr<UPTR_MALLOC>((char*)Alloc(l, NULL));
			::memcpy(cpy.Object(), str, l);
			return cpy;
		}

		void	Hexdump	(const void* p_mem, usys_t sz_mem)
		{
			for(usys_t i = 0; i < sz_mem; i++)
			{
				if(i && (i % 16) == 0 && i + 1 < sz_mem)
					putchar('\n');
				printf("%02hhx ", ((unsigned char*)p_mem)[i]);
			}
			putchar('\n');
		}
	}
}
