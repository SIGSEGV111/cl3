#pragma once

#include <cl3/core/io_stream.hpp>
#include <cl3/core/io_collection_array.hpp>
#include <cl3/core/io_file.hpp>

namespace	cl3
{
	namespace	unittest_support
	{
		using namespace cl3::io::stream;
		using namespace cl3::io::collection::array;
		using namespace cl3::io::file;

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
	}
}
