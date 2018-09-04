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
			using namespace async;
			using namespace synchronization;

			void IFiber::Return()
			{
				CL3_NONCLASS_LOGIC_ERROR(IFiber::Self()->caller == NULL);
				IFiber::Self()->caller->SwitchTo();
			}

			static memory::TUniquePtr<TSelfProcess> proc_self;

			TSelfProcess* TSelfProcess::Self()
			{
				if(proc_self == NULL)
					proc_self = memory::MakeUniquePtr(new TSelfProcess());
				return proc_self.Object();
			}

			CLASS	TProcess::TProcess(pid_t pid) : pid(pid)
			{
			}

			const io::collection::list::TList<IThread* const>& TSelfProcess::Threads() const
			{
				return this->threads;
			}

			void TChildProcess::AsyncCallback(TAsyncEventProcessor*, synchronization::IWaitable* waitable)
			{
				byte_t buffer[4096];

				if(waitable == &this->w_stdin)
				{
					if(fds_stdin.Space() > 0 && this->is_stdin->Remaining() > 0)
					{
						const usys_t n = this->is_stdin->Read(buffer, sizeof(buffer), 0);
						this->fds_stdin.Write(buffer, n);
					}
					else
					{
						//	no more data (child closed stdout)
						this->callback_stdin.Unregister();
						this->fds_stdin.Close();
					}
				}
				else if(waitable == &this->w_stdout)
				{
					if(fds_stdout.Remaining() > 0)
					{
						const usys_t n = this->fds_stdout.Read(buffer, sizeof(buffer), 1);
						this->os_stdout->Write(buffer, n);
					}
					else
					{
						//	no more data (child closed stdout)
						this->callback_stdout.Unregister();
						this->fds_stdout.Close();
						this->os_stdout->Flush();
					}
				}
				else if(waitable == &this->w_stderr)
				{
					if(fds_stderr.Remaining() > 0)
					{
						const usys_t n = this->fds_stderr.Read(buffer, sizeof(buffer), 1);
						this->os_stderr->Write(buffer, n);
					}
					else
					{
						//	no more data (child closed stderr)
						this->callback_stderr.Unregister();
						this->fds_stderr.Close();
						this->os_stderr->Flush();
					}
				}
				else
					CL3_CLASS_LOGIC_ERROR(true);
			}
		}
	}
}
