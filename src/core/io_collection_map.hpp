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
				template<class TKey, class TItem>
				struct TPair
				{
					TItem key;
					TKey value;

					inline bool operator==(const TPair& rhs) const { return this->key == rhs.key && this->value == rhs.value; }
				};

				template<class TKey, class TItem>
				struct	IMap;

				template<class TKey, class TItem>
				struct	IMap<const TKey, const TItem> : virtual IDynamicCollection<const TPair<TKey, TItem>>
				{
					virtual	const TItem&	operator[]	(const TKey&) const CL3_GETTER = 0;
				};

				template<class TKey, class TItem>
				struct	IMap : virtual IMap<const TKey, const TItem>, virtual IDynamicCollection<TPair<TKey, TItem>>
				{
					virtual	TItem&	operator[]	(const TKey&) CL3_GETTER = 0;
				};

				/************************************************************************************/

				template<class TKey, class TItem>
				class TInlinedMap;

				template<class TKey, class TItem>
				class TInlinedMap<const TKey, const TItem> : virtual IMap<const TKey, const TItem>
				{
				};

				template<class TKey, class TItem>
				class TInlinedMap : public TInlinedMap<const TKey, const TItem>, virtual IMap<TKey, TItem>
				{
				};

				/************************************************************************************/

				template<class TKey, class TItem>
				class TStdMap;

				template<class TKey, class TItem>
				class TStdMap<const TKey, const TItem> : public virtual IMap<const TKey, const TItem>
				{
					protected:
						list::TList<TPair<TKey, TItem>> items;

					public:
						//	from IMap
						CL3PUBF	const TItem&	operator[]	(const TKey&) const final override CL3_GETTER;

						//	from IDynamicCollection
						CL3PUBF	void			Add			(const TPair<TKey, TItem>& item_add) final override;
						CL3PUBF	void			Add			(const TPair<TKey, TItem>* arr_items_add, usys_t n_items_add) final override;
						CL3PUBF	void			Add			(const IStaticCollection<const TPair<TKey, TItem>>& collection) final override;
						CL3PUBF	system::memory::TUniquePtr<IDynamicIterator<const TPair<TKey, TItem>> >
									CreateDynamicIterator	() const final override CL3_WARN_UNUSED_RESULT;

						//	from IStaticCollection
						CL3PUBF	usys_t			Count		() const final override CL3_GETTER;
						CL3PUBF	bool			Contains	(const TPair<TKey, TItem>& item) const final override CL3_GETTER;
						CL3PUBF	system::memory::TUniquePtr<IStaticIterator<const TPair<TKey, TItem>> >
									CreateStaticIterator	() const final override CL3_WARN_UNUSED_RESULT;

						// from IOut
						CL3PUBF	usys_t			Write		(const TPair<TKey, TItem>* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						//	from TStdMap
						CL3PUBF	CLASS			TStdMap		();
						CL3PUBF	CLASS virtual	~TStdMap	();
				};

				template<class TKey, class TItem>
				class TStdMap : public virtual TStdMap<const TKey, const TItem>, public virtual IMap<TKey, TItem>
				{
					public:
						using TStdMap<const TKey, const TItem>::CreateDynamicIterator;
						using TStdMap<const TKey, const TItem>::CreateStaticIterator;

						//	from IMap
						CL3PUBF	TItem&	operator[]	(const TKey&) final override CL3_GETTER;

						//	from IDynamicCollection
						CL3PUBF	void	Clear		() final override;
						CL3PUBF	bool	Remove		(const TPair<TKey, TItem>& item_remove) final override;

						CL3PUBF	system::memory::TUniquePtr<IDynamicIterator<TPair<TKey, TItem>> >
									CreateDynamicIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IStaticCollection
						CL3PUBF	system::memory::TUniquePtr<IStaticIterator<TPair<TKey, TItem>> >
									CreateStaticIterator	() final override CL3_WARN_UNUSED_RESULT;

						//	from IIn
						CL3PUBF	usys_t	Remaining	() const final override CL3_GETTER;
						CL3PUBF	usys_t	Read		(TPair<TKey, TItem>* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;

						//	from TStdMap
						CL3PUBF	CLASS			TStdMap		();
						CL3PUBF	CLASS virtual	~TStdMap	();
				};


				template<class TKey, class TItem>
				const TItem&	TStdMap<const TKey, const TItem>::operator[]	(const TKey&) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				void			TStdMap<const TKey, const TItem>::Add		(const TPair<TKey, TItem>& item_add)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				void			TStdMap<const TKey, const TItem>::Add		(const TPair<TKey, TItem>* arr_items_add, usys_t n_items_add)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				void			TStdMap<const TKey, const TItem>::Add		(const IStaticCollection<const TPair<TKey, TItem>>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				system::memory::TUniquePtr<IDynamicIterator<const TPair<TKey, TItem>> > TStdMap<const TKey, const TItem>::CreateDynamicIterator	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				usys_t			TStdMap<const TKey, const TItem>::Count		() const
				{
					return this->items.Count();
				}

				template<class TKey, class TItem>
				bool			TStdMap<const TKey, const TItem>::Contains	(const TPair<TKey, TItem>& item) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				system::memory::TUniquePtr<IStaticIterator<const TPair<TKey, TItem>>> TStdMap<const TKey, const TItem>::CreateStaticIterator	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				usys_t			TStdMap<const TKey, const TItem>::Write		(const TPair<TKey, TItem>* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				CLASS			TStdMap<const TKey, const TItem>::TStdMap	()
				{
				}

				template<class TKey, class TItem>
				CLASS 			TStdMap<const TKey, const TItem>::~TStdMap	()
				{
				}

				template<class TKey, class TItem>
				TItem&	TStdMap<TKey, TItem>::operator[](const TKey&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				void	TStdMap<TKey, TItem>::Clear		()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				bool	TStdMap<TKey, TItem>::Remove	(const TPair<TKey, TItem>& item_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				system::memory::TUniquePtr<IDynamicIterator<TPair<TKey, TItem>>>
						TStdMap<TKey, TItem>::CreateDynamicIterator	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				system::memory::TUniquePtr<IStaticIterator<TPair<TKey, TItem>>>
						TStdMap<TKey, TItem>::CreateStaticIterator	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				usys_t	TStdMap<TKey, TItem>::Remaining	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				usys_t	TStdMap<TKey, TItem>::Read		(TPair<TKey, TItem>* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				template<class TKey, class TItem>
				CLASS	TStdMap<TKey, TItem>::TStdMap	()
				{
				}

				template<class TKey, class TItem>
				CLASS	TStdMap<TKey, TItem>::~TStdMap	()
				{
				}
			}
		}
	}
}

#endif
