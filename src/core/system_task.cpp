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

#include "system_task.hpp"
#include "system_memory.hpp"
#include "error.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			using namespace io::collection::list;
			using namespace synchronization;

			IFiber* IFiber::Self()
			{
				return TLocalThread::Self()->CurrentFiber();
			}

			static memory::TUniquePtr<TLocalProcess> proc_self;

			TLocalProcess* TLocalProcess::Self()
			{
				if(proc_self == NULL)
					proc_self = memory::MakeUniquePtr(new TLocalProcess());
				return proc_self.Object();
			}

			pid_t TLocalProcess::ID() const
			{
				return this->pid;
			}

			const io::collection::list::TList<IThread* const>& TLocalProcess::Threads() const
			{
				return this->threads;
			}

			CLASS TLocalThread::TLocalThread()
			{
			}

			void TAsyncEventProcessor::TCallback::Register(TAsyncEventProcessor* aep, IWaitable* waitable, IReceiver* receiver)
			{
				this->Unregister();

				this->aep = aep;
				this->waitable = waitable;
				this->receiver = receiver;

				aep->callbacks.Add(this);
			}

			void TAsyncEventProcessor::TCallback::Unregister()
			{
				if(this->IsRegistered())
				{
					this->aep->callbacks.Remove(this);
					this->aep = NULL;
				}
			}

			bool TAsyncEventProcessor::TCallback::IsRegistered() const
			{
				return this->aep != NULL;
			}

			CLASS TAsyncEventProcessor::TCallback::TCallback() : aep(NULL), waitable(NULL), receiver(NULL)
			{
			}

			CLASS TAsyncEventProcessor::TCallback::TCallback(TAsyncEventProcessor* aep, IWaitable* waitable, IReceiver* receiver) : aep(NULL), waitable(NULL), receiver(NULL)
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
				return this->callbacks.Count();
			}

			TAsyncEventProcessor& TAsyncEventProcessor::Default()
			{
				if(aep_default == NULL)
					aep_default = memory::MakeUniquePtr(new TAsyncEventProcessor());
				return *aep_default;
			}
		}
	}
}
