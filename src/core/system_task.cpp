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
#include "error.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			using namespace io::collection::list;

			io::text::string::TString
					IThreadRunner::Name		() const
			{
				return name;
			}

			EState	TProcess::State		() const
			{
				return this->state;
			}

			const TList<TThread*>&
					TProcess::Threads	() const
			{
				return this->ls_threads;
			}

			const TList<TProcess*>&
					TProcess::Childs	() const
			{
				return this->ls_childs;
			}

			system::task::synchronization::TSignal&	TProcess::OnStateChange	() const
			{
				return this->on_statechange;
			}

			system::task::synchronization::TSignal&	TThread::OnStateChange	() const
			{
				return this->on_statechange;
			}

			EState	TThread::State	() const
			{
				CL3_CLASS_LOGIC_ERROR(!this->Mutex().HasAcquired());
				return this->state;
			}

			void IThreadRunner::Shutdown()
			{
				CL3_CLASS_LOGIC_ERROR(!this->Mutex().HasAcquired());
				CL3_CLASS_ERROR(this->state == STATE_DEAD, error::TException, "thread is already dead");
				this->OnShutdownRequest();
			}
		}
	}
}
