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

#ifndef	_include_cl3_core_system_task_hpp_
#define	_include_cl3_core_system_task_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_collection_list.hpp"
#include "io_stream_fd.hpp"
#include "io_text_string.hpp"
#include "system_time.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	task
		{
			typedef int pid_t;

			namespace	synchronization
			{
				struct	IWaitable;
			}

			class	TProcess;
			class	TThread;
			class	IFiber;

			enum ELogLevel
			{
				LOGLEVEL_DIAG,
				LOGLEVEL_INFO,
				LOGLEVEL_WARN,
				LOGLEVEL_ERROR
			};

			class	IFiber
			{
				friend class TThread;
				private:
					io::text::string::TString name;

				protected:
					CL3PUBF void Log(const io::text::string::TString& message);
					virtual void Main() = 0;

				public:
					volatile ELogLevel loglevel;
					const io::text::string::TString& Name() const;
					CL3PUBF const io::collection::list::TList<synchronization::IWaitable*>& WaitList() const;
					CL3PUBF CLASS IFiber(io::text::string::TString name);
			};

			class	TThread
			{
				friend class TProcess;
				protected:
					CL3PUBF CLASS TThread(TProcess*, pid_t);

				public:
					CL3PUBF CLASS TThread(IFiber*);
			};

			class	TProcess
			{
				public:
					const io::collection::list::TList<TThread*>& Threads() const CL3_GETTER;

					CL3PUBF CLASS TProcess(pid_t);
			};
		}
	}
}

#endif
