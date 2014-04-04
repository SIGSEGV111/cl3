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

#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_collection_bitmask.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::collection::bitmask;

	TEST(Bitmask, Set_Get_Bits)
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

	TEST(Bitmask, Realloc)
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
	using namespace cl3::io::collection::list;

	TEST(List, Add_Remove)
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
	}
}