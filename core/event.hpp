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

#ifndef	_include_cl3_core_event_hpp_
#define	_include_cl3_core_event_hpp_

#include "error-base.hpp"
#include "system_compiler.hpp"
#include "io_collection_basiclist.hpp"

namespace	cl3
{
	namespace	event
	{
		using namespace system::types;
		template<class TSender, class TData> class TEvent;

		struct	CL3PUBT	IGenericObservable
		{
			//	the data of the void* event parameter are defined by the class implementing the IGenericObservable interface
			virtual	CL3_GETTER	const TEvent<IGenericObservable, void*>&	OnChange	() const = 0;
		};

		template<class TSender, class TData>
		struct	CL3PUBT	IObservable //: IGenericObservable
		{
			typedef TEvent<TSender, TData>	TOnChangeEvent;
			virtual	const TOnChangeEvent&	OnChange	() const = 0;
		};

		template<class TSender, class TData>
		class	CL3PUBT	TEvent //: public IGenericObservable
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
				mutable TEvent<IGenericObservable,void*>* on_change;

				void	RaiseChanged	(EAction action, IReceiver* receiver) const
				{
					if(on_change != NULL)
					{
						TOnChangeData data = { action, receiver };
						on_change->Raise(const_cast<TEvent*>(this), &data);
					}
				}

			public:
				CLASS	TEvent	() : receivers(), on_change(NULL) {}
				CLASS	TEvent	(const TEvent&) : receivers(), on_change(NULL) {}

				CLASS	~TEvent	()
				{
					const usys_t n = receivers.Count();
					for(usys_t i = 0; i < n; ++i)
						if(dynamic_cast<__ICleanup*>(receivers[i]))	//	I clean up! :-)
							delete receivers[i];
					delete on_change;
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

				//	NOTE: it is possible to Register() and Unregister() on a "const TEvent&", while Raise() can only be called on a non-const reference
				//	this allows classes which use TEvent to provide a function which returns a "const TEvent&",
				//	thus allow Register() and Unregister(), but reserving Raise() to itself

				void	Register	(IReceiver* receiver) const
				{
					RaiseChanged(ACTION_REGISTER, receiver);
					receivers.Push(receiver);
				}

				template<class TReceiver>
				void	Register	(TReceiver* receiver, void (TReceiver::*func)(TEvent&,TSender&,TData)) const
				{
					system::memory::TUniquePtr<IReceiver> proxy(new TMemberFuncReceiver<TReceiver>(receiver, func));
					RaiseChanged(ACTION_REGISTER, proxy.Object());
					receivers.Push(proxy.Object());
					proxy.Reset();	//	reset after proxy has been successfully added to the list
				}

				template<class TReceiverData>
				void	Register	(void (*func)(TEvent&,TSender&,TData,TReceiverData&), const TReceiverData& rdata) const
				{
					system::memory::TUniquePtr<IReceiver> proxy(new TStaticFuncReceiver<TReceiverData>(func, rdata));
					RaiseChanged(ACTION_REGISTER, proxy.Object());
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
					for(usys_t i = n; i > 0; --i)
						if(receivers[i-1] == receiver)
						{
							RaiseChanged(ACTION_UNREGISTER, receivers[i]);
							receivers.Remove(i-1,1);
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
							RaiseChanged(ACTION_UNREGISTER, proxy);
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
							RaiseChanged(ACTION_UNREGISTER, proxy);
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
							RaiseChanged(ACTION_UNREGISTER, proxy);
							receivers.Remove(i-1,1);
							delete proxy;	//	clean up what we created in the first place
							return;
						}
					}
					CL3_CLASS_FAIL(error::TException, "receiver-function not registered (receiver = %p)", func);
				}

				const TEvent<IGenericObservable,void*>&	OnChange	() const
				{
					if(on_change == NULL)
						on_change = new TEvent<IGenericObservable,void*>();
					return *on_change;
				}
		};
	}
}

#endif
