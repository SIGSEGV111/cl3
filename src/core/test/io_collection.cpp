/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.hpp"
#include <gtest/gtest.h>
#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_collection_bitmask.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text_string.hpp>

using namespace ::testing;

namespace
{
	using namespace cl3::io::collection::bitmask;

	TEST(io_colection_bitmask_TBitmask, Set_Get_Bits)
	{
		TBitmask bm;
		bm.Count(256);

		bm.Bit(0, true);
		bm.Bit(1, false);
		bm.Bit(128, true);
		bm.Bit(255, true);

		EXPECT_TRUE(bm.Bit(0));
		EXPECT_TRUE(!bm.Bit(1));
		EXPECT_TRUE(bm.Bit(0));
		EXPECT_TRUE(bm.Bit(128));
		EXPECT_TRUE(bm.Bit(255));

		bm.Bit(0, false);
		bm.Bit(128, false);
		bm.Bit(255, false);

		EXPECT_TRUE(!bm.Bit(0));
		EXPECT_TRUE(!bm.Bit(1));
		EXPECT_TRUE(!bm.Bit(128));
		EXPECT_TRUE(!bm.Bit(255));

		bm.Bit(0, false);
		bm.Bit(128, true);
		bm.Bit(255, false);

		EXPECT_TRUE(!bm.Bit(0));
		EXPECT_TRUE(!bm.Bit(1));
		EXPECT_TRUE(bm.Bit(128));
		EXPECT_TRUE(!bm.Bit(255));
	}

	TEST(io_colection_bitmask_TBitmask, Realloc)
	{
		TBitmask bm;
		bm.Count(256);

		bm.Bit(0, true);
		bm.Bit(128, true);
		bm.Bit(255, true);

		EXPECT_TRUE(bm.Bit(0));
		EXPECT_TRUE(bm.Bit(128));
		EXPECT_TRUE(bm.Bit(255));

		bm.Count(512);

		EXPECT_TRUE(bm.Bit(0));
		EXPECT_TRUE(bm.Bit(128));
		EXPECT_TRUE(bm.Bit(255));

		bm.Bit(509, true);
		bm.Bit(510, false);
		bm.Bit(511, true);

		EXPECT_TRUE(bm.Bit(509));
		EXPECT_TRUE(!bm.Bit(510));
		EXPECT_TRUE(bm.Bit(511));
	}
}

namespace
{
	using namespace cl3::io::collection;
	using namespace cl3::io::collection::list;
	using namespace cl3::system::types;
	using namespace cl3::unittest_support;

	TEST(io_collection_list_TList, Construct)
	{
		const int arr_ints[] = { 0, 1, 2, 3, 4, 5, 6 };
		const usys_t n_ints = sizeof(arr_ints) / sizeof(int);	//	must be 7
		EXPECT_TRUE(n_ints == 7);

		{
			TList<int> list(arr_ints, n_ints);
			EXPECT_TRUE(list.Count() == 7);
			EXPECT_TRUE(list[0] == 0);
			EXPECT_TRUE(list[1] == 1);
			EXPECT_TRUE(list[2] == 2);
			EXPECT_TRUE(list[3] == 3);
			EXPECT_TRUE(list[4] == 4);
			EXPECT_TRUE(list[5] == 5);
			EXPECT_TRUE(list[6] == 6);
		}

		{
			int* arr_ints_claimable = (int*)cl3::system::memory::Alloc(n_ints * sizeof(int), NULL);
			memcpy(arr_ints_claimable, arr_ints, n_ints * sizeof(int));

			TList<int> list(arr_ints_claimable, n_ints, true);
			EXPECT_TRUE(list.Count() == 7);
			EXPECT_TRUE(list[0] == 0);
			EXPECT_TRUE(list[1] == 1);
			EXPECT_TRUE(list[2] == 2);
			EXPECT_TRUE(list[3] == 3);
			EXPECT_TRUE(list[4] == 4);
			EXPECT_TRUE(list[5] == 5);
			EXPECT_TRUE(list[6] == 6);

			arr_ints_claimable[0] = 100;
			arr_ints_claimable[1] = 101;
			arr_ints_claimable[2] = 102;
			arr_ints_claimable[3] = 103;
			arr_ints_claimable[4] = 104;
			arr_ints_claimable[5] = 105;
			arr_ints_claimable[6] = 106;

			EXPECT_TRUE(list[0] == 100);
			EXPECT_TRUE(list[1] == 101);
			EXPECT_TRUE(list[2] == 102);
			EXPECT_TRUE(list[3] == 103);
			EXPECT_TRUE(list[4] == 104);
			EXPECT_TRUE(list[5] == 105);
			EXPECT_TRUE(list[6] == 106);
		}

		{
			int* arr_ints_claimable = (int*)cl3::system::memory::Alloc(n_ints * sizeof(int), NULL);
			memcpy(arr_ints_claimable, arr_ints, n_ints * sizeof(int));

			TList<int> list(arr_ints_claimable, n_ints, false);
			EXPECT_TRUE(list.Count() == 7);
			EXPECT_TRUE(list[0] == 0);
			EXPECT_TRUE(list[1] == 1);
			EXPECT_TRUE(list[2] == 2);
			EXPECT_TRUE(list[3] == 3);
			EXPECT_TRUE(list[4] == 4);
			EXPECT_TRUE(list[5] == 5);
			EXPECT_TRUE(list[6] == 6);

			arr_ints_claimable[0] = 100;
			arr_ints_claimable[1] = 101;
			arr_ints_claimable[2] = 102;
			arr_ints_claimable[3] = 103;
			arr_ints_claimable[4] = 104;
			arr_ints_claimable[5] = 105;
			arr_ints_claimable[6] = 106;

			EXPECT_TRUE(list[0] == 0);
			EXPECT_TRUE(list[1] == 1);
			EXPECT_TRUE(list[2] == 2);
			EXPECT_TRUE(list[3] == 3);
			EXPECT_TRUE(list[4] == 4);
			EXPECT_TRUE(list[5] == 5);
			EXPECT_TRUE(list[6] == 6);

			cl3::system::memory::Free(arr_ints_claimable);
		}

		{
			TList<const int> list;
			EXPECT_TRUE(list.Count() == 0);
		}
	}

	TEST(io_collection_list_TList, Add_Remove_Clear)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);
		list.Add(5);
		list.Add(6);
		list.Add(7);
		list.Add(8);
		list.Add(9);

		EXPECT_TRUE(list.Count() == 10);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);
		EXPECT_TRUE(list[5] == 5);
		EXPECT_TRUE(list[6] == 6);
		EXPECT_TRUE(list[7] == 7);
		EXPECT_TRUE(list[8] == 8);
		EXPECT_TRUE(list[9] == 9);

		list.Remove(5,3);	//	removes 5,6,7

		EXPECT_TRUE(list.Count() == 7);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);
		EXPECT_TRUE(list[5] == 8);
		EXPECT_TRUE(list[6] == 9);

		list.Remove(0,1);	//	removes 0

		EXPECT_TRUE(list.Count() == 6);

		EXPECT_TRUE(list[0] == 1);
		EXPECT_TRUE(list[1] == 2);
		EXPECT_TRUE(list[2] == 3);
		EXPECT_TRUE(list[3] == 4);
		EXPECT_TRUE(list[4] == 8);
		EXPECT_TRUE(list[5] == 9);

		list.Remove(5,1);	//	removes 9

		EXPECT_TRUE(list.Count() == 5);

		EXPECT_TRUE(list[0] == 1);
		EXPECT_TRUE(list[1] == 2);
		EXPECT_TRUE(list[2] == 3);
		EXPECT_TRUE(list[3] == 4);
		EXPECT_TRUE(list[4] == 8);

		list.Add(10);
		list.Add(11);

		EXPECT_TRUE(list.Count() == 7);

		EXPECT_TRUE(list[0] == 1);
		EXPECT_TRUE(list[1] == 2);
		EXPECT_TRUE(list[2] == 3);
		EXPECT_TRUE(list[3] == 4);
		EXPECT_TRUE(list[4] == 8);
		EXPECT_TRUE(list[5] == 10);
		EXPECT_TRUE(list[6] == 11);

		const int arr_ints[] = { 20, 21, 22 };
		list.Add(arr_ints, sizeof(arr_ints) / sizeof(int));

		EXPECT_TRUE(list.Count() == 10);

		EXPECT_TRUE(list[0] == 1);
		EXPECT_TRUE(list[1] == 2);
		EXPECT_TRUE(list[2] == 3);
		EXPECT_TRUE(list[3] == 4);
		EXPECT_TRUE(list[4] == 8);
		EXPECT_TRUE(list[5] == 10);
		EXPECT_TRUE(list[6] == 11);
		EXPECT_TRUE(list[7] == 20);
		EXPECT_TRUE(list[8] == 21);
		EXPECT_TRUE(list[9] == 22);

		list.Clear();

		EXPECT_TRUE(list.Count() == 0);
	}

	TEST(io_collection_list_TList, Insert_Remove_Clear)
	{
		TList<int> list;

		EXPECT_TRUE(list.Count() == 0);

		list.Insert(0, 0);
		EXPECT_TRUE(list.Count() == 1);
		EXPECT_TRUE(list[0] == 0);

		list.Insert(1, 1);
		EXPECT_TRUE(list.Count() == 2);
		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);

		list.Insert(1, 2);
		EXPECT_TRUE(list.Count() == 3);
		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 2);
		EXPECT_TRUE(list[2] == 1);

		list.Insert(0, 3);
		EXPECT_TRUE(list.Count() == 4);
		EXPECT_TRUE(list[0] == 3);
		EXPECT_TRUE(list[1] == 0);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 1);

		list.Insert(4, 5);
		EXPECT_TRUE(list.Count() == 5);
		EXPECT_TRUE(list[0] == 3);
		EXPECT_TRUE(list[1] == 0);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 1);
		EXPECT_TRUE(list[4] == 5);

		list.Remove(4, 1);
		EXPECT_TRUE(list.Count() == 4);
		EXPECT_TRUE(list[0] == 3);
		EXPECT_TRUE(list[1] == 0);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 1);

		list.Remove(3, 1);
		EXPECT_TRUE(list.Count() == 3);
		EXPECT_TRUE(list[0] == 3);
		EXPECT_TRUE(list[1] == 0);
		EXPECT_TRUE(list[2] == 2);

		list.Remove(1, 1);
		EXPECT_TRUE(list.Count() == 2);
		EXPECT_TRUE(list[0] == 3);
		EXPECT_TRUE(list[1] == 2);

		list.Remove(0, 1);
		EXPECT_TRUE(list.Count() == 1);
		EXPECT_TRUE(list[0] == 2);

		list.Remove(0, 1);
		EXPECT_TRUE(list.Count() == 0);

		list.Clear();
	}

	TEST(io_collection_list_TList, Copy)
	{
		TList<int> source_list;

		source_list.Add(0);
		source_list.Add(1);
		source_list.Add(2);
		source_list.Add(3);
		source_list.Add(4);
		source_list.Add(5);
		source_list.Add(6);
		source_list.Add(7);
		source_list.Add(8);
		source_list.Add(9);

		TList<int> dest_list(source_list);

		EXPECT_TRUE(dest_list.Count() == 10);

		EXPECT_TRUE(dest_list[0] == 0);
		EXPECT_TRUE(dest_list[1] == 1);
		EXPECT_TRUE(dest_list[2] == 2);
		EXPECT_TRUE(dest_list[3] == 3);
		EXPECT_TRUE(dest_list[4] == 4);
		EXPECT_TRUE(dest_list[5] == 5);
		EXPECT_TRUE(dest_list[6] == 6);
		EXPECT_TRUE(dest_list[7] == 7);
		EXPECT_TRUE(dest_list[8] == 8);
		EXPECT_TRUE(dest_list[9] == 9);
	}

	TEST(io_collection_list_TList, Prepend)
	{
		TList<int> list;

		for(int i = 0; i < 100; i++)
			list.Prepend(i);

		for(int i = 0; i < 100; i++)
			EXPECT_EQ(99-i, list[i]);
	}

	TEST(io_collection_list_TList, Assign)
	{
		TList<int> source_list;

		source_list.Add(0);
		source_list.Add(1);
		source_list.Add(2);
		source_list.Add(3);
		source_list.Add(4);
		source_list.Add(5);
		source_list.Add(6);
		source_list.Add(7);
		source_list.Add(8);
		source_list.Add(9);

		TList<int> dest_list;

		dest_list.Add(100);
		dest_list.Add(101);
		dest_list.Add(102);
		dest_list.Add(103);
		dest_list.Add(104);
		dest_list.Add(105);

		EXPECT_TRUE(dest_list.Count() == 6);

		EXPECT_TRUE(dest_list[0] == 100);
		EXPECT_TRUE(dest_list[1] == 101);
		EXPECT_TRUE(dest_list[2] == 102);
		EXPECT_TRUE(dest_list[3] == 103);
		EXPECT_TRUE(dest_list[4] == 104);
		EXPECT_TRUE(dest_list[5] == 105);

		dest_list = source_list;

		EXPECT_TRUE(dest_list.Count() == 10);

		EXPECT_TRUE(dest_list[0] == 0);
		EXPECT_TRUE(dest_list[1] == 1);
		EXPECT_TRUE(dest_list[2] == 2);
		EXPECT_TRUE(dest_list[3] == 3);
		EXPECT_TRUE(dest_list[4] == 4);
		EXPECT_TRUE(dest_list[5] == 5);
		EXPECT_TRUE(dest_list[6] == 6);
		EXPECT_TRUE(dest_list[7] == 7);
		EXPECT_TRUE(dest_list[8] == 8);
		EXPECT_TRUE(dest_list[9] == 9);
	}

	TEST(io_collection_list_TList, Grow)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);

		EXPECT_TRUE(list.Count() == 5);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);

		list.Grow(5, 100);

		EXPECT_TRUE(list.Count() == 10);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);
		EXPECT_TRUE(list[5] == 100);
		EXPECT_TRUE(list[6] == 100);
		EXPECT_TRUE(list[7] == 100);
		EXPECT_TRUE(list[8] == 100);
		EXPECT_TRUE(list[9] == 100);
	}

	TEST(io_collection_list_TList, Shrink)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);

		EXPECT_TRUE(list.Count() == 5);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);

		list.Shrink(2);

		EXPECT_TRUE(list.Count() == 3);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
	}

	TEST(io_collection_list_TList, ItemPtr)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);

		EXPECT_TRUE(list.Count() == 5);

		int* base = list.ItemPtr(0);

		EXPECT_TRUE(list.ItemPtr(1) == &list[1]);
		EXPECT_TRUE(list.ItemPtr(2) == &list[2]);
		EXPECT_TRUE(list.ItemPtr(3) == &list[3]);
		EXPECT_TRUE(list.ItemPtr(4) == &list[4]);

		EXPECT_TRUE(list.ItemPtr(1) == base+1);
		EXPECT_TRUE(list.ItemPtr(2) == base+2);
		EXPECT_TRUE(list.ItemPtr(3) == base+3);
		EXPECT_TRUE(list.ItemPtr(4) == base+4);
	}

	TEST(io_collection_list_TList, fifo)
	{
		int buffer[8];
		const int arr_ints[] = { 0, 1, 2, 3, 4, 5, 6 };
		const usys_t n_ints = sizeof(arr_ints) / sizeof(int);	//	must be 7
		EXPECT_TRUE(n_ints == 7);

		TList<int> list;

		list.Write(arr_ints, n_ints);

		EXPECT_TRUE(list.Count() == 7);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);
		EXPECT_TRUE(list[5] == 5);
		EXPECT_TRUE(list[6] == 6);

		EXPECT_TRUE(list.Read(buffer, 8, 7) == 7);

		EXPECT_TRUE(list.Count() == 0);

		list.Write(arr_ints, n_ints);

		EXPECT_TRUE(list.Count() == 7);

		EXPECT_TRUE(list.Read(buffer, 5, 1) == 5);

		EXPECT_TRUE(list.Count() == 2);

		EXPECT_TRUE(list[0] == 5);
		EXPECT_TRUE(list[1] == 6);
	}

	TEST(io_collection_list_TList, CountMinMax)
	{
		TList<int> list;

		list.Grow(10, 100);

		EXPECT_TRUE(list.Count() == 10);

		EXPECT_TRUE(list.CountMin(0));
		EXPECT_TRUE(list.CountMin(1));
		EXPECT_TRUE(list.CountMin(9));
		EXPECT_TRUE(list.CountMin(10));
		EXPECT_TRUE(!list.CountMin(11));

		EXPECT_TRUE(!list.CountMax(0));
		EXPECT_TRUE(!list.CountMax(1));
		EXPECT_TRUE(!list.CountMax(9));
		EXPECT_TRUE(list.CountMax(10));
		EXPECT_TRUE(list.CountMax(11));
	}

	TEST(io_collection_list_TList, NegativeIndex)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);
		list.Add(5);
		list.Add(6);
		list.Add(7);
		list.Add(8);
		list.Add(9);

		EXPECT_TRUE(list.Count() == 10);

		EXPECT_TRUE(list[0] == 0);
		EXPECT_TRUE(list[1] == 1);
		EXPECT_TRUE(list[2] == 2);
		EXPECT_TRUE(list[3] == 3);
		EXPECT_TRUE(list[4] == 4);
		EXPECT_TRUE(list[5] == 5);
		EXPECT_TRUE(list[6] == 6);
		EXPECT_TRUE(list[7] == 7);
		EXPECT_TRUE(list[8] == 8);
		EXPECT_TRUE(list[9] == 9);

		EXPECT_TRUE(list[-10] == 0);
		EXPECT_TRUE(list[- 9] == 1);
		EXPECT_TRUE(list[- 8] == 2);
		EXPECT_TRUE(list[- 7] == 3);
		EXPECT_TRUE(list[- 6] == 4);
		EXPECT_TRUE(list[- 5] == 5);
		EXPECT_TRUE(list[- 4] == 6);
		EXPECT_TRUE(list[- 3] == 7);
		EXPECT_TRUE(list[- 2] == 8);
		EXPECT_TRUE(list[- 1] == 9);

		list.Insert(-1, 10);

		EXPECT_TRUE(list[- 2] == 10);
		EXPECT_TRUE(list[- 1] ==  9);
	}

	TEST(io_collection_list_TList, Cut)
	{
		TList<int> list;

		list.Add(0);
		list.Add(1);
		list.Add(2);
		list.Add(3);
		list.Add(4);
		list.Add(5);
		list.Add(6);
		list.Add(7);
		list.Add(8);
		list.Add(9);

		EXPECT_TRUE(list.Count() == 10);

		list.Cut(2, 3);
		EXPECT_TRUE(list.Count() == 5);

		EXPECT_TRUE(list[0] == 2);
		EXPECT_TRUE(list[1] == 3);
		EXPECT_TRUE(list[2] == 4);
		EXPECT_TRUE(list[3] == 5);
		EXPECT_TRUE(list[4] == 6);
	}

	TEST(io_collection_list_TList, IndexOutOfBounds_IndexOperator)
	{	//	FIXME
		TList<int> list;

		EXPECT_THROW(list[0] = 17, TIndexOutOfBoundsException);

		list.Add(8);
		list.Add(9);

		EXPECT_THROW(list[2] = 17, TIndexOutOfBoundsException);
	}

	TEST(io_collection_list_TList, WriteOut_Unlimited)
	{
		TList<int> list1;
		TList<int> list2;

		for(int i = 0; i < 100; i++)
			list1.Add(i);

		EXPECT_TRUE(list1.WriteOut(list2) == 100);
	}

	TEST(io_collection_list_TList, WriteOut_Limited)
	{
		TList<int> list1;
		TLimitedBuffer<int> buffer(21);

		for(int i = 0; i < 100; i++)
			list1.Add(i);

		EXPECT_TRUE(list1.WriteOut(buffer) == 21);
	}

	TEST(io_collection_list_TList, WriteOut_Parameterized)
	{
		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			EXPECT_TRUE(list1.WriteOut(buffer, 71, 0) == 71);
		}

		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			EXPECT_TRUE(list1.WriteOut(buffer, 71, 71) == 71);
		}

		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			EXPECT_TRUE(list1.WriteOut(buffer, 120, 0) == 71);
		}

		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			EXPECT_TRUE(list1.WriteOut(buffer, 60, 17) == 60);
		}

		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			EXPECT_TRUE(list1.WriteOut(buffer, 60, 60) == 60);
		}

		{
			TList<int> list1;
			TLimitedBuffer<int> buffer(71);

			for(int i = 0; i < 100; i++)
				list1.Add(i);

			bool did_throw = false;
			try
			{
				list1.WriteOut(buffer, 72);
			}
			catch(const TSinkFloodedException& sfe)
			{
				EXPECT_TRUE(sfe.n_items_requested_write_max == 72);
				EXPECT_TRUE(sfe.n_items_requested_write_min == 72);
				EXPECT_TRUE(sfe.n_items_actually_written == 0);
				EXPECT_TRUE(sfe.n_items_still_space == 71);
				did_throw = true;
			}
			EXPECT_TRUE(did_throw);
		}
	}

	TEST(io_collection_list_TList, Find_sorted)
	{
		for(usys_t n = 0; n <= 1000; n += 97)
		{
			TList<usys_t> list;
			for(usys_t i = 0; i < n; i++)
				list.Add(i);

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, 0, DIRECTION_FORWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, i, DIRECTION_FORWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, 0, DIRECTION_BACKWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, i, DIRECTION_BACKWARD));
		}
	}

	TEST(io_collection_list_TList, Find_unsorted)
	{
		for(usys_t n = 0; n <= 1000; n += 97)
		{
			TList<usys_t> list;
			for(usys_t i = 0; i < n; i++)
				list.Add(i);

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, 0, DIRECTION_FORWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, i, DIRECTION_FORWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, 0, DIRECTION_BACKWARD));

			for(usys_t i = 0; i < n; i++)
				EXPECT_EQ(i, *list.Find(i, i, DIRECTION_BACKWARD));
		}
	}
}

namespace
{
	using namespace cl3::io::collection;
	using namespace cl3::io::collection::array;
	using namespace cl3::system::types;
	using namespace cl3::system::memory;
	using namespace cl3::unittest_support;

	TEST(io_collection_array_TStaticArray, Construct_without_args)
	{
		TStaticArray<int, 3> array;
	}

	TEST(io_collection_array_TStaticArray, Construct_correct_number_of_args)
	{
		TStaticArray<int, 3> array(0,1,2);

		EXPECT_EQ(0, array[0]);
		EXPECT_EQ(1, array[1]);
		EXPECT_EQ(2, array[2]);
	}

	TEST(io_collection_array_TStaticArray, Construct_wrong_number_of_args)
	{
		bool ok = false;
		int arr_src[] = { 1,2,3,4 };
		try
		{
			TStaticArray<int, 3> array(arr_src, 4);
		}
		catch(const cl3::error::TException&)
		{
			ok = true;
		}
		EXPECT_TRUE(ok);
	}

	TEST(io_collection_array_TStaticArray, With_TUniquePtr)
	{
		TStaticArray<TUniquePtr<int>, 3> array = {
			cl3::system::def::move(MakeUniquePtr(new int(0))),
			cl3::system::def::move(MakeUniquePtr(new int(1))),
			cl3::system::def::move(MakeUniquePtr(new int(2)))
		};

		EXPECT_EQ(0, *array[0]);
		EXPECT_EQ(1, *array[1]);
		EXPECT_EQ(2, *array[2]);
	}
}

namespace
{
	using namespace cl3::io::collection;
	using namespace cl3::io::collection::list;
	using namespace cl3::system::types;
	using namespace cl3::unittest_support;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text;

	TEST(io_collection_list_TList, Apply)
	{
		TList<TString> strlst;
		strlst.Append("test   ");
		strlst.Append("  foo");
		strlst.Append("  bbbbar  ");
		strlst.Apply([](TString& e){ e.Trim(); });

		EXPECT_EQ(TString("test"), strlst[0]);
		EXPECT_EQ(TString("foo"), strlst[1]);
		EXPECT_EQ(TString("bbbbar"), strlst[2]);
	}
}
