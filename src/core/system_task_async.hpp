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

#ifndef	_include_cl3_core_system_task_event_hpp_
#define	_include_cl3_core_system_task_event_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "system_time.hpp"

namespace cl3
{
	using namespace system::types;

	namespace system
	{
		namespace task
		{
			namespace	synchronization
			{
				struct IWaitable;
			}

			namespace	async
			{
				class TAsyncEventProcessor
				{
					public:
						class TCallback
						{
							public:
								struct IReceiver
								{
									virtual void AsyncCallback(TAsyncEventProcessor*, synchronization::IWaitable*) = 0;
								};

							private:
								TCallback* callback_next;
								TCallback* callback_prev;

							protected:
								friend class TAsyncEventProcessor;

								TAsyncEventProcessor* aep;
								synchronization::IWaitable* waitable;
								IReceiver* receiver;

							public:
								CL3PUBF void Register(TAsyncEventProcessor*, synchronization::IWaitable*, IReceiver*);
								CL3PUBF void Unregister();
								CL3PUBF bool IsRegistered() const CL3_GETTER;

								CLASS TCallback(const TCallback&) = delete;
								CL3PUBF CLASS explicit TCallback();
								CL3PUBF CLASS explicit TCallback(TAsyncEventProcessor*, synchronization::IWaitable*, IReceiver*);
								CL3PUBF CLASS ~TCallback();
						};

					protected:
						TCallback* callback_first;

					public:
						CL3PUBF usys_t CountCallbacks() const CL3_GETTER;	//	returns the number of registered callbacks
						CL3PUBF bool ProcessEvents(time::TTime timeout = -1);	//	processes all pending events
						CL3PUBF static TAsyncEventProcessor& Default();
						CL3PUBF CLASS TAsyncEventProcessor();
				};
			}
		}
	}
}

#endif
