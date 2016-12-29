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
						case TValue::EType::NUMBER:    return this->num == other.num;
						case TValue::EType::STRING:    return this->text == other.text || (this->text != NULL && other.text != NULL && *this->text == *other.text);
						case TValue::EType::ARRAY:     return this->array == other.array || (this->array != NULL && other.array != NULL && *this->array == *other.array);
						case TValue::EType::OBJECT:    return this->object == other.object || (this->object != NULL && other.object != NULL && *this->object == *other.object);
					}

					CL3_UNREACHABLE;
					return false;
				}

				TValue& TValue::operator=(const TValue& other)
				{
					switch(this->type)
					{
						case TValue::EType::NULLVALUE:
							break;
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::NUMBER:
							this->num = other.num;
							break;

						case TValue::EType::STRING:
							delete this->text;
							this->text = other.text ? new TString(*other.text) : NULL;
							break;

						case TValue::EType::ARRAY:
							delete this->array;
							this->array = other.array ? new TArray(*other.array) : NULL;
							break;

						case TValue::EType::OBJECT:
							delete this->object;
							this->object = other.object ? new TObject(*other.object) : NULL;
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
							break;
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::NUMBER:
							this->num = other.num;
							break;

						case TValue::EType::STRING:
						case TValue::EType::ARRAY:
						case TValue::EType::OBJECT:
							this->object = other.object;
							other.object = NULL;
							break;
					}

					this->type = other.type;

					return *this;
				}

				CLASS TValue::TValue(EType type) : type(type)
				{
					this->num = 0;
				}

				CLASS TValue::TValue(const io::text::string::TString& text) : type(TValue::EType::STRING)
				{
					this->text = new TString(text);
				}

				CLASS TValue::TValue(f64_t number) : type(TValue::EType::NUMBER)
				{
					this->num = number;
				}

				CLASS TValue::TValue(TValue&& other) : type(other.type)
				{
					switch(other.type)
					{
						case TValue::EType::NULLVALUE:
							break;

						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::NUMBER:
							this->num = other.num;
							break;

						case TValue::EType::STRING:
						case TValue::EType::ARRAY:
						case TValue::EType::OBJECT:
							this->object = other.object;
							other.object = NULL;
							break;
					}
				}

				CLASS TValue::TValue(const TValue& other) : type(other.type)
				{
					switch(other.type)
					{
						case TValue::EType::NULLVALUE:
							break;
						case TValue::EType::UNDEFINED:
							break;

						case TValue::EType::NUMBER:
							this->num = other.num;
							break;

						case TValue::EType::STRING:
							this->text = other.text ? new TString(*other.text) : NULL;
							break;

						case TValue::EType::ARRAY:
							this->array = other.array ? new TArray(*other.array) : NULL;
							break;

						case TValue::EType::OBJECT:
							this->object = other.object ? new TObject(*other.object) : NULL;
							break;
					}
				}

				CLASS TValue::TValue(serialization::IDeserializer& ds) : type(EType.UNDEFINED)
				{
					this->Deserialize(ds);
				}

				CLASS TValue::~TValue()
				{
					switch(this->type)
					{
						case TValue::EType::NULLVALUE:
							break;

						case TValue::EType::UNDEFINED:
						case TValue::EType::NUMBER:
							break;

						case TValue::EType::STRING:
							delete this->text;
							break;

						case TValue::EType::ARRAY:
							delete this->array;
							break;

						case TValue::EType::OBJECT:
							delete this->object;
							break;
					}
				}
			}
		}
	}
}
