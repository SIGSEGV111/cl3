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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// static inline bool operator> (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) >  0; }
// static inline bool operator< (const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) <  0; }
static inline bool operator==(const struct ::pollfd& v1, const struct ::pollfd& v2) { return ::memcmp(&v1, &v2, sizeof(struct ::pollfd)) == 0; }

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
				using namespace system::time;
				using namespace io::collection;
				using namespace io::collection::list;
				using namespace io::collection::array;

				/***************************************************************/

				bool	ISignal::WaitFor	(time::TTime timeout)
				{
					ISignal* arr_signals[1] = { this };
					TArray<ISignal*> signals(arr_signals, 1, false);
					return WaitFor(signals, timeout).Count() > 0;
				}

				/***************************************************************/

				void	TMutex::Acquire		()
				{
					this->on_mutex_action.Raise(this, TOnMutexActionData(MUTEX_ACTION_ACQUIRE, MUTEX_STATUS_ENTER));
					IThreadRunner* const self = task::IThreadRunner::Self();
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
					IThreadRunner* const self = task::IThreadRunner::Self();
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
					CL3_CLASS_ERROR(this->owner != task::IThreadRunner::Self(), TException, "mutex is not owned by the calling thread");
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
					return this->owner == task::IThreadRunner::Self();
				}

				CLASS	TMutex::TMutex	(bool init_acquired)
				{
					CL3_CLASS_LOGIC_ERROR(task::IThreadRunner::Self() == NULL);
					this->owner = init_acquired ? task::IThreadRunner::Self() : NULL;
					this->n_times = init_acquired ? 1 : 0;
					pthread_mutexattr_t attr;
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_init(&attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_init(&this->mtx, &attr));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutexattr_destroy(&attr));
					if(init_acquired)
						CL3_CLASS_PTHREAD_ERROR(pthread_mutex_lock(&this->mtx));
				}

				CLASS	TMutex::~TMutex	()
				{
					CL3_CLASS_ERROR(this->owner != task::IThreadRunner::Self() || this->n_times != 1, TException, "mutex can only be destroyed by a thread which has currently acquired the mutex exactly once");
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_unlock(&this->mtx));
					CL3_CLASS_PTHREAD_ERROR(pthread_mutex_destroy(&this->mtx));
				}

				/***************************************************************/

				struct ::pollfd	TSignal::Handle	() const
				{
					const struct ::pollfd pfd = { pipe_h2w[0], POLLIN, 0 };
					return pfd;
				}

				void	TSignal::BeforeWait	()
				{
					CL3_CLASS_LOGIC_ERROR(!this->mutex->HasAcquired());
					this->mutex->Release();
					CL3_CLASS_LOGIC_ERROR(this->mutex->HasAcquired());
				}

				void	TSignal::AfterWait	()
				{
					CL3_CLASS_LOGIC_ERROR(this->mutex->HasAcquired());
					this->mutex->Acquire();
					this->Reset();
				}

				void	TSignal::Reset		()
				{
					CL3_CLASS_LOGIC_ERROR(!this->mutex->HasAcquired());
					byte_t buffer[8];
					ssize_t r;
					while((r = ::read(this->pipe_h2w[0], buffer, 8)) > 0);
					CL3_CLASS_ERROR(r == -1 && errno != EAGAIN && errno != EWOULDBLOCK, TSyscallException, errno);
				}

				void	TSignal::Raise		()
				{
					CL3_CLASS_LOGIC_ERROR(!this->mutex->HasAcquired());
					const byte_t v = 1;
					const ssize_t r = ::write(this->pipe_h2w[1], &v, 1);
					CL3_CLASS_ERROR(r == -1 && errno != EAGAIN && errno != EWOULDBLOCK, TSyscallException, errno);
				}

				CLASS	TSignal::TSignal	(TMutex* mutex) : mutex(mutex)
				{
					#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)
						CL3_CLASS_SYSERR(::pipe2(pipe_h2w, O_NONBLOCK|O_CLOEXEC));
					#else
						CL3_CLASS_SYSERR(::pipe(pipe_h2w));
						CL3_CLASS_SYSERR(::fcntl(pipe_h2w[0], F_SETFD, FD_CLOEXEC));
						CL3_CLASS_SYSERR(::fcntl(pipe_h2w[1], F_SETFD, FD_CLOEXEC));
						CL3_CLASS_SYSERR(::fcntl(pipe_h2w[0], F_SETFL, O_NONBLOCK));
						CL3_CLASS_SYSERR(::fcntl(pipe_h2w[1], F_SETFL, O_NONBLOCK));
					#endif
				}

				CLASS	TSignal::~TSignal	()
				{
					CL3_CLASS_SYSERR(close(pipe_h2w[0]));
					CL3_CLASS_SYSERR(close(pipe_h2w[1]));
				}
			}
		}
	}
}

#endif
