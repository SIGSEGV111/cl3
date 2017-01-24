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

#ifndef	_include_cl3_core_io_serialization_hpp_
#define	_include_cl3_core_io_serialization_hpp_

#include "system_types.hpp"
#include "system_types_typeinfo.hpp"
#include "system_memory.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			struct TUTF32;

			namespace	string
			{
				class	TString;
			}
		}

		namespace collection
		{
			template<typename T>
			class IStaticCollection;
		}

		namespace	serialization
		{
			/*
				ultra fancy:
					+ perfect semantic accross all storage formats
					+ browseable
					+ perfect error checking
					- huge memory consumption
					- painful to serialize/deserialize
					+ option for backward compatbility

					+ looks perfect in json
					+ looks good in xml
					/ acceptable in cl3ss (bloated)

					requires:
						solution for how to easily encode arrays (BeginArray/EndArray???)

				lightweight wo/ names:
					- little semantic
					+ browseable
					+ robust error checks
					/ acceptable memory footprint
					+ easy to serialize/deserialize
					- no backward compatbility

					- looks like shit in xml (cascaded elements, all called the same)
					/ acceptable in json (semantically cascaded arrays)
					+ perfect in cl3ss

				ultra minimalistic:
					- no semantic
					- not browseable
					- no error checks
					+ minimal possible memory footprint
					+ easy to serialize/deserialize
					- no backward compatbility

					- looks like shit in xml (one single big flat array)
					- looks like shit in json (one single big flat array)
					/ OK in cl3ss
			*/

			using namespace system::types;

			class	CL3PUBT	ISerializer;
			class	CL3PUBT	IDeserializer;
			class	CL3PUBT	ISerializable;

			class	CL3PUBT	IArraySerializer;
			class	CL3PUBT	IArrayDeserializer;

			class ISerializable
			{
				public:
					virtual	void	Serialize	(ISerializer&) const = 0;
			};

			class IDeserializable : public ISerializable
			{
				public:
					virtual	void	Deserialize	(IDeserializer&) = 0;
			};

			class ISerializer
			{
				protected:
					virtual void EnterObject(const char* name, const system::types::typeinfo::TRTTI* rtti) = 0;
					virtual void LeaveObject() = 0;

				public:
					virtual void Push(const char* name, u8_t  value) = 0;
					virtual void Push(const char* name, s8_t  value) = 0;
					virtual void Push(const char* name, u16_t value) = 0;
					virtual void Push(const char* name, s16_t value) = 0;
					virtual void Push(const char* name, u32_t value) = 0;
					virtual void Push(const char* name, s32_t value) = 0;
					virtual void Push(const char* name, u64_t value) = 0;
					virtual void Push(const char* name, s64_t value) = 0;
					virtual void Push(const char* name, f32_t value) = 0;
					virtual void Push(const char* name, f64_t value) = 0;
					virtual void Push(const char* name, const char* value) = 0;
					virtual void Push(const char* name, const wchar_t* value) = 0;
					virtual void Push(const char* name, const collection::IStaticCollection<const text::TUTF32>& value) = 0;

					template<typename T>
					void Push(const char* name, const T& obj)
					{
						this->EnterObject(name, &system::types::typeinfo::TCTTI<T>::rtti);
						obj.Serialize(*this);
						this->LeaveObject();
					}

					virtual	system::memory::TUniquePtr<IArraySerializer>
								PushArray(const char* name, usys_t n_items) = 0;

					virtual CLASS ~ISerializer();
			};

			class	IArraySerializer
			{
				protected:
					virtual void EnterObject(const system::types::typeinfo::TRTTI* rtti) = 0;
					virtual void LeaveObject() = 0;

				public:
					virtual void Push(u8_t  value) = 0;
					virtual void Push(s8_t  value) = 0;
					virtual void Push(u16_t value) = 0;
					virtual void Push(s16_t value) = 0;
					virtual void Push(u32_t value) = 0;
					virtual void Push(s32_t value) = 0;
					virtual void Push(u64_t value) = 0;
					virtual void Push(s64_t value) = 0;
					virtual void Push(f32_t value) = 0;
					virtual void Push(f64_t value) = 0;
					virtual void Push(const char* value) = 0;
					virtual void Push(const wchar_t* value) = 0;
					virtual void Push(const collection::IStaticCollection<const text::TUTF32>& value) = 0;
					virtual void Push(const ISerializable& value) = 0;

					template<typename T>
					void Push(const T& obj)
					{
						// this is to make sure that the type has a deserializing constructor
						system::types::typeinfo::FDeserialize dserctor = system::types::typeinfo::_::generic_deserctor<T>;

						this->EnterObject(&system::types::typeinfo::TCTTI<T>::rtti);
						obj.Serialize(*this);
						this->LeaveObject();
					}

					virtual system::memory::TUniquePtr<IArraySerializer>
								PushArray(usys_t n_items) = 0;

					virtual CLASS ~IArraySerializer	();
			};

			class IDeserializer
			{
				protected:
					virtual void EnterObject(const char* name, const system::types::typeinfo::TRTTI* rtti) = 0;
					virtual void LeaveObject() = 0;

				public:
					virtual void Pop(const char* name, u8_t&  value) = 0;
					virtual void Pop(const char* name, s8_t&  value) = 0;
					virtual void Pop(const char* name, u16_t& value) = 0;
					virtual void Pop(const char* name, s16_t& value) = 0;
					virtual void Pop(const char* name, u32_t& value) = 0;
					virtual void Pop(const char* name, s32_t& value) = 0;
					virtual void Pop(const char* name, u64_t& value) = 0;
					virtual void Pop(const char* name, s64_t& value) = 0;
					virtual void Pop(const char* name, f32_t& value) = 0;
					virtual void Pop(const char* name, f64_t& value) = 0;
					virtual void Pop(const char* name, const char*& value) = 0;
					virtual void Pop(const char* name, const wchar_t*& value) = 0;
					virtual void Pop(const char* name, text::string::TString& value) = 0;
					virtual void Pop(const char* name, IDeserializable&) = 0;

					template<typename T>
					void Pop(const char* name, T& obj)
					{
						this->EnterObject(name, &system::types::typeinfo::TCTTI<T>::rtti);
						obj.Deserialize(*this);
						this->LeaveObject();
					}

					struct TPopper
					{
						IDeserializer* ds;

						CLASS TPopper(IDeserializer* ds, const char* name, const system::types::typeinfo::TRTTI* rtti) : ds(ds) { ds->EnterObject(name, rtti); };
						CLASS TPopper(TPopper&& other) : ds(other.ds) { other.ds = NULL; }
						CLASS ~TPopper() { if(this->ds) this->ds->LeaveObject(); }
					};

					template<typename T>
					TPopper PopCtor(const char* name)
					{
						return TPopper(this, name, &system::types::typeinfo::TCTTI<T>::rtti);
					}

					virtual system::memory::TUniquePtr<IArrayDeserializer>
									PopArray(const char* name) = 0;

					virtual CLASS ~IDeserializer();
			};

			class	IArrayDeserializer
			{
				protected:
					virtual void EnterObject(const system::types::typeinfo::TRTTI* rtti) = 0;
					virtual void LeaveObject() = 0;

				public:
					virtual usys_t CountRemaining() const = 0;

					virtual void Pop(u8_t&  value) = 0;
					virtual void Pop(s8_t&  value) = 0;
					virtual void Pop(u16_t& value) = 0;
					virtual void Pop(s16_t& value) = 0;
					virtual void Pop(u32_t& value) = 0;
					virtual void Pop(s32_t& value) = 0;
					virtual void Pop(u64_t& value) = 0;
					virtual void Pop(s64_t& value) = 0;
					virtual void Pop(f32_t& value) = 0;
					virtual void Pop(f64_t& value) = 0;
					virtual void Pop(const char*& value) = 0;
					virtual void Pop(const wchar_t*& value) = 0;
					virtual void Pop(text::string::TString& value) = 0;
					virtual void Pop(const char* name, IDeserializable&) = 0;

					struct TPopper
					{
						IArrayDeserializer* ds;

						CLASS TPopper(IArrayDeserializer* ds, const system::types::typeinfo::TRTTI* rtti) : ds(ds) { ds->EnterObject(rtti); };
						CLASS TPopper(TPopper&& other) : ds(other.ds) { other.ds = NULL; }
						CLASS ~TPopper() { if(this->ds) this->ds->LeaveObject(); }
					};

					template<typename T>
					TPopper PopCtor(const char* name)
					{
						return TPopper(this, &system::types::typeinfo::TCTTI<T>::rtti);
					}

					virtual system::memory::TUniquePtr<IArrayDeserializer>
									PopArray() = 0;

					virtual	CLASS ~IArrayDeserializer();
			};
		}
	}
}

#endif
