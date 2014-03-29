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

#ifndef	_include_cl3_core_io_collection_hpp_
#define	_include_cl3_core_io_collection_hpp_

#include "io_collection_bitmask.hpp"
#include "io_stream-base.hpp"
#include "system_memory.hpp"
#include "event.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	list
			{
				template<class T>
				struct	IList;

				template<class T>
				class	TList;
			}

			template<class T>
			struct	IMatcher
			{
				virtual	bool	Match	(const T& item_match) const = 0;	//	compares "item_match" against the ciriteria and returns true if the item matches, false otherwise
				virtual	void	Match	(const T* arr_items_match, bitmask::TBitmask& bm_result) const = 0;	//	compares all items within the array against the criteria and sets the coresponding bits in the bitmask to 1 if the item matches or to 0 if it does not match, the size of the array is taken from the bitmask
				virtual	usys_t	Match	(const T* arr_items_match, usys_t n_items_match, list::TList<usys_t>& ls_result, usys_t offset = 0) const = 0;	//	adds the indices of the matching items to "ls_result" after adding "offset" to the index, the function retuns the number of items added/matched
			};

			/*template<class T>
			struct	IComparator : IMatcher<T>
			{
				virtual	int		Compare	(const T& item_compare) const = 0;	//	returns 0 if item_compare matches the reference, returns a value < 0 if the item is less then the reference, returns a value > 0 if the item is bigger than the reference
				virtual	void	Compare	(const T* arr_items_compare, s8* arr_result, usys_t n_items_compare) const = 0;	//	returns 0 if item_compare matches the reference, returns a value < 0 if the item is less then the reference, returns a value > 0 if the item is bigger than the reference

				virtual	bool	operator>	(const T& item_compare) const { return Compare(item_compare) >  0; }
				virtual	bool	operator<	(const T& item_compare) const { return Compare(item_compare) <  0; }
				virtual	bool	operator>=	(const T& item_compare) const { return Compare(item_compare) >= 0; }
				virtual	bool	operator<=	(const T& item_compare) const { return Compare(item_compare) <= 0; }
				virtual	bool	operator==	(const T& item_compare) const { return Match(item_compare); }

				virtual	bool	Match	(const T& item_match) const { return Compare(item_match) == 0; }

				virtual	void	Match	(const T* arr_items_match, bitmask::TBitmask& bm_result) const
				{
					const usys_t n = bm_result.Count();
					for(usys_t i = 0; i < n; ++i)
						bm_result.Bit(i, Match(arr_items_match[i]));
				}

				virtual	usys_t	Match	(const T* arr_items_match, usys_t n_items_match, list::TList<usys_t>& ls_result, usys_t offset = 0) const
				{
					usys_t n = 0;
					for(usys_t i = 0; i < n; i++)
						if(Match(arr_items_match[i]))
						{
							ls_result.Add(i + offset);
							n++;
						}
					return n;
				}
			};*/

			template<class T>	struct	IStaticIterator;
			template<class T>	struct	IDynamicIterator;
			template<class T>	struct	IStaticCollection;
			template<class T>	struct	IDynamicCollection;

			template<class T>
			struct	TOnChangeData
			{
				enum	EChange
				{
					CHANGE_ADD,
					CHANGE_REMOVE
				};

				EChange change;
				const IStaticIterator<T>* iterator;
			};

			template<class T>
			struct	IStaticIterator<const T> : stream::IIn<T>
			{
				virtual	const IStaticCollection<T>&	Collection	() const = 0;
				virtual	bool	FindNext	(const IMatcher<T>& matcher) = 0;	//	forward searches thru the collection for an item that matches starting with the next item, if an item was found the function returns thruw and place sthe iterator on that item, otherwise false is returned and the iterator is places on tail
				virtual	bool	FindPrev	(const IMatcher<T>& matcher) = 0;	//	backward searches thru the collection for an item that matches starting with the previous item,  if an item was found the function returns thruw and place sthe iterator on that item, otherwise false is returned and the iterator is places on head
				virtual	bool	IsValid		() const GETTER = 0;	//	returns whether the iterator is placed on a valid item (not on head or tail)
				virtual	const T&	Item	() const GETTER = 0;	//	returns the current item (throws an exception if the iterator is on head or tail)
				virtual	void	MoveHead	() = 0;	//	move before the first item
				virtual	void	MoveTail	() = 0;	//	move after last item
				virtual	bool	MoveFirst	() = 0;	//	move to the first item (returns false if there is no first item / the collection is empty)
				virtual	bool	MoveLast	() = 0;	//	move to the last item (returns false if there is no last item / the collection is empty)
				virtual	bool	MoveNext	() = 0;	//	move to the next item (returns false if there is no next item and places the iterator on tail)
				virtual	bool	MovePrev	() = 0;	//	move to the previous item (returns false if there is no previous item and places the iterator on head)
			};

			template<class T>
			struct	IStaticIterator : virtual IStaticIterator<const T>, stream::IOut<T>
			{
				using IStaticIterator<const T>::Item;
				virtual	GETTER	T&	Item	() = 0;	//	returns the current item (throws an exception if the iterator is on head or tail)
			};

			template<class T>
			struct	IDynamicIterator : IStaticIterator<T>
			{
				virtual	void	Insert	(const T& item_insert) = 0;	//	inserts an item before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to it
				virtual	void	Insert	(const T* arr_items_insert, usys_t n_items_insert) = 0;	//	inserts items before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to the first of the inserted items (does nothing if the array is empty)
				virtual	void	Remove	() = 0;	//	removes the current item and moves to the first item after the removed (if the collection supports ordering, or at a implementation choosen position otherwise - avoiding head/tail until the last item gets removed)
			};

			template<class T>
			struct	IStaticCollection : event::IObservable< IStaticCollection<T>, TOnChangeData<T> >
			{
				virtual	system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT = 0;
				virtual	system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT = 0;

				virtual	GETTER	usys_t	Count		() const = 0;
				virtual	GETTER	bool	CountMin	(usys_t count_min) const { return Count() >= count_min; }
				virtual	GETTER	bool	CountMax	(usys_t count_max) const { return Count() <= count_max; }
			};

			template<class T>
			struct	IDynamicCollection : virtual IStaticCollection<T>, public virtual stream::IIn<T>, public virtual stream::IOut<T>
			{
				//	IIn removes read items, while IOut adds written items (no strict FIFO requirements!)

				virtual	system::memory::TUniquePtr<IDynamicIterator<T> >		CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT = 0;
				virtual	system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const CL3_WARN_UNUSED_RESULT = 0;

				virtual	void	Add		(const T& item_add) = 0;	//	inserts a single item into the collection, it is left to the implementation to determine where the new item is positioned
				virtual	void	Add		(const T* arr_items_add, usys_t n_items_add) = 0;	//	like above but for multiple items at once
				virtual	void	Add		(const IStaticCollection<T>& collection) = 0;	//	inserts another collection into this collection, it is left to the implementation to determine where the new items are positioned
				virtual	void	Remove	(const T* item_remove) = 0;	//	removes the specified item from the collection, the pointer is free to point to any valid item - the item needs not to be a member of the collection, if however so, then exactly the specified item is removed, if not, one item which compares equal to the specified item is removed - if multiple items compare equal to the specified item, the implementation is free to choose one among them
			};

			/*template<class T>
			class	TSelection : public IStaticCollection<T>
			{
				protected:
					IStaticCollection<T>* collection;
					IMatcher<T>* matcher;

				public:
					template<class TT>	class	TIterator;

					template<class TT>
					class	TIterator<const TT> : public virtual IStaticIterator<const TT>
					{
						protected:
							TSelection* selection;
							IStaticIterator<TT>* iterator;

						public:
							CLASS	TIterator	(TSelection* selection) : selection(selection) {}

							system::memory::TUniquePtr<TIterator>	Clone	() const;
							bool	FindNext	(const IMatcher<TT>& matcher);
							bool	FindPrev	(const IMatcher<TT>& matcher);
							bool	IsValid		() const;
							const TT&	Item	() const;
							void	MoveHead	();
							void	MoveTail	();
							bool	MoveFirst	();
							bool	MoveLast	();
							bool	MoveNext	();
							bool	MovePrev	();

							usys_t	Read	(TT* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
							usys_t	WriteOut(stream::IOut<TT>& os, usys_t n_items_wo_max, usys_t n_items_wo_min = (usys_t)-1);
					};

					template<class TT>
					class	TIterator : public TIterator<const TT>, public virtual IStaticIterator<TT>
					{
						public:
							TT&		Item	();
							usys_t	Write	(const TT* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
							usys_t	ReadIn	(stream::IIn<TT>& is, usys_t n_items_ri_max, usys_t n_items_ri_min = (usys_t)-1);
					};

					CLASS	TSelection	(IStaticCollection<T>* collection, IMatcher<T>* matcher) : collection(collection), matcher(matcher) {}

					TIterator<T>*		CreateIterator	() { return new TIterator<T>(this); }
					TIterator<const T>*	CreateIterator	() const { return new TIterator<const T>(this); }

					usys_t	Count		() const;
					bool	CountMin	(usys_t count_min) const;
					bool	CountMax	(usys_t count_max) const;
			};*/
		}
	}
}

#endif
