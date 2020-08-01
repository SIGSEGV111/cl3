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

		enum class EState : u8_t
		{
			UNDEFINED,
			DEFAULT_CONTRUCTED,
			COPY_CONSTRUCTED,
			MOVE_CONSTRUCTED,
			COPY_ASSIGNED,
			MOVE_ASSIGNED,
			DESTRUCTED
		};

		struct TStateTracker
		{
			static EState last;

			EState* my_state;

			static void Reset()
			{
				last = EState::UNDEFINED;
			}

			void SetTracking(EState* p)
			{
				my_state = p;
				if(my_state) *my_state = last;
			}

			TStateTracker& operator=(const TStateTracker&)
			{
				last = EState::COPY_ASSIGNED;
				if(my_state) *my_state = last;
				return *this;
			}

			TStateTracker& operator=(TStateTracker&&)
			{
				last = EState::MOVE_ASSIGNED;
				if(my_state) *my_state = last;
				return *this;
			}

			TStateTracker() : my_state(nullptr)
			{
				last = EState::DEFAULT_CONTRUCTED;
				if(my_state) *my_state = last;
			}

			TStateTracker(const TStateTracker&)
			{
				last = EState::COPY_CONSTRUCTED;
				if(my_state) *my_state = last;
			}

			TStateTracker(TStateTracker&&)
			{
				last = EState::MOVE_CONSTRUCTED;
				if(my_state) *my_state = last;
			}

			~TStateTracker()
			{
				last = EState::DESTRUCTED;
				if(my_state) *my_state = last;
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
