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
#include "error-ext.hpp"
#include "system_types_typeinfo.hpp"
#include <malloc.h>

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			using namespace error;

			CLASS	TDirtyAllocatorException::TDirtyAllocatorException	(usys_t sz_bytes) : TException("destroying non-empty allocator (allocated size: %zd byte_ts)", sz_bytes), sz_bytes(sz_bytes) {}
			CLASS	TDirtyAllocatorException::TDirtyAllocatorException	(const TDirtyAllocatorException& dae) : TException(dae), sz_bytes(dae.sz_bytes) {}
			CLASS	TDirtyAllocatorException::~TDirtyAllocatorException	() {}

			CLASS	TBadAllocException::TBadAllocException	(usys_t sz_bytes) : TException("memory allocation failed (size: %zd byte_ts)", sz_bytes), sz_bytes(sz_bytes) {}
			CLASS	TBadAllocException::TBadAllocException	(const TBadAllocException& bae) : TException(bae), sz_bytes(bae.sz_bytes) {}
			CLASS	TBadAllocException::~TBadAllocException	() {}

			/*******************************************************/

			void	cxx_free	(void* p_mem)
			{
				::free(p_mem);
			}

			void*	cxx_malloc	(usys_t sz_bytes)
			{
				//	malloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_NONCLASS_ERROR(sz_bytes != 0 && (p = ::malloc(sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}

			void*	cxx_realloc	(void* p_mem, usys_t sz_bytes)
			{
				//	realloc() will not be called when sz_bytes == 0, so p must be initilized to NULL
				void* p = NULL;
				CL3_NONCLASS_ERROR(sz_bytes != 0 && (p = ::realloc(p_mem, sz_bytes)) == NULL, TBadAllocException, sz_bytes);
				return p;
			}

			usys_t	cxx_sizeof	(void* p_mem)
			{
				//	FIXME: this is linux specific
				return ::malloc_usable_size(p_mem);
			}

			/*******************************************************/

			struct	TDefaultAllocator : IDynamicAllocator
			{
				void*	Alloc	(usys_t sz_bytes)
				{
					return cxx_malloc(sz_bytes);
				}

				void	Free	(void* p_mem)
				{
					cxx_free(p_mem);
				}

				void*	Realloc	(void* p_mem, usys_t sz_bytes_new)
				{
					return cxx_realloc(p_mem, sz_bytes_new);
				}

				usys_t	SizeOf	(void* p_mem) const
				{
					if(p_mem)
						return ::malloc_usable_size(p_mem);
					else
						return 0;
				}
			};

			/*******************************************************/

			void*	TRestrictAllocator::Alloc	(usys_t sz_bytes)
			{
				CL3_CLASS_ERROR(sz_current + sz_bytes > sz_limit, TBadAllocException, sz_bytes);
				void* p_mem = allocator->Alloc(sz_bytes);
				const usys_t sz_mb = allocator->SizeOf(p_mem);
				sz_current += sz_mb;
				return p_mem;
			}

			void	TRestrictAllocator::Free	(void* p_mem)
			{
				const usys_t sz_mb = allocator->SizeOf(p_mem);
				CL3_CLASS_LOGIC_ERROR(sz_mb > sz_current);
				allocator->Free(p_mem);
				sz_current -= sz_mb;
			}

			void*	TRestrictAllocator::Realloc	(void* p_mem, usys_t sz_bytes_new)
			{
				const usys_t sz_mb = allocator->SizeOf(p_mem);
				CL3_CLASS_LOGIC_ERROR(sz_mb > sz_current);
				p_mem = allocator->Realloc(p_mem, sz_bytes_new);
				sz_current -= sz_mb;
				sz_current += sz_bytes_new;
				return p_mem;
			}

			usys_t	TRestrictAllocator::SizeOf	(void* p_mem) const
			{
				return allocator->SizeOf(p_mem);
			}

			usys_t	TRestrictAllocator::BytesAllocated	() const
			{
				return sz_current;
			}

			usys_t	TRestrictAllocator::BytesLimit		() const
			{
				return sz_limit;
			}

			CLASS	TRestrictAllocator::TRestrictAllocator	(IDynamicAllocator* allocator, usys_t sz_limit) : allocator(allocator), sz_limit(sz_limit), sz_current(0)
			{
				CL3_CLASS_ARGUMENT_ERROR(allocator == NULL || allocator == (IDynamicAllocator*)this, allocator, "TRestrictAllocator requires an upcall allocator, which cannot be NULL and also not the TRestrictAllocator itself");
			}

			CLASS	TRestrictAllocator::~TRestrictAllocator	()
			{
				//CL3_CLASS_ERROR(BytesAllocated() != 0, TDirtyAllocatorException, sz_current);
				//	FIXME: print diagnostic
			}


			/*******************************************************/

			CL3_PARAMETER_STACK_IMPL(IDynamicAllocator*, allocator, NULL);

			static TDefaultAllocator default_allocator;
			CL3_PARAMETER_STACK_PUSH(allocator, &default_allocator);
			IDynamicAllocator* exception_allocator = &default_allocator;	//	FIXME: assign a dedicated allocator which keeps a reserve

			/*******************************************************/

			void	Free	(void* p_mem)
			{
				if(p_mem)
				{
					IDynamicAllocator** const p_base = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base;

					if(owner)
						owner->Free(p_base);
					else
						cxx_free(p_base);
				}
			}

			void*	Alloc	(usys_t n_items, const typeinfo::TRTTI* rtti)
			{
				const usys_t sz_bytes = rtti == NULL ? n_items : n_items * rtti->sz_bytes;

				if(sz_bytes)
				{
					IDynamicAllocator* const owner = CL3_PARAMETER_STACK_VALUE(allocator);
					IDynamicAllocator** p_base;

					if(owner)
						p_base = reinterpret_cast<IDynamicAllocator**>(owner->Alloc(sizeof(IDynamicAllocator*) + sz_bytes));
					else
						p_base = reinterpret_cast<IDynamicAllocator**>(cxx_malloc(sizeof(IDynamicAllocator*) + sz_bytes));

					*p_base = owner;
					return p_base+1;
				}
				else
					return NULL;
			}

			void*	Realloc	(void* p_mem, usys_t n_items_new, const typeinfo::TRTTI* rtti)
			{
				const usys_t sz_bytes_new = rtti == NULL ? n_items_new : n_items_new * rtti->sz_bytes;

				if(p_mem)
				{
					IDynamicAllocator** p_base = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base;

					if(sz_bytes_new)
					{
						if(owner)
							p_base = reinterpret_cast<IDynamicAllocator**>(owner->Realloc(p_base, sizeof(IDynamicAllocator*) + sz_bytes_new));
						else
							p_base = reinterpret_cast<IDynamicAllocator**>(cxx_realloc(p_base, sizeof(IDynamicAllocator*) + sz_bytes_new));

						return p_base+1;
					}
					else
					{
						owner->Free(p_base);
						return NULL;
					}
				}
				else
					return Alloc(sz_bytes_new, rtti);
			}

			usys_t	SizeOf	(void* p_mem)
			{
				if(p_mem)
				{
					IDynamicAllocator** p_base = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base;
					if(owner)
						return owner->SizeOf(p_base) - sizeof(IDynamicAllocator*);
					else
						return cxx_sizeof(p_base);
				}
				else
					return 0;
			}
		}
	}
}
