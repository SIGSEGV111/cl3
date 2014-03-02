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
#include "error.hpp"
#include <malloc.h>

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			template<class T, bool use_delete = true>
			class	TUniquePtr
			{
				private:
					CLASS		TUniquePtr	(const TUniquePtr&);
					TUniquePtr&	operator=	(const TUniquePtr&);
				protected:
					T* object;
				public:
					TUniquePtr&	operator=	(TUniquePtr& other)
					{
						if(use_delete) delete object; else ::free(object);
						this->object = other.object;
						other.object = NULL;
						return *this;
					}

					TUniquePtr&	operator=	(T* new_object)
					{
						if(use_delete) delete[] object; else ::free(object);
						object = new_object;
						return *this;
					}

					GETTER	T&	operator*	() { return *object; }
					GETTER	T*	operator->	() { return object; }
					GETTER	T*	Object		() { return object; }

					GETTER	T*	Claim		() { T* tmp = object; object = NULL; return tmp; }
					void		Reset		() { object = NULL; }	//	does *NOT* release any memory - just removes control of the object from this class

					GETTER	const T&	operator*	() const { return *object; }
					GETTER	const T*	operator->	() const { return object; }
					GETTER	const T*	Object		() const { return object; }

					CLASS	TUniquePtr	(T* object = NULL) throw() : object(object) {}
					CLASS	TUniquePtr	(TUniquePtr& other) throw() : object(other.object) { other.object = NULL; }
					CLASS	~TUniquePtr	() { if(use_delete) delete object; else ::free(object); }
			};

			template<class T, bool use_delete>
			class	TUniquePtr<T[], use_delete>
			{
				private:
					CLASS		TUniquePtr	(const TUniquePtr&);
					TUniquePtr&	operator=	(const TUniquePtr&);
				protected:
					T* array;
				public:
					TUniquePtr&	operator=	(TUniquePtr& other)
					{
						if(use_delete) delete[] array; else ::free(array);
						this->array = other.array;
						other.array = NULL;
						return *this;
					}

					TUniquePtr&	operator=	(T* new_array)
					{
						if(use_delete) delete[] array; else ::free(array);
						array = new_array;
						return *this;
					}

					GETTER	T&	operator[]	(size_t index) { return array[index]; }
					GETTER	T*	Array		() { return array; }

					GETTER	T*	Claim		() { T* tmp = array; array = NULL; return tmp; }
					void		Reset		() { array = NULL; }	//	does *NOT* release any memory - just removes control of the object from this class

					GETTER	const T&	operator[]	(size_t index) const { return array[index]; }
					GETTER	const T*	Array		() const { return array; }

					CLASS	TUniquePtr	(T* array = NULL) throw() : array(array) {}
					CLASS	TUniquePtr	(TUniquePtr& other) throw() : array(other.array) { other.array = NULL; }
					CLASS	~TUniquePtr	() { if(use_delete) delete[] array; else ::free(array); }
			};

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

			template<size_t sz_item>
			struct	CL3PUBT	IItemAllocator
			{
				virtual	void*	Alloc	() = 0;
				virtual	void	Free	(void* item) = 0;
			};

			struct	CL3PUBT	TDefaultAllocator : IDynamicAllocator
			{
				CL3PUBF static TDefaultAllocator instance;
				CL3PUBF	void*	Alloc	(size_t sz_bytes);
				CL3PUBF	void	Free	(void* p_mem);
				CL3PUBF	void*	Realloc	(void* p_mem, size_t sz_bytes_new);
			};

			CL3PUBF	void* safe_malloc	(size_t);
			CL3PUBF	void* safe_realloc	(void*, size_t);
		}
	}
}

#endif
