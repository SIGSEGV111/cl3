#pragma once

#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection_array.hpp>
#include <cl3/core/io_file.hpp>
#include <cl3/core/system_task.hpp>

namespace	cl3
{
	namespace	unittest_support
	{
		using namespace cl3::io::stream;
		using namespace cl3::io::collection::array;
		using namespace cl3::io::file;

		struct	TTestException {};

		template<class T>
		struct	TLimitedBuffer : virtual IOut<T>, virtual IIn<T>
		{
			TArray<T> array;
			usys_t index;

			CLASS	TLimitedBuffer	(usys_t n_items) : array(n_items), index(0) {}
			CLASS	~TLimitedBuffer	() {}
			TLimitedBuffer(const TLimitedBuffer&) = delete;
			TLimitedBuffer&	operator=(const TLimitedBuffer&) = delete;

			usys_t	Read	(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT
			{
				const usys_t r = array.Read(index, arr_items_read, n_items_read_max, n_items_read_min);
				index += r;
				return r;
			}

			usys_t	Write	(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT
			{
				const usys_t r = array.Write(index, arr_items_write, n_items_write_max, n_items_write_min);
				index += r;
				return r;
			}

			usys_t	Remaining	() const final override CL3_GETTER
			{
				return array.Count() - index;
			}

			usys_t	Space		() const final override CL3_GETTER
			{
				return array.Count() - index;
			}
		};

		static TDirectoryBrowser TestDataDirectory()
		{
			TDirectoryBrowser b;

			while(!b.IsRoot() && !b.Entries().Contains("data"))
				b.EnterDirectory("..");

			b.EnterDirectory("data/tests");

			return b;
		}

		template<typename T>
		struct TBlockingSink : public IOut<T>
		{
			usys_t n_space_report;
			usys_t n_space_noblock;
			usys_t n_space_flooded;
			usys_t n_items_accepted_total;

			usys_t Space() const CL3_GETTER
			{
				return n_space_report;
			}

			usys_t Write(const T*, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT
			{
				if(n_items_write_min == (usys_t)-1)
					n_items_write_min = n_items_write_max;

				CL3_CLASS_ERROR(n_items_write_min > n_space_flooded, TSinkFloodedException, n_items_write_max, n_items_write_min, 0, n_space_flooded);

				usys_t n_items_accepted = 0;

				while(n_items_accepted < n_items_write_min)
				{
					const usys_t n_items_remain = n_items_write_max - n_items_accepted;

					while(n_space_noblock == 0)
						cl3::system::task::IFiber::Return();

					const usys_t n_items_accept = CL3_MIN(n_space_noblock, n_items_remain);
					n_items_accepted += n_items_accept;
					n_items_accepted_total += n_items_accept;

					if(n_space_report != (usys_t)-1)
						n_space_report -= CL3_MIN(n_items_accept, n_space_report);
					if(n_space_noblock != (usys_t)-1)
						n_space_noblock -= CL3_MIN(n_items_accept, n_space_noblock);
					if(n_space_flooded != (usys_t)-1)
						n_space_flooded -= CL3_MIN(n_items_accept, n_space_flooded);
				}

				return n_items_accepted;
			}
		};
	}
}
