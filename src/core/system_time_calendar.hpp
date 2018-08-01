#pragma once

#include "system_time.hpp"

namespace cl3
{
	namespace io
	{
		namespace text
		{
			namespace string
			{
				class TString;
			}
		}
	}

	namespace system
	{
		namespace time
		{
			namespace calendar
			{
				using namespace types;

				struct TGregorian
				{
					s64_t attoseconds;

					s32_t year;
					s32_t month;
					s32_t day;

					s32_t hour;
					s32_t minute;
					s32_t second;

					CL3PUBF TGregorian& operator+=(const TGregorian&);
					CL3PUBF TGregorian& operator-=(const TGregorian&);
					CL3PUBF TGregorian operator+(const TGregorian&) const CL3_GETTER;
					CL3PUBF TGregorian operator-(const TGregorian&) const CL3_GETTER;

					CL3PUBF bool operator==(const TGregorian&) const CL3_GETTER;
					inline bool operator!=(const TGregorian& rhs) const CL3_GETTER { return !(*this == rhs); }

					CL3PUBF bool operator> (const TGregorian&) const CL3_GETTER;
					CL3PUBF bool operator>=(const TGregorian&) const CL3_GETTER;
					CL3PUBF bool operator< (const TGregorian&) const CL3_GETTER;
					CL3PUBF bool operator<=(const TGregorian&) const CL3_GETTER;

					CL3PUBF void Normalize();

					CL3PUBF static u16_t NumDaysInYear(const s32_t year) CL3_GETTER;
					CL3PUBF static u8_t  NumDaysInMonth(const s32_t year, const u8_t month) CL3_GETTER;

					CL3PUBF operator TTime() const CL3_GETTER;
					CL3PUBF explicit operator io::text::string::TString() const CL3_GETTER;

					CL3PUBF CLASS TGregorian();
					CL3PUBF CLASS TGregorian(const TTime time);
					CL3PUBF CLASS TGregorian(s32_t year, s32_t month, s32_t day, s32_t hour = 0, s32_t minute = 0, s32_t second = 0, s64_t attoseconds = 0);
				};
			}
		}
	}
}
