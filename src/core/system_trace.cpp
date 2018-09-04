#include "system_trace.hpp"
#include "system_task.hpp"
#include "system_memory.hpp"
#include "io_network.hpp"

namespace cl3
{
	namespace system
	{
		namespace trace
		{
			using namespace task;
			using namespace memory;
			using namespace io::network;

			// TODO
// 			namespace
// 			{
// 				struct TAutoTrace
// 				{
// 					TUniquePtr<TTracer> tracer;
//
// 					TAutoTrace() : tracer(NULL)
// 					{
// 						auto& env = TSelfProcess::Self()->Environment();
// 						if(env.Contains("CL3_TRACE_IP"))
// 							this->tracer = MakeUniquePtr(new TTracer(TIPv4(env["CL3_TRACE_IP"]), 9913, TSelfProcess::Self()));
// 					}
// 				};
//
// 				static const TAutoTrace auto_trace_obj;
// 			}
		}
	}
}
