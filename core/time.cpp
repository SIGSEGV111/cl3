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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "time.hpp"

namespace cl3
{
	namespace	time
	{
		int timezone;

		void	TTime::Normalize	()
		{
			if(asec <= -1000000000000000000LL)
			{
				const s64 f = asec / 1000000000000000000LL;
				sec -= f;
				asec += f * 1000000000000000000LL;
			}
			else if(asec >= 1000000000000000000LL)
			{
				const s64 f = asec / 1000000000000000000LL;
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

		s64		TTime::ConvertToI	(EUnit cunit) const
		{
			if(cunit < 0)
			{
				s64 mul = 1LL;
				s64 div = 1000000000000000000LL;

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

		double	TTime::ConvertToF	(EUnit cunit) const
		{
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

		TTime	TTime::ConvertFrom	(EUnit cunit, s64 value)
		{
			if(cunit < 0)
			{
				s64 div = 1LL;
				s64 mul = 1000000000000000000LL;

				for(int i = 0; i > cunit; i--)
				{
					div *= 1000LL;
					mul /= 1000LL;
				}

				s64 sec = value / div;
				value -= sec * div;

				return TTime(sec, value * mul);
			}
			else
				return TTime(value * -cunit, 0LL);
		}

		TTime	TTime::ConvertFrom	(EUnit cunit, double value)
		{
			if(cunit < 0)
			{
				double div = 1.0;
				double mul = 1.0/1000000000000000000.0;

				for(int i = 0; i > cunit; i--)
				{
					div *= 1000.0;
					mul *= 1000.0;
				}

				s64 sec = (s64)(value / div);
				value -= sec * div;

				return TTime(sec, (s64)(value / mul));
			}
			else
			{
				value *= (double)cunit;

				s64 sec = (s64)value;
				value -= sec;

				return TTime(sec, (s64)(value * 1000000000000000000.0));
			}
		}

		s64		TTime::UnixTimeI		() const
		{
			return sec;
		}

		double	TTime::UnixTimeF		() const
		{
			return (double)(sec) + ((double)asec) / 1000000000000000000.0;
		}

		TTime	TTime::UnixTime		(double unixtime)
		{
			s64 sec = (s64)unixtime;
			s64 asec = (s64)((unixtime - (double)sec) * 1000000000000000000.0);

			return TTime(sec, asec);
		}

		TTime	TTime::UnixTime		(s64 unixtime)
		{
			return TTime(unixtime, 0);
		}

		TTime	TTime::LocalTime	() const
		{
			return TTime(sec - timezone, asec);
		}

		TTime::operator timespec	() const
		{
			timespec t = { (time_t)(sec), (long)(asec / 1000000000LL) };
			return t;
		}

		CLASS	TTime::TTime		(double Seconds)
		{
			sec = (s64)Seconds;
			Seconds -= sec;
			asec = (s64)(Seconds * 1000000000000000000.0);
		}

		CLASS	TTime::TTime		(s64 Seconds, s64 Attoseconds) : sec(Seconds), asec(Attoseconds)
		{
			Normalize();
		}
	};
};
