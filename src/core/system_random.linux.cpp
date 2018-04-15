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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cl3/core/system_random.hpp>

namespace cl3
{
	namespace system
	{
		namespace random
		{
			CLASS TCryptoRandom::TCryptoRandom()
			{
				CL3_CLASS_SYSERR((io::stream::fd::TFDStream&)(*this) = (fd_t)::open("/dev/urandom", O_RDONLY|O_NOCTTY|O_CLOEXEC));
			}
		}
	}
}
