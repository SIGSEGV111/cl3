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

#if (CL3_OS == CL3_OS_POSIX)

#include <signal.h>
#include <cl3/core/system_compiler.hpp>

namespace cl3
{
	namespace system
	{
		namespace compiler
		{
			void DebugBreak()
			{
				raise(SIGTRAP);
			}
		}
	}
}

#endif
