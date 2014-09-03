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

				struct	CL3PUBT	TMutexEvent
				{
					EMutexAction action;
					EMutexStatus status;

					CL3PUBF	CLASS	TMutexEvent	(EMutexAction action, EMutexStatus status);
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

				struct	CL3PUBT	TSignalEvent
				{
					ESignalAction action;
					ESignalStatus status;

					CL3PUBF	CLASS	TSignalEvent	(ESignalAction action, ESignalStatus status);
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

				struct	CL3PUBT	IMutex : public virtual event::IObservable<IMutex, TMutexEvent>
				{
					virtual	void	Acquire		() = 0;
					virtual	bool	Acquire		(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Release		() = 0;
					virtual	bool	HasAcquired	() const CL3_GETTER = 0;	//	returns wheter or not, the calling thread has acquired this mutex
					inline	bool	TryAcquire	() CL3_WARN_UNUSED_RESULT { return Acquire(time::TTime(0,0)); }
				};

				struct	CL3PUBT	ISignal : public virtual IMutex, public virtual event::IObservable<ISignal, TSignalEvent>
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
						event::TEvent<IMutex, TMutexEvent> on_change;

					public:
						CL3PUBF	const event::TEvent<IMutex, TMutexEvent>&
										OnChange	() const final override;
						CL3PUBF	void	Acquire		() final override;
						CL3PUBF	bool	Acquire		(time::TTime timeout) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		() final override;
						CL3PUBF	bool	HasAcquired	() const final override CL3_GETTER;

						CL3PUBF	CLASS	TMutex	();
						CL3PUBF	CLASS	~TMutex	();
				};

// 				class	CL3PUBT	TSignal : public virtual ISignal
// 				{
// 				};

// 				template<class TSender, class TData>
// 				class	CL3PUBT	TSignalEventReceiver : public event::TEvent<TSender,TData>::IReceiver, public virtual ISignal
// 				{
// 					protected:
// 						void	OnRaise	(event::TEvent<TSender,TData>& event, TSender& sender, TData data)
// 						{
// 						}
//
// 					public:
// 						inline	void	Acquire		(EAccess);
// 						inline	bool	Acquire		(time::TTime timeout, EAccess) CL3_WARN_UNUSED_RESULT;
// 						inline	void	Release		(EAccess);
// 						inline	bool	HasAcquired	(EAccess) const CL3_GETTER;
// 						inline	void	Raise		();
// 						inline	void	WaitFor		();
// 						inline	bool	WaitFor		(time::TTime timeout) CL3_WARN_UNUSED_RESULT;
// 				};

				/*template<ESharing sharing, class T>
				class	CL3PUBT	TInterlocked;

				template<class T>
				class	CL3PUBT	TInterlocked<MUTEX_PRIVATE,T> : public TRWMutex
				{
					protected:
						T object;

					public:
						inline	const T&	R	() const CL3_GETTER	{ CL3_CLASS_LOGIC_ERROR(!HasAcquired(MUTEX_ACCESS_READONLY)); return object; }
						inline	T&			W	() CL3_GETTER		{ CL3_CLASS_LOGIC_ERROR(!HasAcquired(MUTEX_ACCESS_READWRITE)); return object; }

						CLASS	TInterlocked	() : object() {}
						CLASS	TInterlocked	(const T& object) : object(object) {}
						CLASS	TInterlocked	(T&& object) : object(object) {}
				};

				template<class T>
				class	CL3PUBT	TInterlocked<MUTEX_SHARED,T> : public IMutex
				{
					protected:
						TRWMutex* mutex;
						T object;

					public:
						//	from IMutex
						inline	void	Acquire		(EAccess access) { mutex->Acquire(access); }
						inline	bool	Acquire		(time::TTime timeout, EAccess access) CL3_WARN_UNUSED_RESULT { return mutex->Acquire(timeout, access); }
						inline	void	Release		(EAccess access) { mutex->Release(access); }
						inline	bool	HasAcquired	(EAccess access) const CL3_GETTER { return mutex->HasAcquired(access); }

						//	from TInterlocked
						inline	const T&	R	() const CL3_GETTER	{ CL3_CLASS_LOGIC_ERROR(!mutex->HasAcquired(MUTEX_ACCESS_READONLY)); return object; }
						inline	T&			W	() CL3_GETTER		{ CL3_CLASS_LOGIC_ERROR(!mutex->HasAcquired(MUTEX_ACCESS_READWRITE)); return object; }

						CLASS	TInterlocked	(TRWMutex* mutex) : mutex(mutex), object() { CL3_CLASS_LOGIC_ERROR(mutex == NULL); }
						CLASS	TInterlocked	(TRWMutex* mutex, const T& object) : mutex(mutex), object(object) { CL3_CLASS_LOGIC_ERROR(mutex == NULL); }
				};*/

				class	CL3PUBT	TInterlockedRegion
				{
					private:
						IMutex* mutex;

						CLASS	TInterlockedRegion	(const TInterlockedRegion&);

					public:
						CLASS	TInterlockedRegion	(IMutex* mutex) : mutex(mutex) { mutex->Acquire(); }
						CLASS	~TInterlockedRegion	() { mutex->Release(); }
				};

				#define	CL3_CLASS_INTERLOCKED_REGION_BEGIN	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&this->Mutex());
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