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

#ifndef	_include_cl3_core_io_collection_array_hpp_
#define	_include_cl3_core_io_collection_array_hpp_

#include <utility>

#include "io_collection.hpp"
#include "system_types_typeinfo.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	array
			{
				template<class T> struct IArray;
				template<class T> class TArray;
				template<class T> class TIterator;

				/************************************************************************/

				namespace _
				{
					template<class T, bool is_comparable_biggerthan>
					struct TIndexOf_impl;

					template<class T>
					struct TIndexOf_impl<T, true>
					{
						static	usys_t	IndexOf	(const T* arr_items, usys_t n_items, const T& item_find, usys_t idx_start, EDirection dir, bool is_sorted) CL3_GETTER;
					};

					template<class T>
					struct TIndexOf_impl<T, false>
					{
						static	usys_t	IndexOf	(const T* arr_items, usys_t n_items, const T& item_find, usys_t idx_start, EDirection dir, bool is_sorted) CL3_GETTER;
					};

					template<class T, bool is_assignable>
					struct TRead_impl;

					template<class T>
					struct TRead_impl<T, true>
					{
						virtual	usys_t		Read		(uoff_t index, T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;
						inline	void		Read		(uoff_t index, T* arr_items_write, usys_t n_items_write);
					};

					template<class T>
					struct TRead_impl<T, false>
					{
					};

					template<class T, bool is_assignable>
					struct TWrite_impl;

					template<class T>
					struct TWrite_impl<T, true>
					{
						virtual	usys_t	Write		(uoff_t index, const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;
						inline	void	Write		(uoff_t index, const T* arr_items_write, usys_t n_items_write);
					};

					template<class T>
					struct TWrite_impl<T, false>
					{
					};
				}

				/************************************************************************/

				template<class T>
				class	CL3PUBT	TIterator<const T> : public virtual IStaticIterator<const T>
				{
					protected:
						static const usys_t INDEX_TAIL = (usys_t)-1;
						static const usys_t INDEX_HEAD = (usys_t)-2;

						const TArray<const T>* array;
						mutable usys_t index;

						void	FixIndex	() const;

					public:
						//	from IStaticIterator<const T>
						bool	IsValid		() const CL3_GETTER;
						const T&Item		() const CL3_GETTER;
						void	MoveHead	();
						void	MoveTail	();
						bool	MoveFirst	();
						bool	MoveLast	();
						bool	MoveNext	();
						bool	MovePrev	();

						//	from IIn<T>
						usys_t	Read		(T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;

						//	from IStaticIterator
						usys_t	Index		() const CL3_GETTER;
						void	Index		(usys_t new_index) CL3_SETTER;

						//	from TIterator
						CLASS	TIterator	(const TArray<const T>* array, usys_t index);
				};

				template<class T>
				class	CL3PUBT	TIterator : public virtual TIterator<const T>, public virtual IStaticIterator<T>
				{
					public:
						using TIterator<const T>::Item;

						//	from IStaticIterator<T>
						T&		Item		() CL3_GETTER;

						//	from IOut<T>
						usys_t	Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;

						//	from TIterator
						CLASS	TIterator	(const TArray<T>* array, usys_t index);
				};

				/************************************************************************/

				template<class T>
				struct	CL3PUBT	IArray<const T> : virtual IStaticCollection<const T>, public _::TRead_impl<T, system::types::typeinfo::features::is_assignable<T>::value>
				{
					usys_t	AbsIndex	(ssys_t rindex) const CL3_GETTER;

					virtual	bool		IsSorted	() const CL3_GETTER = 0;

					virtual	const T&	operator[]	(ssys_t index) const CL3_GETTER = 0;
					virtual	const T*	ItemPtr		(ssys_t index) const CL3_GETTER = 0;

					virtual	usys_t		IndexOf		(const T& item_find, usys_t idx_start = 0, EDirection dir = DIRECTION_FORWARD) const CL3_GETTER;
					virtual	const T*	Find		(const T& item_find, usys_t idx_start = 0, EDirection dir = DIRECTION_FORWARD) const CL3_GETTER;
				};

				template<class T>
				struct	CL3PUBT	IArray : virtual IArray<const T>,  virtual IStaticCollection<T>, public _::TWrite_impl<T, system::types::typeinfo::features::is_assignable<T>::value>
				{
					using array::IArray<const T>::operator[];
					using array::IArray<const T>::ItemPtr;
					using array::IArray<const T>::Find;

					virtual	T*		Claim		() = 0;

					virtual	T&		operator[]	(ssys_t index) CL3_GETTER = 0;
					virtual	T*		ItemPtr		(ssys_t index) CL3_GETTER = 0;

					virtual	T*		Find		(const T& item_find, usys_t idx_start = 0, EDirection dir = DIRECTION_FORWARD) CL3_GETTER;
				};

				/************************************************************************/

				template<class T>
				class	CL3PUBT	TArray<const T> : public virtual IArray<const T>
				{
					friend class TIterator<T>;
					friend class TIterator<const T>;
					protected:
						T* arr_items;
						usys_t n_items;
						bool b_claim;
						bool is_sorted;

					public:
						//	from IArray
						bool	IsSorted	() const final override CL3_GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const final override CL3_WARN_UNUSED_RESULT;
						usys_t	Count		() const final override CL3_GETTER;
						bool	CountMin	(usys_t count_min) const final override CL3_GETTER;
						bool	CountMax	(usys_t count_max) const final override CL3_GETTER;
						bool	Contains	(const T& item) const final override CL3_GETTER;

						//	from IStaticCollection<const T>
						TArray&		operator=	(const IStaticCollection<const T>& rhs);
						TArray&		operator=	(IStaticCollection<const T>&& rhs);

						const T&	operator[]	(ssys_t rindex) const final override CL3_GETTER;
						const T*	ItemPtr		(ssys_t rindex) const final override CL3_GETTER;

						CLASS		TArray		(const T* arr_items, usys_t n_items, bool b_claim = true);
						CLASS		TArray		(const TArray&);
						CLASS		TArray		(TArray&&);
						CLASS		~TArray		();
				};

				template<class T>
				class	CL3PUBT	TArray : public virtual IArray<T>, public virtual TArray<const T>
				{
					friend class TIterator<T>;
					friend class TIterator<const T>;
					protected:
						using TArray<const T>::arr_items;
						using TArray<const T>::n_items;
					public:
						using TArray<const T>::operator=;
						using TArray<const T>::operator[];
						using TArray<const T>::ItemPtr;
						using TArray<const T>::CreateStaticIterator;

						//	from IStaticCollection<T>
						system::memory::TUniquePtr<IStaticIterator<T> >		CreateStaticIterator	() final override CL3_WARN_UNUSED_RESULT;

						T*			Claim		() final override;

						TArray&		operator=	(const IStaticCollection<T>& rhs);
						TArray&		operator=	(IStaticCollection<T>&& rhs);

						T&			operator[]	(ssys_t rindex) final override CL3_GETTER;
						T*			ItemPtr		(ssys_t rindex) final override CL3_GETTER;

						CLASS		TArray		(T* arr_items, usys_t n_items, bool b_claim = true);
						CLASS		TArray		(const TArray&);
						CLASS		TArray		(TArray&&);
						CLASS		~TArray		();
				};

				/*********************************************************************/

				template<class T>
				usys_t	IArray<const T>::AbsIndex		(ssys_t rindex) const
				{
					if(rindex < 0)
					{
						const usys_t tmp = -rindex;
						return this->Count() - tmp;
					}
					else
						return rindex;
				}

				template<class T>
				usys_t		_::TRead_impl<T, true>::Read	(uoff_t index, T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					const T* const arr_items = static_cast<IArray<const T>*>(this)->ItemPtr(0);
					const usys_t n_items = static_cast<IArray<const T>*>(this)->Count();

					if(n_items_write_min == (usys_t)-1)
						n_items_write_min = n_items_write_max;

					CL3_CLASS_ERROR(index + n_items_write_min > n_items, TIndexOutOfBoundsException, index + n_items_write_min, n_items);

					const usys_t n_items_write = CL3_MIN(n_items - index, n_items_write_max);

					for(usys_t i = 0; i < n_items_write; i++)
						arr_items_write[i] = arr_items[index + i];

					return n_items_write;
				}

				template<class T>
				usys_t		_::TWrite_impl<T, true>::Write	(uoff_t index, const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					const usys_t n_items = static_cast<IArray<T>*>(this)->Count();
					const usys_t n_items_space = n_items - index;

					if(n_items_write_min == (usys_t)-1)
						n_items_write_min = n_items_write_max;

					CL3_CLASS_ERROR(index > n_items, TIndexOutOfBoundsException, index, n_items);
					CL3_CLASS_ERROR(n_items_write_min > n_items_space, stream::TSinkFloodedException, n_items_write_max, n_items_write_min, 0, n_items_space);

					const usys_t n_items_write = CL3_MIN(n_items_space, n_items_write_max);

					if(n_items_write > 0)
					{
						T* const arr_items = static_cast<IArray<T>*>(this)->ItemPtr(index);
						for(usys_t i = 0; i < n_items_write; i++)
							arr_items[i] = arr_items_write[i];
					}

					return n_items_write;
				}

				/************************************************************************/

				//	from TIterator<const T>
				template<class T>
				void	TIterator<const T>::FixIndex	() const
				{
					switch(this->index)
					{
						case INDEX_TAIL:
						case INDEX_HEAD:
							break;
						default:
							if(this->index >= this->array->n_items)
								this->index = INDEX_TAIL;
							break;
					}
				}

				//	from IStaticIterator<const T>
				template<class T>
				bool	TIterator<const T>::IsValid		() const
				{
					return this->index != INDEX_TAIL && this->index != INDEX_HEAD;
				}

				template<class T>
				const T&TIterator<const T>::Item		() const
				{
					CL3_CLASS_ERROR(!this->IsValid(), TIndexOutOfBoundsException, this->index, this->array->n_items);
					return this->array->arr_items[this->index];
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
					this->index = 0;
					this->FixIndex();
					return this->index != INDEX_TAIL && this->index != INDEX_HEAD;
				}

				template<class T>
				bool	TIterator<const T>::MoveLast	()
				{
					this->index = this->array->n_items - 1;
					this->FixIndex();
					return this->IsValid();
				}

				template<class T>
				bool	TIterator<const T>::MoveNext	()
				{
					switch(this->index)
					{
						case INDEX_TAIL:
							return false;
						case INDEX_HEAD:
							this->index = 0;
							break;
						default:
							this->index++;
							break;
					}
					this->FixIndex();
					return this->IsValid();
				}

				template<class T>
				bool	TIterator<const T>::MovePrev	()
				{
					switch(this->index)
					{
						case INDEX_TAIL:
							if(this->array->n_items > 0)
							{
								this->index = this->array->n_items - 1;
								return true;
							}
							else
								return false;
						case INDEX_HEAD:
							return false;
						case 0:
							this->index = INDEX_HEAD;
							return false;
						default:
							this->index--;
							return true;
					}
				}

				//	from IStaticIterator
				template<class T>
				usys_t	TIterator<const T>::Index		() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				void	TIterator<const T>::Index		(usys_t new_index)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from IIn<T>
				template<class T>
				usys_t	TIterator<const T>::Read		(T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from TIterator
				template<class T>
				CLASS	TIterator<const T>::TIterator	(const TArray<const T>* array, usys_t index) : array(array), index(index)
				{
				}

				/************************************************************************/

				//	from IStaticIterator<T>
				template<class T>
				T&		TIterator<T>::Item		()
				{
					this->FixIndex();
					return this->array->arr_items[this->index];
				}

				//	from IOut<T>
				template<class T>
				usys_t	TIterator<T>::Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				//	from TIterator
				template<class T>
				CLASS	TIterator<T>::TIterator	(const TArray<T>* array, usys_t index) : TIterator<const T>(array, index)
				{
				}

				/************************************************************************/

				template<class T>
				bool	TArray<const T>::IsSorted		() const
				{
					return this->is_sorted;
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<const T> >	TArray<const T>::CreateStaticIterator	() const
				{
					return system::memory::MakeUniquePtr<IStaticIterator<const T> >(new TIterator<const T>(this, n_items > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				usys_t	TArray<const T>::Count		() const
				{
					return n_items;
				}

				template<class T>
				bool	TArray<const T>::CountMin	(usys_t count_min) const
				{
					return count_min >= n_items;
				}

				template<class T>
				bool	TArray<const T>::CountMax	(usys_t count_max) const
				{
					return count_max <= n_items;
				}

				template<class T>
				bool	TArray<const T>::Contains	(const T& item) const
				{
					for(usys_t i = 0; i < n_items; i++)
						if(item == arr_items[i])
							return true;
					return false;
				}

				template<class T>
				TArray<const T>&	TArray<const T>::operator=	(const IStaticCollection<const T>& rhs)
				{
					CL3_NOT_IMPLEMENTED;
// 					this->arr_items = rhs.arr_items;
// 					this->n_items = rhs.n_items;
// 					return *this;
				}

				template<class T>
				TArray<const T>&	TArray<const T>::operator=	(IStaticCollection<const T>&& rhs)
				{
					CL3_NOT_IMPLEMENTED;
// 					this->arr_items = rhs.arr_items;
// 					this->n_items = rhs.n_items;
// 					return *this;
				}

				template<class T>
				const T&	TArray<const T>::operator[]	(ssys_t rindex) const
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items[index];
				}

				template<class T>
				const T*	TArray<const T>::ItemPtr		(ssys_t rindex) const
				{
					if(rindex == 0 && n_items == 0)
						return NULL;
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items + index;
				}

				template<class T>
				CLASS		TArray<const T>::TArray		(const T* arr_items, usys_t n_items, bool b_claim) : arr_items((T*)arr_items), n_items(n_items), b_claim(b_claim), is_sorted(false)
				{
				}

				template<class T>
				CLASS		TArray<const T>::TArray		(const TArray& other) : event::IObservable(), arr_items((T*)system::memory::Alloc(other.n_items, &system::types::typeinfo::TCTTI<T>::rtti)), n_items(other.n_items), is_sorted(other.is_sorted)
				{
					for(usys_t i = 0; i < n_items; i++)
						new (arr_items + i) T(other.arr_items[i]);
				}

				template<class T>
				CLASS		TArray<const T>::TArray		(TArray&& other) : arr_items(other.arr_items), n_items(other.n_items), is_sorted(other.is_sorted)
				{
					other.arr_items = NULL;
					other.n_items = 0;
				}

				template<class T>
				CLASS		TArray<const T>::~TArray	()
				{
					if(b_claim)
					{
						for(usys_t i = 0; i < n_items; i++)
							arr_items[i].~T();
						system::memory::Free(arr_items);
					}
				}

				template<class T>
				usys_t		IArray<const T>::IndexOf	(const T& item_find, usys_t idx_start, EDirection dir) const
				{
					return _::TIndexOf_impl<T, typeinfo::features::is_comparable_biggerthan<T>::value>::IndexOf(this->ItemPtr(0), this->Count(), item_find, idx_start, dir, this->IsSorted());
				}

				namespace _
				{
					template<class T>
					usys_t	TIndexOf_impl<T, true>::IndexOf	(const T* arr_items, usys_t n_items, const T& item_find, usys_t idx_start, EDirection dir, bool is_sorted)
					{
						usys_t high = n_items;
						usys_t low = idx_start;

						CL3_NONCLASS_ERROR(idx_start >= n_items, TIndexOutOfBoundsException, idx_start, n_items);

						if(is_sorted)
						{
							usys_t n = high - low;
							while(sizeof(T) * n > 32)
							{
								const usys_t m = low + n / 2;

								if(item_find > arr_items[m])
									low = m;
								else if(item_find == arr_items[m])
									switch(dir)
									{
										case DIRECTION_FORWARD:
											n += 2;
											break;
										case DIRECTION_BACKWARD:
											low = m;
											break;
									}

								n = (n+1) / 2;
							}
							high = low + n;
						}

						switch(dir)
						{
							case DIRECTION_FORWARD:
								for(usys_t i = 0; i < high-low; i++)
									if(item_find == arr_items[low+i])
										return low+i;

							case DIRECTION_BACKWARD:
								for(usys_t i = 0; i < high-low; i++)
									if(item_find == arr_items[high-1-i])
										return high-1-i;
						}

						return (usys_t)-1;
					}

					template<class T>
					usys_t	TIndexOf_impl<T, false>::IndexOf	(const T* arr_items, usys_t n_items, const T& item_find, usys_t idx_start, EDirection dir, bool is_sorted)
					{
						usys_t high = n_items;
						usys_t low = idx_start;

						CL3_NONCLASS_ERROR(idx_start >= n_items, TIndexOutOfBoundsException, idx_start, n_items);

						switch(dir)
						{
							case DIRECTION_FORWARD:
								for(usys_t i = 0; i < high-low; i++)
									if(item_find == arr_items[low+i])
										return low+i;

							case DIRECTION_BACKWARD:
								for(usys_t i = 0; i < high-low; i++)
									if(item_find == arr_items[high-1-i])
										return high-1-i;
						}

						return (usys_t)-1;
					}
				}

				template<class T>
				const T*	IArray<const T>::Find	(const T& item_find, usys_t idx_start, EDirection dir) const
				{
					const usys_t index = this->IndexOf(item_find, idx_start, dir);
					return index == (usys_t)-1 ? NULL : this->ItemPtr(0) + index;
				}

				template<class T>
				T*			IArray<T>::Find		(const T& item_find, usys_t idx_start, EDirection dir)
				{
					const usys_t index = this->IndexOf(item_find, idx_start, dir);
					return index == (usys_t)-1 ? NULL : this->ItemPtr(0) + index;
				}

				/************************************************************************/

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >			TArray<T>::CreateStaticIterator	()
				{
					return system::memory::MakeUniquePtr<IStaticIterator<T> >(new TIterator<T>(this, n_items > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				T*			TArray<T>::Claim		()
				{
					T* arr_items_tmp = arr_items;
					arr_items = NULL;
					n_items = 0;
					return arr_items_tmp;
				}

				template<class T>
				TArray<T>&	TArray<T>::operator=	(const IStaticCollection<T>& rhs)
				{
					CL3_NOT_IMPLEMENTED;
// 					this->arr_items = rhs.arr_items;
// 					this->n_items = rhs.n_items;
// 					return *this;
				}

				template<class T>
				TArray<T>&	TArray<T>::operator=	(IStaticCollection<T>&& rhs)
				{
					CL3_NOT_IMPLEMENTED;
// 					this->arr_items = rhs.arr_items;
// 					this->n_items = rhs.n_items;
// 					return *this;
				}

				template<class T>
				T*			TArray<T>::ItemPtr		(ssys_t rindex)
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items + index;
				}


				template<class T>
				T&			TArray<T>::operator[]	(ssys_t rindex)
				{
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items[index];
				}

				template<class T>
				CLASS		TArray<T>::TArray		(T* arr_items, usys_t n_items, bool b_claim) : TArray<const T>(arr_items, n_items, b_claim)
				{
				}

				template<class T>
				CLASS		TArray<T>::TArray		(const TArray& other) : event::IObservable(), TArray<const T>(other)
				{
				}

				template<class T>
				CLASS		TArray<T>::TArray		(TArray&& other) : TArray<const T>(other)
				{
				}

				template<class T>
				CLASS		TArray<T>::~TArray		()
				{
				}

				template<class T, usys_t n_items>
				class	TStaticArray;

				template<class T, usys_t n_items>
				class	TStaticArray<const T, n_items> : public virtual IArray<const T>
				{
					protected:
						T arr_items[n_items];

						void RecursiveInitItems(usys_t index, const T& a0)
						{
							this->arr_items[index] = a0;
						}

						template<typename... Args>
						void RecursiveInitItems(usys_t index, const T& a0, const Args&... args)
						{
							this->arr_items[index] = a0;
							this->RecursiveInitItems(index + 1, args...);
						}

						void RecursiveInitItems(usys_t index, T&& a0)
						{
							this->arr_items[index] = std::move(a0);
						}

						template<typename... Args>
						void RecursiveInitItems(usys_t index, T&& a0, Args&&... args)
						{
							this->arr_items[index] = std::move(a0);
							this->RecursiveInitItems(index + 1, std::forward<Args>(args)...);
						}

					public:
						system::memory::TUniquePtr<IStaticIterator<const T> >
								CreateStaticIterator	() const final override CL3_WARN_UNUSED_RESULT;
						usys_t	Count					() const final override CL3_GETTER { return n_items; }
						bool	Contains				(const T& item) const final override CL3_GETTER { CL3_NOT_IMPLEMENTED; }

						bool		IsSorted	() const final override CL3_GETTER { return false; }
						const T&	operator[]	(ssys_t index) const final override CL3_GETTER { return this->arr_items[this->AbsIndex(index)]; }
						const T*	ItemPtr		(ssys_t index) const final override CL3_GETTER { return this->arr_items + this->AbsIndex(index); }

						CLASS explicit TStaticArray() {}
						CLASS explicit TStaticArray(const T* arr_items_init, usys_t n_items_init);

						template<typename... Args>
						CLASS explicit TStaticArray(const T& a0, const Args&... args)
						{
							static_assert(sizeof...(Args) == n_items-1, "number of args must match n_items");
							this->arr_items[0] = a0;
							this->RecursiveInitItems(1, args...);
						}

						template<typename... Args>
						CLASS explicit TStaticArray(T&& a0, Args&&... args)
						{
							static_assert(sizeof...(Args) == n_items-1, "number of args must match n_items");
							this->arr_items[0] = std::move(a0);
							this->RecursiveInitItems(1, std::forward<Args>(args)...);
						}
				};

				template<class T, usys_t n_items>
				class	TStaticArray : public virtual TStaticArray<const T, n_items>, public virtual IArray<T>
				{
					public:
						using TStaticArray<const T, n_items>::operator[];
						using TStaticArray<const T, n_items>::ItemPtr;
						using TStaticArray<const T, n_items>::CreateStaticIterator;

						system::memory::TUniquePtr<IStaticIterator<T> >
								CreateStaticIterator	() final override CL3_WARN_UNUSED_RESULT;

						T*		Claim		() final override { CL3_NOT_IMPLEMENTED; }

						T&		operator[]	(ssys_t index) final override CL3_GETTER { return this->arr_items[this->AbsIndex(index)]; }
						T*		ItemPtr		(ssys_t index) final override CL3_GETTER { return this->arr_items + this->AbsIndex(index); }

						CLASS explicit TStaticArray() {}
						CLASS explicit TStaticArray(const T* arr_items_init, usys_t n_items_init) : TStaticArray<const T, n_items>(arr_items_init, n_items_init) {}

						template<typename... Args>
						CLASS explicit TStaticArray(const T& a0, const Args&... args) : TStaticArray<const T, n_items>(a0, args...) {}

						template<typename... Args>
						CLASS explicit TStaticArray(T&& a0, Args&&... args) : TStaticArray<const T, n_items>(std::forward<T>(a0), std::forward<Args>(args)...) {}
				};

				template<class T, usys_t n_items>
				CLASS TStaticArray<const T, n_items>::TStaticArray(const T* arr_items_init, usys_t n_items_init)
				{
					CL3_CLASS_ERROR(n_items != n_items_init, error::TException, "n_items does not match in constructor");
					for(usys_t i = 0; i < n_items; i++)
						this->arr_items[i] = arr_items_init[i];
				}

				template<class T, usys_t n_items>
				system::memory::TUniquePtr<IStaticIterator<const T> >
						TStaticArray<const T, n_items>::CreateStaticIterator	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T, usys_t n_items>
				system::memory::TUniquePtr<IStaticIterator<T> >
						TStaticArray<T, n_items>::CreateStaticIterator		()
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}

#endif
