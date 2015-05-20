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

#include "system_task_synchronization.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			namespace	synchronization
			{
				CLASS	TOnMutexActionData::TOnMutexActionData		(EMutexAction action, EMutexStatus status) : action(action), status(status) {}
				CLASS	TOnSignalActionData::TOnSignalActionData	(ESignalAction action, ESignalStatus status) : action(action), status(status) {}

				CLASS	IMutex::~IMutex			()
				{
				}

				void	ALockable::Acquire		()
				{
					if(mutex != NULL) mutex->Acquire();
				}

				bool	ALockable::Acquire		(time::TTime timeout)
				{
					return (mutex != NULL) ? mutex->Acquire(timeout) : true;
				}

				void	ALockable::Release		()
				{
					if(mutex != NULL) mutex->Release();
				}

				bool	ALockable::HasAcquired	() const
				{
					return (mutex != NULL) ? mutex->HasAcquired() : true;
				}

				CLASS	ALockable::ALockable	() : mutex()
				{
				}

				CLASS	ALockable::ALockable	(system::memory::TUniquePtr<IMutex>&& mutex) : mutex(static_cast<system::memory::TUniquePtr<IMutex>&&>(mutex))
				{
				}

				CLASS	ALockable::~ALockable	()
				{
				}

				void	TSignal::Acquire	()						{ this->mutex->Acquire(); }
				bool	TSignal::Acquire	(time::TTime timeout)	{ return this->mutex->Acquire(timeout); }
				void	TSignal::Release	()						{ this->mutex->Release(); }
				bool	TSignal::HasAcquired() const 				{ return this->mutex->HasAcquired(); }
			}
		}
	}
}
