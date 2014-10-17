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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "io_serialization_json.hpp"
#include "io_text.hpp"
#include "io_text_string.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	serialization
		{
			namespace	json
			{
				using namespace io::text;
				using namespace io::text::string;

				void	TJSONSerializer::BeginNewElement	(const char* name)
				{
					if(!this->empty_scope)
					{
						(*tw)<<',';
					}
					this->empty_scope = false;

					if(this->pretty)
					{
						(*tw)<<'\n';
						for(usys_t i = 0; i < this->depth; i++)
							(*tw)<<'\t';

						(*tw)<<'"'<<name<<"\" : ";
					}
					else
						(*tw)<<'"'<<name<<"\":";
				}

				void	TJSONSerializer::Push		(const char* name, u8_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, s8_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, u16_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, s16_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, u32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, s32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, u64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, s64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, f32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, f64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push		(const char* name, const char* value)
				{
					BeginNewElement(name);
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONSerializer::Push		(const char* name, const wchar_t* value)
				{
					BeginNewElement(name);
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONSerializer::Push		(const char* name, const TString& value)
				{
					BeginNewElement(name);
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONSerializer::Push		(const char* name, const ISerializable& value)
				{
					BeginNewElement(name);
					TJSONSerializer s(this->tw, this->pretty, this->depth);
					value.Serialize(s);
				}

				system::memory::TUniquePtr<IArraySerializer>
						TJSONSerializer::PushArray	(const char* name)
				{
					BeginNewElement(name);
					return system::memory::MakeUniquePtr<IArraySerializer>(new TJSONArraySerializer(this->tw, this->pretty, this->depth));
				}

				CLASS	TJSONSerializer::TJSONSerializer	(ITextWriter* tw, bool pretty, usys_t depth) : tw(tw), pretty(pretty), empty_scope(true), depth(depth+1)
				{
					(*tw)<<'{';
				}

				CLASS	TJSONSerializer::~TJSONSerializer	()
				{
					if(this->pretty && !this->empty_scope)
						(*tw)<<'\n';

					if(this->pretty)
						for(usys_t i = 1; i < this->depth; i++)
							(*tw)<<'\t';
					(*tw)<<'}';
				}

				/*************************************************************************************/

				void	TJSONDeserializer::Pop		(const char* name, u8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, s8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, u16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, s16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, u32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, s32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, u64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, s64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, f32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, f64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, const char*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, const wchar_t*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, TString& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, ISerializable& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop		(const char* name, void* object, const system::types::typeinfo::TRTTI& rtti)
				{
					CL3_NOT_IMPLEMENTED;
				}

				system::memory::TUniquePtr<IArrayDeserializer>
						TJSONDeserializer::PopArray	(const char* name)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TJSONDeserializer::TJSONDeserializer	(ITextReader* tr) : tr(tr)
				{
				}

				CLASS	TJSONDeserializer::~TJSONDeserializer	()
				{
					TUTF32 chr;
					tr->Read(&chr,1);
				}

				/*************************************************************************************/
				/*************************************************************************************/
				/*************************************************************************************/

				void	TJSONArraySerializer::BeginNewElement	()
				{
					if(!this->empty_scope)
					{
						if(this->pretty)
							(*tw)<<", ";
						else
							(*tw)<<',';
					}
					this->empty_scope = false;
				}

				void	TJSONArraySerializer::Push		(u8_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(s8_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(u16_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(s16_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(u32_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(s32_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(u64_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(s64_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(f32_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(f64_t value)
				{
					BeginNewElement();
					(*tw)<<value;
				}

				void	TJSONArraySerializer::Push		(const char* value)
				{
					BeginNewElement();
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONArraySerializer::Push		(const wchar_t* value)
				{
					BeginNewElement();
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONArraySerializer::Push		(const TString& value)
				{
					BeginNewElement();
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONArraySerializer::Push		(const ISerializable& value)
				{
					BeginNewElement();
					TJSONSerializer s(this->tw, this->pretty, this->depth);
					value.Serialize(s);
				}

				system::memory::TUniquePtr<IArraySerializer>
						TJSONArraySerializer::PushArray	()
				{
					BeginNewElement();
					return system::memory::MakeUniquePtr<IArraySerializer>(new TJSONArraySerializer(this->tw, this->pretty, this->depth));
				}

				CLASS	TJSONArraySerializer::TJSONArraySerializer	(ITextWriter* tw, bool pretty, usys_t depth) : tw(tw), pretty(pretty), empty_scope(true), depth(depth+1)
				{
					if(this->pretty)
						(*tw)<<"[ ";
					else
						(*tw)<<'[';
				}

				CLASS	TJSONArraySerializer::~TJSONArraySerializer	()
				{
					if(this->pretty)
						(*tw)<<" ]";
					else
						(*tw)<<']';
				}

				/*************************************************************************************/

				void	TJSONArrayDeserializer::Pop		(u8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(s8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(u16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(s16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(u32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(s32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(u64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(s64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(f32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(f64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(const char*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(const wchar_t*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(TString& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(ISerializable& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				system::memory::TUniquePtr<IArrayDeserializer>
						TJSONArrayDeserializer::PopArray	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONArrayDeserializer::Pop		(void* object, const system::types::typeinfo::TRTTI& rtti)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TJSONArrayDeserializer::TJSONArrayDeserializer	(ITextReader* tr) /*: tr(tr)*/
				{
				}

				CLASS	TJSONArrayDeserializer::~TJSONArrayDeserializer	()
				{
				}

				/*************************************************************************************/
			}
		}
	}
}
