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
#include "error-base.hpp"
#include "context.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			CL3PUBF	void	cxx_free	(void*);
			CL3PUBF	void*	cxx_malloc	(size_t);
			CL3PUBF	void*	cxx_realloc	(void*, size_t);

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
								cxx_free(object);
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
					size_t sz_bytes;

				public:
					CL3PUBF	CLASS	TBadAllocException	(size_t sz_bytes);
					CL3PUBF	CLASS	TBadAllocException	(const TBadAllocException&);
					CL3PUBF	virtual	~TBadAllocException	();
			};

			struct	CL3PUBT	IDynamicAllocator
			{
				virtual	void*	Alloc	(size_t sz_bytes) = 0;
				virtual	void	Free	(void* p_mem) = 0;
				virtual	void*	Realloc	(void* p_mem, size_t sz_bytes_new) = 0;
			};

			CL3_PARAMETER_STACK_DECL(IDynamicAllocator*, allocator);
		}
	}
}

#endif
