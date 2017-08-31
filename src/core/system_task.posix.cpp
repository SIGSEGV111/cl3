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
#include "system_task_async.hpp"

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <pthread.h>
#include <sys/wait.h>

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

			/**************************************************************************************/

			CLASS TLocalProcess::TLocalProcess() : TProcess(getpid())
			{
			}

			CLASS	TProcess::TProcess(pid_t pid) : pid(pid)
			{
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

			CLASS TLocalThread::TLocalThread(IFiber* fiber, bool autostart) : fiber(fiber)
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
					try
					{
						//	child
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
