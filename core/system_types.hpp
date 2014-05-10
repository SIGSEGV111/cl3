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

#ifndef	_include_cl3_core_system_types_hpp_
#define	_include_cl3_core_system_types_hpp_

#include "system_compiler.hpp"
#include "system_os.hpp"

#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG)
	#include <stdint.h>

	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				typedef	uint8_t		u8_t;
				typedef	int8_t		s8_t;
				typedef	uint16_t	u16_t;
				typedef	int16_t		s16_t;
				typedef	uint32_t	u32_t;
				typedef	int32_t		s32_t;
				typedef	uint64_t	u64_t;
				typedef	int64_t		s64_t;

				typedef	float		f32_t;
				typedef	double		f64_t;

				typedef u8_t		byte_t;

				#if (__SIZEOF_SIZE_T__ == 4)
					typedef	u32_t	usys_t;
					typedef	s32_t	ssys_t;
				#elif (__SIZEOF_SIZE_T__ == 8)
					typedef	u64_t	usys_t;
					typedef	s64_t	ssys_t;
				#else
					#error "unknown wordsize"
				#endif

				typedef s64_t	soff_t;
				typedef u64_t	uoff_t;

				#if (CL3_OS == CL3_OS_WINDOWS)
					typedef void* fd_t;	//	HANDLE
				#elif (CL3_OS == CL3_OS_POSIX)
					typedef int fd_t;	//	fd
				#endif
			};
		};
	};
#elif (CL3_CXX == CL3_CXX_MSVC)
	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				#if (_MSC_VER < 1300)
					typedef	unsigned char		u8_t;
					typedef	signed char			s8_t;
					typedef	unsigned short		u16_t;
					typedef	signed short		s16_t;
					typedef	unsigned int		u32_t;
					typedef	signed int			s32_t;
					typedef	unsigned long long	u64_t;
					typedef	signed long long	s64_t;
				#else
					typedef	unsigned __int8		u8_t;
					typedef	signed __int8		s8_t;
					typedef	unsigned __int16	u16_t;
					typedef	signed __int16		s16_t;
					typedef	unsigned __int32	u32_t;
					typedef	signed __int32		s32_t;
					typedef	unsigned __int64	u64_t;
					typedef	signed __int64		s64_t;
				#endif

				typedef	float	f32_t;
				typedef	double	f64_t;

				typedef u8_t byte_t;

				#ifdef _WIN64
					typedef s64	ssys_t;
					typedef u64	usys_t;
				#else
					typedef s32	ssys_t;
					typedef u32	usys_t;
				#endif

				typedef s64_t	soff_t;
				typedef u64_t	uoff_t;
			};
		};
	};
#endif
#endif
