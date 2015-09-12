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
#include "event.hpp"

#if (CL3_OS == CL3_OS_POSIX)
	#include <pthread.h>
	#include <poll.h>
#elif (CL3_OS == CL3_OS_WINDOWS)
	#include <windows.h>
#else
	#error
#endif

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	task
		{
			class	IThreadRunner;

			namespace	synchronization
			{
				enum	EMutexAction
				{
					MUTEX_ACTION_ACQUIRE,
					MUTEX_ACTION_RELEASE
				};

				enum	EMutexStatus
				{
					MUTEX_STATUS_ENTER,
					MUTEX_STATUS_SUCCESS,
					MUTEX_STATUS_TIMEOUT
				};

				struct	CL3PUBT	TOnMutexActionData
				{
					EMutexAction action;
					EMutexStatus status;

					CL3PUBF	CLASS	TOnMutexActionData	(EMutexAction action, EMutexStatus status);
				};

				enum	ESignalAction
				{
					SIGNAL_EVENT_RAISE,
					SIGNAL_EVENT_WAITFOR
				};

				enum	ESignalStatus
				{
					SIGNAL_STATUS_ENTER,
					SIGNAL_STATUS_SUCCESS,
					SIGNAL_STATUS_TIMEOUT
				};

				struct	CL3PUBT	TOnSignalActionData
				{
					ESignalAction action;
					ESignalStatus status;

					CL3PUBF	CLASS	TOnSignalActionData	(ESignalAction action, ESignalStatus status);
				};

				enum	EAccess
				{
					MUTEX_ACCESS_READONLY,
					MUTEX_ACCESS_READWRITE
				};

				enum	ESharing
				{
					MUTEX_PRIVATE,
					MUTEX_SHARED
				};

				class	CL3PUBT	IMutex
				{
					protected:
						event::TEvent<IMutex,TOnMutexActionData> on_mutex_action;

					public:
						virtual	void	Acquire		() = 0;
						virtual	bool	Acquire		(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
						virtual	void	Release		() = 0;
						virtual	bool	HasAcquired	() const CL3_GETTER = 0;	//	returns wheter or not, the calling thread has acquired this mutex
						inline	bool	TryAcquire	() CL3_WARN_UNUSED_RESULT { return Acquire(0); }
						virtual	CLASS	~IMutex		();
				};

				class	CL3PUBT	ISignal
				{
					protected:
						#if (CL3_OS == CL3_OS_POSIX)
							virtual	struct ::pollfd	Handle	() const CL3_GETTER = 0;
						#elif (CL3_OS == CL3_OS_WINDOWS)
							virtual	HANDLE			Handle	() const CL3_GETTER = 0;
						#endif

						virtual	void	BeforeWait	() = 0;
						virtual	void	AfterWait	() = 0;

					public:
						inline	void	WaitFor	() { CL3_CLASS_LOGIC_ERROR(!ISignal::WaitFor(-1)); }
						CL3PUBF	bool	WaitFor	(time::TTime timeout) CL3_WARN_UNUSED_RESULT;

						inline	static	void	WaitFor	(const io::collection::array::IArray<ISignal*>& signals) { CL3_NONCLASS_LOGIC_ERROR(ISignal::WaitFor(signals, -1).Count() == 0); }
						CL3PUBF	static	io::collection::list::TList<ISignal*>
												WaitFor	(const io::collection::array::IArray<ISignal*>& signals, time::TTime timeout) CL3_WARN_UNUSED_RESULT;
				};

				class	CL3PUBT	TMutex : public virtual IMutex
				{
					private:
						CLASS TMutex(TMutex&) = delete;
						TMutex&	operator=(TMutex&) = delete;

						IThreadRunner* owner;
						#if (CL3_OS == CL3_OS_POSIX)
							pthread_mutex_t mtx;
						#elif (CL3_OS == CL3_OS_WINDOWS)
							CRITICAL_SECTION cs;
						#else
							#error "unsupported platform"
						#endif
						unsigned n_times;

					public:
						CL3PUBF	void	Acquire		() final override;
						CL3PUBF	bool	Acquire		(time::TTime timeout) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		() final override;
						CL3PUBF	bool	HasAcquired	() const final override CL3_GETTER;

						CL3PUBF	CLASS	TMutex	(bool init_acquired = true);
						CL3PUBF	CLASS	~TMutex	();
				};

				class	CL3PUBT	ALockable : public virtual IMutex
				{
					private:
						system::memory::TUniquePtr<IMutex> mutex;

					public:
						CL3PUBF	void	Acquire		() final override;
						CL3PUBF	bool	Acquire		(time::TTime timeout) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		() final override;
						CL3PUBF	bool	HasAcquired	() const final override CL3_GETTER;

					protected:
						CL3PUBF	CLASS	ALockable	();
						CL3PUBF	CLASS	ALockable	(system::memory::TUniquePtr<IMutex>&& mutex);
						CLASS			ALockable	(const ALockable&) = delete;	//	call constructor explicitly in your copy constructor
						CL3PUBF	CLASS	~ALockable	();
				};

				class	CL3PUBT	AThreadSafe
				{
					protected:
						mutable TMutex mutex;

					public:
						inline	TMutex&	Mutex	() const CL3_GETTER { return this->mutex; }
				};

				class	CL3PUBT	TInterlockedRegion
				{
					private:
						IMutex* mutex;

						CLASS	TInterlockedRegion	(const TInterlockedRegion&);

					public:
						CLASS	TInterlockedRegion	(IMutex* mutex) : mutex(mutex) { mutex->Acquire(); }
						CLASS	~TInterlockedRegion	() { mutex->Release(); }
				};

				#define	CL3_CLASS_INTERLOCKED_REGION_BEGIN	{ cl3::system::task::synchronization::TInterlockedRegion CL3_PASTE(__interlocked_region__, __COUNTER__)(&this->Mutex());
				#define	CL3_CLASS_INTERLOCKED_REGION_END	}

				#define	CL3_NONCLASS_INTERLOCKED_REGION_BEGIN(mutex)	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&(mutex));
				#define	CL3_NONCLASS_INTERLOCKED_REGION_END				}

				class	CL3PUBT	TRecurseGuard
				{
					protected:
						bool* var;

					public:
						CLASS	TRecurseGuard	(bool* var) : var(var) { CL3_CLASS_LOGIC_ERROR(*var); *var = true; }
						CLASS	~TRecurseGuard	() { *var = false; }
				};

				class	TSignal : public ISignal, public IMutex
				{
					protected:
						CLASS TSignal(const TSignal&) = delete;
						TSignal& operator=(const TSignal&) = delete;

						TMutex* mutex;

						#if (CL3_OS == CL3_OS_POSIX)
							int pipe_h2w[2];
							struct ::pollfd		Handle	() const final override CL3_GETTER;
						#elif (CL3_OS == CL3_OS_WINDOWS)
							HANDLE				Handle	() const final override CL3_GETTER;
						#else
							#error "unsupported platform"
						#endif

						CL3PUBF	void	BeforeWait	() final override;
						CL3PUBF	void	AfterWait	() final override;

					public:
						CL3PUBF	void	Acquire		() final override;
						CL3PUBF	bool	Acquire		(time::TTime timeout) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		() final override;
						CL3PUBF	bool	HasAcquired	() const final override CL3_GETTER;

						CL3PUBF	void	Reset		();
						CL3PUBF	void	Raise		();

						CL3PUBF	CLASS	TSignal		(TMutex*);
						CL3PUBF	CLASS	~TSignal	();
				};
			}
		}
	}
	
	namespace	event
	{
		extern template class TEvent<system::task::synchronization::IMutex, system::task::synchronization::TOnMutexActionData>;
	}
}

#endif
