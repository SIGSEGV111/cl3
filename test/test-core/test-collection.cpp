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
}