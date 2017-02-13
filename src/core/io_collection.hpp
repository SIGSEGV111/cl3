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
#include "system_types_typeinfo.hpp"

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
					template<typename T> struct base_type { typedef T type; };
					template<typename T> struct base_type<T&> { typedef T type; };
					template<typename T> struct base_type<T&&> { typedef T type; };
					template<typename T> struct base_type<const T> { typedef T type; };
					template<typename T> struct base_type<const T&> { typedef T type; };
					template<typename T> struct base_type<const T&&> { typedef T type; };

					template<typename T, bool use_byref> struct select_byref_byvalue;
					template<typename T> struct select_byref_byvalue<T, true>  { typedef T& type; };
					template<typename T> struct select_byref_byvalue<T, false> { typedef T  type; };
				}

				namespace list
				{
					template<typename TItem>
					class TList;
				};

				namespace array
				{
					template<typename TItem>
					class TArray;
				};

				enum class EKeyGeneration
				{
					USER,
					COLLECTION,
					OPTIONAL
				};

				template<typename TKey, typename TValue>
				struct TPair
				{
					TKey key;
					TValue value;
				};

				template<typename TKey, typename TValue>
				struct IStaticCollection;

				template<typename TKey, typename TValue, EKeyGeneration keygen>
				struct IDynamicCollection;

				/********************************************************************************/

				/*
				 * it->MoveHead();
				 * while(it->MoveNext())
				 *		it->->...
				 */
				template<typename TValue>
				struct IBasicIterator
				{
					virtual TValue* operator->() const CL3_GETTER = 0;
					virtual TValue& operator* () const CL3_GETTER = 0;

					virtual bool MoveNext () CL3_WARN_UNUSED_RESULT = 0;	//	moves to the next vald item, retuns false if there is none and remains on the current position
					virtual bool MovePrev () CL3_WARN_UNUSED_RESULT = 0;	//	moves to the previous vald item, retuns false if there is none and remains on the current position
					virtual bool MoveFirst() CL3_WARN_UNUSED_RESULT = 0;	//	moves to the last vald item, retuns false if there is none and remains on the current position
					virtual bool MoveLast () CL3_WARN_UNUSED_RESULT = 0;	//	moves to the first vald item, retuns false if there is none and remains on the current position
					virtual void MoveHead() = 0;							//	moves to the virtual invald item at the beginning of the collection, always succeedes, the next item from this position will be he first valid item (if it exists)
					virtual bool IsValid() const CL3_GETTER = 0;			//	checks if the iterator is placed on a valid item
				};

				template<typename TValue>
				struct IBasicCollection
				{
					virtual IBasicIterator<TValue>* CreateIterator() const CL3_WARN_UNUSED_RESULT = 0;
					virtual	usys_t	Count		() const CL3_GETTER = 0;
					virtual	bool	CountMin	(usys_t count_min) const CL3_GETTER { return Count() >= count_min; }
					virtual	bool	CountMax	(usys_t count_max) const CL3_GETTER { return Count() <= count_max; }
				};

				/********************************************************************************/

				template<typename TKey, typename TValue>
				struct IStaticCollection<TKey, const TValue> :
					public virtual IBasicCollection<const TValue>
				{
					typedef typename _::select_byref_byvalue<typename _::base_type<TKey>::type, (sizeof(TKey) > 2*sizeof(void*))>::type TKeyS;

					virtual const TValue& operator[](const TKeyS) const CL3_GETTER = 0;
				};

				template<typename TKey, typename TValue>
				struct IStaticCollection :
					public virtual IStaticCollection<TKey, const TValue>,
					public virtual IBasicCollection<TValue>
				{
					using typename IStaticCollection<TKey, const TValue>::TKeyS;
					using IStaticCollection<TKey, const TValue>::operator[];

					virtual TValue& operator[](const TKeyS) const CL3_GETTER = 0;
				};

				/********************************************************************************/

				template<typename TKey, typename TValue, EKeyGeneration keygen>
				struct IKeyGenerationFeatures;

				template<typename TKey, typename TValue>
				struct IKeyGenerationFeatures<TKey, TValue, EKeyGeneration::USER> :
					public virtual IStaticCollection<TKey, const TValue>
				{
					using typename IStaticCollection<TKey, const TValue>::TKeyS;

					virtual void Insert(const TKeyS, const TValue&) = 0;
					virtual void Insert(const IBasicCollection<const TPair<TKeyS, TValue> >&) = 0;
					virtual void Insert(std::initializer_list<TPair<TKey, TValue> >) = 0;
				};

				template<typename TKey, typename TValue>
				struct IKeyGenerationFeatures<TKey, TValue, EKeyGeneration::COLLECTION> :
					public virtual IStaticCollection<TKey, const TValue>
				{
					using typename IStaticCollection<TKey, const TValue>::TKeyS;

					virtual TKeyS Add(const TValue&) = 0;
					virtual array::TArray<TKey> Add(const IBasicCollection<const TValue>&) = 0;
					virtual array::TArray<TKey> Add(std::initializer_list<TValue>) = 0;
				};

				template<typename TKey, typename TValue>
				struct IKeyGenerationFeatures<TKey, TValue, EKeyGeneration::OPTIONAL> :
					public virtual IKeyGenerationFeatures<TKey, TValue, EKeyGeneration::USER>,
					public virtual IKeyGenerationFeatures<TKey, TValue, EKeyGeneration::COLLECTION>
				{
				};

				/********************************************************************************/

				template<typename TKey, typename TValue, EKeyGeneration keygen>
				struct IDynamicCollection<TKey, const TValue, keygen> :
					public virtual IStaticCollection<TKey, const TValue>,
					public virtual IKeyGenerationFeatures<TKey, const TValue, keygen>
				{
					using typename IStaticCollection<TKey, const TValue>::TKeyS;

					virtual void Remove(const TKeyS) = 0;
					virtual void Remove(const IBasicCollection<const TKey>&) = 0;
					virtual void Remove(std::initializer_list<TKey>) = 0;
				};

				template<typename TKey, typename TValue, EKeyGeneration keygen>
				struct IDynamicCollection :
					public virtual IDynamicCollection<TKey, const TValue, keygen>,
					public virtual IStaticCollection<TKey, TValue>,
					public virtual IKeyGenerationFeatures<TKey, TValue, keygen>
				{
				};

				/********************************************************************************/

				template<typename TKey, typename TValue>
				struct IIndex
				{
					IStaticCollection<TKey, TValue>* source;
					IDynamicCollection<usys_t, TKey, EKeyGeneration::COLLECTION>* index;
				};

				/********************************************************************************/

				//	linear integer key
				struct IArray;
				struct IStaticArray;
				struct IDynamicArray;
				struct IEmbeddedArray;
				struct IList;

				//	custom key
				struct IMap;

				//	pointer key
				struct IChain;
				struct IUnion;

				namespace array
				{
					template<typename TItem>
					class IArray;

					template<typename TItem>
					class IArray<const TItem> :
						public virtual IStaticCollection<usys_t, const TItem>
					{
						protected:
							TItem* arr_items;
							usys_t n_items;

						public:
							virtual IBasicIterator<const TItem>* CreateIterator() const CL3_WARN_UNUSED_RESULT = 0;
							virtual void Count(usys_t, const TItem&) = 0;
							inline usys_t Count() const CL3_GETTER { return this->n_items; }
							inline const TItem& operator[](const usys_t index) const CL3_GETTER { return this->arr_items[index]; }
					};

					template<typename TItem>
					class IArray :
						public virtual IArray<const TItem>,
						public virtual IStaticCollection<usys_t, TItem>
					{
						public:
							virtual IBasicIterator<TItem>* CreateIterator() const CL3_WARN_UNUSED_RESULT = 0;
							inline TItem& operator[](const usys_t index) const CL3_GETTER { return this->arr_items[index]; }
					};
				}
			}

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

			enum	EAction
			{
				ACTION_ADD,
				ACTION_REMOVE
			};

			template<typename T>
			struct	TOnActionData
			{
				EAction action;
				IStaticIterator<T>* iterator;

				CLASS	TOnActionData	(EAction action, IStaticIterator<T>* iterator) : action(action), iterator(iterator) {}
			};

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
			struct	IStaticCollection<const T> : virtual event::IObservable
			{
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
			class	IDynamicCollection<const T> : public virtual IStaticCollection<const T>, public virtual stream::IOut<T>, public event::TEvent< IDynamicCollection<const T>, TOnActionData<const T> >
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

			template<typename T>
			class	IDynamicCollection : public virtual IDynamicCollection<const T>, public virtual IStaticCollection<T>, public virtual stream::IIn<T>
			{
				public:
					using IDynamicCollection<const T>::CreateDynamicIterator;
					virtual	system::memory::TUniquePtr<IDynamicIterator<T> >	CreateDynamicIterator	() CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Clear	() = 0;	//	removes all items from the collection
					virtual	bool	Remove	(const T& item_remove) = 0;	//	removes the specified item from the collection, the reference is free to point to any valid item - the item needs not to be a member of the collection, if however so, then exactly the specified item is removed, if not, one item which compares equal to the specified item is removed - if multiple items compare equal to the specified item, the implementation is free to choose one among them, if no matching item is found false is returned
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
