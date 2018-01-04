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

#include "io_stream.hpp"
#include "system_types_typeinfo.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			template<typename T>
			struct	IStaticCollection;

			namespace	bitmask
			{
				class	TBitmask;
			}

			namespace	list
			{
				template<typename T>	struct	IList;
				template<typename T>	class	TList;
			}

			class CL3PUBT TIndexOutOfBoundsException : public virtual error::TException
			{
				public:
					ssys_t index;
					usys_t count;

					CL3PUBF	CLASS	TIndexOutOfBoundsException	(ssys_t index, usys_t count);
					CL3PUBF	CLASS	TIndexOutOfBoundsException	(TIndexOutOfBoundsException&&);
					virtual	CLASS	~TIndexOutOfBoundsException	();
			};

			enum	EDirection
			{
				DIRECTION_FORWARD,
				DIRECTION_BACKWARD
			};

			template<typename T>	struct	IStaticIterator;
			template<typename T>	struct	IDynamicIterator;
			template<typename T>	struct	IStaticCollection;
			template<typename T>	class	IDynamicCollection;

			/************************************************************************/

			template<typename T>
			struct	IStaticIterator<const T> : virtual stream::IIn<T>
			{
				virtual	bool	IsValid		() const CL3_GETTER = 0;	//	returns whether the iterator is placed on a valid item (not on head or tail)
				virtual	const T& Item		() const CL3_GETTER = 0;	//	returns the current item (throws an exception if the iterator is on head or tail)
				virtual	void	MoveHead	() = 0;	//	move before the first item
				virtual	void	MoveTail	() = 0;	//	move after last item
				virtual	bool	MoveFirst	() = 0;	//	move to the first item (returns false if there is no first item / the collection is empty)
				virtual	bool	MoveLast	() = 0;	//	move to the last item (returns false if there is no last item / the collection is empty)
				virtual	bool	MoveNext	() = 0;	//	move to the next item (returns false if there is no next item and places the iterator on tail)
				virtual	bool	MovePrev	() = 0;	//	move to the previous item (returns false if there is no previous item and places the iterator on head)
				virtual	CLASS	~IStaticIterator	() {}
			};

			template<typename T>
			struct	IStaticIterator : virtual IStaticIterator<const T>, virtual stream::IOut<T>
			{
				using IStaticIterator<const T>::Item;
				virtual	T&		Item				() CL3_GETTER = 0;	//	returns the current item (throws an exception if the iterator is on head or tail)
				virtual	CLASS	~IStaticIterator	() {}
			};

			/************************************************************************/

			template<typename T>
			struct	IDynamicIterator<const T> : virtual IStaticIterator<const T>
			{
				virtual	void	Insert	(const T& item_insert) = 0;	//	inserts an item before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to it
				virtual	void	Insert	(const T* arr_items_insert, usys_t n_items_insert) = 0;	//	inserts items before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to the first of the inserted items
				virtual	CLASS	~IDynamicIterator	() {}
			};

			template<typename T>
			struct	IDynamicIterator : virtual IDynamicIterator<const T>, virtual IStaticIterator<T>
			{
				virtual	void	Remove	() = 0;	//	removes the current item and moves to the first item after the removed (if the collection supports ordering, or at a implementation choosen position otherwise - avoiding head/tail until the last item gets removed)
				virtual	CLASS	~IDynamicIterator	() {}
			};

			/************************************************************************/

			template<typename T>
			struct	IStaticCollection<const T>
			{
				using value_t = T;
				using valuearg_t = typeinfo::features::type_iif<sizeof(value_t) <= sizeof(void*), value_t, value_t&>;

				virtual	system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT = 0;

				virtual	usys_t	Count		() const CL3_GETTER = 0;
				virtual	bool	CountMin	(usys_t count_min) const CL3_GETTER { return Count() >= count_min; }
				virtual	bool	CountMax	(usys_t count_max) const CL3_GETTER { return Count() <= count_max; }

				virtual	bool	Contains	(const T& item) const CL3_GETTER = 0;
			};

			template<typename T>
			struct	IStaticCollection :
				virtual IStaticCollection<const T>
			{
				using IStaticCollection<const T>::CreateStaticIterator;

				virtual	system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT = 0;

				template<typename F>
				void Apply(F f)
				{
					auto it = this->CreateStaticIterator();
					it->MoveHead();
					while(it->MoveNext())
						f(it->Item());
				}
			};

			/************************************************************************/

			template<typename T>
			class	IDynamicCollection<const T> : public virtual IStaticCollection<const T>, public virtual stream::IOut<T>
			{
				public:
					//	IIn removes read items, while IOut adds written items (no strict FIFO requirements!)
					virtual	system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const CL3_WARN_UNUSED_RESULT = 0;

					virtual	void	Add		(const T& item_add) = 0;	//	inserts a single item into the collection, it is left to the implementation to determine where the new item is positioned
					virtual	void	Add		(const T* arr_items_add, usys_t n_items_add) = 0;	//	like above but for multiple items at once
					virtual	void	Add		(const IStaticCollection<const T>& collection) = 0;	//	inserts another collection into this collection, it is left to the implementation to determine where the new items are positioned

			};

			template<typename T>
			class	IDynamicCollection : public virtual IDynamicCollection<const T>, public virtual IStaticCollection<T>, public virtual stream::IIn<T>
			{
				public:
					using IDynamicCollection<const T>::CreateDynamicIterator;
					virtual	system::memory::TUniquePtr<IDynamicIterator<T> >	CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Clear	() = 0;	//	removes all items from the collection
					virtual	bool	RemoveItem	(const T& item_remove) = 0;	//	removes the specified item from the collection, the reference is free to point to any valid item - the item needs not to be a member of the collection, if however so, then exactly the specified item is removed, if not, one item which compares equal to the specified item is removed - if multiple items compare equal to the specified item, the implementation is free to choose one among them, if no matching item is found false is returned
			};

			/************************************************************************/

// 			template<typename T, typename F>
// 			void IStaticCollection<T>::Apply(F f)
// 			{
// 				auto it = this->CreateStaticIterator();
// 				it->MoveHead();
// 				while(it->MoveNext())
// 					f(it->Item());
// 			}
		}
	}
}

#endif
