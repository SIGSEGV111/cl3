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

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "error.hpp"
#include "system_task_synchronization.hpp"
#include "system_task.hpp"
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// static inline bool operator> (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) >  0; }
// static inline bool operator< (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) <  0; }
// static inline bool operator==(const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) == 0; }

namespace	cl3
{
// 	namespace	event
// 	{
// 		template class TEvent<system::task::synchronization::IMutex, system::task::synchronization::TOnMutexActionData>;
// 	}

	namespace	system
	{
		namespace	task
		{
			namespace	synchronization
			{
				using namespace error;
				using namespace system::time;
				using namespace io::collection;
				using namespace io::collection::list;
				using namespace io::collection::array;
			}
		}
	}
}

#endif
