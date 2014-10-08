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

#include "error.hpp"
#include "system_task_synchronization.hpp"
#include "system_task.hpp"
#include <pthread.h>
#include <errno.h>

namespace	cl3
{
	namespace	event
	{
		template class TEvent<system::task::synchronization::IMutex, system::task::synchronization::TOnMutexActionData>;
	}

	namespace	system
	{
		namespace	task
		{
			namespace	synchronization
			{
				using namespace error;

				void	TMutex::Acquire		()
				{
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_ENTER));
					IThread* const self = task::IThread::Self();
					if(this->owner == self)
						this->n_times++;
					else
					{
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_lock(&this->mtx));
						this->owner = self;
						this->n_times = 1;
					}
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_SUCCESS));
				}

				bool	TMutex::Acquire		(time::TTime timeout)
				{
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_ENTER));
					IThread* const self = task::IThread::Self();
					if(this->owner == self)
					{
						this->n_times++;
						this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_SUCCESS));
						return true;
					}
					else
					{
						const struct ::timespec ts = timeout;
						const int status = pthread_mutex_timedlock(&this->mtx, &ts);
						switch(status)
						{
							case 0:
								this->owner = self;
								this->n_times = 1;
								this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_SUCCESS));
								return true;
							case ETIMEDOUT:
								this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_TIMEOUT));
								return false;
							default:
								CL3_CLASS_PTHREAD_ERROR(status);
						}
						CL3_UNREACHABLE;
					}
				}

				void	TMutex::Release		()
				{
					CL3_CLASS_ERROR(this->owner != task::IThread::Self(), TException, "mutex is not owned by the calling thread");
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_RELEASE, MUTEX_STATUS_ENTER));
					if(--this->n_times == 0)
					{
						this->owner = NULL;
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&this->mtx));
					}
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_RELEASE, MUTEX_STATUS_SUCCESS));
				}

				bool	TMutex::HasAcquired	() const
				{
					return this->owner == task::IThread::Self();
				}

				CLASS	TMutex::TMutex	()
				{
					CL3_CLASS_LOGIC_ERROR(task::IThread::Self() == NULL);
					this->owner = task::IThread::Self();
					this->n_times = 1;
					pthread_mutexattr_t attr;
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_init(&attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_init(&this->mtx, &attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_destroy(&attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_lock(&this->mtx));
				}

				CLASS	TMutex::~TMutex	()
				{
					CL3_CLASS_ERROR(this->owner != task::IThread::Self() || this->n_times != 1, TException, "mutex can only be destroyed by a thread which has currently acquired the mutex exactly once");
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&this->mtx));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_destroy(&this->mtx));
				}
			}
		}
	}
}

#endif
