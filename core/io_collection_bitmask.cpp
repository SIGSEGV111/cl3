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

#include "system_memory.hpp"
#include "system_types_typeinfo.hpp"
#include "io_collection_bitmask.hpp"
#include <string.h>

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	bitmask
			{
				using namespace system::memory;
				using namespace system::types;

				usys_t		TBitmask::Count		() const
				{
					return n_bits;
				}

				void		TBitmask::Count		(usys_t new_count)
				{
					arr_bits = Realloc<byte_t>(arr_bits, new_count / 8U + (((new_count % 8U) != 0U) ? 1U : 0U));
					n_bits = new_count;
				}

				void		TBitmask::Bit		(usys_t index, bool value)
				{
					const usys_t index_byte = index / 8U;
					const byte_t mask = 1U << (index % 8U);

					if(value)
						arr_bits[index_byte] |= mask;
					else
						arr_bits[index_byte] &= ~mask;
				}

				bool		TBitmask::Bit		(usys_t index)
				{
					const usys_t index_byte = index / 8U;
					const byte_t mask = 1U << (index % 8U);

					return (arr_bits[index_byte] & mask) != 0U;
				}

				TBitmask&	TBitmask::operator=	(const TBitmask& rhs)
				{
					Count(rhs.n_bits);
					memcpy(arr_bits, rhs.arr_bits, n_bits / 8U + (((n_bits % 8U) != 0U) ? 1U : 0U));
					return *this;
				}

				CLASS		TBitmask::TBitmask	() : arr_bits(NULL), n_bits(0U)
				{
				}

				CLASS		TBitmask::TBitmask	(const TBitmask& other)
				{
					arr_bits = reinterpret_cast<byte_t*>(Alloc<byte_t>(other.n_bits / 8U + (((other.n_bits % 8U) != 0U) ? 1U : 0U)));
					n_bits = other.n_bits;
				}

				CLASS		TBitmask::~TBitmask	()
				{
					Free(arr_bits);
				}
			}
		}
	}
}
