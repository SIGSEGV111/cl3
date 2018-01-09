/*
 *    libcl3 - common library version 3
 *    Copyright (C) 2013	Simon Brennecke
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "system_time_timer.hpp"
#include "error.hpp"
#include <sys/timerfd.h>

namespace	cl3
{
	namespace	system
	{
		namespace	time
		{
			namespace	timer
			{
				using namespace io::stream::fd;

				system::task::synchronization::waitinfo_t TTimer::WaitInfo() const
				{
					return this->fds.OnInputReady().WaitInfo();
				}

				void TTimer::Start(TTime interval)
				{
					struct itimerspec it;
					it.it_interval.tv_sec = 0;
					it.it_interval.tv_nsec = 0;
					it.it_value = interval;
					CL3_CLASS_SYSERR(timerfd_settime(this->fds.FD(), 0, &it, NULL));
				}

				void TTimer::Stop()
				{
					struct itimerspec it;
					it.it_interval.tv_sec = 0;
					it.it_interval.tv_nsec = 0;
					it.it_value = it.it_interval;
					CL3_CLASS_SYSERR(timerfd_settime(this->fds.FD(), 0, &it, NULL));
				}

				CLASS TTimer::TTimer(EClock clock)
				{
					clockid_t id = -1;

					switch(clock)
					{
						case EClock::TAI:
						case EClock::REALTIME:
							id = CLOCK_REALTIME;
							break;

						case EClock::MONOTONIC:
							id = CLOCK_MONOTONIC;
							break;

						case EClock::PROCESS:
							id = CLOCK_PROCESS_CPUTIME_ID;
							break;

						case EClock::THREAD:
							id = CLOCK_THREAD_CPUTIME_ID;
							break;

						default:
							CL3_CLASS_FAIL(error::TException, "unsupported clock selected");
					}

					fd_t fd;
					CL3_CLASS_SYSERR(fd = timerfd_create(id, TFD_NONBLOCK|TFD_CLOEXEC));
					this->fds.FD(fd);
				}

				CLASS TTimer::~TTimer()
				{
					//	nothing to do
				}
			}
		}
	}
}
