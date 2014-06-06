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
				template<class T> class TArray;

				template<class T>
				struct	CL3PUBT	TIterator : public virtual IStaticIterator<T>
				{
					//	from IStaticIterator<const T>
					const IStaticCollection<T>&	Collection	() const;
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
					CLASS	TIterator	(TArray<T>* list, usys_t index);
				};

				template<class T>
				class	TArray : public virtual IStaticCollection<T>
				{
					protected:
						T* arr_items;
						usys_t n_items;
						const event::TEvent<IStaticCollection<T>, TOnChangeData<T> > on_change;

					public:
						//	from IObservable
						const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&	OnChange	() const CL3_GETTER;

						//	from IStaticCollection
						system::memory::TUniquePtr<IStaticIterator<T> >			CreateStaticIterator	() CL3_WARN_UNUSED_RESULT;
						system::memory::TUniquePtr<IStaticIterator<const T> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT;
						usys_t	Count		() const CL3_GETTER;
						bool	CountMin	(usys_t count_min) const CL3_GETTER;
						bool	CountMax	(usys_t count_max) const CL3_GETTER;

						//	from TArray
						TArray&		operator=	(const TArray<T>& rhs);
						TArray&		operator=	(TArray<T>&& rhs);

						T&			operator[]	(usys_t index) CL3_GETTER;
						const T&	operator[]	(usys_t index) const CL3_GETTER;
						T*			ItemPtr		(usys_t index) CL3_GETTER;
						const T*	ItemPtr		(usys_t index) const CL3_GETTER;

						CLASS		TArray		(T* arr_items, usys_t n_items);
				};

				template<class T>
				const event::TEvent<IStaticCollection<T>, TOnChangeData<T> >&	TArray<T>::OnChange	() const
				{
					return this->on_change;
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<T> >			TArray<T>::CreateStaticIterator	()
				{
					return system::memory::MakeUniquePtr<IStaticIterator<T> >(new TIterator<T>(this, n_items > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				system::memory::TUniquePtr<IStaticIterator<const T> >	TArray<T>::CreateStaticIterator	() const
				{
					return system::memory::MakeUniquePtr<IStaticIterator<const T> >(new TIterator<const T>(this, n_items > 0 ? 0 : (usys_t)-1));
				}

				template<class T>
				usys_t	TArray<T>::Count		() const
				{
					return n_items;
				}

				template<class T>
				bool	TArray<T>::CountMin	(usys_t count_min) const
				{
					return count_min >= n_items;
				}

				template<class T>
				bool	TArray<T>::CountMax	(usys_t count_max) const
				{
					return count_max <= n_items;
				}

				template<class T>
				TArray<T>&	TArray<T>::operator=	(const TArray<T>& rhs)
				{
					this->arr_items = rhs.arr_items;
					this->n_items = rhs.n_items;
					return *this;
				}

				template<class T>
				TArray<T>&	TArray<T>::operator=	(TArray<T>&& rhs)
				{
					this->arr_items = rhs.arr_items;
					this->n_items = rhs.n_items;
					return *this;
				}

				template<class T>
				T&			TArray<T>::operator[]	(usys_t index)
				{
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items[index];
				}

				template<class T>
				const T&	TArray<T>::operator[]	(usys_t index) const
				{
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items[index];
				}

				template<class T>
				T*			TArray<T>::ItemPtr		(usys_t index)
				{
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items + index;
				}

				template<class T>
				const T*	TArray<T>::ItemPtr		(usys_t index) const
				{
					CL3_CLASS_ERROR(index >= n_items, TIndexOutOfBoundsException, index, n_items);
					return arr_items + index;
				}

				template<class T>
				CLASS		TArray<T>::TArray		(T* arr_items, usys_t n_items) : arr_items(arr_items), n_items(n_items)
				{
				}
			}
		}
	}
}

#endif
