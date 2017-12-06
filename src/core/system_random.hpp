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
			struct IRandomNumberGenerator : public virtual io::stream::IIn<byte_t>
			{
				CL3PUBF u32_t GetU32();
				CL3PUBF double Float();
				CL3PUBF double Normal();
				CL3PUBF double PositiveNormal();
				CL3PUBF usys_t NormalIndex(usys_t max);
			};

			class CL3PUBT TCryptoRandom : public virtual IRandomNumberGenerator, public virtual io::stream::fd::TFDStream
			{
				public:
					CL3PUBF CLASS TCryptoRandom();
			};

			CL3PUBF TCryptoRandom& CryptoRandom();

			class CL3PUBT TCMWC : public IRandomNumberGenerator
			{
				public:
					using io::stream::IIn<byte_t>::Read;
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

					CL3PUBF usys_t Read(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF CLASS TCMWC();
					CL3PUBF CLASS TCMWC(const TState& state);
			};
		}
	}
}

#endif
