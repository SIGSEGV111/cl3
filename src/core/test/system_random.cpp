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
		TestRandomness(TCryptoRandom::SharedInstance());
	}

	TEST(system_random, CMWC)
	{
		TestRandomness(TCMWC::SharedInstance());
	}

	TEST(system_random, XorShift)
	{
		TestRandomness(TXorShift::SharedInstance());
	}

	TEST(system_random, Float_Interval)
	{
		TCMWC& cmwc = TCMWC::SharedInstance();

		{
			unsigned n_fail = 0;
			for(unsigned i = 0; i < 4096; i++)
			{
				const double f = cmwc.GenerateF64(true);
				if(f < 0 || f > 1)
					n_fail++;
			}
			EXPECT_EQ(0, n_fail);
		}

		{
			unsigned n_fail = 0;
			unsigned n_neg = 0;
			unsigned n_pos = 0;
			const unsigned n_total = 4096;
			for(unsigned i = 0; i < n_total; i++)
			{
				const double f = cmwc.GenerateF64(false);
				if(f < -1 || f > 1)
					n_fail++;
				if(f < 0)
					n_neg++;
				if(f > 0)
					n_pos++;
			}
			EXPECT_EQ(0, n_fail);
			EXPECT_TRUE(n_neg >= n_total / 4);
			EXPECT_TRUE(n_pos >= n_total / 4);
		}
	}

	TEST(system_random, Float_Randomness)
	{
		TCMWC& cmwc = TCMWC::SharedInstance();
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 1024; i++)
		{
			const double a = cmwc.GenerateF64(false);
			const double b = cmwc.GenerateF64(false);
			if(a == b)
				n_fail++;
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, Normal_Interval)
	{
		TCMWC& cmwc = TCMWC::SharedInstance();
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 4096; i++)
		{
			const double f = cmwc.Normal();
			if(f < -1 || f > 1)
			{
				printf("%f\n", f);
				n_fail++;
			}
		}

		EXPECT_EQ(0, n_fail);
	}

	TEST(system_random, PositiveNormal_Interval)
	{
		TCMWC& cmwc = TCMWC::SharedInstance();
		unsigned n_fail = 0;

		for(unsigned i = 0; i < 4096; i++)
		{
			const double f = cmwc.PositiveNormal();
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
		TCMWC& cmwc = TCMWC::SharedInstance();
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
		TCMWC& cmwc = TCMWC::SharedInstance();

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
