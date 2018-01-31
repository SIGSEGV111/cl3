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

#ifndef	_include_cl3_core_system_random_hpp_
#define	_include_cl3_core_system_random_hpp_

#include "io_stream.hpp"
#include "io_stream_fd.hpp"

namespace cl3
{
	namespace system
	{
		namespace random
		{
			struct CL3PUBT IRandomNumberGenerator : public virtual io::stream::IIn<byte_t>
			{
				protected:
					template<typename T>
					T ReadT()
					{
						T x;
						this->Read((byte_t*)&x, sizeof(x));
						return x;
					}

				public:
					CL3PUBF virtual bool  GenerateBool() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual u8_t  GenerateU8 () CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual u16_t GenerateU16() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual u32_t GenerateU32() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual u64_t GenerateU64() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual s8_t  GenerateS8 () CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual s16_t GenerateS16() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual s32_t GenerateS32() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual s64_t GenerateS64() CL3_WARN_UNUSED_RESULT;
					CL3PUBF virtual f64_t GenerateF64(bool only_positive) CL3_WARN_UNUSED_RESULT;

					CL3PUBF double Normal();
					CL3PUBF double PositiveNormal();
					CL3PUBF usys_t NormalIndex(usys_t max);

					CL3PUBF virtual ~IRandomNumberGenerator() {}
			};

			namespace _
			{
				template<typename G>
				class TImplPNRG : public IRandomNumberGenerator
				{
					protected:
						template<typename T>
						inline T GenerateT()
						{
							G* g = static_cast<G*>(this);
							T r = static_cast<T>(g->Generate());

							using F = decltype(g->Generate());

							const unsigned n = sizeof(T) / sizeof(F);
							for(unsigned i = 1; i < n; i++)
								((F*)&r)[i] = g->Generate();

							return r;
						}

					public:
						using io::stream::IIn<byte_t>::Read;

						CL3PUBF bool  GenerateBool() final override CL3_WARN_UNUSED_RESULT { return (this->GenerateT<u8_t>() % 2) == 0; }
						CL3PUBF u8_t  GenerateU8 () final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<u8_t> (); }
						CL3PUBF u16_t GenerateU16() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<u16_t>(); }
						CL3PUBF u32_t GenerateU32() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<u32_t>(); }
						CL3PUBF u64_t GenerateU64() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<u64_t>(); }
						CL3PUBF s8_t  GenerateS8 () final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<s8_t> (); }
						CL3PUBF s16_t GenerateS16() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<s16_t>(); }
						CL3PUBF s32_t GenerateS32() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<s32_t>(); }
						CL3PUBF s64_t GenerateS64() final override CL3_WARN_UNUSED_RESULT { return this->GenerateT<s64_t>(); }

						CL3PUBF f64_t GenerateF64(bool only_positive) final override CL3_WARN_UNUSED_RESULT
						{
							const f64_t x = this->GenerateU64();
							const f64_t y = x / (long double)18446744073709551615ULL;
							if(only_positive)
								return y;
							else
								return (f64_t)(this->GenerateBool() ? y : -y);
						}

						CL3PUBF usys_t Read(byte_t* arr_items_read, usys_t n_items_read_max, usys_t) final override
						{
							G* g = static_cast<G*>(this);
							using F = decltype(g->Generate());

							const usys_t n = n_items_read_max / sizeof(F);
							for(usys_t i = 0; i < n; i++)
								((F*)arr_items_read)[i] = g->Generate();

							const usys_t n_have = n * sizeof(F);
							const usys_t n_need = n_items_read_max - n_have;
							if(n_need > 0)
							{
								F v = g->Generate();
								for(usys_t i = 0; i < n_need; i++)
									arr_items_read[n_have + i] = (byte_t)(v >>= 8);
							}

							return n_items_read_max;
						}

						CL3PUBF static G& SharedInstance() CL3_GETTER;
				};
			}

			class CL3PUBT TCryptoRandom : public virtual IRandomNumberGenerator, public virtual io::stream::fd::TFDStream
			{
				public:
					CL3PUBF CLASS TCryptoRandom();
					CL3PUBF static TCryptoRandom& SharedInstance() CL3_GETTER;
			};


			class CL3PUBT TCMWC : public _::TImplPNRG<TCMWC>
			{
				public:
					static const u32_t PHI = 0x9e3779b9;

					struct	TState
					{
						u32_t q[4096];
						u32_t c;
						u32_t i;

						CL3PUBF u32_t Generate();
						CL3PUBF void  Seed(u32_t);
						CL3PUBF CLASS TState();
						CL3PUBF explicit TState(u32_t seed);
					};

					TState state;

					inline u32_t Generate() { return this->state.Generate(); }

					CL3PUBF CLASS TCMWC();
					CL3PUBF CLASS TCMWC(const TState& state);
			};

			class CL3PUBT TXorShift : public _::TImplPNRG<TXorShift>
			{
				public:
					u64_t x;
					u64_t y;
					u64_t z;

					inline u64_t Generate()
					{
						u64_t t;
						x ^= x << 16;
						x ^= x >> 5;
						x ^= x << 1;

						t = x;
						x = y;
						y = z;
						z = t ^ x ^ y;

						return z;
					}

					CL3PUBF CLASS TXorShift(u64_t x, u64_t y, u64_t z);
					CL3PUBF CLASS TXorShift();
			};
		}
	}
}

#endif
