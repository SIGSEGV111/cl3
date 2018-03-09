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
#include "system_task_async.hpp"

#if (CL3_OS == CL3_OS_POSIX)
	#include <poll.h>
	#include <pthread.h>
	#include <ucontext.h>
	typedef ucontext_t fiberdata_t;
#elif (CL3_OS == CL3_OS_WINDOWS)
	#include <windows.h>
#endif

namespace cl3
{
	using namespace system::types;

	namespace io
	{
		namespace file
		{
			class TFile;
		}
	}


	namespace system
	{
		#ifdef __x86_64__
			struct TContextRegisters
			{
				union
				{
					u64_t arr[8];
					struct
					{
						u64_t rbx;
						u64_t rbp;
						u64_t r12;
						u64_t r13;
						u64_t r14;
						u64_t r15;
						u64_t rsp;
						u64_t rip;
					};
				};

				static const unsigned n = sizeof(arr) / sizeof(arr[0]);

				TContextRegisters()
				{
					for(unsigned i = 0; i < n; i++)
						this->arr[i] = 0;
				}

				TContextRegisters(const void* p_stack, const usys_t sz_stack, void (*f_boot)())
				{
					for(unsigned i = 0; i < n - 2; i++)
						this->arr[i] = 0;

					const u64_t p_stack_start_aligned = ((u64_t)p_stack + (u64_t)sz_stack) & (u64_t)0xfffffffffffffff0;

					this->rsp = p_stack_start_aligned;
					this->rip = (u64_t)f_boot;
				}
			};
		#else
			#error "unsupported CPU architecture"
		#endif


// 		namespace task_v2
// 		{
// 			typedef int pid_t;
//
// 			class TThread
// 			{
// 				protected:
// 					pid_t pid;
// 					pid_t tid;
//
// 				public:
// 					CL3PUBF pid_t ID() const CL3_GETTER = 0;
// 					CL3PUBF TProcess Process() const CL3_GETTER = 0;
//
// 					CL3PUBF TString Name() const = 0;
//
// 					CL3PUBF void Shutdown() = 0;
// 					CL3PUBF void Kill() = 0;
// 					CL3PUBF bool IsAlive() const = 0;
//
// 					CL3PUBF IWaitable& OnTerminate() const = 0;
//
// 					CL3PUBF static TThread Self();
// 			};
//
// 			class IThread : public TThread
// 			{
// 				using namespace io::text::string;
// 				private:
//
//
// 				protected:
// 					virtual void Main() = 0;
// 			};
//
// 			class TProcess
// 			{
// 				protected:
// 					pid_t pid;
//
// 				public:
// 					using namespace io::text::string;
// 					using namespace io::collection::list;
// 					using namespace io::collection::map;
// 					using namespace synchronization;
//
// 					struct TOpenFile
// 					{
// 						pid_t pid;
// 						fd_t fd;
//
// 						TString filename; // WARNING: this could be outdated or plain out invalid, maybe even dangerous to use
//
// 						CL3PUBF TFile Open() const;	//	this will always give you exactly the same file as the target process uses
// 					};
//
// 					CL3PUBF pid_t ID() const CL3_GETTER = 0;
// 					CL3PUBF TProcess Parent() const = 0;
// 					CL3PUBF bool IsRootProcess() const CL3_GETTER = 0;
// 					CL3PUBF TList<TProcess> Childs() const = 0;
// 					CL3PUBF TList<TThread> Threads() const = 0;
//
// 					CL3PUBF TString Name() const = 0;
// 					CL3PUBF TString ExecutableFile() const CL3_GETTER = 0;
// 					CL3PUBF TList<TString> Arguments() const CL3_GETTER = 0;
// 					CL3PUBF TStdMap<TString, TString> Environment() const CL3_GETTER = 0;
// 					CL3PUBF TList<const TOpenFile> OpenFiles() const CL3_GETTER = 0;
// 					CL3PUBF TTime StartTime() const CL3_GETTER = 0;
//
// 					CL3PUBF void Shutdown() = 0;
// 					CL3PUBF void Kill() = 0;
// 					CL3PUBF bool IsAlive() const = 0;
//
// 					CL3PUBF IWaitable& OnTerminate() const = 0;
//
// 					CL3PUBF static TProcess Self();
//
// 					CL3PUBF CLASS TProcess(const TProcess&);
// 					CL3PUBF CLASS TProcess(TProcess&&);
// 					CL3PUBF CLASS TProcess(pid_t);
// 					CL3PUBF CLASS ~TProcess();
// 			};
//
// 			class TChildProcess : TProcess
// 			{
// 				public:
//
// 			};
// 		}

		namespace task
		{
			typedef int pid_t;

			namespace	synchronization
			{
				struct IWaitable;
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
			class IProcess;
			class TSelfProcess;
			class TCPU;
			class TNUMANode;

			/************************************************************************************/

			class IFiber
			{
				public:
					enum class EState
					{
						RUNNING,
						SUSPENDED,
						TERMINATED
					};

				private:
					usys_t sz_stack;
					memory::TUniquePtr<byte_t,memory::UPTR_ALLOC> p_stack;
					memory::TUniquePtr<const error::TException> e;

					TLocalThread* volatile thread;
					IFiber* volatile caller;
					volatile EState state;
					volatile bool dirty;

					TContextRegisters registers;

					int valgrind_stack_id;

					static void Boot();

					void AfterSwitchTo();
					void ResetContext();

				protected:
					virtual void Main() = 0;

				public:
					inline EState State() const CL3_GETTER { return this->state; }

					CL3PUBF static IFiber* Self() CL3_GETTER;

					CL3PUBF static void Return();

					CL3PUBF void SwitchTo();
					CL3PUBF void Join();
					CL3PUBF CLASS IFiber(usys_t sz_stack = 16*1024);
					CL3PUBF CLASS ~IFiber();

			};


			/************************************************************************************/

			class CL3PUBT TProcess;

			// one random process in the system we have access to
			class CL3PUBT IProcess
			{
				protected:
					mutable time::TTime ts_start;
					mutable system::memory::TUniquePtr<io::collection::list::TList<const io::text::string::TString>> args;
					mutable system::memory::TUniquePtr<io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>> env;

				public:
					virtual pid_t ID() const CL3_GETTER = 0;

					CL3PUBF io::text::string::TString Name() const;
					CL3PUBF void Name(io::text::string::TString);

					CL3PUBF const io::text::string::TString& Executable() const CL3_GETTER;
					CL3PUBF const io::collection::list::TList<const io::text::string::TString>& Arguments() const CL3_GETTER;
					CL3PUBF const io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>& Environment() const CL3_GETTER;

					CL3PUBF void Shutdown();	//	request shutdown (SIGTERM on POSIX)
					CL3PUBF void Kill();		//	hard kill process (SIGKILL on POSIX)

					struct TOpenFile
					{
						pid_t pid;
						fd_t fd;

						io::text::string::TString filename; // WARNING: this could be outdated or plain out invalid, maybe even dangerous to use

						inline bool operator==(const TOpenFile& rhs) const { return this->pid == rhs.pid && this->fd == rhs.fd; }
						CL3PUBF io::file::TFile Open() const;	//	this will always give you exactly the same file as the target process uses
					};

					CL3PUBF TProcess Parent() const;
					CL3PUBF bool IsRootProcess() const CL3_GETTER;
// 					CL3PUBF io::collection::list::TList<TProcess> Childs() const;
// 					CL3PUBF io::collection::list::TList<TThread> Threads() const;
					CL3PUBF io::collection::list::TList<const TOpenFile> OpenFiles() const CL3_GETTER;
					CL3PUBF time::TTime StartTime() const CL3_GETTER;
					CL3PUBF bool IsAlive() const;

					CL3PUBF IProcess();
					CL3PUBF virtual ~IProcess();
			};

			class CL3PUBT TProcess : public IProcess
			{
				protected:
					pid_t pid;

				public:
					inline pid_t ID() const final override CL3_GETTER { return this->pid; }
					CLASS explicit TProcess(pid_t pid);
					CLASS TProcess(const TProcess& other) : pid(other.pid) {}
			};

			class CL3PUBT TSelfProcess : public IProcess
			{
				private:
					CLASS TSelfProcess(const TSelfProcess&) = delete;
					CLASS TSelfProcess();

				protected:
					io::collection::list::TList<IThread*> threads;

				public:
 					CL3PUBF pid_t ID() const final override CL3_GETTER;
					CL3PUBF const io::collection::list::TList<IThread* const>& Threads() const CL3_GETTER;
					CL3PUBF io::text::string::TString Executable() const CL3_GETTER;
					CL3PUBF static TSelfProcess* Self();

					CL3PUBF static io::stream::fd::TFDStream& StdIn() CL3_GETTER;
					CL3PUBF static io::stream::fd::TFDStream& StdOut() CL3_GETTER;
					CL3PUBF static io::stream::fd::TFDStream& StdErr() CL3_GETTER;
			};

			class TChildProcess : public IProcess, private async::TAsyncEventProcessor::TCallback::IReceiver
			{
				private:
					pid_t pid;
					void AsyncCallback(async::TAsyncEventProcessor*, synchronization::IWaitable*) final override;

				protected:
					io::stream::fd::TFDStream fds_stdin;
					io::stream::fd::TFDStream fds_stdout;
					io::stream::fd::TFDStream fds_stderr;
					io::stream::fd::TWaitable w_stdin;
					io::stream::fd::TWaitable w_stdout;
					io::stream::fd::TWaitable w_stderr;
					async::TAsyncEventProcessor::TCallback callback_stdin;
					async::TAsyncEventProcessor::TCallback callback_stdout;
					async::TAsyncEventProcessor::TCallback callback_stderr;
					io::stream::IIn<byte_t>* is_stdin;
					io::stream::IOut<byte_t>* os_stdout;
					io::stream::IOut<byte_t>* os_stderr;
// 					memory::TUniquePtr<io::stream::fd::TPipe> pipe_stdin;
// 					memory::TUniquePtr<io::stream::fd::TPipe> pipe_stdout;
// 					memory::TUniquePtr<io::stream::fd::TPipe> pipe_stderr;
//
// 					memory::TUniquePtr<io::stream::TPump> pump_stdin;
// 					memory::TUniquePtr<io::stream::TPump> pump_stdout;
// 					memory::TUniquePtr<io::stream::TPump> pump_stderr;

				public:
					inline pid_t ID() const final override CL3_GETTER { return this->pid; }
					CLASS TChildProcess(const TChildProcess&) = delete;

					CL3PUBF CLASS explicit TChildProcess(const io::text::string::TString& exe,
														const io::collection::list::TList<const io::text::string::TString>& args,
														const io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>& env = TSelfProcess::Self()->Environment(),
														io::stream::IIn<byte_t>* stdin = &TSelfProcess::StdIn(),
														io::stream::IOut<byte_t>* stdout = &TSelfProcess::StdOut(),
														io::stream::IOut<byte_t>* stderr = &TSelfProcess::StdErr(),
														async::TAsyncEventProcessor* aep = &async::TAsyncEventProcessor::Default());

					CL3PUBF CLASS ~TChildProcess();
			};

			class TLocalThread
			{
				public:
					CL3PUBF static TLocalThread* Self();

					CL3PUBF static void Sleep(time::TTime time, time::EClock clock = time::EClock::MONOTONIC);
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
			CL3PUBF system::memory::TUniquePtr<IProcess> OpenProcess(pid_t);
		}
	}
}

#endif
