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

#include <cl3/core/error.hpp>
#include <cl3/core/system_os.hpp>
#include <cl3/core/system_library.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::library;

	TEST(system_library_TLibrary, load_unload_ct)
	{
		TLibrary lib_core("cl3-core");
	}

	TEST(system_library_TLibrary, load_unload_rt)
	{
		TLibrary lib_mm("cl3-mm");
		TLibrary lib_db("cl3-db");
	}

	TEST(system_library_TLibrary, ResolveSymbol)
	{
		#if	(CL3_OS == CL3_OS_POSIX)
			TLibrary lib("rt");
			EXPECT_TRUE(lib.ResolveSymbol("aio_suspend") != NULL);
			EXPECT_TRUE(lib.ResolveSymbol("aio_error") != NULL);
			EXPECT_TRUE(lib.ResolveSymbol("aio_fsync") != NULL);
		#elif (CL3_OS == CL3_OS_WINDOWS)
			TLibrary lib("gdi32");
			EXPECT_TRUE(lib.ResolveSymbol("BitBlt") != NULL);
			//	TODO
		#else
			#error "unsupported platform"
		#endif
	}

	TEST(system_library_TLibrary, LoadSymbol)
	{
		#if	(CL3_OS == CL3_OS_POSIX)
			TLibrary lib("rt");
			int (*aio_suspend)(const struct aiocb * const aiocb_list[], int nitems, const struct timespec *timeout);
			lib.LoadSymbol("aio_suspend", aio_suspend);
			EXPECT_TRUE(aio_suspend != NULL);
		#elif (CL3_OS == CL3_OS_WINDOWS)
			TLibrary lib("gdi32");
			throw; //	TODO
		#else
			#error "unsupported platform"
		#endif
	}
}
