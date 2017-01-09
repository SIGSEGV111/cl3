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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "io_format_json.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			namespace	json
			{
				using namespace text::string;

				using namespace io::text::string;

				bool TValue::operator==(const TValue& other) const
				{
					if(this->type != other.type)
						return false;

					switch(this->type)
					{
						case TValue::EType::NULLVALUE: return true;
						case TValue::EType::UNDEFINED: return true;
						case TValue::EType::BOOL:      return this->b == other.b;
						case TValue::EType::NUMBER:    return this->n == other.n;
						case TValue::EType::STRING:    return this->s == other.s || (this->s != NULL && other.s != NULL && *this->s == *other.s);
						case TValue::EType::ARRAY:     return this->array == other.array || (this->array != NULL && other.array != NULL && *this->array == *other.array);
						case TValue::EType::OBJECT:    return this->o == other.o || (this->o != NULL && other.o != NULL && *this->o == *other.o);
					}

					CL3_UNREACHABLE;
					return false;
				}

				TValue& TValue::operator=(const TValue& other)
				{
					switch(this->type)
					{
						case TValue::EType::NULLVALUE:
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::BOOL:
							this->b = other.b;
							break;

						case TValue::EType::NUMBER:
							this->n = other.n;
							break;

						case TValue::EType::STRING:
							delete this->s;
							this->s = other.s ? new TString(*other.s) : NULL;
							break;

						case TValue::EType::ARRAY:
							delete this->array;
							this->array = other.array ? new TArray(*other.array) : NULL;
							break;

						case TValue::EType::OBJECT:
							delete this->o;
							this->o = other.o ? new TObject(*other.o) : NULL;
							break;
					}

					this->type = other.type;

					return *this;
				}

				TValue& TValue::operator=(TValue&& other)
				{
					switch(this->type)
					{
						case TValue::EType::NULLVALUE:
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::BOOL:
							this->b = other.b;
							break;

						case TValue::EType::NUMBER:
							this->n = other.n;
							break;

						case TValue::EType::STRING:
						case TValue::EType::ARRAY:
						case TValue::EType::OBJECT:
							this->o = other.o;
							other.o = NULL;
							break;
					}

					this->type = other.type;

					return *this;
				}

				CLASS TValue::TValue(EType type) : type(type)
				{
					this->n = 0;
				}

				CLASS TValue::TValue(const io::text::string::TString& text) : type(TValue::EType::STRING)
				{
					this->s = new TString(text);
				}

				CLASS TValue::TValue(bool b) : type(TValue::EType.BOOL)
				{
					this->b = b;
				}

				CLASS TValue::TValue(f64_t number) : type(TValue::EType::NUMBER)
				{
					this->n = number;
				}

				CLASS TValue::TValue(TValue&& other) : type(other.type)
				{
					switch(other.type)
					{
						case TValue::EType::NULLVALUE:
							break;

						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::BOOL:
							this->b = other.b;
							break;

						case TValue::EType::NUMBER:
							this->n = other.n;
							break;

						case TValue::EType::STRING:
						case TValue::EType::ARRAY:
						case TValue::EType::OBJECT:
							this->o = other.o;
							other.o = NULL;
							break;
					}
				}

				CLASS TValue::TValue(const TValue& other) : type(other.type)
				{
					switch(other.type)
					{
						case TValue::EType::NULLVALUE:
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::BOOL:
							this->b = other.b;
							break;

						case TValue::EType::NUMBER:
							this->n = other.n;
							break;

						case TValue::EType::STRING:
							this->s = other.s ? new TString(*other.s) : NULL;
							break;

						case TValue::EType::ARRAY:
							this->a = other.a ? new TArray(*other.a) : NULL;
							break;

						case TValue::EType::OBJECT:
							this->o = other.o ? new TObject(*other.o) : NULL;
							break;
					}
				}

				CLASS TValue::~TValue()
				{
					switch(this->type)
					{
						case TValue::EType::NULLVALUE:
						case TValue::EType::UNDEFINED:
						case TValue::EType::BOOL:
						case TValue::EType::NUMBER:
							break;

						case TValue::EType::STRING:
							delete this->s;
							break;

						case TValue::EType::ARRAY:
							delete this->array;
							break;

						case TValue::EType::OBJECT:
							delete this->o;
							break;
					}
				}

				static serialization::EDatatype Map2SerializationType(TValue::Etype json_type)
				{
				}

				static bool IsExplicitTypeInfoRequired(const TFormat& f)
				{
				}

				void TObject::Serialize(serialization::ISerializer& s) const
				{
					const bool requires_explicit_ti = IsExplicitTypeInfoRequired(s.Format());

					for(usys_t i = 0; i < this->members.Count(); i++)

				}

				void TObject::Deserialize(serialization::IDeserializer& ds)
				{
				}

				void TArray::Serialize(serialization::ISerializer& s) const
				{
				}

				void TArray::Deserialize(serialization::IDeserializer& ds)
				{
				}
			}
		}
	}
}
