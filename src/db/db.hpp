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

#ifndef	_include_cl3_db_db_hpp_
#define	_include_cl3_db_db_hpp_

#include <cl3/core/system_memory.hpp>
#include <cl3/core/error.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/system_time.hpp>
#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_collection_array.hpp>
#include <cl3/core/io_text_string.hpp>

namespace	cl3
{
	namespace	db
	{
		using namespace system::types;

		class	TColumnMeta
		{
			protected:
				io::text::string::TString name;
				const typeinfo::TRTTI* type;
				unsigned length;
				unsigned precision;

			public:
				CL3PUBF	io::text::string::TString
									Name		() const CL3_GETTER;
				CL3PUBF	void		Name		(io::text::string::TString) CL3_SETTER;
				CL3PUBF	const typeinfo::TRTTI* Type		() const CL3_GETTER;
				CL3PUBF	void		Type		(const typeinfo::TRTTI*) CL3_SETTER;
				CL3PUBF	unsigned	Length		() const CL3_GETTER;
				CL3PUBF	void		Length		(unsigned) CL3_SETTER;
				CL3PUBF	unsigned	Precision	() const CL3_GETTER;
				CL3PUBF	void		Precision	(unsigned) CL3_SETTER;
		};

		class	TTableMeta
		{
			protected:
				io::text::string::TString name;
				io::collection::list::TList<TColumnMeta> columns;

			public:
				CL3PUBF	io::text::string::TString
								Name	() const CL3_GETTER;
				CL3PUBF	void	Name	(io::text::string::TString) CL3_SETTER;

				CL3PUBF	const io::collection::IStaticCollection<const TColumnMeta>&
								Columns	() const CL3_GETTER;
				CL3PUBF	io::collection::list::TList<TColumnMeta>&
								Columns	() CL3_GETTER;
		};

		class	CL3PUBF	TCell
		{
			protected:
				TColumnMeta* column_meta;
				union
				{
					io::collection::array::TArray<byte_t> blob;
					io::text::string::TString string;
					system::time::TTime time;
					s8_t sint8;
					u8_t uint8;
					s16_t sint16;
					u16_t uint16;
					s32_t sint32;
					u32_t uint32;
					s64_t sint64;
					u64_t uint64;
					f32_t float32;
					f64_t float64;
				} value;

			public:
				CL3PUBF	const TColumnMeta&	ColumnMeta	() const CL3_GETTER;
				inline	const void*			GetValue	() const CL3_GETTER { return &this->value; }

				template<class T>	const T&	GetValue	() const
				{
					CL3_CLASS_ERROR(this->column_meta->Type() != typeinfo::TCTTI<T>::rtti, error::TException, "requested data type does not match actual data type");
					return *reinterpret_cast<const T*>(this->value);
				}
		};

		class	CL3PUBT	TRow
		{
			protected:
				const TTableMeta* table_meta;
				io::collection::array::TArray<TCell*> cells;

			public:
				inline	const io::collection::array::TArray<TCell* const>&	Cells	() const CL3_GETTER { return this->cells; }

				CL3PUBF	CLASS	TRow	(const TTableMeta*, void**);
				CL3PUBF	CLASS	TRow	(const TRow&);
				CL3PUBF	CLASS	TRow	(TRow&&);
				CL3PUBF	CLASS	~TRow	();
		};

		class	CL3PUBT	TResultSet : public virtual io::collection::IStaticIterator<const TRow>
		{
			private:
				CLASS	TResultSet	(const TResultSet&) = delete;
			protected:
				TTableMeta table_meta;
				TRow row;

			public:
				inline	TTableMeta&			TableMeta	() CL3_GETTER { return this->table_meta; }
				inline	const TTableMeta&	TableMeta	() const CL3_GETTER { return this->table_meta; }

				CL3PUBF	const TRow&	Item	() const final override CL3_GETTER;
				CL3PUBF	bool	IsValid		() const final override CL3_GETTER;
				CL3PUBF	void	MoveHead	() final override;
				CL3PUBF	void	MoveTail	() final override;
				CL3PUBF	bool	MoveFirst	() final override;
				CL3PUBF	bool	MoveLast	() final override;
				CL3PUBF	bool	MoveNext	() final override;
				CL3PUBF	bool	MovePrev	() final override;
				CL3PUBF	usys_t	Read		(TRow* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;

				CL3PUBF	CLASS	TResultSet	();
				CL3PUBF	CLASS	~TResultSet	();
		};

		class	CL3PUBT	TConnection
		{
			protected:
				io::text::string::TString& dsn;
				TResultSet rs;

			public:
				CL3PUBF	void		Statement	(const io::text::string::TString& sql);
				CL3PUBF	const io::collection::array::TArray<const void*>&
									Parameters	();
				CL3PUBF	const TResultSet*
									Execute		();

				CL3PUBF	CLASS	TConnection	(const io::text::string::TString& dsn);
				CL3PUBF	CLASS	TConnection	(const TConnection&);
				CL3PUBF	CLASS	~TConnection();
		};
	}
}

#endif
