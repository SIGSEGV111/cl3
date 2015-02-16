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

#include <cl3/mm/time_calendar.hpp>
#include <gtest/gtest.h>
#include <iostream>

using namespace ::testing;
using namespace std;

namespace
{
	using namespace cl3::system::time;
	using namespace cl3::system::time::calendar;

	TEST(system_time_calendar_TJulianDate, D_1970_01_01__00_00_00)
	{
		TJulianDate date( TTime(0,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

		EXPECT_TRUE( date.Year()  == 1970 );
		EXPECT_TRUE( date.Month() == 1 );
		EXPECT_TRUE( date.Day()   == 1 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1970_01_02__00_00_00)
	{
		TJulianDate date( TTime(86400,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

		EXPECT_TRUE( date.Year()  == 1970 );
		EXPECT_TRUE( date.Month() == 1 );
		EXPECT_TRUE( date.Day()   == 2 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1970_02_01__00_00_00)
	{
		TJulianDate date( TTime(86400*31,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

		EXPECT_TRUE( date.Year()  == 1970 );
		EXPECT_TRUE( date.Month() == 2 );
		EXPECT_TRUE( date.Day()   == 1 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1969_12_01__00_00_00)
	{
		TJulianDate date( TTime(-86400*31,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

		EXPECT_TRUE( date.Year()  == 1969 );
		EXPECT_TRUE( date.Month() == 12 );
		EXPECT_TRUE( date.Day()   == 1 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1972_01_01__00_00_00)
	{
		TJulianDate date( TTime(31536000*2,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1972 );
		EXPECT_TRUE( date.Month() == 1 );
		EXPECT_TRUE( date.Day()   == 1 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1972_02_29__00_00_00)
	{
		TJulianDate date( TTime(31536000*2 + 86400 * (31 + 28),0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1972 );
		EXPECT_TRUE( date.Month() == 2 );
		EXPECT_TRUE( date.Day()   == 29 );

		EXPECT_TRUE( date.Hour()       == 0 );
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1971_02_05__07_17_19__123456)
	{
		TJulianDate date( TTime((365LL + 35LL) * 24LL * 3600LL + 7LL * 3600LL + 17LL * 60LL + 19LL, 123456LL) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1971 );
		EXPECT_TRUE( date.Month() == 2 );
		EXPECT_TRUE( date.Day()   == 5 );

		EXPECT_TRUE( date.Hour()       == 7 );
		EXPECT_TRUE( date.Minute()     == 17 );
		EXPECT_TRUE( date.Second()     == 19 );
		EXPECT_TRUE( date.Attosecond() == 123456 );
	}

	TEST(system_time_calendar_TJulianDate, D_1973_02_04__07_17_19__123456)
	{
		TJulianDate date( TTime((3*365LL + 35LL) * 24LL * 3600LL + 7LL * 3600LL + 17LL * 60LL + 19LL, 123456LL) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1973 );
		EXPECT_TRUE( date.Month() == 2 );
		EXPECT_TRUE( date.Day()   == 4 );

		EXPECT_TRUE( date.Hour()       == 7 );
		EXPECT_TRUE( date.Minute()     == 17 );
		EXPECT_TRUE( date.Second()     == 19 );
		EXPECT_TRUE( date.Attosecond() == 123456 );
	}

	TEST(system_time_calendar_TJulianDate, D_0001_01_01__18_00_00)
	{
		TJulianDate date( TTime(-62136914400LL,0LL) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1 );
		EXPECT_TRUE( date.Month() == 1 );
		EXPECT_TRUE( date.Day()   == 1 );

		EXPECT_TRUE( date.Hour()       == 18);
		EXPECT_TRUE( date.Minute()     == 0 );
		EXPECT_TRUE( date.Second()     == 0 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	TEST(system_time_calendar_TJulianDate, D_1969_12_31__23_59_59)
	{
		TJulianDate date( TTime(-1,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

// 		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1969 );
		EXPECT_TRUE( date.Month() == 12 );
		EXPECT_TRUE( date.Day()   == 31 );

		EXPECT_TRUE( date.Hour()       == 23 );
		EXPECT_TRUE( date.Minute()     == 59 );
		EXPECT_TRUE( date.Second()     == 59 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}

	//FIXME
	/*TEST(system_time_calendar_TJulianDate, D_1968_12_31__23_59_59)
	{
		TJulianDate date( TTime(-1 - 86400*365,0) );

		//EXPECT_TRUE( (TTime)date == TTime(0, 0) );

		printf("date = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());

		EXPECT_TRUE( date.Year()  == 1968 );
		EXPECT_TRUE( date.Month() == 12 );
		EXPECT_TRUE( date.Day()   == 31 );

		EXPECT_TRUE( date.Hour()       == 23 );
		EXPECT_TRUE( date.Minute()     == 59 );
		EXPECT_TRUE( date.Second()     == 59 );
		EXPECT_TRUE( date.Attosecond() == 0 );
	}*/

	//FIXME
	/*TEST(system_time_calendar_TJulianDate, IterativeN)
	{
		s32_t year   = 1970;
		s32_t month  = 1;
		s32_t day    = 1;
		s32_t hour   = 0;
		s32_t minute = 0;
		s32_t second = 0;

		const u8_t days_of_month_normal[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		//	1/4 day steps (21600 seconds)
		for(s64_t seconds = 0; seconds <= 864000000; seconds += 21600)
		{
			TJulianDate date( TTime(-seconds, 0) );

			EXPECT_FALSE(date.Year() != year || date.Month() != month || date.Day() != day || date.Hour() != hour || date.Minute() != minute || date.Second() != second);
			if(date.Year() != year || date.Month() != month || date.Day() != day || date.Hour() != hour || date.Minute() != minute || date.Second() != second)
			{
				printf("date is        = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());
				printf("date should be = %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
			}

			hour -= 6;
			if(hour < 0)
			{
				day--;
				hour += 24;

				if(day < 1)
				{
					month--;
					if(month < 1)
					{
						month += 12;
						year--;
					}
					day += days_of_month_normal[month-1];
					if(year % 4 == 0 && month == 2)
						day++;
				}
			}
		}
	}

	//FIXME
	TEST(system_time_calendar_TJulianDate, IterativeP)
	{
		s32_t year   = 1970;
		s32_t month  = 1;
		s32_t day    = 1;
		s32_t hour   = 0;
		s32_t minute = 0;
		s32_t second = 0;

		const u8_t days_of_month_normal[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		//	1/4 day steps (21600 seconds)
		for(s64_t seconds = 0; seconds <= 864000000; seconds += 21600)
		{
			TJulianDate date( TTime(seconds, 0) );

			EXPECT_FALSE(date.Year() != year || date.Month() != month || date.Day() != day || date.Hour() != hour || date.Minute() != minute || date.Second() != second);
			if(date.Year() != year || date.Month() != month || date.Day() != day || date.Hour() != hour || date.Minute() != minute || date.Second() != second)
			{
				printf("date is        = %04d-%02d-%02d %02d:%02d:%02d\n", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());
				printf("date should be = %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
			}

			hour += 6;
			if(hour > 23)
			{
				day++;
				hour -= 24;

				if(day > days_of_month_normal[month-1] + ((year % 4 == 0 && month == 2) ? 1 :0))
				{
					day -= days_of_month_normal[month-1];
					if(year % 4 == 0 && month == 2)
						day--;
					month++;
					if(month > 12)
					{
						month -= 12;
						year++;
					}
				}
			}
		}
	}*/
}
