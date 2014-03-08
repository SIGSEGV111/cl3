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

#include "io_text.hpp"
#include "error-base.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

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

			//	*** TUTF32 ***

			const TUTF32 TUTF32::TERMINATOR((u32)0);

			CLASS	TUTF32::TUTF32	(char chr)
			{
				if(chr < 128)
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

			ITextReader&	ITextReader::operator>>	(s8& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u8& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s16& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u16& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s32& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u32& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s64& v)
			{
				ParseInteger<true>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u64& v)
			{
				ParseInteger<false>(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f32& v)
			{
				ParseFloat(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f64& v)
			{
				ParseFloat(*this, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(string::IString&)
			{
				CL3_NOT_IMPLEMENTED;
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(token::ITokenizer&)
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

			ITextWriter&	ITextWriter::operator<<	(s8 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u8 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s16 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u16 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s32 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u32 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s64 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u64 v)
			{
				PrintInteger<true>(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f32 v)
			{
				PrintFloat(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f64 v)
			{
				PrintFloat(*this, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const string::IString&)
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
