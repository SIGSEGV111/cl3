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

#ifndef	_include_cl3_core_util_h_
#define	_include_cl3_core_util_h_

#include "system_compiler.h"
#include "system_memory.h"
#include <malloc.h>
#include <string.h>

namespace	cl3
{
	namespace	util
	{
		CL3PUBF	system::memory::TUniquePtr<char[],false> mprintf(const char* format, ...);
		CL3PUBF	system::memory::TUniquePtr<char[],false> mkstrcpy(const char* str);

		template<class T>
		class	TCoreList
		{
			/*
				This class has some limitations and should not be used outside a very controlled environment.
				Items must be trivialy copyable. There is little error-checking. This list always uses malloc() & co.
			*/
			private:
				TCoreList& operator=(const TCoreList&);

			protected:
				size_t n_prealloc;
				size_t n_items;
				T* arr_items;

				void	Grow	(size_t count)
				{
					if(n_prealloc < count)
					{
						const size_t n_prealloc_new = count + (n_items + count) / 8 + 128U / sizeof(T);
						arr_items = (T*)system::memory::safe_realloc(arr_items, sizeof(T) * (n_items + n_prealloc_new));
						n_prealloc = n_prealloc_new;
					}
					n_prealloc -= count;
					n_items += count;
				}

				void	Shrink	(size_t count)
				{
					n_prealloc += count;
					n_items -= count;
					const size_t n_threshold = n_items + 256U / sizeof(T);
					if(n_prealloc > n_threshold)
					{
						const size_t n_prealloc_new = n_items / 16 + 64U / sizeof(T);
						arr_items = (T*)system::memory::safe_realloc(arr_items, sizeof(T) * (n_items + n_prealloc_new));
						n_prealloc = n_prealloc_new;
					}
				}

			public:
				T*		Push	(const T& item)
				{
					const size_t index = n_items;
					Grow(1);
					new (arr_items + index) T(item);
					return arr_items + index;
				}

				T*		Push	(const T* arr_items_push, size_t n_items_push)
				{
					const size_t index = n_items;
					Grow(n_items_push);
					for(size_t i = 0; i < n_items_push; ++i)
						new (arr_items + (index + i)) T(arr_items_push[i]);
					return arr_items + index;
				}

				void	Pop		(T& item)
				{
					item = arr_items[n_items-1];
					Shrink(1);
				}

				T		Pop		()
				{
					//	fast... but does not perform a Shrink()
					n_prealloc++;
					return arr_items[--n_items];
				}

				T&		First	()
				{
					return arr_items[0];
				}

				T&		Last	()
				{
					return arr_items[n_items-1];
				}

				void	Remove	(size_t index, size_t count = 1U)
				{
					::memmove(arr_items + index, arr_items + index + count, (n_items - index - count) * sizeof(T));
					Shrink(count);
				}

				T*		Claim	()
				{
					T* tmp = arr_items;
					arr_items = NULL;
					n_items = 0;
					n_prealloc = 0;
					return tmp;
				}

				size_t	Count	() const
				{
					return n_items;
				}

				T&			operator[]	(size_t index)
				{
					return arr_items[index];
				}

				const T&	operator[]	(size_t index) const
				{
					return arr_items[index];
				}

				CLASS	TCoreList	() : arr_items(NULL), n_items(0), n_prealloc(0) {}

				CLASS	TCoreList	(const TCoreList& other)
				{
					arr_items = system::memory::safe_malloc(sizeof(T) * other.n_items);
					n_items = other.n_items;
					n_prealloc = 0;
				}

				CLASS	~TCoreList	()
				{
					free(arr_items);
				}
		};
	}
}

#endif
