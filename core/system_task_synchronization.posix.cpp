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
	namespace	system
	{
		namespace	task
		{
			namespace	synchronization
			{
				using namespace error;

				namespace	_
				{
					struct	TMutexData
					{
						IThread* owner;
						unsigned n_times;
						::pthread_mutex_t mutex;
					};
				}

				void	TMutex::Acquire		(EAccess)
				{
					IThread* const self = &task::IThread::Self();
					if(data->owner == self)
						data->n_times++;
					else
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_lock(&data->mutex));
				}

				bool	TMutex::Acquire		(time::TTime timeout, EAccess)
				{
					IThread* const self = &task::IThread::Self();
					if(data->owner == self)
					{
						data->n_times++;
						return true;
					}
					else
					{
						const struct ::timespec ts = timeout;
						const int status = pthread_mutex_timedlock(&data->mutex, &ts);
						switch(status)
						{
							case 0:
								data->owner = self;
								data->n_times = 1;
								return true;
							case ETIMEDOUT:
								return false;
							default:
								CL3_CLASS_PTHREAD_ERROR(status);
						}
						CL3_UNREACHABLE;
					}
				}

				void	TMutex::Release		(EAccess)
				{
					CL3_CLASS_ERROR(data->owner != &task::IThread::Self(), TException, "mutex is not owned by the calling thread");
					if(--data->n_times == 0)
					{
						data->owner = NULL;
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&data->mutex));
					}
				}

				bool	TMutex::HasAcquired	(EAccess) const
				{
					return data->owner == &task::IThread::Self();
				}

				CLASS	TMutex::TMutex	() : data(new _::TMutexData())
				{
					data->owner = NULL;
					data->n_times = 0;
					pthread_mutexattr_t attr;
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_init(&attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_init(&data->mutex, &attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_destroy(&attr));
				}

				CLASS	TMutex::~TMutex	()
				{
					CL3_CLASS_ERROR(data->owner != &task::IThread::Self() || data->n_times != 1, TException, "mutex can only be destroyed by a thread which has currently acquired the mutex exactly once");
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&data->mutex));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_destroy(&data->mutex));
				}
			}
		}
	}
}

#endif
