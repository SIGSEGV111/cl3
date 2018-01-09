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

#ifndef	_include_cl3_core_time_hpp_
#define	_include_cl3_core_time_hpp_

#include "system_os.hpp"
#include "system_compiler.hpp"
#include "system_types.hpp"

#if (CL3_OS == CL3_OS_WINDOWS)	//	windows doesn't know about POSIX struct timespec and timeval....
struct timespec
{
	time_t tv_sec;        /* seconds */
	long   tv_nsec;       /* nanoseconds */
};
struct timeval
{
	time_t      tv_sec;     /* seconds */
	suseconds_t tv_usec;    /* microseconds */
};
#else
#include <sys/time.h>
#endif

namespace	cl3
{
	using namespace system::types;

	namespace io
	{
		namespace serialization
		{
			class ISerializer;
			class IDeserializer;
		}
	}

	namespace	system
	{
		namespace	time
		{
			enum class EUnit : int
			{
				ATTOSECONDS  = -6,
				FEMTOSECONDS = -5,
				PICOSECONDS  = -4,
				NANOSECONDS  = -3,
				MICROSECONDS = -2,
				MILLISECONDS = -1,
				SECONDS =     1,
				MINUTES =    60,
				HOURS   =  3600,
				DAYS    = 86400,
				//	no month, year, etc. because they are not constant
			};

			enum class EClock
			{
				TAI,			//	counted on earth by an monotonic atomic clock (https://en.wikipedia.org/wiki/International_Atomic_Time)
				REALTIME,		//	System-wide real-time clock (based on unixtime - *NOT* MONOTONIC !!!).
				MONOTONIC,		//	Clock that cannot be set and represents monotonic time since some unspecified (and possibly changing) starting point not subject to NTP/leap adjustments.
				PROCESS,		//	Process time is defined as the amount of CPU time consumed by a process.
				PROCESS_USER,	//	as above but only the portion spent in user-mode
				PROCESS_SYS,	//	as above but only the portion spent in kernel-mode
				THREAD,			//	Thread  time is defined as the amount of CPU time consumed by a (/ the calling) thread.
				THREAD_USER,	//	as above but only the portion spent iqn user-mode
				THREAD_SYS,		//	as above but only the portion spent in kernel-mode
			};

			class	CL3PUBT	TTime	//	always unixtime / UTC
			{
				private:
					void	Normalize	();

				protected:
					s64_t	sec;	//	seconds since 1970-01-01 00:00 +0000 (UTC)
					s64_t	asec;	//	atto-seconds (10^-18) of current second

				public:
					inline	s64_t	Seconds		() const { return sec; }
					inline	s64_t	Attoseconds	() const { return asec; }

					CL3PUBF	TTime&	operator+=	(const TTime op);
					CL3PUBF	TTime&	operator-=	(const TTime op);
					CL3PUBF	TTime	operator+	(const TTime op) const;
					CL3PUBF	TTime	operator-	(const TTime op) const;

					CL3PUBF	TTime&	operator*=	(const double f);
					CL3PUBF	TTime&	operator/=	(const double f);
					CL3PUBF	TTime	operator*	(const double f) const;
					CL3PUBF	TTime	operator/	(const double f) const;

					CL3PUBF	bool	operator>	(const TTime op) const;
					CL3PUBF	bool	operator<	(const TTime op) const;
					CL3PUBF	bool	operator>=	(const TTime op) const;
					CL3PUBF	bool	operator<=	(const TTime op) const;
					CL3PUBF	bool	operator==	(const TTime op) const;
					CL3PUBF	bool	operator!=	(const TTime op) const;

					CL3PUBF	s64_t			ConvertToI	(EUnit unit) const;
					CL3PUBF	double			ConvertToF	(EUnit unit) const;
					CL3PUBF	static TTime	ConvertFrom	(EUnit unit, s64_t value);
					CL3PUBF	static TTime	ConvertFrom	(EUnit unit, double value);

					CL3PUBF	static	TTime	Now			(EClock clock);
					CL3PUBF	s64_t			UnixTimeI	() const;
					CL3PUBF	double			UnixTimeF	() const;
					CL3PUBF	static	TTime	UnixTime	(double unixtime);
					CL3PUBF	static	TTime	UnixTime	(s64_t unixtime);
					CL3PUBF	operator timespec			() const;
					CL3PUBF	operator timeval			() const;

					inline	CLASS	TTime	() : sec(0), asec(0) {}
					CL3PUBF	CLASS	TTime	(double seconds);
					CL3PUBF	CLASS	TTime	(s64_t seconds, s64_t attoseconds);
					CL3PUBF	CLASS	TTime	(struct timespec ts);
					CL3PUBF	CLASS	TTime	(struct timeval tv);
					CL3PUBF	CLASS	TTime	(io::serialization::IDeserializer&);

					void Serialize(io::serialization::ISerializer&) const;
					void Deserialize(io::serialization::IDeserializer&);
			};
		}
	}
}

#endif
