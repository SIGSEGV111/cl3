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

#ifndef	_include_cl3_core_math_softnum_hpp_
#define	_include_cl3_core_math_softnum_hpp_

#include "io_collection_list.hpp"
#include "system_types.hpp"

namespace cl3
{
	namespace math
	{
		namespace softnum
		{
			using namespace system::types;

			typedef u8_t digit_t;

			struct TSoftNum
			{
				io::collection::list::TList<digit_t> digits;	//	big-endian (lower index has higher value)
				usys_t
					base : sizeof(digit_t) * 9,
					negative : 1,
					idx_start_fractional;

				CL3PUBF io::collection::list::TList<digit_t> IntegerPart() const CL3_GETTER;
				CL3PUBF io::collection::list::TList<digit_t> FractionalPart() const CL3_GETTER;

				inline unsigned Base() const CL3_GETTER { return (unsigned)this->base; }
				CL3PUBF void Base(unsigned) CL3_SETTER;

				CL3PUBF explicit operator u64_t() const CL3_GETTER;
				CL3PUBF explicit operator s64_t() const CL3_GETTER;
				CL3PUBF explicit operator f64_t() const CL3_GETTER;

				CL3PUBF TSoftNum operator+() const CL3_GETTER;
				CL3PUBF TSoftNum operator-() const CL3_GETTER;
				CL3PUBF TSoftNum operator~() const CL3_GETTER;

				CL3PUBF TSoftNum& operator+=(const TSoftNum&);
				CL3PUBF TSoftNum& operator-=(const TSoftNum&);
				CL3PUBF TSoftNum& operator*=(const TSoftNum&);
				CL3PUBF TSoftNum& operator/=(const TSoftNum&);
				CL3PUBF TSoftNum& operator%=(const TSoftNum&);
				CL3PUBF TSoftNum& operator>>=(const TSoftNum&);
				CL3PUBF TSoftNum& operator<<=(const TSoftNum&);

				CL3PUBF TSoftNum  operator+ (const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator- (const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator* (const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator/ (const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator% (const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator>>(const TSoftNum&) CL3_GETTER;
				CL3PUBF TSoftNum  operator<<(const TSoftNum&) CL3_GETTER;

				CL3PUBF bool operator==(const TSoftNum&) const CL3_GETTER;
				CL3PUBF bool operator!=(const TSoftNum&) const CL3_GETTER;
				CL3PUBF bool operator>=(const TSoftNum&) const CL3_GETTER;
				CL3PUBF bool operator<=(const TSoftNum&) const CL3_GETTER;
				CL3PUBF bool operator> (const TSoftNum&) const CL3_GETTER;
				CL3PUBF bool operator< (const TSoftNum&) const CL3_GETTER;

				CL3PUBF TSoftNum& operator=(const TSoftNum&);
				CL3PUBF TSoftNum& operator=(TSoftNum&&);

				CL3PUBF CLASS TSoftNum(const TSoftNum&);
				CL3PUBF CLASS TSoftNum(const TSoftNum& value, unsigned base);
				CL3PUBF CLASS TSoftNum(TSoftNum&&);

				CL3PUBF CLASS TSoftNum(s8_t  value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(u8_t  value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(s16_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(u16_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(s32_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(u32_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(s64_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(u64_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(f32_t value, unsigned base = 256);
				CL3PUBF CLASS TSoftNum(f64_t value, unsigned base = 256);
			};
		}
	}
}

#endif
