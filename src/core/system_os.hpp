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

#ifndef	_include_cl3_core_system_os_hpp_
#define	_include_cl3_core_system_os_hpp_

#include "system_def.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	os
		{
			enum	EOSClass
			{
				OS_CLASS_POSIX,
				OS_CLASS_WINDOWS
			};

			enum	EOSDerivative
			{
				OS_DERIVATIVE_POSIX_LINUX,
				OS_DERIVATIVE_POSIX_MACOS,
				OS_DERIVATIVE_POSIX_OPENBSD,
				OS_DERIVATIVE_POSIX_FREEBSD,
				OS_DERIVATIVE_POSIX_NETBSD,
				OS_DERIVATIVE_POSIX_HPUX,
				OS_DERIVATIVE_WINDOWS_9X,
				OS_DERIVATIVE_WINDOWS_NT
			};

			#define	CL3_OS_POSIX	1	//	Linux, Unix, SunOS, MacOS ...
			#define	CL3_OS_WINDOWS	2	//	Windows 9x, NT, mobile, etc...

			#define	CL3_OS_DERIVATIVE_POSIX_OTHER	0
			#define	CL3_OS_DERIVATIVE_POSIX_LINUX	1
			#define	CL3_OS_DERIVATIVE_POSIX_MACOS	2

			#define	CL3_OS_DERIVATIVE_WINDOWS_OTHER	0
			#define	CL3_OS_DERIVATIVE_WINDOWS_9X	1
			#define	CL3_OS_DERIVATIVE_WINDOWS_NT	2

			#ifdef __linux__
				#define	CL3_OS		CL3_OS_POSIX
				#define	CL3_OS_DERIVATIVE CL3_OS_DERIVATIVE_POSIX_LINUX
				#define CL3_EXPORT_TLS
			#endif

			#ifndef CL3_OS
				#if (CL3_CXX == CL3_CXX_MSVC)
					#define CL3_OS CL3_OS_WINDOWS
				#endif
				#if (CL3_CXX == CL3_CXX_GCC)
					#define CL3_OS CL3_OS_POSIX
				#endif
			#endif

			#ifndef CL3_OS
				#error "unknown operating system"
			#endif

			#ifndef CL3_OS_DERIVATIVE
				#if (CL3_OS == CL3_OS_WINDOWS)
					#define	CL3_OS_DERIVATIVE	CL3_OS_DERIVATIVE_WINDOWS_NT
				#endif

				#if (CL3_OS == CL3_OS_POSIX)
					#define CL3_OS_DERIVATIVE CL3_OS_DERIVATIVE_POSIX_OTHER
				#else
					#error "unknown operating system variant"
				#endif
			#endif

			#if (CL3_OS == CL3_OS_POSIX)
				#define	PATHNAME_SEPARATOR	'/'
			#endif

			#if (CL3_OS == CL3_OS_WINDOWS)
				#define	PATHNAME_SEPARATOR	'\\'
			#endif
		}
	}
}

#endif
