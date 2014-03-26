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

#include "system_types.hpp"
#include "system_types_typeinfo.hpp"
#include "util.hpp"
#include "io_text_string.hpp"
#include <string.h>

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				using namespace system::memory;
				using namespace io::text::string;

				//	from wikipedia: https://en.wikipedia.org/wiki/Jenkins_hash_function
				static u32 JenkinsHash(const byte *key, size_t len)
				{
					u32 hash = 0;
					for(size_t i = 0; i < len; ++i)
					{
						hash += key[i];
						hash += (hash << 10);
						hash ^= (hash >> 6);
					}
					hash += (hash << 3);
					hash ^= (hash >> 11);
					hash += (hash << 15);
					return hash;
				}

				u32 TRTTI::Hash() const
				{
					TUStringUPtr name(Name());
					return JenkinsHash(reinterpret_cast<const u8*>(name.Object()->ItemPtr(0)), name.Object()->Count() * 4);
				}





				struct	TReplace
				{
					const char* wrong;
					const char* correct;
				};

				static const TReplace arr_replace[] =
				{
					{"char", "s8"},
					{"unsigned char", "u8"},
					{"signed char", "s8"},

					{"short", "s16"},
					{"unsigned short", "u16"},

					{"int", "s32"},
					{"unsigned int", "u32"},

					{"long", sizeof(long) == 4 ? "s32" : sizeof(long) == 8 ? "s64" : NULL },
					{"unsigned long", sizeof(long) == 4 ? "u32" : sizeof(long) == 8 ? "u64" : NULL },

					{"__int64", "s64"},
					{"unsigned __int64", "u64"},

					{"long long", "s64"},
					{"unsigned long long", "u64"},

					{"float", sizeof(float) == 4 ? "f32" : sizeof(float) == 8 ? "f64" : NULL },
					{"double", sizeof(double) == 4 ? "f32" : sizeof(double) == 8 ? "f64" : NULL },

					{"wchar_t", sizeof(wchar_t) == 1 ? "u8" : sizeof(wchar_t) == 2 ? "u16" : sizeof(wchar_t) == 4 ? "u32" : NULL },

					{NULL,NULL}
				};

				static	const char*	FindReplace	(const char* token, size_t token_length)
				{
					for(size_t i = 0; arr_replace[i].wrong != NULL; i++)
					{
						if(strlen(arr_replace[i].wrong) == token_length)
						{
							if(strncmp(arr_replace[i].wrong, token, token_length) == 0)
								return arr_replace[i].correct;
						}
					}
					return NULL;
				}

				TUniquePtr<char,UPTR_MALLOC> UnifyTypename(const char* oldname)
				{
					util::TCoreList<char> newname;

					const char* token_start = oldname;
					for(size_t i = 0; ; ++i)
					{
						switch(oldname[i])
						{
							//	end of token markers:
							case ' ':
							case ',':
							case '*':
							case ':':
							case '\t':
							case '<':
							case '>':
							case '(':
							case ')':
							case '{':
							case '}':
							case '[':
							case ']':
							case '\0':
							{
								const size_t token_length = i - (token_start - oldname);
								size_t offset = 0;

								/*if( token_length > 0 && *token_end == ']' )
								{
									offset = token_length;
								}
								else */

								if( token_length >= 5 && memcmp("class",  token_start, 5) == 0 )
								{
									offset = 5;
								}
								else if( token_length >= 5 && memcmp("union",  token_start, 5) == 0 )
								{
									offset = 5;
								}
								else if( token_length >= 6 && memcmp("struct",  token_start, 6) == 0 )
								{
									offset = 6;
								}
								else if( token_length >= 4 && memcmp("enum",  token_start, 4) == 0 )
								{
									offset = 4;
								}
								else if( token_length >= 7 && memcmp("__ptr64", token_start, 7) == 0 )
								{
									offset = 7;
								}

								if(token_length > offset)
								{
									const char* const replace = FindReplace(token_start + offset, token_length - offset);

									if(replace)
										newname.Push(replace, strlen(replace));
									else
										newname.Push(token_start + offset, token_length - offset);
								}

								switch(oldname[i])
								{
									//	eat anything that (hopefully) is not required to make the name stable and unique
									case ' ':
									case '(':
									case ')':
									case '\t':
										break;
									default:
										newname.Push(oldname[i]);
								}

								token_start = oldname + i + 1;
								break;
							}
						}
						if(oldname[i] == '\0') break;
					}

					TUniquePtr<char,UPTR_MALLOC> _tmp(MakeUniquePtr<UPTR_MALLOC>(newname.Claim()));
					return _tmp;
				}
			}
		}
	}
}
