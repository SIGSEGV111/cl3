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

#ifndef	INSIDE_CL3
#pragma message("ERROR: compiling cl3 source, but INSIDE_CL3 not defined")
#endif

#include "os.h"

#if (_OS == OS_POSIX)

#include "time.h"
#include "error.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

namespace	cl3
{
	namespace	system
	{
		namespace	time
		{
			void	__init		()
			{
			}

			void	__shutdown	()
			{
			}

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
						SYSERR(getrusage(RUSAGE_SELF, &ru));
						return TTime(ru.ru_utime.tv_sec, (s64)ru.ru_utime.tv_usec * (s64)1000000000000);

					case TIME_CLOCK_PROCESS_SYS:
						SYSERR(getrusage(RUSAGE_SELF, &ru));
						return TTime(ru.ru_stime.tv_sec, (s64)ru.ru_stime.tv_usec * (s64)1000000000000);

					case TIME_CLOCK_THREAD_USER:
						SYSERR(getrusage(RUSAGE_THREAD, &ru));
						return TTime(ru.ru_utime.tv_sec, (s64)ru.ru_utime.tv_usec * (s64)1000000000000);

					case TIME_CLOCK_THREAD_SYS:
						SYSERR(getrusage(RUSAGE_THREAD, &ru));
						return TTime(ru.ru_stime.tv_sec, (s64)ru.ru_stime.tv_usec * (s64)1000000000000);

					default:
						ERROR_S(error::TArgumentException, "clock", clock);
				}

				SYSERR(clock_gettime(id, &ts));
				return TTime((s64)ts.tv_sec, (s64)ts.tv_nsec * (s64)1000000000);
			}
		}
	}
}

#endif
