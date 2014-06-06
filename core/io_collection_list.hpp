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
#include "system_memory.hpp"
#include "util.hpp"
//#include "system_types_typeinfo.hpp"

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

				enum	EDirection
				{
					DIRECTION_FORWARD,
					DIRECTION_BACKWARD
				};

				template<class T>
				struct	CL3PUBT	IIterator : public virtual IDynamicIterator<T>
				{
					virtual	usys_t	Index	() const CL3_GETTER = 0;	//	returns the index on which the iterator is currently positioned (might change on Insert()/Remove/Count(x), as the iterator is repositioned to stay on the same item)
					virtual	void	Index	(usys_t new_index) CL3_SETTER = 0;	//	positiones the iterator on the item with the specified index
				};

				template<class T>
				struct	CL3PUBT	IList : virtual IDynamicCollection<T>
				{
					using IDynamicCollection<T>::Count;
					using IDynamicCollection<T>::Remove;

					inline	IList&	operator+=	(const IStaticCollection<T>& rhs) { Append(rhs); return *this; }
					virtual	IList&	operator=	(const IStaticCollection<T>& rhs) = 0;
					virtual	IList&	operator=	(IStaticCollection<T>&& rhs) = 0;

					virtual	T&			operator[]	(ssys_t rindex) CL3_GETTER = 0;	//	returns a reference to the item at index "index", the reference will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	const T&	operator[]	(ssys_t rindex) const CL3_GETTER = 0;	//	as above
					virtual	T*			ItemPtr		(ssys_t rindex) CL3_GETTER = 0;	//	returns a pointer to the item at index "index", the pointer will remain valid until the next function call which changes the size of the list (Insert()/Remove()/Count(x))
					virtual	const T*	ItemPtr		(ssys_t rindex) const CL3_GETTER = 0;	//	as above

					virtual	void	Shrink		(usys_t n_items_shrink) = 0;
					virtual	void	Grow		(usys_t n_items_grow, const T& item_init) = 0;
					virtual	void	Count		(usys_t new_count, const T& item_init = T()) CL3_SETTER = 0;	//	reallocates the list to the specified size, removing items at the end when shrinking and appending new items when enlarging (new items get initialized by copy-constructor from "item_init")

					virtual	void	Insert		(ssys_t rindex, const T& item_insert) = 0;	//	inserts a new item at index "index" which will get copy-constructed from "item_insert"
					virtual	void	Insert		(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert) = 0;	//	inserts "n_items_insert" new item at index "index" which will get copy-constructed from the items in "arr_items_insert"
					virtual	void	Insert		(ssys_t rindex, const IStaticCollection<T>& collection) = 0;	//	as above but draws the new items from another collection

					virtual	void	Remove		(ssys_t rindex, usys_t n_items_remove) = 0;	//	removes "n_items_remove" items from the list starting at index "index"

					virtual	void	Append		(const T& item_append) = 0;
					virtual	void	Append		(const T* arr_items_append, usys_t n_items_append) = 0;
					virtual	void	Append		(const IStaticCollection<T>& collection) = 0;

					//	removed n_head items at the start of the list and n_tail items at the end of the list
					virtual	void	Cut			(usys_t n_head, usys_t n_tail) = 0;

					virtual	usys_t	Find		(const T& item_find, usys_t idx_start = 0, EDirection dir = DIRECTION_FORWARD) const CL3_WARN_UNUSED_RESULT = 0;
				};

				template<class T>
				class	CL3PUBT	TIterator : public virtual IIterator<T>
				{
					protected:
						static const usys_t INDEX_TAIL = (usys_t)-1;
						static const usys_t INDEX_HEAD = (usys_t)-2;

						TList<T>* list;
						mutable usys_t index;

						void	FixIndex	() const;

					public:
						//	from IStaticIterator<const T>
						bool	FindNext	(const IMatcher<T>& matcher);
						bool	FindPrev	(const IMatcher<T>& matcher);
						bool	IsValid		() const CL3_GETTER;
						const T&Item		() const CL3_GETTER;
						void	MoveHead	();
						void	MoveTail	();
						bool	MoveFirst	();
						bool	MoveLast	();
						bool	MoveNext	();
						bool	MovePrev	();

						//	from IStaticIterator<T>
						T&		Item		() CL3_GETTER;

						//	from IDynamicIterator<T>
						void	Insert		(const T& item_insert);
						void	Insert		(const T* arr_items_insert, usys_t n_items_insert);
						void	Remove		();

						//	from IOut<T>
						usys_t	Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						uoff_t	ReadIn		(io::stream::IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIn<T>
						usys_t	Read		(T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						uoff_t	WriteOut	(io::stream::IOut<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIterator
						usys_t	Index		() const CL3_GETTER;
						void	Index		(usys_t new_index) CL3_SETTER;

						//	from TIterator
						CLASS	TIterator	(TList<T>* list, usys_t index);
				};

				template<class T>
				class	CL3PUBT	TList : public virtual IList<T>
				{
					protected:
						T* arr_items;
						usys_t n_items_current;
						usys_t n_items_prealloc;
						const event::TEvent<IStaticCollection<T>, TOnChangeData<T> > on_change;
						bool guard_resize;

						usys_t	AbsIndex	(ssys_t rindex) const;
						void	Prealloc	(usys_t n_items_prealloc_min);

					public:
						//	from IObservable
						const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&	OnChange	() const CL3_GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT;
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT;
						usys_t	Count		() const CL3_GETTER;
						bool	CountMin	(usys_t count_min) const CL3_GETTER;
						bool	CountMax	(usys_t count_max) const CL3_GETTER;

						//	from IDynamicCollection
						system::memory::TUniquePtr<IDynamicIterator<T> >		CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT;
						system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const CL3_WARN_UNUSED_RESULT;

						void	Clear	();
						void	Add		(const T& item_add);
						void	Add		(const T* arr_items_add, usys_t n_items_add);
						void	Add		(const IStaticCollection<T>& collection);
						bool	Remove	(const T* item_remove);

						//	from IOut
						usys_t	Write	(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						uoff_t	ReadIn	(io::stream::IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIn
						usys_t	Read	(T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						uoff_t	WriteOut(io::stream::IOut<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IList
						TList<T>&	operator=	(const IStaticCollection<T>& rhs);
						TList<T>&	operator=	(IStaticCollection<T>&& rhs);
						T&			operator[]	(ssys_t rindex) CL3_GETTER;
						const T&	operator[]	(ssys_t rindex) const CL3_GETTER;
						T*			ItemPtr		(ssys_t rindex) CL3_GETTER;
						const T*	ItemPtr		(ssys_t rindex) const CL3_GETTER;
						void		Shrink		(usys_t n_items_shrink);
						void		Grow		(usys_t n_items_grow, const T& item_init);
						void		Count		(usys_t new_count, const T& item_init = T()) CL3_SETTER;
						void		Insert		(ssys_t rindex, const T& item_insert);
						void		Insert		(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert);
						void		Insert		(ssys_t rindex, const IStaticCollection<T>& collection);
						void		Remove		(ssys_t rindex, usys_t n_items_remove);

						void		Append		(const T& item_append);
						void		Append		(const T* arr_items_append, usys_t n_items_append);
						void		Append		(const IStaticCollection<T>& collection);
						void		Append		(const TList& list);

						void		Cut			(usys_t n_head, usys_t n_tail);

						usys_t		Find		(const T& item_find, usys_t idx_start = 0, EDirection dir = DIRECTION_FORWARD) const CL3_WARN_UNUSED_RESULT;

						//	from TList
						TList<T>&	operator=	(const TList<T>& rhs);
						TList<T>&	operator=	(TList<T>&& rhs);

						CLASS		TList		();
						CLASS		TList		(T* arr_items, usys_t n_items, bool claim);
						CLASS		TList		(const T* arr_items, usys_t n_items);
						CLASS		TList		(const IStaticCollection<T>&);
						CLASS		TList		(const TList&);
						CLASS		~TList		();
				};

				/**************************************************************/

				//	from IStaticIterator<const T>
				template<class T>
				bool	TIterator<T>::FindNext	(const IMatcher<T>& matcher)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::FindPrev	(const IMatcher<T>& matcher)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::IsValid	() const
				{
					this->FixIndex();
					return this->index != INDEX_TAIL && this->index != INDEX_HEAD;
				}

				template<class T>
				const T&TIterator<T>::Item		() const
				{
					this->FixIndex();
					CL3_CLASS_ERROR(this->index == INDEX_TAIL || this->index == INDEX_HEAD, TIndexOutOfBoundsException, this->index, this->list->Count());
					return (*list)[this->index];
				}

				template<class T>
				void	TIterator<T>::MoveHead	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TIterator<T>::MoveTail	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::MoveFirst	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::MoveLast	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::MoveNext	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<T>::MovePrev	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IStaticIterator<T>
				template<class T>
				T&		TIterator<T>::Item		()
				{
					this->FixIndex();
					CL3_CLASS_ERROR(this->index == INDEX_TAIL || this->index == INDEX_HEAD, TIndexOutOfBoundsException, this->index, this->list->Count());
					return (*list)[this->index];
				}

				//	from IDynamicIterator<T>
				template<class T>
				void	TIterator<T>::Insert	(const T& item_insert)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TIterator<T>::Insert	(const T* arr_items_insert, usys_t n_items_insert)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TIterator<T>::Remove	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IOut<T>
				template<class T>
				usys_t	TIterator<T>::Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				uoff_t	TIterator<T>::ReadIn	(io::stream::IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIn<T>
				template<class T>
				usys_t	TIterator<T>::Read		(T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				uoff_t	TIterator<T>::WriteOut	(io::stream::IOut<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIterator
				template<class T>
				usys_t	TIterator<T>::Index		() const
				{
					this->FixIndex();
					return this->index;
				}

				template<class T>
				void	TIterator<T>::Index		(usys_t new_index)
				{
					CL3_CLASS_ERROR(new_index > this->list->Count(), TIndexOutOfBoundsException, new_index, this->list->Count());
					this->index = new_index;
					this->FixIndex();
				}

				//	from TIterator
				template<class T>
				void	TIterator<T>::FixIndex	() const
				{
					if(this->index >= this->list->Count())
						this->index = INDEX_TAIL;
				}

				template<class T>
				CLASS	TIterator<T>::TIterator	(TList<T>* list, usys_t index) : list(list)
				{
					this->Index(index);
				}

				/**************************************************************/

				template<class T>
				usys_t	TList<T>::AbsIndex	(ssys_t rindex) const
				{
					if(rindex < 0)
					{
						const usys_t tmp = -rindex;
						return n_items_current - tmp;
					}
					else
						return rindex;
				}

				template<class T>
				void	TList<T>::Prealloc	(usys_t n_items_prealloc_min)
				{
					const usys_t n_items_prealloc_ideal = (n_items_current >> 8) + (32 / sizeof(T));

					if((n_items_prealloc < n_items_prealloc_min) || (n_items_prealloc - n_items_prealloc_min > 2 * n_items_prealloc_ideal))
					{
						T* arr_items_new = (T*)system::memory::Realloc(arr_items, (n_items_current + n_items_prealloc_ideal + n_items_prealloc_min) * sizeof(T), NULL, true);
						if(!arr_items_new)
						{
							arr_items_new = (T*)system::memory::Alloc(n_items_current + n_items_prealloc_ideal + n_items_prealloc_min, NULL);

							usys_t n_items_copied = 0;
							try
							{
								for(n_items_copied = 0; n_items_copied < n_items_current; n_items_copied++)
									new (arr_items_new + n_items_copied) T(util::move(arr_items[n_items_copied]));
							}
							catch(...)
							{
								for(usys_t i = 0; i < n_items_current; i++)
									arr_items_new[i].~T();
								system::memory::Free(arr_items_new);
								throw;
							}

							for(usys_t i = 0; i < n_items_current; i++)
								arr_items[i].~T();
							system::memory::Free(arr_items);
						}
						arr_items = arr_items_new;
						n_items_prealloc = n_items_prealloc_ideal;
					}
				}

				//	from IObservable
				template<class T>
				const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&
						TList<T>::OnChange	() const
				{
					return on_change;
				}

				//	from IStaticCollection
				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >
						TList<T>::CreateStaticIterator	()
				{
					return system::memory::MakeUniquePtr<IStaticIterator<T> >(new TIterator<T>(this, n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<const T> >
						TList<T>::CreateStaticIterator	() const
				{
					return system::memory::MakeUniquePtr<IStaticIterator<const T> >(new TIterator<T>(const_cast<TList*>(this), n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				usys_t	TList<T>::Count		() const
				{
					return n_items_current;
				}

				template<class T>
				bool	TList<T>::CountMin	(usys_t count_min) const
				{
					return n_items_current >= count_min;
				}

				template<class T>
				bool	TList<T>::CountMax	(usys_t count_max) const
				{
					return n_items_current <= count_max;
				}

				//	from IDynamicCollection
				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<T> >		TList<T>::CreateDynamicIterator()
				{
					return system::memory::MakeUniquePtr<IDynamicIterator<T> >(new TIterator<T>(this, n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<const T> >	TList<T>::CreateDynamicIterator() const
				{
					return system::memory::MakeUniquePtr<IDynamicIterator<const T> >(new TIterator<T>(const_cast<TList*>(this), n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				void	TList<T>::Clear		()
				{
					for(usys_t i = 0; i < n_items_current; i++)
						arr_items[i].~T();
					n_items_prealloc += n_items_current;
					n_items_current = 0;
					system::memory::Free(arr_items);
					arr_items = NULL;
					n_items_prealloc = 0;
				}

				template<class T>
				void	TList<T>::Add		(const T& item_insert)
				{
					Append(item_insert);
				}

				template<class T>
				void	TList<T>::Add		(const T* arr_items_add, usys_t n_items_add)
				{
					Append(arr_items_add, n_items_add);
				}

				template<class T>
				void	TList<T>::Add		(const IStaticCollection<T>& collection)
				{
					Append(collection);
				}

				template<class T>
				bool	TList<T>::Remove	(const T* item_remove)
				{
					usys_t index;
					if(item_remove >= arr_items && item_remove < arr_items + n_items_current)
						index = item_remove - arr_items;
					else
					{
						index = Find(*item_remove);
						if(index == (usys_t)-1)
							return false;
					}

					Remove(index, 1);
					return true;
				}

				//	from IOut
				template<class T>
				usys_t	TList<T>::Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					Append(arr_items_write, n_items_write_max);
					return n_items_write_max;
				}

				template<class T>
				uoff_t	TList<T>::ReadIn	(io::stream::IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIn
				template<class T>
				usys_t	TList<T>::Read		(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					if(n_items_read_min >= n_items_read_max) n_items_read_min = n_items_read_max;
					CL3_CLASS_ERROR(n_items_read_min > n_items_current, stream::TSourceDryException, n_items_read_max, n_items_read_min, 0, n_items_current);
					const usys_t n_items_read = CL3_MIN(n_items_read_max, n_items_current);

					for(usys_t i = 0; i < n_items_read; i++)
						arr_items_read[i] = util::move(arr_items[i]);

					Remove(0, n_items_read);
					return n_items_read;
				}

				template<class T>
				uoff_t	TList<T>::WriteOut	(io::stream::IOut<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IList
				template<class T>
				TList<T>&	TList<T>::operator=		(const IStaticCollection<T>& rhs)
				{
					const TList<T>* list = dynamic_cast<const TList<T>*>(&rhs);
					if(list)
						(*this) = (*list);
					else
					{
						Clear();
						Append(rhs);
					}
					return *this;
				}

				template<class T>
				TList<T>&	TList<T>::operator=		(IStaticCollection<T>&& rhs)
				{
					TList<T>* list = dynamic_cast<TList<T>*>(&rhs);
					if(list)
						(*this) = util::move(*list);
					else
					{
						Clear();
						Append(rhs);
					}
					return *this;
				}

				template<class T>
				T&			TList<T>::operator[]	(ssys_t rindex)
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items[index];
				}

				template<class T>
				const T&	TList<T>::operator[]	(ssys_t rindex) const
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items[index];
				}

				template<class T>
				T*			TList<T>::ItemPtr	(ssys_t rindex)
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items + index;
				}

				template<class T>
				const T*	TList<T>::ItemPtr	(ssys_t rindex) const
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items + index;
				}

				template<class T>
				void		TList<T>::Shrink	(usys_t n_items_shrink)
				{
					for(usys_t i = 1; i <= n_items_shrink; i++)
						arr_items[n_items_current - i].~T();

					n_items_current  -= n_items_shrink;
					n_items_prealloc += n_items_shrink;

					Prealloc(0);
				}

				template<class T>
				void		TList<T>::Grow		(usys_t n_items_grow, const T& item_init)
				{
					Prealloc(n_items_grow);

					for(usys_t i = 0; i < n_items_grow; i++)
						new (arr_items + n_items_current + i) T(item_init);

					n_items_current  += n_items_grow;
					n_items_prealloc -= n_items_grow;
				}

				template<class T>
				void		TList<T>::Count		(usys_t new_count, const T& item_init)
				{
					if(new_count > n_items_current)
						Grow(new_count - n_items_current, item_init);
					else if(new_count < n_items_current)
						Shrink(n_items_current - new_count);
				}

				template<class T>
				void		TList<T>::Insert	(ssys_t rindex, const T& item_insert)
				{
					Insert(rindex, &item_insert, 1);
				}

				template<class T>
				void		TList<T>::Insert	(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert)
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index > n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					const usys_t n_items_relocate = n_items_current - index;

					Prealloc(n_items_insert);

					for(usys_t i = 1; i <= n_items_relocate; i++)
					{
						const usys_t idx_from = index + n_items_relocate - i;
						const usys_t idx_to   = n_items_current + n_items_insert - i;
						new (arr_items + idx_to) T(util::move(arr_items[idx_from]));
					}

					const usys_t n_items_assign = CL3_MIN(n_items_relocate, n_items_insert);
					for(usys_t i = 0; i < n_items_assign; i++)
						arr_items[index + i] = arr_items_insert[i];

					for(usys_t i = n_items_assign; i < n_items_insert; i++)
						new (arr_items + index + i) T(arr_items_insert[i]);

					n_items_current  += n_items_insert;
					n_items_prealloc -= n_items_insert;
				}

				template<class T>
				void		TList<T>::Insert	(ssys_t rindex, const IStaticCollection<T>& collection)
				{
					const TList<T>* list = dynamic_cast<const TList<T>*>(&collection);
					if(list)
						Insert(rindex, list->arr_items, list->n_items_current);
					else
					{
						usys_t index = AbsIndex(rindex);
						CL3_CLASS_ERROR(index > n_items_current, TIndexOutOfBoundsException, index, n_items_current);
						system::memory::TUniquePtr<IStaticIterator<const T> > it = collection.CreateStaticIterator();
						it->MoveHead();
						while(it->MoveNext())
							Insert(index++, it->Item());
					}
				}

				template<class T>
				void		TList<T>::Remove	(ssys_t rindex, usys_t n_items_remove)
				{
					const usys_t index = AbsIndex(rindex);
					CL3_CLASS_ERROR(index + n_items_remove > n_items_current, TIndexOutOfBoundsException, index + n_items_remove, n_items_current);

					const usys_t n_items_after = n_items_current - index - n_items_remove;

					for(usys_t i = 0; i < n_items_after; i++)
						arr_items[index + i] = util::move(arr_items[index + i + n_items_remove]);

					Shrink(n_items_remove);
				}

				template<class T>
				void		TList<T>::Append	(const T& item_append)
				{
					Append(&item_append, 1);
				}

				template<class T>
				void		TList<T>::Append	(const T* arr_items_append, usys_t n_items_append)
				{
					Prealloc(n_items_append);

					for(usys_t i = 0; i < n_items_append; i++)
						new (arr_items + n_items_current + i) T(arr_items_append[i]);

					n_items_current  += n_items_append;
					n_items_prealloc -= n_items_append;
				}

				template<class T>
				void		TList<T>::Append	(const IStaticCollection<T>& collection)
				{
					const TList<T>* list = dynamic_cast<const TList<T>*>(&collection);
					if(list)
						Append(list->arr_items, list->n_items_current);
					else
					{
						system::memory::TUniquePtr<IStaticIterator<const T> > it = collection.CreateStaticIterator();
						it->MoveHead();
						while(it->MoveNext())
							Append(it->Item());
					}
				}

				template<class T>
				void		TList<T>::Append	(const TList& list)
				{
					Append(list.arr_items, list.n_items_current);
				}

				template<class T>
				void		TList<T>::Cut		(usys_t n_head, usys_t n_tail)
				{
					this->Remove(this->n_items_current - n_tail, n_tail);
					this->Remove(0, n_head);
				}

				template<class T>
				usys_t		TList<T>::Find		(const T& item_find, usys_t idx_start, EDirection dir) const
				{
					switch(dir)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = idx_start; i < n_items_current; i++)
								if(item_find == arr_items[i])
									return i;
							return (usys_t)-1;

						case DIRECTION_BACKWARD:
							for(usys_t i = idx_start; i != (usys_t)-1; i--)
								if(item_find == arr_items[i])
									return i;
							return (usys_t)-1;
					}
					CL3_UNREACHABLE;
				}

				//	from TList<T>
				template<class T>
				TList<T>&	TList<T>::operator=	(const TList<T>& rhs)
				{
					Clear();
					Append(rhs);
					return *this;
				}

				template<class T>
				TList<T>&	TList<T>::operator=	(TList<T>&& rhs)
				{
					Clear();
					arr_items = rhs.arr_items;
					n_items_current = rhs.n_items_current;
					n_items_prealloc = rhs.n_items_prealloc;
					rhs.arr_items = NULL;
					rhs.n_items_current = 0;
					rhs.n_items_prealloc = 0;
					return *this;
				}

				template<class T>
				CLASS		TList<T>::TList		() : arr_items(NULL), n_items_current(0), n_items_prealloc(0), on_change(), guard_resize(false)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(T* arr_items, usys_t n_items, bool claim) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), on_change(), guard_resize(false)
				{
					if(claim)
					{
						this->arr_items = arr_items;
						this->n_items_current = n_items;
					}
					else
					{
						Prealloc(n_items);
						Append(arr_items, n_items);
					}
				}

				template<class T>
				CLASS		TList<T>::TList		(const T* arr_items, usys_t n_items) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), on_change(), guard_resize(false)
				{
					Prealloc(n_items);
					Append(arr_items, n_items);
				}

				template<class T>
				CLASS		TList<T>::TList		(const IStaticCollection<T>& collection) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), on_change(), guard_resize(false)
				{
					Append(collection);
				}

				template<class T>
				CLASS		TList<T>::TList		(const TList& list) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), on_change(), guard_resize(false)
				{
					Prealloc(list.n_items_current);
					Append(list.arr_items, list.n_items_current);
				}

				template<class T>
				CLASS		TList<T>::~TList	()
				{
					Clear();
				}
			}
		}
	}
}

#endif
