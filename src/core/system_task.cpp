#include "system_task.hpp"
#include "io_stream.hpp"
#include "io_stream_fd.hpp"

namespace cl3
{
	namespace system
	{
		namespace task
		{
			using namespace io::stream;
			using namespace io::stream::fd;

			static TFDStream stdin(0);
			static TFDStream stdout(1);
			static TFDStream stderr(2);

			IIn<byte_t>&  TSelfProcess::StdIn()
			{
				return stdin;
			}

			IOut<byte_t>& TSelfProcess::StdOut()
			{
				return stdout;
			}

			IOut<byte_t>& TSelfProcess::StdErr()
			{
				return stderr;
			}
		}
	}
}
