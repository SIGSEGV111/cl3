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

#ifndef INSIDE_CL3CORE
#error "compiling cl3 source code but macro INSIDE_CL3CORE is not defined"
#endif

#include "system_os.hpp"
#if	(CL3_OS == CL3_OS_WINDOWS)

#include "system_library.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	library
		{
			CLASS	TLibrary::TLibrary		(const io::text::string::TString& libname)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TLibrary::TLibrary		(const char* libname)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TLibrary::~TLibrary		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void*	TLibrary::ResolveSymbol	(const io::text::string::TString& name)
			{
				CL3_NOT_IMPLEMENTED;
			}

			void*	TLibrary::ResolveSymbol	(const char* name)
			{
				CL3_NOT_IMPLEMENTED;
			}
		}
	}
}

#endif
