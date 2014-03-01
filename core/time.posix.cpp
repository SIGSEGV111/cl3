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

#include "time.hpp"
#include "error.hpp"
#include "system_types_typeinfo.hpp"

#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

namespace	cl3
{
	namespace	time
	{
		TTime	TTime::Now			(EClock clock)
		{
			timespec ts;
			clockid_t id;
			rusage ru;

			switch(clock)
			{
				case TIME_CLOCK_REALTIME:
					id = CLOCK_REALTIME;
					break;

				case TIME_CLOCK_MONOTONIC:
					#if (_OST == OST_LINUX)
						id = CLOCK_MONOTONIC_RAW;
					#else
						id = CLOCK_MONOTONIC;
					#endif
					break;

				case TIME_CLOCK_PROCESS:
					id = CLOCK_PROCESS_CPUTIME_ID;
					break;

				case TIME_CLOCK_THREAD:
					id = CLOCK_THREAD_CPUTIME_ID;
					break;

				case TIME_CLOCK_PROCESS_USER:
					CL3_SYSERR(getrusage(RUSAGE_SELF, &ru));
					return TTime(ru.ru_utime.tv_sec, (s64)ru.ru_utime.tv_usec * (s64)1000000000000);

				case TIME_CLOCK_PROCESS_SYS:
					CL3_SYSERR(getrusage(RUSAGE_SELF, &ru));
					return TTime(ru.ru_stime.tv_sec, (s64)ru.ru_stime.tv_usec * (s64)1000000000000);

				case TIME_CLOCK_THREAD_USER:
					CL3_SYSERR(getrusage(RUSAGE_THREAD, &ru));
					return TTime(ru.ru_utime.tv_sec, (s64)ru.ru_utime.tv_usec * (s64)1000000000000);

				case TIME_CLOCK_THREAD_SYS:
					CL3_SYSERR(getrusage(RUSAGE_THREAD, &ru));
					return TTime(ru.ru_stime.tv_sec, (s64)ru.ru_stime.tv_usec * (s64)1000000000000);

				default:
				{
					char buffer[16] = {};
					snprintf(buffer, 16, "%d", clock);
					CL3_FAIL(error::TCoreArgumentException, &typeinfo::TCTTI<EClock>::rtti, "clock", buffer);
				}
			}

			CL3_SYSERR(clock_gettime(id, &ts));
			return TTime((s64)ts.tv_sec, (s64)ts.tv_nsec * (s64)1000000000);
		}

		/*TTime	TTime::LocalTime	() const
		{
			struct ::timeval tv;
			struct ::timezone tz;
			::gettimeofday(&tv, &tz);
			return TTime(sec - (tz.tz_minuteswest * 60LL), asec);
		}*/
	}
}

#endif
