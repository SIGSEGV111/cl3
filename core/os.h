/*
    libcl2 - common library version 2
    Copyright (C) 2010	Simon Brennecke

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

#ifndef	_include_cl3_system_os_h_
#define	_include_cl3_system_os_h_

#include "def.h"
#include "compiler.h"

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

			enum	EOSVersion
			{
				OS_VERSION_POSIX_LINUX_24,	//	Linux Kernel 2.4.xx
				OS_VERSION_POSIX_LINUX_26,	//	Linux Kernel 2.6.xx and 3.x.xx
				OS_VERSION_POSIX_MACOS_105,	//	MacOS X 10.5
				OS_VERSION_WINDOWS_9X_95,	//	Windows 95
				OS_VERSION_WINDOWS_9X_98,	//	Windows 98
				OS_VERSION_WINDOWS_9X_ME,	//	Windows Millenium
				OS_VERSION_WINDOWS_NT_40,	//	Windows NT 4.0
				OS_VERSION_WINDOWS_NT_50,	//	Windows 2000
				OS_VERSION_WINDOWS_NT_51,	//	Windows XP
				OS_VERSION_WINDOWS_NT_52,	//	Windows Server 2003
				OS_VERSION_WINDOWS_NT_60,	//	Windows Vista
				OS_VERSION_WINDOWS_NT_61,	//	Windows 7
			};

			#define	OS_POSIX	1	//	Linux, Unix, SunOS, MacOS ...
			#define	OS_WINDOWS	2	//	Windows 9x + NT

			#ifndef _OS
				#if (_CXX == CXX_MSVC)
					#define _OS	OS_WINDOWS
				#endif

				#if (_CXX == CXX_GNU)
					#define _OS	OS_POSIX
				#endif
			#endif

			#ifndef _OS
				#error "unknown operating system"
			#endif

			#define	OST_POSIX_OTHER	0
			#define	OST_POSIX_LINUX	1
			#define	OST_POSIX_MACOS	2

			#define	OST_WINDOWS_OTHER	0
			#define	OST_WINDOWS_NT		1

			#ifndef _OST
				#if (_OS == OS_POSIX)
					#define	OS_CLASS		cl2::os::OS_CLASS_POSIX

					#ifdef __linux__
						#define	_OST	OST_POSIX_LINUX

						#define	OS_DERIVATIVE	cl2::os::OS_DERIVATIVE_POSIX_LINUX
					#endif
				#elif (_OS == OS_WINDOWS)
					#define	_OST	OST_WINDOWS_NT

					#define	OS_CLASS		cl2::os::OS_CLASS_WINDOWS
					#define	OS_DERIVATIVE	cl2::os::OS_DERIVATIVE_WINDOWS_NT
				#endif
			#endif

			#ifndef _OST
				#if (_OS == OS_POSIX)
					#define _OST OST_POSIX_OTHER
				#elif (_OS == OS_WINDOWS)
					#define _OST OST_WINDOWS_OTHER
				#else
					#error "unknown operating system"
				#endif
			#endif

			#if (_OS == OS_POSIX)
				#include <unistd.h>
				#include <signal.h>

				#define	PATHNAME_SEPARATOR	'/'
				#define	DEBUGGER_BREAK	IFDBG(CL2_DBGLVL_BASIC, ::raise(SIGTRAP))
			#endif

			#if (_OS == OS_WINDOWS)
				#define	PATHNAME_SEPARATOR	'\\'
				#define	DEBUGGER_BREAK	IFDBG(CL2_DBGLVL_BASIC, _CrtDbgBreak())
			#endif

			#ifndef PATH_MAX
			#define	PATH_MAX	256
			#endif

			#if (_OS == OS_WINDOWS)
				#include <WinSock2.h>
			#endif
		}
	}
}

#endif
