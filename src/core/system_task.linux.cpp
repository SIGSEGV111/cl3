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

// #include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
// #include <sched.h>

// #include <valgrind/valgrind.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <fcntl.h>

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

static void cl3_init(int argc, char* argv[], char* envv[]);
__attribute__((section(".init_array"))) void (* p_cl3_init)(int,char*[],char*[]) = &cl3_init;

static void cl3_init(int argc, char* argv[], char* envv[])
{
	if(p_cl3_init == NULL) throw;	//	prevent warning about unused variable

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

			static io::collection::list::TList<byte_t> ReadProcFile(const io::text::string::TString& filename)
			{
				io::collection::list::TList<byte_t> buffer;
				io::file::TFile file(filename);
				io::file::TStream stream(&file);
				stream.WriteOut(buffer);
				return buffer;
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
					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/cmdline", this->pid);

					auto args = MakeUniquePtr(new io::collection::list::TList<const io::text::string::TString>());
					const auto buffer = ReadProcFile(filename);

					if(buffer.Count() > 0)
					{
						// TString only initializes itself up to the terminating \0 character, the following code makes use this feature...

						args->Append(TString((const char*)buffer.ItemPtr(0)));

						const char* e = (const char*)buffer.ItemPtr(-1);
						for(const char* p = (const char*)buffer.ItemPtr(0); p < e; p++)
							if(*p == 0)
								args->Append(TString(p+1));
					}

					this->args.AtomicSwap(NULL, def::move(args));
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
					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/environ", this->pid);

					auto env = MakeUniquePtr(new io::collection::map::TStdMap<const io::text::string::TString, const io::text::string::TString>());
					const auto buffer = ReadProcFile(filename);

					if(buffer.Count() > 0)
					{
						env->Add(SplitKeyValue((const char*)buffer.ItemPtr(0)));

						const char* e = (const char*)buffer.ItemPtr(-1);
						for(const char* p = (const char*)buffer.ItemPtr(0); p < e; p++)
							if(*p == 0)
								env->Add(SplitKeyValue(p+1));
					}

					this->env.AtomicSwap(NULL, def::move(env));
				}

				return *this->env;
			}
		}
	}
}

#endif
