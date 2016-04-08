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

/*
	*** WARNING WARNING WARNING ***
	this file contains black magic!
	do not try this at home!
	it is utterly linux, and probably also architecture and kernel-version specific
	but it seems to work for now....
*/

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "system_os.hpp"

#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)

#include "system_task.hpp"
#include "io_text_string.hpp"
#include "io_collection_list.hpp"
#include "io_text_encoding.hpp"
#include "io_file.hpp"
#include "util.hpp"
#include "error.hpp"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>

#include <valgrind/valgrind.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace cl3::system::types;

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			using namespace error;
			using namespace system::memory;
			using namespace io::text::string;
			using namespace io::text::encoding;
			using namespace io::collection::list;

			static char** argv = NULL;
			static char** envv = NULL;

			static TUniquePtr<const TList<const TString> > ls_args;
			static TUniquePtr<const TList<const TString> > ls_env;
		}
	}
}

static const char chr_pad = 0x01;
static const usys_t len_progname_want = 64;
static usys_t len_progname_got = 0;

static void cl3_init(int argc, char* argv[], char* envv[]);
__attribute__((section(".init_array"))) void (* p_cl3_init)(int,char*[],char*[]) = &cl3_init;

static bool UsingGDB()
{
	char buffer[256];
	snprintf(buffer, 256, "/proc/%d/exe", getppid());
	ssize_t l = readlink(buffer, buffer, 256);
	buffer[l] = 0;
	return l > 4 && memcmp(buffer+l-4, "/gdb", 4) == 0;
}

static void cl3_init(int argc, char* argv[], char* envv[])
{
	if(p_cl3_init == NULL) throw;	//	p_cl3_main is never NULL; and the compiler knows; thus he will optimize this away, but he will not complain about an unused variable

	const usys_t len_progname_current = strlen(argv[0]);

	if(RUNNING_ON_VALGRIND == 0 && !UsingGDB())	// valgrind resets argv[0], thus libcl3 will keep execvpe()'ing until all eternity
	{
		if(len_progname_current < len_progname_want - 1)
		{
			puts("*** now performing black magic ... ");

			//	enlarge the program-name buffer
			//	this works by execvpe()'ing ourself with a larger (padded) program-name as argv[0], we will have to remove that padding later again

			//	create a new argv
			char* argv_new[argc+1];
			char program_new[len_progname_want];
			memcpy(argv_new, argv, sizeof(char*) * argc);
			argv_new[0] = program_new;
			argv_new[argc] = NULL;

			//	copy program-name, but pad it with chr_pad characters to the desired length
			::memcpy(argv_new[0], argv[0], len_progname_current);
			::memset(argv_new[0]+len_progname_current, chr_pad, len_progname_want-len_progname_current-1);
			argv_new[0][len_progname_want-1] = 0;

			//	execvpe() ourself with a new (padded) program-name
			::execve(argv[0], argv_new, envv);

			//	it did not work... to bad. lets continue as if nothing had happend
			perror("execve");
			puts("*** black magic has failed, but we survived it!");
		}
		else
		{
			puts("*** black magic succeeded!");

			//	fix the program-name buffer, we have to assume it is as messed up as we wanted it to be when we called execvpe()
			for(char* p = argv[0]; *p != 0; p++)
				if(*p == chr_pad)
					*p = 0;
		}
	}

	len_progname_got = len_progname_current;
	cl3::system::task::argv = argv;
	cl3::system::task::envv = envv;
}

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			using namespace io::file;
			using namespace io::collection::map;
			using namespace memory;
			using namespace error;

			pid_t gettid()
			{
				return (pid_t)syscall(SYS_gettid);
			}

			/************************************************************************************/

			static void ReadProcFile(const char* filename, char*& buffer, usys_t& sz)
			{
				sz = 4096;
				usys_t p = 0;
				ssize_t r;
				buffer = (char*)Alloc(sz, NULL);
				int fd;
				CL3_NONCLASS_SYSERR(fd = ::open(filename, O_RDONLY|O_CLOEXEC|O_NOCTTY));

				for(;;)
				{
					CL3_NONCLASS_SYSERR(r = ::read(fd, buffer + p, sz - p));
					if(r == 0)
						break;

					p += r;
					if(sz == p)
					{
						sz += 512 + sz/5;
						buffer = (char*)Realloc(buffer, sz, NULL, false);
					}
				}
				::close(fd);
				sz = p;
			}

			io::text::string::TString TLocalProcess::Executable() const
			{
				char symlink[32] = {};
				snprintf(symlink, sizeof(symlink), "/proc/%d/exe", this->pid);
				char exe[256];
				CL3_CLASS_SYSERR(readlink(symlink, exe, sizeof(exe)));
				return io::text::string::TString(exe);
			}

			const io::collection::list::TList<const io::text::string::TString>& TProcess::Arguments() const
			{
				if(this->args == NULL)
				{
					this->args = MakeUniquePtr(new io::collection::list::TList<const io::text::string::TString>());

					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/cmdline", this->pid);
					char* buffer = NULL;
					usys_t sz = 0;

					try
					{
						ReadProcFile(filename, buffer, sz);

						if(sz > 0)
							this->args->Append(TString(buffer));

						char* e = buffer + sz - 1;
						for(char* p = buffer; p < e; p++)
							if(*p == 0)
								this->args->Append(TString(p+1));

						Free(buffer);
					}
					catch(...)
					{
						Free(buffer);
						throw;
					}
				}

				return *this->args;
			}

			static TPair<TString, TString> SplitKeyValue(const TString& str)
			{
				TString key = str.Left(str.Find("="));
				TString value = str.Slice(str.Find("=")+1);
				return TPair<TString, TString>(key, value);
			}

			const io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>& TProcess::Environment() const
			{
				if(this->env == NULL)
				{
					this->env = MakeUniquePtr(new io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>());

					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/environ", this->pid);
					char* buffer = NULL;
					usys_t sz = 0;

					try
					{
						ReadProcFile(filename, buffer, sz);

						if(sz > 0)
							this->env->Add(SplitKeyValue(buffer));

						char* e = buffer + sz - 1;
						for(char* p = buffer; p < e; p++)
							if(*p == 0)
								this->env->Add(SplitKeyValue(p+1));

						Free(buffer);
					}
					catch(...)
					{
						Free(buffer);
						throw;
					}
				}
				return *this->env;
			}
		}
	}
}

#endif
