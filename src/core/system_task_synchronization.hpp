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
#include "io_collection_basiclist.hpp"
#include "event.hpp"

#if (CL3_OS == CL3_OS_POSIX)
#include <pthread.h>
#elif (CL3_OS == CL3_OS_WINDOWS)
#include <windows.h>
#else
#error
#endif

namespace	cl3
{
	namespace	collection
	{
		template<class T>
		struct	IStaticCollection;
	}

	using namespace system::types;

	namespace	system
	{
		namespace	task
		{
			class	IThread;

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

				class	CL3PUBT	IMutex : public virtual event::IObservable
				{
					protected:
						event::TEvent<IMutex,TOnMutexActionData> on_mutex_action;

					public:
						virtual	void	Acquire		() = 0;
						virtual	bool	Acquire		(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
						virtual	void	Release		() = 0;
						virtual	bool	HasAcquired	() const CL3_GETTER = 0;	//	returns wheter or not, the calling thread has acquired this mutex
						inline	bool	TryAcquire	() CL3_WARN_UNUSED_RESULT { return Acquire(time::TTime(0,0)); }
						virtual	CLASS	~IMutex		();
				};

				struct	CL3PUBT	ISignal : public virtual IMutex, public virtual event::IObservable
				{
					virtual	void	Raise	() = 0;
					virtual	void	WaitFor	() = 0;
					virtual	bool	WaitFor	(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;

					inline	static	void	WaitFor	(const collection::IStaticCollection<ISignal*>& signals) { CL3_NONCLASS_LOGIC_ERROR(!ISignal::WaitFor(signals, time::TTime(-1,0))); }
					CL3PUBF	static	bool	WaitFor	(const collection::IStaticCollection<ISignal*>& signals, time::TTime timeout) CL3_WARN_UNUSED_RESULT;
				};

				class	CL3PUBT	TMutex : public virtual IMutex
				{
					private:
						CLASS	TMutex	(TMutex&&);

						IThread* owner;
						#if (CL3_OS == CL3_OS_POSIX)
							pthread_mutex_t mtx;
						#elif (CL3_OS == CL3_OS_WINDOWS)
							CRITICAL_SECTION cs;
						#endif
						unsigned n_times;

					public:
						CL3PUBF	void	Acquire		() final override;
						CL3PUBF	bool	Acquire		(time::TTime timeout) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		() final override;
						CL3PUBF	bool	HasAcquired	() const final override CL3_GETTER;

						CL3PUBF	CLASS	TMutex	();
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
			}
		}
	}
}

#endif
