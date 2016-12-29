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
			namespace	string
			{
				class	TString;
			}
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
					/ acceptable in cl3ss (bloated / names get dropped)

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

			class ISerializable
			{
				public:
					virtual	void	Serialize	(ISerializer&) const = 0;
			};

			class IDeserializable
			{
				public:
					virtual	void	Deserialize	(IDeserializer&) = 0;
			};

			class ISerializer
			{
				public:
					template<typename T>
					void Push(const char* name, const T& value)
					{
						struct TProxy : public ISerializable
						{
							const T* obj;

							void Serialize(ISerializer& s) const final override
							{
								obj->Serialize(s);
							}

							CLASS TProxy(const T* obj) : obj(obj) {}
						};

						TProxy proxy(&value);

						this->Push(name, (ISerializable&)proxy);
					}

					virtual	void Push(const char* name, u8_t  value) = 0;
					virtual	void Push(const char* name, s8_t  value) = 0;
					virtual	void Push(const char* name, u16_t value) = 0;
					virtual	void Push(const char* name, s16_t value) = 0;
					virtual	void Push(const char* name, u32_t value) = 0;
					virtual	void Push(const char* name, s32_t value) = 0;
					virtual	void Push(const char* name, u64_t value) = 0;
					virtual	void Push(const char* name, s64_t value) = 0;
					virtual	void Push(const char* name, f32_t value) = 0;
					virtual	void Push(const char* name, f64_t value) = 0;
					virtual	void Push(const char* name, const ISerializable& value) = 0;
			};

			class IDeserializer
			{
				protected:
					virtual void EnterObject() = 0;
					virtual void LeaveObject() = 0;

				public:
					template<typename T>
					void Pop(const char* name, T& value)
					{
						struct TProxy : public IDeserializable
						{
							T* obj;

							void Deserialize(IDeserializer& ds) final override
							{
								obj->Deserialize(ds);
							}

							CLASS TProxy(T* obj) : obj(obj) {}
						};

						TProxy proxy(&value);

						this->Pop(name, (IDeserializable&)proxy);
					}

					virtual	void Pop(const char* name, u8_t&  value) = 0;
					virtual	void Pop(const char* name, s8_t&  value) = 0;
					virtual	void Pop(const char* name, u16_t& value) = 0;
					virtual	void Pop(const char* name, s16_t& value) = 0;
					virtual	void Pop(const char* name, u32_t& value) = 0;
					virtual	void Pop(const char* name, s32_t& value) = 0;
					virtual	void Pop(const char* name, u64_t& value) = 0;
					virtual	void Pop(const char* name, s64_t& value) = 0;
					virtual	void Pop(const char* name, f32_t& value) = 0;
					virtual	void Pop(const char* name, f64_t& value) = 0;
					virtual	void Pop(const char* name, IDeserializable& value) = 0;

					struct TObjectPopper
					{
						IDeserializer* const ds;
						inline operator IDeserializer&() const { return *this->ds; }

						CLASS TObjectPopper(IDeserializer* ds) : ds(ds) { ds->EnterObject(); }
						CLASS ~TObjectPopper() { ds->LeaveObject(); }
						CLASS TObjectPopper(const TObjectPopper&) = delete;
						CLASS TObjectPopper(TObjectPopper&&) = default;
					};

					virtual TObjectPopper&& PopObject(const char* name) = 0;
			};
		}
	}
}

#endif
