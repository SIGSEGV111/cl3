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

#ifndef	_include_cl3_core_io_collection_list_hpp_
#define	_include_cl3_core_io_collection_list_hpp_

#include "io_collection.hpp"
#include "error.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	list
			{
				//	index: a negative index starts at the end of the list (e.g. -1 would be the last item (Count()-1), whereas -2 would be the second last item, ...)
				//	a list preserves the order of the items unless it was explicitly switched into "lose mode" with "allow_reorder" set to "true"

				class	TIndexOutOfBoundsException : public error::TException
				{
					protected:
						ssize_t index;
						size_t count;

					public:
						CL3PUBF	CLASS	TIndexOutOfBoundsException	(ssize_t index, size_t count);
				};

				template<class T>
				struct	IIterator : IDynamicIterator<T>
				{
					virtual	GETTER	size_t	Index	() const = 0;	//	returns the index on which the iterator is currently positioned (might change on Insert()/Remove/Count(x), as the iterator is repositioned to stay on the same item)
					virtual	SETTER	void	Index	(size_t new_index) = 0;	//	positiones the iterator on the item with the specified index
				};

				template<class T>
				struct	IList : IDynamicCollection<T>
				{
					virtual	IIterator<T>*		CreateIterator	() = 0;
					virtual	IIterator<const T>*	CreateIterator	() const = 0;

					virtual	GETTER	T&			operator[]	(ssize_t index) = 0;	//	returns a reference to the item at index "index", the reference will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	GETTER	const T&	operator[]	(ssize_t index) const = 0;	//	as above
					virtual	GETTER	T*			ItemPtr		(ssize_t index) = 0;	//	returns a pointer to the item at index "index", the pointer will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	GETTER	const T*	ItemPtr		(ssize_t index) const = 0;	//	as above

					virtual	SETTER	void		Count		(size_t new_count, const T& item_init = T()) = 0;	//	reallocates the list to the specified size, removing items at the end when shrinking and appending new items when enlarging (new items get initialized by copy-constructor from "item_init")

					virtual	void				Insert		(ssize_t index, const T& item_insert) = 0;	//	inserts a new item at index "index" which will get copy-constructed from "item_insert"
					virtual	void				Insert		(ssize_t index, const T* arr_items_insert, size_t n_items_insert) = 0;	//	inserts "n_items_insert" new item at index "index" which will get copy-constructed from the items in "arr_items_insert"
					virtual	void				Insert		(ssize_t index, const IStaticCollection<T>& collection) = 0;	//	as above but draws the new items from another collection
					virtual	void				Remove		(ssize_t index, size_t n_items_remove) = 0;	//	removes "n_items_remove" items from the list starting at index "index"
				};
			}
		}
	}
}

#endif
