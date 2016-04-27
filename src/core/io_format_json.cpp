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

				void SerializeJSON(const TValue& root, io::text::ITextWriter& w)
				{
					switch(root.type)
					{
						case TValue::EType::UNDEFINED: w<<"null"; break;
						case TValue::EType::SINT8:   w<<root.s8; break;
						case TValue::EType::SINT16:  w<<root.s16; break;
						case TValue::EType::SINT32:  w<<root.s32; break;
						case TValue::EType::SINT64:  w<<root.s64; break;
						case TValue::EType::UINT8:   w<<root.u8; break;
						case TValue::EType::UINT16:  w<<root.u16; break;
						case TValue::EType::UINT32:  w<<root.u32; break;
						case TValue::EType::UINT64:  w<<root.u64; break;
						case TValue::EType::FLOAT32: w<<root.f32; break;
						case TValue::EType::FLOAT64: w<<root.f64; break;
						case TValue::EType::STRING:
						{
							TString copy = root.text ? *root.text : "";
							copy.Replace("\\", "\\\\");
							copy.Replace("\"", "\\\"");
							w<<'"'<<copy<<'"';
							break;
						}
						case TValue::EType::ARRAY:
							w<<'[';
							for(usys_t i = 0; i < root.array->items.Count(); i++)
							{
								SerializeJSON(root.array->items[i], w);
								if(i + 1 < root.array->items.Count())
									w<<',';
							}
							w<<']';
							break;
						case TValue::EType::OBJECT:
							w<<'{';
							for(usys_t i = 0; i < root.object->members.Count(); i++)
							{
								w<<'\"'<<root.object->members[i].name<<"\":";
								SerializeJSON(root.object->members[i].value, w);
								if(i + 1 < root.object->members.Count())
									w<<',';
							}
							w<<'}';
							break;
					}
				}

				TValue DeserializeJSON(io::text::ITextReader&)
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
