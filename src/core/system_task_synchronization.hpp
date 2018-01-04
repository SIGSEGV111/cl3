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

#ifndef	_include_cl3_core_system_task_synchronization_hpp_
#define	_include_cl3_core_system_task_synchronization_hpp_

#include "error.hpp"
#include "system_compiler.hpp"
#include "system_os.hpp"
#include "system_time.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	task
		{
			namespace	synchronization
			{
				struct waitinfo_t
				{
					fd_t fd;
					short events;
					short revents;
				};

				struct IWaitable
				{
					virtual waitinfo_t WaitInfo() const CL3_GETTER = 0;

					CL3PUBF bool WaitFor(time::TTime timeout) CL3_WARN_UNUSED_RESULT;
					inline void WaitFor() { CL3_CLASS_LOGIC_ERROR(!this->WaitFor(time::TTime(-1))); }
					CL3PUBF static io::collection::list::TList<bool> WaitFor(const io::collection::IStaticCollection<IWaitable* const>& waitables, time::TTime timeout = -1);
				};

				struct IMutex
				{
					virtual IWaitable& OnRelease() = 0;
					virtual bool Acquire(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
					inline void Acquire() { CL3_CLASS_LOGIC_ERROR(!this->Acquire(time::TTime(-1))); }
					inline bool TryAcquire() CL3_WARN_UNUSED_RESULT { return this->Acquire(time::TTime(0)); }
					virtual void Release() = 0;
					virtual usys_t TimesAcquired() const CL3_GETTER = 0;
					inline bool HasAcquired() const CL3_GETTER { return this->TimesAcquired() > 0; }
				};

				struct ISignal
				{
					virtual IWaitable& OnSignal() CL3_GETTER = 0;
					virtual IMutex& Mutex() CL3_GETTER = 0;
				};

				class TSignal : public ISignal
				{
					public:
						CL3PUBF IWaitable& OnSignal() CL3_GETTER;
						CL3PUBF IMutex& Mutex() CL3_GETTER;
						CL3PUBF void Raise();
						CL3PUBF void Clear();
				};

				class	CL3PUBT	TRecurseGuard
				{
					protected:
						bool* var;

					public:
						CLASS	TRecurseGuard	(bool* var) : var(var) { CL3_CLASS_LOGIC_ERROR(*var); *var = true; }
						CLASS	~TRecurseGuard	() { *var = false; }
				};
			}
		}
	}
/*
	namespace	event
	{
		extern template class TEvent<system::task::synchronization::IMutex, system::task::synchronization::TOnMutexActionData>;
	}*/
}

#endif
