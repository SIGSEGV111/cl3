#ifndef	_include_cl3_core_system_task_hpp_
#define	_include_cl3_core_system_task_hpp_

#include "system_types.hpp"

namespace cl3
{
	namespace io
	{
		namespace stream
		{
			template<typename T>
			struct IIn;

			template<typename T>
			struct IOut;
		}
	}

	namespace system
	{
		namespace task
		{
			using namespace types;

			class TSelfProcess
			{
				public:
					CL3PUBF static io::stream::IIn<byte_t>&  StdIn() CL3_GETTER;
					CL3PUBF static io::stream::IOut<byte_t>& StdOut() CL3_GETTER;
					CL3PUBF static io::stream::IOut<byte_t>& StdErr() CL3_GETTER;
			};
		}
	}
}

#endif
