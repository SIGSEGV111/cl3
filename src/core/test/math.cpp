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

#include <cl3/core/math.hpp>
#include <cl3/core/io_text_string.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::math;
	using namespace cl3::system::memory;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace std;

	TEST(math_TVector_3i, add)
	{
		TVector<int, 3> op1(1,2,3);
		TVector<int, 3> op2(1,2,3);
		TVector<int, 3> out = op1 + op2;

		EXPECT_EQ(2, out[0]);
		EXPECT_EQ(4, out[1]);
		EXPECT_EQ(6, out[2]);
	}

	TEST(math_TVector_3i, sub)
	{
		TVector<int, 3> op1(1,2,3);
		TVector<int, 3> op2(1,2,3);
		TVector<int, 3> out = op1 - op2;

		EXPECT_EQ(0, out[0]);
		EXPECT_EQ(0, out[1]);
		EXPECT_EQ(0, out[2]);
	}

	TEST(math_TVector_3i, mul)
	{
		TVector<int, 3> op1(1,2,3);
		TVector<int, 3> op2(1,2,3);
		TVector<int, 3> out = op1 * op2;

		EXPECT_EQ(1, out[0]);
		EXPECT_EQ(4, out[1]);
		EXPECT_EQ(9, out[2]);
	}

	TEST(math_TVector_3i, div)
	{
		TVector<int, 3> op1(1,2,3);
		TVector<int, 3> op2(1,2,3);
		TVector<int, 3> out = op1 / op2;

		EXPECT_EQ(1, out[0]);
		EXPECT_EQ(1, out[1]);
		EXPECT_EQ(1, out[2]);
	}

	TEST(math_TVector_3i, assign)
	{
		TVector<int, 3> op1(1,2,3);
		TVector<int, 3> op2(2,4,6);
		op2 = op1;

		EXPECT_EQ(1, op2[0]);
		EXPECT_EQ(2, op2[1]);
		EXPECT_EQ(3, op2[2]);
	}

	TEST(math_TVector_3i, equal)
	{
		{
			TVector<int, 3> op1(1,2,3);
			TVector<int, 3> op2(1,2,3);
			EXPECT_TRUE(op2 == op1);
			EXPECT_FALSE(op2 != op1);
		}

		{
			TVector<int, 3> op1(1,2,3);
			TVector<int, 3> op2(2,2,3);
			EXPECT_TRUE(op2 != op1);
			EXPECT_FALSE(op2 == op1);
		}

		{
			TVector<int, 3> op1(1,2,4);
			TVector<int, 3> op2(2,2,3);
			EXPECT_TRUE(op2 != op1);
			EXPECT_FALSE(op2 == op1);
		}

		{
			TVector<int, 3> op1(1,5,4);
			TVector<int, 3> op2(2,2,3);
			EXPECT_TRUE(op2 != op1);
			EXPECT_FALSE(op2 == op1);
		}
	}

	TEST(math_TVector_3i, Square)
	{
		{
			TVector<int, 3> op1(1,1,1);
			EXPECT_EQ(3, op1.Square());
		}

		{
			TVector<int, 3> op1(2,2,2);
			EXPECT_EQ(12, op1.Square());
		}

		{
			TVector<int, 3> op1(2,2,-2);
			EXPECT_EQ(12, op1.Square());
		}
	}

	TEST(math_TVector_3i, Volume)
	{
		{
			TVector<int, 3> op1(1,1,1);
			EXPECT_EQ(1, op1.Volume());
		}

		{
			TVector<int, 3> op1(2,2,2);
			EXPECT_EQ(8, op1.Volume());
		}

		{
			TVector<int, 3> op1(2,2,-2);
			EXPECT_EQ(-8, op1.Volume());
		}
	}

	TEST(math_TVector_3i, Length)
	{
		{
			TVector<int, 3> op1(1,1,1);
			EXPECT_EQ(1, op1.Length());
		}

		{
			TVector<int, 3> op1(2,2,2);
			EXPECT_EQ(3, op1.Length());
		}

		{
			TVector<int, 3> op1(2,2,-2);
			EXPECT_EQ(3, op1.Length());
		}
	}

	TEST(math_TVector_3i, Max)
	{
		{
			TVector<int, 3> op1(1,1,1);
			EXPECT_EQ(1, op1.Max());
		}

		{
			TVector<int, 3> op1(2,2,2);
			EXPECT_EQ(2, op1.Max());
		}

		{
			TVector<int, 3> op1(2,2,-2);
			EXPECT_EQ(2, op1.Max());
		}

		{
			TVector<int, 3> op1(17,4,13);
			EXPECT_EQ(17, op1.Max());
		}

		{
			TVector<int, 3> op1(17,4,18);
			EXPECT_EQ(18, op1.Max());
		}
	}

	TEST(math_TVector_3i, Min)
	{
		{
			TVector<int, 3> op1(1,1,1);
			EXPECT_EQ(1, op1.Min());
		}

		{
			TVector<int, 3> op1(2,2,2);
			EXPECT_EQ(2, op1.Min());
		}

		{
			TVector<int, 3> op1(2,2,-2);
			EXPECT_EQ(-2, op1.Min());
		}

		{
			TVector<int, 3> op1(17,4,13);
			EXPECT_EQ(4, op1.Min());
		}

		{
			TVector<int, 3> op1(4,17,18);
			EXPECT_EQ(4, op1.Min());
		}
	}

	TEST(math_TVector_3i, Abs)
	{
		{
			TVector<int, 3> op1(-1,-1,1);
			TVector<int, 3> op2 = op1.Abs();
			EXPECT_EQ(1, op2[0]);
			EXPECT_EQ(1, op2[1]);
			EXPECT_EQ(1, op2[2]);
		}

		{
			TVector<int, 3> op1(-1,-17,4);
			TVector<int, 3> op2 = op1.Abs();
			EXPECT_EQ(1, op2[0]);
			EXPECT_EQ(17, op2[1]);
			EXPECT_EQ(4, op2[2]);
		}

		{
			TVector<int, 3> op1(-1,-17,-4);
			TVector<int, 3> op2 = op1.Abs();
			EXPECT_EQ(1, op2[0]);
			EXPECT_EQ(17, op2[1]);
			EXPECT_EQ(4, op2[2]);
		}
	}
}
