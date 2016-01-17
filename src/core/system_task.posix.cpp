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

#include "system_task.hpp"
#include "system_task_synchronization.hpp"

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <pthread.h>
#include <sys/wait.h>

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			struct	TUTF32;
		}
	}

	namespace	system
	{
		namespace	task
		{
			using namespace io::collection;
			using namespace io::collection::list;
			using namespace io::text::string;
			using namespace io::stream;
			using namespace io::text::encoding;
			using namespace error;
			using namespace memory;
			using namespace time;
			using namespace synchronization;

			/**************************************************************************************/

			pid_t gettid();

			static CL3_THREAD TLocalThread* th_self = NULL;

			/**************************************************************************************/

			CLASS TLocalProcess::TLocalProcess()
			{
				this->id = getpid();
			}

			/**************************************************************************************/

			void	IThread::Sleep	(TTime time, EClock clock)
			{
				const struct timespec ts = time;
				switch(clock)
				{
					case TIME_CLOCK_TAI:
					case TIME_CLOCK_MONOTONIC:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL));
						break;

					case TIME_CLOCK_REALTIME:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL));
						break;

					case TIME_CLOCK_PROCESS:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, 0, &ts, NULL));
						break;

					default:
						CL3_NONCLASS_FAIL(TException, "the requested clock is not supported by Sleep()");
				}
			}

			void TLocalThread::Suspend()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGSTOP));
			}

			void TLocalThread::Resume()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGCONT));
			}

			void TLocalThread::Kill()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGKILL));
			}

			void TLocalThread::Shutdown()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGTERM));
			}

			void TLocalThread::Start()
			{
				CL3_NOT_IMPLEMENTED;
			}

			TLocalThread* TLocalThread::Self()
			{
				CL3_NONCLASS_LOGIC_ERROR(th_self == NULL);
				return th_self;
			}

			CLASS TLocalThread::TLocalThread(IFiber* fiber, bool autostart) : fiber(fiber)
			{
				CL3_CLASS_ERROR(fiber == NULL, TException, "fiber cannot be NULL");
				CL3_NOT_IMPLEMENTED;
				if(autostart)
					this->Start();
			}

			CLASS TLocalThread::~TLocalThread()
			{
				/*
				 * 1) Acquire lock
				 * 2) if running; Stop()
				 */
				CL3_NOT_IMPLEMENTED;
			}
		}
	}
}

#endif
