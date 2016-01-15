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
#include "io_collection_map.hpp"
#include "io_stream_fd.hpp"
#include "io_text_string.hpp"
#include "system_time.hpp"

#if (CL3_OS == CL3_OS_POSIX)
	#include <poll.h>
	#include <pthread.h>
#elif (CL3_OS == CL3_OS_WINDOWS)
	#include <windows.h>
#endif

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
				struct IWaitable
				{
					#if (CL3_OS == CL3_OS_POSIX)
						virtual struct ::pollfd __PollInfo() const CL3_GETTER = 0;
					#elif (CL3_OS == CL3_OS_WINDOWS)
						virtual HANDLE __Handle() const CL3_GETTER = 0;
					#else
						#error
					#endif

					CL3PUBF bool WaitFor(time::TTime timeout) CL3_WARN_UNUSED_RESULT;
					inline void WaitFor() { CL3_CLASS_LOGIC_ERROR(!this->WaitFor(time::TTime(-1))); }
					CL3PUBF static io::collection::list::TList<bool> WaitFor(const io::collection::list::TList<IWaitable* const>& waitables, time::TTime timeout = -1);
				};

				struct IMutex : IWaitable
				{
					virtual bool Acquire(time::TTime timeout) CL3_WARN_UNUSED_RESULT = 0;
					inline void Acquire() { CL3_CLASS_LOGIC_ERROR(!this->Acquire(time::TTime(-1))); }
					inline bool TryAcquire() CL3_WARN_UNUSED_RESULT { return this->Acquire(time::TTime(0)); }
					virtual void Release() = 0;
					virtual usys_t TimesAcquired() const CL3_GETTER = 0;
					inline bool HasAcquired() const CL3_GETTER { return this->TimesAcquired() > 0; }
				};

				struct ISignal : IMutex
				{
					virtual void Raise() = 0;
					virtual void Clear() = 0;
				};
			}

			enum ELogLevel
			{
				LOGLEVEL_DIAG,
				LOGLEVEL_INFO,
				LOGLEVEL_WARN,
				LOGLEVEL_ERROR
			};

			class IFiber;
			struct IThread;
			class TForeignThread;
			class TLocalThread;
			struct IProcess;
			class TForeignProcess;
			class TLocalProcess;
			class TCPU;
			class TNUMANode;

			/************************************************************************************/

			class IFiber
			{
				private:
					io::text::string::TString name;
					io::collection::list::TList<synchronization::IWaitable*> waitlist;
					IFiber* on_block;

				protected:
					virtual void Main() = 0;

				public:
					CL3PUBF static void Log(ELogLevel reqll, const io::text::string::TString& message);
					CL3PUBF const io::collection::list::TList<synchronization::IWaitable*>& WaitList() const CL3_GETTER;
					CL3PUBF void SwitchTo();
					CL3PUBF void OnBlock(IFiber* on_block) CL3_SETTER;
					CL3PUBF IFiber* OnBlock() const CL3_GETTER;

					CL3PUBF CLASS explicit IFiber(const io::text::string::TString& name);

					CL3PUBF static IFiber* Self();
			};

			/************************************************************************************/

			struct IProcess
			{
				virtual pid_t ID() const CL3_GETTER = 0;
				virtual const io::collection::list::TList<IThread* const>& Threads() const CL3_GETTER = 0;
				virtual io::text::string::TString Executable() const CL3_GETTER = 0;
				virtual io::collection::list::TList<io::text::string::TString>& Arguments() const CL3_GETTER = 0;
				virtual io::collection::map::TStdMap<io::text::string::TString, io::text::string::TString> Environment() const CL3_GETTER = 0;
				//	TODO: io-streams...
			};

			class TForeignProcess : public IProcess
			{
				protected:
					io::collection::list::TList<TForeignThread*> threads;
					pid_t id;

				public:
			};

			class TLocalProcess : public IProcess
			{
				protected:
					io::collection::list::TList<TLocalThread*> threads;
					pid_t id;

				public:
					CL3PUBF static TLocalProcess& Self();
			};

			/************************************************************************************/

			struct IThread
			{
				CL3PUBF static void Sleep(time::TTime time, time::EClock clock = time::TIME_CLOCK_MONOTONIC);
				virtual pid_t ID() const CL3_GETTER = 0;
				virtual io::collection::list::TList<TCPU* const>& Affinity() = 0;
				virtual IProcess* Process() const CL3_GETTER = 0;
				virtual void Suspend() = 0;
				virtual void Resume() = 0;
				virtual void Kill() = 0;
			};

			class TForeignThread : public IThread
			{
				private:
					CLASS TForeignThread(const TForeignThread&) = delete;
					TForeignProcess* process;
					io::collection::list::TList<TCPU* const> affinity;

				protected:
					pid_t pid_thread;

				public:
					CL3PUBF pid_t ID() const final override CL3_GETTER;
					CL3PUBF io::collection::list::TList<TCPU* const>& Affinity() final override;
					CL3PUBF TForeignProcess* Process() const  final override CL3_GETTER;
					CL3PUBF void Suspend() final override;
					CL3PUBF void Resume() final override;
					CL3PUBF void Kill() final override;
			};

			class TLocalThread : public IThread
			{
				private:
					CLASS TLocalThread(const TLocalThread&) = delete;

				protected:
					#if (CL3_OS == CL3_OS_POSIX)
						pthread_t pth;
					#elif (CL3_OS == CL3_OS_WINDOWS)
						HANDLE hth;
					#else
						#error
					#endif

				public:
					CL3PUBF pid_t ID() const final override CL3_GETTER;
					CL3PUBF io::collection::list::TList<TCPU* const>& Affinity() final override;
					CL3PUBF TLocalProcess* Process() const  final override CL3_GETTER;
					CL3PUBF void Suspend() final override;
					CL3PUBF void Resume() final override;
					CL3PUBF void Kill() final override;
					CL3PUBF static TLocalThread* Self();
					CL3PUBF CLASS explicit TLocalThread(IFiber*);
			};

			/************************************************************************************/

			class TCPU
			{
				public:
					CL3PUBF TNUMANode* NumaNode() const CL3_GETTER;
			};

			class TNUMANode
			{
				public:
					CL3PUBF const io::collection::list::TList<TCPU* const>& CPUs() const CL3_GETTER;
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
				io::collection::map::TStdMap<io::text::string::TString, io::text::string::TString> env;
				io::collection::list::TList<pid_t> threads;
			};

			CL3PUBF thread_info_t ThreadInfo(pid_t, pid_t);
			CL3PUBF process_info_t ProcessInfo(pid_t);
			CL3PUBF io::collection::list::TList<pid_t> EnumProcesses();
			CL3PUBF system::memory::TUniquePtr<TForeignProcess> OpenProcess(pid_t);

			CL3PUBF system::memory::TUniquePtr<TForeignProcess> CreateProcess(const io::text::string::TString& exe,
					const io::collection::list::TList<const io::text::string::TString>& args,
					const io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>& env,
					io::stream::IIn<byte_t>* stdin,
					io::stream::IOut<byte_t>* stdout,
					io::stream::IOut<byte_t>* stderr);	//	TODO: default parameters
		}
	}
}

#endif
