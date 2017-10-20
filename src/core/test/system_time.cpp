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
#include <cl3/core/system_time_timer.hpp>
#include <cl3/core/system_types.hpp>
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
			ostream& operator<<(ostream& os, TTime t)
			{
				os<<"{ "<<t.Seconds()<<"; "<<t.Attoseconds()<<" }";
				return os;
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

			TEST(system_time_TTime, ConvertTo_sint64)
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

			TEST(system_time_TTime, ConvertTo_float64)
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
					const TTime t1(-1LL,-123456789123456789LL);
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

			TEST(system_time_TTime, ConvertToF)
			{
				{
					const TTime t1(1LL,123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_ATTOSECONDS)  == 1123456789123456789.0);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_FEMTOSECONDS) == 1123456789123456.7890);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_PICOSECONDS)  == 1123456789123.4567890);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_NANOSECONDS)  == 1123456789.1234567890);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MICROSECONDS) == 1123456.7891234567890);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MILLISECONDS) == 1123.4567891234567890);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_SECONDS)      == 1.1234567891234567890);
				}
				{
					//	1 day, 1 hour, 54 minutes, 17 seconds, 250 milliseconds
					const TTime t1(86400LL+3600LL+3240LL+17LL,250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_SECONDS) == 93257.25);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MINUTES) == 1554.2875);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_HOURS)   == 25.904791666666666666666666666666666667);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_DAYS)    == 1.0793663194444444444444444444444444444);
				}
				{
					const TTime t1(-1LL,-123456789123456789LL);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_ATTOSECONDS)  == -1123456789123456789.0);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_FEMTOSECONDS) == -1123456789123456.789);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_PICOSECONDS)  == -1123456789123.456789);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_NANOSECONDS)  == -1123456789.123456789);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MICROSECONDS) == -1123456.789123456789);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MILLISECONDS) == -1123.456789123456789);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_SECONDS)      == -1.123456789123456789);
				}
				{
					//	-1 day, -1 hour, -54 minutes, -17 seconds, -250 milliseconds
					const TTime t1(-(86400LL+3600LL+3240LL+17LL),-250000000000000000LL);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_SECONDS) == -93257.25);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_MINUTES) == -1554.2875);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_HOURS)   == -25.904791666666666666666666666666666667);
					EXPECT_TRUE(t1.ConvertToF(TIME_UNIT_DAYS)    == -1.0793663194444444444444444444444444444);
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
			}

			TEST(system_time_TTime, ConvertFrom_Int)
			{
				EXPECT_TRUE(TTime::ConvertFrom(TIME_UNIT_MILLISECONDS, (s64_t)123456789)  == TTime(123456LL,789000000000000000LL));
				EXPECT_TRUE(TTime::ConvertFrom(TIME_UNIT_SECONDS, (s64_t)123456789)  == TTime(123456789LL,0LL));
			}

			TEST(system_time_TTime, ConvertFrom_Float)
			{
				EXPECT_TRUE(TTime::ConvertFrom(TIME_UNIT_MILLISECONDS, 1000.0)  == TTime(1,0));
				EXPECT_TRUE(TTime::ConvertFrom(TIME_UNIT_SECONDS,   123456789.500)  == TTime(123456789LL,500000000000000000LL));
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
			}

			TEST(system_time_TTime, Now_Realtime)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_REALTIME);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(TIME_CLOCK_REALTIME);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_Monotonic)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_MONOTONIC);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(TIME_CLOCK_MONOTONIC);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_TAI)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_TAI);
				::usleep(10000L);	// sleep 10ms
				const TTime te = TTime::Now(TIME_CLOCK_TAI);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_min(0LL,  9000000000000000LL);	//   9ms
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out > td_min);
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_PROCESS);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(TIME_CLOCK_PROCESS);
				EXPECT_TRUE(ts < te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_THREAD);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(TIME_CLOCK_THREAD);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process_User)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_PROCESS_USER);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(TIME_CLOCK_PROCESS_USER);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Process_Sys)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_PROCESS_SYS);
				for(unsigned i = 0; i < 1000; i++) ::usleep(1L);
				const TTime te = TTime::Now(TIME_CLOCK_PROCESS_SYS);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread_User)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_THREAD_USER);
				for(volatile unsigned i = 0; i < 0xfffff; i++);
				const TTime te = TTime::Now(TIME_CLOCK_THREAD_USER);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_TTime, Now_CPU_Thread_Sys)
			{
				const TTime ts = TTime::Now(TIME_CLOCK_THREAD_SYS);
				for(unsigned i = 0; i < 1000; i++) ::usleep(1L);
				const TTime te = TTime::Now(TIME_CLOCK_THREAD_SYS);
				EXPECT_TRUE(ts <= te);
				const TTime td_out = te - ts;
				const TTime td_max(0LL,500000000000000000LL);	// 500ms
				EXPECT_TRUE(td_out < td_max);
			}

			TEST(system_time_timer_TTimer, Basics)
			{
				TTimer timer(TIME_CLOCK_MONOTONIC);
				const TTime ts_start = TTime::Now(TIME_CLOCK_MONOTONIC);
				timer.Start(TTime(0.01), false);
				EXPECT_TRUE(timer.WaitFor(1));
				EXPECT_TRUE(timer.WaitFor(0));
				const TTime ts_end = TTime::Now(TIME_CLOCK_MONOTONIC);
				const TTime ts_diff = ts_end - ts_start;
				EXPECT_TRUE(ts_diff >= TTime(0.01));
				EXPECT_TRUE(ts_diff <= TTime(1));
				EXPECT_TRUE(timer.WaitFor(0));
				timer.Stop();
				EXPECT_FALSE(timer.WaitFor(0.1));
				timer.Start(TTime(0.01), false);
				EXPECT_TRUE(timer.WaitFor(1));
			}
		}
	}
}
