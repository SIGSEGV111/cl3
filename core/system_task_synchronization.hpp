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
					virtual	bool	Acquire	(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Release	() = 0;
					virtual	bool	Acquired() const = 0;	//	returns wheter or not, the calling thread has acquired this mutex

					inline	bool	TryAcquire	() CL3_WARN_UNUSED_RESULT { return Acquire(time::TTime(0,0)); }
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

						CLASS	TMutex		(const TMutex&);
						TMutex&	operator=	(const TMutex&);

					public:
						GETTER	bool	Enabled	() const { return data != NULL; }
						CL3PUBF	void	Acquire	();
						CL3PUBF	bool	Acquire	(time::TTime timeout) CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release	();
						CL3PUBF	bool	Acquired() const;

						CL3PUBF	CLASS	TMutex	(bool enabled);
						CL3PUBF	CLASS	~TMutex	();
				};

				struct	IInterlocked
				{
					virtual	GETTER	IMutex&	Mutex	() = 0;
				};

				class	TDemandInterlocked : IInterlocked
				{
					private:
						TMutex mutex;

					protected:
						CLASS	TDemandInterlocked	(bool enabled) : mutex(enabled) {}

					public:
						GETTER	IMutex&	Mutex	() { return mutex; }
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
			}
		}
	}
}

#endif
