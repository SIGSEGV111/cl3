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
					virtual	void	Deserialize	(IDeserializer&) = 0;
			};

			class ISerializer
			{
				public:
					CL3PUBF	virtual	void	Push		(const char* name, u8_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, s8_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, u16_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, s16_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, u32_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, s32_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, u64_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, s64_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, f32_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, f64_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, const char* value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, const wchar_t* value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, const text::string::TString& value) = 0;
					CL3PUBF	virtual	void	Push		(const char* name, const ISerializable& value) = 0;
					CL3PUBF	virtual	system::memory::TUniquePtr<IArraySerializer>
											PushArray	(const char* name) = 0;

					CL3PUBF	virtual	CLASS	~ISerializer	();
			};

			class	IArraySerializer
			{
				public:
					CL3PUBF	virtual	void	Push		(u8_t value) = 0;
					CL3PUBF	virtual	void	Push		(s8_t value) = 0;
					CL3PUBF	virtual	void	Push		(u16_t value) = 0;
					CL3PUBF	virtual	void	Push		(s16_t value) = 0;
					CL3PUBF	virtual	void	Push		(u32_t value) = 0;
					CL3PUBF	virtual	void	Push		(s32_t value) = 0;
					CL3PUBF	virtual	void	Push		(u64_t value) = 0;
					CL3PUBF	virtual	void	Push		(s64_t value) = 0;
					CL3PUBF	virtual	void	Push		(f32_t value) = 0;
					CL3PUBF	virtual	void	Push		(f64_t value) = 0;
					CL3PUBF	virtual	void	Push		(const char* value) = 0;
					CL3PUBF	virtual	void	Push		(const wchar_t* value) = 0;
					CL3PUBF	virtual	void	Push		(const text::string::TString& value) = 0;
					CL3PUBF	virtual	void	Push		(const ISerializable& value) = 0;
					CL3PUBF	virtual	system::memory::TUniquePtr<IArraySerializer>
											PushArray	() = 0;

					CL3PUBF	virtual	CLASS	~IArraySerializer	();
			};

			class IDeserializer
			{
				public:
					CL3PUBF	virtual	void	Pop			(const char* name, u8_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, s8_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, u16_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, s16_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, u32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, s32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, u64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, s64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, f32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, f64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, const char*& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, const wchar_t*& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, text::string::TString& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, ISerializable& value) = 0;
					CL3PUBF	virtual	system::memory::TUniquePtr<IArrayDeserializer>
											PopArray	(const char* name) = 0;
					CL3PUBF	virtual	void	Pop			(const char* name, void* object, const system::types::typeinfo::TRTTI& rtti) = 0;

					CL3PUBF	virtual	CLASS	~IDeserializer	();
			};

			class	IArrayDeserializer
			{
				public:
					CL3PUBF	virtual	void	Pop			(u8_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(s8_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(u16_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(s16_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(u32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(s32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(u64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(s64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(f32_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(f64_t& value) = 0;
					CL3PUBF	virtual	void	Pop			(const char*& value) = 0;
					CL3PUBF	virtual	void	Pop			(const wchar_t*& value) = 0;
					CL3PUBF	virtual	void	Pop			(text::string::TString& value) = 0;
					CL3PUBF	virtual	void	Pop			(ISerializable& value) = 0;
					CL3PUBF	virtual	system::memory::TUniquePtr<IArrayDeserializer>
											PopArray	() = 0;
					CL3PUBF	virtual	void	Pop			(void* object, const system::types::typeinfo::TRTTI& rtti) = 0;

					CL3PUBF	virtual	CLASS	~IArrayDeserializer	();
			};
		}
	}
}

#endif
