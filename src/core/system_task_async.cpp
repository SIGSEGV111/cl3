/*
 * libcl3 - common library version 3
 * Copyright (C) 2013	Simon Brennecke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "system_task_synchronization.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			namespace	async
			{
				using namespace synchronization;
				using namespace io::collection::list;

				usys_t TAsyncEventProcessor::ProcessEvents()
				{
					TList<TCallback*> callbacks;

					for(TCallback* p = this->callback_first; p != NULL; p = p->callback_next)
						callbacks.Add(p);

					const usys_t n_callbacks = callbacks.Count();

					struct pollfd pfds[n_callbacks];
					for(usys_t i = 0; i < n_callbacks; i++)
						pfds[i] = callbacks[i]->waitable->__PollInfo();

					int n_events;
					CL3_CLASS_SYSERR(n_events = ::poll(pfds, n_callbacks, 0));

					if(n_events > 0)
						for(usys_t i = 0; i < n_callbacks; i++)
							if(pfds[i].revents != 0)
								callbacks[i]->receiver->AsyncCallback(this, callbacks[i]->waitable);

							return n_events;
				}

				void TAsyncEventProcessor::TCallback::Register(TAsyncEventProcessor* aep, IWaitable* waitable, IReceiver* receiver)
				{
					this->Unregister();

					this->callback_next = aep->callback_first;
					this->callback_prev = NULL;
					if(this->callback_next)
						this->callback_next->callback_prev = this;
					aep->callback_first = this;

					this->aep = aep;
					this->waitable = waitable;
					this->receiver = receiver;
				}

				void TAsyncEventProcessor::TCallback::Unregister()
				{
					if(this->IsRegistered())
					{
						if(this->aep->callback_first == this)
							this->aep->callback_first = this->callback_next;
						if(this->callback_next)
							this->callback_next->callback_prev = this->callback_prev;
						if(this->callback_prev)
							this->callback_prev->callback_next = this->callback_next;

						this->callback_next = this;
						this->callback_prev = this;
					}
				}

				bool TAsyncEventProcessor::TCallback::IsRegistered() const
				{
					return !(this->callback_next == this && this->callback_prev == this);
				}

				CLASS TAsyncEventProcessor::TCallback::TCallback() : callback_next(this), callback_prev(this), aep(NULL), waitable(NULL), receiver(NULL)
				{
				}

				CLASS TAsyncEventProcessor::TCallback::TCallback(TAsyncEventProcessor* aep, IWaitable* waitable, IReceiver* receiver) : callback_next(this), callback_prev(this), aep(NULL), waitable(NULL), receiver(NULL)
				{
					this->Register(aep, waitable, receiver);
				}

				CLASS TAsyncEventProcessor::TCallback::~TCallback()
				{
					this->Unregister();
				}

				static memory::TUniquePtr<TAsyncEventProcessor> aep_default;

				usys_t TAsyncEventProcessor::CountCallbacks() const
				{
					usys_t n = 0;
					for(TCallback* p = this->callback_first; p != NULL; p = p->callback_next)
						n++;
					return n;
				}

				TAsyncEventProcessor& TAsyncEventProcessor::Default()
				{
					if(aep_default == NULL)
						aep_default = memory::MakeUniquePtr(new TAsyncEventProcessor());
					return *aep_default;
				}

				CLASS TAsyncEventProcessor::TAsyncEventProcessor() : callback_first(NULL)
				{
				}
			}
		}
	}
}
