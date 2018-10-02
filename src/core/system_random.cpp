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

#include <math.h>
#include "system_random.hpp"
#include "system_memory.hpp"

// from "extlib/gauss.cpp"
double gaussian_ziggurat(cl3::system::random::IRandomNumberGenerator& rng, double sigma);

namespace cl3
{
	namespace system
	{
		namespace random
		{
			bool  IRandomNumberGenerator::GenerateBool() { return (this->ReadT<u8_t>() % 2) == 0; }
			u8_t  IRandomNumberGenerator::GenerateU8 () { return this->ReadT<u8_t> (); }
			u16_t IRandomNumberGenerator::GenerateU16() { return this->ReadT<u16_t>(); }
			u32_t IRandomNumberGenerator::GenerateU32() { return this->ReadT<u32_t>(); }
			u64_t IRandomNumberGenerator::GenerateU64() { return this->ReadT<u64_t>(); }
			s8_t  IRandomNumberGenerator::GenerateS8 () { return this->ReadT<s8_t> (); }
			s16_t IRandomNumberGenerator::GenerateS16() { return this->ReadT<s16_t>(); }
			s32_t IRandomNumberGenerator::GenerateS32() { return this->ReadT<s32_t>(); }
			s64_t IRandomNumberGenerator::GenerateS64() { return this->ReadT<s64_t>(); }

			f64_t IRandomNumberGenerator::GenerateF64(bool only_positive)
			{
				const long double x = this->GenerateU64();
				const long double y = x / (long double)18446744073709551615ULL;
				if(only_positive)
					return y;
				else
					return (f64_t)(this->GenerateBool() ? y : -y);
			}

			double IRandomNumberGenerator::Normal()
			{
				for(;;)
				{
					const double x = ::gaussian_ziggurat(*this, 1) / 3.0;
					if(x >= -1 && x <= 1)
						return x;
				}
			}

			double IRandomNumberGenerator::PositiveNormal()
			{
				for(;;)
				{
					const double x = ::gaussian_ziggurat(*this, 1) / 3.0;
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

			static system::memory::TUniquePtr<TCryptoRandom> rnd_crypto_instance;

			TCryptoRandom& TCryptoRandom::SharedInstance()
			{
				if(rnd_crypto_instance.Object() == NULL)
					rnd_crypto_instance.AtomicSwap(NULL, system::memory::MakeUniquePtr(new TCryptoRandom()));
				return *rnd_crypto_instance.Object();
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
				TCryptoRandom::SharedInstance().Read((byte_t*)this, sizeof(TState));
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

			CL3PUBF CLASS TXorShift::TXorShift(u64_t x, u64_t y, u64_t z) : x(x), y(y), z(z)
			{
			}

			CL3PUBF CLASS TXorShift::TXorShift()
			{
				TCryptoRandom::SharedInstance().Read((byte_t*)&x, 8);
				TCryptoRandom::SharedInstance().Read((byte_t*)&y, 8);
				TCryptoRandom::SharedInstance().Read((byte_t*)&z, 8);
			}

			namespace _
			{
				static system::memory::TUniquePtr<TXorShift> rnd_xor_instance;

				template<>
				TXorShift& TImplPNRG<TXorShift>::SharedInstance()
				{
					if(rnd_xor_instance.Object() == NULL)
						rnd_xor_instance.AtomicSwap(NULL, system::memory::MakeUniquePtr(new TXorShift()));
					return *rnd_xor_instance.Object();
				}

				static system::memory::TUniquePtr<TCMWC> rnd_cmwc_instance;

				template<>
				TCMWC& TImplPNRG<TCMWC>::SharedInstance()
				{
					if(rnd_cmwc_instance.Object() == NULL)
						rnd_cmwc_instance.AtomicSwap(NULL, system::memory::MakeUniquePtr(new TCMWC()));
					return *rnd_cmwc_instance.Object();
				}
			}
		}
	}
}
