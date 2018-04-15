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

#include <cl3/core/system_memory.hpp>
#include <cl3/core/system_types_typeinfo.hpp>

#include <sys/mman.h>
#include <string.h>
#include <valgrind/valgrind.h>
#include <stdio.h>

using namespace cl3::system::types;

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			using namespace error;

			CLASS	TBadAllocException::TBadAllocException	(usys_t sz_bytes) : TException("memory allocation failed (size: %zd bytes)", sz_bytes), sz_bytes(sz_bytes) {}
			CLASS	TBadAllocException::TBadAllocException	(TBadAllocException&& bae) : TException(static_cast<TException&&>(bae)), sz_bytes(bae.sz_bytes) {}
			CLASS	TBadAllocException::~TBadAllocException	() {}

			/*******************************************************/

			struct	TDefaultAllocator : IDynamicAllocator
			{
				void*	Alloc	(usys_t sz_bytes)
				{
					sz_bytes += sizeof(usys_t);
					usys_t* p;

					if( (sz_bytes % def::CL3_OPT__PAGE_SIZE) != 0 )
						sz_bytes = ((sz_bytes / def::CL3_OPT__PAGE_SIZE)+1) * def::CL3_OPT__PAGE_SIZE;

					CL3_CLASS_SYSERR(p = (usys_t*)mmap(NULL, sz_bytes, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
					*p++ = sz_bytes;

					return p;
				}

				void	Free	(void* p_mem)
				{
					usys_t* p = (usys_t*)p_mem;
					p--;
					CL3_CLASS_SYSERR(munmap(p, *p));
				}

				void*	Realloc	(void* p_mem, usys_t sz_bytes_new, bool inplace)
				{
					usys_t* p = (usys_t*)p_mem;
					p--;
					sz_bytes_new += sizeof(usys_t);

					if( (sz_bytes_new % def::CL3_OPT__PAGE_SIZE) != 0 )
						sz_bytes_new = ((sz_bytes_new / def::CL3_OPT__PAGE_SIZE)+1) * def::CL3_OPT__PAGE_SIZE;

					if(*p == sz_bytes_new)
						return p+1;

					CL3_CLASS_SYSERR(p = (usys_t*)mremap(p, *p, sz_bytes_new, inplace ? 0 : MREMAP_MAYMOVE));
					*p++ = sz_bytes_new;
					return p;
				}

				usys_t	SizeOf	(void* p_mem) const
				{
					if(p_mem)
					{
						usys_t* p = (usys_t*)p_mem;
						p--;
						return *p;
					}
					else
						return 0;	//	LCOV_EXCL_LINE
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

			void*	TRestrictAllocator::Realloc	(void* p_mem, usys_t sz_bytes_new, bool inplace)
			{
				const usys_t sz_mb = allocator->SizeOf(p_mem);
				CL3_CLASS_LOGIC_ERROR(sz_mb > sz_current);
				CL3_CLASS_ERROR(sz_bytes_new > sz_mb && sz_current - sz_mb + sz_bytes_new > sz_limit, TBadAllocException, sz_bytes_new - sz_mb);
				p_mem = allocator->Realloc(p_mem, sz_bytes_new, inplace);
				sz_current -= sz_mb;
				sz_current += allocator->SizeOf(p_mem);
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
				CL3_CLASS_ERROR(allocator == NULL || allocator == (IDynamicAllocator*)this, TException, "TRestrictAllocator requires an upcall allocator, which cannot be NULL and also not the TRestrictAllocator itself");
			}

			CLASS	TRestrictAllocator::~TRestrictAllocator	()
			{
// 				CL3_CLASS_LOGIC_ERROR(BytesAllocated() != 0);	//	FIXME
			}


			/*******************************************************/

			static TDefaultAllocator allocator_default;
			CL3_CONTEXT_VARIABLE_IMPL(IDynamicAllocator*, allocator_generic, &allocator_default);
			CL3_CONTEXT_VARIABLE_IMPL(IDynamicAllocator*, allocator_exception, &allocator_default);

			/*******************************************************/

			void	Free	(void* p_mem)
			{
				if(p_mem)
				{
					IDynamicAllocator** const p_base = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base;
					owner->Free(p_base);
					VALGRIND_FREELIKE_BLOCK(p_base+1, 0);
				}
			}

			void*	Alloc	(usys_t n_items, const typeinfo::TRTTI* rtti)
			{
				CL3_NONCLASS_ERROR(n_items > (usys_t)-(sizeof(void*)*4), TException, "value in n_items is too big");
				const usys_t sz_bytes = rtti == NULL ? n_items : n_items * rtti->sz_bytes;

				if(sz_bytes)
				{
					IDynamicAllocator* const owner = allocator_generic();
					IDynamicAllocator** p_base;

					p_base = reinterpret_cast<IDynamicAllocator**>(owner->Alloc(sizeof(IDynamicAllocator*) + sz_bytes));

					VALGRIND_MALLOCLIKE_BLOCK(p_base+1, sz_bytes, 0, false);

					*p_base = owner;
					void* r = p_base+1;
					return r;
				}
				else
					return NULL;	//	LCOV_EXCL_LINE
			}

			void*	Realloc	(void* p_mem, usys_t n_items_new, const typeinfo::TRTTI* rtti, bool inplace)
			{
				CL3_NONCLASS_ERROR(n_items_new > (usys_t)-(sizeof(void*)*4), TException, "value in n_items_new is too big");
				const usys_t sz_bytes_new = (rtti == NULL ? n_items_new : n_items_new * rtti->sz_bytes) + sizeof(IDynamicAllocator*);

				if(p_mem)
				{
					IDynamicAllocator** const p_base_old = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base_old;

					if(n_items_new)
					{
// 						const usys_t sz_old = owner->SizeOf(p_mem);

						IDynamicAllocator** const p_base_new = reinterpret_cast<IDynamicAllocator**>(owner->Realloc(p_base_old, sz_bytes_new, inplace));

						if(p_base_old == p_base_new)
						{
// 							VALGRIND_RESIZEINPLACE_BLOCK(p_base_old, sz_old, sz_bytes_new, 0);
						}
						else
						{
							VALGRIND_FREELIKE_BLOCK(p_base_old+1, 0);
							VALGRIND_MALLOCLIKE_BLOCK(p_base_new+1, sz_bytes_new - sizeof(IDynamicAllocator), 0, false);
							*p_base_new = owner;
						}

						return p_base_new+1;
					}
					else
					{
						owner->Free(p_base_old);
						VALGRIND_FREELIKE_BLOCK(p_base_old+1, 0);
						return NULL;
					}
				}
				else
					return Alloc(n_items_new, rtti);
			}

			usys_t	SizeOf	(void* p_mem)
			{
				if(p_mem)
				{
					IDynamicAllocator** p_base = reinterpret_cast<IDynamicAllocator**>(p_mem)-1;
					IDynamicAllocator* const owner = *p_base;
					return owner->SizeOf(p_base) - sizeof(IDynamicAllocator*);
				}
				else
					return 0;
			}
		}
	}
}

extern "C"
{
	CL3_CXX_EXPORT_FUNC void* malloc(usys_t sz_bytes) throw()
	{
		void* v = NULL;
		try { v = cl3::system::memory::Alloc(sz_bytes, NULL); } catch(...) {}
		return v;
	}

	CL3_CXX_EXPORT_FUNC void* realloc(void* p, usys_t sz_bytes_new) throw()
	{
		void* v = NULL;
		try { v = cl3::system::memory::Realloc(p, sz_bytes_new, NULL, false); } catch(...) {}
		return v;
	}

	CL3_CXX_EXPORT_FUNC void free(void* p) throw()
	{
		try { cl3::system::memory::Free(p); } catch(...) {}
	}

	CL3_CXX_EXPORT_FUNC void *calloc(usys_t n_items, size_t sz_item) throw()
	{
		const usys_t sz_bytes = n_items * sz_item;
		void* v = malloc(sz_bytes);
		if(v != NULL)
			memset(v, 0, sz_bytes);
		return v;
	}
};

CL3_CXX_EXPORT_FUNC void* operator new(size_t sz)
{
	return cl3::system::memory::Alloc(sz, NULL);
}

CL3_CXX_EXPORT_FUNC void* operator new[](size_t sz)
{
	return cl3::system::memory::Alloc(sz, NULL);
}

CL3_CXX_EXPORT_FUNC void* operator new(size_t, void* p) throw()
{
	return p;
}

CL3_CXX_EXPORT_FUNC void* operator new(size_t sz, const std::nothrow_t&) throw()
{
	return cl3::system::memory::Alloc(sz, NULL);
}

CL3_CXX_EXPORT_FUNC void* operator new[](size_t sz, const std::nothrow_t&) throw()
{
	return cl3::system::memory::Alloc(sz, NULL);
}

CL3_CXX_EXPORT_FUNC void operator delete(void* p) throw()
{
	cl3::system::memory::Free(p);
}

CL3_CXX_EXPORT_FUNC void operator delete[](void* p) throw()
{
	cl3::system::memory::Free(p);
}
