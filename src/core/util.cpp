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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "util.hpp"
#include "system_memory.hpp"
#include "system_types_typeinfo.hpp"
#include "error.hpp"
#include "io_text.hpp"

namespace	cl3
{
	namespace	util
	{
		using namespace system::memory;
		using namespace system::types;

		TUniquePtr<char,UPTR_MALLOC> MakeCStringCopy(const char* str)
		{
			usys_t l = ::strlen(str) + 1;
			auto cpy = MakeUniquePtr<UPTR_MALLOC>((char*)malloc(l));
			::memcpy(cpy.Object(), str, l);
			return cpy;
		}

		static const char hex[] = "0123456789abcdef";

		static	bool	IsPrintable	(char c)
		{
			return c >= 32 && c < 127;
		}

		void	Hexdump	(const void* p_mem_, usys_t sz_mem, io::text::ITextWriter& w)
		{
			const char* const p_mem = (const char*)p_mem_;
			const usys_t base = (usys_t)p_mem_;

			char line[] = "[    ]                 :                                                 |                |\n";

			for(usys_t i = 0; i < sz_mem; i += 16)
			{
				line[1] = '0' + (i/16)/1000;
				line[2] = '0' + ((i/16)%1000)/100;
				line[3] = '0' + ((i/16)%100)/10;
				line[4] = '0' + ((i/16)%10);

				//TODO: fix this properly

				#if(UINTPTR_MAX > UINT32_MAX)
				{
					line[ 7] = hex[((base+i) >> 60) & 0x0f];
					line[ 8] = hex[((base+i) >> 56) & 0x0f];
					line[ 9] = hex[((base+i) >> 52) & 0x0f];
					line[10] = hex[((base+i) >> 48) & 0x0f];
					line[11] = hex[((base+i) >> 44) & 0x0f];
					line[12] = hex[((base+i) >> 40) & 0x0f];
					line[13] = hex[((base+i) >> 36) & 0x0f];
					line[14] = hex[((base+i) >> 32) & 0x0f];
				}
				#endif

				line[15] = hex[((base+i) >> 28) & 0x0f];
				line[16] = hex[((base+i) >> 24) & 0x0f];
				line[17] = hex[((base+i) >> 20) & 0x0f];
				line[18] = hex[((base+i) >> 16) & 0x0f];
				line[19] = hex[((base+i) >> 12) & 0x0f];
				line[20] = hex[((base+i) >>  8) & 0x0f];
				line[21] = hex[((base+i) >>  4) & 0x0f];
				line[22] = hex[((base+i) >>  0) & 0x0f];

				for(usys_t j = 0; j < 16; j++)
				{
					if(i + j < sz_mem)
					{
						const u8_t c = p_mem[i + j];

						line[7+18+j*3+0] = hex[c >> 4];
						line[7+18+j*3+1] = hex[c & 0x0f];
						line[7+18+16*3+1+j] = IsPrintable(c) ? c : '.';
					}
					else
					{
						line[7+18+j*3+0] = ' ';
						line[7+18+j*3+1] = ' ';
						line[7+18+16*3+1+j] = ' ';
					}
				}

				w.Write(line, sizeof(line)-1);
			}
		}
	}
}
