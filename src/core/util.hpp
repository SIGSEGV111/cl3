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

#ifndef	_include_cl3_core_util_hpp_
#define	_include_cl3_core_util_hpp_

#include "system_compiler.hpp"
#include "system_memory.hpp"
#include "system_def.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			class	ITextWriter;
		}
	}

	namespace	util
	{
		CL3PUBF	system::memory::TUniquePtr<char,system::memory::UPTR_ALLOC> MakeCStringCopy(const char* str, system::memory::IDynamicAllocator* allocator = NULL);

		CL3PUBF	void	Hexdump	(const void* p_mem, system::types::usys_t sz_mem, io::text::ITextWriter&);
	}
}

#endif
