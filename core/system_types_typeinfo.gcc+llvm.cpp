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

#include "system_compiler.hpp"

#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG)

#include "system_types_typeinfo.hpp"
#include "error-base.hpp"
#include "io_text_string.hpp"

#include <cxxabi.h>

namespace	cl3
{
	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				using namespace system::memory;
				using namespace io::text::string;

				TUStringUPtr UnifyTypename(const TUString& name);

				TUStringUPtr TRTTI::Name() const
				{
					TUString name;
					CL3_CLASS_ERROR((name = abi::__cxa_demangle(sys_type_info->name(), NULL, NULL, NULL)).Count() == 0, error::TException, "name decoding failed");
					return UnifyTypename(name);
				}
			}
		}
	}
}

#endif
