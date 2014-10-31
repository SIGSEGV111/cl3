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
#include "io_text_string.hpp"
#include "io_text_parser.hpp"
#include "io_text_encoding.hpp"
#include "error.hpp"
#include "io_collection_array.hpp"
#include "io_collection_matcher.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	text
		{
			using namespace stream;
			using namespace string;
			using namespace collection::array;

			static const TUTF32 ARR_WHITESPACE_DEFAULT[] = { 0x0020U, 0x0009U, 0x000AU, 0x000CU, 0x000DU, 0x000BU };
			static const collection::array::TArray<const TUTF32> COLLECTION_WHITESPACE_DEFAULT(ARR_WHITESPACE_DEFAULT, sizeof(ARR_WHITESPACE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* whitespace = &COLLECTION_WHITESPACE_DEFAULT;

			const collection::IStaticCollection<const TUTF32>* eos_markers = &COLLECTION_WHITESPACE_DEFAULT;

			static const TUTF32 ARR_NEWLINE_DEFAULT[] = { 0x000AU };
			static const collection::array::TArray<const TUTF32> COLLECTION_NEWLINE_DEFAULT(ARR_NEWLINE_DEFAULT, sizeof(ARR_NEWLINE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* newline_markers = &COLLECTION_NEWLINE_DEFAULT;

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
				if(encoding::CODEC_CXX_WCHAR == encoding::CODEC_UTF32)
				{
					code = wchr;
				}
				else
				{
					if(wchr < 128)
						code = wchr;
					else
						CL3_NOT_IMPLEMENTED;
				}
			}

			CLASS	TTextFormat::TTextFormat	() :	eos_markers(text::eos_markers), /*max_string_length((usys_t)-1),*/ discard_eos_marker(true)
			{
			}

			//	*** ITextReader ***

// 			static const TUTF32 ARR_NUMBER_CHARACTERS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+' };

			template<bool b_signed, class T>
			static	void	ParseInteger	(IIn<TUTF32>& is, const TNumberFormat& format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			template<class T>
			static	void	ParseFloat		(IIn<TUTF32>& is, const TNumberFormat& format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			ITextReader&	ITextReader::operator>>	(char& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			ITextReader&	ITextReader::operator>>	(wchar_t& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			ITextReader&	ITextReader::operator>>	(TUTF32& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s8_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u8_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s16_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u16_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s32_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u32_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s64_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u64_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f32_t& v)
			{
				ParseFloat(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f64_t& v)
			{
				ParseFloat(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(string::TString& v)
			{

				parser::TTokenizer tokenizer(this, parser::MATCHTYPE_EXCLUDE, newline_markers);

				tokenizer.Next();
				v = system::def::move(tokenizer.CurrentToken());

				return *this;
			}

			//	*** ITextWriter ***

			//	FIXME honor TNumberFormat

			template<bool b_signed, class T>
			static	void	PrintInteger	(IOut<char>& os, T num)
			{
				char buffer[32] = {};
				int n;
				if(b_signed)
					n = snprintf(buffer, sizeof(buffer), "%lld", (signed long long)num);
				else
					n = snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)num);
				os.Write(buffer, n);
			}

			template<class T>
			static	void	PrintFloat		(IOut<char>& os, T num)
			{
				char buffer[64];
				os.Write(buffer, snprintf(buffer, sizeof(buffer), "%f", (double)num));
			}

			ITextWriter&	ITextWriter::operator<<	(char v)
			{
				Write(&v, 1);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(wchar_t v)
			{
				Write(&v, 1);
				return *this;
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

			ITextWriter&	ITextWriter::operator<<	(const string::TString& str)
			{
				this->Write(str.ItemPtr(0), str.Count());
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
