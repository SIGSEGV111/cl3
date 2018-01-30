#pragma once

#include "io_text_string.hpp"
#include "io_collection_list.hpp"

namespace cl3
{
	namespace error
	{
		namespace diag
		{
			struct TFunction
			{
				usys_t addr_start;
				io::text::string::TString name_mangled;
				io::text::string::TString name;
				io::text::string::TString binary;

				inline bool operator==(const TFunction& rhs) const { return this->addr_start == rhs.addr_start; }
			};

			struct TCallsite
			{
				TFunction function;
				usys_t offset;

				inline bool operator==(const TCallsite& rhs) const { return this->function == rhs.function && this->offset == rhs.offset; }
			};

			struct TBacktrace
			{
				io::collection::list::TList<TCallsite> callstack;
			};

			CL3PUBF TBacktrace GenerateBacktrace(usys_t n_frames_max = (usys_t)-1);
		}
	}
}
