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

#include <cl3/core/system_time.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_text_string.hpp>

namespace	cl3
{
	namespace	system
	{
		namespace	time
		{
			namespace	calendar
			{
				using namespace system::types;

				class	CL3PUBT	TJulianDate
				{
					protected:
						s32_t year;
						u32_t month  : 4;
						u32_t day    : 5;
						u32_t hour   : 5;
						u32_t minute : 6;
						u32_t second : 6;
						u64_t attosecond;

					public:
						inline	s32_t	Year		() const CL3_GETTER				{ return year; }
						inline	void	Year		(s32_t _year) CL3_SETTER		{ Set(_year, month, day, hour, minute, second, attosecond); }
						inline	u8_t	Month		() const CL3_GETTER				{ return month; }
						inline	void	Month		(s32_t _month) CL3_SETTER		{ Set(year, _month, day, hour, minute, second, attosecond); }
						inline	u8_t	Day			() const CL3_GETTER				{ return day; }
						inline	void	Day			(s32_t _day)  CL3_SETTER		{ Set(year, month, _day, hour, minute, second, attosecond); }
						inline	u8_t	Hour		() const CL3_GETTER 			{ return hour; }
						inline	void	Hour		(s32_t _hour) CL3_SETTER		{ Set(year, month, day, _hour, minute, second, attosecond); }
						inline	u8_t	Minute		() const CL3_GETTER				{ return minute; }
						inline	void	Minute		(s32_t _minute) CL3_SETTER		{ Set(year, month, day, hour, _minute, second, attosecond); }
						inline	u8_t	Second		() const CL3_GETTER 			{ return second; }
						inline	void	Second		(s32_t _second) CL3_SETTER		{ Set(year, month, day, hour, minute, _second, attosecond); }
						inline	u64_t	Attosecond	() const CL3_GETTER 			{ return attosecond; }
						inline	void	Attosecond	(s64_t _attosecond) CL3_SETTER	{ Set(year, month, day, hour, minute, second, _attosecond); }

						CL3PUBF	virtual	void	Set	(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond) CL3_SETTER;
						CL3PUBF	virtual	TJulianDate&	operator=	(TTime);
						CL3PUBF	virtual	operator TTime	() const CL3_GETTER;

						inline	CLASS	TJulianDate		() : year(1970), month(1), day(1), hour(0), minute(0), second(0), attosecond(0) {}
						inline	CLASS	TJulianDate		(TTime time) { *this = time; }
						inline	CLASS	TJulianDate		(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond) { Set(_year, _month, _day, _hour, _minute, _second, _attosecond); }
				};

				class	CL3PUBT	TGregorianDate : public TJulianDate
				{
					public:
						CL3PUBF	virtual	void	Set	(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond) CL3_SETTER;
						CL3PUBF	virtual	TGregorianDate&	operator=	(TTime);
						CL3PUBF	virtual	operator TTime	() const CL3_GETTER;

						inline	CLASS	TGregorianDate	() : TJulianDate() {}
						inline	CLASS	TGregorianDate	(TTime time) { *this = time; }
						inline	CLASS	TGregorianDate	(s32_t _year, s32_t _month, s32_t _day, s32_t _hour, s32_t _minute, s32_t _second, s64_t _attosecond) { Set(_year, _month, _day, _hour, _minute, _second, _attosecond); }
				};
			}
		}
	}
}

#endif
