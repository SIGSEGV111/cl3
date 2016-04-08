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
#include "event.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	v2
			{
				namespace _
				{
					template<typename TValue, bool byref>
					struct MakeRef;

					template<typename TValue>
					struct MakeRef<TValue, true> { typedef TValue& TDef; typedef const TValue& TCDef; };

					template<typename TValue>
					struct MakeRef<TValue, false> { typedef TValue TDef; typedef TValue TCDef; };

					template<typename _TKey, bool>
					struct IIteratorKeyMod;

					template<typename _TKey>
					struct IIteratorKeyMod<_TKey, true> { virtual _TKey& Key() CL3_GETTER = 0; };

					template<typename _TKey>
					struct IIteratorKeyMod<_TKey, false> {};
				}

				enum class	EKeyGeneration
				{
					COLLECTION,	//	the collection implementation chooses they key for new elements
					USER		//	the user specifies the key for new elements along with the values
				};

				enum class	EValueStyle
				{
					BYREF,
					BYVALUE
				};

				struct TNoValidItemException : error::TException
				{
				};

				struct TNoSuchKeyException : error::TException
				{
				};

				template<typename TKey, typename TValue, EKeyGeneration keygen, EValueStyle retval>
				struct	IStaticCollection
				{
					typedef typename _::MakeRef<TValue, (retval == EValueStyle::BYREF)>::TDef	_TValue;	//	defines "_TValue" to be either "TValue" or "TValue&"
					typedef typename _::MakeRef<TValue, (retval == EValueStyle::BYREF)>::TCDef	_TCValue;	//	defines "_TCValue" to be either "TValue" or "const TValue&"
					typedef typename _::MakeRef<TKey, (sizeof(TKey) > sizeof(void*))>::TDef		_TKey;		//	defines "_TKey" to be either "TKey" or "TKey&"; if "TKey" is small enough to fit into a register (sizeof(void*)) then the key is passed by-value, otherwise it is passed by-ref; this purely a performance optimization

					struct IIterator : _::IIteratorKeyMod<_TKey, (keygen == EKeyGeneration::USER)>
					{
						inline _TCValue& operator*() const CL3_GETTER	{ CL3_CLASS_ERROR(!this->IsValid(), TNoValidItemException); return *this->operator->(); }
						inline _TValue& operator*() CL3_GETTER			{ CL3_CLASS_ERROR(!this->IsValid(), TNoValidItemException); return *this->operator->(); }

						inline operator _TCValue*() const CL3_GETTER	{ return this->operator->(); }
						inline operator _TValue*() CL3_GETTER			{ return this->operator->(); }

						inline bool IsValid() const CL3_GETTER			{ return this->operator->() != NULL; }

						virtual _TCValue* operator->() const CL3_GETTER = 0;	//	returns NULL if the iterator is not placed on a valid item
						virtual _TValue* operator->() CL3_GETTER = 0;			//	returns NULL if the iterator is not placed on a valid item

						virtual bool GotoFirst() const = 0;	//	goto first valid element; returns false if the collection is empty
						virtual bool GotoLast() const = 0;	//	goto last valid element; returns false if the collection is empty
						virtual bool GotoNext() const = 0;	//	goto next valid element; returns false if there is no next element
						virtual bool GotoPrev() const = 0;	//	goto previous valid element; returns false if there is no previous element

						virtual void GotoTail() const = 0;	//	"tail" always exists (it is the position after the last valid item)
						virtual void Goto(const _TKey) const = 0;	//	throws TNoSuchKeyException if the key does not already exists

						virtual const _TKey Key() const CL3_GETTER = 0;	//	returns the key for the current item
						//virtual _TKey& Key() CL3_GETTER = 0;			//	only defined if EKeyGeneration::USER

						virtual ~IIterator() {};
					};

					virtual system::memory::TUniquePtr<const IIterator> CreateStaticIterator() const CL3_WARN_UNUSED_RESULT = 0;
					virtual system::memory::TUniquePtr<IIterator> CreateStaticIterator() CL3_WARN_UNUSED_RESULT = 0;

					virtual usys_t Count() const CL3_GETTER = 0;
					virtual _TCValue operator[](const _TKey) const CL3_GETTER = 0;
					virtual _TValue operator[](const _TKey) CL3_GETTER = 0;
				};

				struct	IDynamicCollection;

				//	linear integer key
				struct	IArray;
				struct	IStaticArray;
				struct	IDynamicArray;
				struct	IEmbeddedArray;
				struct	IList;	//	IDynamicArray

				//	custom key
				struct	IMap;

				//	pointer key
				struct	IChain;

				struct	IUnion;
				struct	IIndex;
			}

			namespace	bitmask
			{
				class	CL3PUBT	TBitmask;
			}

			namespace	list
			{
				template<class T>	struct	CL3PUBT	IList;
				template<class T>	class	CL3PUBT	TList;
			}

			class	CL3PUBT	TIndexOutOfBoundsException : public virtual error::TException
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

			template<class T>	struct	IStaticIterator;
			template<class T>	struct	IDynamicIterator;
			template<class T>	struct	IStaticCollection;
			template<class T>	class	IDynamicCollection;

			enum	EAction
			{
				ACTION_ADD,
				ACTION_REMOVE
			};

			template<class T>
			struct	TOnActionData
			{
				EAction action;
				IStaticIterator<T>* iterator;

				CLASS	TOnActionData	(EAction action, IStaticIterator<T>* iterator) : action(action), iterator(iterator) {}
			};

			/************************************************************************/

			template<class T>
			struct	CL3PUBT	IStaticIterator<const T> : virtual stream::IIn<T>
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

			template<class T>
			struct	CL3PUBT	IStaticIterator : virtual IStaticIterator<const T>, virtual stream::IOut<T>
			{
				using IStaticIterator<const T>::Item;
				virtual	T&		Item				() CL3_GETTER = 0;	//	returns the current item (throws an exception if the iterator is on head or tail)
				virtual	CLASS	~IStaticIterator	() {}
			};

			/************************************************************************/

			template<class T>
			struct	CL3PUBT	IDynamicIterator<const T> : virtual IStaticIterator<const T>
			{
				virtual	void	Insert	(const T& item_insert) = 0;	//	inserts an item before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to it
				virtual	void	Insert	(const T* arr_items_insert, usys_t n_items_insert) = 0;	//	inserts items before the current item (if the collection supports ordering, or at a implementation choosen position otherwise) and moves to the first of the inserted items
				virtual	CLASS	~IDynamicIterator	() {}
			};

			template<class T>
			struct	CL3PUBT	IDynamicIterator : virtual IDynamicIterator<const T>, virtual IStaticIterator<T>
			{
				virtual	void	Remove	() = 0;	//	removes the current item and moves to the first item after the removed (if the collection supports ordering, or at a implementation choosen position otherwise - avoiding head/tail until the last item gets removed)
				virtual	CLASS	~IDynamicIterator	() {}
			};

			/************************************************************************/

			template<class T>
			struct	CL3PUBT	IStaticCollection<const T> : virtual event::IObservable
			{
				virtual	system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT = 0;

				virtual	usys_t	Count		() const CL3_GETTER = 0;
				virtual	bool	CountMin	(usys_t count_min) const CL3_GETTER { return Count() >= count_min; }
				virtual	bool	CountMax	(usys_t count_max) const CL3_GETTER { return Count() <= count_max; }

				virtual	bool	Contains	(const T& item) const CL3_GETTER = 0;
			};

			template<class T>
			struct	CL3PUBT	IStaticCollection : virtual IStaticCollection<const T>
			{
				using IStaticCollection<const T>::CreateStaticIterator;

				virtual	system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT = 0;
			};

			/************************************************************************/

			template<class T>
			class	CL3PUBT	IDynamicCollection<const T> : public virtual IStaticCollection<const T>, public virtual stream::IOut<T>, public event::TEvent< IDynamicCollection<const T>, TOnActionData<const T> >
			{
				public:
					typedef event::TEvent< const IDynamicCollection<const T>, const TOnActionData<const T>& > TOnActionEvent;

				protected:
					TOnActionEvent on_action;

				public:
					inline	const TOnActionEvent&	OnAction	() const CL3_GETTER { return this->on_action; }

					//	IIn removes read items, while IOut adds written items (no strict FIFO requirements!)
					virtual	system::memory::TUniquePtr<IDynamicIterator<const T> >	CreateDynamicIterator	() const CL3_WARN_UNUSED_RESULT = 0;

					virtual	void	Add		(const T& item_add) = 0;	//	inserts a single item into the collection, it is left to the implementation to determine where the new item is positioned
					virtual	void	Add		(const T* arr_items_add, usys_t n_items_add) = 0;	//	like above but for multiple items at once
					virtual	void	Add		(const IStaticCollection<const T>& collection) = 0;	//	inserts another collection into this collection, it is left to the implementation to determine where the new items are positioned

			};

			template<class T>
			class	CL3PUBT	IDynamicCollection : public virtual IDynamicCollection<const T>, public virtual IStaticCollection<T>, public virtual stream::IIn<T>
			{
				public:
					using IDynamicCollection<const T>::CreateDynamicIterator;
					virtual	system::memory::TUniquePtr<IDynamicIterator<T> >	CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Clear	() = 0;	//	removes all items from the collection
					virtual	bool	Remove	(const T& item_remove) = 0;	//	removes the specified item from the collection, the reference is free to point to any valid item - the item needs not to be a member of the collection, if however so, then exactly the specified item is removed, if not, one item which compares equal to the specified item is removed - if multiple items compare equal to the specified item, the implementation is free to choose one among them, if no matching item is found false is returned
			};
		}
	}
}

#endif
