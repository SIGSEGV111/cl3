/*
 *   libcl3 - common library version 3
 *   Copyright (C) 2013	Simon Brennecke
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cl3/core/system_random.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::random;

	static void TestRandomness(cl3::io::stream::IIn<byte_t>& rnd)
	{
		static const unsigned n = 15;

		byte_t a[n] = {};
		byte_t b[n] = {};
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 1024; i++)
		{
			rnd.Read(a, n);
			rnd.Read(b, n);
			if(memcmp(a, b, n) == 0)
				n_fail++;
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, CryptoRandom)
	{
		TestRandomness(CryptoRandom());
	}

	TEST(system_random, CMWC)
	{
		TCMWC cmwc;
		TestRandomness(cmwc);
	}

	TEST(system_random, Float_Interval)
	{
		TCMWC cmwc;
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 4096; i++)
		{
			const double f = cmwc.Float();
			if(f < 0 || f > 1)
				n_fail++;
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, Float_Randomness)
	{
		TCMWC cmwc;
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 1024; i++)
		{
			const double a = cmwc.Float();
			const double b = cmwc.Float();
			if(a == b)
				n_fail++;
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, Normal_Interval)
	{
		TCMWC cmwc;
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 4096; i++)
		{
			const double f = cmwc.Normal();
			if(f < 0 || f > 1)
			{
				printf("%f\n", f);
				n_fail++;
			}
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, Normal_Randomness)
	{
		TCMWC cmwc;
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 1024; i++)
		{
			const double a = cmwc.Normal();
			const double b = cmwc.Normal();
			if(a == b)
				n_fail++;
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, NormalIndex)
	{
		TCMWC cmwc;

		static const usys_t n = 16;
		u32_t hist[n] = {};

		for(u32_t i = 0; i < 0x10000; i++)
		{
			const usys_t index = cmwc.NormalIndex(n-1);
			if(index >= n)
			{
				FAIL();
				return;
			}
			hist[index]++;
		}

		for(usys_t i = 1; i < n; i++)
			EXPECT_TRUE(hist[i-1] > hist[i]);
	}
}
