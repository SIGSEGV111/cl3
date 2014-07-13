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
#include "io_text_encoding.hpp"
#include "util.hpp"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

using namespace cl3::system::types;

namespace	cl3
{
	namespace	system
	{
		namespace	task
		{
			static char** argv = NULL;
			static char** envv = NULL;
		}
	}
}

static const usys_t len_progname_want = 64;
static usys_t len_progname_got = 0;

static void cl3_init(int argc, char* argv[], char* envv[]);
__attribute__((section(".init_array"))) void (* p_cl3_init)(int,char*[],char*[]) = &cl3_init;

static void cl3_init(int argc, char* argv[], char* envv[])
{
	if(p_cl3_init == NULL) throw;	//	p_cl3_main is never NULL; and the compiler knows; thus he will optimize this away, but he will not complain about an unused variable

	const usys_t len_progname_current = strlen(argv[0]);

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

		//	copy program-name, but pad it with 0x01 characters to the desired length
		::memcpy(argv_new[0], argv[0], len_progname_current);
		::memset(argv_new[0]+len_progname_current, 1, len_progname_want-len_progname_current-1);
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
			if(*p == 1)
				*p = 0;
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
			using namespace error;
			using namespace io::text::string;
			using namespace io::text::encoding;

			TString	TProcess::Name		() const
			{
				return argv[0];
			}

			void	TProcess::Name		(const TString& new_name)
			{
				const TCString cstr_name(new_name, CODEC_CXX_CHAR);

				CL3_CLASS_ERROR(cstr_name.Count() > len_progname_got, TException, "the new name cannot be longer that the initially reserved amount of space for the program name - cl3 performs black magic to enlarge this buffer; check if it succeeded");

				memset(argv[0] + cstr_name.Count(), 0, len_progname_got - cstr_name.Count());
				memcpy(argv[0], cstr_name.Chars(), cstr_name.Count());
			}
		}
	}
}

#endif
