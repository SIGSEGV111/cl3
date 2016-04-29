/*
 *    libcl3 - common library version 3
 *    Copyright (C) 2013	Simon Brennecke
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	_include_cl3_core_system_endian_hpp_
#define	_include_cl3_core_system_endian_hpp_

#include "system_types.hpp"

#define	CL3_ENDIANITY_LITTLE	1
#define	CL3_ENDIANITY_BIG		2

namespace cl3
{
	namespace system
	{
		namespace endian
		{
			using namespace system::types;

			//	there are only two endianness: BIG and LITTLE, and my bet is that in a few years from now there will only be one: LITTLE

			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wmultichar"
			static const bool IS_BIG_ENDIAN = '\x00\x00\x00\x01' == 1L;
			#pragma GCC diagnostic pop

			static const bool IS_LITTLE_ENDIAN = !IS_BIG_ENDIAN;

			namespace _
			{
				template<typename T>
				static inline T SwapEndianness(T value)
				{
					switch(sizeof(T))
					{
						case 1:
							return value;
						case 2:
							return	(value & 0xff00) >> 8 |
							(value & 0x00ff) << 8;
						case 4:
							return	(value & 0xff000000) >> 24 |
							(value & 0x00ff0000) >>  8 |
							(value & 0x0000ff00) <<  8 |
							(value & 0x000000ff) << 24;
						case 8:
							return	(value & 0xff00000000000000) >> 56 |
							(value & 0x00ff000000000000) >> 40 |
							(value & 0x0000ff0000000000) >> 24 |
							(value & 0x000000ff00000000) >>  8 |
							(value & 0x00000000ff000000) <<  8 |
							(value & 0x0000000000ff0000) << 24 |
							(value & 0x000000000000ff00) >> 40 |
							(value & 0x00000000000000ff) << 56;
					}
				}

				template<typename T>
				static inline T ConvertBigEndianToNative(T value)
				{
					if(IS_BIG_ENDIAN)
						return value;
					else
						return SwapEndianness(value);
				}

				template<typename T>
				static inline T ConvertLittleEndianToNative(T value)
				{
					if(IS_LITTLE_ENDIAN)
						return value;
					else
						return SwapEndianness(value);
				}

				template<typename T>
				static inline T ConvertNativeToBigEndian(T value)
				{
					if(IS_BIG_ENDIAN)
						return value;
					else
						return SwapEndianness(value);
				}

				template<typename T>
				static inline T ConvertNativeToLittleEndian(T value)
				{
					if(IS_LITTLE_ENDIAN)
						return value;
					else
						return SwapEndianness(value);
				}
			}

			static inline s8_t  ConvertBigEndianToNative(s8_t  value) { return _::ConvertBigEndianToNative(value); }
			static inline u8_t  ConvertBigEndianToNative(u8_t  value) { return _::ConvertBigEndianToNative(value); }
			static inline s16_t ConvertBigEndianToNative(s16_t value) { return _::ConvertBigEndianToNative(value); }
			static inline u16_t ConvertBigEndianToNative(u16_t value) { return _::ConvertBigEndianToNative(value); }
			static inline s32_t ConvertBigEndianToNative(s32_t value) { return _::ConvertBigEndianToNative(value); }
			static inline u32_t ConvertBigEndianToNative(u32_t value) { return _::ConvertBigEndianToNative(value); }
			static inline s64_t ConvertBigEndianToNative(s64_t value) { return _::ConvertBigEndianToNative(value); }
			static inline u64_t ConvertBigEndianToNative(u64_t value) { return _::ConvertBigEndianToNative(value); }
// 			static inline f32_t ConvertBigEndianToNative(f32_t value) { return _::ConvertBigEndianToNative(value); }
// 			static inline f64_t ConvertBigEndianToNative(f64_t value) { return _::ConvertBigEndianToNative(value); }

			static inline s8_t  ConvertLittleEndianToNative(s8_t  value) { return _::ConvertLittleEndianToNative(value); }
			static inline u8_t  ConvertLittleEndianToNative(u8_t  value) { return _::ConvertLittleEndianToNative(value); }
			static inline s16_t ConvertLittleEndianToNative(s16_t value) { return _::ConvertLittleEndianToNative(value); }
			static inline u16_t ConvertLittleEndianToNative(u16_t value) { return _::ConvertLittleEndianToNative(value); }
			static inline s32_t ConvertLittleEndianToNative(s32_t value) { return _::ConvertLittleEndianToNative(value); }
			static inline u32_t ConvertLittleEndianToNative(u32_t value) { return _::ConvertLittleEndianToNative(value); }
			static inline s64_t ConvertLittleEndianToNative(s64_t value) { return _::ConvertLittleEndianToNative(value); }
			static inline u64_t ConvertLittleEndianToNative(u64_t value) { return _::ConvertLittleEndianToNative(value); }
// 			static inline f32_t ConvertLittleEndianToNative(f32_t value) { return _::ConvertLittleEndianToNative(value); }
// 			static inline f64_t ConvertLittleEndianToNative(f64_t value) { return _::ConvertLittleEndianToNative(value); }

			static inline s8_t  ConvertNativeToBigEndian(s8_t  value) { return _::ConvertNativeToBigEndian(value); }
			static inline u8_t  ConvertNativeToBigEndian(u8_t  value) { return _::ConvertNativeToBigEndian(value); }
			static inline s16_t ConvertNativeToBigEndian(s16_t value) { return _::ConvertNativeToBigEndian(value); }
			static inline u16_t ConvertNativeToBigEndian(u16_t value) { return _::ConvertNativeToBigEndian(value); }
			static inline s32_t ConvertNativeToBigEndian(s32_t value) { return _::ConvertNativeToBigEndian(value); }
			static inline u32_t ConvertNativeToBigEndian(u32_t value) { return _::ConvertNativeToBigEndian(value); }
			static inline s64_t ConvertNativeToBigEndian(s64_t value) { return _::ConvertNativeToBigEndian(value); }
			static inline u64_t ConvertNativeToBigEndian(u64_t value) { return _::ConvertNativeToBigEndian(value); }
// 			static inline f32_t ConvertNativeToBigEndian(f32_t value) { return _::ConvertNativeToBigEndian(value); }
// 			static inline f64_t ConvertNativeToBigEndian(f64_t value) { return _::ConvertNativeToBigEndian(value); }

			static inline s8_t  ConvertNativeToLittleEndian(s8_t  value) { return _::ConvertNativeToLittleEndian(value); }
			static inline u8_t  ConvertNativeToLittleEndian(u8_t  value) { return _::ConvertNativeToLittleEndian(value); }
			static inline s16_t ConvertNativeToLittleEndian(s16_t value) { return _::ConvertNativeToLittleEndian(value); }
			static inline u16_t ConvertNativeToLittleEndian(u16_t value) { return _::ConvertNativeToLittleEndian(value); }
			static inline s32_t ConvertNativeToLittleEndian(s32_t value) { return _::ConvertNativeToLittleEndian(value); }
			static inline u32_t ConvertNativeToLittleEndian(u32_t value) { return _::ConvertNativeToLittleEndian(value); }
			static inline s64_t ConvertNativeToLittleEndian(s64_t value) { return _::ConvertNativeToLittleEndian(value); }
			static inline u64_t ConvertNativeToLittleEndian(u64_t value) { return _::ConvertNativeToLittleEndian(value); }
// 			static inline f32_t ConvertNativeToLittleEndian(f32_t value) { return _::ConvertNativeToLittleEndian(value); }
// 			static inline f64_t ConvertNativeToLittleEndian(f64_t value) { return _::ConvertNativeToLittleEndian(value); }
		}
	}
}

#endif
