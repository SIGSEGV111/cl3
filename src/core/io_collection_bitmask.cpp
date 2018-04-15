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

#include <cl3/core/system_memory.hpp>
//#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/io_collection_bitmask.hpp>
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

				void		TIterator::CheckIndex	() const
				{
					if(index >= bm->Count())
						index = (usys_t)-2;
				}

				const IStaticCollection<TBoolProxy>&	TIterator::Collection	() const
				{
					return *bm;
				}

				bool		TIterator::IsValid	() const
				{
					CheckIndex();
					return index != (usys_t)-1 && index != (usys_t)-2;
				}

				const TBoolProxy&	TIterator::Item		() const
				{
					CL3_CLASS_ERROR(!IsValid(), TIndexOutOfBoundsException, index, bm->Count());
					return *this;
				}

				void		TIterator::MoveHead	()
				{
					index = (usys_t)-1;
				}

				void		TIterator::MoveTail	()
				{
					index = (usys_t)-2;
				}

				bool		TIterator::MoveFirst()
				{
					CheckIndex();
					if(bm->Count() > 0)
					{
						index = 0;
						return true;
					}
					else
						return false;
				}

				bool		TIterator::MoveLast	()
				{
					CheckIndex();
					if(bm->Count() > 0)
					{
						index = bm->Count()-1;
						return true;
					}
					else
						return false;
				}

				bool		TIterator::MoveNext	()
				{
					CheckIndex();
					if(index < bm->Count()-1)
					{
						index++;
						return true;
					}
					else
						return false;
				}

				bool		TIterator::MovePrev	()
				{
					CheckIndex();
					if(index > 0)
					{
						index--;
						return true;
					}
					else
						return false;
				}

				TBoolProxy&	TIterator::Item		()
				{
					CL3_CLASS_ERROR(!IsValid(), TIndexOutOfBoundsException, index, bm->Count());
					return *this;
				}

				usys_t		TIterator::Write	(const TBoolProxy* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t		TIterator::Read		(TBoolProxy* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t		TIterator::Write	(const bool* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t		TIterator::Read		(bool* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t		TIterator::Index	() const
				{
					CheckIndex();
					return index;
				}

				void		TIterator::Index	(usys_t new_index)
				{
					CL3_CLASS_ERROR(new_index >= bm->Count(), TIndexOutOfBoundsException, new_index, bm->Count());
					index = new_index;
				}

				/******************************************************************/

				usys_t		TBitmask::Count		() const
				{
					return n_bits;
				}

				void		TBitmask::Count		(usys_t new_count)
				{
					arr_bits = (byte_t*)Realloc(arr_bits, new_count / 8U + (((new_count % 8U) != 0U) ? 1U : 0U), NULL, false);
					n_bits = new_count;
				}

				bool		TBitmask::Contains	(const TBoolProxy&) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				bool		TBitmask::Contains	(const bool) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				void		TBitmask::Bit		(usys_t index, bool value)
				{
					CL3_CLASS_ERROR(index >= n_bits, TIndexOutOfBoundsException, index, n_bits);
					const usys_t index_byte = index / 8U;
					const byte_t mask = 1U << (index % 8U);

					if(value)
						arr_bits[index_byte] |= mask;
					else
						arr_bits[index_byte] &= ~mask;
				}

				bool		TBitmask::Bit		(usys_t index)
				{
					CL3_CLASS_ERROR(index >= n_bits, TIndexOutOfBoundsException, index, n_bits);
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
					arr_bits = reinterpret_cast<byte_t*>(Alloc(other.n_bits / 8U + (((other.n_bits % 8U) != 0U) ? 1U : 0U), NULL));
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
