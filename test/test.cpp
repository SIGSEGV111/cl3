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

#include <cl3/core/time.hpp>
#include <gtest/gtest.h>
#include <iostream>

using namespace ::testing;
using namespace std;

namespace	cl3
{
	namespace	time
	{
		ostream& operator<<(ostream& os, TTime t)
		{
			os<<"{ "<<t.Seconds()<<"; "<<t.Attoseconds()<<" }";
			return os;
		}
	}
}

namespace
{
	namespace	time
	{
		using namespace cl3::time;

		TEST(cl3_core, Time_Copy)
		{
			TTime t1(1,1000000000);
			TTime t2(t1);
			EXPECT_TRUE(t1 == t2);
		}

		TEST(cl3_core, Time_Roundtrip_CL_UX_CL)
		{
			TTime t1(1,1000000000);
			timespec ts = t1;
			TTime t2 = ts;
			EXPECT_TRUE(t1 == t2);
		}

		TEST(cl3_core, Time_Overflow)
		{
			{
				TTime t1(0,1000000000000000000ULL);
				TTime t2(1,0);
				EXPECT_TRUE(t1 == t2);
			}
			{
				TTime t1(0,9223372036854775807ULL);
				TTime t2(9, 223372036854775807ULL);
				EXPECT_TRUE(t1 == t2);
			}
		}

		TEST(cl3_core, Time_BasicMath)
		{
			{
				TTime t1(1,1);
				TTime t2(2,2);
				TTime tr(3,3);
				EXPECT_TRUE(tr == t1 + t2);
			}
			{
				TTime t1(2,2);
				TTime t2(1,1);
				TTime tr(1,1);
				EXPECT_TRUE(tr == t1 - t2);
			}
		}

		TEST(cl3_core, Time_BasicMath_Overflow)
		{
			{
				TTime t1(1,1);
				TTime t2(2,999999999999999999ULL);
				TTime tr(4,0);
				EXPECT_TRUE(tr == t1 + t2);
			}
			{
				TTime t1(2, 0);
				TTime t2(0,-3);
				TTime tr(1,1000000000000000000ULL-3ULL);
				EXPECT_TRUE(tr == t1 + t2);
			}
		}

		TEST(cl3_core, Time_InitFromFloat)
		{
			const TTime td(0,1000);
			{
				TTime t1(1,900000000000000000LL);
				TTime t2(1.9);
				EXPECT_TRUE(t1 + td > t2);
				EXPECT_TRUE(t1 - td < t2);
			}
			{
				TTime t1(1,100000000000000000LL);
				TTime t2(1.1);
				EXPECT_TRUE(t1 + td > t2);
				EXPECT_TRUE(t1 - td < t2);
			}
			{
				TTime t1(1,500000000000000000LL);
				TTime t2(1.5);
				EXPECT_TRUE(t1 + td > t2);
				EXPECT_TRUE(t1 - td < t2);
			}
			{
				TTime t1(1,700000000000000000LL);
				TTime t2(1.7);
				EXPECT_TRUE(t1 + td > t2);
				EXPECT_TRUE(t1 - td < t2);
			}
			{
				TTime t1(1,300000000000000000LL);
				TTime t2(1.3);
				EXPECT_TRUE(t1 + td > t2);
				EXPECT_TRUE(t1 - td < t2);
			}
		}

		TEST(cl3_core, Time_ConvertToI)
		{
			{
				const TTime t1(1,123456789123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_ATTOSECONDS)  == 1123456789123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_FEMTOSECONDS) == 1123456789123456LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_PICOSECONDS)  == 1123456789123LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_NANOSECONDS)  == 1123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MICROSECONDS) == 1123456LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MILLISECONDS) == 1123LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_SECONDS)  == 1);
			}
			{
				//	1 day, 1 hour, 54 minutes, 17 seconds, 250 milliseconds
				const TTime t1(86400LL+3600LL+3240LL+17LL,250000000000000000LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_SECONDS) == 86400LL+3600LL+3240LL+17LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MINUTES) == 1554);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_HOURS)   == 25);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_DAYS)    == 1);
			}
			{
				const TTime t1(-1,-123456789123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_ATTOSECONDS)  == -1123456789123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_FEMTOSECONDS) == -1123456789123456LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_PICOSECONDS)  == -1123456789123LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_NANOSECONDS)  == -1123456789LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MICROSECONDS) == -1123456LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MILLISECONDS) == -1123LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_SECONDS)  == -1);
			}
			{
				//	-1 day, -1 hour, -54 minutes, -17 seconds, -250 milliseconds
				const TTime t1(-(86400LL+3600LL+3240LL+17LL),-250000000000000000LL);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_SECONDS) == -(86400LL+3600LL+3240LL+17LL));
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_MINUTES) == -1554);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_HOURS)   == -25);
				EXPECT_TRUE(t1.ConvertToI(TIME_UNIT_DAYS)    == -1);
			}
		}
	}
}
