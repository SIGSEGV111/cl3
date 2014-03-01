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

#ifndef	_include_cl3_core_time_calendar_h_
#define	_include_cl3_core_time_calendar_h_

#include <cl3/core/time.hpp>
#include <cl3/core/system_memory.hpp>

namespace	cl3
{
	namespace	time
	{
		namespace	calendar
		{
			struct	IDate;
			struct	ICalendar;
			typedef	system::memory::TUniquePtr<IDate> IDateUPtr;

			struct	IDate
			{
				virtual	ICalendar&	Calendar	() const = 0;	//	returns the calendar to which the IDate belongs to
				virtual	TTime		RangeStart	() const = 0;	//	returns the first point in time of the currently selected time-unit
				virtual	TTime		RangeEnd	() const = 0;	//	returns the last point in time of the currently selected time-unit

				virtual	IDate&		operator+=	(int) = 0;	//	increment date by the smallest time-unit defined by the calendar times the argument
				virtual	IDate&		operator-=	(int) = 0;	//	decrement date by the smallest time-unit defined by the calendar times the argument
				virtual	int			operator-	(const IDate&) const = 0;	//	returns the number of time-units which lie between the two dates

				virtual	IDate&		operator=	(TTime) = 0;	//	selects the time-unit which contains the specified point in time

				virtual	bool		operator==	(const IDate&) const = 0;
				virtual	bool		operator>=	(const IDate&) const = 0;
				virtual	bool		operator<=	(const IDate&) const = 0;
				virtual	bool		operator>	(const IDate&) const = 0;
				virtual	bool		operator<	(const IDate&) const = 0;
			};

			struct	ICalendar
			{
				virtual	IDateUPtr	FindDate	(TTime) const = 0;
			};








			/*class	TJulianDate
			{
				protected:
					union
					{
						struct
						{
							u64	sec : 6,
								min : 6,
								hour : 5,
								day : 5,
								month : 4;
							s64	year : 38;
						};
						s64 __cmp;
					}
					s64 asec;

				public:
					inline	s64	Year	() const { return year; }
					inline	u8	Month	() const { return (u8)month; }
					inline	u8	Day		() const { return (u8)day; }
					inline	u8	Hour	() const { return (u8)hour; }
					inline	u8	Minute	() const { return (u8)min; }
					inline	u8	Second	() const { return (u8)sec; }

					CL3PUBF	TJulianDate&	Year	(s64 _year)
					{
						//CL3_OBJECT_ERROR(_year < -137438953472 || _year > 137438953471, TArgumentException, _year);
						year = _year;
						return *this;
					}

					CL3PUBF	TJulianDate&	Month	(u8 _month)
					{
						//CL3_OBJECT_ERROR(_month < 1 || _month > 12, TArgumentException, _month);
						month = _month;
						return *this;
					}

					CL3PUBF	TJulianDate&	Day		(u8 _day)
					{
						//CL3_OBJECT_ERROR(_day < 1 || _day > DaysOfMonth(year, month), TArgumentException, _day);
						day = _day;
						return *this;
					}

					CL3PUBF	TJulianDate&	Hour	(u8 _hour)
					{
						//CL3_OBJECT_ERROR(_hour < 1 || _hour > 23, TArgumentException, _hour);
						hour = _hour;
						return *this;
					}

					CL3PUBF	TJulianDate&	Minute	(u8 _minute)
					{
						//CL3_OBJECT_ERROR(_minute < 1 || _minute > 59, TArgumentException, _minute);
						min = _minute;
						return *this;
					}

					CL3PUBF	TJulianDate&	Second	(u8 _second)
					{
						//CL3_OBJECT_ERROR(_second < 1 || _second > 59, TArgumentException, _second);
						sec = _second;
						return *this;
					}

					CL3PUBF	operator TTime	() const;
					CL3PUBF	TJulianDate&	operator=	(TTime);

					CL3PUBF	CLASS	TJulianDate	(s64 year = 1, int month = 1, int day = 1, int hour = 0, int minute = 0, int second = 0, s64 asec = 0);
					CL3PUBF	CLASS	TJulianDate	(TTime);
			};

			//	the gregorian calendar which was introduced by Gregory XIII in 1582 to replace the julian calendar
			//	it automatically switches back to the semantics of the julian calendar for dates before 1582
			class	TGregorianCalendar
			{
			};*/
		}
	}
}

#endif
