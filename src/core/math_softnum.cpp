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

#include <string.h>
#include "error.hpp"
#include "system_endian.hpp"
#include "math_softnum.hpp"

namespace cl3
{
	namespace math
	{
		namespace softnum
		{
			using namespace error;
			using namespace system::endian;
			using namespace io::collection::list;

			TList<digit_t> TSoftNum::IntegerPart() const
			{
				if(this->idx_start_fractional == (usys_t)-1)
					return this->digits;
				else
					return this->digits.SliceSL(0, this->idx_start_fractional);
			}

			TList<digit_t> TSoftNum::FractionalPart() const
			{
				if(this->idx_start_fractional == (usys_t)-1)
					return TList<digit_t>();
				else
					return this->digits.SliceSL(this->idx_start_fractional);
			}

			void TSoftNum::Base(unsigned new_base)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum::operator u64_t() const
			{
				TSoftNum b(*this, 256);
				u64_t r = 0;
				const usys_t n_copy = CL3_MIN(b.digits.Count(), sizeof(u64_t));
				memcpy(&r, b.digits.ItemPtr(b.digits.Count() - n_copy), n_copy);
				return ConvertBigEndianToNative(r);
			}

			TSoftNum::operator s64_t() const
			{
				TSoftNum b(*this, 256);
				s64_t r = 0;
				const usys_t n_copy = CL3_MIN(b.digits.Count(), sizeof(u64_t));
				memcpy(&r, b.digits.ItemPtr(b.digits.Count() - n_copy), n_copy);
				r = ConvertBigEndianToNative(r);
				if(this->negative)
					r = -r;
				return r;
			}

			TSoftNum::operator f64_t() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum TSoftNum::operator+() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum TSoftNum::operator-() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum TSoftNum::operator~() const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator+=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator-=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator*=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator/=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator%=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator>>=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator<<=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator+ (const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator- (const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator* (const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator/ (const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator% (const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator>>(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum  TSoftNum::operator<<(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator==(const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator!=(const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator>=(const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator<=(const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator> (const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			bool TSoftNum::operator< (const TSoftNum&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator=(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TSoftNum& TSoftNum::operator=(TSoftNum&& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS TSoftNum::TSoftNum(const TSoftNum& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS TSoftNum::TSoftNum(const TSoftNum& other, unsigned base)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS TSoftNum::TSoftNum(TSoftNum&& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			static void SetBinaryInt(TList<digit_t>& digits, u64_t value)
			{
				value = ConvertNativeToBigEndian(value);
				digits.Clear();
				digits.Append(reinterpret_cast<const digit_t*>(&value), sizeof(value) / sizeof(digit_t));
			}

			static void SetBinaryFloat(TList<digit_t>& digits, f64_t value)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS TSoftNum::TSoftNum(s8_t  value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value & 0x7F);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(u8_t  value, unsigned base)
			{
				this->base = 256;
				this->negative = 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(s16_t value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value & 0x7FFF);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(u16_t value, unsigned base)
			{
				this->base = 256;
				this->negative = 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(s32_t value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value & 0x7FFFFFFF);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(u32_t value, unsigned base)
			{
				this->base = 256;
				this->negative = 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(s64_t value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value & 0x7FFFFFFFFFFFFFFF);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(u64_t value, unsigned base)
			{
				this->base = 256;
				this->negative = 0;
				this->idx_start_fractional = 0;
				SetBinaryInt(this->digits, value);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(f32_t value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryFloat(this->digits, value);
				this->Base(base);
			}

			CLASS TSoftNum::TSoftNum(f64_t value, unsigned base)
			{
				this->base = 256;
				this->negative = value < 0 ? 1 : 0;
				this->idx_start_fractional = 0;
				SetBinaryFloat(this->digits, value);
				this->Base(base);
			}
		}
	}
}
