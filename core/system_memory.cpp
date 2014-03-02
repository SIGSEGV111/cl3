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

#include "system_memory.hpp"
#include "error.hpp"
#include "system_types_typeinfo.hpp"
#include <malloc.h>

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			using namespace error;

			CLASS	TBadAllocException::TBadAllocException	(size_t sz_bytes) : TException("memory allocation failed (size: %zd bytes)", sz_bytes), sz_bytes(sz_bytes) {}
			CLASS	TBadAllocException::TBadAllocException	(const TBadAllocException& bae) : TException(bae), sz_bytes(bae.sz_bytes) {}
			CLASS	TBadAllocException::~TBadAllocException	() {}

			TDefaultAllocator TDefaultAllocator::instance;

			void*	TDefaultAllocator::Alloc	(size_t sz_bytes)
			{
				return safe_malloc(sz_bytes);
			}

			void	TDefaultAllocator::Free		(void* p_mem)
			{
				::free(p_mem);
			}

			void*	TDefaultAllocator::Realloc	(void* p_mem, size_t sz_bytes_new)
			{
				return safe_realloc(p_mem, sz_bytes_new);
			}

			void* safe_malloc(size_t sz_bytes)
			{
				//	malloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_ERROR(sz_bytes != 0 && (p = ::malloc(sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}

			void* safe_realloc(void* old, size_t sz_bytes)
			{
				//	realloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_ERROR(sz_bytes != 0 && (p = ::realloc(old, sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}
		}
	}
}
