#pragma once

#include "system_task_async.hpp"

namespace cl3
{
	namespace io
	{
		namespace stream
		{
			template<typename T>
			class TPump
			{
				protected:
					IIn<T>* is;
					IOut<T>* os;

					system::task::async::TAsyncEvent ae_is_ready;
					system::task::async::TAsyncEvent ae_os_ready;

					TFifo fifo;

					bool FlushFifo()
					{
						if(this->fifo.Remaining() == 0)
							return true;

					}

					void DoPump(TAsyncEventProcessor&, const TAsyncEvent*)
					{

					}

				public:
					CLASS TPump(IIn<T>* is, IOut<T>* os) :
						is(is),
						os(os),
						ae_is_ready(&is->OnReady(), &DoPump),
						ae_os_ready(&os->OnReady(), &DoPump),
						n_buffer(0)
					{
					}
			};
		}
	}
}
