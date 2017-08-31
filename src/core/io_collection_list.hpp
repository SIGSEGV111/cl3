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
#include "io_collection_array.hpp"
#include "error.hpp"
#include "system_memory.hpp"
#include "util.hpp"
#include "io_serialization.hpp"
#include "system_types_typeinfo.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	list
			{
				namespace	_
				{
				}

				//	index: a negative index starts at the end of the list (e.g. -1 would be the last item (Count()-1), whereas -2 would be the second last item, ...)
				//	a list preserves the order of the items unless it was explicitly switched into "lose mode" with "allow_reorder" set to "true"

				//	IIn & IOut follow strict FIFO semantics.
				//	IIn will remove items from the head
				//	IOut will append new items at the tail

				template<class T> struct IList;
				template<class T> class TList;
				template<class T> struct IIterator;
				template<class T> class TIterator;

				template<class T>
				struct	CL3PUBT	IIterator<const T> : virtual IDynamicIterator<const T>
				{
					virtual	usys_t	Index	() const CL3_GETTER = 0;	//	returns the index on which the iterator is currently positioned (might change on Insert()/Remove/Count(x), as the iterator is repositioned to stay on the same item)
					virtual	void	Index	(usys_t new_index) CL3_SETTER = 0;	//	positiones the iterator on the item with the specified index

					virtual	CLASS	~IIterator	() {}
				};

				template<class T>
				struct	CL3PUBT	IIterator : virtual IDynamicIterator<T>, virtual IIterator<const T>
				{
					virtual	CLASS	~IIterator	() {}
				};

				template<class T>
				struct	CL3PUBT	IList<const T> : virtual IDynamicCollection<const T>, virtual array::IArray<const T>
				{
					inline	IList<const T>&	operator+=	(const IStaticCollection<const T>& rhs) { Append(rhs); return *this; }
// 					inline	IList<const T>&	operator+=	(std::initializer_list<T> l)
// 					{
// // 						this->Prealloc(l.size());
// 						for(usys_t i = 0; i < l.size(); i++)
// 							this->Append(l.begin()[i]);
// 						return *this;
// 					}

					virtual	void	Grow		(usys_t n_items_grow, const T& item_init) = 0;

					virtual	void	Insert		(ssys_t rindex, const T& item_insert) = 0;	//	inserts a new item at index "index" which will get copy-constructed from "item_insert"
					virtual	void	Insert		(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert) = 0;	//	inserts "n_items_insert" new item at index "index" which will get copy-constructed from the items in "arr_items_insert"
					virtual	void	Insert		(ssys_t rindex, const IStaticCollection<const T>& collection) = 0;	//	as above but draws the new items from another collection

					virtual	void	Prepend		(const T& item_prepend) = 0;
					virtual	void	Prepend		(const T* arr_items_prepend, usys_t n_items_prepend) = 0;
					virtual	void	Prepend		(const IStaticCollection<const T>& collection) = 0;

					virtual	void	Append		(const T& item_append) = 0;
					virtual	void	Append		(const T* arr_items_append, usys_t n_items_append) = 0;
					virtual	void	Append		(const IStaticCollection<const T>& collection) = 0;

					virtual	CLASS	~IList		() {}
				};

				template<class T>
				struct	CL3PUBT	IList : virtual IDynamicCollection<T>, virtual array::IArray<T>, virtual IList<const T>
				{
					using IStaticCollection<const T>::Count;
					using IDynamicCollection<T>::Remove;
					using stream::IOut<T>::Write;
					using array::IArray<T>::Read;
					using IList<const T>::operator+=;

// 					inline	IList&	operator+=	(const IStaticCollection<T>& rhs) { Append(rhs); return *this; }
					virtual	IList&	operator=	(const IStaticCollection<T>& rhs) = 0;
					virtual	IList&	operator=	(IStaticCollection<T>&& rhs) = 0;
 					virtual	IList&	operator=	(std::initializer_list<T>) = 0;

					virtual	void	Count		(usys_t new_count, const T& item_init = T()) CL3_SETTER = 0;	//	reallocates the list to the specified size, removing items at the end when shrinking and appending new items when enlarging (new items get initialized by copy-constructor from "item_init")
					virtual	T*		Claim		() CL3_WARN_UNUSED_RESULT = 0;	//	takes control of the internal memory of the list
					virtual	void	Remove		(ssys_t rindex, usys_t n_items_remove) = 0;	//	removes "n_items_remove" items from the list starting at index "index"
					virtual	void	Cut			(usys_t n_head, usys_t n_tail) = 0;	//	removed n_head items at the start of the list and n_tail items at the end of the list
					virtual	void	Shrink		(usys_t n_items_shrink) = 0;

					using IList<const T>::Grow;
					using IList<const T>::Count;
					using IList<const T>::Insert;
					using IList<const T>::Append;

					virtual	CLASS	~IList		() {}
				};

				template<class T>
				class	CL3PUBT	TIterator<const T> : public virtual IIterator<const T>
				{
					protected:
						static const usys_t INDEX_TAIL = (usys_t)-1;
						static const usys_t INDEX_HEAD = (usys_t)-2;

						TList<const T>* list;
						mutable usys_t index;

						void	FixIndex	() const;

					public:
						//	from IStaticIterator<const T>
						bool	IsValid		() const CL3_GETTER;
						const T& Item		() const CL3_GETTER;
						void	MoveHead	();
						void	MoveTail	();
						bool	MoveFirst	();
						bool	MoveLast	();
						bool	MoveNext	();
						bool	MovePrev	();

						//	from IDynamicIterator<T>
						void	Insert		(const T& item_insert);
						void	Insert		(const T* arr_items_insert, usys_t n_items_insert);

						//	from IIn<T>
						usys_t	Read		(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) CL3_WARN_UNUSED_RESULT;

						//	from IIterator
						usys_t	Index		() const CL3_GETTER;
						void	Index		(usys_t new_index) CL3_SETTER;

						//	from TIterator
						CLASS	TIterator	(TList<const T>* list, usys_t index);
						virtual	~TIterator	() {}
				};

				template<class T>
				class	CL3PUBT	TIterator : public virtual IIterator<T>, public virtual TIterator<const T>
				{
					protected:
						using TIterator<const T>::INDEX_TAIL;
						using TIterator<const T>::INDEX_HEAD;
						using TIterator<const T>::list;
						using TIterator<const T>::index;
						using TIterator<const T>::FixIndex;

					public:
						using TIterator<const T>::IsValid;
						using TIterator<const T>::Item;
						using TIterator<const T>::MoveHead;
						using TIterator<const T>::MoveTail;
						using TIterator<const T>::MoveFirst;
						using TIterator<const T>::MoveLast;
						using TIterator<const T>::MoveNext;
						using TIterator<const T>::MovePrev;
						using TIterator<const T>::Insert;
						using TIterator<const T>::Read;
						using TIterator<const T>::Index;

						//	from IStaticIterator<T>
						T&		Item		() CL3_GETTER;
						void	Remove		();

						//	from IOut<T>
						usys_t	Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;

						//	from TIterator
						CLASS	TIterator	(TList<T>* list, usys_t index);
						virtual	~TIterator	() {}
				};

				template<class T>
				class	CL3PUBT	TList<const T> : public virtual IList<const T>
				{
					protected:
						T* arr_items;
						usys_t n_items_current;
						usys_t n_items_prealloc;
						bool is_sorted;

						void		Prealloc		(usys_t n_items_prealloc_min);
						void		InternalClear	();

					public:
						//	from IArray
						bool	IsSorted	() const final override CL3_GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const final override CL3_WARN_UNUSED_RESULT;
						usys_t		Count		() const final override CL3_GETTER;
						bool		CountMin	(usys_t count_min) const final override CL3_GETTER;
						bool		CountMax	(usys_t count_max) const final override CL3_GETTER;
						bool		Contains	(const T& item) const final override CL3_GETTER;

						//	from IDynamicCollection
						system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const final override CL3_WARN_UNUSED_RESULT;

						void		Add		(const T& item_add) final override;
						void		Add		(const T* arr_items_add, usys_t n_items_add) final override;
						void		Add		(const IStaticCollection<const T>& collection) final override;

						//	from IOut<T>
						usys_t		Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from IList
						const T&	operator[]	(ssys_t rindex) const final override CL3_GETTER;
						const T*	ItemPtr		(ssys_t rindex) const final override CL3_GETTER;
						void		Grow		(usys_t n_items_grow, const T& item_init) final override;
						void		Insert		(ssys_t rindex, const T& item_insert) final override;
						void		Insert		(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert) final override;
						void		Insert		(ssys_t rindex, const IStaticCollection<const T>& collection) final override;

						void		Prepend		(const T& item_prepend) final override;
						void		Prepend		(const T* arr_items_prepend, usys_t n_items_prepend) final override;
						void		Prepend		(const IStaticCollection<const T>& collection) final override;
						void		Prepend		(const TList& list);

						void		Append		(const T& item_append) final override;
						void		Append		(const T* arr_items_append, usys_t n_items_append) final override;
						void		Append		(const IStaticCollection<const T>& collection) final override;
						void		Append		(const TList& list);

						TList<T>	SliceSL		(ssys_t rindex_start, usys_t n_items = (usys_t)-1) const;
						TList<T>	SliceSE		(ssys_t rindex_start, ssys_t rindex_end) const;

						//	from TList
						CLASS		TList		();
						explicit CLASS		TList		(T* arr_items, usys_t n_items, bool claim);
						explicit CLASS		TList		(const T* arr_items, usys_t n_items);
						explicit CLASS		TList		(const IStaticCollection<const T>&);
						explicit CLASS		TList		(const TList&);
						explicit CLASS		TList		(TList&&);
						explicit CLASS		TList		(serialization::IDeserializer&);
								 CLASS		TList		(std::initializer_list<T>);
						virtual		~TList		();
				};

				template<class T>
				class	CL3PUBT	TList : public virtual IList<T>, public virtual TList<const T>
				{
					protected:
						using TList<const T>::arr_items;
						using TList<const T>::n_items_current;
						using TList<const T>::n_items_prealloc;
						using TList<const T>::on_change;
						using TList<const T>::Prealloc;

					public:
						using IStaticCollection<const T>::Count;
						using stream::IOut<T>::Write;
						using array::IArray<T>::Write;
						using TList<const T>::OnChange;
						using TList<const T>::CreateStaticIterator;
						using TList<const T>::CountMin;
						using TList<const T>::CountMax;
						using TList<const T>::CreateDynamicIterator;
						using TList<const T>::Add;
						using TList<const T>::Read;
						using TList<const T>::operator[];
						using TList<const T>::ItemPtr;
						using TList<const T>::Grow;
						using TList<const T>::Insert;
						using TList<const T>::Append;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<T> >		CreateStaticIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IDynamicCollection
						system::memory::TUniquePtr<IDynamicIterator<T> >	CreateDynamicIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IArray<T>
						//	Read is taken from IArray<T> as-is, we only override Write here
						usys_t		Write		(uoff_t index, const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from IIn<T>
						using stream::IIn<T>::WriteOut;
						usys_t		Read		(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
						usys_t		Remaining	() const final override CL3_GETTER;
						usys_t		WriteOut	(stream::IOut<T>& os) final override CL3_WARN_UNUSED_RESULT;
						usys_t		WriteOut	(stream::IOut<T>& os, usys_t n_items_wo_max, usys_t n_items_wo_min) final override CL3_WARN_UNUSED_RESULT;

						//	from IList
						TList<T>&	operator=	(const IStaticCollection<T>& rhs) final override;
						TList<T>&	operator=	(IStaticCollection<T>&& rhs) final override;
 						TList<T>&	operator=	(std::initializer_list<T>) final override;
						T&			operator[]	(ssys_t rindex) final override CL3_GETTER;

						T*			Claim		() final override;
						T*			ItemPtr		(ssys_t rindex) final override CL3_GETTER;

						bool		Remove		(const T& item_remove) final override;
						void		Remove		(ssys_t rindex, usys_t n_items_remove) final override;
						void		Clear		() final override;
						void		Cut			(usys_t n_head, usys_t n_tail) final override;
						void		Shrink		(usys_t n_items_shrink) final override;
						void		Count		(usys_t new_count, const T& item_init = T()) final override CL3_SETTER;

						//	from TList
						TList<T>&	operator=	(const TList<T>& rhs);
						TList<T>&	operator=	(TList<T>&& rhs);

						CLASS		TList		();
						CLASS		TList		(T* arr_items, usys_t n_items, bool claim);
						CLASS		TList		(const T* arr_items, usys_t n_items);
						CLASS		TList		(const IStaticCollection<const T>&);
						CLASS		TList		(const TList&);
						CLASS		TList		(TList&&);
 						CLASS		TList		(std::initializer_list<T>);
						virtual		~TList		();
				};

				/**************************************************************/

				//	from IStaticIterator<const T>
				template<class T>
				bool	TIterator<const T>::IsValid	() const
				{
					this->FixIndex();
					return this->index != INDEX_TAIL && this->index != INDEX_HEAD;
				}

				template<class T>
				const T&TIterator<const T>::Item		() const
				{
					this->FixIndex();
					CL3_CLASS_ERROR(this->index == INDEX_TAIL || this->index == INDEX_HEAD, TIndexOutOfBoundsException, this->index, this->list->Count());
					return (*list)[this->index];
				}

				template<class T>
				void	TIterator<const T>::MoveHead	()
				{
					this->index = INDEX_HEAD;
				}

				template<class T>
				void	TIterator<const T>::MoveTail	()
				{
					this->index = INDEX_TAIL;
				}

				template<class T>
				bool	TIterator<const T>::MoveFirst	()
				{
					if(this->list->Count())
					{
						this->index = 0;
						return true;
					}
					else
					{
						this->index = INDEX_HEAD;
						return false;
					}
				}

				template<class T>
				bool	TIterator<const T>::MoveLast	()
				{
					if(this->list->Count())
					{
						this->index = this->list->Count() - 1;
						return true;
					}
					else
					{
						this->index = INDEX_TAIL;
						return false;
					}
				}

				template<class T>
				bool	TIterator<const T>::MoveNext	()
				{
					switch(this->index)
					{
						case INDEX_HEAD:
							return MoveFirst();
						case INDEX_TAIL:
							return false;
						default:
							if(this->index + 1 < this->list->Count())
							{
								this->index++;
								return true;
							}
							else
							{
								this->index = INDEX_TAIL;
								return false;
							}
					}
				}

				template<class T>
				bool	TIterator<const T>::MovePrev	()
				{
					switch(this->index)
					{
						case INDEX_HEAD:
							return false;
						case INDEX_TAIL:
							return MoveLast();
						default:
							if(this->index > 0 && this->list->Count())
							{
								this->index--;
								return true;
							}
							else
							{
								this->index = INDEX_HEAD;
								return false;
							}
					}
				}

				//	from IStaticIterator<T>
				template<class T>
				T&		TIterator<T>::Item		()
				{
					this->FixIndex();
					CL3_CLASS_ERROR(this->index == INDEX_TAIL || this->index == INDEX_HEAD, TIndexOutOfBoundsException, this->index, this->list->Count());
					return const_cast<T&>((*list)[this->index]);
				}

				//	from IDynamicIterator<T>
				template<class T>
				void	TIterator<const T>::Insert	(const T& /*item_insert*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TIterator<const T>::Insert	(const T* /*arr_items_insert*/, usys_t /*n_items_insert*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IOut<T>
				template<class T>

				usys_t	TIterator<T>::Write			(const T* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIn<T>
				template<class T>
				usys_t	TIterator<const T>::Read		(T* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIterator
				template<class T>
				usys_t	TIterator<const T>::Index		() const
				{
					this->FixIndex();
					return this->index;
				}

				template<class T>
				void	TIterator<const T>::Index		(usys_t new_index)
				{
					switch(new_index)
					{
						case INDEX_HEAD:
						case INDEX_TAIL:
							this->index = new_index;
							break;
						default:
							CL3_CLASS_ERROR(new_index > this->list->Count(), TIndexOutOfBoundsException, new_index, this->list->Count());
							this->index = new_index;
							this->FixIndex();
							break;
					}
				}

				//	from TIterator
				template<class T>
				void	TIterator<const T>::FixIndex	() const
				{
					if(this->index >= this->list->Count())
						this->index = INDEX_TAIL;
				}

				template<class T>
				CLASS	TIterator<const T>::TIterator	(TList<const T>* list, usys_t index) : list(list)
				{
					this->Index(index);
				}

				template<class T>
				void	TIterator<T>::Remove	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				CLASS	TIterator<T>::TIterator	(TList<T>* list, usys_t index) : TIterator<const T>(list, index)
				{
				}

				/**************************************************************/

				template<class T>
				void	TList<const T>::Prealloc	(usys_t n_items_prealloc_min)
				{
					const usys_t n_items_prealloc_ideal = (n_items_current >> 8) + (32 / sizeof(T));

					if((n_items_prealloc < n_items_prealloc_min) || (n_items_prealloc - n_items_prealloc_min > 2 * n_items_prealloc_ideal))
					{
						T* arr_items_new = NULL;
// 						try { arr_items_new = (T*)system::memory::Realloc(arr_items, (n_items_current + n_items_prealloc_ideal + n_items_prealloc_min) * sizeof(T), NULL, true); }
// 						catch(const error::TException&) {}	//	FIXME, should be TBadAllocException

						if(!arr_items_new)
						{
							arr_items_new = (T*)system::memory::Alloc((n_items_current + n_items_prealloc_ideal + n_items_prealloc_min) * sizeof(T), NULL);

							usys_t n_items_copied = 0;
							try
							{
								for(n_items_copied = 0; n_items_copied < n_items_current; n_items_copied++)
									new (arr_items_new + n_items_copied) T(system::def::move(arr_items[n_items_copied]));
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

				template<class T>
				bool	TList<const T>::IsSorted	() const
				{
					return this->is_sorted;
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<const T> >
						TList<const T>::CreateStaticIterator	() const
				{
					return system::memory::MakeUniquePtr<IStaticIterator<const T> >(new TIterator<const T>(const_cast<TList*>(this), n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				usys_t	TList<const T>::Count		() const
				{
					return n_items_current;
				}

				template<class T>
				bool	TList<const T>::CountMin	(usys_t count_min) const
				{
					return n_items_current >= count_min;
				}

				template<class T>
				bool	TList<const T>::CountMax	(usys_t count_max) const
				{
					return n_items_current <= count_max;
				}

				template<class T>
				bool	TList<const T>::Contains	(const T& item) const
				{
					return this->IndexOf(item) != (usys_t)-1;
				}

				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<const T> >	TList<const T>::CreateDynamicIterator() const
				{
					return system::memory::MakeUniquePtr<IDynamicIterator<const T> >(new TIterator<const T>(const_cast<TList*>(this), n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				void	TList<const T>::Add		(const T& item_insert)
				{
					Append(item_insert);
				}

				template<class T>
				void	TList<const T>::Add		(const T* arr_items_add, usys_t n_items_add)
				{
					Append(arr_items_add, n_items_add);
				}

				template<class T>
				void	TList<const T>::Add		(const IStaticCollection<const T>& collection)
				{
					Append(collection);
				}

				//	from IOut
				template<class T>
				usys_t	TList<const T>::Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t)
				{
					Append(arr_items_write, n_items_write_max);
					return n_items_write_max;
				}

				//	from IList
				template<class T>
				const T&	TList<const T>::operator[]	(ssys_t rindex) const
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items[index];
				}

				template<class T>
				const T*	TList<const T>::ItemPtr	(ssys_t rindex) const
				{
					const usys_t index = this->AbsIndex(rindex);
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
				void		TList<const T>::Grow		(usys_t n_items_grow, const T& item_init)
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
				void		TList<const T>::Insert	(ssys_t rindex, const T& item_insert)
				{
					Insert(rindex, &item_insert, 1);
				}

				template<class T>
				void		TList<const T>::Insert	(ssys_t rindex, const T* arr_items_insert, usys_t n_items_insert)
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index > n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					const usys_t n_items_relocate = n_items_current - index;

					Prealloc(n_items_insert);

					for(usys_t i = 1; i <= n_items_relocate; i++)
					{
						const usys_t idx_from = index + n_items_relocate - i;
						const usys_t idx_to   = n_items_current + n_items_insert - i;
						new (arr_items + idx_to) T(system::def::move(arr_items[idx_from]));
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
				void		TList<const T>::Insert	(ssys_t rindex, const IStaticCollection<const T>& collection)
				{
					const TList<const T>* list = dynamic_cast<const TList<const T>*>(&collection);
					if(list)
						Insert(rindex, list->arr_items, list->n_items_current);
					else
					{
						usys_t index = this->AbsIndex(rindex);
						CL3_CLASS_ERROR(index > n_items_current, TIndexOutOfBoundsException, index, n_items_current);
						system::memory::TUniquePtr<IStaticIterator<const T> > it = collection.CreateStaticIterator();
						it->MoveHead();
						while(it->MoveNext())
							Insert(index++, it->Item());
					}
				}

				template<class T>
				void		TList<const T>::Prepend	(const T& item_prepend)
				{
					Prepend(&item_prepend, 1);
				}

				template<class T>
				void		TList<const T>::Prepend	(const T* arr_items_prepend, usys_t n_items_prepend)
				{
					this->Insert(0, arr_items_prepend, n_items_prepend);
				}

				template<class T>
				void		TList<const T>::Prepend	(const IStaticCollection<const T>& collection)
				{
					this->Insert(0, collection);
				}

				template<class T>
				void		TList<const T>::Prepend	(const TList& list)
				{
					Prepend(list.arr_items, list.n_items_current);
				}

				template<class T>
				void		TList<const T>::Append	(const T& item_append)
				{
					Append(&item_append, 1);
				}

				template<class T>
				void		TList<const T>::Append	(const T* arr_items_append, usys_t n_items_append)
				{
					Prealloc(n_items_append);

					for(usys_t i = 0; i < n_items_append; i++)
						new (arr_items + n_items_current + i) T(arr_items_append[i]);

					n_items_current  += n_items_append;
					n_items_prealloc -= n_items_append;
				}

				template<class T>
				void		TList<const T>::Append	(const IStaticCollection<const T>& collection)
				{
					const TList<const T>* list = dynamic_cast<const TList<const T>*>(&collection);
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
				void		TList<const T>::Append	(const TList& list)
				{
					Append(list.arr_items, list.n_items_current);
				}

				template<class T>
				TList<T>	TList<const T>::SliceSL	(ssys_t rindex_start, usys_t n_items) const
				{
					const usys_t idx_start = this->AbsIndex(rindex_start);
					if(n_items == (usys_t)-1)
						n_items = this->n_items_current - idx_start;

					TList r;
					r.Append(this->arr_items + idx_start, n_items);
					return r;
				}

				template<class T>
				TList<T>	TList<const T>::SliceSE	(ssys_t rindex_start, ssys_t rindex_end) const
				{
					const usys_t idx_start = this->AbsIndex(rindex_start);
					const usys_t idx_end = this->AbsIndex(rindex_end);
					TList r;
					if(idx_start < idx_end)
						r.Append(this->arr_items + idx_start, idx_end - idx_start);
					else if(idx_end > idx_start)
						for(usys_t i = idx_end; i != idx_start; i--)
							r.Append(this->arr_items[i]);
					return r;
				}

				template<class T>
				void		TList<T>::Cut		(usys_t n_head, usys_t n_tail)
				{
					this->Remove(this->n_items_current - n_tail, n_tail);
					this->Remove(0, n_head);
				}

				template<class T>
				void	TList<const T>::InternalClear	()
				{
					if(this->on_action.HasReceivers())
					{
						TIterator<const T> it(this, 0);
						TOnActionData<const T> data(ACTION_REMOVE, &it);
						this->on_action.Raise(*this, data);
					}

					for(usys_t i = 0; i < n_items_current; i++)
						arr_items[i].~T();
					n_items_prealloc += n_items_current;
					n_items_current = 0;
					system::memory::Free(arr_items);
					arr_items = NULL;
					n_items_prealloc = 0;
				}

				//	from TList<T>
				template<class T>
				CLASS		TList<const T>::TList		() : arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(false)
				{
				}

				template<class T>
				CLASS		TList<const T>::TList		(T* arr_items, usys_t n_items, bool claim) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(false)
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
				CLASS		TList<const T>::TList		(const T* arr_items, usys_t n_items) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(false)
				{
					Prealloc(n_items);
					Append(arr_items, n_items);
				}

				template<class T>
				CLASS		TList<const T>::TList		(const IStaticCollection<const T>& collection) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(false)
				{
					Append(collection);
				}

				template<class T>
				CLASS		TList<const T>::TList		(const TList& list) : event::IObservable(), arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(list.is_sorted)
				{
					Prealloc(list.n_items_current);
					Append(list.arr_items, list.n_items_current);
				}

				template<class T>
				CLASS		TList<const T>::TList		(TList&& list) : arr_items(list.arr_items), n_items_current(list.n_items_current), n_items_prealloc(list.n_items_prealloc), is_sorted(list.is_sorted)
				{
					list.arr_items = NULL;
					list.n_items_current = 0;
					list.n_items_prealloc = 0;
				}

				template<class T>
				CLASS		TList<const T>::TList		(std::initializer_list<T> l) : arr_items(NULL), n_items_current(0), n_items_prealloc(0), is_sorted(false)
				{
					this->Prealloc(l.size());
					for(usys_t i = 0; i < l.size(); i++)
						this->Append(l.begin()[i]);
				}

				template<class T>
				CLASS		TList<const T>::~TList	()
				{
					InternalClear();
				}

				/**************************************************************/

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >
						TList<T>::CreateStaticIterator	()
				{
					return system::memory::MakeUniquePtr<IStaticIterator<T> >(new TIterator<T>(this, n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				system::memory::TUniquePtr<IDynamicIterator<T> >
						TList<T>::CreateDynamicIterator	()
				{
					return system::memory::MakeUniquePtr<IDynamicIterator<T> >(new TIterator<T>(this, n_items_current > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				usys_t	TList<T>::Write		(uoff_t /*index*/, const T* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				TList<T>&	TList<T>::operator=	(const IStaticCollection<T>& rhs)
				{
					const TList<const T>* list = dynamic_cast<const TList<const T>*>(&rhs);
					if(list)
						this->operator=(*list);
// 						(*this) = *list;
					else
					{
						Clear();
						Append(rhs);
					}
					return *this;
				}

				template<class T>
				TList<T>&	TList<T>::operator=	(IStaticCollection<T>&& rhs)
				{
					TList<const T>* list = dynamic_cast<TList<const T>*>(&rhs);
					if(list)
						(*this) = system::def::move(*list);
					else
					{
						Clear();
						Append(rhs);
					}
					return *this;
				}

				template<class T>
				TList<T>&	TList<T>::operator=	(std::initializer_list<T> l)
				{
					this->Clear();
					this->Prealloc(l.size());
					for(usys_t i = 0; i < l.size(); i++)
						this->Append(l.begin()[i]);
					return *this;
				}

				template<class T>
				T&		TList<T>::operator[](ssys_t rindex)
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items[index];
				}

				template<class T>
				T*		TList<T>::Claim		()
				{
					T* tmp = this->arr_items;
					this->arr_items = NULL;
					this->n_items_current = 0;
					return tmp;
				}

				template<class T>
				T*		TList<T>::ItemPtr	(ssys_t rindex)
				{
					if(rindex == 0 && n_items_current == 0)
						return NULL;

					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items_current, TIndexOutOfBoundsException, index, n_items_current);
					return arr_items + index;
				}

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
				bool	TList<T>::Remove	(const T& item_remove)
				{
					usys_t index;
					if(&item_remove >= arr_items && &item_remove < arr_items + n_items_current)
						index = &item_remove - arr_items;
					else
					{
						index = this->IndexOf(item_remove);
						if(index == (usys_t)-1)
							return false;
					}

					Remove(index, 1);
					return true;
				}

				template<class T>
				void		TList<T>::Remove	(ssys_t rindex, usys_t n_items_remove)
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index + n_items_remove > n_items_current, TIndexOutOfBoundsException, index + n_items_remove, n_items_current);

					const usys_t n_items_after = n_items_current - index - n_items_remove;

					for(usys_t i = 0; i < n_items_after; i++)
						arr_items[index + i] = system::def::move(arr_items[index + i + n_items_remove]);

					Shrink(n_items_remove);
				}

				template<class T>
				void		TList<T>::Clear		()
				{
					TList<const T>::InternalClear();
				}

				//	from IIn
				template<class T>
				usys_t	TList<T>::Read		(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					if(n_items_read_min >= n_items_read_max) n_items_read_min = n_items_read_max;
					CL3_CLASS_ERROR(n_items_read_min > n_items_current, stream::TSourceDryException, n_items_read_max, n_items_read_min, 0, n_items_current);
					const usys_t n_items_read = CL3_MIN(n_items_read_max, n_items_current);

					for(usys_t i = 0; i < n_items_read; i++)
						arr_items_read[i] = system::def::move(arr_items[i]);

					Remove(0, n_items_read);
					return n_items_read;
				}

				template<class T>
				usys_t	TList<T>::Remaining	() const
				{
					return this->Count();
				}

				template<class T>
				usys_t	TList<T>::WriteOut	(stream::IOut<T>& os)
				{
					const usys_t n_items_wo = os.Write(this->arr_items, this->n_items_current, 0);
					CL3_CLASS_LOGIC_ERROR(n_items_wo > this->n_items_current);
					this->Cut(n_items_wo, 0);
					return n_items_wo;
				}

				template<class T>
				usys_t	TList<T>::WriteOut	(stream::IOut<T>& os, usys_t n_items_wo_max, usys_t n_items_wo_min)
				{
					CL3_CLASS_ERROR(this->n_items_current < n_items_wo_min, stream::TSourceDryException, n_items_wo_max, n_items_wo_min, 0, this->n_items_current);
					if(n_items_wo_max > this->n_items_current)
						n_items_wo_max = this->n_items_current;

					const usys_t n_items_wo = os.Write(this->arr_items, n_items_wo_max, n_items_wo_min);
					CL3_CLASS_LOGIC_ERROR(n_items_wo < n_items_wo_min || n_items_wo > n_items_wo_max);
					this->Cut(n_items_wo, 0);

					return n_items_wo;
				}

				template<class T>
				CLASS		TList<T>::TList		() : TList<const T>()
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(T* arr_items, usys_t n_items, bool claim) : TList<const T>(arr_items, n_items, claim)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(const T* arr_items, usys_t n_items) : TList<const T>(arr_items, n_items)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(const IStaticCollection<const T>& other) : TList<const T>(other)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(const TList& other) : event::IObservable(), TList<const T>(other)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(TList&& other) : TList<const T>(other)
				{
				}

				template<class T>
				CLASS		TList<T>::TList		(std::initializer_list<T> l) : TList()
				{
					this->Prealloc(l.size());
					for(usys_t i = 0; i < l.size(); i++)
						this->Append(l.begin()[i]);
				}

				template<class T>
				CLASS		TList<T>::~TList	()
				{
				}
			}
		}
	}
}

#endif
