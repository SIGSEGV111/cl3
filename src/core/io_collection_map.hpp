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

#ifndef	_include_cl3_core_io_collection_map_hpp_
#define	_include_cl3_core_io_collection_map_hpp_

#include "io_collection.hpp"
#include "error.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	map
			{
				template<class TKey, class TValue>
				struct TPair
				{
					TKey key;
					TValue value;

					inline bool operator==(const TPair& rhs) const { return this->key == rhs.key && this->value == rhs.value; }

					CLASS explicit TPair() {}
					CLASS explicit TPair(TKey key, TValue value) : key(key), value(value) {}
				};

				template<class TKey, class TValue>
				struct	IMap;

				template<class TKey, class TValue>
				struct	IMap<const TKey, const TValue> : virtual IDynamicCollection<const TPair<TKey, TValue>>
				{
					virtual	const TValue&	operator[]	(const TKey&) const CL3_GETTER = 0;
				};

				template<class TKey, class TValue>
				struct	IMap : virtual IMap<const TKey, const TValue>, virtual IDynamicCollection<TPair<TKey, TValue>>
				{
					virtual	TValue&	operator[]	(const TKey&) CL3_GETTER = 0;
				};

				/************************************************************************************/

				template<class TKey, class TValue>
				class TInlinedMap;

				template<class TKey, class TValue>
				class TInlinedMap<const TKey, const TValue> : virtual IMap<const TKey, const TValue>
				{
				};

				template<class TKey, class TValue>
				class TInlinedMap : public TInlinedMap<const TKey, const TValue>, virtual IMap<TKey, TValue>
				{
				};

				/************************************************************************************/

				template<class TKey, class TValue>
				class TStdMap;

				template<class TKey, class TValue>
				class TStdMap<const TKey, const TValue> : public virtual IMap<const TKey, const TValue>
				{
					protected:
						list::TList<TPair<TKey, TValue>> items;

					public:
						//	from IMap
						CL3PUBF	const TValue&	operator[]	(const TKey&) const final override CL3_GETTER;

						//	from IDynamicCollection
						CL3PUBF	void			Add			(const TPair<TKey, TValue>& item_add) final override;
						CL3PUBF	void			Add			(const TPair<TKey, TValue>* arr_items_add, usys_t n_items_add) final override;
						CL3PUBF	void			Add			(const IStaticCollection<const TPair<TKey, TValue>>& collection) final override;
						CL3PUBF	system::memory::TUniquePtr<IDynamicIterator<const TPair<TKey, TValue>> >
									CreateDynamicIterator	() const final override CL3_WARN_UNUSED_RESULT;

						//	from IStaticCollection
						CL3PUBF	usys_t			Count		() const final override CL3_GETTER;
						CL3PUBF	bool			Contains	(const TPair<TKey, TValue>& item) const final override CL3_GETTER;
						CL3PUBF	system::memory::TUniquePtr<IStaticIterator<const TPair<TKey, TValue>> >
									CreateStaticIterator	() const final override CL3_WARN_UNUSED_RESULT;

						// from IOut
						CL3PUBF	usys_t			Write		(const TPair<TKey, TValue>* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from TStdMap
						CL3PUBF	CLASS			TStdMap		();
						CL3PUBF	CLASS virtual	~TStdMap	();
				};

				template<class TKey, class TValue>
				class TStdMap : public virtual TStdMap<const TKey, const TValue>, public virtual IMap<TKey, TValue>
				{
					public:
						using TStdMap<const TKey, const TValue>::CreateDynamicIterator;
						using TStdMap<const TKey, const TValue>::CreateStaticIterator;

						//	from IMap
						CL3PUBF	TValue&	operator[]	(const TKey&) final override CL3_GETTER;

						//	from IDynamicCollection
						CL3PUBF	void	Clear		() final override;
						CL3PUBF	bool	Remove		(const TPair<TKey, TValue>& item_remove) final override;

						CL3PUBF	system::memory::TUniquePtr<IDynamicIterator<TPair<TKey, TValue>> >
									CreateDynamicIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IStaticCollection
						CL3PUBF	system::memory::TUniquePtr<IStaticIterator<TPair<TKey, TValue>> >
									CreateStaticIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IIn
						CL3PUBF	usys_t	Remaining	() const final override CL3_GETTER;
						CL3PUBF	usys_t	Read		(TPair<TKey, TValue>* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;

						//	from TStdMap
						CL3PUBF	CLASS			TStdMap		();
						CL3PUBF	CLASS virtual	~TStdMap	();
				};


				template<class TKey, class TValue>
				const TValue&	TStdMap<const TKey, const TValue>::operator[]	(const TKey&) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				void			TStdMap<const TKey, const TValue>::Add		(const TPair<TKey, TValue>& item_add)
				{
					this->items.Append(item_add);	//	FIXME
				}

				template<class TKey, class TValue>
				void			TStdMap<const TKey, const TValue>::Add		(const TPair<TKey, TValue>* arr_items_add, usys_t n_items_add)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				void			TStdMap<const TKey, const TValue>::Add		(const IStaticCollection<const TPair<TKey, TValue>>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				system::memory::TUniquePtr<IDynamicIterator<const TPair<TKey, TValue>> > TStdMap<const TKey, const TValue>::CreateDynamicIterator	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				usys_t			TStdMap<const TKey, const TValue>::Count		() const
				{
					return this->items.Count();
				}

				template<class TKey, class TValue>
				bool			TStdMap<const TKey, const TValue>::Contains	(const TPair<TKey, TValue>& item) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				system::memory::TUniquePtr<IStaticIterator<const TPair<TKey, TValue>>> TStdMap<const TKey, const TValue>::CreateStaticIterator	() const
				{
					return this->items.CreateStaticIterator();
				}

				template<class TKey, class TValue>
				usys_t			TStdMap<const TKey, const TValue>::Write		(const TPair<TKey, TValue>* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				CLASS			TStdMap<const TKey, const TValue>::TStdMap	()
				{
				}

				template<class TKey, class TValue>
				CLASS 			TStdMap<const TKey, const TValue>::~TStdMap	()
				{
				}

				template<class TKey, class TValue>
				TValue&	TStdMap<TKey, TValue>::operator[](const TKey&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				void	TStdMap<TKey, TValue>::Clear		()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				bool	TStdMap<TKey, TValue>::Remove	(const TPair<TKey, TValue>& item_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				system::memory::TUniquePtr<IDynamicIterator<TPair<TKey, TValue>>>
						TStdMap<TKey, TValue>::CreateDynamicIterator	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				system::memory::TUniquePtr<IStaticIterator<TPair<TKey, TValue>>>
						TStdMap<TKey, TValue>::CreateStaticIterator	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				usys_t	TStdMap<TKey, TValue>::Remaining	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				usys_t	TStdMap<TKey, TValue>::Read		(TPair<TKey, TValue>* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TValue>
				CLASS	TStdMap<TKey, TValue>::TStdMap	()
				{
				}

				template<class TKey, class TValue>
				CLASS	TStdMap<TKey, TValue>::~TStdMap	()
				{
				}
			}
		}
	}
}

#endif
