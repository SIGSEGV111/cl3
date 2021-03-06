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

#include "system_time.hpp"
#include "io_serialization.hpp"
#include <math.h>

namespace cl3
{
	namespace	system
	{
		namespace	time
		{
			void	TTime::Normalize	()
			{
				if(asec <= -1000000000000000000LL)
				{
					const s64_t f = asec / 1000000000000000000LL;
					sec += f;
					asec -= f * 1000000000000000000LL;
				}
				else if(asec >= 1000000000000000000LL)
				{
					const s64_t f = asec / 1000000000000000000LL;
					sec += f;
					asec -= f * 1000000000000000000LL;
				}

				if(asec < 0 && sec > 0)
				{
					sec--;
					asec += 1000000000000000000LL;
				}
				else if(asec > 0 && sec < 0)
				{
					sec++;
					asec -= 1000000000000000000LL;
				}
			}

			TTime&	TTime::operator+=	(const TTime op)
			{
				sec += op.sec;
				asec += op.asec;

				Normalize();

				return *this;
			}

			TTime&	TTime::operator-=	(const TTime op)
			{
				sec -= op.sec;
				asec -= op.asec;

				Normalize();

				return *this;
			}

			TTime	TTime::operator+	(const TTime op) const
			{
				return TTime(*this) += op;
			}

			TTime	TTime::operator-	(const TTime op) const
			{
				return TTime(*this) -= op;
			}

			TTime&	TTime::operator*=	(const double f)
			{
				this->sec *= f;
				this->asec *= f;
				Normalize();
				return *this;
			}

			TTime&	TTime::operator/=	(const double f)
			{
				this->asec += fmod(this->sec, f) * 1000000000000000000LL;
				this->asec /= f;
				this->sec /= f;
				Normalize();
				return *this;
			}

			TTime	TTime::operator*	(const double f) const
			{
				return TTime(*this) *= f;
			}

			TTime	TTime::operator/	(const double f) const
			{
				return TTime(*this) /= f;
			}

			bool	TTime::operator>	(const TTime op) const
			{
				if(sec > op.sec)
					return true;
				if(sec < op.sec)
					return false;
				return asec > op.asec;
			}

			bool	TTime::operator<	(const TTime op) const
			{
				if(sec < op.sec)
					return true;
				if(sec > op.sec)
					return false;
				return asec < op.asec;
			}

			bool	TTime::operator>=	(const TTime op) const
			{
				if(sec > op.sec)
					return true;
				if(sec < op.sec)
					return false;
				return asec >= op.asec;
			}

			bool	TTime::operator<=	(const TTime op) const
			{
				if(sec < op.sec)
					return true;
				if(sec > op.sec)
					return false;
				return asec <= op.asec;
			}

			bool	TTime::operator==	(const TTime op) const
			{
				return sec == op.sec && asec == op.asec;
			}

			bool	TTime::operator!=	(const TTime op) const
			{
				return sec != op.sec || asec != op.asec;
			}

			s64_t		TTime::ConvertToI	(EUnit cunit_) const
			{
				const int cunit = (int)cunit_;
				if(cunit < 0)
				{
					s64_t mul = 1LL;
					s64_t div = 1000000000000000000LL;

					for(int i = 0; i > cunit; i--)
					{
						mul *= 1000LL;
						div /= 1000LL;
					}

					return sec * mul + asec / div;
				}
				else
					return sec / cunit;
			}

			double	TTime::ConvertToF	(EUnit cunit_) const
			{
				const int cunit = (int)cunit_;
				if(cunit < 0)
				{
					double mul = 1.0;
					double div = 1.0/1000000000000000000.0;

					for(int i = 0; i > cunit; i--)
					{
						mul *= 1000.0;
						div *= 1000.0;
					}

					return (double)sec * mul + (double)asec * div;
				}
				else
					return ((double)sec + (double)asec / 1000000000000000000.0) / (double)cunit;
			}

			TTime	TTime::ConvertFrom	(EUnit cunit_, s64_t value)
			{
				const int cunit = (int)cunit_;
				if(cunit < 0)
				{
					s64_t div = 1LL;
					s64_t mul = 1000000000000000000LL;

					for(int i = 0; i > cunit; i--)
					{
						div *= 1000LL;
						mul /= 1000LL;
					}

					const s64_t sec = value / div;
					value -= sec * div;

					return TTime((s64_t)sec, (s64_t)(value * mul));
				}
				else
					return TTime(value * cunit, (s64_t)0);
			}

			TTime	TTime::ConvertFrom	(EUnit cunit_, double value)
			{
				const int cunit = (int)cunit_;
				if(cunit < 0)
				{
					double div = 1.0;
					double mul = 1.0/1000000000000000000.0;

					for(int i = 0; i > cunit; i--)
					{
						div *= 1000.0;
						mul *= 1000.0;
					}

					s64_t sec = (s64_t)(value / div);
					value -= sec * div;

					return TTime(sec, (s64_t)(value / mul));
				}
				else
				{
					value *= (double)cunit;

					s64_t sec = (s64_t)value;
					value -= sec;

					return TTime(sec, (s64_t)(value * 1000000000000000000.0));
				}
			}

			s64_t	TTime::ConvertToI	(s64_t  tps) const
			{
				return this->sec * tps + this->asec / (1000000000000000000LL / tps);
			}

			double	TTime::ConvertToF	(double tps) const
			{
				return this->sec * tps + this->asec / (1000000000000000000LL / tps);
			}

			TTime	TTime::ConvertFrom	(s64_t  tps, s64_t value)
			{
				const s64_t sec = value / tps;
				value -= sec * tps;	// compute fractional part
				value *= 1000000000000000000ULL / tps;	// ticks per attosecond
				return TTime(sec, value);
			}

			TTime	TTime::ConvertFrom	(double tps, double value)
			{
				return TTime(value / tps);
			}

			s64_t		TTime::UnixTimeI		() const
			{
				return sec;
			}

			double	TTime::UnixTimeF		() const
			{
				return (double)(sec) + ((double)asec) / 1000000000000000000.0;
			}

			TTime	TTime::UnixTime		(double unixtime)
			{
				s64_t sec = (s64_t)unixtime;
				unixtime -= sec;
				unixtime *= 1000000000000000000.0;
				s64_t asec = (s64_t)unixtime;

				return TTime(sec, asec);
			}

			TTime	TTime::UnixTime		(s64_t unixtime)
			{
				return TTime(unixtime, (s64_t)0);
			}

			TTime::operator timespec	() const
			{
				timespec t = { (time_t)(sec), (long)(asec / 1000000000LL) };
				return t;
			}

			TTime::operator timeval		() const
			{
				timeval t = { (time_t)(sec), (long)(asec / 1000000000000LL) };
				return t;
			}

			CLASS	TTime::TTime		(double Seconds)
			{
				sec = (s64_t)Seconds;
				Seconds -= sec;
				asec = (s64_t)(Seconds * 1000000000000000000.0);
			}

			CLASS	TTime::TTime		(s64_t seconds, s64_t attoseconds) : sec(seconds), asec(attoseconds)
			{
				Normalize();
			}

// 			CLASS	TTime::TTime		(io::serialization::IDeserializer& ds)
// 			{
// 				ds.Pop("seconds", this->sec);
// 				ds.Pop("attoseconds", this->asec);
// 			}

			CLASS	TTime::TTime		(struct timespec ts) : sec((s64_t)ts.tv_sec), asec((s64_t)ts.tv_nsec * (s64_t)1000000000LL)
			{
				Normalize();
			}

			CLASS	TTime::TTime		(struct timeval tv) : sec((s64_t)tv.tv_sec), asec((s64_t)tv.tv_usec * (s64_t)1000000000000LL)
			{
				Normalize();
			}

// 			void TTime::Serialize(io::serialization::ISerializer& s) const
// 			{
// 				s.Push("seconds", this->sec);
// 				s.Push("attoseconds", this->asec);
// 			}
//
// 			void TTime::Deserialize(io::serialization::IDeserializer& ds)
// 			{
// 				ds.Pop("seconds", this->sec);
// 				ds.Pop("attoseconds", this->asec);
// 			}
		}
	}
}
