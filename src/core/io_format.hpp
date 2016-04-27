/*
 *	 libcl3 - common library version 3
 *	 Copyright (C) 2013	Simon Brennecke
 *
 *	 This program is free software: you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation, either version 3 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	_include_cl3_core_io_format_hpp_
#define	_include_cl3_core_io_format_hpp_

#include "system_types.hpp"
#include "io_text_string.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			using namespace system::types;

			struct TValue;
			struct TMember;
			struct TObject;
			struct TArray;

			struct TValue
			{
				enum class EType
				{
					UNDEFINED,

					SINT8,
					SINT16,
					SINT32,
					SINT64,

					UINT8,
					UINT16,
					UINT32,
					UINT64,

					FLOAT32,
					FLOAT64,

					STRING,
					ARRAY,
					OBJECT
				};

				EType type;

				union
				{
					s8_t s8;
					s16_t s16;
					s32_t s32;
					s64_t s64;

					u8_t u8;
					u16_t u16;
					u32_t u32;
					u64_t u64;

					f32_t f32;
					f64_t f64;

					io::text::string::TString* text;
					TArray* array;
					TObject* object;
				};

				CL3PUBF bool operator==(const TValue& other) const;
				inline bool operator!=(const TValue& other) const { return !(*this == other); }
				CL3PUBF TValue& operator=(const TValue&);
				CL3PUBF TValue& operator=(TValue&&);

				CL3PUBF CLASS TValue();
				CL3PUBF CLASS TValue(TValue&&);
				CL3PUBF CLASS TValue(const TValue&);
				CL3PUBF CLASS ~TValue();
			};

			struct TMember
			{
				io::text::string::TString name;
				TValue value;

				CL3PUBF bool operator==(const TMember&) const;
				inline bool operator!=(const TMember& other) const { return !(*this == other); }
			};

			struct TObject
			{
				io::collection::list::TList<TMember> members;

				CL3PUBF bool operator==(const TObject&) const;
				inline bool operator!=(const TObject& other) const { return !(*this == other); }
			};

			struct TArray
			{
				io::collection::list::TList<TValue> items;

				CL3PUBF bool operator==(const TArray&) const;
				inline bool operator!=(const TArray& other) const { return !(*this == other); }
			};
		}
	}
}

#endif
