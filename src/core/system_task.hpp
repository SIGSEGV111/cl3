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

namespace cl3
{
	using namespace system::types;

	namespace system
	{
		namespace task
		{
			typedef int pid_t;

			namespace synchronization
			{
				struct	IWaitable;
			}

			class TProcess;
			class TLocalThread;
			class IFiber;
			class TCPU;

			typedef io::collection::list::TList<const TCPU* const> TCPUAffinity;

			enum ELogLevel
			{
				LOGLEVEL_DIAG,
				LOGLEVEL_INFO,
				LOGLEVEL_WARN,
				LOGLEVEL_ERROR
			};

			class IFiber
			{
				friend class TLocalThread;
				private:
					io::text::string::TString name;
					TLocalThread* volatile thread;

				protected:
					CL3PUBF void Log(const io::text::string::TString& message);
					virtual void Main() = 0;

				public:
					volatile ELogLevel loglevel;
					const io::text::string::TString& Name() const;
					CL3PUBF const io::collection::list::TList<synchronization::IWaitable*>& WaitList() const;
					CL3PUBF CLASS IFiber(io::text::string::TString name);
					CL3PUBF static IFiber* Current();
					CL3PUBF void SwitchTo();
			};

			/************************************************************************************/

			class IThread
			{
				friend class TProcess;
				private:
					IProcess* process;
					pid_t pid_thread;

				public:
					virtual IProcess* Process() const CL3_GETTER = 0;
					virtual void Suspend() = 0;
					virtual void Resume() = 0;
					virtual void Kill() = 0;
			};

			class TLocalThread : public IThread
			{
				protected:
					CL3PUBF CLASS TLocalThread(IProcess*, pid_t);
					CL3PUBF CLASS TLocalThread(IFiber*);

				public:
					CL3PUBF TLocalProcess* Process() const final override CL3_GETTER;
					CL3PUBF static TLocalThread* Current();
			};

			class TRemoteThread : public IThread
			{
			};

			/************************************************************************************/

			class IProcess
			{
				protected:
					IHost* host;
					io::collection::list::TList<IThread*> threads;

				public:
					CL3PUBF IHost* Host() const CL3_GETTER;
					CL3PUBF const io::collection::list::TList<IThread* const>& Threads() const CL3_GETTER;
					CL3PUBF virtual CLASS ~IProcess();
			};

			class TLocalProcess : public IProcess
			{
			};

			class TOwnProcess : public TLocalProcess
			{
				public:
					CL3PUBF TLocalThread* CreateThread(IFiber*);
					CL3PUBF static TOwnProcess& Instance();
			};

			class TRemoteProcess : public IProcess
			{
			};

			/************************************************************************************/

			class TCPU
			{
				public:
					CL3PUBF TNumaNode* NumaNode() const CL3_GETTER;
			};

			class TNumaNode
			{
				public:
					CL3PUBF const io::collection::TList<TCPU* const>& CPUs() const CL3_GETTER;
			};

			/************************************************************************************/

			struct thread_info_t
			{
				pid_t pid_process;
				pid_t pid_thread;
			};

			struct	process_info_t
			{
				pid_t pid_process;
				io::text::string::TString exe;
				io::collection::list::TList<io::text::string::TString> args;
				io::collection::list::TList<pid_t> threads;
			};

			struct	IHost
			{
				virtual thread_info_t ThreadInfo(pid_t, pid_t) const = 0;
				virtual process_info_t ProcessInfo(pid_t) const = 0;
				virtual io::collection::list::TList<pid_t> EnumProcesses() const = 0;
				virtual IProcess* OpenProcess(pid_t) = 0;
				virtual IProcess* CreateProcess(const io::text::string::TString& exe, const io::collection::list::TList<const io::text::string::TString>& args, io::IIn<byte_t>& stdin, io::IOut<byte_t>& stdout, io::IOut<byte_t>& stderr) = 0;
			};

			class TLocalHost : public IHost
			{
				public:
					CL3PUBF static TLocalHost* Instance();
			};

			class TRemoteHost : public IHost
			{
			};
		}
	}
}

#endif
