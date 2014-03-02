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

#ifndef	_include_cl3_core_system_def_hpp_
#define	_include_cl3_core_system_def_hpp_

#ifndef	NULL
	#define	NULL	(0U)
#endif

#define	CLASS

#ifdef CL3_DEBUG
	#define	CL3_IFDBG(expr) expr
#else
	#define	CL3_IFDBG(expr)
#endif

namespace	cl3
{
	namespace	system
	{
		namespace	def
		{
			template<class T>
			inline static void swap(T& a, T& b)
			{
				T tmp = a;
				a = b;
				b = tmp;
			}

			const static unsigned long CL3_OPT__PAGE_SIZE = 4096U;	//	the smallest size of a page in the system
			const static unsigned long CL3_OPT__SYSTEM_ALIGN = 16;	//	the minimum memory alignment which is enforced automatically by the system libraries (malloc & co.)

			#define CL3_PASTE2(x, y) x##y
			#define CL3_PASTE(x, y) CL3_PASTE2(x, y)

			#define	CL3_MIN(a,b)	( ((a) < (b)) ? (a) : (b) )
			#define	CL3_MAX(a,b)	( ((a) > (b)) ? (a) : (b) )
		}
	}
}

#endif
