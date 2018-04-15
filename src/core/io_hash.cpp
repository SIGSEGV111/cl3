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

#include <cl3/core/io_hash.hpp>

#include <string.h>

void hashlittle2(const void *key, size_t length, cl3::system::types::u32_t *pc, cl3::system::types::u32_t *pb);

namespace cl3
{
	namespace io
	{
		namespace hash
		{

			u64_t JenkinsHash(const byte_t* arr_items, usys_t n_items)
			{
				union
				{
					struct
					{
						u32_t l,h;
					};
					u64_t qw;
				} a;

				a.qw = 0;

				::hashlittle2(arr_items, n_items, &a.l, &a.h);

				return a.qw;
			}

			static const u32_t MURMUR_M = 0x5bd1e995;

			void	TMurMur32Hash::Flush()
			{
				//	flush is a no-op
			}

			usys_t	TMurMur32Hash::Write(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t)
			{
				const int r = 24;

				usys_t n_processed = 0;

				if(n_partial > 0 && n_items_write_max + n_partial >= 4)
				{
					const usys_t n_need = 4 - n_partial;

					memcpy(this->arr_partial + this->n_partial, arr_items_write, n_need);

					u32_t k;

					k  = arr_partial[0];
					k |= arr_partial[1] << 8;
					k |= arr_partial[2] << 16;
					k |= arr_partial[3] << 24;

					k *= MURMUR_M;
					k ^= k >> r;
					k *= MURMUR_M;

					this->h *= MURMUR_M;
					this->h ^= k;

					n_processed += n_need;
					arr_items_write += n_need;
					this->n_partial = 0;
				}

				while(n_items_write_max - n_processed >= 4)
				{
					u32_t k;

					k  = arr_items_write[0];
					k |= arr_items_write[1] << 8;
					k |= arr_items_write[2] << 16;
					k |= arr_items_write[3] << 24;

					k *= MURMUR_M;
					k ^= k >> r;
					k *= MURMUR_M;

					this->h *= MURMUR_M;
					this->h ^= k;

					n_processed += 4;
					arr_items_write += 4;
				}

				if(n_items_write_max - n_processed > 0)
				{
					//	append remaining bytes to arr_partial
					const usys_t n_unprocessed = n_items_write_max - n_processed;
					memcpy(this->arr_partial + this->n_partial, arr_items_write, n_unprocessed);
					this->n_partial += n_unprocessed;
				}

				return n_processed;
			}

			u32_t	TMurMur32Hash::Hash() const
			{
				u32_t h_local = this->h;

				switch(this->n_partial)
				{
					case 3: h_local ^= this->arr_partial[2] << 16; break;
					case 2: h_local ^= this->arr_partial[1] << 8; break;
					case 1: h_local ^= this->arr_partial[0]; break;
				   h_local *= MURMUR_M;
				};

				h_local ^= h_local >> 13;
				h_local *= MURMUR_M;
				h_local ^= h_local >> 15;

				return h_local;
			}

			CLASS	TMurMur32Hash::TMurMur32Hash(u32_t seed) : h(seed), n_partial(0)
			{
			}
		}
	}
}
