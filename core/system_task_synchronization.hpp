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

#include "system_compiler.hpp"
#include "time.hpp"
#include "event.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			class	IThread;

			namespace	synchronization
			{
				namespace	_
				{
					struct	TMutexData;
					struct	TRWMutexData;

					enum	EMutexEvent
					{
						MUTEX_EVENT_ACQUIRING,
						MUTEX_EVENT_ACQUIRED,
						MUTEX_EVENT_RELEASING,
						MUTEX_EVENT_RELEASED
					};

					struct	TMutexOnChangeData
					{
						EMutexEvent event;
						IThread* thread;
					};

					enum	ESignalEvent
					{
						SIGNAL_EVENT_RAISED
					};

					struct	TSignalOnChangeData
					{
						ESignalEvent event;
						IThread* thread;
					};
				}

				enum	EAccess
				{
					MUTEX_ACCESS_READ,
					MUTEX_ACCESS_WRITE
				};

				struct	IMutex
				{
					virtual	void	Acquire		(EAccess) = 0;
					virtual	bool	Acquire		(time::TTime timeout, EAccess) CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Release		(EAccess) = 0;
					virtual	bool	HasAcquired	(EAccess) const GETTER = 0;	//	returns wheter or not, the calling thread has acquired this mutex
					inline	bool	TryAcquire	(EAccess access) CL3_WARN_UNUSED_RESULT { return Acquire(time::TTime(0,0), access); }
				};

				struct	ISignal
				{
					virtual	void	Raise	() = 0;
					virtual	void	WaitFor	() = 0;
					virtual	bool	WaitFor	(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
				};

				class	TMutex : public IMutex
				{
					private:
						_::TMutexData* const data;

						CLASS TMutex(const TMutex&);
						TMutex& operator=(const TMutex&);

					public:
						CL3PUBF	void	Acquire		(EAccess access = MUTEX_ACCESS_WRITE);
						CL3PUBF	bool	Acquire		(time::TTime timeout, EAccess access = MUTEX_ACCESS_WRITE) CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		(EAccess access = MUTEX_ACCESS_WRITE);
						CL3PUBF	bool	HasAcquired	(EAccess access = MUTEX_ACCESS_WRITE) const GETTER;

						CL3PUBF	CLASS	TMutex	();
						CL3PUBF	CLASS	~TMutex	();
				};

				class	TRWMutex : public IMutex
				{
					private:
						_::TRWMutexData* const data;

						CLASS TRWMutex(const TRWMutex&);
						TRWMutex& operator=(const TRWMutex&);

					public:
						CL3PUBF	void	Acquire		(EAccess access);
						CL3PUBF	bool	Acquire		(time::TTime timeout, EAccess access) CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		(EAccess access);
						CL3PUBF	bool	HasAcquired	(EAccess access) const GETTER;

						CL3PUBF	CLASS	TRWMutex	();
						CL3PUBF	CLASS	~TRWMutex	();
				};

				/*struct	IInterlocked
				{
					virtual	GETTER	IMutex&	Mutex	() = 0;
				};*/

				template<class T>
				class	TInterlocked : public TRWMutex
				{
					protected:
						T object;

					public:
						inline	const T&	R	() const GETTER	{ CL3_CLASS_LOGIC_ERROR(!HasAcquired(MUTEX_ACCESS_READ)); return object; }
						inline	T&			W	() GETTER		{ CL3_CLASS_LOGIC_ERROR(!HasAcquired(MUTEX_ACCESS_WRITE)); return object; }

						CLASS	TInterlocked	() : object() {}
						CLASS	TInterlocked	(const T& object) : object(object) {}
				};

				class	TInterlockedRegion
				{
					private:
						EAccess access;
						IMutex* mutex;

						CLASS	TInterlockedRegion	(const TInterlockedRegion&);

					public:
						CLASS	TInterlockedRegion	(IMutex* mutex, EAccess access) : access(access), mutex(mutex) { mutex->Acquire(access); }
						CLASS	~TInterlockedRegion	() { mutex->Release(access); }
				};

				#define	CL3_CLASS_INTERLOCKED_REGION_BEGIN	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&this->Mutex());
				#define	CL3_CLASS_INTERLOCKED_REGION_END	}

				#define	CL3_NONCLASS_INTERLOCKED_REGION_BEGIN(mutex)	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&(mutex));
				#define	CL3_NONCLASS_INTERLOCKED_REGION_END				}


				class	TRecurseGuard
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
