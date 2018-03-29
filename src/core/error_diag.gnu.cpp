#ifdef __GNUC__

#ifndef INSIDE_CL3CORE
#error "compiling cl3 source code but macro INSIDE_CL3CORE is not defined"
#endif

#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <cxxabi.h>

#include "error_diag.hpp"
#include "system_memory.hpp"

// int backtrace(void **buffer, int size);
// char **backtrace_symbols(void *const *buffer, int size);

namespace cl3
{
	namespace error
	{
		namespace diag
		{
			using namespace system::memory;
			using namespace io::text::string;
			using namespace io::text::encoding;
			using namespace io::collection::list;
			using namespace io::collection;

			TBacktrace GenerateBacktrace(usys_t n_frames_max)
			{
				TList<void*> frames;
				frames.Count(32);

				for(;;)
				{
					int n_frames;
					CL3_NONCLASS_LOGIC_ERROR((n_frames = ::backtrace(frames.ItemPtr(0), frames.Count())) < 0);
					if((unsigned)n_frames < frames.Count() || (unsigned)n_frames >= n_frames_max)
					{
						frames.Count(n_frames);
						break;
					}

					if(frames.Count() * 2 > n_frames_max)
						frames.Count(n_frames_max);
					else
						frames.Count(frames.Count() * 2);
				}

				auto symbols = MakeUniquePtr<UPTR_MALLOC, char*>(::backtrace_symbols(frames.ItemPtr(0), frames.Count()));

				TBacktrace bt;

				for(usys_t i = 0; i < frames.Count(); i++)
				{
					TCallsite cs;
					const TString symbol(symbols.Object()[i]);

					// ......../libcl3-core.so(_ZN3cl35error4diag17GenerateBacktraceEm+0xe5) [0x51a4903]
					// <full path to binary> "(" <mangled function name> "+" <offset hex> ")" <whitespace> "[" <address hex> "]"
					//                       ^0^                         ^1^              ^2^              ^3^               ^4^

					usys_t p[5];
					if((p[0] = symbol.Find("(", symbol.Count()-1, ESearchDirection::BACKWARD)) != (usys_t)-1)
					if((p[1] = symbol.Find("+", p[0]+1, ESearchDirection::FORWARD)) != (usys_t)-1)
					if((p[2] = symbol.Find(")", p[1]+1, ESearchDirection::FORWARD)) != (usys_t)-1)
					if((p[3] = symbol.Find("[", p[2]+1, ESearchDirection::FORWARD)) != (usys_t)-1)
					if((p[4] = symbol.Find("]", p[3]+1, ESearchDirection::FORWARD)) != (usys_t)-1)
					{
						cs.function.binary = symbol.Left(p[0]);
						cs.function.name_mangled = symbol.Slice(p[0]+1, p[1]-p[0]-1);

						int demangle_status = 0;
						auto demangled_name = MakeUniquePtr<UPTR_MALLOC, char>(::abi::__cxa_demangle(TCString(cs.function.name_mangled, CODEC_CXX_CHAR).Chars(), NULL, 0, &demangle_status));

						if(demangled_name.Object() != NULL && demangle_status == 0)
							cs.function.name = demangled_name.Object();

						symbol.Slice(p[1]+1, p[2]-p[1]-1) >> cs.offset;
						symbol.Slice(p[3]+1, p[4]-p[3]-1) >> cs.function.addr_start;
						cs.function.addr_start -= cs.offset;
						bt.callstack.Append(cs);
						continue;
					}

					cs.function.addr_start = 0;
					cs.offset = 0;

					bt.callstack.Append(cs);
				}

				return bt;
			}
		}
	}
}

#endif
