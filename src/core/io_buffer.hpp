#pragma once

#include "error.hpp"
#include "system_memory.hpp"
#include "system_task.hpp"
#include "io_stream.hpp"
#include "context.hpp"

namespace cl3
{
	namespace io
	{
		namespace buffer
		{
			using namespace system::types;

			template<typename T>
			class TFifo : public stream::IIn<T>, public stream::IOut<T>
			{
				protected:
					T* arr_items;
					const usys_t n_items;	// total size of fifo
					usys_t p_read;	// next item to be read
					usys_t p_write;	// next item to be written

				public:
					using stream::IOut<T>::Write;
					using stream::IIn<T>::Read;

					usys_t Remaining() const CL3_GETTER
					{
						return (p_read <= p_write) ? (p_write - p_read) : (n_items - p_read + p_write);
					}

					usys_t Read(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) CL3_WARN_UNUSED_RESULT
					{
						if(n_items_read_min == (usys_t)-1)
							n_items_read_min = n_items_read_max; // LCOV_EXCL_LINE
						CL3_CLASS_LOGIC_ERROR(n_items_read_min > this->n_items - 1);
						CL3_CLASS_ERROR(n_items_read_min > this->Remaining(), stream::TSourceDryException, n_items_read_max, n_items_read_min, 0, this->Remaining());

						const usys_t n_items_copy_total = CL3_MIN(n_items_read_max, this->Remaining());
						const usys_t n_items_max_run1 = (this->p_write < this->p_read) ? (this->n_items - this->p_read) : (p_write - p_read);

						const usys_t n_items_run1 = CL3_MIN(n_items_max_run1, n_items_copy_total);
						const usys_t n_items_run2 = n_items_copy_total - n_items_run1;

						for(usys_t i = 0; i < n_items_run1; i++)
							arr_items_read[i] = this->arr_items[this->p_read + i];

						for(usys_t i = 0; i < n_items_run2; i++)
							arr_items_read[n_items_run1 + i] = this->arr_items[i];

						this->p_read += n_items_copy_total;
						this->p_read %= this->n_items;

						return n_items_copy_total;
					}

					usys_t Space() const CL3_GETTER
					{
						return (p_read > p_write) ? (p_read - p_write - 1) : (n_items - p_write + p_read - 1);
					}

					usys_t Write(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT
					{
						if(n_items_write_min == (usys_t)-1)
							n_items_write_min = n_items_write_max; // LCOV_EXCL_LINE
						CL3_CLASS_LOGIC_ERROR(n_items_write_min > this->n_items - 1);
						CL3_CLASS_ERROR(n_items_write_min > this->Space(), stream::TSinkFloodedException, n_items_write_max, n_items_write_min, 0, this->Space());

						const usys_t n_items_copy_total = CL3_MIN(n_items_write_max, this->Space());
						const usys_t n_items_max_run1 = (this->p_read <= this->p_write) ? (this->n_items - this->p_write) : (this->p_read - this->p_write - 1);

						const usys_t n_items_run1 = CL3_MIN(n_items_max_run1, n_items_copy_total);
						const usys_t n_items_run2 = n_items_copy_total - n_items_run1;

						for(usys_t i = 0; i < n_items_run1; i++)
							this->arr_items[this->p_write + i] = arr_items_write[i];

						for(usys_t i = 0; i < n_items_run2; i++)
							this->arr_items[i] = arr_items_write[n_items_run1 + i];

						this->p_write += n_items_copy_total;
						this->p_write %= this->n_items;

						return n_items_copy_total;
					}

					usys_t Size() const CL3_GETTER { return this->n_items; }

					CLASS TFifo(usys_t n_items) : arr_items(new T[n_items]()), n_items(n_items), p_read(0), p_write(0)
					{
					}

					CLASS TFifo(const TFifo&) = delete;

					CLASS TFifo(TFifo&& other) : arr_items(other.arr_items), n_items(other.n_items), p_read(other.p_read), p_write(other.p_write)
					{
						other.arr_items = NULL;
					}

					CLASS ~TFifo()
					{
						delete[](this->arr_items);
					}
			};
		}
	}
}
