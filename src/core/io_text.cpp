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

			/******************************** Constants and Variables ******************************************************/

			static const TUTF32 ARR_WHITESPACE_DEFAULT[] = { 0x0020U, 0x0009U, 0x000AU, 0x000CU, 0x000DU, 0x000BU };
			static const collection::array::TArray<const TUTF32> COLLECTION_WHITESPACE_DEFAULT(ARR_WHITESPACE_DEFAULT, sizeof(ARR_WHITESPACE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* whitespace = &COLLECTION_WHITESPACE_DEFAULT;

			const collection::IStaticCollection<const TUTF32>* eos_markers = &COLLECTION_WHITESPACE_DEFAULT;

			static const TUTF32 ARR_NEWLINE_DEFAULT[] = { 0x000AU };
			static const collection::array::TArray<const TUTF32> COLLECTION_NEWLINE_DEFAULT(ARR_NEWLINE_DEFAULT, sizeof(ARR_NEWLINE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* newline_markers = &COLLECTION_NEWLINE_DEFAULT;

			static const TUTF32 ARR_OCTAL_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
			static const collection::array::TArray<const TUTF32> COLLECTION_OCTAL_DIGITS(ARR_OCTAL_DIGITS, sizeof(ARR_OCTAL_DIGITS) / sizeof(TUTF32), false);

			static const TUTF32 ARR_DECIMAL_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
			static const collection::array::TArray<const TUTF32> COLLECTION_DECIMAL_DIGITS(ARR_DECIMAL_DIGITS, sizeof(ARR_DECIMAL_DIGITS) / sizeof(TUTF32), false);

			static const TUTF32 ARR_HEX_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
			static const collection::array::TArray<const TUTF32> COLLECTION_HEX_DIGITS(ARR_HEX_DIGITS, sizeof(ARR_HEX_DIGITS) / sizeof(TUTF32), false);

			CL3_CONTEXT_VARIABLE_IMPL(const TCollationSequence*, default_collation_sequence, NULL);

			/******************************** TUTF32 ******************************************************/

			const TUTF32 TUTF32::TERMINATOR((u32_t)0);

			usys_t	TUTF32::strlen	(const TUTF32* str)
			{
				const TUTF32* p = str;
				for(; p->code != 0; p++);
				return p - str;
			}

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

			/******************************** TNumberFormat ******************************************************/

			CLASS	TNumberFormat::TNumberFormat	() : prefix_sign(NULL), prefix_digits(NULL), postfix_digits(NULL), postfix_sign(NULL), digits(&COLLECTION_DECIMAL_DIGITS), positive_mark(TUTF32::TERMINATOR), negative_mark('-'), zero_mark(TUTF32::TERMINATOR), decimal_mark('.'), grouping_mark(TUTF32::TERMINATOR), integer_padding(TUTF32::TERMINATOR), fractional_padding(TUTF32::TERMINATOR), exponential_mark('e'), positive_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), negative_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), zero_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), grouping_length(0), integer_length_min(0), integer_length_max((u16_t)-1), fractional_length_min(0), fractional_length_max((u16_t)-1), total_length_min(0), total_length_max((u16_t)-1)
			{
			}

			static	TNumberFormat	MakeStdHexNumberFormat	()
			{
				TNumberFormat format;
				format.digits = &COLLECTION_HEX_DIGITS;
				return format;
			}

			const TNumberFormat TNumberFormat::DECIMAL_SIMPLE;
			const TNumberFormat TNumberFormat::HEX = MakeStdHexNumberFormat();
			const TNumberFormat* TNumberFormat::default_format = &TNumberFormat::DECIMAL_SIMPLE;

			/******************************** TTextIOCommon ******************************************************/

			CLASS	TTextIOCommon::TTextIOCommon	() : text_format(NULL), number_format(NULL) {}

			/******************************** ITextReader ******************************************************/

			template<bool b_signed, class T>
			static	void	ParseInteger	(IIn<TUTF32>& is, const TNumberFormat* format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			template<class T>
			static	void	ParseFloat		(IIn<TUTF32>& is, const TNumberFormat* format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			ITextReader&	ITextReader::operator>>	(char& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(wchar_t& v)
			{
				Read(&v, 1);
				return *this;
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

			/******************************** ITextWriter ******************************************************/

			static	void	MakeNumberHead	(TString& buffer, const TNumberFormat* format, bool sign, bool is_signed, bool is_zero)
			{
				if(format->prefix_sign != NULL)
					buffer<<*format->prefix_sign;

				if(is_zero && format->zero_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE && format->zero_mark != TUTF32::TERMINATOR)
					buffer<<format->zero_mark;
				else if(is_signed && !sign && format->negative_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE && format->negative_mark != TUTF32::TERMINATOR)
					buffer<<format->negative_mark;
				else if(is_signed && sign && format->positive_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE && format->positive_mark != TUTF32::TERMINATOR)
					buffer<<format->positive_mark;

				if(format->prefix_digits != NULL)
					buffer<<*format->prefix_digits;
			}

			static	void	MakeNumberTail	(TString& buffer, const TNumberFormat* format, bool sign, bool is_signed, bool is_zero)
			{
				if(format->postfix_digits != NULL)
					buffer<<*format->postfix_digits;

				if(is_zero && format->zero_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_AFTER && format->zero_mark != TUTF32::TERMINATOR)
					buffer<<format->zero_mark;
				else if(is_signed && !sign && format->negative_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_AFTER && format->negative_mark != TUTF32::TERMINATOR)
					buffer<<format->negative_mark;
				else if(is_signed && sign && format->positive_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_AFTER && format->positive_mark != TUTF32::TERMINATOR)
					buffer<<format->positive_mark;

				if(format->postfix_sign != NULL)
					buffer<<*format->postfix_sign;
			}

			static	void	PrintNumber	(IOut<TUTF32>& os, const TNumberFormat* format, u64_t num, bool sign, bool is_signed)
			{
				TString buffer;

				if(format == NULL)
					format = TNumberFormat::default_format;

				MakeNumberHead(buffer, format, sign, is_signed, num == 0);

				{
					TUTF32 arr_digits_out_tmp[64];
					const TUTF32* arr_digits = format->digits->ItemPtr(0);
					const usys_t n_digits = format->digits->Count();
					usys_t n = 0;
					u64_t v = num;

					if(v == 0)
					{
						arr_digits_out_tmp[63] = '0';
						n = 1;
					}
					else
						while(v != 0)
						{
							arr_digits_out_tmp[63 - n++] = arr_digits[v % n_digits];
							v /= n_digits;
						}

					if(n < format->integer_length_min && format->integer_padding != TUTF32::TERMINATOR)
						buffer.Pad(POSITION_TAIL, format->integer_length_min - n, format->integer_padding);

					n = CL3_MIN(n, format->integer_length_max);
					buffer.Append(arr_digits_out_tmp + 64 - n, n);
				}

				if(format->fractional_length_min > 0 && format->fractional_padding != TUTF32::TERMINATOR)
				{
					buffer<<format->decimal_mark;
					buffer.Pad(POSITION_TAIL, format->fractional_length_min, format->fractional_padding);
				}

				MakeNumberTail(buffer, format, sign, is_signed, num == 0);

				os.Write(buffer.ItemPtr(0), buffer.Count());
			}

			static	void	PrintNumber	(IOut<TUTF32>& os, const TNumberFormat* format, f64_t num)
			{
// 				TString buffer;
//
// 				if(format == NULL)
// 					format = TNumberFormat::default_format;
//
// 				union
// 				{
// 					struct
// 					{
// 						u64_t digits : 52;
// 						u64_t exponent : 11, sign : 1;
// 					};
// 					f64_t num;
// 				} wrapper;
// 				wrapper.num = num;
//
// 				MakeNumberHead(buffer, format, wrapper.sign, true, num == 0);
//
// 				printf("wrapper.num      = %f\n",   wrapper.num);
// 				printf("wrapper.digits   = %llu\n", (unsigned long long)wrapper.digits);
// 				printf("wrapper.exponent = %llu\n", (unsigned long long)wrapper.exponent);
// 				printf("wrapper.sign     = %llu\n", (unsigned long long)wrapper.sign);
//
// 				s16_t e = (s16_t)wrapper.exponent - 1023;
// 				u64_t m = wrapper.digits | 0x10000000000000;
//
// // m => 53bits
// // 10010011001100110011001011111111111111111111110110011 m (expected)
// // 10010011001100110011001011111111111111111111110110011 m (actual)
// // 00000111111111111111111111111111111111111111111111111 mask (expected)
// // 00000111111111111111111111111111111111111111111111111 mask (actual)
//
//
// 				u64_t mask = (0x1FFFFFFFFFFFFF >> (e+1));
//
// 				u64_t f = m & mask;
// 				u64_t i = (m & ~mask) >> (52-e);
//
// 				printf("f = %llu\n", (unsigned long long)f);
// 				printf("i = %llu\n", (unsigned long long)i);
//
// 				PrintNumber(buffer, &TNumberFormat::DECIMAL_SIMPLE, i, true, false);
//
// 				MakeNumberTail(buffer, format, wrapper.sign, true, num == 0);
// 				os.Write(buffer.ItemPtr(0), buffer.Count());

				TString str;

				if(format == NULL)
					format = TNumberFormat::default_format;

				char buffer[128];
				const int precision = format->fractional_length_max > 48 ? 48 : format->fractional_length_max;
				const int len = snprintf(buffer, sizeof(buffer) / sizeof(char), "%0.*f", precision, (double)num);
				CL3_NONCLASS_LOGIC_ERROR(len >= 128 || len < 1);

				char* integer_part = buffer;
				char* fractional_part = buffer;
				for(; *fractional_part != '\0' && *fractional_part != '.'; fractional_part++);
				if(*fractional_part == '.')
				{
					*fractional_part = '\0';
					fractional_part++;
				}
				else
					fractional_part = NULL;

				const ssize_t len_integer_part = strlen(integer_part);
				const ssize_t len_fractional_part = fractional_part == NULL ? 0 : strlen(fractional_part);

				MakeNumberHead(str, format, num >= 0, true, num == 0);

				if(len_integer_part < format->integer_length_min && format->integer_padding != TUTF32::TERMINATOR)
					str.Pad(POSITION_TAIL, format->integer_length_min - len_integer_part, format->integer_padding);

				str.Append(integer_part, CL3_MIN(len_integer_part, format->integer_length_max));

				if(len_fractional_part > 0)
				{
					if(format->decimal_mark != TUTF32::TERMINATOR)
						str.Append(format->decimal_mark);

					str.Append(fractional_part, len_fractional_part);

					if(len_fractional_part < format->fractional_length_min)
						str.Pad(POSITION_TAIL, format->fractional_length_min - len_fractional_part, format->fractional_padding);
				}

				MakeNumberTail(str, format, num >= 0, true, num == 0);

				os.Write(str.ItemPtr(0), str.Count());
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
				PrintNumber(*this, this->number_format, CL3_ABS(v), v >= 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u8_t v)
			{
				PrintNumber(*this, this->number_format, v, true, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s16_t v)
			{
				PrintNumber(*this, this->number_format, CL3_ABS(v), v >= 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u16_t v)
			{
				PrintNumber(*this, this->number_format, v, true, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s32_t v)
			{
				PrintNumber(*this, this->number_format, CL3_ABS(v), v >= 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u32_t v)
			{
				PrintNumber(*this, this->number_format, v, true, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s64_t v)
			{
				PrintNumber(*this, this->number_format, CL3_ABS(v), v >= 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u64_t v)
			{
				PrintNumber(*this, this->number_format, v, true, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f32_t v)
			{
				PrintNumber(*this, this->number_format, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f64_t v)
			{
				PrintNumber(*this, this->number_format, v);
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
