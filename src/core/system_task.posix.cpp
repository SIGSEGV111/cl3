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
#include <valgrind/valgrind.h>

#include <stdio.h>

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			struct	TUTF32;
		}
	}

	namespace	system
	{
		namespace	task
		{
			using namespace io::collection;
			using namespace io::collection::list;
			using namespace io::text::string;
			using namespace io::stream;
			using namespace io::stream::fd;
			using namespace io::text::encoding;
			using namespace error;
			using namespace memory;
			using namespace time;
			using namespace async;
			using namespace synchronization;

			/**************************************************************************************/

			pid_t gettid();

			static CL3_THREAD TLocalThread* th_self = NULL;

			struct TMainThread : public TLocalThread
			{
				CLASS TMainThread()
				{
					th_self = this;
				}

				CLASS ~TMainThread()
				{
					TEventData ed;
					this->on_shutdown.Raise(*this, ed);
				}
			};

			static TMainThread th_main;

			static CL3_THREAD IFiber* volatile current_fiber = NULL;

			struct TMainFiber : IFiber
			{
				void Main()
				{
					// should never get called
					CL3_CLASS_LOGIC_ERROR(true);
				}

				TMainFiber() : IFiber() { current_fiber = this; }
			};

			static TMainFiber fib_main;

			/**************************************************************************************/

			CLASS TLocalProcess::TLocalProcess() : TProcess(getpid())
			{
			}

			CLASS	TProcess::TProcess(pid_t pid) : pid(pid)
			{
			}

			/**************************************************************************************/

			void IFiber::Boot()
			{
				IFiber* fiber = current_fiber;

				fiber->AfterSwitchTo();

				try { fiber->Main(); }
				catch(const TException* e)
				{
					fiber->e = def::move(MakeUniquePtr(e));
				}
				catch(const TException& e)
				{
					fiber->e = def::move(MakeUniquePtr<const TException>(new TException(e)));
				}
				catch(const char* msg)
				{
					TException* const e = new TException(msg);
					e->Set(fiber, __FILE__, __PRETTY_FUNCTION__, "fiber->Main()" , NULL, __LINE__);
					fiber->e = def::move(MakeUniquePtr<const TException>(e));
				}
				catch(...)
				{
					TException* const e = new TException("unknown error occured during fiber execution");
					e->Set(fiber, __FILE__, __PRETTY_FUNCTION__, "fiber->Main()" , NULL, __LINE__);
					fiber->e = def::move(MakeUniquePtr<const TException>(e));
				}

// 				fprintf(stderr, "TLocalThread::Self() = %p running fiber %p exitting... hopeing to switch to fibber %p\n", TLocalThread::Self(), IFiber::Self(), fiber->caller);

				fiber->state = IFiber::EState::TERMINATED;

				fiber->caller->SwitchTo();

				CL3_NONCLASS_LOGIC_ERROR(true);
			}

			IFiber* IFiber::Self()
			{
				return current_fiber;
			}

			void IFiber::Join()
			{
// 				fprintf(stderr, "TLocalThread::Self() = %p running fiber %p executing join on %p\n", TLocalThread::Self(), IFiber::Self(), this);
				while(this->state != IFiber::EState::TERMINATED)
					this->SwitchTo();

				this->ResetContext();

				// check for active exceptions from the called fiber
				if(this->e != NULL)
				{
					TException e("called fiber exited with an active exception");
					e.Set(this, __FILE__, __PRETTY_FUNCTION__, "IFiber::SwitchTo()" , this->e.Claim(), __LINE__);
					this->thread = NULL;
					throw e;
				}
			}

			void IFiber::AfterSwitchTo()
			{
// 				fprintf(stderr, "TLocalThread::Self() = %p running fiber %p doing post-switch bookkeeping in fibber %p\n", TLocalThread::Self(), IFiber::Self(), this);

				// set the callers state to suspended
				if(this->caller->state != IFiber::EState::TERMINATED)
					this->caller->state = IFiber::EState::SUSPENDED;

				// clear the owner of the calling fiber
				this->caller->thread = NULL;

				// set our state to running
				this->state = IFiber::EState::RUNNING;
			}

			void IFiber::SwitchTo()
			{
// 				fprintf(stderr, "TLocalThread::Self() = %p running fiber %p switching to fiber %p\n", TLocalThread::Self(), IFiber::Self(), this);

				// nothing to do...
				if(this == current_fiber)
					return;

				CL3_CLASS_ERROR(this->state == IFiber::EState::TERMINATED, TException, "cannot switch to terminted fiber; call Join() first");
				CL3_CLASS_LOGIC_ERROR(this->state == IFiber::EState::RUNNING);
				CL3_CLASS_ERROR(compiler::AtomicSwap(this->thread, (TLocalThread*)NULL, TLocalThread::Self()) != NULL, TException, "fiber is already taken by another thread");
				this->caller = current_fiber;
				current_fiber = this;

				// during this call "this" and "current_fiber" will change
				CL3_CLASS_SYSERR(swapcontext(&this->caller->context, &this->context));

				current_fiber->AfterSwitchTo();
			}

			void IFiber::ResetContext()
			{
// 				fprintf(stderr, "TLocalThread::Self() = %p running fiber %p resetting context of fiber %p\n", TLocalThread::Self(), IFiber::Self(), this);

				this->state = IFiber::EState::SUSPENDED;

// 				fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
				memset(&this->context, 0, sizeof(this->context));

				getcontext(&this->context);

				this->context.uc_stack.ss_size = this->sz_stack;
				this->context.uc_stack.ss_sp = this->p_stack.Object();

				makecontext(&this->context, &IFiber::Boot, 0);
			}

			CLASS IFiber::IFiber(usys_t sz_stack) : sz_stack(this == &fib_main ? 0 : sz_stack), p_stack(MakeUniquePtr((byte_t*)(this == &fib_main ? NULL : memory::Alloc(sz_stack, NULL)))), thread(this == &fib_main ? &th_main : NULL), caller(NULL)
			{
				if(this != &fib_main)
				{
					this->ResetContext();
					this->valgrind_stack_id = VALGRIND_STACK_REGISTER(this->p_stack.Object(), this->p_stack.Object() + this->sz_stack);
				}
			}

			CLASS IFiber::~IFiber()
			{
				if(this != &fib_main)
				{
// 					CL3_CLASS_LOGIC_ERROR(this == current_fiber);
					this->Join();
					VALGRIND_STACK_DEREGISTER(this->valgrind_stack_id);
				}
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
						CL3_NONCLASS_FAIL(TException, "the requested clock is not supported by Sleep()");
				}
			}

			void TLocalThread::Suspend()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGSTOP));
			}

			void TLocalThread::Resume()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGCONT));
			}

			void TLocalThread::Kill()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGKILL));
			}

			void TLocalThread::Shutdown()
			{
				CL3_CLASS_PTHREAD_ERROR(pthread_kill(this->pth, SIGTERM));
			}

			void TLocalThread::Start()
			{
				CL3_NOT_IMPLEMENTED;
			}

			TLocalThread* TLocalThread::Self()
			{
				CL3_NONCLASS_LOGIC_ERROR(th_self == NULL);
				return th_self;
			}

			CLASS TLocalThread::TLocalThread(IFiber* fiber, bool autostart)
			{
				CL3_CLASS_ERROR(fiber == NULL, TException, "fiber cannot be NULL");
				CL3_NOT_IMPLEMENTED;
				if(autostart)
					this->Start();
			}

			CLASS TLocalThread::~TLocalThread()
			{
				/*
				 * 1) Acquire lock
				 * 2) if running; Stop()
				 */
// 				CL3_NOT_IMPLEMENTED;
			}

			bool TProcess::IsAlive() const
			{
				const int status = kill(this->pid, 0);
				if(status == -1)
				{
					if(errno == ESRCH)
						return false;
					CL3_CLASS_FAIL(error::TSyscallException, errno);
				}
				else
					return true;
			}

			/**************************************************************************************/

			static TFDStream fds_stdin(STDIN_FILENO);
			static TFDStream fds_stdout(STDOUT_FILENO);
			static TFDStream fds_stderr(STDERR_FILENO);

			io::stream::fd::TFDStream& TLocalProcess::StdIn()
			{
				return fds_stdin;
			}

			io::stream::fd::TFDStream& TLocalProcess::StdOut()
			{
				return fds_stdout;
			}

			io::stream::fd::TFDStream& TLocalProcess::StdErr()
			{
				return fds_stderr;
			}

			/**************************************************************************************/

			struct TPipe
			{
				int fd[2];	//	[0] = read; [1] = write

				void CloseRead()
				{
					if(fd[0] != -1)
					{
						CL3_NONCLASS_SYSERR(::close(fd[0]));
						fd[0] = -1;
					}
				}

				void CloseWrite()
				{
					if(fd[1] != -1)
					{
						CL3_NONCLASS_SYSERR(::close(fd[1]));
						fd[1] = -1;
					}
				}

				int ReadFD() const CL3_GETTER
				{
					return this->fd[0];
				}

				int WriteFD() const CL3_GETTER
				{
					return this->fd[1];
				}

				void Claim(bool read, bool write)
				{
					if(read)
						this->fd[0] = -1;
					if(write)
						this->fd[1] = -1;
				}

				CLASS TPipe()
				{
					fd[0] = -1;
					fd[1] = -1;
					CL3_NONCLASS_SYSERR(::pipe(this->fd));
					try
					{
						CL3_CLASS_LOGIC_ERROR(fd[0] == STDIN_FILENO || fd[0] == STDOUT_FILENO || fd[0] == STDERR_FILENO);
						CL3_CLASS_LOGIC_ERROR(fd[1] == STDIN_FILENO || fd[1] == STDOUT_FILENO || fd[1] == STDERR_FILENO);
					}
					catch(...)
					{
						::close(fd[0]);
						::close(fd[1]);
						throw;
					}
				}

				CLASS ~TPipe()
				{
					if(fd[0] != -1)
						::close(fd[0]);
					if(fd[1] != -1)
						::close(fd[1]);
				}

				CLASS TPipe(const TPipe&) = delete;
			};

			/**************************************************************************************/

			CLASS TProcess::TProcess() : pid(-1)
			{
			}

			CLASS TChildProcess::TChildProcess(const io::text::string::TString& exe,
												const io::collection::list::TList<const io::text::string::TString>& args,
												const io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>& env,
												io::stream::IIn<byte_t>* stdin,
												io::stream::IOut<byte_t>* stdout,
												io::stream::IOut<byte_t>* stderr,
												TAsyncEventProcessor* aep) : TProcess(), is_stdin(stdin), os_stdout(stdout), os_stderr(stderr)
			{
				TUniquePtr<TPipe> pipe_stdin = MakeUniquePtr(stdin != NULL && stdin != &TLocalProcess::StdIn() ? new TPipe() : NULL);
				TUniquePtr<TPipe> pipe_stdout = MakeUniquePtr(stdout != NULL && stdout != &TLocalProcess::StdOut() ? new TPipe() : NULL);
				TUniquePtr<TPipe> pipe_stderr = MakeUniquePtr(stderr != NULL && stderr != &TLocalProcess::StdErr() ? new TPipe() : NULL);

				int pid_child;
				CL3_NONCLASS_SYSERR(pid_child = ::fork());

				if(pid_child == 0)
				{
					//	child
					try
					{
						if(pipe_stdin != NULL)
						{
							pipe_stdin->CloseWrite();
							CL3_NONCLASS_SYSERR(::dup2(pipe_stdin->ReadFD(), STDIN_FILENO));
							pipe_stdin->CloseRead();
						}
						else if(stdin == NULL)
							::close(STDIN_FILENO);	//	FIXME: pass a handle to /dev/null

						if(pipe_stdout != NULL)
						{
							pipe_stdout->CloseRead();
							CL3_NONCLASS_SYSERR(::dup2(pipe_stdout->WriteFD(), STDOUT_FILENO));
							pipe_stdout->CloseWrite();
						}
						else if(stdout == NULL)
							::close(STDOUT_FILENO);	//	FIXME: pass a handle to /dev/null

						if(pipe_stderr != NULL)
						{
							pipe_stderr->CloseRead();
							CL3_NONCLASS_SYSERR(::dup2(pipe_stderr->WriteFD(), STDERR_FILENO));
							pipe_stderr->CloseWrite();
						}
						else if(stderr == NULL)
							::close(STDERR_FILENO);	//	FIXME: pass a handle to /dev/null

						TList<char*> args_cstr;
						args_cstr.Count(args.Count() + 2);	//	+2 for exe filename and terminating NULL
						memset(args_cstr.ItemPtr(0), 0, args_cstr.Count() * sizeof(char*));

						TList<char*> env_cstr;
						env_cstr.Count(env.Count() + 1);	//	+1 for the terminating NULL
						memset(env_cstr.ItemPtr(0),  0, env_cstr.Count()  * sizeof(char*));

						args_cstr[0] = (char*)TCString(exe, CODEC_CXX_CHAR).Claim();

						{
							auto it = args.CreateStaticIterator();
							it->MoveHead();
							for(usys_t i = 1; it->MoveNext(); i++)
								args_cstr[i] = (char*)TCString(it->Item(), CODEC_CXX_CHAR).Claim();
						}

						{
							auto it = env.CreateStaticIterator();
							for(usys_t i = 0; it->MoveNext(); i++)
								env_cstr[i] = (char*)TCString(it->Item().key + "=" + it->Item().value, CODEC_CXX_CHAR).Claim();
						}

						CL3_NONCLASS_SYSERR(::execvpe(args_cstr[0], (char**)args_cstr.ItemPtr(0), (char**)env_cstr.ItemPtr(0)));
					}
					catch(const TException& e)
					{
						e.Print();
					}
					::_exit(1);
					CL3_NONCLASS_LOGIC_ERROR(true);
				}
				else
				{
					//	parent
					if(pipe_stdin != NULL)
					{
						pipe_stdin->CloseRead();
						this->fds_stdin.FD(pipe_stdin->WriteFD());
						this->w_stdin = this->fds_stdin.OnOutputReady();
						pipe_stdin->Claim(false,true);
						this->callback_stdin.Register(aep, &this->w_stdin, this);
					}

					if(pipe_stdout != NULL)
					{
						pipe_stdout->CloseWrite();
						this->fds_stdout.FD(pipe_stdout->ReadFD());
						this->w_stdout = this->fds_stdout.OnInputReady();
						pipe_stdout->Claim(true,false);
						this->callback_stdout.Register(aep, &this->w_stdout, this);
					}

					if(pipe_stderr != NULL)
					{
						pipe_stderr->CloseWrite();
						this->fds_stderr.FD(pipe_stderr->ReadFD());
						this->w_stderr = this->fds_stderr.OnInputReady();
						pipe_stderr->Claim(true,false);
						this->callback_stderr.Register(aep, &this->w_stderr, this);
					}

					this->pid = pid_child;
				}
			}

			CLASS TChildProcess::~TChildProcess()
			{
				::kill(this->pid, SIGKILL);
				int status = 0;
				::waitpid(this->pid, &status, 0);
			}
		}
	}
}

#endif
