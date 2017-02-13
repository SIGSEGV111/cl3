/*
 * libcl3 - common library version 3
 * Copyright (C) 2013	Simon Brennecke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "system_task_synchronization.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			namespace	async
			{
				using namespace synchronization;
				using namespace io::collection::list;
				using namespace time;

				usys_t TAsyncEventProcessor::ProcessEvents()
				{
					return this->WaitAndProcessEvents(0);
				}

				usys_t TAsyncEventProcessor::WaitAndProcessEvents(time::TTime timeout)
				{
					TList<TCallback*> callbacks;

					for(TCallback* p = this->callback_first; p != NULL; p = p->callback_next)
						callbacks.Add(p);

					const usys_t n_callbacks = callbacks.Count();

					struct pollfd pfds[n_callbacks];
					for(usys_t i = 0; i < n_callbacks; i++)
						pfds[i] = callbacks[i]->waitable->__PollInfo();

					int n_events;
					CL3_CLASS_SYSERR(n_events = ::poll(pfds, n_callbacks, timeout.ConvertToI(TIME_UNIT_MILLISECONDS)));

					if(n_events > 0)
						for(usys_t i = 0; i < n_callbacks; i++)
							if(pfds[i].revents != 0)
								callbacks[i]->receiver->AsyncCallback(this, callbacks[i]->waitable);

					return n_events;
				}
			}
		}
	}
}

#endif