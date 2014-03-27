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
#include "error-base.hpp"
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

			void	cxx_free	(void* ptr)
			{
				::free(ptr);
			}

			void*	cxx_malloc	(size_t sz_bytes)
			{
				//	malloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_NONCLASS_ERROR(sz_bytes != 0 && (p = ::malloc(sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}

			void*	cxx_realloc	(void* old, size_t sz_bytes)
			{
				//	realloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_NONCLASS_ERROR(sz_bytes != 0 && (p = ::realloc(old, sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}

			struct	TDefaultAllocator : IDynamicAllocator
			{
				void*	Alloc	(size_t sz_bytes)
				{
					IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(cxx_malloc(sizeof(IDynamicAllocator*) + sz_bytes));
					*p = this;
					return p+1;
				}

				void	Free	(void* p_mem)
				{
					IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(p_mem);
					p--;
					if(*p == this)
						cxx_free(p);
					else
						(*p)->Free(p_mem);
				}

				void*	Realloc	(void* p_mem, size_t sz_bytes_new)
				{
					if(p_mem)
					{
						IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(p_mem);
						p--;
						if(*p == this)
						{
							p = reinterpret_cast<IDynamicAllocator**>(cxx_realloc(p, sizeof(IDynamicAllocator*) + sz_bytes_new));
							return p+1;
						}
						else
							return (*p)->Realloc(p_mem, sz_bytes_new);
					}
					else
						return Alloc(sz_bytes_new);
				}
			};

			static TDefaultAllocator default_allocator;
			CL3_PARAMETER_STACK_IMPL(IDynamicAllocator*, allocator, &default_allocator);
		}
	}
}
