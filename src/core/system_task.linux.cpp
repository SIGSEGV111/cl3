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

#include <cl3/core/system_os.hpp>

#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)

#include <cl3/core/system_task.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/io_file.hpp>
#include <cl3/core/util.hpp>
#include <cl3/core/error.hpp>

// #include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <string.h>
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

static void cl3_init(int, char* argv[], char* envv[])
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
			using namespace io::collection::list;
			using namespace io::text::encoding;
			using namespace memory;
			using namespace error;
			using namespace io::text::string;

// 			pid_t gettid()
// 			{
// 				return (pid_t)syscall(SYS_gettid);
// 			}

			static TList<byte_t> ReadProcFile(const TString& filename)
			{
				TList<byte_t> buffer;
				io::file::TFile file(filename);
				io::file::TStream stream(&file);
				stream.WriteOut(buffer);
				return buffer;
			}

			struct TStatFile
			{
				int pid;
				TString comm;
				io::text::TUTF32 state;
				int ppid;
				int pgrp;
				int session;
				int tty_nr;
				int tpgid;
				unsigned flags;
				unsigned long minflt;
				unsigned long cminflt;
				unsigned long majflt;
				unsigned long cmajflt;
				unsigned long utime;
				unsigned long stime;
				long cutime;
				long cstime;
				long priority;
				long nice;
				long num_threads;
				long itrealvalue;
				s64_t starttime;
				unsigned long vsize;
				long rss;
				unsigned long rsslim;
				unsigned long startcode;
				unsigned long endcode;
				unsigned long startstack;
				unsigned long kstkesp;
				unsigned long kstkeip;
				unsigned long signal;
				unsigned long blocked;
				unsigned long sigignore;
				unsigned long sigcatch;
				unsigned long wchan;
				unsigned long nswap;
				unsigned long cnswap;
				int exit_signal;
				int processor;
				unsigned rt_priority;
				unsigned policy;
				u64_t delayacct_blkio_ticks;
				unsigned long guest_time;
				long cguest_time;
				unsigned long start_data;
				unsigned long end_data;
				unsigned long start_brk;
				unsigned long arg_start;
				unsigned long arg_end;
				unsigned long env_start;
				unsigned long env_end;
				int exit_code;
			};

			static TStatFile ReadStatFile(pid_t pid)
			{
				char filename[32] = {}; // "/proc/" + sign + 10 digits + "/stat" + null
				snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
				auto buffer = ReadProcFile(TString(filename));
				TString stat((const char*)buffer.ItemPtr(0), buffer.Count());

				TStatFile sf;
				stat >> sf.pid;

				const usys_t p_first_ob = stat.Find("(", 0, io::collection::ESearchDirection::FORWARD);
				CL3_NONCLASS_LOGIC_ERROR(p_first_ob != 0);
				const usys_t p_last_cb = stat.Find(")", stat.Count()-1, io::collection::ESearchDirection::BACKWARD);
				sf.comm = stat.Slice(p_first_ob + 1, p_last_cb - p_first_ob - 1);
				stat.Cut(p_last_cb + 2, 0);

				stat >> sf.state;
				stat >> sf.ppid;
				stat >> sf.pgrp;
				stat >> sf.session;
				stat >> sf.tty_nr;
				stat >> sf.tpgid;
				stat >> sf.flags;
				stat >> sf.minflt;
				stat >> sf.cminflt;
				stat >> sf.majflt;
				stat >> sf.cmajflt;
				stat >> sf.utime;
				stat >> sf.stime;
				stat >> sf.cutime;
				stat >> sf.cstime;
				stat >> sf.priority;
				stat >> sf.nice;
				stat >> sf.num_threads;
				stat >> sf.itrealvalue;
				stat >> sf.starttime;
				stat >> sf.vsize;
				stat >> sf.rss;
				stat >> sf.rsslim;
				stat >> sf.startcode;
				stat >> sf.endcode;
				stat >> sf.startstack;
				stat >> sf.kstkesp;
				stat >> sf.kstkeip;
				stat >> sf.signal;
				stat >> sf.blocked;
				stat >> sf.sigignore;
				stat >> sf.sigcatch;
				stat >> sf.wchan;
				stat >> sf.nswap;
				stat >> sf.cnswap;
				stat >> sf.exit_signal;
				stat >> sf.processor;
				stat >> sf.rt_priority;
				stat >> sf.policy;
				stat >> sf.delayacct_blkio_ticks;
				stat >> sf.guest_time;
				stat >> sf.cguest_time;
				stat >> sf.start_data;
				stat >> sf.end_data;
				stat >> sf.start_brk;
				stat >> sf.arg_start;
				stat >> sf.arg_end;
				stat >> sf.env_start;
				stat >> sf.env_end;
				stat >> sf.exit_code;

				return sf;
			}

			/************************************************************************************/

			TString IProcess::Name() const
			{
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "/proc/%d/comm", this->ID());
				TFile f(buffer, TAccess::RW);
				TStream s(&f);
				TList<byte_t> c;
				s.WriteOut(c);
				if(c[-1] == '\n')
					c.Cut(0,1);
				return TString((const char*)c.ItemPtr(0), c.Count());
			}

			void IProcess::Name(TString new_name)
			{
				char buffer[64];
				snprintf(buffer, sizeof(buffer), "/proc/%d/comm", this->ID());
				TFile f(buffer, TAccess::RW);
				TCString c(new_name, CODEC_CXX_CHAR);
				f.FD().Write((const byte_t*)c.Chars(), c.Count());
			}

			TProcess IProcess::Parent() const
			{
				const TStatFile sf = ReadStatFile(this->ID());
				return TProcess(sf.ppid);
			}

			bool IProcess::IsRootProcess() const
			{
				return this->ID() == 1 || this->ID() == 0;
			}

// 			TList<TProcess> IProcess::Childs() const
// 			{
// 				CL3_NOT_IMPLEMENTED;
// 			}

			TList<const IProcess::TOpenFile> IProcess::OpenFiles() const
			{
				TList<const IProcess::TOpenFile> ofs;

				char fd_dir[32];
				snprintf(fd_dir, sizeof(fd_dir), "/proc/%d/fd", this->ID());

				TDirectoryBrowser db(fd_dir);

				TList<TString> symlinks;
				db.EnumEntries(symlinks);

				for(usys_t i = 0; i < symlinks.Count(); i++)
				{
					TOpenFile of;
					of.pid = this->ID();
					TCString str(TString("/proc/") + of.pid + "/fd/" + symlinks[i], CODEC_CXX_CHAR);
					char buffer[256] = {};
					CL3_CLASS_SYSERR(readlink(str.Chars(), buffer, sizeof(buffer)));
					symlinks[i] >> of.fd;
					of.filename = buffer;
					ofs.Append(of);
				}

				return cl3::system::def::move(ofs);
			}

			time::TTime IProcess::StartTime() const
			{
				if(this->ts_start == time::TTime(-1,-1))
				{
					const TStatFile sf = ReadStatFile(this->ID());
					this->ts_start = time::TTime::ConvertFrom((s64_t)sysconf(_SC_CLK_TCK), (s64_t)sf.starttime);
				}
				return this->ts_start;
			}

			/************************************************************************************/

			static const int FD_SIGNALS[] = { SIGINT, SIGTERM, SIGHUP, SIGQUIT };
			static const int ERROR_SIGNALS[] = { SIGABRT, SIGTRAP, SIGILL, SIGFPE, SIGSEGV, SIGBUS };

			static const unsigned NUM_FD_SIGNALS = sizeof(FD_SIGNALS) / sizeof(FD_SIGNALS[0]);
			static const unsigned NUM_ERROR_SIGNALS = sizeof(ERROR_SIGNALS) / sizeof(ERROR_SIGNALS[0]);

			static io::stream::fd::TFDStream fd_signal;

			CLASS TSelfProcess::TSelfProcess()
			{
				sigset_t ss;

				sigfillset(&ss);
				for(usys_t i = 0; i < NUM_ERROR_SIGNALS; i++)
				{
					sigdelset(&ss, ERROR_SIGNALS[i]);
					struct sigaction sa;
					memset(&sa, 0, sizeof(sa));
					sa.sa_handler = SIG_DFL;
					CL3_CLASS_SYSERR(sigaction(ERROR_SIGNALS[i], &sa, NULL));
				}
				CL3_CLASS_PTHREAD_ERROR(pthread_sigmask(SIG_SETMASK, &ss, NULL));

				sigemptyset(&ss);
				for(usys_t i = 0; i < NUM_FD_SIGNALS; i++)
					sigaddset(&ss, FD_SIGNALS[i]);
				CL3_CLASS_SYSERR(fd_signal = signalfd(-1, &ss, SFD_CLOEXEC|SFD_NONBLOCK));

				//
			}

			TString TSelfProcess::Executable() const
			{
				char symlink[32] = {};
				snprintf(symlink, sizeof(symlink), "/proc/%d/exe", this->ID());
				char exe[256] = {};
				CL3_CLASS_SYSERR(readlink(symlink, exe, sizeof(exe)));
				return TString(exe);
			}

			const TList<const TString>& IProcess::Arguments() const
			{
				if(this->args == NULL)
				{
					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/cmdline", this->ID());

					auto args = MakeUniquePtr(new TList<const TString>());
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
				const auto p = str.Find("=");
				TString key = str.Left(p);
				TString value = str.Slice(p+1);
				return TPair<TString, TString>(key, value);
			}

			const io::collection::map::TStdMap<const TString, const TString>& IProcess::Environment() const
			{
				if(this->env == NULL)
				{
					char filename[32] = {};
					snprintf(filename, sizeof(filename), "/proc/%d/environ", this->ID());

					auto env = MakeUniquePtr(new io::collection::map::TStdMap<const TString, const TString>());
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
