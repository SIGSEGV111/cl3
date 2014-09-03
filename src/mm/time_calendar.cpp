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

#include "time_calendar.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	time
		{
			namespace	calendar
			{
				/**********************************************************/

				static const s64_t days_per_regular_year = 365;
				static const s64_t days_per_leapcluster  = 4 * days_per_regular_year + 1;	//	4 years + 1 leapday

				static const s64_t seconds_per_second =  1;	//	obvious, isn't it?
				static const s64_t seconds_per_minute = 60;
				static const s64_t seconds_per_hour = 3600;	//	60 minutes * 60 seconds
				static const s64_t seconds_per_day = 86400;	//	24 hours * 60 minutes * 60 seconds
				static const s64_t seconds_per_leapcluster  = seconds_per_day * days_per_leapcluster;
				static const s64_t seconds_per_regular_year = seconds_per_day * days_per_regular_year;

				static const u8_t days_of_month_normal[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
				static const u8_t days_of_month_leap[]   = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

				void	TJulianDate::Set	(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TJulianDate&	TJulianDate::operator=	(TTime time)
				{
					s64_t seconds = time.Seconds() - 3 * seconds_per_regular_year - seconds_per_day;

					//	a leap cluster is a pack of 4 years (which is always "seconds_per_leapcluster" seconds long)
					//	leapcluster can be negative, but seconds has to be positive to make modulo (%) work properly
					const s64_t leapcluster = seconds / seconds_per_leapcluster + ((seconds < 0 && (-seconds % seconds_per_leapcluster) != 0) ? -1 : 0);

					//	adjust the number of remaining seconds, according to the number of leapclusters, which we take as base
					seconds -= leapcluster * seconds_per_leapcluster;

					attosecond = time.Attoseconds();

					year = seconds / seconds_per_regular_year;
					seconds -= year * seconds_per_regular_year;
					year += 1973 + leapcluster * 4;

					const u8_t* const days_of_month = (year % 4) == 0 ? days_of_month_leap : days_of_month_normal;
					for(month = 0; seconds >= (days_of_month[month] * seconds_per_day); month++)
						seconds -= (days_of_month[month] * seconds_per_day);
					month++;

					day = seconds / seconds_per_day + 1;

					hour   = (seconds % seconds_per_day)    / seconds_per_hour;
					minute = (seconds % seconds_per_hour)   / seconds_per_minute;
					second = (seconds % seconds_per_minute) / seconds_per_second;

					return *this;
				}

				TJulianDate::operator TTime	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				/**********************************************************/

				void	TGregorianDate::Set	(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TGregorianDate&	TGregorianDate::operator=	(TTime time)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TGregorianDate::operator TTime	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				/**********************************************************/
			}
		}
	}
}
