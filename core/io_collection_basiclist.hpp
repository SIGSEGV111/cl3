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

#ifndef	_include_cl3_core_io_collection_basiclist_hpp_
#define	_include_cl3_core_io_collection_basiclist_hpp_

#include "system_memory.hpp"
#include "system_types.hpp"
#include <string.h>

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	basiclist
			{
				using namespace system::types;

				template<class T>
				class TBasicList
				{
					/*
						This class has some limitations and should not be used outside a very controlled environment.
						Items must be trivialy copyable. There is little error-checking. This list always uses malloc() & co.
					*/
					private:
						TBasicList& operator=(const TBasicList&);

					protected:
						usys_t n_prealloc;
						usys_t n_items;
						T* arr_items;

						void	Grow	(usys_t count)
						{
							if(n_prealloc < count)
							{
								const usys_t n_prealloc_new = count + (n_items + count) / 8 + 128U / sizeof(T);
								arr_items = (T*)Realloc(arr_items, sizeof(T) * (n_items + n_prealloc_new));
								n_prealloc = n_prealloc_new;
							}
							n_prealloc -= count;
							n_items += count;
						}

						void	Shrink	(usys_t count)
						{
							n_prealloc += count;
							n_items -= count;
							const usys_t n_threshold = n_items + 256U / sizeof(T);
							if(n_prealloc > n_threshold)
							{
								const usys_t n_prealloc_new = n_items / 16 + 64U / sizeof(T);
								arr_items = (T*)Realloc(arr_items, sizeof(T) * (n_items + n_prealloc_new));
								n_prealloc = n_prealloc_new;
							}
						}

					public:
						T*		Push	(const T& item)
						{
							const usys_t index = n_items;
							Grow(1);
							new (arr_items + index) T(item);
							return arr_items + index;
						}

						T*		Push	(const T* arr_items_push, usys_t n_items_push)
						{
							const usys_t index = n_items;
							Grow(n_items_push);
							for(usys_t i = 0; i < n_items_push; ++i)
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

						void	Remove	(usys_t index, usys_t count = 1U)
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

						usys_t	Count	() const
						{
							return n_items;
						}

						T&			operator[]	(usys_t index)
						{
							return arr_items[index];
						}

						const T&	operator[]	(usys_t index) const
						{
							return arr_items[index];
						}

						CLASS	TBasicList	() : n_prealloc(0), n_items(0), arr_items(NULL) {}

						CLASS	TBasicList	(const TBasicList& other)
						{
							arr_items = Alloc(sizeof(T) * other.n_items);
							n_items = other.n_items;
							n_prealloc = 0;
						}

						CLASS	~TBasicList	()
						{
							free(arr_items);
						}
				};
			}
		}
	}
}

#endif
