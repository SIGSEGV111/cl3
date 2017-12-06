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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include <math.h>
#include "system_random.hpp"

// from "extlib/gauss.cpp"
double gaussian_ziggurat(cl3::system::random::IRandomNumberGenerator& rng, double sigma);

namespace cl3
{
	namespace system
	{
		namespace random
		{
			u32_t IRandomNumberGenerator::GetU32()
			{
				u32_t u;
				this->Read((byte_t*)&u, 4);
				return u;
			}

			f64_t IRandomNumberGenerator::Float()
			{
				u64_t x;
				this->Read((byte_t*)&x, 8);
				return (f64_t)x / (f64_t)((u64_t)-1);
			}

			double IRandomNumberGenerator::Normal()
			{
				for(;;)
				{
					const double x = ::gaussian_ziggurat(*this, 1.67332005307) / 6.0;
					if(x >= -1 && x <= 1)
						return x;
				}
			}

			double IRandomNumberGenerator::PositiveNormal()
			{
				for(;;)
				{
					const double x = ::gaussian_ziggurat(*this, 1.67332005307) / 6.0;
					const double y = CL3_ABS(x);
					if(y >= 0 && y <= 1)
						return y;
				}
			}

			usys_t IRandomNumberGenerator::NormalIndex(usys_t max)
			{
				for(;;)
				{
					const double v = this->PositiveNormal() * (max + 1);
					const usys_t i = (usys_t)v;
					if(i <= max)
						return i;
				}
			}

			TCryptoRandom& CryptoRandom()
			{
				static TCryptoRandom crng;
				return crng;
			}

			usys_t TCMWC::Read(byte_t* arr_items_read, usys_t n_items_read_max, usys_t)
			{
				byte_t* p = arr_items_read;
				byte_t* e = arr_items_read + (n_items_read_max / 4) * 4;

				for(; p < e; p += 4)
					*(u32_t*)p = this->state.Generate();

				if(n_items_read_max % 4)
				{
					e = arr_items_read + n_items_read_max;
					u32_t r = this->state.Generate();

					for(; p < e; p++)
						*p = r >> 8;
				}

				return n_items_read_max;
			}

			u32_t TCMWC::TState::Generate()
			{
				u64_t t, a = 18782LL;
				u32_t x, r = 0xfffffffe;
				i = (i + 1) & 4095;
				t = a * q[i] + c;
				c = (t >> 32);
				x = ((u32_t)t) + c;
				if (x < c)
				{
					x++;
					c++;
				}
				return (q[i] = r - x);
			}

			void TCMWC::TState::Seed(u32_t seed)
			{
				q[0] = seed;
				q[1] = seed + PHI;
				q[2] = seed + PHI + PHI;

				for(unsigned j = 3; j < 4096; j++)
					q[j] = q[j - 3] ^ q[j - 2] ^ PHI ^ j;

				c = 18781;
				i = 4095;

				for(unsigned i = 0; i < 4096; i++) Generate();
			}

			CLASS TCMWC::TState::TState()
			{
				CryptoRandom().Read((byte_t*)this, sizeof(TState));
				for(unsigned i = 0; i < 4096; i++) Generate();
			}

			CLASS TCMWC::TState::TState(u32_t seed)
			{
				this->Seed(seed);
			}

			CLASS TCMWC::TCMWC()
			{
			}

			CLASS TCMWC::TCMWC(const TState& state) : state(state)
			{
			}
		}
	}
}
