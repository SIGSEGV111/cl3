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

#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)

#include "error.hpp"
#include "system_task_synchronization.hpp"
#include "system_task.hpp"
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>

static inline bool operator> (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) >  0; }
static inline bool operator< (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) <  0; }
static inline bool operator==(const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) == 0; }

namespace	cl3
{
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

				/***************************************************************/

				CL3_THREAD fd_t fd_timer = -1;

				static	void	DeleteThreadTimerFD
					(
						cl3::event::TEvent<const io::collection::IDynamicCollection<cl3::system::task::TThread *const>, const TOnActionData<cl3::system::task::TThread *const>&> &,
						const io::collection::IDynamicCollection<cl3::system::task::TThread *const> &,
						const io::collection::TOnActionData<cl3::system::task::TThread *const>& data,
						void *
					)
				{
					if(data.action == io::collection::ACTION_REMOVE && fd_timer != -1)
					{
						CL3_NONCLASS_SYSERR(::close(fd_timer));
						fd_timer = -1;
					}
				}

				static	fd_t	TimerFD		()
				{
					if(fd_timer == -1)
					{
						CL3_NONCLASS_SYSERR(fd_timer = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC));
						system::task::TProcess::Self()->Threads().OnAction().Register<void>(&DeleteThreadTimerFD, NULL);
					}
					return fd_timer;
				}

				TList<ISignal*>
						ISignal::WaitFor	(const IArray<ISignal*>& signals, time::TTime timeout)
				{
					const usys_t n_signals = signals.Count();
					TList<struct ::pollfd> pfds;
					TList<ISignal*> signaled_signals;
					pfds.Count(n_signals+1);

					pfds[0].fd = timeout < 0 ? -1 : TimerFD();
					pfds[0].events = POLLIN;
					pfds[0].revents = 0;

					if(timeout >= 0)
					{
						struct ::itimerspec its;
						its.it_interval.tv_sec = 0;
						its.it_interval.tv_nsec = 0;
						its.it_value = timeout;
						CL3_NONCLASS_SYSERR(timerfd_settime(pfds[0].fd, 0, &its, NULL));
					}

					for(usys_t i = 0; i < n_signals; i++)
					{
						pfds[i+1] = signals[i]->Handle();
						pfds[i+1].revents = 0;
					}

					for(;;)
					{
						for(usys_t i = 0; i < n_signals; i++)
							if(signals[i]->Validate())
								signaled_signals.Add(signals[i]);

						if(signaled_signals.Count() > 0 || pfds[0].revents != 0)
							return signaled_signals;

						for(usys_t i = 0; i < n_signals; i++)
							signals[i]->BeforeWait();

						CL3_NONCLASS_ERROR(poll(pfds.ItemPtr(0), n_signals+1, -1) == -1 && errno != EINTR, TSyscallException, errno);

						for(usys_t i = 0; i < n_signals; i++)
							signals[i]->AfterWait();
					}
				}
			}
		}
	}
}

#endif
