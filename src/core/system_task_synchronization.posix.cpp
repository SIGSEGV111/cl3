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

#include <poll.h>

#include "error.hpp"
#include "system_task_synchronization.hpp"
#include "system_task.hpp"

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

				bool IWaitable::WaitFor(time::TTime timeout)
				{
					const waitinfo_t wi = this->WaitInfo();
					struct pollfd pfd;
					pfd.fd = wi.fd;
					pfd.events = wi.events;
					pfd.revents = 0;
					CL3_CLASS_SYSERR(::poll(&pfd, 1, timeout.ConvertToI(time::TIME_UNIT_MILLISECONDS)));
					return pfd.revents != 0;
				}

				io::collection::list::TList<bool> WaitFor(const io::collection::IStaticCollection<IWaitable* const>& waitables, time::TTime timeout)
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}

#endif
