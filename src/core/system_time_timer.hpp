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

#ifndef	_include_cl3_core_system_time_timer_hpp_
#define	_include_cl3_core_system_time_timer_hpp_

#include "system_time.hpp"
#include "io_stream_fd.hpp"
#include "system_task_synchronization.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	time
		{
			namespace	timer
			{
				class CL3PUBT TTimer : public virtual task::synchronization::IWaitable
				{
					protected:
						const TTime interval;
						io::stream::fd::TFDStream fds;

						CL3PUBF system::task::synchronization::waitinfo_t WaitInfo() const final override CL3_GETTER;

					public:
						CL3PUBF void Restart();
						CL3PUBF void Stop();
						CL3PUBF CLASS TTimer(const TTimer&) = delete;
						CL3PUBF CLASS TTimer(TTimer&&) = default;
						CL3PUBF CLASS TTimer(EClock clock, TTime interval);
						CL3PUBF CLASS ~TTimer();
				};
			}
		}
	}
}

#endif
