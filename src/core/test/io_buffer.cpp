#include "common.hpp"
#include <cl3/core/io_buffer.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::io::buffer;
	using namespace cl3::io::stream;
	using namespace cl3::io::collection::list;
	using namespace cl3::unittest_support;

	TEST(io_buffer_TFifo, Basic_Read_Write)
	{
		TFifo<char> fifo(17);
		char buffer[32];

		EXPECT_EQ(0U, fifo.Remaining());
		EXPECT_EQ(17U-1U, fifo.Space());
		fifo.Write("test", 4);
		EXPECT_EQ(4U, fifo.Remaining());
		EXPECT_EQ(17U-4U-1U, fifo.Space());
		fifo.Read(buffer, 4);
		EXPECT_TRUE(memcmp(buffer, "test", 4) == 0);

		EXPECT_EQ(0U, fifo.Remaining());
		EXPECT_EQ(17U-1U, fifo.Space());
		fifo.Write("test", 4);
		fifo.Write("hello", 5);
		fifo.Write("world", 5);
		EXPECT_EQ(14U, fifo.Remaining());
		EXPECT_EQ(17U-14U-1U, fifo.Space());
		EXPECT_TRUE(fifo.Read(buffer, 32, 0) == 14);
		EXPECT_TRUE(memcmp(buffer, "testhelloworld", 14) == 0);

		EXPECT_EQ(0U, fifo.Remaining());
		EXPECT_EQ(17U-1U, fifo.Space());
		fifo.Write("aaaa", 4);
		fifo.Write("bbbb", 4);
		fifo.Write("cccc", 4);
		EXPECT_EQ(12U, fifo.Remaining());
		EXPECT_EQ(17U-12U-1U, fifo.Space());
		EXPECT_THROW(fifo.Read(buffer, 13), TSourceDryException);
		EXPECT_EQ(12U, fifo.Remaining());
		EXPECT_EQ(17U-12U-1U, fifo.Space());

		fifo.Read(buffer, 11);
		EXPECT_EQ(1U, fifo.Remaining());
		EXPECT_EQ(17U-1U-1U, fifo.Space());
		EXPECT_TRUE(memcmp(buffer, "aaaabbbbccc", 11) == 0);

		fifo.Read(buffer, 1);
		EXPECT_TRUE(memcmp(buffer, "c", 1) == 0);
		EXPECT_EQ(0U, fifo.Remaining());
		EXPECT_EQ(17U-1U, fifo.Space());

		fifo.Write("aaaa", 4);
		fifo.Write("bbbb", 4);
		fifo.Write("cccc", 4);
		fifo.Write("dddd", 4);
		EXPECT_EQ(16U, fifo.Remaining());
		EXPECT_EQ(0U, fifo.Space());
		EXPECT_EQ(0U, fifo.Write("eeee", 4, 0));
		EXPECT_THROW(fifo.Write("d", 1), TSinkFloodedException);
	}

	TEST(io_buffer_TPump, BufferToBuffer)
	{
		const char* s = "hello world foobar test 123";

		TList<char> l1;
		TList<char> l2;

		l1.Append(s, 27);

		TPump<char> pump(&l1, &l2);

		EXPECT_EQ(0U, l1.Count());
		EXPECT_EQ(27U, l2.Count());
		EXPECT_TRUE(memcmp(l2.ItemPtr(0), s, 27) == 0);
		EXPECT_TRUE(pump.HasFinished());
	}

	TEST(io_buffer_TPump, BlockingSink)
	{
		const char* s = "hello world foobar test 123";

		TList<char> l1;
		l1.Append(s, 27);

		TBlockingSink<char> bs;
		bs.n_space_report = (usys_t)-1;
		bs.n_space_noblock = 5;
		bs.n_space_flooded = (usys_t)-1;
		bs.n_items_accepted_total = 0;

		TPump<char> pump(&l1, &bs);

		EXPECT_EQ(5U, bs.n_items_accepted_total);
		EXPECT_FALSE(pump.HasFinished());

		bs.n_space_noblock = 5;
		pump.Continue();

		EXPECT_EQ(10U, bs.n_items_accepted_total);
		EXPECT_FALSE(pump.HasFinished());

		bs.n_space_noblock = 20;
		pump.Continue();

		EXPECT_EQ(27U, bs.n_items_accepted_total);
		EXPECT_TRUE(pump.HasFinished());
	}

	// FIXME: needs exception support in IFiber
// 	TEST(io_buffer_TPump, MissbehavingSink)
// 	{
// 		const char* s = "hello world foobar test 123";
//
// 		TList<char> l1;
// 		l1.Append(s, 27);
//
// 		TBlockingSink<char> bs;
// 		bs.n_space_report = (usys_t)-1;
// 		bs.n_space_noblock = 5;
// 		bs.n_space_flooded = 10;
// 		bs.n_items_accepted_total = 0;
//
// 		TPump<char> pump(&l1, &bs);
//
// 		EXPECT_EQ(5, bs.n_items_accepted_total);
// 		EXPECT_FALSE(pump.HasFinished());
//
// 		bs.n_space_noblock = 5;
// 		pump.Continue();
//
// 		EXPECT_EQ(10, bs.n_items_accepted_total);
// 		EXPECT_FALSE(pump.HasFinished());
//
// 		bs.n_space_noblock = 20;
// 		pump.Continue();
//
// 		EXPECT_EQ(10, bs.n_items_accepted_total);
// 		EXPECT_TRUE(pump.HasFinished());
// 	}
}
