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

#ifndef	_include_cl3_core_system_library_hpp_
#define	_include_cl3_core_system_library_hpp_

#include "system_os.hpp"
#include "system_compiler.hpp"
#include "io_text_string.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	library
		{
			CL3PUBV	io::collection::list::TList<io::text::string::TString> library_search_path;

			class	CL3PUBT	TLibrary
			{
				private:
					CLASS TLibrary(const TLibrary&) = delete;
					TLibrary operator=(const TLibrary&) = delete;

				protected:
					#if	(CL3_OS == CL3_OS_POSIX)
						void* handle;
					#elif (CL3_OS == CL3_OS_WINDOWS)
						HMODULE handle;
					#else
						#error "unsupported platform"
					#endif

				public:
					//	libname can either be a filename (e.g. "libtest.so", or "test.dll" - this will not work cross-platform)
					//	or libname can also only be a plain name (like "test"), the loader will then format the name correctly for the current platforms (e.g. append ".dll" or "lib<libname>.so")
					CL3PUBF	CLASS	TLibrary		(const io::text::string::TString& libname);
					CL3PUBF	CLASS	~TLibrary		();

					CL3PUBF	void*	ResolveSymbol	(const io::text::string::TString& name) CL3_WARN_UNUSED_RESULT;
					CL3PUBF	void*	ResolveSymbol	(const char* name) CL3_WARN_UNUSED_RESULT;

					template<class T>
					void	LoadSymbol	(const io::text::string::TString& name, T*& variable)
					{
						variable = reinterpret_cast<T*>(this->ResolveSymbol(name));
					}

					template<class T>
					void	LoadSymbol	(const char* name, T*& variable)
					{
						variable = reinterpret_cast<T*>(this->ResolveSymbol(name));
					}
			};
		}
	}
}

#endif
