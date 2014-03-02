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

#include "time.hpp"
#include "event.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			struct	IThread;

			namespace	synchronization
			{
				namespace	_
				{
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

				struct	IMutex
				{
					virtual	void	Acquire	() = 0;
					virtual	bool	Acquire	(time::TTime timeout) = 0;
					virtual	void	Release	() = 0;

					inline	bool	TryAcquire	() { return Acquire(time::TTime(0,0)); }
				};

				struct	ISignal
				{
					virtual	void	Raise	() = 0;
					virtual	void	WaitFor	() = 0;
					virtual	bool	WaitFor	(time::TTime timeout) = 0;
				};

				class	TDemandMutex : public IMutex
				{
					private:
						IMutex* mutex;

						CLASS	TDemandMutex	(const TDemandMutex&);

					public:
						void	Acquire	()						{ if(mutex) mutex->Acquire(); }
						bool	Acquire	(time::TTime timeout)	{ if(mutex) return mutex->Acquire(timeout); else return true; }
						void	Release	()						{ if(mutex) mutex->Release(); }

						CLASS	TDemandMutex	(IMutex* mutex) : mutex(mutex) {}
				};

				struct	IInterlocked
				{
					virtual	IMutex&	Mutex	() = 0;
				};

				class	TDemandInterlocked : IInterlocked
				{
					private:
						TDemandMutex dm;

					protected:
						CLASS	TDemandInterlocked	(IMutex* mutex) : dm(mutex) {}

					public:
						IMutex&	Mutex	() { return dm; }
				};

				class	TInterlockedRegion
				{
					private:
						IMutex* mutex;

						CLASS	TInterlockedRegion	(const TInterlockedRegion&);

					public:
						CLASS	TInterlockedRegion	(IMutex* mutex) : mutex(mutex)	{ mutex->Acquire(); }
						CLASS	~TInterlockedRegion	()								{ mutex->Release(); }
				};

				#define	CL3_CLASS_INTERLOCKED_REGION_BEGIN	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&this->Mutex());
				#define	CL3_CLASS_INTERLOCKED_REGION_END	}

				#define	CL3_NONCLASS_INTERLOCKED_REGION_BEGIN(mutex)	{ cl3::system::task::synchronization::TInterlockedRegion __interlocked_region(&(mutex));
				#define	CL3_NONCLASS_INTERLOCKED_REGION_END				}



				class	TMutex : public IMutex
				{
					protected:

					public:
						void	Acquire	();
						bool	Acquire	(time::TTime timeout);
						void	Release	();
				};
			}
		}
	}
}

#endif
