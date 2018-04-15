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

#include <cl3/core/system_os.hpp>
#if	(CL3_OS == CL3_OS_POSIX)

#include <cl3/core/system_library.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/error.hpp>
#include <dlfcn.h>

namespace	cl3
{
	namespace	system
	{
		namespace	library
		{
			using namespace io::collection;
			using namespace io::collection::list;
			using namespace io::text::string;
			using namespace io::text::encoding;
			using namespace error;

			TList<TString> library_search_path;

			//	NOTE: the error detection via dlerror() is not 100% thread safe... hope it never becomes a problem

			CLASS	TLibrary::TLibrary		(const TString& libname)
			{
				const usys_t pos_slash = libname.Find("/", 0, ESearchDirection::BACKWARD);	//	find the last '/' character in libname

				TCString cstr(CODEC_CXX_CHAR);

				if(pos_slash == (usys_t)-1 && libname.Left(3) != "lib")
				{
					TString tmp = libname;
					tmp.Prepend("lib");
					tmp.Append(".so");
					cstr = TCString(tmp, CODEC_CXX_CHAR);
				}
				else
					cstr = TCString(libname, CODEC_CXX_CHAR);

				int flags = RTLD_LAZY|RTLD_LOCAL;

				CL3_CLASS_ERROR( (this->handle = dlopen(cstr.Chars(), flags)) == NULL, TException, "dlerror() = %s", dlerror() );
			}

			CLASS	TLibrary::~TLibrary		()
			{
				dlclose(this->handle);
			}

			void*	TLibrary::ResolveSymbol	(const io::text::string::TString& name)
			{
				return this->ResolveSymbol(TCString(name, CODEC_CXX_CHAR).Chars());
			}

			void*	TLibrary::ResolveSymbol	(const char* name)
			{
				dlerror();	//	clear error status
				void* const addr = dlsym(this->handle, name);
				const char* const err = dlerror();
				CL3_CLASS_ERROR(addr == NULL && err != NULL, TException, err);
				return addr;
			}
		}
	}
}

#endif
