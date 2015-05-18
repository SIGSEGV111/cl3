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

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <pthread.h>

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

			/**************************************************************************************/

			static CL3_THREAD IThread* th_self = NULL;

			namespace
			{
				struct	TMainThread : IThread
				{
					void	ThreadMain	()
					{
						CL3_CLASS_LOGIC_ERROR(true);
					}

					CLASS	TMainThread	() : IThread("main")
					{
						th_self = this;
					}

					CLASS	~TMainThread()
					{
						th_self = NULL;
					}
				};

				static TMainThread th_main;
			}

			IThread*	IThread::Self	()
			{
				return th_self;
			}

			/**************************************************************************************/

			static TProcess proc_self(0);

			pid_t	TProcess::Handle	() const
			{
				return this->handle == 0 ? getpid() : this->handle;
			}

			const TList<IThread*>&
					TProcess::Threads	() const
			{
				return this->ls_threads;
			}

			void	TProcess::Shutdown	()
			{
				CL3_CLASS_ERROR(this->handle == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->handle, SIGTERM));
			}

			void	TProcess::Kill		()
			{
				CL3_CLASS_ERROR(this->handle == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->handle, SIGKILL));
			}

			void	TProcess::Suspend	()
			{
				CL3_CLASS_ERROR(this->handle == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->handle, SIGSTOP));
			}

			void	TProcess::Resume	()
			{
				CL3_CLASS_ERROR(this->handle == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(this->handle, SIGCONT));
			}

			int		TProcess::Wait		(time::TTime timeout)
			{
				CL3_CLASS_ERROR(this->handle == -1, TException, "the process was already shut down or did never ran in the first place");
				if(timeout != -1) CL3_NOT_IMPLEMENTED;	//	FIXME

				int status_child;
				CL3_CLASS_SYSERR(waitpid(this->handle, &status_child, 0));
				this->handle = -1;
				return status_child;
			}

			EState	TProcess::State		() const
			{
				if(this->handle == -1)
					return STATE_DEAD;
				else
				{
					if(::kill(this->handle, 0) == -1)
					{
						CL3_CLASS_SYSERR(errno != ESRCH);
						return STATE_DEAD;
					}
					else
						return STATE_ALIVE_EXECUTING;	//	FIXME
				}
			}

			CLASS	TProcess::TProcess	(pid_t pid) : handle(pid)
			{
				CL3_CLASS_ERROR(pid < -1 || (this != &proc_self && pid == 0), TException, "pid must be > 0");

				if(pid != -1)
				{
					if(pid == 0)
					{
						//	proc_self
						CL3_CLASS_LOGIC_ERROR(this != &proc_self);
						ls_threads.Append(th_self);
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
			}

			CLASS	TProcess::TProcess	(TProcess&& other) : handle(other.handle)
			{
				other.handle = -1;
			}

			CLASS	TProcess::~TProcess	()
			{
				if(this->handle > 0)
				{
					CL3_CLASS_SYSERR(::ptrace(PTRACE_DETACH, this->handle, NULL, NULL));
				}
			}

			TProcess*	TProcess::Self		()
			{
				return &proc_self;
			}

			/**************************************************************************************/

			void	IThread::Sleep	(TTime time, EClock clock)
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

			void*	IThread::PThreadMain	(void* self)
			{
				th_self = (IThread*)self;
				try
				{
					((IThread*)self)->ThreadMain();
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

			void	IThread::Start		(usys_t sz_stack)
			{
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
					CL3_CLASS_PTHREAD_ERROR(pthread_create(&this->pth, &attr, &IThread::PThreadMain, this));
					this->state = STATE_ALIVE_EXECUTING;
				}
				catch(...)
				{
					pthread_attr_destroy(&attr);
					throw;
				}
			}

			void	IThread::Shutdown	()
			{
				CL3_CLASS_ERROR(this->state == STATE_DEAD, TException, "thread is already dead");

				TException* ex = NULL;
				CL3_CLASS_PTHREAD_ERROR(pthread_join(this->pth, (void**)&ex));
				this->state = STATE_DEAD;
				if(ex)
					CL3_CLASS_FORWARD_ERROR(ex, TException, "thread raised an exception");
			}

			void	IThread::Suspend	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			void	IThread::Resume		()
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	IThread::IThread	(io::text::string::TString name) : name(name), state(STATE_DEAD)
			{
				TProcess::Self()->ls_threads.Append(this);
			}

			CLASS	IThread::~IThread	()
			{
				TProcess::Self()->ls_threads.Remove(this);
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
				CL3_CLASS_ERROR(this->handle != -1, TException, "process is already/still running and cannot be started a second time in parallel");

				int pipe_c2p[2] = {-1,-1};
				int pipe_p2c[2] = {-1,-1};

				TCString cstr_exe(exe, CODEC_CXX_CHAR);
				TList<char*> ls_cstr_args;

				ls_cstr_args.Append((char*)cstr_exe.Chars());
				for(auto it = args.CreateStaticIterator(); it->IsValid(); it->MoveNext())
					ls_cstr_args.Append((char*)TCString(it->Item(), CODEC_CXX_CHAR).Claim());
				ls_cstr_args.Append((char*)NULL);

				this->handle = -1;
				try
				{
					CL3_CLASS_SYSERR(::pipe(pipe_c2p));
					CL3_CLASS_SYSERR(::pipe(pipe_p2c));
					CL3_CLASS_SYSERR(this->handle = fork());

					if(this->handle == 0)
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
						catch(...)
						{
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
					if(this->handle != -1) ::kill(handle, SIGTERM);
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
				//	nothing else to do
			}

			CLASS	TProcessRunner::~TProcessRunner	()
			{
				if(this->handle != -1)
				{
					this->CloseTX();
					this->CloseRX();
					this->Wait();
				}
			}
		}
	}
}

#endif
