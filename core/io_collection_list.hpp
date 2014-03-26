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
#include "error-base.hpp"

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

				//	IIn & IOut follow strict FIFO semantics.
				//	IIn will remove items from the head
				//	IOut will append new items at the tail

				template<class T> struct IList;
				template<class T> class TList;

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
				struct	IList : virtual IDynamicCollection<T>
				{
					using IDynamicCollection<T>::Count;
					using IDynamicCollection<T>::Remove;

					virtual	GETTER	T&			operator[]	(ssize_t index) = 0;	//	returns a reference to the item at index "index", the reference will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	GETTER	const T&	operator[]	(ssize_t index) const = 0;	//	as above
					virtual	GETTER	T*			ItemPtr		(ssize_t index) = 0;	//	returns a pointer to the item at index "index", the pointer will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	GETTER	const T*	ItemPtr		(ssize_t index) const = 0;	//	as above

					virtual	SETTER	void		Count		(size_t new_count, const T& item_init = T()) = 0;	//	reallocates the list to the specified size, removing items at the end when shrinking and appending new items when enlarging (new items get initialized by copy-constructor from "item_init")

					virtual	void				Insert		(ssize_t index, const T& item_insert) = 0;	//	inserts a new item at index "index" which will get copy-constructed from "item_insert"
					virtual	void				Insert		(ssize_t index, const T* arr_items_insert, size_t n_items_insert) = 0;	//	inserts "n_items_insert" new item at index "index" which will get copy-constructed from the items in "arr_items_insert"
					virtual	void				Insert		(ssize_t index, const IStaticCollection<T>& collection) = 0;	//	as above but draws the new items from another collection

					virtual	void				Remove		(ssize_t index, size_t n_items_remove) = 0;	//	removes "n_items_remove" items from the list starting at index "index"

					virtual	void				Append		(const T& item_append) = 0;
					virtual	void				Append		(const T* arr_items_append, size_t n_items_append) = 0;
					virtual	void				Append		(const IStaticCollection<T>& collection) = 0;
				};

				template<class T>
				class	CL3PUBT	TList : public virtual IList<T>
				{
					public:
						//	from IInterlocked
						system::task::synchronization::IMutex&	Mutex	() GETTER;

						//	from IObservable
						const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&	OnChange	() const GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT;
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT;
						size_t	Count		() const GETTER;
						bool	CountMin	(size_t count_min) const GETTER;
						bool	CountMax	(size_t count_max) const GETTER;

						//	from IDynamicCollection
						system::memory::TUniquePtr<IDynamicIterator<T> >		CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT;
						system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const CL3_WARN_UNUSED_RESULT;

						void	Add		(const T& item_add);
						void	Add		(const T* arr_items_add, size_t n_items_add);
						void	Add		(const IStaticCollection<T>& collection);
						void	Remove	(const T* item_remove);

						//	from IOut
						size_t			Write	(const T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min = (size_t)-1);
						off64_t			ReadIn	(io::stream::IIn<T>& is, off64_t n_items_ri_max, off64_t n_items_ri_min = (off64_t)-1);
						off64_t	Space	(size_t sz_unit) const GETTER;

						//	from IIn
						size_t			Read	(T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min = (size_t)-1);
						off64_t			WriteOut(io::stream::IOut<T>& is, off64_t n_items_ri_max, off64_t n_items_ri_min = (off64_t)-1);
						off64_t	Left	(size_t sz_unit) const GETTER;

						//	from IList
						GETTER	T&			operator[]	(ssize_t index);
						GETTER	const T&	operator[]	(ssize_t index) const;
						GETTER	T*			ItemPtr		(ssize_t index);
						GETTER	const T*	ItemPtr		(ssize_t index) const;
						SETTER	void		Count		(size_t new_count, const T& item_init = T());
						void				Insert		(ssize_t index, const T& item_insert);
						void				Insert		(ssize_t index, const T* arr_items_insert, size_t n_items_insert);
						void				Insert		(ssize_t index, const IStaticCollection<T>& collection);
						void				Remove		(ssize_t index, size_t n_items_remove);

						void				Append		(const T& item_append);
						void				Append		(const T* arr_items_append, size_t n_items_append);
						void				Append		(const IStaticCollection<T>& collection);
						void				Append		(const TList& list);
				};

				/**************************************************************/

				//	from IInterlocked
				template<class T>
				system::task::synchronization::IMutex&	TList<T>::Mutex	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IObservable
				template<class T>
				const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&	TList<T>::OnChange	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IStaticCollection
				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >			TList<T>::CreateStaticIterator	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<const T> >	TList<T>::CreateStaticIterator	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				size_t	TList<T>::Count		() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TList<T>::CountMin	(size_t count_min) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TList<T>::CountMax	(size_t count_max) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IDynamicCollection
				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<T> > TList<T>::CreateDynamicIterator()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<const T> > TList<T>::CreateDynamicIterator() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TList<T>::Add		(const T& item_insert)
				{
					Append(item_insert);
				}

				template<class T>
				void	TList<T>::Add		(const T* arr_items_add, size_t n_items_add)
				{
					Append(arr_items_add, n_items_add);
				}

				template<class T>
				void	TList<T>::Add		(const IStaticCollection<T>& collection)
				{
					Append(collection);
				}

				template<class T>
				void	TList<T>::Remove	(const T* item_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IOut
				template<class T>
				size_t	TList<T>::Write		(const T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				off64_t	TList<T>::ReadIn	(io::stream::IIn<T>& is, off64_t n_items_ri_max, off64_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				off64_t	TList<T>::Space		(size_t sz_unit) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIn
				template<class T>
				size_t	TList<T>::Read		(T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				off64_t	TList<T>::WriteOut	(io::stream::IOut<T>& is, off64_t n_items_ri_max, off64_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				off64_t	TList<T>::Left		(size_t sz_unit) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IList
				template<class T>
				T&			TList<T>::operator[]	(ssize_t index)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				const T&	TList<T>::operator[]	(ssize_t index) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				T*			TList<T>::ItemPtr		(ssize_t index)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				const T*	TList<T>::ItemPtr		(ssize_t index) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Count		(size_t new_count, const T& item_init)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Insert	(ssize_t index, const T& item_insert)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Insert	(ssize_t index, const T* arr_items_insert, size_t n_items_insert)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Insert	(ssize_t index, const IStaticCollection<T>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Remove	(ssize_t index, size_t n_items_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Append	(const T& item_append)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Append	(const T* arr_items_append, size_t n_items_append)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Append	(const IStaticCollection<T>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void		TList<T>::Append	(const TList& list)
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}

#endif
