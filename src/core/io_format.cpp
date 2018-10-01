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

#include "io_format.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			using namespace io::text::string;

			bool TValue::operator==(const TValue& other) const
			{
				if(this->type != other.type)
					return false;

				switch(this->type)
				{
					case TValue::EType::UNDEFINED: return true;
					case TValue::EType::SINT8:   return this->s8  == other.s8;
					case TValue::EType::SINT16:  return this->s16 == other.s16;
					case TValue::EType::SINT32:  return this->s32 == other.s32;
					case TValue::EType::SINT64:  return this->s64 == other.s64;
					case TValue::EType::UINT8:   return this->u8  == other.s8;
					case TValue::EType::UINT16:  return this->u16 == other.u16;
					case TValue::EType::UINT32:  return this->u32 == other.u32;
					case TValue::EType::UINT64:  return this->u64 == other.u64;
					case TValue::EType::FLOAT32: return this->f32 == other.f32;
					case TValue::EType::FLOAT64: return this->f64 == other.f64;
					case TValue::EType::STRING:  return this->text == other.text || (this->text != NULL && other.text != NULL && *this->text == *other.text);
					case TValue::EType::ARRAY:   return this->array == other.array || (this->array != NULL && other.array != NULL && *this->array == *other.array);
					case TValue::EType::OBJECT:  return this->object == other.object || (this->object != NULL && other.object != NULL && *this->object == *other.object);
				}

				CL3_UNREACHABLE;
				return false;
			}

			TValue& TValue::operator=(const TValue& other)
			{
				switch(this->type)
				{
					case TValue::EType::UNDEFINED:
					case TValue::EType::SINT8:
					case TValue::EType::SINT16:
					case TValue::EType::SINT32:
					case TValue::EType::SINT64:
					case TValue::EType::UINT8:
					case TValue::EType::UINT16:
					case TValue::EType::UINT32:
					case TValue::EType::UINT64:
					case TValue::EType::FLOAT32:
					case TValue::EType::FLOAT64:
						this->s64 = other.s64;
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
					case TValue::EType::UNDEFINED:
					case TValue::EType::SINT8:
					case TValue::EType::SINT16:
					case TValue::EType::SINT32:
					case TValue::EType::SINT64:
					case TValue::EType::UINT8:
					case TValue::EType::UINT16:
					case TValue::EType::UINT32:
					case TValue::EType::UINT64:
					case TValue::EType::FLOAT32:
					case TValue::EType::FLOAT64:
						this->s64 = other.s64;
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

			CLASS TValue::TValue() : type(TValue::EType::UNDEFINED)
			{
				this->s64 = 0;
			}

			CLASS TValue::TValue(TValue&& other) : type(other.type)
			{
				switch(other.type)
				{
					case TValue::EType::UNDEFINED:
					case TValue::EType::SINT8:
					case TValue::EType::SINT16:
					case TValue::EType::SINT32:
					case TValue::EType::SINT64:
					case TValue::EType::UINT8:
					case TValue::EType::UINT16:
					case TValue::EType::UINT32:
					case TValue::EType::UINT64:
					case TValue::EType::FLOAT32:
					case TValue::EType::FLOAT64:
						this->s64 = other.s64;
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
					case TValue::EType::UNDEFINED:
					case TValue::EType::SINT8:
					case TValue::EType::SINT16:
					case TValue::EType::SINT32:
					case TValue::EType::SINT64:
					case TValue::EType::UINT8:
					case TValue::EType::UINT16:
					case TValue::EType::UINT32:
					case TValue::EType::UINT64:
					case TValue::EType::FLOAT32:
					case TValue::EType::FLOAT64:
						this->s64 = other.s64;
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

			CLASS TValue::~TValue()
			{
				switch(this->type)
				{
					case TValue::EType::UNDEFINED:
					case TValue::EType::SINT8:
					case TValue::EType::SINT16:
					case TValue::EType::SINT32:
					case TValue::EType::SINT64:
					case TValue::EType::UINT8:
					case TValue::EType::UINT16:
					case TValue::EType::UINT32:
					case TValue::EType::UINT64:
					case TValue::EType::FLOAT32:
					case TValue::EType::FLOAT64:
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

			bool TMember::operator==(const TMember& other) const
			{
				return this->name == other.name && this->value == other.value;
			}

			bool TArray::operator==(const TArray& other) const
			{
				if(this->items.Count() != other.items.Count())
					return false;
				for(usys_t i = 0; i < this->items.Count(); i++)
					if(this->items[i] != other.items[i])
						return false;
				return true;
			}

			bool TObject::operator==(const TObject& other) const
			{
				if(this->members.Count() != other.members.Count())
					return false;
				for(usys_t i = 0; i < this->members.Count(); i++)
					if(this->members[i] != other.members[i])
						return false;
				return true;
			}
		}
	}
}
