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
				namespace	_
				{
					enum	EMutexEvent
					{
						MUTEX_EVENT_ACQUIRING,
						MUTEX_EVENT_ACQUIRED,
						MUTEX_EVENT_RELEASING,
						MUTEX_EVENT_RELEASED
					};
				}

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

				struct	IMutex
				{
					virtual	void	Acquire		(EAccess) = 0;
					virtual	bool	Acquire		(time::TTime timeout, EAccess) CL3_WARN_UNUSED_RESULT = 0;
					virtual	void	Release		(EAccess) = 0;
					virtual	bool	HasAcquired	(EAccess) const CL3_GETTER = 0;	//	returns wheter or not, the calling thread has acquired this mutex
					inline	bool	TryAcquire	(EAccess access) CL3_WARN_UNUSED_RESULT { return Acquire(time::TTime(0,0), access); }
				};

// 				struct	ISignal : IMutex
// 				{
// 					virtual	void	Raise	() = 0;
// 					virtual	void	WaitFor	() = 0;
// 					virtual	bool	WaitFor	(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
//
// 					CL3PUBF	static	void	WaitFor	(const collection::IStaticCollection<ISignal*>& signals);
// 					CL3PUBF	static	bool	WaitFor	(const collection::IStaticCollection<ISignal*>& signals, time::TTime timeout) CL3_WARN_UNUSED_RESULT;
// 				};

				class	CL3PUBT	TMutex : public IMutex
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
						CL3PUBF	void	Acquire		(EAccess access = MUTEX_ACCESS_READWRITE);
						CL3PUBF	bool	Acquire		(time::TTime timeout, EAccess access = MUTEX_ACCESS_READWRITE) CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		(EAccess access = MUTEX_ACCESS_READWRITE);
						CL3PUBF	bool	HasAcquired	(EAccess access = MUTEX_ACCESS_READWRITE) const CL3_GETTER;

						CL3PUBF	CLASS	TMutex	();
						CL3PUBF	CLASS	~TMutex	();
				};

				class	CL3PUBT	TRWMutex : public IMutex
				{
					private:
						IThread* owner;
						#if (CL3_OS == CL3_OS_POSIX)
							pthread_rwlock_t rwl;
						#elif (CL3_OS == CL3_OS_WINDOWS)
							CRITICAL_SECTION cs;
						#endif
						unsigned n_times;

						CLASS TRWMutex(const TRWMutex&);
						TRWMutex& operator=(const TRWMutex&);

					public:
						CL3PUBF	void	Acquire		(EAccess access);
						CL3PUBF	bool	Acquire		(time::TTime timeout, EAccess access) CL3_WARN_UNUSED_RESULT;
						CL3PUBF	void	Release		(EAccess access);
						CL3PUBF	bool	HasAcquired	(EAccess access) const CL3_GETTER;

						CL3PUBF	CLASS	TRWMutex	();
						CL3PUBF	CLASS	~TRWMutex	();
				};

				template<class TSender, class TData> class TEvent;

				template<class TSender, class TData>
				struct	CL3PUBT	IObservable
				{
					typedef TEvent<TSender, TData>	TOnChangeEvent;
					virtual	const TOnChangeEvent&	OnChange	() const = 0;
				};

				template<class TSender, class TData>
				class	CL3PUBT	TEvent : public TMutex
				{
					private:
						TEvent&	operator=(const TEvent&);

					public:
						struct	IReceiver
						{
							//	function which gets called, when the event is Raise()'ed
							virtual void OnRaise(TEvent& event, TSender& sender, TData data) = 0;
							virtual	~IReceiver	() {}
						};

						enum	EAction
						{
							ACTION_REGISTER, ACTION_UNREGISTER
						};

						struct	TOnChangeData
						{
							EAction action;
							IReceiver* receiver;
						};

					protected:
						struct	__ICleanup {};	//	just there so we later know what we have to delete() and what not

						template<class TReceiver>
						struct	TMemberFuncReceiver : IReceiver, __ICleanup
						{
							typedef void (TReceiver::*FReceiver)(TEvent&, TSender&, TData);

							TReceiver* receiver;
							FReceiver func;

							void OnRaise(TEvent& event, TSender& sender, TData data) { (receiver->*func)(event, sender, data); }

							CLASS	TMemberFuncReceiver	(TReceiver* receiver, FReceiver func) : receiver(receiver), func(func) {}
						};

						template<class TReceiverData>
						struct	TStaticFuncReceiver : IReceiver, __ICleanup
						{
							typedef void (*FReceiver)(TEvent&, TSender&, TData, TReceiverData&);

							TReceiverData rdata;
							FReceiver func;

							void OnRaise(TEvent& event, TSender& sender, TData data) { func(event, sender, data, rdata); }

							CLASS	TStaticFuncReceiver	(FReceiver func, const TReceiverData& rdata) : rdata(rdata), func(func) {}
						};

						mutable io::collection::basiclist::TBasicList<IReceiver*> receivers;

					public:
						CLASS	TEvent	() : receivers() {}
						CLASS	TEvent	(const TEvent&) : receivers() {}

						CLASS	~TEvent	()
						{
							const usys_t n = receivers.Count();
							for(usys_t i = 0; i < n; ++i)
								if(dynamic_cast<__ICleanup*>(receivers[i]))	//	I clean up! :-)
									delete receivers[i];
						}

						//	NOTE: receiver which get registered during the execution of an Raise() will *NOT* get called for the current invocation
						usys_t	Raise		(TSender& sender, TData data)
						{
							const usys_t n = receivers.Count();
							for(usys_t i = 0; i < n; ++i)
								receivers[i]->OnRaise(*this, sender, data);
							return n;	//	return the number of receivers which received the event
						}

						usys_t	Raise		(TSender* sender, TData data)
						{
							const usys_t n = receivers.Count();
							for(usys_t i = 0; i < n; ++i)
								receivers[i]->OnRaise(*this, *sender, data);
							return n;	//	return the number of receivers which received the event
						}

						bool	WaitFor		(TData& out_data, time::TTime timeout = -1) const CL3_WARN_UNUSED_RESULT
						{
							struct TSignal : IReceiver
							{
								TData* data;

							} signal;

							signal.data = &out_data;

							signal.Acquire();
							Register(&signal);
							const bool status = signal.WaitFor(timeout);
							Unregister(&signal);
							return status;
						}

						//	NOTE: it is possible to Register() and Unregister() on a "const TEvent&", while Raise() can only be called on a non-const reference
						//	this allows classes which use TEvent to provide a function which returns a "const TEvent&",
						//	thus allow Register() and Unregister(), but reserving Raise() to itself

						void	Register	(IReceiver* receiver) const
						{
// 							RaiseChanged(ACTION_REGISTER, receiver);
							receivers.Push(receiver);
						}

						template<class TReceiver>
						void	Register	(TReceiver* receiver, void (TReceiver::*func)(TEvent&,TSender&,TData)) const
						{
							system::memory::TUniquePtr<IReceiver> proxy(new TMemberFuncReceiver<TReceiver>(receiver, func));
// 							RaiseChanged(ACTION_REGISTER, proxy.Object());
							receivers.Push(proxy.Object());
							proxy.Reset();	//	reset after proxy has been successfully added to the list
						}

						template<class TReceiverData>
						void	Register	(void (*func)(TEvent&,TSender&,TData,TReceiverData&), const TReceiverData& rdata) const
						{
							system::memory::TUniquePtr<IReceiver> proxy(new TStaticFuncReceiver<TReceiverData>(func, rdata));
// 							RaiseChanged(ACTION_REGISTER, proxy.Object());
							receivers.Push(proxy.Object());
							proxy.Reset();	//	reset after proxy has been successfully added to the list
						}


						void	Unregister	(IReceiver* receiver) const
						{
							//	reverse scanning to (hopefully) improve performance, as receivers which recently were
							//	registered are also likely to be unregistered before receivers which were registered earlier (stack-like)
							//	and thus we need to scan less memory, and also if the receiver was registered multiple times, we remove
							//	the last registry first, this makes the order a bit more stable, and requires less memory moving
							//	however, this is an assumption for the "average" program...
							const usys_t n = receivers.Count();
							for(usys_t i = n-1; i != (usys_t)-1; --i)
								if(receivers[i] == receiver)
								{
// 									RaiseChanged(ACTION_UNREGISTER, receivers[i]);
									receivers.Remove(i,1);
									return;	//	only remove one registry (the receiver might be registered multiple times)
								}
							//	fail if the receiver was not registered
							CL3_CLASS_FAIL(error::TException, "IReceiver not registered (receiver = %p)", receiver);
						}

						template<class TReceiver>
						void	Unregister	(TReceiver* receiver, void (TReceiver::*func)(TEvent&,TSender&,TData)) const
						{
							const usys_t n = receivers.Count();
							for(usys_t i = n; i > 0; --i)
							{
								TMemberFuncReceiver<TReceiver>* proxy = dynamic_cast<TMemberFuncReceiver<TReceiver>*>(receivers[i-1]);
								if(proxy != NULL && proxy->receiver == receiver && proxy->func == func)
								{
// 									RaiseChanged(ACTION_UNREGISTER, proxy);
									receivers.Remove(i-1,1);
									delete proxy;	//	clean up what we created in the first place
									return;
								}
							}
							CL3_CLASS_FAIL(error::TException, "TReceiver not registered (receiver = %p)", receiver);
						}

						template<class TReceiverData>
						void	Unregister	(void (*func)(TEvent&,TSender&,TData,TReceiverData& rdata)) const
						{
							const usys_t n = receivers.Count();
							for(usys_t i = n; i > 0; --i)
							{
								TStaticFuncReceiver<TReceiverData>* proxy = dynamic_cast<TStaticFuncReceiver<TReceiverData>*>(receivers[i-1]);
								if(proxy != NULL && proxy->func == func)
								{
// 									RaiseChanged(ACTION_UNREGISTER, proxy);
									receivers.Remove(i-1,1);
									delete proxy;	//	clean up what we created in the first place
									return;
								}
							}
							CL3_CLASS_FAIL(error::TException, "receiver-function not registered (receiver = %p)", func);
						}

						template<class TReceiverData>
						void	Unregister	(void (*func)(TEvent&,TSender&,TData,TReceiverData& rdata), const TReceiverData& rdata) const
						{
							const usys_t n = receivers.Count();
							for(usys_t i = n; i > 0; --i)
							{
								TStaticFuncReceiver<TReceiverData>* proxy = dynamic_cast<TStaticFuncReceiver<TReceiverData>*>(receivers[i-1]);
								if(proxy != NULL && proxy->func == func && proxy->rdata == rdata)
								{
// 									RaiseChanged(ACTION_UNREGISTER, proxy);
									receivers.Remove(i-1,1);
									delete proxy;	//	clean up what we created in the first place
									return;
								}
							}
							CL3_CLASS_FAIL(error::TException, "receiver-function not registered (receiver = %p)", func);
						}
				};

				template<ESharing sharing, class T>
				class	TInterlocked;

				template<class T>
				class	TInterlocked<MUTEX_PRIVATE,T> : public TRWMutex
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
				class	TInterlocked<MUTEX_SHARED,T> : public IMutex
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
