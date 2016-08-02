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

#include <cl3/core/system_endian.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::endian;

// 	static inline T ConvertBigEndianToNative(T value);
// 	static inline T ConvertLittleEndianToNative(T value);
// 	static inline T ConvertNativeToBigEndian(T value);
// 	static inline T ConvertNativeToLittleEndian(T value);

	TEST(system_endian, ConvertBigEndianToNative)
	{
		{
			const s8_t& big = *(const s8_t*)"\xAA";
			const s8_t c = ConvertBigEndianToNative(big);
			EXPECT_EQ((s8_t)0xAA, c);
		}

		{
			const s16_t& big = *(const s16_t*)"\xAA\xBB";
			const s16_t c = ConvertBigEndianToNative(big);
			EXPECT_EQ((s16_t)0xAABB, c);
		}

		{
			const s32_t& big = *(const s32_t*)"\xAA\xBB\xCC\xDD";
			const s32_t c = ConvertBigEndianToNative(big);
			EXPECT_EQ((s32_t)0xAABBCCDDL, c);
		}

		{
			const s64_t& big = *(const s64_t*)"\xAA\xBB\xCC\xDD\xEE\xFF\xCC\x00";
			const s64_t c = ConvertBigEndianToNative(big);
			EXPECT_EQ((s64_t)0xAABBCCDDEEFFCC00LL, c);
		}
	}
}
