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

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "system_task.hpp"
#include "system_task_synchronization.hpp"

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <pthread.h>
#include <sys/wait.h>

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			using namespace io::collection;
			using namespace io::collection::list;
			using namespace io::text::string;
			using namespace io::stream;
			using namespace io::text::encoding;
			using namespace error;
			using namespace memory;
			using namespace time;
			using namespace synchronization;

			/**************************************************************************************/

			pid_t gettid();

			static CL3_THREAD IThreadRunner* th_self = NULL;
			static CL3_THREAD IThreadRunner* th_main = NULL;

			namespace
			{
				struct	TThreadPointerSetter
				{
					CLASS	TThreadPointerSetter	(IThreadRunner* th)
					{
						th_self = th;
						th_main = th;
					}
				};

				struct	TMainThread : TThreadPointerSetter, IThreadRunner
				{
					void	ThreadMain	()
					{
						CL3_CLASS_LOGIC_ERROR(true);
					}

					CLASS	TMainThread	() : TThreadPointerSetter(this), IThreadRunner("main")
					{
						this->tid = gettid();
						this->state = STATE_ALIVE_EXECUTING;
						struct ::sigaction sa;
						memset(&sa, 0, sizeof(sa));
						sa.sa_sigaction = &TThread::SignalHandler;
						sa.sa_flags = SA_RESTART | SA_SIGINFO;
						CL3_CLASS_SYSERR(::sigaction(SIGCHLD, &sa, NULL));
					}

					CLASS	~TMainThread()
					{
						this->state = STATE_DEAD;
					}
				};

				static TMainThread th_main_hidden;
			}

			static TProcess proc_self(0);

			IThreadRunner*	IThreadRunner::Self	()
			{
				return th_self;
			}

			/**************************************************************************************/

			pid_t	TProcess::ID		() const
			{
				return this->pid == 0 ? getpid() : this->pid;
			}

			void	TProcess::Shutdown	()
			{
				CL3_CLASS_ERROR(this->pid == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->pid, SIGTERM));
			}

			void	TProcess::Kill		()
			{
				CL3_CLASS_ERROR(this->pid == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->pid, SIGKILL));
			}

			void	TProcess::Suspend	()
			{
				CL3_CLASS_ERROR(this->pid == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->pid, SIGSTOP));
			}

			void	TProcess::Resume	()
			{
				CL3_CLASS_ERROR(this->pid == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->pid, SIGCONT));
			}

			CLASS	TProcess::TProcess	(pid_t pid) : pid(pid), state(STATE_DEAD), on_statechange(&this->mutex)
			{
				CL3_CLASS_ERROR(pid < -1 || (this != &proc_self && pid == 0), TException, "pid must be > 0");

				if(pid != -1)
				{
					if(pid == 0)
					{
						//	proc_self
						CL3_CLASS_LOGIC_ERROR(this != &proc_self);
						this->ls_threads.Append(th_self);
					}
					else
					{
						CL3_CLASS_SYSERR(::ptrace(PTRACE_ATTACH, pid, NULL, NULL));

						/*	TODO:
							- enumerate all threads
							- set various ptrace flags
						*/
						CL3_CLASS_SYSERR(::ptrace(PTRACE_CONT, pid, NULL, NULL));
					}
				}

				this->Mutex().Release();
			}

			CLASS	TProcess::TProcess	(TProcess&& other) : pid(other.pid), state(other.state), on_statechange(&this->mutex)
			{
				other.pid = -1;
				other.state = STATE_DEAD;
			}

			CLASS	TProcess::~TProcess	()
			{
				this->Mutex().Acquire();
				this->ls_threads.Clear();
			}

			TProcess*	TProcess::Self		()
			{
				return &proc_self;
			}

			/**************************************************************************************/

			void	TThread::Sleep	(TTime time, EClock clock)
			{
				const struct timespec ts = time;
				switch(clock)
				{
					case TIME_CLOCK_TAI:
					case TIME_CLOCK_MONOTONIC:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL));
						break;

					case TIME_CLOCK_REALTIME:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL));
						break;

					case TIME_CLOCK_PROCESS:
						CL3_NONCLASS_SYSERR(clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, 0, &ts, NULL));
						break;

					default:
						CL3_NONCLASS_FAIL(TException, "the requested clock is not supported for Sleep()");
				}
			}

			void	TThread::Shutdown	()
			{
				CL3_CLASS_LOGIC_ERROR(!this->Mutex().HasAcquired());
				CL3_CLASS_LOGIC_ERROR(this->tid <= 0);
				CL3_CLASS_SYSERR(kill(this->tid, SIGTERM));
			}

			void	TThread::Suspend	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	TThread::Resume		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TThread::TThread	() : tid(-1), state(STATE_DEAD), on_statechange(&this->mutex)
			{
				this->Mutex().Release();
			}

			CLASS	TThread::TThread	(pid_t tid) : tid(tid), state(STATE_DEAD), on_statechange(&this->mutex)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TThread::TThread	(TThread&& other) : tid(other.tid), state(other.state), on_statechange(&this->mutex)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TThread::~TThread	()
			{
				this->Mutex().Acquire();
			}

			/**************************************************************************************/

			void*	IThreadRunner::PThreadMain	(void* self)
			{
				th_self = (IThreadRunner*)self;

				{
					TInterlockedRegion lock(&th_self->mutex);
					th_self->tid = gettid();
					th_self->state = STATE_ALIVE_EXECUTING;
					th_self->on_statechange.Raise();
				}

				try
				{
					((IThreadRunner*)self)->ThreadMain();
				}
				catch(const TException& ex)
				{
					return new TException(ex);
				}
				catch(const char* msg)
				{
					TException* ex = new TException("%s", msg);
					ex->Set(NULL, __FILE__, __PRETTY_FUNCTION__, NULL, NULL, __LINE__);
					return ex;
				}
				catch(...)
				{
					TException* ex = new TException("received unknown exception class");
					ex->Set(NULL, __FILE__, __PRETTY_FUNCTION__, NULL, NULL, __LINE__);
					return ex;
				}
				return NULL;
			}

			void	IThreadRunner::Start		(usys_t sz_stack)
			{
				CL3_CLASS_LOGIC_ERROR(!this->Mutex().HasAcquired());
				if(sz_stack == (usys_t)-1)
					sz_stack = 64*1024;
				CL3_CLASS_ERROR(this->state != STATE_DEAD, TException, "thread is already alive");
				this->sz_stack = sz_stack;
				this->p_stack = MakeUniquePtr<system::memory::UPTR_ALLOC>((byte_t*)system::memory::Alloc(sz_stack, NULL));

				pthread_attr_t attr;
				CL3_CLASS_PTHREAD_ERROR(pthread_attr_init(&attr));
				try
				{
					CL3_CLASS_PTHREAD_ERROR(pthread_attr_setstack(&attr, p_stack.Object(), sz_stack));
					CL3_CLASS_PTHREAD_ERROR(pthread_create(&this->pth, &attr, &IThreadRunner::PThreadMain, this));
					this->state = STATE_ALIVE_SUSPENDED;
					this->on_statechange.Raise();
					while(this->state == STATE_ALIVE_SUSPENDED)
						this->on_statechange.WaitFor();
				}
				catch(...)
				{
					pthread_attr_destroy(&attr);
					throw;
				}
			}

			CLASS	IThreadRunner::IThreadRunner	(io::text::string::TString name) : name(name)
			{
				if(th_self != th_main)
				{
					TInterlockedRegion lock(&TProcess::Self()->Mutex());
					TProcess::Self()->ls_threads.Append(this);
				}
			}

			CLASS	IThreadRunner::~IThreadRunner	()
			{
				if(this != th_main)
				{
					TInterlockedRegion lock_this(&this->Mutex());
					while(this->state != STATE_DEAD)
					{
						this->Shutdown();
						this->OnStateChange().WaitFor();
					}

					{
						TInterlockedRegion lock_self(&TProcess::Self()->Mutex());
						TProcess::Self()->ls_threads.Remove(this);
					}
				}
			}

			/**************************************************************************************/

			usys_t	TProcessRunner::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_CLASS_ERROR(this->is.FD() == -1, TException, "input stream is already/still closed");
				return this->is.Read(arr_items_read, n_items_read_max, n_items_read_min);
			}

			usys_t	TProcessRunner::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				CL3_CLASS_ERROR(this->os.FD() == -1, TException, "output stream is already/still closed");
				return this->os.Write(arr_items_write, n_items_write_max, n_items_write_min);
			}

			void	TProcessRunner::Start		(const TString& exe, const io::collection::IStaticCollection<const TString>& args)
			{
				CL3_CLASS_ERROR(this->pid != -1, TException, "process is already/still running and cannot be started a second time in parallel");

				int pipe_c2p[2] = {-1,-1};
				int pipe_p2c[2] = {-1,-1};

				TCString cstr_exe(exe, CODEC_CXX_CHAR);
				TList<char*> ls_cstr_args;

				ls_cstr_args.Append((char*)cstr_exe.Chars());
				for(auto it = args.CreateStaticIterator(); it->IsValid(); it->MoveNext())
					ls_cstr_args.Append((char*)TCString(it->Item(), CODEC_CXX_CHAR).Claim());
				ls_cstr_args.Append((char*)NULL);

				this->pid = -1;
				try
				{
					CL3_CLASS_SYSERR(::pipe(pipe_c2p));
					CL3_CLASS_SYSERR(::pipe(pipe_p2c));
					CL3_CLASS_SYSERR(this->pid = fork());

					if(this->pid == 0)
					{
						//	this is the child
						try
						{
							CL3_CLASS_SYSERR(::close(pipe_c2p[0]));	//	close read-end of child-to-parent pipe
							CL3_CLASS_SYSERR(::close(pipe_p2c[1]));	//	close write-end of parent-to-child pipe
							CL3_CLASS_SYSERR( ::dup2(pipe_c2p[1], STDOUT_FILENO));	//	replace STDOUT with write-end of child-to-parent pipe
							CL3_CLASS_SYSERR( ::dup2(pipe_p2c[0], STDIN_FILENO ));	//	replace STDIN with read-end of parent-to-child pipe
							CL3_CLASS_SYSERR(::close(pipe_c2p[1]));	//	close original fd of the pipe
							CL3_CLASS_SYSERR(::close(pipe_p2c[0]));	//	close original fd of the pipe
							CL3_CLASS_SYSERR(::execvp((char*)cstr_exe.Chars(), (char**)ls_cstr_args.ItemPtr(0)));	//	execute the new binary
						}
						catch(TException* ex)
						{
							ex->Print();
							//	child does no error handling
						}
						::_exit(-7);
					}
					else
					{
						//	this is the parent
						CL3_CLASS_SYSERR(::close(pipe_c2p[1]));	//	close write-end of child-to-parent pipe
						CL3_CLASS_SYSERR(::close(pipe_p2c[0]));	//	close read-end of parent-to-child pipe
						this->os.FD(pipe_p2c[1]);	//	set output-stream to write-end of parent-to-child pipe
						this->is.FD(pipe_c2p[0]);	//	set input-stream to read-end of child-to-parent pipe
					}
				}
				catch(...)
				{
					::close(pipe_c2p[0]);
					::close(pipe_c2p[1]);
					::close(pipe_p2c[0]);
					::close(pipe_p2c[1]);
					if(this->pid != -1) ::kill(this->pid, SIGTERM);
					for(usys_t i = 1; i < ls_cstr_args.Count(); i++)
						system::memory::Free(ls_cstr_args[i]);
					throw;
				}
				for(usys_t i = 1; i < ls_cstr_args.Count(); i++)
					system::memory::Free(ls_cstr_args[i]);
			}

			void	TProcessRunner::CloseTX		()
			{
				if(this->os.FD() != -1)
					this->os.FD(-1);
			}

			void	TProcessRunner::CloseRX		()
			{
				if(this->is.FD() != -1)
					this->is.FD(-1);
			}

			CLASS	TProcessRunner::TProcessRunner	() : TProcess(-1), is(), os()
			{
				TInterlockedRegion lock(&TProcess::Self()->Mutex());
				TProcess::Self()->ls_childs.Add(this);
			}

			CLASS	TProcessRunner::~TProcessRunner	()
			{
				if(this->pid != -1)
				{
					this->CloseTX();
					this->CloseRX();

					TInterlockedRegion lock(&this->Mutex());
					while(this->State() != STATE_DEAD)
					{
						this->OnStateChange().WaitFor();
					}
				}

				TInterlockedRegion lock(&TProcess::Self()->Mutex());
				TProcess::Self()->ls_childs.Remove(this);
			}

			/*******************************************************************/

			void	TThread::SignalHandler	(int signal, siginfo_t* si, void*)
			{
				CL3_NONCLASS_LOGIC_ERROR(signal != si->si_signo);

				switch(signal)
				{
					case SIGCHLD:
					{
						//	 si_pid, si_uid, si_status, si_utime, si_stime
						TInterlockedRegion lock_self(&proc_self.Mutex());
						const usys_t n_threads = proc_self.Threads().Count();
						for(usys_t i = 0; i < n_threads; i++)
						{
							TThread* const thread = proc_self.Threads()[i];
							if(si->si_pid == thread->ID())
							{
								TInterlockedRegion lock_thread(&thread->Mutex());
								switch(si->si_code)
								{
									case CLD_EXITED:
									case CLD_KILLED:
									case CLD_DUMPED:
										CL3_NONCLASS_SYSERR(::waitpid(si->si_pid, NULL, WNOHANG));
										thread->state = STATE_DEAD;
										break;
									case CLD_STOPPED:
										thread->state = STATE_ALIVE_SUSPENDED;
										break;
									case CLD_CONTINUED:
										CL3_NONCLASS_LOGIC_ERROR(thread->state != STATE_ALIVE_SUSPENDED);
										thread->state = STATE_ALIVE_EXECUTING;
										break;
									case CLD_TRAPPED:
										//	we don't do anything special here
										//	we also won't raise a state-change signal, as debugging should be transparent to the application
										return;
									default:
										CL3_NONCLASS_LOGIC_ERROR(true);
								}

								thread->on_statechange.Raise();
								break;
							}
						}

						const usys_t n_processes = proc_self.Childs().Count();
						for(usys_t i = 0; i < n_processes; i++)
						{
							TProcess* const process = proc_self.Childs()[i];
							if(si->si_pid == process->ID())
							{
								TInterlockedRegion lock_process(&process->Mutex());
								switch(si->si_code)
								{
									case CLD_EXITED:
									case CLD_KILLED:
									case CLD_DUMPED:
										CL3_NONCLASS_SYSERR(::waitpid(si->si_pid, NULL, WNOHANG));
										process->state = STATE_DEAD;
										break;
									case CLD_STOPPED:
										process->state = STATE_ALIVE_SUSPENDED;
										break;
									case CLD_CONTINUED:
										CL3_NONCLASS_LOGIC_ERROR(process->state != STATE_ALIVE_SUSPENDED);
										process->state = STATE_ALIVE_EXECUTING;
										break;
									case CLD_TRAPPED:
										//	we don't do anything special here
										//	we also won't raise a state-change signal, as debugging should be transparent to the application
										return;
									default:
										CL3_NONCLASS_LOGIC_ERROR(true);
								}

								process->on_statechange.Raise();
								break;
							}
						}
						break;
					}
					default:
						CL3_UNREACHABLE;
				}
			}
		}
	}
}

#endif
