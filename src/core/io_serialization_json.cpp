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

namespace	cl3
{
	namespace	io
	{
		namespace	serialization
		{
			namespace	json
			{
				using namespace io::text;

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

				void	TJSONSerializer::Push	(const char* name, u8_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, s8_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, u16_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, s16_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, u32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, s32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, u64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, s64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, f32_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, f64_t value)
				{
					BeginNewElement(name);
					(*tw)<<value;
				}

				void	TJSONSerializer::Push	(const char* name, const char* value)
				{
					BeginNewElement(name);
					(*tw)<<'"'<<value<<'"';
				}

				void	TJSONSerializer::Push	(const char* name, const wchar_t* value)
				{
					BeginNewElement(name);
					(*tw)<<'"'<<value<<'"';
				}


				void	TJSONSerializer::Push	(const char* name, const ISerializable& value)
				{
					BeginNewElement(name);
					(*tw)<<'{';
					this->empty_scope = true;
					this->depth++;
					value.Serialize(*this);
					this->depth--;
					if(this->pretty && !this->empty_scope)
					{
						(*tw)<<'\n';
						for(usys_t i = 0; i < this->depth; i++)
							(*tw)<<'\t';
					}
					(*tw)<<'}';
					this->empty_scope = false;
				}

				CLASS			TJSONSerializer::TJSONSerializer	(ITextWriter* tw, bool pretty) : tw(tw), pretty(pretty), empty_scope(true), depth(1)
				{
					(*tw)<<'{';
				}

				CLASS			TJSONSerializer::~TJSONSerializer	()
				{
					if(this->pretty && !this->empty_scope)
						(*tw)<<'\n';
					(*tw)<<'}';
				}

				/*************************************************************************************/

				void	TJSONDeserializer::Pop	(const char* name, u8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, s8_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, u16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, s16_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, u32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, s32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, u64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, s64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, f32_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, f64_t& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, const char*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, const wchar_t*& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	TJSONDeserializer::Pop	(const char* name, ISerializable& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS				TJSONDeserializer::TJSONDeserializer	(ITextReader* tr) : tr(tr)
				{
				}

				CLASS				TJSONDeserializer::~TJSONDeserializer	()
				{
					TUTF32 chr;
					tr->Read(&chr,1);
				}
			}
		}
	}
}
