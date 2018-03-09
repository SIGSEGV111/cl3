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

#include <cl3/core/system_time.hpp>
#include <cl3/core/system_time_calendar.hpp>
#include <cl3/core/system_time_timer.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_terminal.hpp>
#include <gtest/gtest.h>
#include <iostream>

using namespace ::testing;
using namespace std;

namespace	cl3
{
	namespace	system
	{
		namespace	time
		{
			using namespace io::text;

			ostream& operator<<(ostream& os, TTime t)
			{
				os<<"{ "<<t.Seconds()<<"; "<<t.Attoseconds()<<" }";
				return os;
			}

			ITextWriter& operator<<(ITextWriter& w, TTime t)
			{
				w<<"{ "<<t.Seconds()<<"; "<<t.Attoseconds()<<" }";
				return w;
			}

			bool operator==(const timespec ts1, const timespec ts2)
			{
				return ts1.tv_sec == ts2.tv_sec && ts1.tv_nsec == ts2.tv_nsec;
			}

			bool operator==(const timeval tv1, const timeval tv2)
			{
				return tv1.tv_sec == tv2.tv_sec && tv1.tv_usec == tv2.tv_usec;
			}
		}
	}
}

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::time;
	using namespace cl3::system::time::calendar;
	using namespace cl3::io::text;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::terminal;

	TEST(system_time_calendar_TGregorian, Normalize)
	{
		{
			TGregorian g(2018, 03, 14, 25, 61, 1000);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2018, 03, 15, 2, 1+16, 40));
		}

		{
			TGregorian g(2018, 0, 1, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2017, 12, 1, 0, 0, 0));
		}

		{
			TGregorian g(2018, -1, 1, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2017, 11, 1, 0, 0, 0));
		}

		{
			TGregorian g(2018, -11, 1, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2017, 1, 1, 0, 0, 0));
		}

		{
			TGregorian g(2018, -12, 1, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2016, 12, 1, 0, 0, 0));
		}

		{
			TGregorian g(2018, 1, 0, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2017, 12, 31, 0, 0, 0));
		}

		{
			TGregorian g(2018, 1, -1, 0, 0, 0);
			g.Normalize();
			EXPECT_TRUE(g == TGregorian(2017, 12, 30, 0, 0, 0));
		}
	}

	TEST(system_time_calendar_TGregorian, Stringify)
	{
		TGregorian g(2018, 03, 14, 01, 17, 12);
		EXPECT_TRUE( (TString)g == "2018-03-14 01:17:12" );
	}

	TEST(system_time_calendar_TGregorian, TTime_to_TGregorian)
	{
		TGregorian g;
		TTime t;

		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 57, 378545339 * 1000000000ULL)) == (t = TTime(1521192717, 378545339ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 57, 883431062 * 1000000000ULL)) == (t = TTime(1521192717, 883431062ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 58, 388149982 * 1000000000ULL)) == (t = TTime(1521192718, 388149982ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 58, 893133914 * 1000000000ULL)) == (t = TTime(1521192718, 893133914ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 59, 398072161 * 1000000000ULL)) == (t = TTime(1521192719, 398072161ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 31, 59, 902685885 * 1000000000ULL)) == (t = TTime(1521192719, 902685885ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 00, 407328676 * 1000000000ULL)) == (t = TTime(1521192720, 407328676ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 00, 912023523 * 1000000000ULL)) == (t = TTime(1521192720, 912023523ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 01, 416972799 * 1000000000ULL)) == (t = TTime(1521192721, 416972799ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 01, 921867452 * 1000000000ULL)) == (t = TTime(1521192721, 921867452ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 02, 426581072 * 1000000000ULL)) == (t = TTime(1521192722, 426581072ULL * 1000000000ULL)));
		EXPECT_TRUE((g = TGregorian(2018, 3, 16, 9, 32, 02, 931404298 * 1000000000ULL)) == (t = TTime(1521192722, 931404298ULL * 1000000000ULL)));
// 		Terminal()<<"g: "<<(TString)g<<" == t: "<<(TString)(TGregorian)t<<"\n";
	}

	TEST(system_time_calendar_TGregorian, TGregorian_to_TTime)
	{
		TGregorian g;
		TTime t;

		EXPECT_TRUE((t = TTime(1521192717, 378545339ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 57, 378545339 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192717, 883431062ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 57, 883431062 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192718, 388149982ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 58, 388149982 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192718, 893133914ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 58, 893133914 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192719, 398072161ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 59, 398072161 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192719, 902685885ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 31, 59, 902685885 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192720, 407328676ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 00, 407328676 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192720, 912023523ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 00, 912023523 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192721, 416972799ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 01, 416972799 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192721, 921867452ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 01, 921867452 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192722, 426581072ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 02, 426581072 * 1000000000ULL)));
		EXPECT_TRUE((t = TTime(1521192722, 931404298ULL * 1000000000ULL)) == (g = TGregorian(2018, 3, 16, 9, 32, 02, 931404298 * 1000000000ULL)));
		Terminal()<<"g: "<<(TTime)g<<"\nt: "<<t<<"\n";
	}

	TEST(system_time_calendar_TGregorian, NumDaysInMonth)
	{
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2017,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2018,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2019,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2020,  2), 29);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2000,  2), 29);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1600,  2), 29);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1700,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1800,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1583, 10), 21);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1584, 10), 31);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(2018, 10), 31);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1399,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1400,  2), 29);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1401,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1402,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1403,  2), 28);
		EXPECT_EQ((int)TGregorian::NumDaysInMonth(1404,  2), 29);
	}

	TEST(system_time_calendar_TGregorian, NumDaysInYear)
	{
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2017), 365);
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2018), 365);
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2019), 365);
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2020), 366);
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2000), 366);
		EXPECT_EQ((int)TGregorian::NumDaysInYear(2100), 365);
	}
}

namespace
{
	using namespace cl3::system::types;

	namespace	system
	{
		namespace	time
		{
			using namespace cl3::system::time;
			using namespace cl3::system::time::timer;

			TEST(system_time_TTime, Copy)
			{
				const TTime t1(8589934592LL,1000000000LL);
				const TTime t2(t1);
				EXPECT_TRUE(t1 == t2);
			}

			TEST(system_time_TTime, InitFrom_sint64)
			{
				{
					const TTime t1 = TTime::UnixTime((s64_t)8589934592LL);
					const TTime t2(8589934592LL,0LL);
					EXPECT_TRUE(t1 == t2);
				}
				{
					const TTime t1 = TTime::UnixTime((s64_t)-8589934592LL);
					const TTime t2(-8589934592LL,0LL);
					EXPECT_TRUE(t1 == t2);
				}
			}

			TEST(system_time_TTime, InitFrom_float64)
			{
				{
					const double ux_in = 1024.0009765625;
					const TTime t1(1024LL, 976562500000000LL);
					const TTime t2(ux_in);
					EXPECT_TRUE(t1 == t2);
				}
				{
					const double ux_in = -1024.0009765625;
					const TTime t1(-1024LL, -976562500000000LL);
					const TTime t2(ux_in);
					EXPECT_TRUE(t1 == t2);
				}
				{
					const double ux_in = 1024.0009765625;
					const TTime t1 = TTime::UnixTime(ux_in);
					const TTime t2(1024LL, 976562500000000LL);
					EXPECT_TRUE(t1 == t2);
				}
				{
					const double ux_in = -1024.0009765625;
					const TTime t1 = TTime::UnixTime(ux_in);
					const TTime t2(-1024LL, -976562500000000LL);
					EXPECT_TRUE(t1 == t2);
				}
			}

			TEST(system_time_TTime, ConvertTo_timespec)
			{
				{
					const TTime t1(85899345LL, 654743500000000LL);
					const timespec ts_out = t1;
					const timespec ts_exp = { 85899345L, 654743L };
					EXPECT_TRUE(ts_out == ts_exp);
				}
				{
					const TTime t1(-85899345LL, -654743500000000LL);
					const timespec ts_out = t1;
					const timespec ts_exp = { -85899345L, -654743L };
					EXPECT_TRUE(ts_out == ts_exp);
				}
			}

			TEST(system_time_TTime, ConvertTo_timeval)
			{
				{
					const TTime t1(85899345LL, 654743500000000000LL);
					const timeval tv_out = t1;
					const timeval tv_exp = { 85899345L, 654743LL };
					EXPECT_TRUE(tv_out == tv_exp);
				}
				{
					const TTime t1(-85899345LL, -654743500000000000LL);
					const timeval tv_out = t1;
					const timeval tv_exp = { -85899345L, -654743LL };
					EXPECT_TRUE(tv_out == tv_exp);
				}
			}

			TEST(system_time_TTime, UnixTimeI)
			{
				{
					const TTime t1(8589934592LL, 654743500000000LL);
					const s64_t ux_out = t1.UnixTimeI();
					const s64_t ux_exp = 8589934592LL;
					EXPECT_TRUE(ux_out == ux_exp);
				}
				{
					const TTime t1(-8589934592LL, -654743500000000LL);
					const s64_t ux_out = t1.UnixTimeI();
					const s64_t ux_exp = -8589934592LL;
					EXPECT_TRUE(ux_out == ux_exp);
				}
			}

			TEST(system_time_TTime, UnixTimeF)
			{
				{
					const TTime t1(1024LL, 976562500000000LL);
					const f64_t ux_out = t1.UnixTimeF();
					const f64_t ux_exp = 1024.0009765625;
					EXPECT_TRUE(ux_out == ux_exp);
				}
				{
					const TTime t1(-1024LL, -976562500000000LL);
					const f64_t ux_out = t1.UnixTimeF();
					const f64_t ux_exp = -1024.0009765625;
					EXPECT_TRUE(ux_out == ux_exp);
				}
			}

			TEST(system_time_TTime, Roundtrip_timespec)
			{
				{
					const TTime t1(2147483647LL,1000000000LL);
					const timespec ts = t1;
					const TTime t2 = ts;
					EXPECT_TRUE(t1 == t2);
				}
				{
					const TTime t1(-2147483647LL,-1000000000LL);
					const timespec ts = t1;
					const TTime t2 = ts;
					EXPECT_TRUE(t1 == t2);
				}
			}

			TEST(system_time_TTime, Roundtrip_timeval)
			{
				{
					const TTime t1(2147483647LL, 1000000000000LL);
					const timeval tv = t1;
					const TTime t2 = tv;
					EXPECT_TRUE(t1 == t2);
				}
				{
					const TTime t1(-2147483647LL, -1000000000000LL);
					const timeval tv = t1;
					const TTime t2 = tv;
					EXPECT_TRUE(t1 == t2);
				}
			}

			TEST(system_time_TTime, Roundtrip_sint64)
			{
				{
					const TTime t1(8589934592LL,0LL);
					const s64_t tt = t1.UnixTimeI();;
					const TTime t2 = tt;
					EXPECT_TRUE(t1 == t2);
				}
				{
					const TTime t1(-8589934592LL,0LL);
					const s64_t tt = t1.UnixTimeI();;
					const TTime t2 = tt;
					EXPECT_TRUE(t1 == t2);
				}
			}

			TEST(system_time_TTime, Roundtrip_float64)
			{
				{
					const double ux_in  = 1024.0009765625;
					const double ux_out = TTime::UnixTime(ux_in).UnixTimeF();
					EXPECT_TRUE(ux_out == ux_in);
				}
				{
					const double ux_in  = -1024.0009765625;
					const double ux_out = TTime::UnixTime(ux_in).UnixTimeF();
					EXPECT_TRUE(ux_out == ux_in);
				}
			}

			TEST(system_time_TTime, Overflow)
			{
				{
					const TTime t1(0LL,1000000000000000000ULL);
					const TTime t2(1,0);
					EXPECT_TRUE(t1 == t2);
				}
				{
					const TTime t1(0LL,9223372036854775807ULL);
					const TTime t2(9LL, 223372036854775807ULL);
					EXPECT_TRUE(t1 == t2);
				}

				{
					const TTime t1(1LL,-2000000000000000000ULL);
					EXPECT_TRUE(t1 == TTime(-1,0));
				}

				{
					const TTime t1(-1LL,500000000000000000LL);
					EXPECT_TRUE(t1 == TTime(-0.5));
				}
			}

			TEST(system_time_TTime, BasicMath)
			{
				{
					const TTime t1(1,1);
					const TTime t2(2,2);
					const TTime tr(3,3);
					EXPECT_TRUE(tr == t1 + t2);
				}
				{
					const TTime t1(2,2);
					const TTime t2(1,1);
					const TTime tr(1,1);
					EXPECT_TRUE(tr == t1 - t2);
				}
			}

			TEST(system_time_TTime, BasicMath_Overflow)
			{
				{
					const TTime t1(1,1);
					const TTime t2(2LL,999999999999999999ULL);
					const TTime tr(4,0);
					EXPECT_TRUE(tr == t1 + t2);
				}
				{
					const TTime t1(2, 0);
					const TTime t2(0,-3);
					const TTime tr(1LL,1000000000000000000ULL-3ULL);
					EXPECT_TRUE(tr == t1 + t2);
				}
			}

			TEST(system_time_TTime, ConvertToI)
			{
				{
					const TTime t1(1LL,123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::ATTOSECONDS)  == 1123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::FEMTOSECONDS) == 1123456789123456LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::PICOSECONDS)  == 1123456789123LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::NANOSECONDS)  == 1123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::MICROSECONDS) == 1123456LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::MILLISECONDS) == 1123LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::SECONDS)  == 1);
				}
				{
					//	1 day, 1 hour, 54 minutes, 17 seconds, 250 milliseconds
					const TTime t1(86400LL+3600LL+3240LL+17LL,250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::SECONDS) == 86400LL+3600LL+3240LL+17LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::MINUTES) == 1554);
					EXPECT_TRUE(t1.ConvertToI(EUnit::HOURS)   == 25);
					EXPECT_TRUE(t1.ConvertToI(EUnit::DAYS)    == 1);
				}
				{
					const TTime t1(-1LL,-123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::ATTOSECONDS)  == -1123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::FEMTOSECONDS) == -1123456789123456LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::PICOSECONDS)  == -1123456789123LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::NANOSECONDS)  == -1123456789LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::MICROSECONDS) == -1123456LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::MILLISECONDS) == -1123LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::SECONDS)  == -1);
				}
				{
					//	-1 day, -1 hour, -54 minutes, -17 seconds, -250 milliseconds
					const TTime t1(-(86400LL+3600LL+3240LL+17LL),-250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToI(EUnit::SECONDS) == -(86400LL+3600LL+3240LL+17LL));
					EXPECT_TRUE(t1.ConvertToI(EUnit::MINUTES) == -1554);
					EXPECT_TRUE(t1.ConvertToI(EUnit::HOURS)   == -25);
					EXPECT_TRUE(t1.ConvertToI(EUnit::DAYS)    == -1);
				}
				{
					const TTime t1(5.25);
					EXPECT_TRUE(t1.ConvertToI(25) == (s64_t)(5.25*25));
					EXPECT_TRUE(t1.ConvertToI(100ULL) == (s64_t)(5.25*100));
				}
				{
					const TTime t1(-5.25);
					EXPECT_TRUE(t1.ConvertToI(25) == (s64_t)(-5.25*25));
					EXPECT_TRUE(t1.ConvertToI(100ULL) == (s64_t)(-5.25*100));
				}
			}

			TEST(system_time_TTime, ConvertToF)
			{
				{
					const TTime t1(1LL,123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToF(EUnit::ATTOSECONDS)  == 1123456789123456789.0);
					EXPECT_TRUE(t1.ConvertToF(EUnit::FEMTOSECONDS) == 1123456789123456.7890);
					EXPECT_TRUE(t1.ConvertToF(EUnit::PICOSECONDS)  == 1123456789123.4567890);
					EXPECT_TRUE(t1.ConvertToF(EUnit::NANOSECONDS)  == 1123456789.1234567890);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MICROSECONDS) == 1123456.7891234567890);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MILLISECONDS) == 1123.4567891234567890);
					EXPECT_TRUE(t1.ConvertToF(EUnit::SECONDS)      == 1.1234567891234567890);
				}
				{
					//	1 day, 1 hour, 54 minutes, 17 seconds, 250 milliseconds
					const TTime t1(86400LL+3600LL+3240LL+17LL,250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToF(EUnit::SECONDS) == 93257.25);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MINUTES) == 1554.2875);
					EXPECT_TRUE(t1.ConvertToF(EUnit::HOURS)   == 25.904791666666666666666666666666666667);
					EXPECT_TRUE(t1.ConvertToF(EUnit::DAYS)    == 1.0793663194444444444444444444444444444);
				}
				{
					const TTime t1(-1LL,-123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToF(EUnit::ATTOSECONDS)  == -1123456789123456789.0);
					EXPECT_TRUE(t1.ConvertToF(EUnit::FEMTOSECONDS) == -1123456789123456.789);
					EXPECT_TRUE(t1.ConvertToF(EUnit::PICOSECONDS)  == -1123456789123.456789);
					EXPECT_TRUE(t1.ConvertToF(EUnit::NANOSECONDS)  == -1123456789.123456789);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MICROSECONDS) == -1123456.789123456789);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MILLISECONDS) == -1123.456789123456789);
					EXPECT_TRUE(t1.ConvertToF(EUnit::SECONDS)      == -1.123456789123456789);
				}
				{
					//	-1 day, -1 hour, -54 minutes, -17 seconds, -250 milliseconds
					const TTime t1(-(86400LL+3600LL+3240LL+17LL),-250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToF(EUnit::SECONDS) == -93257.25);
					EXPECT_TRUE(t1.ConvertToF(EUnit::MINUTES) == -1554.2875);
					EXPECT_TRUE(t1.ConvertToF(EUnit::HOURS)   == -25.904791666666666666666666666666666667);
					EXPECT_TRUE(t1.ConvertToF(EUnit::DAYS)    == -1.0793663194444444444444444444444444444);
				}
				{
					const TTime t1(5.25);
					EXPECT_TRUE(t1.ConvertToF(25) == (5.25*25));
					EXPECT_TRUE(t1.ConvertToF(100ULL) == (5.25*100));
				}
				{
					const TTime t1(-5.25);
					EXPECT_TRUE(t1.ConvertToF(25) == (-5.25*25));
					EXPECT_TRUE(t1.ConvertToF(100ULL) == (-5.25*100));
				}
			}

			TEST(system_time_TTime, Compare)
			{
				const TTime t1(1,5);
				const TTime t2(2,2);
				const TTime t3(-2,-2);
				const TTime t4(-2,-10);
				const TTime t5(1,5);
				const TTime t6(1,6);
				const TTime t7(2,5);

				EXPECT_TRUE(t1 <  t2);
				EXPECT_TRUE(t1 <= t2);
				EXPECT_TRUE(t2 >  t1);
				EXPECT_TRUE(t2 >= t1);

				EXPECT_TRUE(t1 >  t3);
				EXPECT_TRUE(t1 >= t3);
				EXPECT_TRUE(t3 <  t1);
				EXPECT_TRUE(t3 <= t1);

				EXPECT_TRUE(t1 >  t4);
				EXPECT_TRUE(t1 >= t4);
				EXPECT_TRUE(t4 <  t1);
				EXPECT_TRUE(t4 <= t1);

				EXPECT_FALSE(t1 > t5);
				EXPECT_TRUE(t1 >= t5);
				EXPECT_FALSE(t5 < t1);
				EXPECT_TRUE(t5 <= t1);

				EXPECT_TRUE(t1 <  t6);
				EXPECT_TRUE(t1 <= t6);
				EXPECT_TRUE(t6 >  t1);
				EXPECT_TRUE(t6 >= t1);

				EXPECT_TRUE(t1 == t5);
				EXPECT_FALSE(t1 != t5);
				EXPECT_TRUE(t1 != t6);
				EXPECT_FALSE(t1 == t6);

				EXPECT_TRUE(t1 != t7);
				EXPECT_FALSE(t1 == t7);

				EXPECT_FALSE(t1 > t2);
				EXPECT_FALSE(t2 < t1);
				EXPECT_FALSE(t1 >= t2);
				EXPECT_FALSE(t2 <= t1);
			}

			TEST(system_time_TTime, ConvertFrom_Int)
			{
				EXPECT_TRUE(TTime::ConvertFrom(EUnit::MILLISECONDS, (s64_t)123456789)  == TTime(123456LL,789000000000000000LL));
				EXPECT_TRUE(TTime::ConvertFrom(EUnit::SECONDS, (s64_t)123456789)  == TTime(123456789LL,0LL));
				EXPECT_TRUE(TTime::ConvertFrom(25, (s64_t)17)  == TTime(0.68));
			}

			TEST(system_time_TTime, ConvertFrom_Float)
			{
				EXPECT_TRUE(TTime::ConvertFrom(EUnit::MILLISECONDS, 1000.0)  == TTime(1,0));
				EXPECT_TRUE(TTime::ConvertFrom(EUnit::SECONDS,   123456789.500)  == TTime(123456789LL,500000000000000000LL));
				EXPECT_TRUE(TTime::ConvertFrom(25.0, 17.0)  == TTime(0.68));
			}

			TEST(system_time_TTime, MulDiv)
			{
				{
					TTime t1(11, 0);
					t1 *= 3;
					EXPECT_TRUE(t1.Seconds() == 33 && t1.Attoseconds() == 0);
				}

				{
					TTime t1(10, 0);
					t1 *= 3.5;
					EXPECT_TRUE(t1.Seconds() == 35 && t1.Attoseconds() == 0);
				}

				{
					TTime t1(10LL, 500000000000000000LL);
					t1 *= 3.5;
					EXPECT_TRUE(t1.Seconds() == 36 && t1.Attoseconds() == 750000000000000000LL);
				}

				{
					TTime t1(10, 0);
					t1 /= 2.5;
					EXPECT_TRUE(t1 == TTime(4,0));
				}

				{
					TTime t1(10, 0);
					t1 /= 4;
					EXPECT_TRUE(t1 == TTime(2.5));
				}

				{
					TTime t1 = (TTime(1.5) * 5 + TTime(2.5)) / 4;
					EXPECT_TRUE(t1 == TTime(2.5));
				}
			}

			TEST(system_time_TTime, Now_Realtime)
			{
				const TTime ts = TTime::Now(EClock::REALTIME);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(EClock::REALTIME);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_Monotonic)
			{
				const TTime ts = TTime::Now(EClock::MONOTONIC);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(EClock::MONOTONIC);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_TAI)
			{
				const TTime ts = TTime::Now(EClock::TAI);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(EClock::TAI);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process)
			{
				const TTime ts = TTime::Now(EClock::PROCESS);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(EClock::PROCESS);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread)
			{
				const TTime ts = TTime::Now(EClock::THREAD);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(EClock::THREAD);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process_User)
			{
				const TTime ts = TTime::Now(EClock::PROCESS_USER);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(EClock::PROCESS_USER);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process_Sys)
			{
				const TTime ts = TTime::Now(EClock::PROCESS_SYS);
				for(unsigned i = 0; i < 1000; i++) ::usleep(1L);
				const TTime te = TTime::Now(EClock::PROCESS_SYS);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread_User)
			{
				const TTime ts = TTime::Now(EClock::THREAD_USER);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(EClock::THREAD_USER);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread_Sys)
			{
				const TTime ts = TTime::Now(EClock::THREAD_SYS);
				for(unsigned i = 0; i < 1000; i++) ::usleep(1L);
				const TTime te = TTime::Now(EClock::THREAD_SYS);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			static void TestTimerWait(TTimer& timer)
			{
				const TTime ts_start = TTime::Now(EClock::MONOTONIC);
				timer.Start(TTime(0.01));
				EXPECT_TRUE(timer.WaitFor(1));
				EXPECT_TRUE(timer.WaitFor(0));
				const TTime ts_end = TTime::Now(EClock::MONOTONIC);
				const TTime ts_diff = ts_end - ts_start;
				EXPECT_TRUE(ts_diff >= TTime(0.01));
				EXPECT_TRUE(ts_diff <= TTime(1));
				EXPECT_TRUE(timer.WaitFor(0));
				timer.Stop();
				EXPECT_FALSE(timer.WaitFor(0.1));
				timer.Start(TTime(0.01));
				EXPECT_TRUE(timer.WaitFor(1));
			}

			TEST(system_time_timer_TTimer, Basics)
			{
				TTimer tmr_realtime(EClock::REALTIME);
				TTimer tmr_monotonic(EClock::MONOTONIC);

				TestTimerWait(tmr_realtime);
				TestTimerWait(tmr_monotonic);
			}
		}
	}
}
