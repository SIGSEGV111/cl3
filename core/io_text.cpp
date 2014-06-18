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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "io_text.hpp"
#include "error-base.hpp"
#include "io_collection_array.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		//	DIGITS = DIGITS DIGIT | DIGIT
		//	NUMBER = [PLUS_SIGN | MINUS_SIGN] DIGITS
		namespace	text
		{
			using namespace stream;

			static const TUTF32 ARR_WHITESPACE_DEFAULT[] = { 0x0020U, 0x0009U, 0x000AU, 0x000CU, 0x000DU, 0x000BU };
			static const collection::array::TArray<const TUTF32> COLLECTION_WHITESPACE_DEFAULT(ARR_WHITESPACE_DEFAULT, sizeof(ARR_WHITESPACE_DEFAULT) / sizeof(TUTF32));
			const collection::IStaticCollection<const TUTF32>* COLLECTION_WHITESPACE = &COLLECTION_WHITESPACE_DEFAULT;

			//	*** TUTF32 ***

			const TUTF32 TUTF32::TERMINATOR((u32_t)0);

			CLASS	TUTF32::TUTF32	(char chr)
			{
				if((unsigned char)chr < 128)
					code = chr;
				else
					CL3_NOT_IMPLEMENTED;
			}

			CLASS	TUTF32::TUTF32	(wchar_t wchr)
			{
				if(wchr < 128)
					code = wchr;
				else
					CL3_NOT_IMPLEMENTED;
			}

			//	*** ITextReader ***

			static const TUTF32 ARR_NUMBER_CHARACTERS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+' };

			template<bool b_signed, class T>
			static	void	ParseInteger	(IIn<TUTF32>&, T&)
			{
			}

			template<class T>
			static	void	ParseFloat		(IIn<TUTF32>&, T&)
			{
			}

			ITextReader&	ITextReader::operator>>	(TUTF32& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s8_t& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u8_t& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s16_t& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u16_t& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s32_t& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u32_t& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s64_t& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u64_t& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f32_t& v)
			{
				ParseFloat(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f64_t& v)
			{
				ParseFloat(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(string::TString&)
			{
				CL3_NOT_IMPLEMENTED;
				return *this;
			}

			//	*** ITextWriter ***

			template<bool b_signed, class T>
			static	void	PrintInteger	(IOut<char>& os, T num)
			{
				char buffer[32];
				if(b_signed)
					os.Write(buffer, snprintf(buffer, sizeof(buffer), "%lld", (signed long long)num));
				else
					os.Write(buffer, snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)num));
			}

			template<class T>
			static	void	PrintFloat		(IOut<char>& os, T num)
			{
				char buffer[64];
				os.Write(buffer, snprintf(buffer, sizeof(buffer), "%f", (double)num));
			}

			ITextWriter&	ITextWriter::operator<<	(TUTF32 v)
			{
				Write(&v, 1);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s8_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u8_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s16_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u16_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s32_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u32_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s64_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u64_t v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f32_t v)
			{
				PrintFloat(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f64_t v)
			{
				PrintFloat(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const string::TString&)
			{
				CL3_NOT_IMPLEMENTED;
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const char* v)
			{
				Write(v, strlen(v));
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const wchar_t* v)
			{
				Write(v, wcslen(v));
				return *this;
			}
		}
	}
}
