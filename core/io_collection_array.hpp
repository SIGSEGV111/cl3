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

#include "io_collection.hpp"

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
				struct	CL3PUBT	IArray<const T> : virtual IStaticCollection<const T>
				{
					usys_t	AbsIndex	(ssys_t rindex) const CL3_GETTER;

					virtual	const T&	operator[]	(ssys_t index) const CL3_GETTER = 0;
					virtual	const T*	ItemPtr		(ssys_t index) const CL3_GETTER = 0;

					virtual	usys_t		Read		(uoff_t index, T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;
					inline	void		Read		(uoff_t index, T* arr_items_write, usys_t n_items_write);
				};

				template<class T>
				struct	CL3PUBT	IArray : virtual IArray<const T>,  virtual IStaticCollection<T>
				{
					using array::IArray<const T>::operator[];
					using array::IArray<const T>::ItemPtr;
					using array::IArray<const T>::Read;

					virtual	T&			operator[]	(ssys_t index) CL3_GETTER = 0;
					virtual	T*			ItemPtr		(ssys_t index) CL3_GETTER = 0;

					virtual	usys_t		Write		(uoff_t index, const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;
					inline	void		Write		(uoff_t index, const T* arr_items_write, usys_t n_items_write);
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
						const event::TEvent<const IStaticCollection<const T>, TOnChangeData<const T> > on_change;

					public:
						//	from IObservable
						const event::TEvent<const IStaticCollection<const T>, TOnChangeData<const T> >&	OnChange	() const CL3_GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT;
						usys_t	Count		() const CL3_GETTER;
						bool	CountMin	(usys_t count_min) const CL3_GETTER;
						bool	CountMax	(usys_t count_max) const CL3_GETTER;

						//	from IStaticCollection<const T>
						TArray&		operator=	(const IStaticCollection<const T>& rhs);
						TArray&		operator=	(IStaticCollection<const T>&& rhs);

						const T&	operator[]	(ssys_t rindex) const CL3_GETTER;
						const T*	ItemPtr		(ssys_t rindex) const CL3_GETTER;

						CLASS		TArray		(const T* arr_items, usys_t n_items);
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
						system::memory::TUniquePtr<IStaticIterator<T> >		CreateStaticIterator	() CL3_WARN_UNUSED_RESULT;

						TArray&		operator=	(const IStaticCollection<T>& rhs);
						TArray&		operator=	(IStaticCollection<T>&& rhs);

						T&			operator[]	(ssys_t rindex) CL3_GETTER;
						T*			ItemPtr		(ssys_t rindex) CL3_GETTER;

						CLASS		TArray		(T* arr_items, usys_t n_items);
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
				usys_t		IArray<const T>::Read		(uoff_t index, T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					const T* const arr_items = this->ItemPtr(0);
					const usys_t n_items = this->Count();

					if(n_items_write_min == (usys_t)-1)
						n_items_write_min = n_items_write_max;

					CL3_CLASS_ERROR(index + n_items_write_min > n_items, TIndexOutOfBoundsException, index + n_items_write_min, n_items);

					const usys_t n_items_write = CL3_MIN(n_items - index, n_items_write_max);

					for(usys_t i = 0; i < n_items_write; i++)
						arr_items_write[i] = arr_items[index + i];

					return n_items_write;
				}

				template<class T>
				usys_t		IArray<T>::Write	(uoff_t index, const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					T* const arr_items = this->ItemPtr(0);
					const usys_t n_items = this->Count();

					if(n_items_write_min == (usys_t)-1)
						n_items_write_min = n_items_write_max;

					CL3_CLASS_ERROR(index + n_items_write_min > n_items, TIndexOutOfBoundsException, index + n_items_write_min, n_items);

					const usys_t n_items_write = CL3_MIN(n_items - index, n_items_write_max);

					for(usys_t i = 0; i < n_items_write; i++)
						arr_items[index + i] = arr_items_write[i];

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
				bool	TIterator<const T>::FindNext	(const IMatcher<T>& matcher)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class T>
				bool	TIterator<const T>::FindPrev	(const IMatcher<T>& matcher)
				{
					CL3_NOT_IMPLEMENTED;
				}

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
				const event::TEvent<const IStaticCollection<const T>, TOnChangeData<const T> >&	TArray<const T>::OnChange	() const
				{
					return this->on_change;
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
					const usys_t index = this->AbsIndex(rindex);
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items + index;
				}

				template<class T>
				CLASS		TArray<const T>::TArray		(const T* arr_items, usys_t n_items) : arr_items((T*)arr_items), n_items(n_items)
				{
				}

				/************************************************************************/

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >			TArray<T>::CreateStaticIterator	()
				{
					return system::memory::MakeUniquePtr<IStaticIterator<T> >(new TIterator<T>(this, n_items > 0 ? 0 : (usys_t)-1));
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
				CLASS		TArray<T>::TArray		(T* arr_items, usys_t n_items) : TArray<const T>(arr_items, n_items)
				{
				}
			}
		}
	}
}

#endif
