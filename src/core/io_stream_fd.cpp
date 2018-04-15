#include <cl3/core/io_stream_fd.hpp>

namespace cl3
{
	namespace io
	{
		namespace stream
		{
			namespace fd
			{
				usys_t TPipe::Read(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					return this->fd[0].Read(arr_items_read, n_items_read_max, n_items_read_min);
				}

				usys_t TPipe::Write(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					return this->fd[1].Write(arr_items_write, n_items_write_max, n_items_write_min);
				}

				usys_t TPipe::Remaining() const
				{
					return this->fd[0].Remaining();
				}

				CLASS TPipe::~TPipe()
				{
					// nothing to do
				}
			}
		}
	}
}
