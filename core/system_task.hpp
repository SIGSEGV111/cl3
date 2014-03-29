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
#include "io_collection.hpp"

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
				typedef int TPID;
			#elif (CL3_OS == CL3_OS_WINDOWS)
				typedef u32_t TPID;
			#else
				#error "define TPID for this OS"
			#endif

			class	TNumaNode : public io::collection::IStaticCollection<TCPU*>
			{
				public:
					CL3PUBF	GETTER	THost&	Host	() const;	//	returns the host machine to which this node belongs
			};

			class	TCPU
			{
				public:
					CL3PUBF	GETTER	TNumaNode&	NumaNode	() const;	//	returns the numa node on which this CPU is located
			};

			class	TProcessInfo
			{
				public:
					CL3PUBF	GETTER	TPID		ID		() const;
					CL3PUBF GETTER	TProcess*	Open	() const;	//	open the process for direct access
			};

			class	THost
			{
				public:
					CL3PUBF	usys_t	EnumProcesses	(io::collection::IDynamicCollection<TProcessInfo>&) const;	//	enumerates all processes on this host and adds them to the list; returns the number of processes added
			};

			class	TProcess : public TProcessInfo
			{
				public:
					CL3PUBF	GETTER	const char*	Name	() const;	//	name of the process
					CL3PUBF	SETTER	void		Name	(const char* new_name);
					CL3PUBF	GETTER	const io::collection::IDynamicCollection<const char*>&	CommandlineArguments	() const;	//	commandline arguments (like argv[] in main())
					CL3PUBF	GETTER	const io::collection::IDynamicCollection<IThread*>&		Threads	() const;	//	list of threads that belong to this process. for the calling threads process all registered threads are listed (even dead ones), but for other processed only those the operating system knows about are listed (usually only those that are alive)
			};

			class	IThread
			{
				public:
					enum	EState
					{
						STATE_DEAD,
						STATE_ALIVE_SUSPENDED,
						STATE_ALIVE_EXECUTING
					};

					CL3PUBF	static	IThread&	Self	() GETTER { CL3_NOT_IMPLEMENTED; }	//	returns the IThread& for the calling thread

					CL3PUBF	const char*	Name	() const GETTER;	//	returns the name of the thread
					CL3PUBF	TProcess&	Process	() GETTER;	//	returns the process to which the thread belongs
					CL3PUBF	EState		State	() const GETTER;	//	returns the threads current state
					CL3PUBF	THost&		Host	() const GETTER;	//	returns the host machine this thread is executed on (possibly volatile in cluster systems)
					CL3PUBF	io::collection::IDynamicCollection<TCPU*>&	Affinity() GETTER;	//	returns the threads CPU affinity list

					CL3PUBF	void*	StackStart	() const GETTER;	//	start address of the execution stack - might be less than end (stack grows down)
					CL3PUBF	void*	StackEnd	() const GETTER;	//	end address of the execution stack - might be bigger than start (stack grows up)
					CL3PUBF	void*	StackCurrent() const GETTER;	//	current address of the stack pointer (NOTE: obviously highly volatile if the thread is not suspended)
					CL3PUBF	usys_t	StackSize	() const GETTER;	//	returns the size in byte_ts of the stacks size
					CL3PUBF	usys_t	StackFree	() const GETTER;	//	returns the size in byte_ts of free (still unused / available) stack space (computed from Current() and End())
					CL3PUBF	usys_t	StackUsed	() const GETTER;	//	returns the size in byte_ts of used stack space (computed from Current() and Start())

					CL3PUBF	void	Start		(usys_t sz_stack = -1UL);	//	starts the thread if it is not alive yet, throws if it is already alive
					CL3PUBF	void	Shutdown	();	//	sets the "request shutdown" flag, throws if the thread is not alive
					CL3PUBF	void	Suspend		();	//	suspends the execution of the thread, throws if the thread is already suspended or not alive at all
					CL3PUBF	void	Resume		();	//	resumes the execution of a suspended thread, throws if the thread is not suspended or not alive at all

				protected:
					CL3PUBF	void	Name		(const char* new_name) SETTER;
					virtual	void	ThreadMain	() = 0;
			};
		}
	}
}

#endif
