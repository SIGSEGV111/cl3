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
#include "system_task_synchronization.hpp"
#include "io_collection_list.hpp"
#include "io_stream_fd.hpp"
#include "io_text_string.hpp"
#include "system_time.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	system
	{
		namespace	task
		{
			class CL3PUBT IThread;
			class CL3PUBT TProcess;
			class CL3PUBT TProcessInfo;
			class CL3PUBT TCPU;
			class CL3PUBT TNumaNode;
			class CL3PUBT THost;
			class CL3PUBT TAffinity;

			#if (CL3_OS == CL3_OS_POSIX)
				typedef int pid_t;
				typedef int handle_t;
			#elif (CL3_OS == CL3_OS_WINDOWS)
				typedef int pid_t;
				typedef void* handle_t;
			#else
				#error "define pid_t for this OS"
			#endif

			enum	EState
			{
				STATE_DEAD,
				STATE_ALIVE_SUSPENDED,
				STATE_ALIVE_EXECUTING
			};

			class	CL3PUBT	TNumaNode : public io::collection::IStaticCollection<TCPU*>
			{
				public:
					CL3PUBF	CL3_GETTER	THost&	Host	() const;	//	returns the host machine to which this node belongs
			};

			class	CL3PUBT	TCPU
			{
				public:
					CL3PUBF	CL3_GETTER	TNumaNode&	NumaNode	() const;	//	returns the numa node on which this CPU is located
			};

			class	CL3PUBT	TProcessInfo
			{
				public:
					CL3PUBF	CL3_GETTER	pid_t		ID		() const;
					CL3PUBF	io::text::string::TString
													Name	() const CL3_GETTER;	//	the public visible name of the process (usually the name of its executable, or command-line)
					CL3PUBF CL3_GETTER	TProcess	Open	() const;	//	open the process for direct access
			};

			class	CL3PUBT	THost
			{
				public:
					CL3PUBF	usys_t	EnumProcesses	(io::collection::IDynamicCollection<TProcessInfo>&) const;	//	enumerates all processes on this host and adds them to the list; returns the number of processes added
			};

			class	CL3PUBT TProcess
			{
				protected:
					handle_t handle;
					io::collection::list::TList<IThread*> ls_threads;

				public:
					CL3PUBF	pid_t	Handle		() const CL3_GETTER;

					CL3PUBF	void	Name		(const io::text::string::TString& new_name) CL3_SETTER;
					CL3PUBF	io::text::string::TString
									Name		() const CL3_GETTER;

					//	commandline arguments (like argv[] in main())
					CL3PUBF	const io::collection::IStaticCollection<const io::text::string::TString>&
									CommandlineArguments	() const CL3_GETTER;

					//	environment varaibles (like envp[] in main())
					CL3PUBF	const io::collection::IStaticCollection<const io::text::string::TString>&
									Environment	() const CL3_GETTER;	//	TODO: use a map (KEY=VALUE) instead of a list

					CL3PUBF	const io::collection::list::TList<IThread*>&
									Threads		() const CL3_GETTER;

					CL3PUBF	void	Shutdown	();	//	requests the process to shut down
					CL3PUBF	void	Suspend		();	//	suspends the execution of the process (suspends all threads)
					CL3PUBF	void	Resume		();	//	resumes the execution of the process (resumes all threads)
					CL3PUBF	int		Wait		(time::TTime timeout = -1);	//	waits until the process has shut down (this will not send a shutdown request by its own)
					CL3PUBF	void	Kill		();	//	kills the process, unsaved data might get lost

					CL3PUBF	EState	State		() const CL3_GETTER;	//	returns the process' current state

					CL3PUBF	static	TProcess*	Self	() CL3_GETTER;

					CL3PUBF	CLASS	TProcess	(pid_t);
					CL3PUBF	CLASS	TProcess	(TProcess&&);
					CL3PUBF	CLASS	~TProcess	();
			};

			class	CL3PUBT	TProcessRunner : public virtual TProcess, public virtual io::stream::IIn<byte_t>, public virtual io::stream::IOut<byte_t>
			{
				private:
					TProcessRunner(const TProcessRunner&);	//	no copy constructor

				protected:
					io::stream::fd::TFDStream is;
					io::stream::fd::TFDStream os;

				public:
					//	from IIn<byte_t>
					using io::stream::IIn<byte_t>::Read;
					CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) CL3_WARN_UNUSED_RESULT;

					//	from IOut<byte_t>
					using io::stream::IOut<byte_t>::Write;
					CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;

					//	from TProcessRunner
					CL3PUBF	void	Start		(const io::text::string::TString& exe, const io::collection::IStaticCollection<const io::text::string::TString>& args = io::collection::list::TList<io::text::string::TString>());	//	starts the process

					CL3PUBF	void	CloseTX		();
					CL3PUBF	void	CloseRX		();

					CL3PUBF	CLASS	TProcessRunner	();
					CL3PUBF	CLASS	~TProcessRunner	();
			};

			class	CL3PUBT	IThread
			{
				public:
					CL3PUBF	static	IThread*	Self	() CL3_GETTER;	//	returns the IThread* for the calling thread

					CL3PUBF	io::text::string::TString
							Name	() const CL3_GETTER;	//	returns the name of the thread
					CL3PUBF	TProcess*	Process	() CL3_GETTER;			//	returns the process to which the thread belongs
					CL3PUBF	EState		State	() const CL3_GETTER;	//	returns the threads current state
					CL3PUBF	THost*		Host	() const CL3_GETTER;	//	returns the host machine this thread is executed on (possibly volatile in cluster systems)
					CL3PUBF	io::collection::IDynamicCollection<TCPU*>&	Affinity() CL3_GETTER;	//	returns the threads CPU affinity list

					CL3PUBF	void*	StackStart	() const CL3_GETTER;	//	start address of the execution stack
					CL3PUBF	void*	StackEnd	() const CL3_GETTER;	//	end address of the execution stack
					CL3PUBF	void*	StackCurrent() const CL3_GETTER;	//	current address of the stack pointer (NOTE: obviously highly volatile if the thread is not suspended)
					CL3PUBF	usys_t	StackSize	() const CL3_GETTER;	//	returns the size in bytes of the threads stacks
					CL3PUBF	usys_t	StackFree	() const CL3_GETTER;	//	returns the size in bytes of free (still unused / available) stack space (computed from StackCurrent() and StackEnd() - NOTE: see StackCurrent)
					CL3PUBF	usys_t	StackUsed	() const CL3_GETTER;	//	returns the size in bytes of used stack space (computed from Current() and Start())

					CL3PUBF	void	Start		(usys_t sz_stack = -1UL);	//	starts the thread if it is not alive yet, throws if it is already alive
					CL3PUBF	void	Shutdown	();	//	sets the "request shutdown" flag, throws if the thread is not alive
					CL3PUBF	void	Suspend		();	//	suspends the execution of the thread, throws if the thread is already suspended or not alive at all
					CL3PUBF	void	Resume		();	//	resumes the execution of a suspended thread, throws if the thread is not suspended or not alive at all

				protected:
					CL3PUBF	void	Name		(const io::text::string::TString&) CL3_SETTER;
					virtual	void	ThreadMain	() = 0;
			};
		}
	}
}

#endif
