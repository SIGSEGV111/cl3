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
#include "io_text_encoding_utf8.hpp"
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
				using namespace io::text;

				//	from wikipedia: https://en.wikipedia.org/wiki/Jenkins_hash_function
				static u32_t JenkinsHash(const byte_t *key, usys_t len)
				{
					u32_t hash = 0;
					for(usys_t i = 0; i < len; ++i)
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

				u32_t TRTTI::Hash() const
				{
					const TCString name(*Name().Object(), io::text::encoding::utf8::CODEC_UTF8);
					return JenkinsHash(reinterpret_cast<const u8_t*>(name.Chars()), name.Count());
				}





				struct	TReplace
				{
					const char* wrong;
					const char* correct;
				};

				static const TReplace arr_replace[] =
				{
					{"unsigned char", "u8_t"},
					{"signed char", "s8_t"},
					{"char", ( (char)-1 > (char)0 ? "u8_t" : "s8_t") },

					{"unsigned short", "u16_t"},
					{"signed short", "s16_t"},
					{"short", "s16_t"},

					{"unsigned int", "u32_t"},
					{"signed int", "s32_t"},
					{"int", "s32_t"},

					{"unsigned long long", sizeof(unsigned long long) == 4 ? "u32_t" : sizeof(unsigned long long) == 8 ? "u64_t" : NULL },
					{"signed long long", sizeof(signed long long) == 4 ? "s32_t" : sizeof(signed long long) == 8 ? "s64_t" : NULL },
					{"long long", sizeof(long long) == 4 ? "s32_t" : sizeof(long long) == 8 ? "s64_t" : NULL },

					{"unsigned long", sizeof(unsigned long) == 4 ? "u32_t" : sizeof(unsigned long) == 8 ? "u64_t" : NULL },
					{"signed long", sizeof(signed long) == 4 ? "s32_t" : sizeof(signed long) == 8 ? "s64_t" : NULL },
					{"long", sizeof(long) == 4 ? "s32_t" : sizeof(long) == 8 ? "s64_t" : NULL },

					{"unsigned __int8", "u8_t"},
					{"signed __int8", "s8_t"},
					{"__int8", "s8_t"},

					{"unsigned __int16", "u16_t"},
					{"signed __int16", "s16_t"},
					{"__int16", "s16_t"},

					{"unsigned __int32", "u32_t"},
					{"signed __int32", "s32_t"},
					{"__int32", "s32_t"},

					{"unsigned __int64", "u64_t"},
					{"signed __int64", "s64_t"},
					{"__int64", "s64_t"},

					{"float", sizeof(float) == 4 ? "f32_t" : sizeof(float) == 8 ? "f64_t" : NULL },
					{"double", sizeof(double) == 4 ? "f32_t" : sizeof(double) == 8 ? "f64_t" : NULL },

					{"wchar_t", sizeof(wchar_t) == 1 ? "u8_t" : sizeof(wchar_t) == 2 ? "u16_t" : sizeof(wchar_t) == 4 ? "u32_t" : NULL },
				};

				TUniquePtr<TString> UnifyTypename(const TString& oldname)
				{
					auto ret = MakeUniquePtr(new TString(oldname));
					for(usys_t i = 0; i < sizeof(arr_replace) / sizeof(TReplace); i++)
						ret->Replace(arr_replace[i].wrong, arr_replace[i].correct);
					return ret;

					/*TStringUPtr newname = MakeUniquePtr(new TString());

					const TUTF32* token_start = oldname.ItemPtr(0);
					for(usys_t i = 0; ; ++i)
					{
						switch(oldname[i].code)
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
								const usys_t token_length = i - (token_start - oldname.ItemPtr(0));
								usys_t offset = 0;

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
										newname.Object()->Append(replace, strlen(replace));
									else
										newname.Object()->Append(token_start + offset, token_length - offset);
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
										newname.Object()->Append(oldname[i]);
								}

								token_start = oldname + i + 1;
								break;
							}
						}
						if(oldname[i] == '\0') break;
					}

					return newname;*/
				}
			}
		}
	}
}
