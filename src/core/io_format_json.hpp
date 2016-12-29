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

#ifndef	_include_cl3_core_io_format_json_hpp_
#define	_include_cl3_core_io_format_json_hpp_

#include "system_types.hpp"
#include "io_format.hpp"
#include "io_serialization.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			namespace	json
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
						NULLVALUE,
						BOOL,
						NUMBER,
						STRING,
						ARRAY,
						OBJECT
					};

					EType type;

					union
					{
						bool b;
						f64_t n;
						io::text::string::TString* s;
						TArray* a;
						TObject* o;
					};

					CL3PUBF bool operator==(const TValue& other) const;
					inline bool operator!=(const TValue& other) const { return !(*this == other); }
					CL3PUBF TValue& operator=(const TValue&);
					CL3PUBF TValue& operator=(TValue&&);

					CL3PUBF CLASS TValue(EType type = EType::UNDEFINED);
					CL3PUBF CLASS TValue(const io::text::string::TString& text);
					CL3PUBF CLASS TValue(f64_t number);
					CL3PUBF CLASS TValue(TValue&&);
					CL3PUBF CLASS TValue(const TValue&);
					CL3PUBF CLASS TValue(serialization::IDeserializer& ds);
					CL3PUBF CLASS ~TValue();
				};

				struct TObject : serialization::ISerializable, serialization::IDeserializable
				{
					struct TMember
					{
						io::text::string::TString name;
						TValue value;
					};

					io::collection::list::TList<TMember> members;	//	FIXME: use map

					CL3PUBF void Serialize(serialization::ISerializer& s) const final override;
					CL3PUBF void Deserialize(serialization::IDeserializer& ds) final override;
				};

				struct TArray : serialization::ISerializable, serialization::IDeserializable
				{
					io::collection::list::TList<TValue> items;

					CL3PUBF void Serialize(serialization::ISerializer& s) const final override;
					CL3PUBF void Deserialize(serialization::IDeserializer& ds) final override;
				};
			}
		}
	}
}

#endif
