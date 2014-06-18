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

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

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

			static CL3_THREAD IThread* th_self = NULL;

			IThread&	IThread::Self	()
			{
				return *th_self;
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

			void	TProcessRunner::Start		()
			{
				CL3_CLASS_ERROR(this->State() != STATE_DEAD, TException, "process is already/still running and cannot be started a second time in parallel");

				int pipe_c2p[2] = {-1,-1};
				int pipe_p2c[2] = {-1,-1};
				CL3_CLASS_SYSERR(::pipe(pipe_c2p));
				CL3_CLASS_SYSERR(::pipe(pipe_p2c));

				TCString cstr_exe(exe, CODEC_CXX_CHAR);
				TList<char*> ls_cstr_args;

				ls_cstr_args.Append((char*)cstr_exe.Chars());
				for(auto it = args.CreateStaticIterator(); it->IsValid(); it->MoveNext())
					ls_cstr_args.Append((char*)TCString(it->Item(), CODEC_CXX_CHAR).Claim());
				ls_cstr_args.Append((char*)NULL);

				this->pid_child = -1;
				try
				{
					CL3_CLASS_SYSERR(this->pid_child = fork());

					if(this->pid_child == 0)
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
							CL3_CLASS_SYSERR(::execvp((char*)cstr_exe.Chars(), (char**)ls_cstr_args.ItemPtr(0)));
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
						if(this->pid_child != -1) ::kill(pid_child, SIGTERM);
						for(usys_t i = 1; i < ls_cstr_args.Count(); i++)
							system::memory::Free(ls_cstr_args[i]);
						throw;
				}
				for(usys_t i = 1; i < ls_cstr_args.Count(); i++)
					system::memory::Free(ls_cstr_args[i]);
			}

			void	TProcessRunner::Shutdown	()
			{
				CL3_CLASS_LOGIC_ERROR(this->pid_child < -1 || this->pid_child == 0);
				CL3_CLASS_ERROR(this->pid_child == -1, TException, "the process was already shut down or did never ran in the first place");

				CloseTX();
				CloseRX();

				//	send the shutdown signal
				if(this->pid_child > 0)
				{
					if(::kill(pid_child, SIGTERM) == -1)
						if(errno != ESRCH)
							CL3_CLASS_SYSERR(-1);
				}
			}

			void	TProcessRunner::Suspend		()
			{
				CL3_CLASS_ERROR(this->pid_child == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(pid_child, SIGSTOP));
			}

			void	TProcessRunner::Resume		()
			{
				CL3_CLASS_ERROR(this->pid_child == -1, TException, "the process was already shut down or did never ran in the first place");
				CL3_CLASS_SYSERR(::kill(pid_child, SIGCONT));
			}

			int		TProcessRunner::Wait		(time::TTime timeout)
			{
				if(timeout != -1) CL3_NOT_IMPLEMENTED;	//	FIXME

				CL3_CLASS_ERROR(this->pid_child == -1, TException, "the process was already shut down or did never ran in the first place");

				int status_child;
				CL3_CLASS_SYSERR(waitpid(this->pid_child, &status_child, 0));
				this->pid_child = -1;
				return status_child;
			}

			void	TProcessRunner::Kill		()
			{
				CL3_CLASS_LOGIC_ERROR(this->pid_child < -1 || this->pid_child == 0);
				if(this->pid_child > 0)
				{
					if(::kill(pid_child, SIGKILL) == -1)
						if(errno != ESRCH)
							CL3_CLASS_SYSERR(-1);
				}
			}

			EState	TProcessRunner::State		() const
			{
				CL3_CLASS_LOGIC_ERROR(this->pid_child < -1 || this->pid_child == 0);
				if(this->pid_child == -1 || ::kill(this->pid_child, 0) == -1) return STATE_DEAD;
				else return STATE_ALIVE_EXECUTING;	//	FIXME
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

			CLASS	TProcessRunner::TProcessRunner	(const io::text::string::TString& exe, const io::collection::IStaticCollection<const io::text::string::TString>& args) : exe(exe), args(args), is(), os(), pid_child(-1)
			{
				//	nothing else to do
			}

			CLASS	TProcessRunner::~TProcessRunner	()
			{
				this->Shutdown();
				this->Wait();
			}
		}
	}
}
