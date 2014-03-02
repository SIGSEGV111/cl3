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

#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_LLVM)
	#include <stdint.h>

	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				typedef	uint8_t				u8;
				typedef	int8_t				s8;

				typedef	uint16_t			u16;
				typedef	int16_t				s16;

				typedef	uint32_t			u32;
				typedef	int32_t				s32;

				typedef	uint64_t			u64;
				typedef	int64_t				s64;

					//typedef	signed __int128		s128;
					//typedef	unsigned __int128	u128;

				typedef	float				f32;
				typedef	double				f64;
					//typedef	__float128			f128;

				typedef	uintptr_t			us;
				typedef	intptr_t			ss;

				typedef	signed int			sf;
				typedef	unsigned int		uf;

				typedef u8 byte;
			};
		};
	};
#endif

#if (CL3_CXX == CL3_CXX_MSVC)
	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				#if (_MSC_VER < 1300)
					typedef	unsigned char		u8;
					typedef	signed char			s8;
					typedef	unsigned short		u16;
					typedef	signed short		s16;
					typedef	unsigned int		u32;
					typedef	signed int			s32;
					typedef	unsigned long long	u64;
					typedef	signed long long	s64;
				#else
					typedef	unsigned __int8		u8;
					typedef	signed __int8		s8;
					typedef	unsigned __int16	u16;
					typedef	signed __int16		s16;
					typedef	unsigned __int32	u32;
					typedef	signed __int32		s32;
					typedef	unsigned __int64	u64;
					typedef	signed __int64		s64;
				#endif

				#ifdef _WIN64
					typedef s64			ss;
					typedef u64			us;
				#else
					typedef s32			ss;
					typedef u32			us;
				#endif

				typedef	float			f32;
				typedef	double			f64;

				typedef	signed int		sf;
				typedef	unsigned int	uf;

				typedef u8 byte;
			};
		};
	};
#endif
#endif
