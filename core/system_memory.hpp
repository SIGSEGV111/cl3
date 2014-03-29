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

#ifndef	_include_cl3_core_system_memory_hpp_
#define	_include_cl3_core_system_memory_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "error-base.hpp"
#include "context.hpp"

extern "C" void free(void*) throw();

namespace	cl3
{
	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				struct	TRTTI;
			}
		}

		namespace	memory
		{
			using namespace types;

			enum	EUnqiuePtrType
			{
				UPTR_OBJECT,
				UPTR_ARRAY,
				UPTR_MALLOC
			};

			template<class T, EUnqiuePtrType type = UPTR_OBJECT>
			class	TUniquePtr
			{
				private:
					CLASS		TUniquePtr	(T* object) throw() : object(object) {}
				protected:
					mutable T* object;

					void	Release	()
					{
						switch(type)
						{
							case UPTR_OBJECT:
								delete object;
								break;
							case UPTR_ARRAY:
								delete[] object;
								break;
							case UPTR_MALLOC:
								::free(object);
								break;
						};
					}

				public:
					TUniquePtr&	operator=	(const TUniquePtr& other)
					{
						Release();
						this->object = other.object;
						other.object = NULL;
						return *this;
					}

					SETTER	void	Object		(T* new_object)
					{
						Release();
						object = new_object;
					}

					GETTER	T&		Object		() { return *object; }
					GETTER	T*		Claim		() { T* tmp = object; object = NULL; return tmp; }
					void			Reset		() { object = NULL; }	//	does *NOT* release any memory - just removes control of the object from this class

					GETTER	const T*	Object		() const { return object; }

					CLASS	TUniquePtr	() throw() : object(NULL) {}
					CLASS	TUniquePtr	(const TUniquePtr& other) throw() : object(other.object) { other.object = NULL; }
					CLASS	~TUniquePtr	() { Release(); }
			};

			template<class T> GETTER static TUniquePtr<T, UPTR_OBJECT> MakeUniquePtr(T* object) { TUniquePtr<T, UPTR_OBJECT> uptr; uptr.Object(object); return uptr; }
			template<EUnqiuePtrType type, class T> GETTER static TUniquePtr<T, type> MakeUniquePtr(T* object) { TUniquePtr<T, type> uptr; uptr.Object(object); return uptr; }

			class	CL3PUBT	TBadAllocException : public error::TException
			{
				protected:
					usys_t sz_bytes;

				public:
					CL3PUBF	CLASS	TBadAllocException	(usys_t sz_bytes);
					CL3PUBF	CLASS	TBadAllocException	(const TBadAllocException&);
					CL3PUBF	virtual	~TBadAllocException	();
			};

			class	CL3PUBT	TDirtyAllocatorException : public error::TException
			{
				protected:
					usys_t sz_bytes;

				public:
					CL3PUBF	CLASS	TDirtyAllocatorException	(usys_t sz_bytes);
					CL3PUBF	CLASS	TDirtyAllocatorException	(const TDirtyAllocatorException&);
					CL3PUBF	virtual	~TDirtyAllocatorException	();
			};

			struct	CL3PUBT	IDynamicAllocator
			{
				virtual	void*	Alloc	(usys_t sz_bytes) CL3_WARN_UNUSED_RESULT = 0;
				virtual	void	Free	(void* p_mem) = 0;
				virtual	void*	Realloc	(void* p_mem, usys_t sz_byte_ts_new) CL3_WARN_UNUSED_RESULT = 0;
				virtual	usys_t	SizeOf	(void* p_mem) const GETTER = 0;
			};

			class	CL3PUBT	TRestrictAllocator : public IDynamicAllocator
			{
				private:
					CLASS	TRestrictAllocator	(const TRestrictAllocator&);

				protected:
					IDynamicAllocator* allocator;
					usys_t sz_limit;
					usys_t sz_current;

				public:
					CL3PUBF	void*	Alloc	(usys_t sz_bytes) CL3_WARN_UNUSED_RESULT;
					CL3PUBF	void	Free	(void* p_mem);
					CL3PUBF	void*	Realloc	(void* p_mem, usys_t sz_byte_ts_new) CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	SizeOf	(void* p_mem) const GETTER;
					CL3PUBF	usys_t	BytesAllocated	() const GETTER;
					CL3PUBF	usys_t	BytesLimit		() const GETTER;

					CL3PUBF	CLASS	TRestrictAllocator	(IDynamicAllocator* allocator, usys_t sz_limit);
					CL3PUBF	CLASS	~TRestrictAllocator	();
			};

			CL3_PARAMETER_STACK_DECL(IDynamicAllocator*, allocator);
			CL3PUBF extern IDynamicAllocator* exception_allocator;

			CL3PUBF	void	Free	(void*);
			CL3PUBF	void*	Alloc	(usys_t, const typeinfo::TRTTI*);
			CL3PUBF	void*	Realloc	(void*, usys_t, const typeinfo::TRTTI*);
			CL3PUBF	usys_t	SizeOf	(void*);

			//template<class T>	T*	Alloc	(usys_t n_items);	//	defined in system_types_typeinfo.hpp
		}
	}
}

#endif
