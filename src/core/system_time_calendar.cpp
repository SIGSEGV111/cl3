#include "system_time_calendar.hpp"
#include "error.hpp"
#include "io_text_string.hpp"

namespace cl3
{
	namespace system
	{
		namespace time
		{
			namespace calendar
			{
				using namespace error;

				static const s64_t SECONDS_PER_MINUTE = 60;
				static const s64_t SECONDS_PER_HOUR = 60 * SECONDS_PER_MINUTE;
				static const s64_t SECONDS_PER_DAY = 24 * SECONDS_PER_HOUR;

				template<typename T>
				inline static void NormalizeSingle(T& value, const T upper_bound, s32_t& overflow_field)
				{
					const s64_t f = value / upper_bound + (value < 0 ? 1 : 0);
					overflow_field += f;
					value %= upper_bound;
					value = value < 0 ? -value : value;
				}

				inline static void NormalizeMonthYear(s32_t& year, s32_t& month)
				{
					if(month < 1)
					{
						while(month < 1)
						{
							month += 12;
							year--;
						}
					}
					else if(month > 12)
					{
						while(month > 12)
						{
							month -= 12;
							year++;
						}
					}
				}

				TGregorian& TGregorian::operator+=(const TGregorian& rhs)
				{
					attoseconds += rhs.attoseconds;
					year += rhs.year;
					month += rhs.month;
					day += rhs.day;
					hour += rhs.hour;
					minute += rhs.minute;
					second += rhs.second;
					Normalize();
					return *this;
				}

				TGregorian& TGregorian::operator-=(const TGregorian& rhs)
				{
					attoseconds -= rhs.attoseconds;
					year -= rhs.year;
					month -= rhs.month;
					day -= rhs.day;
					hour -= rhs.hour;
					minute -= rhs.minute;
					second -= rhs.second;
					Normalize();
					return *this;
				}

				TGregorian TGregorian::operator+(const TGregorian& rhs) const
				{
					TGregorian r = *this;
					r += rhs;
					return r;
				}

				TGregorian TGregorian::operator-(const TGregorian& rhs) const
				{
					TGregorian r = *this;
					r -= rhs;
					return r;
				}

				bool TGregorian::operator==(const TGregorian& rhs) const
				{
					return
						this->attoseconds == rhs.attoseconds &&
						this->year == rhs.year &&
						this->month == rhs.month &&
						this->day == rhs.day &&
						this->hour == rhs.hour &&
						this->minute == rhs.minute &&
						this->second == rhs.second;
				}

				bool TGregorian::operator> (const TGregorian& rhs) const
				{
					if(this->year > rhs.year)
						return true;
					else if(this->year < rhs.year)
						return false;

					if(this->month > rhs.month)
						return true;
					else if(this->month < rhs.month)
						return false;

					if(this->day > rhs.day)
						return true;
					else if(this->day < rhs.day)
						return false;

					if(this->hour > rhs.hour)
						return true;
					else if(this->hour < rhs.hour)
						return false;

					if(this->minute > rhs.minute)
						return true;
					else if(this->minute < rhs.minute)
						return false;

					if(this->second > rhs.second)
						return true;
					else if(this->second < rhs.second)
						return false;

					if(this->attoseconds > rhs.attoseconds)
						return true;
					else
						return false;
				}

				bool TGregorian::operator>=(const TGregorian& rhs) const
				{
					if(this->year > rhs.year)
						return true;
					else if(this->year < rhs.year)
						return false;

					if(this->month > rhs.month)
						return true;
					else if(this->month < rhs.month)
						return false;

					if(this->day > rhs.day)
						return true;
					else if(this->day < rhs.day)
						return false;

					if(this->hour > rhs.hour)
						return true;
					else if(this->hour < rhs.hour)
						return false;

					if(this->minute > rhs.minute)
						return true;
					else if(this->minute < rhs.minute)
						return false;

					if(this->second > rhs.second)
						return true;
					else if(this->second < rhs.second)
						return false;

					if(this->attoseconds > rhs.attoseconds)
						return true;
					else if(this->attoseconds < rhs.attoseconds)
						return false;

					return true;
				}

				bool TGregorian::operator< (const TGregorian& rhs) const
				{
					return !(*this >= rhs);
				}

				bool TGregorian::operator<=(const TGregorian& rhs) const
				{
					return !(*this > rhs);
				}

				void TGregorian::Normalize()
				{
					NormalizeSingle(attoseconds, (s64_t)1000000000000000000LL, second);
					NormalizeSingle(second, 60, minute);
					NormalizeSingle(minute, 60, hour);
					NormalizeSingle(hour, 24, day);

					NormalizeMonthYear(year, month);

					while(day > NumDaysInMonth(year, month))
					{
						day -= NumDaysInMonth(year, month);
						month++;
						NormalizeMonthYear(year, month);
					}

					while(day <= 0)
					{
						month--;
						NormalizeMonthYear(year, month);
						day += NumDaysInMonth(year, month);
					}
				}

				u16_t TGregorian::NumDaysInYear(const s32_t year)
				{
					return 365 - 28 - 31 + NumDaysInMonth(year, 2) + NumDaysInMonth(year, 10);
				}

				static const u8_t ARR_DAYS_PER_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

				u8_t TGregorian::NumDaysInMonth(const s32_t year, const u8_t month)
				{
					CL3_NONCLASS_ERROR(month < 1 || month > 12, TException, "month must be in the range of 1 to 12 (including)");

					if(CL3_UNLIKELY(year == 1583 && month == 10))
						return 31-10;

					if(CL3_UNLIKELY(month == 2))
					{
						if(year > 1583)
						{
							if((year % 4) == 0)
							{
								if((year % 100) == 0)
								{
									if((year % 400) == 0)
										return 29;
									else
										return 28;
								}
								else
									return 29;
							}
							else
								return 28;
						}
						else
							return 28 + (((year % 4) == 0) ? 1 : 0);
					}

					return ARR_DAYS_PER_MONTH[month-1];
				}

				TGregorian::operator TTime() const
				{
					if(year >= 1970)
					{
						s64_t s = 0;

						for(s32_t y = 1970; y < year; y++)
							s += NumDaysInYear(y) * SECONDS_PER_DAY;

						for(s32_t m = 1; m < month; m++)
							s += NumDaysInMonth(year, m) * SECONDS_PER_DAY;

						s += (day - 1) * SECONDS_PER_DAY;
						s += hour * SECONDS_PER_HOUR;
						s += minute * SECONDS_PER_MINUTE;
						s += second;

						return TTime(s, attoseconds);
					}
					else
						CL3_NOT_IMPLEMENTED;
				}

				TGregorian::operator io::text::string::TString() const
				{
					using namespace io::text;
					using namespace io::text::string;

					TNumberFormat nf_year = TNumberFormat::DECIMAL_SIMPLE;
					nf_year.integer_padding = ' ';
					nf_year.integer_length_min = 4;

					TNumberFormat nf_std = TNumberFormat::DECIMAL_SIMPLE;
					nf_std.integer_padding = '0';
					nf_std.integer_length_min = 2;

					return TString::Format("§-§-§ §:§:§",
										   TString(this->year, &nf_year),
										   TString(this->month, &nf_std),
										   TString(this->day, &nf_std),
										   TString(this->hour, &nf_std),
										   TString(this->minute, &nf_std),
										   TString(this->second, &nf_std));
				}

				CLASS TGregorian::TGregorian() :
					attoseconds(0),
					year(1),
					month(1),
					day(1),
					hour(0),
					minute(0),
					second(0)
				{
				}

				CLASS TGregorian::TGregorian(const TTime time) : attoseconds(time.Attoseconds()), year(1970), month(1), day(1), hour(0), minute(0), second(0)
				{
					// TTime is in seconds since 1970-01-01
					if(time.Seconds() >= 0)
					{
						const u64_t seconds_per_day = 24 * 60 * 60;
						this->day += time.Seconds() / seconds_per_day;
						this->second += time.Seconds() % seconds_per_day;
						this->Normalize();
					}
					else
						CL3_NOT_IMPLEMENTED;
				}

				CLASS TGregorian::TGregorian(s32_t year, s32_t month, s32_t day, s32_t hour, s32_t minute, s32_t second, s64_t attoseconds) :
					attoseconds(attoseconds),
					year(year),
					month(month),
					day(day),
					hour(hour),
					minute(minute),
					second(second)
				{
				}
			}
		}
	}
}
