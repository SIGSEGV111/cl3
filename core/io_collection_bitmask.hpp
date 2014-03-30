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

#ifndef	_include_cl3_core_io_collection_bitmask_hpp_
#define	_include_cl3_core_io_collection_bitmask_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_collection.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	collection
		{
			namespace	bitmask
			{
				class	TBitmask;

				class	TBoolProxy
				{
					protected:
						TBitmask* bm;
						mutable usys_t index;

					public:
						inline operator bool() const;
						inline TBoolProxy& operator=(bool);

						CLASS	TBoolProxy	(TBitmask* bm, usys_t index) : bm(bm), index(index) {}
				};

				struct	IBitmask : public IStaticCollection<TBoolProxy>
				{
					virtual	bool	Bit		(usys_t index) GETTER = 0;
					virtual	void	Bit		(usys_t index, bool value) SETTER = 0;
				};

				struct	IIterator : public virtual IStaticIterator<TBoolProxy>, public virtual stream::IOut<bool>, public virtual stream::IIn<bool>
				{
					virtual	usys_t	Index	() const GETTER = 0;
					virtual	void	Index	(usys_t new_index) SETTER = 0;
				};

				class	TIterator : public IIterator, protected TBoolProxy
				{
					protected:
						void	CheckIndex	() const;

					public:
						//	from IStaticIterator<const bool>
						CL3PUBF	const IStaticCollection<TBoolProxy>&	Collection	() const;
						CL3PUBF	bool		FindNext	(const IMatcher<TBoolProxy>& matcher);
						CL3PUBF	bool		FindPrev	(const IMatcher<TBoolProxy>& matcher);
						CL3PUBF	bool		IsValid		() const GETTER;
						CL3PUBF	const TBoolProxy&	Item() const GETTER;
						CL3PUBF	void		MoveHead	();
						CL3PUBF	void		MoveTail	();
						CL3PUBF	bool		MoveFirst	();
						CL3PUBF	bool		MoveLast	();
						CL3PUBF	bool		MoveNext	();
						CL3PUBF	bool		MovePrev	();

						//	from IStaticIterator<TBoolProxy>
						CL3PUBF	TBoolProxy&	Item		() GETTER;

						//	from IOut<TBoolProxy>
						CL3PUBF	usys_t	Write	(const TBoolProxy* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<TBoolProxy>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIn<TBoolProxy>
						CL3PUBF	usys_t	Read	(TBoolProxy* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<TBoolProxy>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IOut<bool>
						CL3PUBF	usys_t	Write	(const bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<bool>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIn<bool>
						CL3PUBF	usys_t	Read	(bool* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<bool>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IOut<byte_t>
						CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIn<byte_t>
						CL3PUBF	usys_t	Read	(byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

						//	from IIterator
						CL3PUBF	usys_t	Index	() const GETTER;
						CL3PUBF	void	Index	(usys_t new_index) SETTER;

						//	from TIterator
						inline	CLASS	TIterator	(TBitmask* bm, usys_t index) : TBoolProxy(bm, index) {}
				};

				class	TBitmask : public IBitmask
				{
					friend class TIterator;
					protected:
						byte_t* arr_bits;
						usys_t n_bits;
						event::TEvent<IStaticCollection<TBoolProxy>, TOnChangeData<TBoolProxy> > on_change;

					public:
						//	from IObservable
						inline const event::TEvent<IStaticCollection<TBoolProxy>, TOnChangeData<TBoolProxy> >& OnChange() const GETTER
						{ return on_change; }

						//	from IStaticCollection
						inline system::memory::TUniquePtr<IStaticIterator<TBoolProxy> > CreateStaticIterator() CL3_WARN_UNUSED_RESULT
						{ return system::memory::MakeUniquePtr<IStaticIterator<TBoolProxy> >(new TIterator(this, n_bits > 0 ? 0 : (usys_t)-1)); }

						inline system::memory::TUniquePtr<IStaticIterator<const TBoolProxy> > CreateStaticIterator() const CL3_WARN_UNUSED_RESULT
						{ return system::memory::MakeUniquePtr<IStaticIterator<const TBoolProxy> >(new TIterator(const_cast<TBitmask*>(this), n_bits > 0 ? 0 : (usys_t)-1)); }

						CL3PUBF	usys_t	Count	() const GETTER;

						//	from IBitmask
						CL3PUBF	bool	Bit		(usys_t index) GETTER;
						CL3PUBF	void	Bit		(usys_t index, bool value) SETTER;

						//	from TBitmask
						CL3PUBF	void	Count	(usys_t new_count) SETTER;
						CL3PUBF	TBitmask&	operator=	(const TBitmask&);

						CL3PUBF	CLASS	TBitmask();
						CL3PUBF	CLASS	TBitmask(const TBitmask&);
						CL3PUBF	CLASS	~TBitmask();
				};

				inline TBoolProxy::operator bool() const { return bm->Bit(index); }
				inline TBoolProxy& TBoolProxy::operator=(bool b) { bm->Bit(index, b); return *this; }
			}
		}
	}
}

#endif
