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

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "error.hpp"
#include "system_task_synchronization.hpp"
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
					/*
						typedef union
						{
						struct __pthread_mutex_s
						{
							int __lock;
							unsigned int __count;
							int __owner;
						#if __WORDSIZE == 64
							unsigned int __nusers;
						#endif
							int __kind;
						#if __WORDSIZE == 64
							int __spins;
							__pthread_list_t __list;
						# define __PTHREAD_MUTEX_HAVE_PREV	1
						#else
							unsigned int __nusers;
							__extension__ union
							{
							int __spins;
							__pthread_slist_t __list;
							};
						#endif
						} __data;
						char __size[__SIZEOF_PTHREAD_MUTEX_T];
						long int __align;
						} pthread_mutex_t;
					*/
					struct	TMutexData
					{
						::pthread_mutex_t mutex;
					};
				}

				void	TMutex::Acquire	()
				{
					if(data) CL3_CLASS_PTHREAD_ERROR(pthread_mutex_lock(&data->mutex));
				}

				bool	TMutex::Acquire	(time::TTime timeout)
				{
					if(data)
					{
						const struct ::timespec ts = timeout;
						const int status = pthread_mutex_timedlock(&data->mutex, &ts);
						switch(status)
						{
							case 0:
								return true;
							case ETIMEDOUT:
								return false;
							default:
								CL3_CLASS_PTHREAD_ERROR(status);
						}
						CL3_UNREACHABLE;
					}
					else
						return true;
				}

				void	TMutex::Release	()
				{
					if(data) CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&data->mutex));
				}

				CLASS	TMutex::TMutex	(bool enabled) : data(enabled ? new _::TMutexData() : NULL)
				{
					if(enabled)
					{
						pthread_mutexattr_t attr;
						CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_init(&attr));
						CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
						CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_init(&data->mutex, &attr));
						CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_destroy(&attr));
					}
				}

				CLASS	TMutex::~TMutex	()
				{
					if(data) CL3_CLASS_PTHREAD_ERROR(pthread_mutex_destroy(&data->mutex));
				}
			}
		}
	}
}

#endif
